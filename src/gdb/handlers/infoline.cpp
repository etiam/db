/*
 * gdbUtiil.cpp
 *
 *  Created on: Apr 10, 2018
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

#include "handlers.h"

namespace Gdb
{

namespace Handlers
{

// match the response of "info line ..." command
bool
infoline(const Gdb::Result &result, int token, boost::any data)
{
    static std::regex startsataddress(R"regex(Line (\d+) of \\"(.*)\\" starts at address 0x[0-9a-f]+ <(.*)\(.*\)>.*)regex");
    bool match = false;

    /*
    {'message': None,
     'payload': u'Line 23 of \\"../tests/multiunitA.cpp\\" starts at address 0x400d30 <main(int, char**)> and ends at 0x400d5a <main(int, char**)+42>.\\n',
     'stream': 'stdout',
     'type': 'console'}
    */

    std::smatch smatch;
    if (std::regex_match(result.payload.string.data, smatch, startsataddress))
    {
        match = true;

        auto &vars = Core::state()->vars();

        const auto line = std::stoi(smatch[1]);
        const auto filename = smatch[2].str();
        const auto func = smatch[3].str();

        const auto location = Core::Location({func, filename, line});

        // if the editor has not displayed anything yet load filename and set the cursor
        if(!vars.has("initialdisplay") || !vars.get<bool>("initialdisplay"))
        {
            Core::Signals::loadEditorSource(boost::filesystem::canonical(filename).string());
            Core::Signals::setCursorLocation(location);
            vars.set("initialdisplay", true);
        }
    }

    return match;
}

}

}
