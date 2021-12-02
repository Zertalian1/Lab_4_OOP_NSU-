#pragma once
#ifndef TUPLE_H
#define TUPLE_H

#include <tuple>
#include <functional>
#include <iostream>
#include <sstream>

template<const int index, typename... Args>
struct iterate
{
    static void next(std::tuple<Args...>& t, std::ostream& stream)
    {
        iterate<index - 1,  Args...>::next(t, stream);
        stream << std::get<index>(t) << " ";
    }
};

template<typename... Args>
struct iterate<-1, Args...>
{
    static void next(std::tuple<Args...>& t, std::ostream& stream) {}
};

template<typename... Args>
void forEach(std::tuple<Args...>& t, std::ostream& stream)
{
    int const t_size = std::tuple_size<std::tuple<Args...>>::value;
    iterate<t_size - 1, Args...>::next(t, stream);
}


template<typename _CharT, typename _Traits, typename... Args>
std::basic_ostream<_CharT, _Traits>&  operator<<(std::basic_ostream<_CharT, _Traits>& stream, std::tuple<Args...>& t)
{
    forEach(t, stream);
 
    return stream;
}

namespace parser_util
{
/// /////////////////////////////////////////////////////////////////////////////////////////////////////////////
                                               // *PARSE* //
template <class _Type>
    class cast
    {
        _Type value;
    public:
        cast(const std::string& src)
        {
            std::stringstream s;
            s.str(src);
            s >> value;
            if (!s)
                throw std::exception();
        }

        operator _Type& () {
            return value;
        }
    };



/// /////////////////////////////////////////////////////////////////////////////////////////////////////////
                                               // *ITERATOR* //

    

    template<int index, typename Callback, typename... Args>
    struct iterate
    {
        static void next(std::tuple<Args...>& t, Callback callback, std::vector<std::string>::iterator& it)
        {
            iterate<index - 1, Callback, Args...>::next(t, callback, it);
            callback(std::get<index>(t), it);
        }
    };


    template<typename Callback, typename... Args>
    struct iterate<-1, Callback, Args...>
    {
        static void next(std::tuple<Args...>& t, Callback callback, std::vector<std::string>::iterator& it) {}
    };


    template<typename Callback, typename... Args>
    void forEach(std::tuple<Args...>& t, Callback callback, std::vector<std::string>::iterator& it)
    {
        int const t_size = std::tuple_size<std::tuple<Args...>>::value;

        iterate<t_size - 1, Callback, Args...>::next(t, callback, it);
    }

    struct callback
    {
        template<typename T>
        void operator()(T& t, std::vector<std::string>::iterator& it)
        {
            t = cast<T>(*it);
            it++;
        }
    };

    template <typename ...Args>
    void parse(std::tuple<Args...>& tuple, std::vector<std::string>::iterator& it)
    {
        forEach(tuple, callback(), it);
    }
}
#endif  //TUPLE_H