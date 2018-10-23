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

    {'message': u'done',
     'payload': {u'stack': [{u'addr': u'0x00007ffff70c1b97',
                             u'file': u'../csu/libc-start.c',
                             u'fullname': u'/build/glibc-OTsEL5/glibc-2.27/csu/../csu/libc-start.c',
                             u'func': u'__libc_start_main',
                             u'level': u'0',
                             u'line': u'344'},
                            {u'addr': u'0x0000000000400c6a',
                             u'func': u'_start',
                             u'level': u'1'}]},
     'stream': 'stdout',
     'token': 16,
     'type': 'result'}
    */

    if (match)
    {
        const auto &dict = result.payload.dict;
        if (dict.find("stack") != std::end(dict))
        {
            auto stack = boost::any_cast<Gdb::Payload::List>(result.payload.dict.at("stack"));
            for (const auto &stackentry : stack)
            {
                const auto &entry = boost::any_cast<Gdb::Payload::Dict>(stackentry);
                auto func = boost::any_cast<char *>(entry.at("func"));
                auto level = std::stoi(boost::any_cast<char *>(entry.at("level")));

                std::string fullname = "None";
                if (entry.find("fullname") != std::end(entry))
                    fullname = boost::any_cast<char *>(entry.at("fullname"));

                int line = 0;
                if (entry.find("line") != std::end(entry))
                    line = std::stoi(boost::any_cast<char *>(entry.at("line")));

                Core::state()->callStack().emplace_back(Core::Location({func, fullname, line}), level);
            }
        }

        Core::Signals::callStackUpdated.emit();
    }

    return {"stacklistframes", match, Controller::MatchType::TOKEN};
}

}

}
