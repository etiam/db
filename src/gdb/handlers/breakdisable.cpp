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
#include <boost/filesystem/operations.hpp>

#include "core/global.h"
#include "core/state.h"
#include "core/signals.h"
#include "gdb/controller.h"

#include "gdb/result.h"

namespace Gdb
{

namespace Handlers
{

Controller::HandlerReturn
breakdisable(const Gdb::Result &result, int token, boost::any data)
{
    auto match = result.token.value == token;

    if (match)
    {
        auto number = boost::any_cast<int>(data);
        Core::state()->breakPoints().disableBreakpoint(number);
    }

    return {"breakdisable", match, Controller::MatchType::TOKEN};
};

}

}




