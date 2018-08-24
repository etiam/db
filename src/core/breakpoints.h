/*
 * breakpoints.h
 *
 *  Created on: Apr 22, 2018
 *      Author: root
 */

#pragma once
#ifndef CORE_BREAKPOINTS_H_
#define CORE_BREAKPOINTS_H_

#ifdef HAVE_CONFIG_H
# include "config.h"
#endif

#include <functional>

#include "state.h"

namespace Core
{

struct Breakpoint
{
    Location location;
    int breakpointnumber;
    unsigned int hitcount;
    bool enabled;
};

class Breakpoints
{
  public:
    Breakpoints() = default;
    ~Breakpoints() = default;

    // insert a new breakpoint
    void insertBreakpoint(const Breakpoint &breakpoint);

    // delete the given breakpoint
    void deleteBreakpoint(int breakpointnumber);

    // enable the given breakpoint
    void enableBreakpoint(int breakpointnumber);

    // disable the given breakpoint
    void disableBreakpoint(int breakpointnumber);

    // is a breakpoint present at the given filename. line
    bool exists(const Core::Location &location) const;

    // is the breakpoint at the given filename, line enabled?
    bool enabled(const Core::Location &location) const;

    // return the breakpoint at the given filename, line
    Breakpoint & find(const Core::Location &location);

    // return the breakpoint at the given filename, line
    Breakpoint & find(int breakpointnumber);

    // return a const list of breakpoints
    const std::vector<Breakpoint> & getAll() const;

  private:
    std::vector<Breakpoint>     m_breakpoints;
};

} // namespace Core

#endif // CORE_BREAKPOINTS_H_
