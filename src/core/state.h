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

class State
{
  public:
    enum class Debugger : char
    {
        LAUNCHED,
        RUNNING,
        PAUSED,
        NONE
    };

    State();
    ~State() = default;

    AnyMap &            vars();
    Breakpoints &       breakpoints();

    const Location &    currentLocation() const;

    Debugger            debuggerState() const;

  private:

    // wink signal handlers
    void                onLoadFileSignal(const std::string &filename);
    void                onSetCurrentLocationSignal(const Core::Location &location);

    AnyMap              m_vars;
    Breakpoints         m_breakpoints;
    Location            m_currentLocation;
    Debugger            m_debuggerState = Debugger::NONE;
};

}

#endif // CORE_STATE_H_
