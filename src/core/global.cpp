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

#include "ast/astBuilder.h"
#include "gdb/gdbController.h"

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

    static OptionsManagerPtr &      optionsManager();

    static Gdb::GdbControllerPtr &  gdbController();
    static Ast::AstBuilderPtr &     astBuilder();


  private:
    Master();

    static Master &                 instance();

    OptionsManagerPtr               m_optionsManager;
    Gdb::GdbControllerPtr           m_gdbController;
    Ast::AstBuilderPtr              m_AstBuilder;
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

OptionsManagerPtr &
Master::optionsManager()
{
    return instance().m_optionsManager;
}

Gdb::GdbControllerPtr &
Master::gdbController()
{
    return instance().m_gdbController;
}

Ast::AstBuilderPtr &
Master::astBuilder()
{
    return instance().m_AstBuilder;
}

Master::Master() :
    m_optionsManager(std::make_unique<OptionsManager>()),
    m_gdbController(std::make_unique<Gdb::GdbController>()),
    m_AstBuilder(std::make_unique<Ast::AstBuilder>())
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

OptionsManagerPtr &
optionsManager()
{
    return Master::optionsManager();
}

Gdb::GdbControllerPtr &
gdbController()
{
    return Master::gdbController();
}

Ast::AstBuilderPtr &
astBuilder()
{
    return Master::astBuilder();
}

} // namespace Core
