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

#include <QFile>
#include <Python.h>
#include <marshal.h>

#include "core/global.h"
#include "core/optionsManager.h"
#include "core/signal.h"

#include "gdb/responses.h"
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

    int             executeCommand(const std::string &command, Controller::ResponseFunc response, boost::any data);
    void            addResponse(Controller::ResponseFunc response);

    void            loadProgram(const std::string &filename);
    void            insertBreakpoint(const std::string &location);
    void            infoAddress(const std::string &function);

    bool            m_verbose = false;
    bool            m_initialized = false;
    bool            m_valid = true;
    bool            m_resultThreadActive = true;
    bool            m_resultThreadDone = false;

    int             m_token = 1;

    PyObject *      m_writeMethod = nullptr;
    PyObject *      m_getResponseMethod = nullptr;

    struct ResponseData
    {
        Controller::ResponseFunc    response;
        int                         token;
        boost::any                  data;
    };

    std::vector<ResponseData>       m_responses;

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
    m_verbose = Core::optionsManager()->has("verbose");

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
        // add module to global module dict
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
            m_getResponseMethod = PyObject_GetAttrString(gdbmiinstance, "get_gdb_response");

            if (!m_writeMethod || !PyCallable_Check(m_writeMethod) ||
                !m_getResponseMethod || !PyCallable_Check(m_getResponseMethod))
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
    auto it=std::begin(m_responses);
    for (; it != std::end(m_responses); ++it)
    {
        if (it->response(result, it->token, it->data))
        {
            match = true;
            break;
        }
    }

    if (match)
        m_responses.erase(it);
    else
        std::cout << "no match!" << std::endl;

    std::cout << m_responses.size() << std::endl;
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
        auto value = PyObject_Call(m_getResponseMethod, args, nullptr);

        if (value && PyList_Check(value))
        {
            auto n = PyList_Size(value);
            for (auto i=0; i < n; i++)
            {
                auto r = parseResult(PyList_GetItem(value, i));
                resultHandler(r);
            }
        }
        else
        {
            PyErr_Print();
        }

        // release python GIL
        PyGILState_Release(gstate);
    }

    m_resultThreadDone = true;
}

int
ControllerImpl::executeCommand(const std::string &command, Controller::ResponseFunc response, boost::any data)
{
    Result result;

    if (!m_initialized)
        initialize();

    std::stringstream stream;
    stream << m_token << "-" << command;

    // store response data
    if (response != nullptr)
        m_responses.push_back({response, m_token, data});

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
ControllerImpl::addResponse(Controller::ResponseFunc response)
{
    m_responses.push_back({response, m_token, true});
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
Controller::executeCommand(const std::string &command, ResponseFunc response, boost::any data)
{
    return m_impl->executeCommand(command, response, data);
}

void
Controller::addResponse(Controller::ResponseFunc response)
{
    m_impl->addResponse(response);
}

}
