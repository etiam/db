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

#include "gdb/result.h"
#include "gdb/controller.h"

#include "handlers.h"

namespace Gdb
{

namespace Handlers
{

Controller::HandlerReturn
processexited(const Gdb::Result &result, int token, boost::any data)
{
    // process exited response comes to stdout console
    auto typematch = result.token.value == -1 &&
                     result.stream == Stream::STDOUT &&
                     result.type == Type::CONSOLE;

    // now use regex to match payload string
    bool regexmatch = false;
    if (typematch)
    {
        static std::regex pattern(R"regex(\[Inferior \d \(process (\d+)\) exited normally\]\\n)regex");

        std::smatch smatch;

        if (std::regex_match(result.payload.string.data, smatch, pattern))
        {
            regexmatch = true;
            auto payloadpid = std::stoi(smatch[1]);

            if(Core::state()->vars().has("pid"))
            {
                auto pid = Core::state()->vars().get<int>("pid");

                // if response message pid matched current process pid set state to LOADED
                if (payloadpid == pid)
                {
                    Core::state()->setDebuggerState(Core::State::Debugger::LOADED);
                }
            }

        }
    }

    return {"processexited", typematch && regexmatch, Controller::MatchType::REGEX};
};

}

}
