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

  private:
    Master();

    static Master &                 instance();

    StatePtr                        m_state;
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

Master::Master() :
    m_state(std::make_unique<State>())
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

} // namespace Core
