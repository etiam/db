/*
 * resultReaderThread.cpp
 *
 *  Created on: Sep 10, 2018
 *      Author: jasonr
 */

#ifdef HAVE_CONFIG_H
# include "config.h"
#endif

#include <iostream>

#include <Python.h>
#include <boost/python.hpp>

#include "core/global.h"
#include "core/signals.h"
#include "core/utils.h"

#include "resultReaderThread.h"

namespace Gdb
{

ResultReaderThread::ResultReaderThread(std::function<void (const Result &)> handler, PyObject * gethandlermethod, bool verbose) :
    Core::WorkerThread("gdbreader"),
    m_handler(handler),
    m_getHandlerMethod(gethandlermethod),
    m_verbose(verbose)
{
    m_thread = std::make_unique<std::thread>(&ResultReaderThread::run, std::ref(*this));
}

ResultReaderThread::~ResultReaderThread()
{
}

void
ResultReaderThread::run()
{
    pthread_setname_np(pthread_self(), m_threadName.c_str());

    // loop until end is signaled
    while(!m_done)
    {
        m_doneLock.lock();
        try
        {
            process();
        }

        catch (...)
        {
            std::cerr << Core::dumpStack() << std::endl;
        }
        m_doneLock.unlock();

        std::this_thread::sleep_for(std::chrono::milliseconds(50));
    }
}

void
ResultReaderThread::process()
{
    // acquire python GIL
    auto gstate = PyGILState_Ensure();

    // call get_gdb_response with no timeout
    auto args = Py_BuildValue("(d,i,i)", 0.0, false, m_verbose);
    auto value = PyObject_Call(m_getHandlerMethod, args, nullptr);

    if (PyErr_Occurred())
    {
        namespace bp = boost::python;

        PyObject *exc, *val, *tb;
        bp::object formatted_list, formatted;
        PyErr_Fetch(&exc, &val, &tb);
        bp::handle<> hexc(exc), hval(bp::allow_null(val));
        bp::object traceback(bp::import("traceback"));
        bp::object exceptionstringobject(traceback.attr("format_exception_only"));
        formatted_list = exceptionstringobject(hexc, hval);

        auto exceptionstring = bp::extract<std::string>(formatted_list[0])();

        // pygdbmi will throw a NoGdbProcessError when the gdb process exits
        // (this can happen if the user types "quit" in the gdb tab)
        // emit a quitRequested signal in this case to shutdown everything
        if (exceptionstring.find("NoGdbProcessError:") != std::string::npos)
        {
            Core::Signals::quitRequested();
        }

        PyErr_Clear();
    }

    else if (value && PyList_Check(value))
    {
        auto n = PyList_Size(value);
        for (auto i=0; i < n; i++)
        {
            auto r = parseResult(PyList_GetItem(value, i));

            try
            {
                m_handler(r);
            }
            catch (const std::exception &e)
            {
                std::cerr << "Exception caught in Gdb::Controller::resultReaderThread(): " << e.what() << std::endl;
                std::cerr << Core::dumpStack() << std::endl;
            }
        }
    }
    else
    {
        PyErr_Print();
    }

    // release python GIL
    PyGILState_Release(gstate);
}

}
