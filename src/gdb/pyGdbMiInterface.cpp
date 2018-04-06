/*
 * PyGdbInterface.cpp
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
#include <QFile>
#include <Python.h>
#include <marshal.h>

#include "pyGdbMiInterface.h"

class PyGdbMiInterfaceImpl
{
  public:
    PyGdbMiInterfaceImpl(bool verbose);
    ~PyGdbMiInterfaceImpl();

    int             executeCommand(const std::string &command);

    PyObject *      importModule(const std::string &bytecodename, const std::string &modulename);
    PyObject *      createInstance(const std::string &modulename, const std::string &classname);

    void            resultReceived(PyGdbMiResult result);

    void            resultReader();

    bool            m_verbose;
    bool            m_valid = true;
    bool            m_resultThreadActive = true;
    bool            m_resultThreadDone = false;

    int             m_token = 1;

    PyObject *      m_moduleDict = nullptr;

    PyObject *      m_gdbMiModule = nullptr;
    PyObject *      m_gdbMiParserModule = nullptr;
    PyObject *      m_gdbControllerModule = nullptr;

    PyObject *      m_gdbMiInstance = nullptr;

    PyObject *      m_writeMethod = nullptr;
    PyObject *      m_getResponseMethod = nullptr;

    PyThreadState * m_threadState = nullptr;

    std::unique_ptr<std::thread>    m_readerThread;
};

PyGdbMiInterfaceImpl::PyGdbMiInterfaceImpl(bool verbose) :
    m_verbose(verbose)
{
    Py_Initialize();
    if (!PyEval_ThreadsInitialized())
    {
        PyEval_InitThreads();
    }

    m_moduleDict = PyImport_GetModuleDict();

    // create blank module named 'pygdbmi'
    m_gdbMiModule = PyModule_New("pygdbmi");
    if (m_gdbMiModule && PyModule_Check(m_gdbMiModule))
    {
        // add module to global module dict
        PyDict_SetItemString(m_moduleDict, "pygdbmi", m_gdbMiModule);

        // import pygdbmi submodules
        auto printcolormodule = importModule(":/pyc/printcolor.pyc", "pygdbmi.printcolor");
        if (printcolormodule && PyModule_Check(printcolormodule))
            PyModule_AddObject(m_gdbMiModule, "printcolor", printcolormodule);
        else
            m_valid = false;

        auto stringstreammodule = importModule(":/pyc/StringStream.pyc", "pygdbmi.StringStream");
        if (stringstreammodule && PyModule_Check(stringstreammodule))
            PyModule_AddObject(m_gdbMiModule, "StringStream", stringstreammodule);
        else
            m_valid = false;

        m_gdbMiParserModule = importModule(":/pyc/gdbmiparser.pyc", "pygdbmi.gdbmiparser");
        if (m_gdbMiParserModule && PyModule_Check(m_gdbMiParserModule))
            PyModule_AddObject(m_gdbMiModule, "gdbmiparser", m_gdbMiParserModule);
        else
            m_valid = false;

        m_gdbControllerModule = importModule(":/pyc/gdbcontroller.pyc", "pygdbmi.gdbcontroller");
        if (m_gdbControllerModule && PyModule_Check(m_gdbControllerModule))
            PyModule_AddObject(m_gdbMiModule, "gdbcontroller", m_gdbMiParserModule);
        else
            m_valid = false;

        m_gdbMiInstance = createInstance("pygdbmi.gdbcontroller", "GdbController");

        m_writeMethod = PyObject_GetAttrString(m_gdbMiInstance, "write");
        m_getResponseMethod = PyObject_GetAttrString(m_gdbMiInstance, "get_gdb_response");

        if (!m_writeMethod || !PyCallable_Check(m_writeMethod) ||
            !m_getResponseMethod || !PyCallable_Check(m_getResponseMethod))
            m_valid = false;

        // start read thread
        m_readerThread = std::make_unique<std::thread>(&PyGdbMiInterfaceImpl::resultReader, std::ref(*this));
    }
    else
        m_valid = false;

    PyEval_SaveThread();

    if (!m_valid)
        std::cerr << "PyGdbMiInterface::PyGdbMiInterface(): failed to initialize class" << std::endl;
}

PyGdbMiInterfaceImpl::~PyGdbMiInterfaceImpl()
{
    // stops result thread loop
    m_resultThreadActive = false;

    // waits for result thread loop to stop
    while(!m_resultThreadDone);

    // waits for result thread to finish
    m_readerThread->join();

//    Py_Finalize();
}

int
PyGdbMiInterfaceImpl::executeCommand(const std::string &command)
{
    PyGdbMiResult result;
    std::stringstream stream;
    stream << m_token << "-" << command;

    // acquire python GIL
    auto gstate = PyGILState_Ensure();

    auto args = Py_BuildValue("(s)", stream.str().c_str());
    auto kw = Py_BuildValue("{s:i,s:i}", "verbose", m_verbose, "read_response", false);
    auto value = PyObject_Call(m_writeMethod, args, kw);

    if (value)
    {
        auto n = PyList_Size(value);
        std::cout << "got " << n << " results in executeCommand()" << std::endl;
    }
    else
    {
        PyErr_Print();
    }

    // release python GIL
    PyGILState_Release(gstate);

    return m_token++;
}

PyObject *
PyGdbMiInterfaceImpl::importModule(const std::string &bytecodename, const std::string &modulename)
{
    PyObject *module = nullptr;
    QFile file(QString::fromStdString(bytecodename));

    if (file.open(QIODevice::ReadOnly))
    {
        QByteArray blob = file.readAll();

        PyObject *code = PyMarshal_ReadObjectFromString(blob.data()+8, blob.length()-8);
        module = PyImport_ExecCodeModule(const_cast<char*>(modulename.c_str()), code);
    }
    else
        std::cerr << "PyGdbMiInterface::importModule(): could not open \"" << bytecodename << "\"" << std::endl;

    return module;
}

PyObject *
PyGdbMiInterfaceImpl::createInstance(const std::string &modulename, const std::string &classname)
{
    PyObject *instance = nullptr;

    auto module = PyDict_GetItemString(m_moduleDict, modulename.c_str());
    if (module)
    {
        auto dict = PyModule_GetDict(module);
        auto klass = PyDict_GetItemString(dict, classname.c_str());
        if (PyCallable_Check(klass))
        {
            auto args = Py_BuildValue("{s:i}", "verbose", false);
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
PyGdbMiInterfaceImpl::resultReceived(PyGdbMiResult result)
{
    std::cout << result << std::endl;
}

void
PyGdbMiInterfaceImpl::resultReader()
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
                resultReceived(r);
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

///////////////////////////////////

PyGdbMiInterface::PyGdbMiInterface(bool verbose) :
    m_impl(std::make_unique<PyGdbMiInterfaceImpl>(verbose))
{
}

PyGdbMiInterface::~PyGdbMiInterface()
{
}

int
PyGdbMiInterface::executeCommand(const std::string &command)
{
    return m_impl->executeCommand(command);
}
