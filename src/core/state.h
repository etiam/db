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
    Variables & variables();
    Variables & previousVariables();

    int currentStackFrame() const;
    void setCurrentStackFrame(int level);

    void setDebuggerState(Debugger state);
    Debugger debuggerState() const;

    SourceFiles & sourceFiles();

    // get reference to ast objects
    Ast::Data & astData();

  private:
    CallStack m_callStack;
    Variables m_variables;
    Variables m_previousVariables;
    AnyMap m_vars;
    Breakpoints m_breakpoints;
    Debugger m_debuggerState = Debugger::NONE;
    int m_currentStackFrame = 0;

    SourceFiles  m_sourceFiles;
    Ast::Data m_data;
};

}

#endif // CORE_STATE_H_
