/*
 * commands.h
 *
 *  Created on: Apr 21, 2018
 *      Author: jasonr
 */

#pragma once
#ifndef SRC_GDB_COMMANDS_H_
#define SRC_GDB_COMMANDS_H_

#ifdef HAVE_CONFIG_H
# include "config.h"
#endif

#include <memory>

#include "controller.h"

namespace Gdb
{

class Commands
{
  public:
    Commands();
    ~Commands();

    int     executeCommand(const std::string &command, Controller::HandlerFunc handler = nullptr, boost::any data = nullptr);

    void    loadProgram(const std::string &filename);

    void    infoAddress(const std::string &function);

    void    insertBreakpoint(const std::string &location);
    void    disableBreakpoint(int number);
    void    deleteBreakpoint(int number);

    void    run();
    void    cont();
    void    pause();
    void    stop();
    void    stepover();
    void    stepinto();
    void    stepout();

  private:
    std::unique_ptr<Controller> m_controller;
};

} // namespace Gdb

#endif // SRC_GDB_COMMANDS_H_
