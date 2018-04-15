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

wink::signal<wink::slot<void (const std::string &filename)>>     loadFileSignal;
wink::signal<wink::slot<void ()>>                                loadFileCompleteSignal;
wink::signal<wink::slot<void (int, int)>>                        setCursorPositionSignal;

wink::signal<wink::slot<void (const std::string &text, bool newline)>>  appendConsoleTextSignal;

} // namespace Core
