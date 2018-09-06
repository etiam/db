/*
 * global.cpp
 *
 *  Created on: Oct 8, 2012
 *      Author: jasonr
 */

#ifdef HAVE_CONFIG_H
# include "config.h"
#endif

#include <memory>

#include <boost/utility.hpp>

#include "ast/scanner.h"

#include "state.h"
#include "redirector.h"
#include "stdWatcherThread.h"
#include "global.h"

namespace Core
{

class Master: boost::noncopyable
{
public:
    ~Master() = default;

    static void initialize();
    static void shutdown();

    static StatePtr & state();

private:
    Master();

    static Master & instance();

    StatePtr m_state;
    std::unique_ptr<Redirector> m_stdout;
    std::unique_ptr<Redirector> m_stderr;
    std::unique_ptr<StdWatcherThread> m_stdWatcher;
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

StatePtr &
Master::state()
{
    return instance().m_state;
}

Master::Master() :
    m_state(std::make_unique<State>()),
    m_stdout(std::make_unique<Redirector>(stdout)),
    m_stderr(std::make_unique<Redirector>(stderr))
{
    if (m_stdout->getReadDescriptor() != -1 && m_stderr->getReadDescriptor() != -1)
    {
        m_stdWatcher = std::make_unique<StdWatcherThread>(m_stdout->getReadDescriptor(), m_stderr->getReadDescriptor());
//        m_stdWatcher = std::make_unique<StdWatcherThread>(0, m_stderr->getReadDescriptor());
    }
}

Master &
Master::instance()
{
    if (!g_instance)
        g_instance = std::unique_ptr<Master>(new Master());
    return *g_instance;
}

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

StatePtr &
state()
{
    return Master::state();
}

} // namespace Core
