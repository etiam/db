/*
 * running.cpp
 *
 *  Created on: May 15, 2018
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
running(const Result &result, int token, boost::any data)
{
    auto match = result.message.type == Message::Type::STRING &&
                 result.message.string.data == "running" &&
                 result.payload.type == Payload::Type::NONE &&
                 result.stream == Stream::STDOUT &&
                 result.type == Type::RESULT;

    /*
    {'message': u'running',
     'payload': None,
     'stream': 'stdout',
     'token': 6,
     'type': 'result'}
    */

    if (match)
    {
        Core::state()->setDebuggerState(Core::State::Debugger::RUNNING);
    }

    return {"running", match, Controller::MatchType::TOKEN};
}

}

}
