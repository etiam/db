/*
 * running.cpp
 *
 *  Created on: May 15, 2018
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

#include "gdb/global.h"
#include "gdb/commands.h"
#include "gdb/result.h"
#include "gdb/controller.h"

namespace
{

// https://stackoverflow.com/questions/11295019/environment-path-directories-iteration
const std::vector<std::string> &
get_environment_path()
{
    static std::vector<std::string> result;
    if (!result.empty())
        return result;

#if _WIN32
    const std::string path = convert_to_utf8( _wgetenv(L"PATH") ); // Handle Unicode, just remove if you don't want/need this. convert_to_utf8 uses WideCharToMultiByte in the Win32 API
    const char delimiter = ';';
#else
    const std::string path = getenv("PATH");
    const char delimiter = ':';
#endif
    if (path.empty())
        throw std::runtime_error("PATH should not be empty");

    size_t previous = 0;
    size_t index = path.find(delimiter);
    while (index != std::string::npos)
    {
        result.push_back(path.substr(previous, index - previous));
        previous = index + 1;
        index = path.find(delimiter, previous);
    }
    result.push_back(path.substr(previous));

    return result;
}

}

namespace Gdb
{

namespace Handlers
{

Controller::HandlerReturn
gdbversion(const Result &result, int token, boost::any data)
{
    auto match = result.token.value == token;

    if (match)
    {
        const auto &vars = Core::state()->vars();

        // once gdb-version command completes load the program if progname is defined
        if (vars.has("progname"))
        {
            auto &vars = Core::state()->vars();

            bool found = false;
            auto prog = vars.get<std::string>("progname");

            // if prog doesn't exist in current dir search $PATH for it
            if(!boost::filesystem::exists(prog))
            {
                auto path = get_environment_path();
                for (const auto &p : path)
                {
                    if (boost::filesystem::exists(boost::filesystem::path(p) / prog))
                    {
                        prog = (boost::filesystem::path(p) / prog).string();
                        found = true;
                        break;
                    }
                }
            }
            else
            {
                found = true;
            }

            if (found)
            {
                auto buildpath = boost::filesystem::canonical(boost::filesystem::path(prog).parent_path()).string();
                vars.set("buildpath", buildpath);

                Core::Signals::appendConsoleText.emit("Reading symbols from " + prog + "...");
                commands()->loadProgram(prog);
            }
            else
            {
                std::stringstream msg;
                msg << prog << " : No such file or directory." << std::endl;
                Core::Signals::appendConsoleText.emit(msg.str());
            }


        }
    }

    return {"gdbversion", match, Controller::MatchType::TOKEN};
}

}

}
