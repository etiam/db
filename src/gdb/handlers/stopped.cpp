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
#include "core/signal.h"

#include "gdb/commands.h"
#include "gdb/result.h"

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

            Core::Signal::loadFile(filename);
            Core::Signal::setCursorPosition(row, 0);
            Core::Signal::updateGutterMarker(row);

            Core::Signal::setCurrentLocation(Core::Location({filename, row}));

            Core::state()->setDebuggerState(Core::State::Debugger::PAUSED);
        }
    }

    return match;
}

}

}
