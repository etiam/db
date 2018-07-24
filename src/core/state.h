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

    State();
    ~State() = default;

    AnyMap &            vars();
    Breakpoints &       breakPoints();
    CallStack &         callStack();

    const Location &    currentLocation() const;

    void                setDebuggerState(Debugger state);
    Debugger            debuggerState() const;

    void                setCallStack(const CallStack &callstack);

  private:
    // wink signal handlers
    void                onLoadFileSignal(const std::string &filename);
    void                onSetCurrentLocationSignal(const Core::Location &location);

    CallStack           m_callStack;
    AnyMap              m_vars;
    Breakpoints         m_breakpoints;
    Location            m_currentLocation;
    Debugger            m_debuggerState = Debugger::NONE;
};

}

#endif // CORE_STATE_H_
