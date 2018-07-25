/*
 * breakpoints.cpp
 *
 *  Created on: Apr 22, 2018
 *      Author: root
 */

#ifdef HAVE_CONFIG_H
# include "config.h"
#endif

#include "gdb/commands.h"

#include "global.h"
#include "signal.h"
#include "state.h"
#include "breakpoints.h"

namespace Core
{

void
Breakpoints::insertBreakpoint(const Location &location, int breakpointnumber)
{
    m_breakpoints.push_back({location, breakpointnumber, true});
    Core::Signal::updateGutterMarker(location.row);
}

void
Breakpoints::enableBreakpoint(int number)
{
    auto it = std::find_if(std::begin(m_breakpoints), std::end(m_breakpoints),
        [&](const Breakpoint &b) { return b.breakpointnumber == number; });

    if (it != std::end(m_breakpoints))
    {
        it->enabled = true;
    }

    Core::Signal::updateGutterMarker(it->location.row);
}

void
Breakpoints::disableBreakpoint(int number)
{
    auto it = std::find_if(std::begin(m_breakpoints), std::end(m_breakpoints),
        [&](const Breakpoint &b) { return b.breakpointnumber == number; });

    if (it != std::end(m_breakpoints))
    {
        it->enabled = false;
    }

    Core::Signal::updateGutterMarker(it->location.row);
}

void
Breakpoints::deleteBreakpoint(int number)
{
    auto it = std::find_if(std::begin(m_breakpoints), std::end(m_breakpoints),
        [&](const Breakpoint &b) { return b.breakpointnumber == number; });

    if (it != std::end(m_breakpoints))
    {
        m_breakpoints.erase(it);
    }

    Core::Signal::updateGutterMarker(it->location.row);
}

bool
Breakpoints::exists(const std::string &filename, int line) const
{
    auto it = std::find_if(std::begin(m_breakpoints), std::end(m_breakpoints),
        [&](const Breakpoint &b) { return b.location.filename == filename && b.location.row == line; });

    return it != std::end(m_breakpoints);
}

bool
Breakpoints::enabled(const std::string &filename, int line) const
{
    auto it = std::find_if(std::begin(m_breakpoints), std::end(m_breakpoints),
           [&](const Breakpoint &b) { return b.location.filename == filename && b.location.row == line; });

    return it != std::end(m_breakpoints) && it->enabled;
}

const Breakpoint &
Breakpoints::find(const std::string &filename, int line) const
{
    auto it = std::find_if(std::begin(m_breakpoints), std::end(m_breakpoints),
           [&](const Breakpoint &b) { return b.location.filename == filename && b.location.row == line; });

    return *it;
}

const std::vector<Breakpoint> &
Breakpoints::getAll() const
{
    return m_breakpoints;
}

} // namespace Core
