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

#include "gdb/handlers.h"
#include "gdb/commands.h"
#include "gdb/result.h"

namespace Gdb
{

namespace Handlers
{

bool
infoaddressresponse(const Gdb::Result &result, int token)
{
    static std::regex addrRegex(R"regex(Symbol \\"(.*)\(.*\)\\" is a function at address (0x[0-9a-f]+)\.\\n)regex");

    std::smatch match;
    auto ret = result.message.type == Gdb::Message::Type::NONE &&
               result.payload.type == Gdb::Payload::Type::STRING &&
               result.stream == Gdb::Stream::STDOUT &&
               std::regex_match(result.payload.string.string, match, addrRegex);

    if (ret)
    {
        std::string cmd = "interpreter-exec console \"info line *" + match[2].str() + "\"";
        Core::gdb()->executeCommand(cmd);
    }

    return ret;
};

}

}
