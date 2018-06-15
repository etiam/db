/*
 * gdbUtiil.cpp
 *
 *  Created on: Apr 10, 2018
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
#include "core/signal.h"

#include "gdb/commands.h"
#include "gdb/result.h"

namespace Gdb
{

namespace Handlers
{

bool
fileexec(const Gdb::Result &result, int token, boost::any data)
{
    auto match = result.token.value == token;

    if (match)
    {
        Core::state()->setDebuggerState(Core::State::Debugger::LOADED);
        Core::Signal::appendConsoleText(result.message.string.data + '\n');
    }

    return match;
}

}

}
