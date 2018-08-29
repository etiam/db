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
#include "core/signals.h"

#include "gdb/global.h"
#include "gdb/commands.h"
#include "gdb/result.h"
#include "gdb/controller.h"

#include "handlers.h"

namespace Gdb
{

namespace Handlers
{

// match the response of "info address ..." command
Controller::HandlerReturn
infoaddress(const Gdb::Result &result, int token, boost::any data)
{
    static std::regex regex(R"regex(Symbol \\"(.*)\(.*\)\\" is a function at address (0x[0-9a-f]+)\.\\n)regex");
    bool match = false;

    std::smatch smatch;
    if (std::regex_match(result.payload.string.data, smatch, regex))
    {
        match = true;
        Gdb::commands()->executeConsoleCommand("info line *" + smatch[2].str());
    }

    return {"infoaddress", match, Controller::MatchType::REGEX};
}

}

}
