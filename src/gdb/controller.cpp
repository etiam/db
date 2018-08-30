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
#include <boost/uuid/uuid_generators.hpp>
#include <boost/python.hpp>

#include <QFile>
#include <Python.h>
#include <marshal.h>

#include "core/global.h"
#include "core/signals.h"
#include "core/utils.h"

#include "controller.h"

namespace Gdb
{

class ControllerImpl
{
  public:
    ControllerImpl() = default;
    ~ControllerImpl();

    void            initialize();

    PyObject *      importModule(const std::string &bytecodename, const std::string &modulename);
    PyObject *      createInstance(const std::string &modulename, const std::string &classname);

    void            resultHandler(const Result &result);
    void            resultReaderThread();

    int             executeCommand(const std::string &command, Controller::HandlerFunc handler, boost::any data);

    /** Add a gdb command handler to the queue of handlers.  When a gdb response is received the handlers are called
     * in priority order (lower is higher priority).  Further checking of handlers stop after the first handler
     * that returns true.
     *
     * @param handler function object to add to the queue.
     * @param priority lower priority is better.
     * @param persistent handler will not be removed from queue if true.
     * @param blind data sent to handler.
     *
     */
    void            addHandler(Controller::HandlerFunc handler, int priority, bool persistent, boost::any data);

    void            loadProgram(const std::string &filename);
    void            insertBreakpoint(const std::string &location);
    void            infoAddress(const std::string &function);

    bool            m_verbose = false;
    bool            m_initialized = false;
    bool            m_valid = true;
    bool            m_resultThreadActive = true;
    bool            m_resultThreadDone = false;

    int             m_token = 1;

    PyObject *      m_writeMethod = nullptr;        // pyobject holding ref to function that sends command to gdb
    PyObject *      m_getHandlerMethod = nullptr;   // pyobject holding ref to function that get's next gdb response

    struct HandlerData
    {
        Controller::HandlerFunc     handler;
        int                         token;
        int                         priority;
        bool                        persistent;
        boost::any                  data;
        boost::uuids::uuid          uuid;
    };

    std::vector<HandlerData>        m_handlers;

    std::unique_ptr<std::thread>    m_readerThread;
};

ControllerImpl::~ControllerImpl()
{
    if (m_readerThread)
    {
        // stops result thread loop
        m_resultThreadActive = false;

        // waits for result thread loop to stop
        while(!m_resultThreadDone);

        // waits for result thread to finish
        m_readerThread->join();

//        Py_Finalize();        // TODO : fixme!
    }
}

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

            // start read thread
            m_readerThread = std::make_unique<std::thread>(&ControllerImpl::resultReaderThread, std::ref(*this));
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
    bool match = false;

    // since m_handlers can get modified inside the loop, store the uuid
    // of the matched handler to possibly delete it later
    boost::uuids::uuid uuid;
    for (auto it=std::begin(m_handlers); it != std::end(m_handlers); ++it)
    {
        uuid = it->uuid;
        auto res = it->handler(result, it->token, it->data);
        if (res.matched)
        {
            std::cout << "handler \"" << res.name << "\" matched ("
                      << (res.type == Controller::MatchType::TOKEN ? "token" : "regex") << ")" << std::endl;
            match = true;
            break;
        }
    }

    if (match)
    {
        // remove matched handler if non-persistent
        for (auto it=std::begin(m_handlers); it != std::end(m_handlers); ++it)
        {
            if (uuid == it->uuid && !it->persistent)
                m_handlers.erase(it);
        }
    }
    else
    {
        std::cout << "no match : " << result << std::endl;
    }
}

void
ControllerImpl::resultReaderThread()
{
    pthread_setname_np(pthread_self(), "resultreader");

    while(m_resultThreadActive)
    {
        // acquire python GIL
        auto gstate = PyGILState_Ensure();

        auto args = Py_BuildValue("(d,i,i)", 0.1, false, m_verbose);
        auto value = PyObject_Call(m_getHandlerMethod, args, nullptr);

        /////

        if (PyErr_Occurred())
        {
            namespace bp = boost::python;

            PyObject *exc, *val, *tb;
            bp::object formatted_list, formatted;
            PyErr_Fetch(&exc, &val, &tb);
            bp::handle<> hexc(exc), hval(bp::allow_null(val));
            bp::object traceback(bp::import("traceback"));
            bp::object excstrobject(traceback.attr("format_exception_only"));
            formatted_list = excstrobject(hexc, hval);

            auto excstr = bp::extract<std::string>(formatted_list[0])();
            if (excstr.find("NoGdbProcessError:") != std::string::npos)
            {
                Core::Signals::quitRequested();
            }

            PyErr_Clear();
        }

        /////

        else if (value && PyList_Check(value))
        {
            auto n = PyList_Size(value);
            for (auto i=0; i < n; i++)
            {
                auto r = parseResult(PyList_GetItem(value, i));

                try
                {
                    resultHandler(r);
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

        std::this_thread::sleep_for(std::chrono::milliseconds(500));
    }

    m_resultThreadDone = true;
}

int
ControllerImpl::executeCommand(const std::string &command, Controller::HandlerFunc handler, boost::any data)
{
    Result result;

    if (!m_initialized)
        initialize();

    std::stringstream stream;
    stream << m_token << "-" << command;

    // store handler data
    if (handler != nullptr)
        addHandler(handler, 0, false, data);

    // acquire python GIL
    auto gstate = PyGILState_Ensure();

    auto args = Py_BuildValue("(s)", stream.str().c_str());
    auto kw = Py_BuildValue("{s:i,s:i}", "verbose", m_verbose, "read_response", false);
    PyObject_Call(m_writeMethod, args, kw);

    // release python GIL
    PyGILState_Release(gstate);

    return m_token++;
}

void
ControllerImpl::addHandler(Controller::HandlerFunc handler, int priority, bool persistent, boost::any data)
{
    static boost::uuids::random_generator uuid_gen;

    // persistent handlers don't receive valid tokens so set to 0
    if (persistent)
        m_handlers.push_back({handler, 0, priority, persistent, data, uuid_gen()});
    else
        m_handlers.push_back({handler, m_token, priority, persistent, data, uuid_gen()});

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

}
