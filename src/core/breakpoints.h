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

namespace Core
{

class Breakpoints
{
  public:
    Breakpoints();
    ~Breakpoints();

    void    toggleBreakpoint(const std::string &filename, int line);

    void    insertBreakpoint(const std::string &filename, int line, int number);
    void    disableBreakpoint(int number);
    void    deleteBreakpoint(int number);

    bool    present(int line) const;
    bool    enabled(int line) const;

  private:
    struct Breakpoint
    {
        std::string filename;
        int         line;
        int         number;
        bool        enabled;
    };

    std::vector<Breakpoint>     m_breakpoints;
};

} // namespace Core

#endif // CORE_BREAKPOINTS_H_
