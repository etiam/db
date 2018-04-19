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

#include "gdb/controller.h"
#include "gdb/result.h"

namespace
{

bool
infolineresponse(const Gdb::Result &result, int token)
{
    static std::regex lineRegex(R"regex(Line (\d+) of \\"(.*)\\" starts at address 0x[0-9a-f]+ <(.*)\(.*\)>.*)regex");

    std::smatch match;
    auto ret = result.token.value == -1 &&
               result.message.type == Gdb::Message::Type::NONE &&
               result.payload.type == Gdb::Payload::Type::STRING &&
               std::regex_match(result.payload.string.string, match, lineRegex);

    if (ret)
    {
        auto &state = Core::state();
        auto filename = match[2].str();

        // if filename is relative, convert to absolute based on buildpath
        if (!boost::filesystem::path(match[2].str()).is_absolute())
        {
            auto buildpath = state->get<std::string>("buildpath");
            filename = boost::filesystem::absolute(filename, buildpath).string();
        }

        if(!state->has("initialdisplay") || !state->get<bool>("initialdisplay"))
        {
            Core::loadFileSignal(filename);
            Core::setCursorPositionSignal(std::stoi(match[1]), 0);
            state->set("initialdisplay", true);
        }
    }

    return ret;
}

bool
infoaddressresponse(const Gdb::Result &result, int token)
{
    static std::regex addrRegex(R"regex(Symbol \\"(.*)\(.*\)\\" is a function at address (0x[0-9a-f]+)\.\\n)regex");

    std::smatch match;
    auto ret = result.token.value == -1 &&
               result.message.type == Gdb::Message::Type::NONE &&
               result.payload.type == Gdb::Payload::Type::STRING &&
               result.stream == Gdb::Stream::STDOUT &&
               std::regex_match(result.payload.string.string, match, addrRegex);

    if (ret)
    {
        std::string cmd = "interpreter-exec console \"info line *" + match[2].str() + "\"";
        Core::gdb()->executeCommand(cmd, infolineresponse);
    }

    return ret;
};

}

namespace Gdb
{

namespace Handlers
{

void infoAddress(const std::string &function)
{
    std::string cmd = "interpreter-exec console \"info address " + function + "\"";
    Core::gdb()->executeCommand(cmd, infoaddressresponse);
}

}

}
