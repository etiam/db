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
#include <functional>
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

    using FilterFunc = std::function<bool(const GdbResult, int)>;
    using ResponseFunc = std::function<void(const GdbResult, int)>;

    int executeCommand(const std::string &command, FilterFunc filter = nullptr, ResponseFunc response = nullptr);

  private:
    std::unique_ptr<GdbControllerImpl>   m_impl;
};

}

#endif // GDB_GDBCONTROLLER_H_
