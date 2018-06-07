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
#include "location.h"

namespace Core
{

struct CallStackEntry
{
    CallStackEntry(std::string file, std::string full, std::string func, int le, int li) :
        filename(std::move(file)),
        fullname(std::move(full)),
        function(std::move(func)),
        level(le),
        line(li) {};
    std::string filename;
    std::string fullname;
    std::string function;
    int level;
    int line;
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
