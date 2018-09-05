/*
 * loaderThread.cpp
 *
 *  Created on: Aug 30, 2018
 *      Author: jasonr
 */

#ifdef HAVE_CONFIG_H
# include "config.h"
#endif

#include <numeric>

#include "core/global.h"
#include "core/state.h"
#include "core/signals.h"
#include "core/timer.h"

#include "global.h"
#include "commands.h"
#include "loaderThread.h"

namespace Gdb
{

LoaderThread::LoaderThread() :
    Core::WorkerThread("gdbloader")
{
    m_thread = std::make_unique<std::thread>(&LoaderThread::run, std::ref(*this));
}

LoaderThread::~LoaderThread()
{
}

// TODO : look into moving all this to fileexec.cpp

void
LoaderThread::process()
{
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

    // if breakonmain true, set breakpoint, otherwise find source file for main
    if (vars.has("breakonmain") && vars.get<bool>("breakonmain"))
        gdb->insertBreakpoint("main");
    else
        gdb->infoAddress("main");

    gdb->getSourceFiles();
}

} // namespace Gdb
