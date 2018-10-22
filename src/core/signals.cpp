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
Signal<> requestQuit;

Signal<const Location &> setCursorLocation;
Signal<const Location &> highlightLocation;

Signal<const Location &> updateGutterMarker;

Signal<> clearCurrentLocation;

Signal<const std::string &> appendConsoleText;
Signal<const std::string &> appendLogText;
Signal<const std::string &> appendOutputText;
Signal<const std::string &> appendStdoutText;
Signal<const std::string &> appendStderrText;

Signal<const std::string &> setStatusbarText;

Signal<> callStackUpdated;

Signal<> debuggerStateUpdated;

Signal<> breakPointsUpdated;

Signal<> sourceListUpdated;

Signal<> functionListUpdated;

Signal<> programLoaded;

Signal<> uiRealized;

Signal<const std::string &> executeGdbCommand;
}

}
