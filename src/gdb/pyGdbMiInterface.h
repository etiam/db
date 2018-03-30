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

//struct GdbMiResult
//{
//    std::string message;
//    Payload 'payload': {u'thread-id': u'all'},
//    Stream      stream;
//    Token       token;
//    Type        type;
//
//};

class PyGdbMiInterface
{
  public:
    PyGdbMiInterface();
    ~PyGdbMiInterface();

    void executeCommand(const std::string &command);

  private:
    PyObject *  importModule(const std::string &bytecodename, const std::string &modulename);
    PyObject *  createInstance(const std::string &modulename, const std::string &classname);
    PyObject *  callFunction(PyObject *module, const std::string &functionname, PyObject *args);
    boost::any  parseMessage(PyObject *object);

    bool        m_valid = true;

    PyObject *  m_moduleDict;
    PyObject *  m_gdbmiModule;
    PyObject *  m_gdbmiParserModule;
    PyObject *  m_gdbControllerModule;
    PyObject *  m_gdbmiInstance;
};

#endif // GDB_PYGDBMIINTERFACE_H_
