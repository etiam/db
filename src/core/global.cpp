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

#include "ast/builder.h"
#include "gdb/commands.h"

#include "state.h"
#include "optionsManager.h"
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
    static OptionsManagerPtr &      optionsManager();

    static Gdb::CommandsPtr &       gdbCommands();
    static Ast::BuilderPtr &        astBuilder();

  private:
    Master();

    static Master &                 instance();

    StatePtr                        m_state;
    OptionsManagerPtr               m_optionsManager;
    Gdb::CommandsPtr                m_gdbCommands;
    Ast::BuilderPtr                 m_AstBuilder;
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

OptionsManagerPtr &
Master::optionsManager()
{
    return instance().m_optionsManager;
}

Gdb::CommandsPtr &
Master::gdbCommands()
{
    return instance().m_gdbCommands;
}

Ast::BuilderPtr &
Master::astBuilder()
{
    return instance().m_AstBuilder;
}

Master::Master() :
    m_state(std::make_unique<State>()),
    m_optionsManager(std::make_unique<OptionsManager>()),
    m_gdbCommands(std::make_unique<Gdb::Commands>()),
    m_AstBuilder(std::make_unique<Ast::Builder>())
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

OptionsManagerPtr &
optionsManager()
{
    return Master::optionsManager();
}

Gdb::CommandsPtr &
gdb()
{
    return Master::gdbCommands();
}

Ast::BuilderPtr &
ast()
{
    return Master::astBuilder();
}

} // namespace Core
