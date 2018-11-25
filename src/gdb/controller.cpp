/*
 * controller.cpp
 *
 *  Created on: Feb 22, 2018
 *      Author: jasonr
 */

#ifdef HAVE_CONFIG_H
# include "config.h"
#endif

#include <iostream>
#include <sstream>
#include <thread>
#include <regex>

#include <boost/any.hpp>
#include <boost/uuid/uuid.hpp>
#include <boost/uuid/uuid_io.hpp>
#include <boost/uuid/uuid_generators.hpp>
#include <boost/python.hpp>

#include <QFile>
#include <Python.h>
#include <marshal.h>

#include "fmt/format.h"
#include "fmt/ostream.h"

#include "core/global.h"
#include "core/signals.h"
#include "core/utils.h"

#include "resultReaderThread.h"
#include "controller.h"

namespace Gdb
{

class ControllerImpl
{
  public:
    ControllerImpl() = default;
    ~ControllerImpl() = default;

    void initialize();

    PyObject * importModule(const std::string &bytecodename, const std::string &modulename);
    PyObject * createInstance(const std::string &modulename, const std::string &classname);

    void resultHandler(const Result &result);

    int executeCommand(const std::string &command, Controller::HandlerFunc handler, boost::any data);

    /** Add a gdb command handler to the queue of handlers.  When a gdb response is received the handlers are called
     * in priority order (lower is higher priority).  Further checking of handlers stop after the first handler
     * that returns true.
     *
     * @param handler function object to add to the queue.
     * @param priority lower priority is better.
     * @param persistent handler will not be removed from queue if true.
     * @param blind data sent to handler.
     */
    void addHandler(Controller::HandlerFunc handler, int priority, bool persistent, boost::any data);

    void loadProgram(const std::string &filename);
    void insertBreakpoint(const std::string &location);
    void infoAddress(const std::string &function);

    bool m_verbose = false;
    bool m_initialized = false;
    bool m_valid = true;
    bool m_resultThreadActive = true;
    bool m_resultThreadDone = false;

    int m_token = 1;

    PyObject * m_writeMethod = nullptr; // pyobject holding ref to function that sends command to gdb
    PyObject * m_getHandlerMethod = nullptr; // pyobject holding ref to function that get's next gdb response

    struct HandlerData
    {
        Controller::HandlerFunc handler;
        int token;
        int priority;
        bool persistent;
        boost::any data;
        boost::uuids::uuid uuid;
    };

    std::mutex m_handlersLock;
    std::vector<HandlerData> m_handlers;
    std::unique_ptr<ResultReaderThread> m_readerThread;
};

void
ControllerImpl::initialize()
{
    m_verbose = Core::state()->vars().has("verbose");

    Py_Initialize();
    if (!PyEval_ThreadsInitialized())
    {
        PyEval_InitThreads();
    }

    auto moduledict = PyImport_GetModuleDict();

    // create blank module named 'pygdbmi'
    auto gdbmimodule = PyModule_New("pygdbmi");
    if (gdbmimodule && PyModule_Check(gdbmimodule))
    {
        // add module to global module dictionary
        PyDict_SetItemString(moduledict, "pygdbmi", gdbmimodule);

        // import pygdbmi submodules
        auto printcolormodule = importModule(":/pyc/printcolor.pyc", "pygdbmi.printcolor");
        if (printcolormodule && PyModule_Check(printcolormodule))
            PyModule_AddObject(gdbmimodule, "printcolor", printcolormodule);
        else
            m_valid = false;

        auto stringstreammodule = importModule(":/pyc/StringStream.pyc", "pygdbmi.StringStream");
        if (stringstreammodule && PyModule_Check(stringstreammodule))
            PyModule_AddObject(gdbmimodule, "StringStream", stringstreammodule);
        else
            m_valid = false;

        auto gdbmiparsermodule = importModule(":/pyc/gdbmiparser.pyc", "pygdbmi.gdbmiparser");
        if (gdbmiparsermodule && PyModule_Check(gdbmiparsermodule))
            PyModule_AddObject(gdbmimodule, "gdbmiparser", gdbmiparsermodule);
        else
            m_valid = false;

        auto gdbcontrollermodule = importModule(":/pyc/gdbcontroller.pyc", "pygdbmi.gdbcontroller");
        if (gdbcontrollermodule && PyModule_Check(gdbcontrollermodule))
            PyModule_AddObject(gdbmimodule, "gdbcontroller", gdbmiparsermodule);
        else
            m_valid = false;

        auto gdbmiinstance = createInstance("pygdbmi.gdbcontroller", "GdbController");

        if (gdbmiinstance)
        {
            m_writeMethod = PyObject_GetAttrString(gdbmiinstance, "write");
            m_getHandlerMethod = PyObject_GetAttrString(gdbmiinstance, "get_gdb_response");

            if (!m_writeMethod || !PyCallable_Check(m_writeMethod) ||
                !m_getHandlerMethod || !PyCallable_Check(m_getHandlerMethod))
                m_valid = false;

            // start reader thread
            m_readerThread = std::make_unique<ResultReaderThread>(std::bind(&ControllerImpl::resultHandler, this, std::placeholders::_1), m_getHandlerMethod, m_verbose);
        }
        else
            m_valid = false;
    }
    else
        m_valid = false;

    PyEval_SaveThread();

    if (!m_valid)
        std::cerr << "Gdb::Controller(): failed to initialize class" << std::endl;

    m_initialized = true;
}

PyObject *
ControllerImpl::importModule(const std::string &bytecodename, const std::string &modulename)
{
    PyObject *module = nullptr;
    QFile file(QString::fromStdString(bytecodename));

    if (file.open(QIODevice::ReadOnly))
    {
        auto blob = file.readAll();
        auto code = PyMarshal_ReadObjectFromString(blob.data()+8, blob.length()-8);
        module = PyImport_ExecCodeModule(const_cast<char*>(modulename.c_str()), code);
    }
    else
        std::cerr << "Gdb::Controller::importModule(): could not open \"" << bytecodename << "\"" << std::endl;

    return module;
}

PyObject *
ControllerImpl::createInstance(const std::string &modulename, const std::string &classname)
{
    PyObject *instance = nullptr;

    auto moduledict = PyImport_GetModuleDict();
    auto module = PyDict_GetItemString(moduledict, modulename.c_str());
    if (module)
    {
        auto dict = PyModule_GetDict(module);
        auto klass = PyDict_GetItemString(dict, classname.c_str());
        if (PyCallable_Check(klass))
        {
            auto args = Py_BuildValue("{s:i}", "verbose", m_verbose);
            instance = PyObject_Call(klass, nullptr, args);

            if (!instance || !PyInstance_Check(instance))
            {
                PyErr_Print();
            }
        }
        else
        {
            PyErr_Print();
        }
    }
    else
    {
        PyErr_Print();
    }

    return instance;
}

void
ControllerImpl::resultHandler(const Result &result)
{
    static auto verbose = Core::state()->vars().has("verbose");

    // since m_handlers can get modified inside the loop, store the uuid(s)
    // of the matched handler(s) to possibly delete later.
    std::vector<boost::uuids::uuid> uuids;
    bool match = false;

    // make a copy of m_handlers since m_handlers may get modified inside the loop
    std::vector<HandlerData> handlers;
    {
    std::unique_lock<std::mutex> locker(m_handlersLock);
    handlers = m_handlers;
    }

    for (const auto &handler : handlers)
    {
        Controller::HandlerReturn res;
        res = handler.handler(result, handler.token, handler.data);

        if (res.matched)
        {
            match = true;
            if (!handler.persistent)
                uuids.push_back(handler.uuid);
            if (verbose)
                fmt::print("handler \"{0}\" matched ({1})\n", res.name, res.type);
        }
    }

    auto removeexpired = [&](const HandlerData &h)
        { return std::find(std::begin(uuids), std::end(uuids), h.uuid) != std::end(uuids) && !h.persistent; };

    if (match)
    {
        std::unique_lock<std::mutex> locker(m_handlersLock);
        m_handlers.erase(std::remove_if(std::begin(m_handlers), std::end(m_handlers), removeexpired), std::end(m_handlers));
    }
    else
    {
        std::cout << "no match : " << result << std::endl;
    }
}

int
ControllerImpl::executeCommand(const std::string &command, Controller::HandlerFunc handler, boost::any data)
{
    Result result;

    if (!m_initialized)
        initialize();

    // store handler data
    if (handler != nullptr)
        addHandler(handler, 0, false, data);

    // acquire python GIL
    auto gstate = PyGILState_Ensure();

    auto cmd = fmt::format("{0}-{1}", m_token, command);
    auto args = Py_BuildValue("(s)", cmd.c_str()); //stream.str().c_str());
    auto kw = Py_BuildValue("{s:i,s:i}", "verbose", m_verbose, "read_response", false);
    PyObject_Call(m_writeMethod, args, kw);

    Py_DECREF(args);
    Py_DECREF(kw);

    // release python GIL
    PyGILState_Release(gstate);

    return m_token++;
}

void
ControllerImpl::addHandler(Controller::HandlerFunc handler, int priority, bool persistent, boost::any data)
{
    static boost::uuids::random_generator uuid_gen;

    auto uuid = uuid_gen();

    {
    std::unique_lock<std::mutex> locker(m_handlersLock);
    // persistent handlers don't receive valid tokens so set to 0
    if (persistent)
        m_handlers.push_back({handler, 0, priority, persistent, data, uuid});
    else
        m_handlers.push_back({handler, m_token, priority, persistent, data, uuid});
    }

    // sort handlers based on priority
    std::sort(std::begin(m_handlers), std::end(m_handlers),
              [](const HandlerData &a, const HandlerData &b) { return a.priority < b.priority; });
}

///////////////////////////////////

Controller::Controller() :
    m_impl(std::make_unique<ControllerImpl>())
{
}

Controller::~Controller()
{
}

int
Controller::executeCommand(const std::string &command, HandlerFunc handler, boost::any data)
{
    return m_impl->executeCommand(command, handler, data);
}

void
Controller::addHandler(Controller::HandlerFunc handler, int priority, bool persistent, boost::any data)
{
    m_impl->addHandler(handler, priority, persistent, data);
}

std::ostream &
operator <<(std::ostream &out, const Controller::MatchType &type)
{
    switch (type)
    {
        case Controller::MatchType::TOKEN:
            out << "token";
            break;

        case Controller::MatchType::REGEX:
            out << "regex";
            break;

        case Controller::MatchType::METADATA:
            out << "metadata";
            break;

        case Controller::MatchType::NONE:
            out << "None";
            break;
    }

    return out;
}

}

