/*
 * infosources.cpp
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

#include "gdb/global.h"
#include "gdb/commands.h"
#include "gdb/result.h"
#include "gdb/controller.h"

#include "handlers.h"

namespace Gdb
{

namespace Handlers
{

// match the response of "info address ..." command
Controller::HandlerReturn
infofunctions(const Gdb::Result &result, int token, boost::any data)
{
    return {"infofunctions", false, Controller::MatchType::METADATA};
}

}

}
