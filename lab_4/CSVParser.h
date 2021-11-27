#pragma once
#ifndef CSVPARSER_H
#define CSVPARSER_H

#include <fstream>
#include "Tuple.h"

using namespace std;

template<class ... Args> // если лежит не здесь то что-то ломается)
class CSVParser         // information about file
{
private:
    ifstream& input;
    size_t offset;
    int fileLength = -1;
    char columnDel = ',';


    void reset()
    {
        input.clear();
        input.seekg(0, ios::beg);
    }

    int getLength()  // get file string size
    {
        if (fileLength == -1)
        {
            
            reset();

            string line;
            for (fileLength = 0; getline(input, line); fileLength++);

           
            reset();
        }
        return fileLength;
    }

    class CSVIterator  // file string // начледование не прокатит
    /*probably it would be possible to throw everything off here, but it seems to me that it would look illogical*/
    {
    private:
        string strBuffer;
        ifstream& input;
        size_t index;
        CSVParser<Args...>& parent; // previous comment
        bool last = false;  //there may be the same line in the middle of the text and at the end

        friend class CSVParser;

    public:
        CSVIterator(ifstream& ifs, size_t index, CSVParser<Args...>& parent) : index(index), parent(parent), input(ifs)
        {
            for (int i = 0; i < index - 1; i++, getline(input, strBuffer));

            getline(input, strBuffer);
        }

        CSVIterator operator++()
        {
            if (index < parent.fileLength)
            {
                index++;
                input.clear();
                input.seekg(0, ios::beg);
                
                for (int i = 0; i < index - 1; ++i, getline(input, strBuffer)); // ignore first elements

                getline(input, strBuffer);
            }
            else
            {
                strBuffer = "";
                last = true;
            }

            return *this;
        }

        bool operator==(const CSVIterator& other) const
        {
            return this->last == other.last && this->index == other.index && this->strBuffer == other.strBuffer;
        }

        bool operator!=(const CSVIterator& other)
        {
            return !(*this == other);
        }

        tuple<Args...> operator*()
        {
            return parent.parseLine(strBuffer, index);
        }
    };

public:
    explicit CSVParser(ifstream& ifs, size_t offset) : input(ifs), offset(offset)
    {
        if (!ifs.is_open()) {
            cerr << "Error: Can't open file";
            throw std::invalid_argument("");
        }
        if (offset >= getLength()) {
            cerr << "Error: offset >= file";
            throw logic_error("");
        }
        if (offset < 0) {
            cerr << "Error: offset < 0";
            throw logic_error("");
        }
    }


    CSVIterator begin()
    {
        CSVIterator a(input, offset + 1, *this);
        return a;
    }

    CSVIterator end()
    {
        int t = getLength();

        CSVIterator a(input, t, *this);
        a.last = true;
        return a;
    }

    vector<string> read_string(string& line)
    {
        vector<string> fields{ "" };
        size_t fcounter = 0;
        for (char c : line)
        {
                if (c == columnDel)
                {
                    fields.emplace_back("");
                    fcounter++;
                }
                else
                {
                    fields[fcounter].push_back(c);
                }
        }
        return fields;
    }

    tuple<Args...> parseLine(string& line, int number) // the "Args..." stores the data types required by tuple
    {
        size_t size = sizeof...(Args);
        if (line.empty())
            throw invalid_argument("Line " + to_string(number) + " is empty!");

        tuple<Args...> table_str;
        vector<string> fields = read_string(line);   //split into columns

        if (fields.size() != size)
            throw invalid_argument("Wrong number of fields in line " + to_string(number) + "!");

        
        auto a = fields.begin();
        try
        {
            parser_util::parse(table_str, a);   // data type conversion
        }
        catch (exception& e)
        {
            throw invalid_argument("Line " + to_string(number) + " contains bad types!");
        }

        return table_str;
    }
};

#endif  //CSVPARSER_H