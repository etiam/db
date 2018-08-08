/*
 * running.cpp
 *
 *  Created on: May 15, 2018
 *      Author: jasonr
 */


#ifdef HAVE_CONFIG_H
# include "config.h"
#endif

#include <iostream>
#include <regex>
#include <boost/filesystem/operations.hpp>

#include "core/global.h"
#include "core/state.h"
#include "core/signals.h"

#include "gdb/commands.h"
#include "gdb/result.h"

namespace Gdb
{

namespace Handlers
{

bool
execrun(const Result &result, int token, boost::any data)
{
    auto match = result.token.value == token;

    if (match)
    {
        Core::state()->setDebuggerState(Core::State::Debugger::RUNNING);
    }

    return match;
}

}

}
