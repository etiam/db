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
#include <boost/filesystem/operations.hpp>

#include "core/global.h"
#include "core/state.h"
#include "core/signals.h"
#include "gdb/controller.h"

#include "gdb/result.h"

namespace Gdb
{

namespace Handlers
{

Controller::HandlerReturn
breakdelete(const Gdb::Result &result, int token, boost::any data)
{
    auto matchtoken = (result.token.value == token);

    auto matchpayload = result.message.type == Message::Type::STRING &&
                        result.message.string.data == "breakpoint-deleted" &&
                        result.payload.type == Payload::Type::DICT &&
                        result.stream == Stream::STDOUT &&
                        result.type == Type::NOTIFY;

    auto match = matchtoken || matchpayload;

    /*
    {'message': u'breakpoint-deleted',
     'payload': {u'id': u'1'},
     'stream': 'stdout',
     'token': None,
     'type': 'notify'}
    */

    if (matchtoken)
    {
        auto number = boost::any_cast<int>(data);
        Core::state()->breakPoints().deleteBreakpoint(number);
    }

    else if (matchpayload)
    {
        auto number = std::stoi(boost::any_cast<char *>(result.payload.dict.at("id")));
        Core::state()->breakPoints().deleteBreakpoint(number);
    }

    return {"breakdelete", match, matchtoken ? Controller::MatchType::TOKEN : Controller::MatchType::METADATA};
};

}

}




