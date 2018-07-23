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
breakinsert(const Result &result, int token, boost::any data)
{
    auto match = result.token.value == token;

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
    */

    if (match)
    {
        auto &state = Core::state();
        auto bkpt = boost::any_cast<Gdb::Payload::Dict>(result.payload.dict.at("bkpt"));

        auto filename = boost::any_cast<char *>(bkpt.at("fullname"));
        auto row = std::stoi(boost::any_cast<char *>(bkpt.at("line")));
        auto breakpointnumber = std::stoi(boost::any_cast<char *>(bkpt.at("number")));

        state->breakPoints().insertBreakpoint(filename, row, breakpointnumber);

        // if the editor has not displayed anything yet
        auto &vars = Core::state()->vars();
        if(!vars.has("initialdisplay") || !vars.get<bool>("initialdisplay"))
        {
            Core::Signal::loadFile(filename);
            Core::Signal::setCursorPosition(row, 0);
            vars.set("initialdisplay", true);
        }
    }

    return match;
};

}

}




