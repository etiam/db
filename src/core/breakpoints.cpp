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
Breakpoints::toggleBreakpoint(const std::string &filename, int line)
{
    auto it = std::find_if(std::begin(m_breakpoints), std::end(m_breakpoints),
        [&](const Breakpoint &b) { return b.location.filename == filename && b.location.row == line; });

    if (it == std::end(m_breakpoints))
    {
        Core::gdb()->insertBreakpoint(filename + ":" + std::to_string(line));
    }
    else
    {
       if (it->enabled)
       {
          Core::gdb()->disableBreakpoint(it->breakpointnumber);
       }
       else
       {
          Core::gdb()->deleteBreakpoint(it->breakpointnumber);
       }
    }
}

void
Breakpoints::insertBreakpoint(const Location &location, int breakpointnumber)
{
    m_breakpoints.push_back({location, breakpointnumber, true});
    Core::Signal::updateGutterMarker(location.row);
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
Breakpoints::present(int line) const
{
    bool present = false;
    for (const auto &breakpoint : m_breakpoints)
    {
        if (breakpoint.location.row == line)
        {
            present = true;
            break;
        }
    }
    return present;
}

bool
Breakpoints::enabled(int line) const
{
    bool enabled = false;
    for (const auto &breakpoint : m_breakpoints)
    {
        if (breakpoint.location.row == line && breakpoint.enabled)
        {
            enabled = true;
            break;
        }
    }

    return enabled;
}

const std::vector<Breakpoint> &
Breakpoints::get() const
{
    return m_breakpoints;
}

} // namespace Core
