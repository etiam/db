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

#include <boost/utility.hpp>

#include "core/global.h"
#include "core/state.h"
#include "core/signals.h"
#include "core/timer.h"

#include "global.h"
#include "scannerThread.h"
#include "scanner.h"

namespace Ast
{

class Master: boost::noncopyable
{
public:
    ~Master();

    static void initialize();
    static void shutdown();

private:
    Master();

    static Master & instance();

    void WorkerThread();

    ScannerThread m_scannerThread;
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

Master::Master() //:
{
    Core::Signals::sourceListUpdated.connect([this]() { m_scannerThread.trigger(); });
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

} // namespace Ast
