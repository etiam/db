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
       if (it->disabled)
       {
          Core::gdb()->deleteBreakpoint(it->number);
       }
       else
       {
          Core::gdb()->disableBreakpoint(it->number);
       }
    }
}

void
Breakpoints::insertBreakpoint(const std::string &filename, int line, int number)
{
    Core::Signal::showBreakpointMarker(line, true);
    m_breakpoints.push_back({filename, line, number, false});

    auto &state = Core::state();
    auto &vars = state->vars();
    if(!vars.has("initialdisplay") || !vars.get<bool>("initialdisplay"))
    {
        Core::Signal::loadFile(filename);
        Core::Signal::setCursorPosition(line, 0);
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
        Core::Signal::showBreakpointMarker(it->line, false);
        it->disabled = true;
    }
}

void
Breakpoints::deleteBreakpoint(int number)
{
    auto it = std::find_if(std::begin(m_breakpoints), std::end(m_breakpoints),
        [&](const Breakpoint &b) { return b.number == number; });

    if (it != std::end(m_breakpoints))
    {
        Core::Signal::clearBreakpointMarker(it->line);
        m_breakpoints.erase(it);
    }
}

} // namespace Core
