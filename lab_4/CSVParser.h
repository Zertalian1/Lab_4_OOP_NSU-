#ifndef CSVPARSER_H
#define CSVPARSER_H

#include <fstream>
#include <typeinfo>
#include <regex>
#include "Tuple.h"

using namespace std;

template<class ... Args>
class CSVParser         // information about file
{
private:
    ifstream& input;
    size_t offset;
    int fileLength = -1;
    char columnDelimiter = ',';
    char lineDelimiter = '\n';


    void getLine(ifstream& is, string& str)
    {
        str.clear();

        char c;
        while (is.get(c))
        {
            if (c == lineDelimiter)
                break;
            str.push_back(c);
        }
    }

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
    {
    private:
        string strBuffer;
        ifstream& input;
        size_t index;
        CSVParser<Args...>& parent;
        bool last = false;

        friend class CSVParser;

    public:
        CSVIterator(ifstream& ifs, size_t index, CSVParser<Args...>& parent) : index(index), parent(parent), input(ifs)
        {
            for (int i = 0; i < index - 1; i++, parent.getLine(input, strBuffer));

            parent.getLine(input, strBuffer);
            //if (!input )   // позволило упростить end, но стоит ли
            //    throw logic_error("Bad file!");
        }

        CSVIterator operator++()
        {
            if (index < parent.fileLength)
            {
                index++;
                input.clear();
                input.seekg(0, ios::beg);
                
                for (int i = 0; i < index - 1; ++i, parent.getLine(input, strBuffer)); // ignore first elements

                parent.getLine(input, strBuffer);
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
        if (!ifs.is_open())
            throw std::invalid_argument("Can't open file");
        if (offset >= getLength())
            throw logic_error("Bad file offset! offset >= file");
        if (offset < 0)
            throw logic_error("Bad file offset! offset < 0");
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

    vector<string> read_string(string& line, int lineNum)
    {
        vector<string> fields{ "" };
        size_t fcounter = 0;
        size_t linePos = 0;
        bool accessWriteDelim = false;
        for (char c : line)
        {
                if (c == columnDelimiter)
                {
                    fields.emplace_back("");
                    fcounter++;
                }
                else
                {
                    fields[fcounter].push_back(c);
                }
            linePos++;
        }
        return fields;
    }

    tuple<Args...> parseLine(string& line, int number) // the "Args..." stores the data types required by tuple
    {
        size_t size = sizeof...(Args);
        if (line.empty())
            throw invalid_argument("Line " + to_string(number) + " is empty!");

        tuple<Args...> table_str;
        vector<string> fields = read_string(line, number);

        if (fields.size() != size)
            throw invalid_argument("Wrong number of fields in line " + to_string(number) + "!");

        // у нас почти есть tuple, теперь нужно преобразовать в нужный тип дпнных
        auto a = fields.begin();
        try
        {
            parser_utils::parse(table_str, a);
        }
        catch (exception& e)
        {
            throw invalid_argument("Line " + to_string(number) + " contains bad types!");
        }

        return table_str;
    }
};

#endif  //CSVPARSER_H