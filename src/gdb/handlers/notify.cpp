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

#include "gdb/commands.h"
#include "gdb/result.h"

namespace Gdb
{

namespace Handlers
{

bool
notify(const Result &result, int token, boost::any data)
{
    auto match = result.message.type == Message::Type::STRING &&
                 result.stream == Stream::STDOUT &&
                 result.type == Type::NOTIFY;

    if (match)
    {
        Core::Signals::appendLogText(result.message.string.data + '\n');
    }

    return match;
}

}

}
