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

#include "pyGdbMiResult.h"
#include "gdbMiResultsReaderThread.h"

GdbMiResultsReaderThread::GdbMiResultsReaderThread(const std::string &threadname,
                                                   PyObject *getresponsemethod,
                                                   std::function<void(PyGdbMiResult)> callable) :
    WorkerThreadBase(threadname),
    m_getResponseMethod(getresponsemethod),
    m_callback(callable)
{
    m_thread = std::make_unique<std::thread>(&GdbMiResultsReaderThread::run, std::ref(*this));
}

GdbMiResultsReaderThread::~GdbMiResultsReaderThread()
{
}

void
GdbMiResultsReaderThread::initialize()
{
    PyEval_InitThreads();
    PyEval_SaveThread();
}

void
GdbMiResultsReaderThread::process()
{
    // save python GIL
    PyGILState_STATE gstate;
    gstate = PyGILState_Ensure();

    auto args = Py_BuildValue("(d,i,i)", 0.5, false, false);
    auto value = PyObject_Call(m_getResponseMethod, args, nullptr);

    if (value && PyList_Check(value))
    {
        auto n = PyList_Size(value);
        for (auto i=0; i < n; i++)
        {
            auto r = parseResult(PyList_GetItem(value, i));
            m_callback(r);
        }

    }
    else
    {
        PyErr_Print();
    }

    // restore python GIL
    PyGILState_Release(gstate);
}
