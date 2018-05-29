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
interpreterexec(const Gdb::Result &result, int token, boost::any data)
{
    // interpreter-exec results always have the following set
    auto typematch = result.token.value == -1 &&
                     result.stream == Stream::STDOUT &&
                     result.type == Type::CONSOLE;

    // now use regexes to match payload string
    bool regexmatch = false;
    if (typematch)
    {
        static std::regex funcaddress(R"regex(Symbol \\"(.*)\(.*\)\\" is a function at address (0x[0-9a-f]+)\.\\n)regex");
        static std::regex startsataddress(R"regex(Line (\d+) of \\"(.*)\\" starts at address 0x[0-9a-f]+ <(.*)\(.*\)>.*)regex");

        std::smatch smatch;

        if (std::regex_match(result.payload.string.string, smatch, funcaddress))
        {
            regexmatch = true;
            std::string cmd = "interpreter-exec console \"info line *" + smatch[2].str() + "\"";
            Core::gdb()->executeCommand(cmd, Handlers::infoline);
        }

        else if (std::regex_match(result.payload.string.string, smatch, startsataddress))
        {
            auto &state = Core::state();
            auto &vars = state->vars();
            auto filename = smatch[2].str();

            // if filename is relative, convert to absolute based on buildpath
            if (!boost::filesystem::path(smatch[2].str()).is_absolute())
            {
                auto buildpath = vars.get<std::string>("buildpath");
                filename = boost::filesystem::canonical(filename).string();
            }

            if(!vars.has("initialdisplay") || !vars.get<bool>("initialdisplay"))
            {
                Core::Signal::loadFile(filename);
                Core::Signal::setCursorPosition(std::stoi(smatch[1]), 0);
                vars.set("initialdisplay", true);
            }
        }

    }

    return typematch && regexmatch;
};

}

}
