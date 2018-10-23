/*
 * stacklistframes.cpp
 *
 *  Created on: Jun 5, 2018
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

#include "gdb/result.h"
#include "gdb/controller.h"

namespace Gdb
{

namespace Handlers
{

Controller::HandlerReturn
stackinfoframe(const Gdb::Result &result, int token, boost::any data)
{
    auto match = result.token.value == token;

    /*
    {'token': 8,
     'message': 'done',
     'type': 'result',
     'payload': {'frame': '{'addr': '0x00007ffff773f0f7',
                            'file': '../sysdeps/ieee754/dbl-64/s_sin.c',
                            'fullname': '/build/glibc-OTsEL5/glibc-2.27/math/../sysdeps/ieee754/dbl-64/s_sin.c',
                            'func': '__cos_fma',
                            'level': '0',
                            'line': '578'}},
     'stream': 'stdout'}
    */

    if (match)
    {
        Core::CallStack callstack;

        // build callstack from results
        const auto &dict = result.payload.dict;
        if (dict.find("frame") != std::end(dict))
        {
            auto stack = boost::any_cast<Gdb::Payload::Dict>(result.payload.dict.at("frame"));
            auto level = std::stoi(boost::any_cast<char *>(stack.at("level")));

            // set global callstack's current level from 'level' in payload result
            Core::state()->setCurrentStackFrame(level);
        }
    }

    return {"stackinfoframe", match, Controller::MatchType::TOKEN};
}

}

}
