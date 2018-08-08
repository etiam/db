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

namespace Signals
{

Signal<const std::string &> loadFile;
Signal<> quitRequested;

Signal<int, int> setCursorPosition;
Signal<int> updateGutterMarker;

Signal<const Location &> setCurrentLocation;
Signal<> clearCurrentLocation;

Signal<const std::string &> appendConsoleText;

Signal<const std::string &> appendLogText;
Signal<const std::string &> appendOutputText;

Signal<> callStackUpdated;

Signal<> debuggerStateUpdated;

Signal<> breakPointsUpdated;

Signal<> sourceListUpdated;
}

}
