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
#include <Python.h>

class PyGdbMiInterface
{
  public:
    PyGdbMiInterface();
    ~PyGdbMiInterface();

    void executeCommand(const std::string &command);

  private:
    PyObject * importModule(const std::string &bytecodename, const std::string &modulename);

    PyObject * createInstance(const std::string &modulename,
                              const std::string &classname);

    PyObject * callClassFunction(const std::string &modulename,
                                 const std::string &classname,
                                 const std::string &functionname);

    bool m_valid = true;

    PyObject * m_moduleDict;

    PyObject * m_gdbmiModule;
    PyObject * m_gdbmiParserModule;
    PyObject * m_gdbControllerModule;
    PyObject * m_gdbmiInstance;
};

#endif // GDB_PYGDBMIINTERFACE_H_
