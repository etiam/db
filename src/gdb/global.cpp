/*
 * global.cpp
 *
 *  Created on: Aug 19, 2018
 *      Author: jasonr
 */

#ifdef HAVE_CONFIG_H
# include "config.h"
#endif

#include <memory>

#include <thread>
#include <boost/utility.hpp>

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

    /*
    // set program arguments
    if (vm.count("args"))
    {
        const auto &args = vm["args"].as<std::vector<std::string>>();
        std::string argstr = std::accumulate(std::begin(args), std::end(args), std::string{},
            [](std::string &s, const std::string &piece) -> decltype(auto) { return s += piece + " "; });
        gdb->setArgs(argstr);
    }

    // if breakonmain true, set breakpoint, otherwise find source file for main
    if (Core::state()->vars().get<bool>("breakonmain"))
        gdb->insertBreakpoint("main");
    else
        gdb->infoAddress("main");

    gdb->getSourceFiles();
    */

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
