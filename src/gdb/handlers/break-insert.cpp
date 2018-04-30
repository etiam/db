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
breakinsert(const Result &result, int token, boost::any data)
{
    bool match = false;

    const auto &payload = result.payload;
    if (payload.type == Payload::Type::DICT && payload.dict.find("bkpt") != std::end(payload.dict))
    {
        match = true;
        auto &state = Core::state();
        auto bkpt = boost::any_cast<Gdb::Payload::Dict>(result.payload.dict.at("bkpt"));

        auto fullname = boost::any_cast<char *>(bkpt.at("fullname"));
        auto line = std::stoi(boost::any_cast<char *>(bkpt.at("line")));
        auto number = std::stoi(boost::any_cast<char *>(bkpt.at("number")));

        state->breakpoints().insertBreakpoint(fullname, line, number);
    }

    return match;
};

}

}




