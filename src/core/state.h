/*
 * state.h
 *
 *  Created on: Apr 12, 2018
 *      Author: jasonr
 */


#pragma once
#ifndef CORE_STATE_H_
#define CORE_STATE_H_

#ifdef HAVE_CONFIG_H
# include "config.h"
#endif

#include "ast/data.h"

#include "anymap.h"
#include "types.h"
#include "breakpoints.h"

namespace Core
{

struct CallStackEntry
{
    CallStackEntry(Location loc, int le) :
        location(std::move(loc)),
        level(le) {}
    Location    location;
    int level;
};

using CallStack = std::vector<CallStackEntry>;

class State
{
  public:
    enum class Debugger : char
    {
        LOADED,
        RUNNING,
        PAUSED,
        NONE
    };

    State() = default;
    ~State() = default;

    AnyMap & vars();

    Breakpoints & breakPoints();
    CallStack & callStack();

    const Location & currentLocation() const;

    void setDebuggerState(Debugger state);
    Debugger debuggerState() const;

    void setCallStack(const CallStack &callstack);

    std::vector<std::string> & sourceFiles();

    // get reference to ast objects
    Ast::Data & astData();

  private:
    CallStack m_callStack;
    AnyMap m_vars;
    Breakpoints m_breakpoints;
    Debugger m_debuggerState = Debugger::NONE;

    std::vector<std::string> m_sourceFiles;
    Ast::Data m_data;
};

}

#endif // CORE_STATE_H_
