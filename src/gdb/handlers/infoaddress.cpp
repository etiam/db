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

bool
infoaddress(const Gdb::Result &result, int token, boost::any data)
{
    static std::regex addrRegex(R"regex(Symbol \\"(.*)\(.*\)\\" is a function at address (0x[0-9a-f]+)\.\\n)regex");

    std::smatch smatch;
    auto match = result.token.value == -1 &&
                 std::regex_match(result.payload.string.string, smatch, addrRegex);

    if (match)
    {
        std::string cmd = "interpreter-exec console \"info line *" + smatch[2].str() + "\"";
        Core::gdb()->executeCommand(cmd, Handlers::infoline);
    }

    return match;
};

}

}
