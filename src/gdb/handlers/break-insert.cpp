/*
 * breakpoint.cpp
 *
 *  Created on: Apr 17, 2018
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
breakinsertresponse(const Result &result, int token)
{
    bool match = false;

    const auto &payload = result.payload;
    if (payload.type == Payload::Type::DICT && payload.dict.find("bkpt") != std::end(payload.dict))
    {
        match = true;
        auto &state = Core::state();
        auto &vars = state->vars();
        auto bkpt = boost::any_cast<Gdb::Payload::Dict>(result.payload.dict.at("bkpt"));

        auto filename = boost::any_cast<char *>(bkpt.at("fullname"));
        auto line = boost::any_cast<char *>(bkpt.at("line"));

//        state->
        Core::showBreakpointMarkerSignal(std::stoi(line), true);

        if(!vars.has("initialdisplay") || !vars.get<bool>("initialdisplay"))
        {
            Core::loadFileSignal(filename);
            Core::setCursorPositionSignal(std::stoi(line), 0);
            vars.set("initialdisplay", true);
        }

    }

    return match;
};

}

}




