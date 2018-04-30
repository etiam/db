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
        Core::Signal::appendConsoleText("* stopped\n");

        const auto &dict = result.payload.dict;
        auto number = std::stoi(boost::any_cast<char *>(dict.at("bkptno")));
        if (dict.find("frame") != std::end(dict))
        {
            auto frame = boost::any_cast<Gdb::Payload::Dict>(dict.at("frame"));
            auto fullname = boost::any_cast<char *>(frame.at("fullname"));
            auto line = std::stoi(boost::any_cast<char *>(frame.at("line")));

            std::cout << fullname << " " << line << " " << number << std::endl;
            Core::state()->setCurrentLocation(Core::Location({fullname, line}));
        }

        /*
        {'message': u'stopped',
         'payload': {u'bkptno': u'1',
             u'core': u'12',
             u'disp': u'keep',
             u'frame': {u'addr': u'0x0000000000400d3a',
                        u'args': [],
                        u'file': u'main.cpp',
                        u'fullname': u'/home/jasonr/workspace/db/test/main.cpp',
                        u'func': u'main',
                        u'line': u'22'},
             u'reason': u'breakpoint-hit',
             u'stopped-threads': u'all',
             u'thread-id': u'1'},
         'stream': 'stdout',
         'token': None,
         'type': 'notify'}
         */
    }

    return match;
}

}

}
