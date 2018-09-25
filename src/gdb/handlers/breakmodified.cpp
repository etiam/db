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
#include "core/signals.h"

#include "gdb/global.h"
#include "gdb/commands.h"
#include "gdb/result.h"
#include "gdb/controller.h"

namespace Gdb
{

namespace Handlers
{

Controller::HandlerReturn
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
        auto line = std::stoi(boost::any_cast<char *>(bkpt.at("line")));
        auto func = boost::any_cast<char *>(bkpt.at("func"));
        auto fullname = boost::any_cast<char *>(bkpt.at("fullname"));
        unsigned int times = std::stoi(boost::any_cast<char *>(bkpt.at("times")));
        auto enabled = std::string(boost::any_cast<char *>(bkpt.at("enabled"))) == "y" ? true : false;

        auto &bp = Core::state()->breakPoints().find(breakpointnumber);
        bp.hitcount = times;
        bp.enabled = enabled;

        Core::Signals::breakPointsUpdated.emit();
        Core::Signals::updateGutterMarker.emit(Core::Location({func, fullname, line}));
    }

    return {"breakmodified", match, Controller::MatchType::REGEX};
};

}

}




