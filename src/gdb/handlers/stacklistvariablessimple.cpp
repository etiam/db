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
stacklistvariablessimple(const Gdb::Result &result, int token, boost::any data)
{
    auto match = result.token.value == token;

    /*
{'message': u'done',
 'payload': {u'variables': [{u'arg': u'1',
                             u'name': u'argc',
                             u'type': u'int',
                             u'value': u'2'},
                            {u'arg': u'1',
                             u'name': u'argv',
                             u'type': u'char **',
                             u'value': u'0x7fffffffd9b8'},
                            {u'name': u'infile_name',
                             u'type': u'std::__cxx11::string'},
                            {u'name': u'infile', u'type': u'std::ifstream'},
                            {u'name': u'width',
                             u'type': u'int',
                             u'value': u'725871085'},
                            {u'name': u'height',
                             u'type': u'int',
                             u'value': u'0'},
                            {u'name': u'outfile_name',
                             u'type': u'std::__cxx11::string'},
                            {u'name': u'scene', u'type': u'Scene'},
                            {u'name': u'image', u'type': u'Image'},
                            {u'name': u'start',
                             u'type': u'time_t',
                             u'value': u'4224500'}]},
 'stream': 'stdout',
 'token': 9,
 'type': 'result'}
    */

    if (match)
    {
        const auto &dict = result.payload.dict;
        if (dict.find("variables") != std::end(dict))
        {
            auto &svars = Core::state()->variables();
            auto variables = boost::any_cast<Gdb::Payload::List>(result.payload.dict.at("variables"));
            for (const auto &variable : variables)
            {
                const auto &entry = boost::any_cast<Gdb::Payload::Dict>(variable);

                auto name = boost::any_cast<char *>(entry.at("name"));
                auto type = boost::any_cast<char *>(entry.at("type"));

                svars[name] = { type, std::string() };
            }

            Core::Signals::variablesUpdated.emit();
        }
    }

    return {"stacklistvariablessimple", match, Controller::MatchType::TOKEN};
}

}

}
