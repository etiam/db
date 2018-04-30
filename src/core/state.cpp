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
State::setCurrentLocation(const Location& location)
{
    m_currentLocation = location;
    Signal::setCurrentLocation(location);
}

}
