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

namespace Core
{

class Breakpoints
{
  public:
    Breakpoints() = default;
    ~Breakpoints() = default;

    // this function only sends a request to gdb to insert, disable or delete a breakpoint.
    void    toggleBreakpoint(const std::string &filename, int line);

    // these functions directly modify m_breakpoints
    void    insertBreakpoint(const std::string &filename, int line, int breakpointnumber);
    void    disableBreakpoint(int breakpointnumber);
    void    deleteBreakpoint(int breakpointnumber);

    bool    present(int line) const;
    bool    enabled(int line) const;

    void    visit(std::function<void(const std::string &, int, int, bool)> visitor);

  private:
    struct Breakpoint
    {
        std::string filename;
        int         row;
        int         breakpointnumber;
        bool        enabled;
    };

    std::vector<Breakpoint>     m_breakpoints;
};

} // namespace Core

#endif // CORE_BREAKPOINTS_H_
