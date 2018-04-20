/*
 * signal.h
 *
 *  Created on: Jul 19, 2017
 *      Author: jasonr
 */

#ifndef CORE_SIGNAL_H_
#define CORE_SIGNAL_H_

#ifdef HAVE_CONFIG_H
# include "config.h"
#endif

#include <string>

#pragma GCC diagnostic push
#pragma GCC diagnostic ignored "-Wreorder"
#pragma GCC diagnostic ignored "-Wunused-local-typedef"
#pragma GCC diagnostic ignored "-Wunused-local-typedefs"
#include "external/wink/wink/signal.hpp"
#pragma GCC diagnostic pop

namespace Core
{

extern wink::signal<wink::slot<void (const std::string &filename)>>             loadFileSignal;
extern wink::signal<wink::slot<void (int row, int col)>>                        setCursorPositionSignal;
extern wink::signal<wink::slot<void (int rot, bool enabled)>>                   showBreakpointMarkerSignal;
extern wink::signal<wink::slot<void (const std::string &text, bool newline)>>   appendConsoleTextSignal;

} // namespace Core

#endif // CORE_SIGNAL_H_
