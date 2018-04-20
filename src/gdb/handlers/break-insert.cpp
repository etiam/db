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

#include "gdb/controller.h"
#include "gdb/result.h"

namespace
{

bool
breakinsertresponse(const Gdb::Result &result, int token)
{
    auto ret = result.token.value == token;

    if (ret)
    {
        auto &state = Core::state();
        auto bkpt = boost::any_cast<Gdb::Payload::Dict>(result.payload.dict.at("bkpt"));

        auto filename = boost::any_cast<char *>(bkpt.at("fullname"));
        auto line = boost::any_cast<char *>(bkpt.at("line"));

        if(!state->has("initialdisplay") || !state->get<bool>("initialdisplay"))
        {
            Core::loadFileSignal(filename);
            Core::setCursorPositionSignal(std::stoi(line), 0);
            state->set("initialdisplay", true);
        }

        Core::showBreakpointMarkerSignal(std::stoi(line), true);
    }

    return ret;
};

}

namespace Gdb
{

namespace Handlers
{

void breakInsert(const std::string &location)
{
    std::string cmd = "break-insert  " + location;
    Core::gdb()->executeCommand(cmd, breakinsertresponse);
}

}

}




