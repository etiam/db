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

namespace Signal
{

wink::signal<wink::slot<void (const std::string &filename)>>            loadFile;
wink::signal<wink::slot<void (int row, int col)>>                       setCursorPosition;
wink::signal<wink::slot<void (int row, bool enabled)>>                  showBreakpointMarker;
wink::signal<wink::slot<void (int row)>>                                clearBreakpointMarker;

wink::signal<wink::slot<void (const std::string &text)>>                appendConsoleText;
wink::signal<wink::slot<void (const std::string &text)>>                appendLogText;
wink::signal<wink::slot<void (const std::string &text)>>                appendOutputText;

}

}
