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
                 result.message.string.string == "stopped" &&
                 result.payload.type == Payload::Type::DICT &&
                 result.stream == Stream::STDOUT &&
                 result.type == Type::NOTIFY;

    if (match)
    {
        const auto &dict = result.payload.dict;
        if (dict.find("frame") != std::end(dict))
        {
            auto frame = boost::any_cast<Gdb::Payload::Dict>(dict.at("frame"));
            auto filename = boost::any_cast<char *>(frame.at("fullname"));
            auto row = std::stoi(boost::any_cast<char *>(frame.at("line")));

            // update global state
            Core::state()->setDebuggerState(Core::State::Debugger::PAUSED);

            // update editor with current filename
            Core::Signal::loadFile(filename);

            // update global current location
            Core::Signal::setCurrentLocation(Core::Location({filename, row}));

            // update current gutter marker
            Core::Signal::updateGutterMarker(row);

            // get current call stack from gdb
            std::string cmd = "stack-list-frames";
            Core::gdb()->executeCommand(cmd, Gdb::Handlers::stacklistframes);
        }
    }

    return match;
}

}

}
