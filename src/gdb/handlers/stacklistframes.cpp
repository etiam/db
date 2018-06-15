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
#include "core/signal.h"

#include "gdb/commands.h"
#include "gdb/result.h"

namespace Gdb
{

namespace Handlers
{

bool
stacklistframes(const Gdb::Result &result, int token, boost::any data)
{
    auto match = result.token.value == token;

    /*
    {'message': u'done',
     'payload': {u'stack': [{u'addr': u'0x0000000000400e4c',
                             u'file': u'../tests/multiunitB.cpp',
                             u'fullname': u'/home/jasonr/workspace/db/tests/multiunitB.cpp',
                             u'func': u'somefunc',
                             u'level': u'0',
                             u'line': u'4'},
                            {u'addr': u'0x0000000000400dfc',
                             u'file': u'../tests/multiunitA.cpp',
                             u'fullname': u'/home/jasonr/workspace/db/tests/multiunitA.cpp',
                             u'func': u'main',
                             u'level': u'1',
                             u'line': u'27'}]},
     'stream': 'stdout',
     'token': 10,
     'type': 'result'}
    */

    if (match)
    {
        Core::CallStack callstack;

        // build callstack from results
        const auto &dict = result.payload.dict;
        if (dict.find("stack") != std::end(dict))
        {
            auto stack = boost::any_cast<Gdb::Payload::List>(result.payload.dict.at("stack"));
            for (const auto &anyentry : stack)
            {
                const auto &entry = boost::any_cast<Gdb::Payload::Dict>(anyentry);
                auto file = boost::any_cast<char *>(entry.at("file"));
                auto fullname = boost::any_cast<char *>(entry.at("fullname"));
                auto func = boost::any_cast<char *>(entry.at("func"));
                auto level = std::stoi(boost::any_cast<char *>(entry.at("level")));
                auto line = std::stoi(boost::any_cast<char *>(entry.at("line")));
                callstack.emplace_back(Core::Location({fullname, line}), file, func, level);
            }
        }

        // set the callstack global state
        Core::state()->setCallStack(callstack);
    }

    return match;
}

}

}