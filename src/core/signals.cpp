/*
 * signal.cpp
 *
 *  Created on: Jul 19, 2017
 *      Author: jasonr
 */

#ifdef HAVE_CONFIG_H
# include "config.h"
#endif

#include "signals.h"

namespace Core
{

namespace Signals
{

Signal<const std::string &> loadEditorSource;
Signal<> quitRequested;

Signal<const Location &> setCursorLocation;
Signal<const Location &> highlightCurrentLocation;

Signal<const Location &> updateGutterMarker;

Signal<> clearCurrentLocation;

Signal<const std::string &> appendConsoleText;
Signal<const std::string &> appendLogText;
Signal<const std::string &> appendOutputText;

Signal<const std::string &> setStatusbarText;

Signal<> callStackUpdated;

Signal<> debuggerStateUpdated;

Signal<> breakPointsUpdated;

Signal<> sourceListUpdated;

Signal<> functionListUpdated;
}

}
