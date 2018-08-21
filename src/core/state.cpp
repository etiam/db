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

State::State()
{
    Core::Signals::loadEditorSource.connect(this, &State::onLoadFileSignal);
    Core::Signals::setCurrentLocation.connect(this, &State::onSetCurrentLocationSignal);
}

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

const Location &
State::currentLocation() const
{
    return m_currentLocation;
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

// signal handlers

void
State::onLoadFileSignal(const std::string &filename)
{
    m_currentLocation.filename = filename;
}

void
State::onSetCurrentLocationSignal(const Core::Location &location)
{
    auto oldloc = m_currentLocation.row;

    m_currentLocation = location;

    // update previous position's gutter marker
    Signals::updateGutterMarker(oldloc);

    Signals::setCursorPosition(m_currentLocation.row, 0);
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
