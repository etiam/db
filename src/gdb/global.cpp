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

#include <boost/utility.hpp>

#include "global.h"
#include "commands.h"

namespace Gdb
{

class Master : boost::noncopyable
{
  public:
    ~Master() = default;

    static void                     initialize();
    static void                     shutdown();

    static Gdb::CommandsPtr &       gdbCommands();

  private:
    Master();

    static Master &                 instance();

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

CommandsPtr &
Master::gdbCommands()
{
    return instance().m_gdbCommands;
}

Master::Master() :
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

CommandsPtr &
commands()
{
    return Master::gdbCommands();
}

} // namespace Gdb
