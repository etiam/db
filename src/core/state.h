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

class State
{
  public:
    State() = default;
    ~State() = default;

    AnyMap &        vars();
    Breakpoints &   breakpoints();

  private:
    AnyMap          m_vars;
    Breakpoints     m_breakpoints;
};

}

#endif // CORE_STATE_H_
