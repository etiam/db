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
    Controller();
    ~Controller();

    enum class MatchType : char
    {
        TOKEN,
        REGEX,
        METADATA,
        NONE
    };

    struct HandlerReturn
    {
        std::string name;
        bool matched;
        MatchType type;
    };

    using HandlerFunc = std::function<HandlerReturn(const Result, int, boost::any data)>;

    int executeCommand(const std::string &command, HandlerFunc handler = nullptr, boost::any data = nullptr);
    void addHandler(Controller::HandlerFunc handler, int priority, bool persistent, boost::any data = nullptr);

private:
    std::unique_ptr<ControllerImpl> m_impl;
};

} // namespace Gdb

std::ostream & operator <<(std::ostream &out, const Gdb::Controller::MatchType &type);

#endif // GDB_CONTROLLER_H_
