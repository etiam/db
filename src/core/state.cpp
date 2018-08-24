/*
 * state.cpp
 *
 *  Created on: Apr 22, 2018
 *      Author: root
 */

#ifdef HAVE_CONFIG_H
# include "config.h"
#endif

#include "signals.h"
#include "state.h"

namespace Core
{

AnyMap &
State::vars()
{
    return m_vars;
}

Breakpoints &
State::breakPoints()
{
    return m_breakpoints;
}

CallStack &
State::callStack()
{
    return m_callStack;
}

void
State::setDebuggerState(State::Debugger state)
{
    m_debuggerState = state;
    Core::Signals::debuggerStateUpdated();
}

State::Debugger
State::debuggerState() const
{
    return m_debuggerState;
}

void
State::setCallStack(const CallStack& callstack)
{
    m_callStack = callstack;

    Signals::callStackUpdated();
}

std::vector<std::string> &
State::sourceFiles()
{
    return m_sourceFiles;
}

Ast::Data &
State::astData()
{
    return m_data;
}

}
