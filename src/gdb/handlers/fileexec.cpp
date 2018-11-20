/*
 * gdbUtiil.cpp
 *
 *  Created on: Apr 10, 2018
 *      Author: jasonr
 */

#ifdef HAVE_CONFIG_H
# include "config.h"
#endif

#include <numeric>

#include "core/global.h"
#include "core/state.h"
#include "core/signals.h"

#include "gdb/result.h"
#include "gdb/controller.h"
#include "gdb/global.h"
#include "gdb/commands.h"

namespace Gdb
{

namespace Handlers
{

Controller::HandlerReturn
fileexec(const Gdb::Result &result, int token, boost::any data)
{
    auto match = result.token.value == token;

    if (match)
    {
        Core::state()->setDebuggerState(Core::State::Debugger::LOADED);
        Core::Signals::appendConsoleText.emit(result.message.string.data + '\n');
        Core::Signals::programLoaded.emit();

        auto &gdb = Gdb::commands();
        auto &vars = Core::state()->vars();

        // set program arguments
        if (vars.has("args"))
        {
            const auto args = vars.get<std::vector<std::string>>("args");
            std::string argstr = std::accumulate(std::begin(args), std::end(args), std::string{},
                [](std::string &s, const std::string &piece) -> decltype(auto) { return s += piece + " "; });
            gdb->setArgs(argstr);
        }

        // wait for "breakonmain" to be available
        while(!vars.has("breakonmain"));

        // if breakonmain true, set breakpoint, otherwise find source file for main
        if (vars.get<bool>("breakonmain"))
            gdb->insertBreakpoint("main");
        else
            gdb->infoAddress("main");

        gdb->getSourceFiles();
    }

    return {"fileexec", match, Controller::MatchType::TOKEN};
}

}

}
