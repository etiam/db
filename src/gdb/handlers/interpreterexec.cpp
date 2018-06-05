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

namespace
{

// match the response of "info address ..." command
bool
infoaddress(const Gdb::Result &result)
{
    static std::regex regex(R"regex(Symbol \\"(.*)\(.*\)\\" is a function at address (0x[0-9a-f]+)\.\\n)regex");
    bool match = false;

    std::smatch smatch;
    if (std::regex_match(result.payload.string.string, smatch, regex))
    {
        match = true;
        std::string cmd = "interpreter-exec console \"info line *" + smatch[2].str() + "\"";
        Core::gdb()->executeCommand(cmd, Gdb::Handlers::infoline);
    }

    return match;
}

// match the response of "info line ..." command
bool
infoline(const Gdb::Result &result)
{
    static std::regex startsataddress(R"regex(Line (\d+) of \\"(.*)\\" starts at address 0x[0-9a-f]+ <(.*)\(.*\)>.*)regex");
    bool match = false;

    std::smatch smatch;
    if (std::regex_match(result.payload.string.string, smatch, startsataddress))
    {
        match = true;

        auto &state = Core::state();
        auto &vars = state->vars();
        auto filename = smatch[2].str();

        // if filename is relative, convert to absolute based on buildpath
        if (!boost::filesystem::path(smatch[2].str()).is_absolute())
        {
            auto buildpath = vars.get<std::string>("buildpath");
            filename = boost::filesystem::canonical(filename).string();
        }

        // if the editor has not displayed anything yet
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

namespace Gdb
{

namespace Handlers
{

// the responses from "interpreter-exec console ..." commands do not have tokens and so
// must be matched only by their payload.  in most cases with a regex.
bool
interpreterexec(const Gdb::Result &result, int token, boost::any data)
{
    // static list of matchers
    static std::vector<std::function<bool(const Gdb::Result &)>> matchers = {
        ::infoaddress,
        ::infoline
    };
    bool match = false;

    // stop if a matcher matches
    for (const auto &matcher : matchers)
    {
        if (matcher(result))
        {
            match = true;
            break;
        }
    }

    return match;
};

}

}
