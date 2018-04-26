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

namespace Responses
{

bool
infoline(const Gdb::Result &result, int token, boost::any data)
{
    static std::regex lineRegex(R"regex(Line (\d+) of \\"(.*)\\" starts at address 0x[0-9a-f]+ <(.*)\(.*\)>.*)regex");

    std::smatch smatch;
    auto match = result.token.value == -1 &&
                 std::regex_match(result.payload.string.string, smatch, lineRegex);

    if (match)
    {
        auto &state = Core::state();
        auto &vars = state->vars();
        auto filename = smatch[2].str();

        // if filename is relative, convert to absolute based on buildpath
        if (!boost::filesystem::path(smatch[2].str()).is_absolute())
        {
            auto buildpath = vars.get<std::string>("buildpath");
            filename = boost::filesystem::absolute(filename, buildpath).string();
        }

        if(!vars.has("initialdisplay") || !vars.get<bool>("initialdisplay"))
        {
            Core::Signal::loadFile(filename);
            Core::Signal::setCursorPosition(std::stoi(smatch[1]), 0);
            vars.set("initialdisplay", true);
        }
    }

    return match;
}

}

}
