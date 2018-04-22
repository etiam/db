/*
 * state.cpp
 *
 *  Created on: Apr 22, 2018
 *      Author: root
 */

#ifdef HAVE_CONFIG_H
# include "config.h"
#endif

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

}

