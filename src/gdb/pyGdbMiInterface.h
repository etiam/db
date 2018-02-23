/*
 * PyGdbInterface.h
 *
 *  Created on: Feb 22, 2018
 *      Author: jasonr
 */

#pragma once
#ifndef PYGDBMIINTERFACE_H_
#define PYGDBMIINTERFACE_H_

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

  private:
    void importModule(const std::string &modulename);

    PyObject* createInstance(const std::string &modulename,
                             const std::string &classname);

    void callClassFunction(const std::string &modulename,
                           const std::string &classname,
                           const std::string &functionname);


    PyObject * m_modulesDict;
    PyObject * m_gdbModule;
};

#endif // PYGDBMIINTERFACE_H_
