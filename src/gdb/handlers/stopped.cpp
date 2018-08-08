/*
 * stopped.cpp
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
#include "core/signal.h"

#include "gdb/commands.h"
#include "gdb/result.h"

#include "handlers.h"

namespace Gdb
{

namespace Handlers
{

bool
stopped(const Result &result, int token, boost::any data)
{
    auto match = result.message.type == Message::Type::STRING &&
                 result.message.string.data == "stopped" &&
                 result.payload.type == Payload::Type::DICT &&
                 result.stream == Stream::STDOUT &&
                 result.type == Type::NOTIFY;

    if (match)
    {
        const auto &dict = result.payload.dict;
        if (dict.find("frame") != std::end(dict))
        {
            auto frame = boost::any_cast<Gdb::Payload::Dict>(dict.at("frame"));
            auto func = boost::any_cast<char *>(frame.at("func"));
            auto fullname = boost::any_cast<char *>(frame.at("fullname"));
            auto line = std::stoi(boost::any_cast<char *>(frame.at("line")));

            // update global state
            Core::state()->setDebuggerState(Core::State::Debugger::PAUSED);

            // load editor with contents of filename
            Core::Signals::loadFile(fullname);

            // update global current location
            Core::Signals::setCurrentLocation(Core::Location({func, fullname, line}));

            // update current gutter marker
            Core::Signals::updateGutterMarker(line);

            // update call stack
            Core::gdb()->updateCallStack();
        }
    }

    return match;
}

}

}
