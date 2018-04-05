/*
 * gdbMiResultsReaderThread.cpp
 *
 *  Created on: Apr 5, 2018
 *      Author: jasonr
 */

#ifdef HAVE_CONFIG_H
# include "config.h"
#endif

#include <iostream>

#include "gdbMiResultsReaderThread.h"

GdbMiResultsReaderThread::GdbMiResultsReaderThread(const std::string &threadname, std::function<void()> callable) :
    WorkerThreadBase(threadname),
    m_callback(callable)
{
    m_thread = std::make_unique<std::thread>(&GdbMiResultsReaderThread::run, std::ref(*this));
}

GdbMiResultsReaderThread::~GdbMiResultsReaderThread()
{
}

void
GdbMiResultsReaderThread::process()
{
    m_callback();
}
