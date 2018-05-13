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

Breakpoints::Breakpoints()
{
}

Breakpoints::~Breakpoints()
{
}

void
Breakpoints::toggleBreakpoint(const std::string &filename, int line)
{
    auto it = std::find_if(std::begin(m_breakpoints), std::end(m_breakpoints),
        [&](const Breakpoint &b) { return b.filename == filename && b.line == line; });

    if (it == std::end(m_breakpoints))
    {
        Core::gdb()->insertBreakpoint(filename + ":" + std::to_string(line));
    }
    else
    {
       if (it->enabled)
       {
          Core::gdb()->disableBreakpoint(it->number);
       }
       else
       {
          Core::gdb()->deleteBreakpoint(it->number);
       }
    }
}

void
Breakpoints::insertBreakpoint(const std::string &filename, int line, int number)
{
    m_breakpoints.push_back({filename, line, number, true});
    Core::Signal::updateGutterMarker(line);

    auto &vars = Core::state()->vars();
    if(!vars.has("initialdisplay") || !vars.get<bool>("initialdisplay"))
    {
        Core::Signal::setCurrentLocation(Location({filename, line}));
        vars.set("initialdisplay", true);
    }
}

void
Breakpoints::disableBreakpoint(int number)
{
    auto it = std::find_if(std::begin(m_breakpoints), std::end(m_breakpoints),
        [&](const Breakpoint &b) { return b.number == number; });

    if (it != std::end(m_breakpoints))
    {
        it->enabled = false;
    }

    Core::Signal::updateGutterMarker(it->line);
}

void
Breakpoints::deleteBreakpoint(int number)
{
    auto it = std::find_if(std::begin(m_breakpoints), std::end(m_breakpoints),
        [&](const Breakpoint &b) { return b.number == number; });

    if (it != std::end(m_breakpoints))
    {
        m_breakpoints.erase(it);
    }

    Core::Signal::updateGutterMarker(it->line);
}


bool
Breakpoints::present(int line) const
{
    bool present = false;
    for (const auto &breakpoint : m_breakpoints)
    {
        if (breakpoint.line == line)
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
        if (breakpoint.line == line && breakpoint.enabled)
        {
            enabled = true;
            break;
        }
    }

    return enabled;
}

} // namespace Core
