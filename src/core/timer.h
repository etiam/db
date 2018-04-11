/*
 * timer.h
 *
 *  Created on: Apr 10, 2018
 *      Author: jasonr
 */

#pragma once
#ifndef CORE_TIMER_H_
#define CORE_TIMER_H_

#ifdef HAVE_CONFIG_H
# include "config.h"
#endif

#include <iosfwd>
#include <chrono>

namespace Core
{

class Timer
{

  public:
    Timer() = default;
    ~Timer() = default;

    void    start()
    {
        m_stopped = false;
        m_start = std::chrono::high_resolution_clock::now();
    }

    void    stop()
    {
        m_stopped = true;
        m_stop = std::chrono::high_resolution_clock::now();
    }

    double  elapsedTime() const
    {
        // for some reason, returning the count casted to milliseconds has less precision
        // than casting to microseconds and dividing by 1000.0.
        if(m_stopped)
            return std::chrono::duration_cast<std::chrono::microseconds>(m_stop - m_start).count() / 1000.0f;
        else
            return std::chrono::duration_cast<std::chrono::microseconds>(std::chrono::high_resolution_clock::now() - m_start).count() / 1000.0f;
    }

  private:
    bool                                                            m_stopped = false;
    std::chrono::time_point<std::chrono::high_resolution_clock>     m_start = std::chrono::high_resolution_clock::now();
    std::chrono::time_point<std::chrono::high_resolution_clock>     m_stop = std::chrono::time_point<std::chrono::high_resolution_clock>();
};

} // namespace Core

inline
std::ostream &
operator <<(std::ostream &out, const Core::Timer &timer)
{
    out << timer.elapsedTime();

    return out;
}

#endif // CORE_TIMER_H_
