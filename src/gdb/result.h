/*
 * result.h
 *
 *  Created on: Apr 4, 2018
 *      Author: jasonr
 */


#pragma once
#ifndef RESULT_H_
#define RESULT_H_

#ifdef HAVE_CONFIG_H
# include "config.h"
#endif

#include <string>
#include <vector>
#include <map>
#include <boost/any.hpp>
#include <Python.h>

namespace Gdb
{

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
    LOG,
    NONE,
};

struct Result
{
    Message     message;
    Payload     payload;
    Stream      stream = Stream::NONE;
    Token       token;
    Type        type = Type::NONE;
};

Result parseResult(PyObject *object);

std::ostream & operator <<(std::ostream &out, const String &string);
std::ostream & operator <<(std::ostream &out, const Message message);
std::ostream & operator <<(std::ostream &out, const Payload::Dict dict);
std::ostream & operator <<(std::ostream &out, const Payload::List list);
std::ostream & operator <<(std::ostream &out, const Payload payload);
std::ostream & operator <<(std::ostream &out, const Stream stream);
std::ostream & operator <<(std::ostream &out, const Token token);
std::ostream & operator <<(std::ostream &out, const Type type);
std::ostream & operator <<(std::ostream &out, const Result &result);

}

#endif // RESULT_H_
