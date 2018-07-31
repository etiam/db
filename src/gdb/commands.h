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

    // execute a command in the mi interpreter.
    int     executeCommand(const std::string &command, Controller::HandlerFunc handler = nullptr, boost::any data = nullptr);

    // execute a command in the console interpreter.
    // useful for running commands not available in the mi interface
    void    executeConsoleCommand(const std::string &command, Controller::HandlerFunc handler = nullptr, boost::any data = nullptr);

    // requests gdb load program
    void    loadProgram(const std::string &filename);

    // sets program arguments
    void    setArgs(const std::string &args);

    // requests list of source files from gdb
    void    listSourceFiles();

    // requests current call stack from gdb
    void    updateCallStack();

    // requests function address from gdb
    void    infoAddress(const std::string &function);

    void    insertBreakpoint(const std::string &location);
    void    deleteBreakpoint(int number);

    void    enableBreakpoint(int number);
    void    disableBreakpoint(int number);

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
