/*
 * PyGdbInterface.h
 *
 *  Created on: Feb 22, 2018
 *      Author: jasonr
 */

#pragma once
#ifndef GDB_PYGDBMIINTERFACE_H_
#define GDB_PYGDBMIINTERFACE_H_

#ifdef HAVE_CONFIG_H
# include "config.h"
#endif

#include <string>
#include <boost/any.hpp>
#include <Python.h>

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

enum class Token : char
{
    NONE,
};

enum class Type : char
{
    RESULT,
    NOTIFY,
    OUTPUT,
    CONSOLE,
    NONE,
};

struct GdbMiResult
{
    Message     message;
    Payload     payload;
    Stream      stream = Stream::NONE;
    Token       token = Token::NONE;
    Type        type = Type::NONE;
};

class PyGdbMiInterface
{
  public:
    PyGdbMiInterface(const std::string &filename);
    ~PyGdbMiInterface();

    void            executeCommand(const std::string &command);

  private:
    PyObject *      importModule(const std::string &bytecodename, const std::string &modulename);
    PyObject *      createInstance(const std::string &modulename, const std::string &classname);
    PyObject *      callFunction(PyObject *module, const std::string &functionname, PyObject *args);

    GdbMiResult     parseResult(PyObject *object);
    Message         parseMessage(PyObject *object);
    Payload         parsePayload(PyObject *object);
    Stream          parseStream(PyObject *object);
    Token           parseToken(PyObject *object);
    Type            parseType(PyObject *object);
    boost::any      parseObject(PyObject *object);

    bool            m_valid = true;

    PyObject *      m_moduleDict;

    PyObject *      m_gdbMiModule;
    PyObject *      m_gdbMiParserModule;
    PyObject *      m_gdbControllerModule;
    PyObject *      m_gdbMiInstance;
};

#endif // GDB_PYGDBMIINTERFACE_H_
