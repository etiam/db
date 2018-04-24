/*
 * commands.h
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

    using ResponseFunc = std::function<bool(const Result, int, boost::any data)>;

    int     executeCommand(const std::string &command, Controller::ResponseFunc response = nullptr, boost::any data = nullptr);
    void    addResponse(Controller::ResponseFunc response);

  private:
    std::unique_ptr<ControllerImpl>   m_impl;
};

}

#endif // GDB_CONTROLLER_H_
