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
#include <memory>
#include <Python.h>

#include "pyGdbMiResult.h"

class PyGdbMiInterfaceImpl;

class PyGdbMiInterface
{
  public:
    PyGdbMiInterface();
    ~PyGdbMiInterface();

    PyGdbMiResult executeCommand(const std::string &command);

  private:
    std::unique_ptr<PyGdbMiInterfaceImpl>   m_impl;
};

#endif // GDB_PYGDBMIINTERFACE_H_
