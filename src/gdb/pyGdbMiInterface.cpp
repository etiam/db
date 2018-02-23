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

    m_modulesDict = PyImport_GetModuleDict();

	importModule("gdbcontroller");

	m_gdbModule = createInstance("gdbcontroller", "GdbController");

	std::string cmd = "-file-exec-and-symbols /home/jasonr/workspace/db/build-dbg-amd64/db";
	auto value = PyObject_CallMethod(m_gdbModule, (char*)"write", (char*)"(s)", cmd.c_str());
	(void)value;
}

PyGdbMiInterface::~PyGdbMiInterface()
{
	Py_Finalize();
}

void
PyGdbMiInterface::importModule(const std::string &modulename)
{
    QFile file(":/pyc/gdbcontroller.pyc");
    if (file.open(QIODevice::ReadOnly))
    {
        QByteArray blob = file.readAll();
        std::cout << blob.length() << std::endl;

        PyObject *pCode = PyMarshal_ReadObjectFromString(blob.data()+8, blob.length()-8);
        PyImport_ExecCodeModule(const_cast<char*>(modulename.c_str()), pCode);
    }
}

PyObject *
PyGdbMiInterface::createInstance(const std::string &modulename,
                                 const std::string &classname)
{
    PyObject *instance = nullptr;

    auto module = PyDict_GetItemString(m_modulesDict, modulename.c_str());

    if (module)
    {
        auto dict = PyModule_GetDict(module);

        auto klass = PyDict_GetItemString(dict, classname.c_str());
        if (PyCallable_Check(klass))
        {
            instance = PyObject_CallFunction(klass, (char*)"(s)", "verbose=True");
        }
    }

    return instance;
}

void
PyGdbMiInterface::callClassFunction(const std::string &modulename,
                                    const std::string &classname,
                                    const std::string &functionname)
{
    static auto modulesDict = PyImport_GetModuleDict();

    auto module = PyDict_GetItemString(modulesDict, modulename.c_str());

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
