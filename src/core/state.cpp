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

Variables &
State::variables()
{
    return m_variables;
}

Variables &
State::previousVariables()
{
    return m_previousVariables;
}

int
State::currentStackFrame() const
{
    return m_currentStackFrame;
}

void
State::setCurrentStackFrame(int frame)
{
    m_currentStackFrame = frame;
    Core::Signals::callStackUpdated.emit();
}

void
State::setDebuggerState(State::Debugger state)
{
    m_debuggerState = state;
    Core::Signals::debuggerStateUpdated.emit();
}

State::Debugger
State::debuggerState() const
{
    return m_debuggerState;
}

SourceFiles &
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
