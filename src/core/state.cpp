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
    Core::Signal::debuggerStateSet(state);
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
    // update previous current location
    Signal::updateGutterMarker(m_currentLocation.m_row);

    m_currentLocation = location;
}


}
