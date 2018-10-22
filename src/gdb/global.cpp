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
#include <thread>
#include <future>

#include <thread>
#include <boost/utility.hpp>

#include "core/global.h"
#include "core/state.h"
#include "core/signals.h"
#include "core/timer.h"

#include "global.h"
#include "handlers/handlers.h"
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

    Gdb::CommandsPtr m_gdbCommands;
};

std::unique_ptr<Master> g_instance;

void
Master::initialize()
{
    instance();
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
    m_gdbCommands(std::make_unique<Gdb::Commands>())
{
    Core::Signals::uiRealized.connect([this]() { m_gdbCommands->executeCommand("gdb-version", Handlers::gdbversion ); });
}

Master::~Master()
{
}

Master &
Master::instance()
{
    if (!g_instance)
        g_instance = std::unique_ptr<Master>(new Master());
    return *g_instance;
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
