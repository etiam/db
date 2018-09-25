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

namespace Gdb
{

namespace Handlers
{

Controller::HandlerReturn
output(const Result &result, int token, boost::any data)
{
    auto match = result.message.type == Message::Type::NONE &&
                 result.payload.type == Payload::Type::STRING &&
                 result.stream == Stream::STDOUT &&
                 result.type == Type::OUTPUT;

    if (match)
    {
        Core::Signals::appendOutputText.emit(result.payload.string.data + '\n');
    }

    return {"output", match, Controller::MatchType::REGEX};
}

}

}
