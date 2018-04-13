/*
 * controller.h
 *
 *  Created on: Feb 22, 2018
 *      Author: jasonr
 */

#pragma once
#ifndef GDB_CONTROLLER_H_
#define GDB_CONTROLLER_H_

#ifdef HAVE_CONFIG_H
# include "config.h"
#endif

#include <string>
#include <memory>
#include <functional>
#include <Python.h>

#include "result.h"

namespace Gdb
{

class ControllerImpl;

class Controller
{
  public:
    explicit Controller();
    ~Controller();

    using ResponseFunc = std::function<bool(const Result, int)>;

    int     executeCommand(const std::string &command, Controller::ResponseFunc response = nullptr, bool persistent = false);

    void    loadFile(const std::string &filename);
    void    jumpToMain();

  private:
    std::unique_ptr<ControllerImpl>   m_impl;
};

}

#endif // GDB_CONTROLLER_H_
