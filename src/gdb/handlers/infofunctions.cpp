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

#include "handlers.h"

namespace Gdb
{

namespace Handlers
{

// match the response of "info address ..." command
bool
infofunctions(const Gdb::Result &result, int token, boost::any data)
{
    static std::regex regex(R"regex(Symbol \\"(.*)\(.*\)\\" is a function at address (0x[0-9a-f]+)\.\\n)regex");
    bool match = false;

    std::smatch smatch;
    if (std::regex_match(result.payload.string.data, smatch, regex))
    {
        match = true;
        Core::gdb()->executeConsoleCommand("info line *" + smatch[2].str());
    }

    return match;
}

}

}
