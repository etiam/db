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
#include "gdb/commands.h"

#include "state.h"
#include "global.h"

namespace Core
{

class Master : boost::noncopyable
{
  public:
    ~Master() = default;

    static void                     initialize();
    static void                     shutdown();

    static StatePtr &               state();

    static Gdb::CommandsPtr &       gdbCommands();

  private:
    Master();

    static Master &                 instance();

    StatePtr                        m_state;
    Gdb::CommandsPtr                m_gdbCommands;
};

std::unique_ptr<Master> theinstance;

void
Master::initialize()
{
}

void
Master::shutdown()
{
    if (theinstance)
        theinstance.reset();
}

StatePtr &
Master::state()
{
    return instance().m_state;
}

Gdb::CommandsPtr &
Master::gdbCommands()
{
    return instance().m_gdbCommands;
}

Master::Master() :
    m_state(std::make_unique<State>()),
    m_gdbCommands(std::make_unique<Gdb::Commands>())
{
}

Master &
Master::instance()
{
    if (!theinstance)
        theinstance = std::unique_ptr<Master>(new Master());
    return *theinstance;
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

Gdb::CommandsPtr &
gdb()
{
    return Master::gdbCommands();
}

} // namespace Core
