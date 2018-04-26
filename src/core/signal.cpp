/*
 * signal.cpp
 *
 *  Created on: Jul 19, 2017
 *      Author: jasonr
 */

#ifdef HAVE_CONFIG_H
# include "config.h"
#endif

#include "signal.h"

namespace Core
{

wink::signal<wink::slot<void (const std::string &filename)>>            loadFileSignal;
wink::signal<wink::slot<void (int row, int col)>>                       setCursorPositionSignal;
wink::signal<wink::slot<void (int row, bool enabled)>>                  showBreakpointMarkerSignal;
wink::signal<wink::slot<void (int row)>>                                clearBreakpointMarkerSignal;
wink::signal<wink::slot<void (const std::string &text)>>                appendConsoleTextSignal;

} // namespace Core
