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
#include <QFile>
#include <Python.h>
#include <marshal.h>

#include "pyGdbMiInterface.h"

PyGdbMiInterface::PyGdbMiInterface()
{
    Py_Initialize();

    m_moduleDict = PyImport_GetModuleDict();

    // create blank module named 'pygdbmi'
    m_gdbmiModule = PyModule_New("pygdbmi");
    if (m_gdbmiModule && PyModule_Check(m_gdbmiModule))
    {
        // add module to global module dict
        PyDict_SetItemString(m_moduleDict, "pygdbmi", m_gdbmiModule);

//        PyRun_SimpleString("import sys\nprint dir(sys.modules['pygdbmi'])");
//        PyRun_SimpleString("import sys\nprint sys.modules.keys()");

        // import pygdbmi submodules
        auto printcolormodule = importModule(":/pyc/printcolor.pyc", "pygdbmi.printcolor");
        if (printcolormodule && PyModule_Check(printcolormodule))
            PyModule_AddObject(m_gdbmiModule, "printcolor", printcolormodule);
        else
            m_valid = false;

        auto stringstreammodule = importModule(":/pyc/StringStream.pyc", "pygdbmi.StringStream");
        if (stringstreammodule && PyModule_Check(stringstreammodule))
            PyModule_AddObject(m_gdbmiModule, "StringStream", stringstreammodule);
        else
            m_valid = false;

        m_gdbmiParserModule = importModule(":/pyc/gdbmiparser.pyc", "pygdbmi.gdbmiparser");
        if (m_gdbmiParserModule && PyModule_Check(m_gdbmiParserModule))
            PyModule_AddObject(m_gdbmiModule, "gdbmiparser", m_gdbmiParserModule);
        else
            m_valid = false;

        m_gdbControllerModule = importModule(":/pyc/gdbcontroller.pyc", "pygdbmi.gdbcontroller");
        if (m_gdbControllerModule && PyModule_Check(m_gdbControllerModule))
            PyModule_AddObject(m_gdbmiModule, "gdbcontroller", m_gdbmiParserModule);
        else
            m_valid = false;

        m_gdbmiInstance = createInstance("pygdbmi.gdbcontroller", "GdbController");
        std::string cmd = "-file-exec-and-symbols /home/jasonr/workspace/db/build-dbg-amd64/db";
        auto value = PyObject_CallMethod(m_gdbmiInstance, (char*)"write", (char*)"(s)", cmd.c_str());
        (void)value;
    }
    else
        m_valid = false;

    if (!m_valid)
        std::cerr << "PyGdbMiInterface::PyGdbMiInterface(): failed to initialize class" << std::endl;
}

PyGdbMiInterface::~PyGdbMiInterface()
{
    Py_Finalize();
}

PyObject *
PyGdbMiInterface::importModule(const std::string &bytecodename, const std::string &modulename)
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

void
dumpDict(PyObject *dict)
{
    PyObject *list = PyDict_Keys(dict);
    Py_ssize_t n = PyList_Size(list);

    for (Py_ssize_t i = 0; i < n; i++)
    {
        PyObject *obj = PyList_GetItem(list, i);
        if (PyString_Check(obj))
        {
            std::cout << PyString_AsString(obj) << std::endl;
        }
    }
}

PyObject *
PyGdbMiInterface::createInstance(const std::string &modulename, const std::string &classname)
{
    PyObject *instance = nullptr;

    auto module = PyDict_GetItemString(m_moduleDict, modulename.c_str());
    if (module)
    {
        auto dict = PyModule_GetDict(module);
        auto klass = PyDict_GetItemString(dict, classname.c_str());
        if (PyCallable_Check(klass))
        {
            instance = PyObject_CallFunction(klass, NULL);
//            instance = PyObject_CallFunction(klass, (char*)"(s)", "verbose=True");
        }
    }

    return instance;
}

void
PyGdbMiInterface::callClassFunction(const std::string &modulename,
                                    const std::string &classname,
                                    const std::string &functionname)
{
    auto module = PyDict_GetItemString(m_moduleDict, modulename.c_str());

    if (module)
    {
        auto dict = PyModule_GetDict(module);

        auto klass = PyDict_GetItemString(dict, classname.c_str());
        if (PyCallable_Check(klass))
        {
            auto pInstance = PyObject_CallObject(klass, NULL);
            auto pValue = PyObject_CallMethod(pInstance, const_cast<char*>(functionname.c_str()), (char*)"");
            (void)pValue;
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
}
