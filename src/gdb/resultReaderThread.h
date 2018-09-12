/*
 * resultReaderThread.h
 *
 *  Created on: Sep 10, 2018
 *      Author: jasonr
 */

#ifndef SRC_GDB_RESULTREADERTHREAD_H_
#define SRC_GDB_RESULTREADERTHREAD_H_

#ifdef HAVE_CONFIG_H
# include "config.h"
#endif

#include <functional>

#include <Python.h>

#include "core/workerThread.h"

#include "result.h"

namespace Gdb
{

class ResultReaderThread : public Core::WorkerThread
{
public:
    ResultReaderThread(std::function<void (const Result &)> handler, PyObject * gethandlermethod, bool verbose);
    virtual ~ResultReaderThread();

protected:
    // custom version of run that doesn't wait for a trigger
    virtual void run() final;

private:
    virtual void process() final;

    std::function<void (const Result &)> m_handler;
    PyObject * m_getHandlerMethod;
    bool m_verbose;
};

}

#endif // SRC_GDB_RESULTREADERTHREAD_H_
