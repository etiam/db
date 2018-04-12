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
    explicit GdbController();
    ~GdbController();

    using ResponseFunc = std::function<bool(const GdbResult, int)>;

    int     executeCommand(const std::string &command, GdbController::ResponseFunc response = nullptr, bool persistent = false);

    void    jumpToProgramStart();

  private:
    std::unique_ptr<GdbControllerImpl>   m_impl;
};

}

#endif // GDB_GDBCONTROLLER_H_
