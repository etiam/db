/*
 * callStack.cpp
 *
 *  Created on: Sep 7, 018
 *      Author: jasonr
 */

#ifdef HAVE_CONFIG_H
# include "config.h"
#endif

#include "callStack.h"

namespace Core
{

CallStack::CallStack()
{
}

int
CallStack::currentFrame() const
{
    return m_currentFrame;
}

void
CallStack::setCurrentFrame(int level)
{
    m_currentFrame = level;
}

CallStack::Entries &
CallStack::entries()
{
    return m_entries;
}


} // namespace Core
