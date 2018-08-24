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

#include "ast/scanner.h"

#include "gdb/global.h"
#include "gdb/commands.h"
#include "gdb/result.h"

namespace Gdb
{

namespace Handlers
{

bool
breakinsert(const Result &result, int token, boost::any data)
{
    auto matchtoken = (result.token.value == token);

    auto matchpayload = result.message.type == Message::Type::STRING &&
                        result.message.string.data == "breakpoint-created" &&
                        result.payload.type == Payload::Type::DICT &&
                        result.stream == Stream::STDOUT &&
                        result.type == Type::NOTIFY;

    auto match = matchtoken || matchpayload;

    /*
    {'message': u'done',
     'payload': {u'bkpt': {u'addr': u'0x0000000000400d9a',
                           u'disp': u'keep',
                           u'enabled': u'y',
                           u'file': u'../tests/multiunitA.cpp',
                           u'fullname': u'/home/jasonr/workspace/db/tests/multiunitA.cpp',
                           u'func': u'main(int, char**)',
                           u'line': u'24',
                           u'number': u'1',
                           u'original-location': u'main',
                           u'thread-groups': [u'i1'],
                           u'times': u'0',
                           u'type': u'breakpoint'}},
     'stream': 'stdout',
     'token': 2,
     'type': 'result'}

    {'message': u'done',
     'payload': {u'bkpt': {u'addr': u'0x0000000000401e24',
                           u'at': u'<main+4>',
                           u'disp': u'keep',
                           u'enabled': u'y',
                           u'number': u'1',
                           u'original-location': u'main',
                           u'thread-groups': [u'i1'],
                           u'times': u'0',
                           u'type': u'breakpoint'}},
     'stream': 'stdout',
     'token': 3,
     'type': 'result'}
    */

    if (match)
    {
        auto bkpt = boost::any_cast<Gdb::Payload::Dict>(result.payload.dict.at("bkpt"));

        try
        {
            const auto fullname = boost::any_cast<char *>(bkpt.at("fullname"));
            const auto func = boost::any_cast<char *>(bkpt.at("func"));
            const auto line = std::stoi(boost::any_cast<char *>(bkpt.at("line")));
            const auto breakpointnumber = std::stoi(boost::any_cast<char *>(bkpt.at("number")));
            const unsigned int times = std::stoi(boost::any_cast<char *>(bkpt.at("times")));
            const auto enabled = std::string(boost::any_cast<char *>(bkpt.at("enabled"))) == "y" ? true : false;

            const auto location = Core::Location({func, fullname, line});

            const auto bp = Core::Breakpoint({location, breakpointnumber, times, enabled});
            Core::state()->breakPoints().insertBreakpoint(bp);

            // if the editor has not displayed anything yet load filename and set the cursor
            auto &vars = Core::state()->vars();
            if(!vars.has("initialdisplay") || !vars.get<bool>("initialdisplay"))
            {
                Core::Signals::loadEditorSource(fullname);
                Core::Signals::setCurrentLocation(location);
                vars.set("initialdisplay", true);
            }
        }
        catch (std::out_of_range &)
        {
        }
    }

    return match;
};

}

}




