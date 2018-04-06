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
#include <Python.h>

#include "core/workerThreadBase.h"

#include "pyGdbMiResult.h"

class GdbMiResultsReaderThread: public WorkerThreadBase
{
  public:
    GdbMiResultsReaderThread(const std::string &threadname,
                             PyObject *getresponsemethod,
                             std::function<void(PyGdbMiResult)> callable);
    virtual ~GdbMiResultsReaderThread();

  protected:
    virtual void            initialize() final;

  private:
    virtual void            process() final;

    PyObject *              m_getResponseMethod;

    std::function<void(PyGdbMiResult)>   m_callback;
};

#endif // GDBMIRESULTSREADERTHREAD_H_
