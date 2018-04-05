/*
 * gdbMiResultsReaderThread.h
 *
 *  Created on: Apr 5, 2018
 *      Author: jasonr
 */

#pragma once
#ifndef GDBMIRESULTSREADERTHREAD_H_
#define GDBMIRESULTSREADERTHREAD_H_

#ifdef HAVE_CONFIG_H
# include "config.h"
#endif

#include <functional>

#include "core/workerThreadBase.h"

class GdbMiResultsReaderThread: public WorkerThreadBase
{
  public:
    GdbMiResultsReaderThread(const std::string &threadname, std::function<void()> callable);
    virtual ~GdbMiResultsReaderThread();

  private:
    virtual void          process() final;

    std::function<void()> m_callback;
};

#endif // GDBMIRESULTSREADERTHREAD_H_
