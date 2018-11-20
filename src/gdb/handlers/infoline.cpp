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
#include "gdb/controller.h"

#include "handlers.h"

namespace Gdb
{

namespace Handlers
{

// match the response of "info line ..." command
Controller::HandlerReturn
infoline(const Gdb::Result &result, int token, boost::any data)
{
    static std::regex pattern1(R"regex(Line (\d+) of \\"(.*)\\" starts at address 0x[0-9a-f]+ <(.*)\(.*\)> and ends.*)regex");
    static std::regex pattern2(R"regex(Line (\d+) of \\"(.*)\\" starts at address 0x[0-9a-f]+ <(.*)> and ends.*)regex");
    bool match = false;

    /*
    {'message': None,
     'payload': u'Line 23 of \\"../tests/multiunitA.cpp\\" starts at address 0x400d30 <main(int, char**)> and ends at 0x400d5a <main(int, char**)+42>.\\n',
     'stream': 'stdout',
     'type': 'console'}
    */

    std::smatch smatch;
    if (std::regex_match(result.payload.string.data, smatch, pattern1) ||
        std::regex_match(result.payload.string.data, smatch, pattern2))
    {
        auto symbolname = boost::any_cast<std::string>(data);

        std::cout << "FOO " << symbolname << " " << smatch[3] << std::endl;

        if (symbolname == smatch[3].str())
        {
            match = true;

            auto &vars = Core::state()->vars();

            const auto line = std::stoi(smatch[1]);
            const auto func = smatch[3].str();
            auto filename = smatch[2].str();

            // if path is relative, convert to absolute based on buildpath
            auto rel = !boost::filesystem::path(filename).is_absolute();
            if (rel)
            {
                auto parpath = Core::state()->vars().get<std::string>("buildpath");
                filename = boost::filesystem::canonical(boost::filesystem::path(parpath) / boost::filesystem::path(filename)).string();
            }

            const auto location = Core::Location({func, filename, line});

            // if the editor has not displayed anything yet load filename and set the cursor
            if(!vars.has("initialdisplay") || !vars.get<bool>("initialdisplay"))
            {
                Core::Signals::loadEditorSource.emit(boost::filesystem::canonical(filename).string());
                Core::Signals::setCursorLocation.emit(location);
                vars.set("initialdisplay", true);
            }
        }
    }

    return {"infoline", match, Controller::MatchType::REGEX};
}

}

}
