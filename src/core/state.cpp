/*
 * state.cpp
 *
 *  Created on: Apr 22, 2018
 *      Author: root
 */

#ifdef HAVE_CONFIG_H
# include "config.h"
#endif

#include "signal.h"
#include "state.h"

namespace Core
{

State::State()
{
    Core::Signal::loadFile.connect(this, &State::onLoadFileSignal);
    Core::Signal::setCurrentLocation.connect(this, &State::onSetCurrentLocationSignal);
}

AnyMap &
State::vars()
{
    return m_vars;
}

Breakpoints &
State::breakpoints()
{
    return m_breakpoints;
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
    Core::Signal::debuggerStateUpdated();
}

State::Debugger
State::debuggerState() const
{
    return m_debuggerState;
}

// wink signal handlers

void
State::onLoadFileSignal(const std::string &filename)
{
    m_currentLocation.m_filename = filename;
}

void
State::onSetCurrentLocationSignal(const Core::Location &location)
{
    // update previous position's gutter marker
    Signal::updateGutterMarker(m_currentLocation.m_row);

    Signal::setCursorPosition(m_currentLocation.m_row, 0);

    m_currentLocation = location;
}


void
State::setCallStack(const CallStack& callstack)
{
    m_callStack = callstack;

    Signal::callStackUpdated();
}

const CallStack &
State::callStack() const
{
    return m_callStack;
}

}
