#pragma once
#include <tuple>
#include <functional>
#include <iostream>
#include <sstream>

template<const int index, typename Callback, typename... Args>
struct iterate
{
    static void next(std::tuple<Args...>& t, Callback callback, std::ostream& stream)
    {
        iterate<index - 1, Callback, Args...>::next(t, callback, stream);
        callback(std::get<index>(t), stream);
    }
};

template<typename Callback, typename... Args>
struct iterate<-1, Callback, Args...>
{
    static void next(std::tuple<Args...>& t, Callback callback, std::ostream& stream) {}
};

template<typename Callback, typename... Args>
void forEach(std::tuple<Args...>& t, Callback callback, std::ostream& stream)
{
    int const t_size = std::tuple_size<std::tuple<Args...>>::value;
    iterate<t_size - 1, Callback, Args...>::next(t, callback, stream);
}

struct callback
{
    template<typename T>
    void operator()(T&& t, std::ostream& stream)
    {
        stream << t << " ";
    }
};

template<typename _CharT, typename _Traits, typename... Args>
std::basic_ostream<_CharT, _Traits>&  operator<<(std::basic_ostream<_CharT, _Traits>& stream, std::tuple<Args...>& t)
{
    forEach(t, callback(), stream);
 
    return stream;
}

namespace parser_util
{
/// /////////////////////////////////////////////////////////////////////////////////////////////////////////////
                                               // *PARSE* //
template <class _Type>
    class lexical_cast
    {
        _Type value;
    public:
        lexical_cast(const std::string& src)
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
            t = lexical_cast<T>(*it);
            it++;
        }
    };

    template <typename ...Args>
    void parse(std::tuple<Args...>& tuple, std::vector<std::string>::iterator& it)
    {
        forEach(tuple, callback(), it);
    }
}