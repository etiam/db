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

#include "location.h"
#include "state.h"

namespace Core
{

namespace Signal
{

extern wink::signal<wink::slot<void (const std::string &filename)>>             loadFile;
extern wink::signal<wink::slot<void (int row, int col)>>                        setCursorPosition;
extern wink::signal<wink::slot<void (const Location &location)>>                setCurrentLocation;

extern wink::signal<wink::slot<void (int row)>>                                 updateGutterMarker;

extern wink::signal<wink::slot<void (const std::string &text)>>                 appendConsoleText;
extern wink::signal<wink::slot<void (const std::string &text)>>                 appendLogText;
extern wink::signal<wink::slot<void (const std::string &text)>>                 appendOutputText;

extern wink::signal<wink::slot<void (State::Debugger state)>>                   setDebuggerState;

}

}

#endif // CORE_SIGNAL_H_
