/*
 * result.h
 *
 *  Created on: Apr 5, 2018
 *      Author: jasonr
 */

#ifdef HAVE_CONFIG_H
# include "config.h"
#endif

#include <iostream>
#include <regex>

#include "core/utils.h"

#include "result.h"

namespace Gdb
{

Message
parseMessage(PyObject *object)
{
    Message message;

    if (object == Py_None)
    {
        message.type = Message::Type::NONE;
    }
    else if (PyString_Check(object) || PyUnicode_Check(object))
    {
        message.type = Message::Type::STRING;
        message.string.data = PyString_AsString(object);
    }

    return message;
}

boost::any
parseObject(PyObject *object)
{
    if (PyInt_Check(object))
        return _PyInt_AsInt(object);

    if (PyFloat_Check(object))
        return PyFloat_AsDouble(object);

    if (PyString_Check(object) || PyUnicode_Check(object))
        return PyString_AsString(object);

    if (PyList_Check(object))
    {
        auto n = PyList_Size(object);
        Payload::List list(n);

        for (auto i=0; i < n; i++)
        {
            auto item = PyList_GetItem(object, i);
            Py_INCREF(item);
            list[i] = parseObject(item);
            Py_DECREF(item);
        }

        return list;
    }

    if (PyDict_Check(object))
    {
        auto list = PyDict_Keys(object);
        auto n = PyList_Size(list);
        std::map<std::string, boost::any> map;

        for (auto i=0; i < n; i++)
        {
            auto key = PyList_GetItem(list, i);
            auto val = PyDict_GetItem(object, key);
            Py_INCREF(key);
            Py_INCREF(val);
            map[PyString_AsString(key)] = parseObject(val);
            Py_DECREF(key);
            Py_DECREF(val);
        }

        Py_DECREF(list);

        return map;
    }

    if (object == Py_None)
        return nullptr;

    std::stringstream errmsg;
    auto objstr = PyObject_Str(object);
    errmsg << "parseObject(): un-handled type " << PyString_AsString(objstr);
    Py_DECREF(objstr);
    throw std::runtime_error(errmsg.str().c_str());
}

Payload
parsePayload(PyObject* object)
{
    Payload payload;

    if (object == Py_None)
    {
        payload.type = Payload::Type::NONE;
    }
    else if (PyString_Check(object) || PyUnicode_Check(object))
    {
        payload.type = Payload::Type::STRING;
        payload.string.data = PyString_AsString(object);
    }
    else if (PyDict_Check(object))
    {
        payload.type = Payload::Type::DICT;
        payload.dict = boost::any_cast<Payload::Dict>(parseObject(object));
    }

    return payload;
}

Stream
parseStream(PyObject* object)
{
    Stream stream = Stream::NONE;

    if (PyString_Check(object) || PyUnicode_Check(object))
    {
        auto string = std::string(PyString_AsString(object));
        if (string == "stdout")
            stream = Stream::STDOUT;
        else
        {
            auto objstr = PyObject_Str(object);
            std::cerr << "parseStream(): unknown value : " << PyString_AsString(objstr) << std::endl;
            Py_DECREF(objstr);
        }
    }

    return stream;
}

Token
parseToken(PyObject* object)
{
    Token token;

    if (object == Py_None)
    {
        token.value = -1;
    }
    else if (PyInt_Check(object))
    {
        token.value = static_cast<int>(_PyInt_AsInt(object));
    }
    else
    {
        auto objstr = PyObject_Str(object);
        std::cerr << "parseToken(): unknown value : " << PyString_AsString(objstr) << std::endl;
        Py_DECREF(objstr);
    }


    return token;
}

Type
parseType(PyObject* object)
{
    Type type = Type::NONE;

    if (PyString_Check(object) || PyUnicode_Check(object))
    {
        auto string = std::string(PyString_AsString(object));
        if (string == "result")
            type = Type::RESULT;
        else if (string == "notify")
            type = Type::NOTIFY;
        else if (string == "output")
            type = Type::OUTPUT;
        else if (string == "console")
            type = Type::CONSOLE;
        else if (string == "log")
            type = Type::LOG;
        else
        {
            auto objstr = PyObject_Str(object);
            std::cerr << "parseType(): unknown value : " << PyString_AsString(objstr) << std::endl;
            Py_DECREF(objstr);
        }
    }

    return type;
}

Result
parseResult(PyObject *object)
{
    // must be a dictionary type
    if (!PyDict_Check(object))
    {
        std::stringstream errmsg;
        auto objstr = PyObject_Str(object);
        errmsg << "parseResult(): invalid data " << PyString_AsString(objstr);
        Py_DECREF(objstr);
        throw std::runtime_error(errmsg.str().c_str());
    }

    Result result;

    auto list = PyDict_Keys(object);
    auto n = PyList_Size(list);
    std::map<std::string, boost::any> map;

    for (auto i=0; i < n; i++)
    {
        auto key = std::string(PyString_AsString(PyList_GetItem(list, i)));
        auto val = PyDict_GetItem(object, PyList_GetItem(list, i));
        Py_INCREF(val);

        if (key == "message")
        {
            result.message = parseMessage(val);
        }
        else if (key == "payload")
        {
            result.payload = parsePayload(val);
        }
        else if (key == "stream")
        {
            result.stream = parseStream(val);
        }
        else if (key == "token")
        {
            result.token = parseToken(val);
        }
        else if (key == "type")
        {
            result.type = parseType(val);
        }
        else
        {
            std::cerr << "ParseMessage(): unknown key : " << key << std::endl;
        }
        Py_DECREF(val);
    }
    Py_DECREF(list);

    return result;
}

std::ostream &
operator <<(std::ostream &out, const String &string)
{
    out << "'" << std::regex_replace(string.data, std::regex("(\n)"),"\\n") << "'";
    return out;
}

std::ostream &
operator <<(std::ostream &out, const Message message)
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

std::ostream &
operator <<(std::ostream &out, const Payload::List list)
{
    out << "[";
    for (auto&& a : Core::enumerate(list))
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

std::ostream &
operator <<(std::ostream &out, const Payload::Dict dict)
{
    out << "{";
    for (auto&& a : Core::enumerate(dict))
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

std::ostream &
operator <<(std::ostream &out, const Payload payload)
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

std::ostream &
operator <<(std::ostream &out, const Stream stream)
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

std::ostream &
operator <<(std::ostream &out, const Token token)
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

std::ostream &
operator <<(std::ostream &out, const Type type)
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

    case Type::LOG:
        out << "'log'";
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

std::ostream &
operator<<(std::ostream &stream, const Result &result)
{
    stream << "{'token': "   << result.token << ", "
           <<  "'message': " << result.message << ", "
           <<  "'type': "    << result.type << ", "
           <<  "'payload': " << result.payload << ", "
           <<  "'stream': "  << result.stream << "}";
    return stream;
}


}
