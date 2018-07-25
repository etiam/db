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
breakmodified(const Gdb::Result &result, int token, boost::any data)
{
    auto match = result.message.type == Message::Type::STRING &&
                 result.message.string.data == "breakpoint-modified" &&
                 result.payload.type == Payload::Type::DICT &&
                 result.stream == Stream::STDOUT &&
                 result.type == Type::NOTIFY;

    /*
    {'message': u'breakpoint-modified',
     'payload': {u'bkpt': {u'addr': u'0x00000000004005ee',
                           u'disp': u'keep',
                           u'enabled': u'y',
                           u'file': u'/home/jasonr/workspace/db/tests/infiniteloop.cpp',
                           u'fullname': u'/home/jasonr/workspace/db/tests/infiniteloop.cpp',
                           u'func': u'main(int, char**)',
                           u'line': u'8',
                           u'number': u'1',
                           u'original-location': u'main',
                           u'thread-groups': [u'i1'],
                           u'times': u'4',
                           u'type': u'breakpoint'}},
     'stream': 'stdout',
     'token': None,
     'type': 'notify'}
    */

    if (match)
    {
        auto bkpt = boost::any_cast<Gdb::Payload::Dict>(result.payload.dict.at("bkpt"));

        auto breakpointnumber = std::stoi(boost::any_cast<char *>(bkpt.at("number")));
        unsigned int times = std::stoi(boost::any_cast<char *>(bkpt.at("times")));

        auto &bp = Core::state()->breakPoints().find(breakpointnumber);
        bp.hitcount = times;

        Core::Signal::breakPointsUpdated();
    }

    return match;
};

}

}




