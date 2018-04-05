/*
 * pyGdbMiResult.h
 *
 *  Created on: Apr 4, 2018
 *      Author: jasonr
 */


#pragma once
#ifndef PYGDBMIRESULT_H_
#define PYGDBMIRESULT_H_

#ifdef HAVE_CONFIG_H
# include "config.h"
#endif

#include <string>
#include <vector>
#include <map>
#include <regex>
#include <boost/any.hpp>

struct String
{
    std::string     string;
};

struct Message
{
    enum class Type : char
    {
        STRING,
        NONE
    };

    Type            type = Type::NONE;
    String          string;
};

struct Payload
{
    enum class Type : char
    {
        STRING,
        DICT,
        NONE
    };

    using Dict = std::map<std::string, boost::any>;
    using List = std::vector<boost::any>;

    Type            type = Type::NONE;
    String          string;
    Dict            dict;
};

enum class Stream : char
{
    STDOUT,
    NONE
};

struct Token
{
    int             value = -1;
};

enum class Type : char
{
    RESULT,
    NOTIFY,
    OUTPUT,
    CONSOLE,
    NONE,
};

struct PyGdbMiResult
{
    Message     message;
    Payload     payload;
    Stream      stream = Stream::NONE;
    Token       token;
    Type        type = Type::NONE;
};

template<typename Iterable>
class enumerate_object
{
    private:
        Iterable _iter;
        std::size_t _size;
        decltype(std::begin(_iter)) _begin;
        const decltype(std::end(_iter)) _end;

    public:
        enumerate_object(Iterable iter):
            _iter(iter),
            _size(0),
            _begin(std::begin(iter)),
            _end(std::end(iter))
        {}

        const enumerate_object& begin() const { return *this; }
        const enumerate_object& end()   const { return *this; }

        bool operator!=(const enumerate_object&) const
        {
            return _begin != _end;
        }

        void operator++()
        {
            ++_begin;
            ++_size;
        }

        auto operator*() const
            -> std::pair<std::size_t, decltype(*_begin)>
        {
            return { _size, *_begin };
        }
};

template<typename Iterable>
auto enumerate(Iterable&& iter) -> enumerate_object<Iterable>
{
    return { std::forward<Iterable>(iter) };
}

std::ostream & operator <<(std::ostream &out, Payload::Dict dict);
std::ostream & operator <<(std::ostream &out, Payload::List dict);

inline
std::ostream &
operator <<(std::ostream &out, String string)
{
    out << "'" << std::regex_replace(string.string, std::regex("(\n)"),"\\n") << "'";
    return out;
}

inline
std::ostream &
operator <<(std::ostream &out, Message message)
{
    switch (message.type)
    {
    case Message::Type::STRING:
        out << message.string;
        break;

    case Message::Type::NONE:
        out << "None";
        break;
    }

    return out;
}

inline
std::ostream &
operator <<(std::ostream &out, Payload::List list)
{
    out << "[";
    for (auto&& a : enumerate(list))
    {
        auto index = std::get<0>(a);
        auto item = std::get<1>(a);

        try
        {
            out << "'" << boost::any_cast<char *>(item) << "'";
        }
        catch (boost::bad_any_cast &)
        {
        }

        try
        {
            out << boost::any_cast<Payload::Dict>(item);
        }
        catch (boost::bad_any_cast &)
        {
        }

        try
        {
            out << boost::any_cast<Payload::List>(item);
        }
        catch (boost::bad_any_cast &)
        {
        }

        if (index < list.size()-1)
            out << ", ";
    }
    out << "]";

    return out;
}

inline
std::ostream &
operator <<(std::ostream &out, Payload::Dict dict)
{
    out << "{";
    for (auto&& a : enumerate(dict))
    {
        auto index = std::get<0>(a);
        auto item = std::get<1>(a);

        out << "'" << item.first << "': ";

        try
        {
            out << "'" << boost::any_cast<char *>(item.second) << "'";
        }
        catch (boost::bad_any_cast &)
        {
        }

        try
        {
            out << boost::any_cast<Payload::Dict>(item.second);
        }
        catch (boost::bad_any_cast &)
        {
        }

        try
        {
            out << boost::any_cast<Payload::List>(item.second);
        }
        catch (boost::bad_any_cast &)
        {
        }

        if (index < dict.size()-1)
            out << ", ";
    }
    out << "}";
    return out;
}

inline
std::ostream &
operator <<(std::ostream &out, Payload payload)
{
    switch (payload.type)
    {
    case Payload::Type::STRING:
        out << payload.string;
        break;

    case Payload::Type::DICT:
        out << payload.dict;
        break;

    case Payload::Type::NONE:
        out << "None";
        break;
    }

    return out;
}

inline
std::ostream &
operator <<(std::ostream &out, Stream stream)
{
    switch (stream)
    {
    case Stream::STDOUT:
        out << "'stdout'";
        break;

    case Stream::NONE:
        out << "None";
        break;

    default:
        out << "unknown";
        break;
    }
    return out;
}

inline
std::ostream &
operator <<(std::ostream &out, Token token)
{
    if (token.value == -1)
    {
        out << "None";
    }
    else
    {
        out << token.value;
    }
    return out;
}

inline
std::ostream &
operator <<(std::ostream &out, Type type)
{
    switch (type)
    {
    case Type::RESULT:
        out << "'result'";
        break;

    case Type::NOTIFY:
        out << "'notify'";
        break;

    case Type::OUTPUT:
        out << "'output'";
        break;

    case Type::CONSOLE:
        out << "'console'";
        break;

    case Type::NONE:
        out << "None";
        break;

    default:
        out << "unknown";
        break;
    }
    return out;
}

inline
std::ostream &
operator<<(std::ostream &stream, const PyGdbMiResult &result)
{
    stream << "{'token': "   << result.token << ", "
           <<  "'message': " << result.message << ", "
           <<  "'type': "    << result.type << ", "
           <<  "'payload': " << result.payload << ", "
           <<  "'stream': "  << result.stream << "}";
    return stream;
}

#endif // PYGDBMIRESULT_H_
