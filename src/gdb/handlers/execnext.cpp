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

#include "gdb/result.h"
#include "gdb/controller.h"

namespace Gdb
{

namespace Handlers
{

Controller::HandlerReturn
execnext(const Gdb::Result &result, int token, boost::any data)
{
    auto match = result.token.value == token;

    if (match)
    {
    }

    return {"execnext", match, Controller::MatchType::TOKEN};
};

}

}




