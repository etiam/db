/*
 * global.cpp
 *
 *  Created on: Aug 19, 2018
 *      Author: jasonr
 */

#ifdef HAVE_CONFIG_H
# include "config.h"
#endif

#include <iostream>
#include <memory>
#include <numeric>

#include <thread>
#include <boost/utility.hpp>

#include "core/global.h"
#include "core/state.h"
#include "core/signals.h"

#include "global.h"
#include "commands.h"

namespace Gdb
{

class Master: boost::noncopyable
{
public:
    ~Master();

    static void initialize();
    static void shutdown();

    static Gdb::CommandsPtr & gdbCommands();

private:
    Master();

    static Master & instance();

    void WorkerThread();

    std::unique_ptr<std::thread> m_workerThread;
    Gdb::CommandsPtr m_gdbCommands;
};

std::unique_ptr<Master> g_instance;

void
Master::initialize()
{
}

void
Master::shutdown()
{
    if (g_instance)
        g_instance.reset();
}

CommandsPtr &
Master::gdbCommands()
{
    return instance().m_gdbCommands;
}

Master::Master() :
    m_workerThread(std::make_unique<std::thread>(&Master::WorkerThread, std::ref(*this))),
    m_gdbCommands(std::make_unique<Gdb::Commands>())
{
}

Master::~Master()
{
    m_workerThread->join();
}

Master &
Master::instance()
{
    if (!g_instance)
        g_instance = std::unique_ptr<Master>(new Master());
    return *g_instance;
}

void
Master::WorkerThread()
{
    pthread_setname_np(pthread_self(), "gdbworker");

    bool signaled = false;
    Core::Signals::programLoaded.connect([&signaled]() { signaled = true; });

    // wait for programLoaded signal to fire
    while(!signaled)
    {
        std::this_thread::sleep_for(std::chrono::milliseconds(10));
    }

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
    if (vars.get<bool>("breakonmain"))
        gdb->insertBreakpoint("main");
    else
        gdb->infoAddress("main");

    gdb->getSourceFiles();
}

////////////////////

void
initialize()
{
    Master::initialize();
}

void
shutdown()
{
    Master::shutdown();
}

CommandsPtr &
commands()
{
    return Master::gdbCommands();
}

} // namespace Gdb
