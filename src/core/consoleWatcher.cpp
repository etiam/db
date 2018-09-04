/*
 * redirectThread.cpp
 *
 *  Created on: Sep 2, 2018
 *      Author: jasonr
 */

#ifdef HAVE_CONFIG_H
# include "config.h"
#endif

#include <core/consoleWatcher.h>

namespace Core
{

ConsoleWatcher::ConsoleWatcher() :
    WorkerThread("redirector")
{
    m_thread = std::make_unique<std::thread>(&ConsoleWatcher::run, std::ref(*this));
}

ConsoleWatcher::~ConsoleWatcher()
{
}

void
ConsoleWatcher::process()
{
}

} // namespace Ui
