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

#include "core/global.h"
#include "core/state.h"

#include "gdb/result.h"
#include "gdb/controller.h"

namespace Gdb
{

namespace Handlers
{

Controller::HandlerReturn
threadgroupstarted(const Gdb::Result &result, int token, boost::any data)
{
    /*
    {'message': u'thread-group-started',
     'payload': {u'id': u'i1', u'pid': u'7653'},
     'stream': 'stdout',
     'token': None,
     'type': 'notify'}
    */

    auto match = result.token.value == -1 &&
                 result.message.string.data == "thread-group-started" &&
                 result.type == Type::NOTIFY &&
                 result.payload.type == Payload::Type::DICT &&
                 result.payload.dict.find("pid") != std::end(result.payload.dict);

    if (match)
    {
        auto pid = std::stoi(boost::any_cast<char *>(result.payload.dict.at("pid")));
        Core::state()->vars().set("pid", pid);
    }

    return {"threadgroupstarted", match, Controller::MatchType::METADATA};
};

}

}




