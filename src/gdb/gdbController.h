/*
 * GdbController.h
 *
 *  Created on: Feb 22, 2018
 *      Author: jasonr
 */

#pragma once
#ifndef GDB_GDBCONTROLLER_H_
#define GDB_GDBCONTROLLER_H_

#ifdef HAVE_CONFIG_H
# include "config.h"
#endif

#include <string>
#include <memory>
#include <Python.h>

#include "gdbResult.h"

namespace Gdb
{

class GdbControllerImpl;

class GdbController
{
  public:
    explicit GdbController(bool verbose=false);
    ~GdbController();

    int executeCommand(const std::string &command);

  private:
    std::unique_ptr<GdbControllerImpl>   m_impl;
};

}

#endif // GDB_GDBCONTROLLER_H_
