/*
 * callStack.h
 *
 *  Created on: Sep 27, 2018
 *      Author: jasonr
 */

#ifndef CORE_CALLSTACK_H_
#define CORE_CALLSTACK_H_

#ifdef HAVE_CONFIG_H
# include "config.h"
#endif

#include <vector>

#include "types.h"

namespace Core
{

struct CallStackEntry
{
    CallStackEntry(Location loc, int le) :
        location(std::move(loc)),
        level(le) {}
    Location location;
    int level;
};

class CallStack
{
public:
    CallStack();
    ~CallStack() = default;

    using Entries = std::vector<CallStackEntry>;

    int currentFrame() const;
    void setCurrentFrame(int level);
    Entries & entries();

private:
    int m_currentFrame = 0;
    Entries m_entries;
};

} // namespace Core

#endif // CORE_CALLSTACK_H_
