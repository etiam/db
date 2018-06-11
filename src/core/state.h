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
#include "breakpoints.h"

namespace Core
{

struct Location
{
    std::string     filename;
    int             row;

    bool operator ==(const Location &other) const
    {
        return other.filename == filename && other.row == row;
    }
};

struct CallStackEntry
{
    CallStackEntry(Location loc, std::string file, std::string func, int le) :
        location(std::move(loc)),
        filename(std::move(file)),
        function(std::move(func)),
        level(le) {}
    Location    location;
    std::string filename;
    std::string function;
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
    Breakpoints &       breakpoints();

    const Location &    currentLocation() const;

    void                setDebuggerState(Debugger state);
    Debugger            debuggerState() const;

    void                setCallStack(const CallStack &callstack);
    const CallStack &   callStack() const;

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
