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
#include <QFile>
#include <Python.h>
#include <marshal.h>

#include "pyGdbMiInterface.h"

class PyGdbMiInterfaceImpl
{
  public:
    PyGdbMiInterfaceImpl(const std::string &filename);
    ~PyGdbMiInterfaceImpl();

    PyGdbMiResult   executeCommand(const std::string &command);

    PyObject *      importModule(const std::string &bytecodename, const std::string &modulename);
    PyObject *      createInstance(const std::string &modulename, const std::string &classname);
    PyObject *      callFunction(PyObject *module, const std::string &functionname, PyObject *args);

    PyGdbMiResult   parseResult(PyObject *object);
    Message         parseMessage(PyObject *object);
    Payload         parsePayload(PyObject *object);
    Stream          parseStream(PyObject *object);
    Token           parseToken(PyObject *object);
    Type            parseType(PyObject *object);
    boost::any      parseObject(PyObject *object);

    bool            m_valid = true;

    int             m_token = 1;

    PyObject *      m_moduleDict = nullptr;

    PyObject *      m_gdbMiModule = nullptr;
    PyObject *      m_gdbMiParserModule = nullptr;
    PyObject *      m_gdbControllerModule = nullptr;

    PyObject *      m_gdbMiInstance = nullptr;
};

PyGdbMiInterfaceImpl::PyGdbMiInterfaceImpl(const std::string &filename)
{
    Py_Initialize();

    m_moduleDict = PyImport_GetModuleDict();

//    auto testmodule = importModule(":/pyc/parsetest.pyc", "parsetest");
//    for (auto n=0; n < 32; ++n)
//    {
//        auto result = callFunction(testmodule, "getn", Py_BuildValue("(i)", n));
//        if (result)
//        {
//            auto r = parseResult(result);
//            std::cout << r << std::endl;
//        }
//    }

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
    }
    else
        m_valid = false;

    if (!m_valid)
        std::cerr << "PyGdbMiInterface::PyGdbMiInterface(): failed to initialize class" << std::endl;
}

PyGdbMiInterfaceImpl::~PyGdbMiInterfaceImpl()
{
    Py_Finalize();
}

PyGdbMiResult
PyGdbMiInterfaceImpl::executeCommand(const std::string &command)
{
    PyGdbMiResult result;
    std::stringstream stream;
    stream << m_token << "-" << command;

    auto value = PyObject_CallMethod(m_gdbMiInstance, (char*)"write", (char*)"(s)", stream.str().c_str());

    auto n = PyList_Size(value);

    for (auto i=0; i < n; i++)
    {
        auto r = parseResult(PyList_GetItem(value, i));
        if (r.token.value == m_token)
        {
            result = r;
            m_token++;
        }
//        else
//        {
//            m_outofband.push_back(r);
//        }
    }

    return result;
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
            dict = Py_BuildValue("{s:p}", "verbose", true);
            instance = PyObject_Call(klass, NULL, dict);

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

PyObject *
PyGdbMiInterfaceImpl::callFunction(PyObject *module, const std::string &functionname, PyObject *args)
{
    PyObject *result = nullptr;

    auto dict = PyModule_GetDict(module);
    auto function = PyDict_GetItemString(dict, functionname.c_str());
    if (function && PyCallable_Check(function))
    {
        result = PyObject_CallObject(function, args);
    }
    else
    {
        PyErr_Print();
    }

    return result;
}

PyGdbMiResult
PyGdbMiInterfaceImpl::parseResult(PyObject *object)
{
    // must be a dictionary type
    if (!PyDict_Check(object))
    {
        std::stringstream errmsg;
        errmsg << "ParseMessage(): invalid data : " << PyString_AsString(PyObject_Str(object));
        throw std::runtime_error(errmsg.str().c_str());
    }

    PyGdbMiResult result;

    auto list = PyDict_Keys(object);
    auto n = PyList_Size(list);
    std::map<std::string, boost::any> map;

    for (auto i=0; i < n; i++)
    {
        auto key = std::string(PyString_AsString(PyList_GetItem(list, i)));
        auto val = PyDict_GetItem(object, PyList_GetItem(list, i));

        if (key == "message")
        {
            result.message = parseMessage(val);
        }
        else if (key == "payload")
        {
            result.payload = parsePayload(val);
        }
        else if (key == "stream")
        {
            result.stream = parseStream(val);
        }
        else if (key == "token")
        {
            result.token = parseToken(val);
        }
        else if (key == "type")
        {
            result.type = parseType(val);
        }
        else
        {
            std::cerr << "ParseMessage(): unknown key : " << key << std::endl;
        }
    }

    return result;
}

Message
PyGdbMiInterfaceImpl::parseMessage(PyObject *object)
{
    Message message;

    if (object == Py_None)
    {
        message.type = Message::Type::NONE;
    }
    else if (PyString_Check(object) || PyUnicode_Check(object))
    {
        message.type = Message::Type::STRING;
        message.string.string = PyString_AsString(object);
    }

    return message;
}

Payload
PyGdbMiInterfaceImpl::parsePayload(PyObject* object)
{
    Payload payload;

    if (object == Py_None)
    {
        payload.type = Payload::Type::NONE;
    }
    else if (PyString_Check(object) || PyUnicode_Check(object))
    {
        payload.type = Payload::Type::STRING;
        payload.string.string = PyString_AsString(object);
    }
    else if (PyDict_Check(object))
    {
        payload.type = Payload::Type::DICT;
        payload.dict = boost::any_cast<Payload::Dict>(parseObject(object));
    }

    return payload;
}

Stream
PyGdbMiInterfaceImpl::parseStream(PyObject* object)
{
    Stream stream = Stream::NONE;

    if (PyString_Check(object) || PyUnicode_Check(object))
    {
        auto string = std::string(PyString_AsString(object));
        if (string == "stdout")
            stream = Stream::STDOUT;
        else
            std::cerr << "parseStream(): unknown value : " << PyString_AsString(PyObject_Str(object)) << std::endl;
    }

    return stream;
}

Token
PyGdbMiInterfaceImpl::parseToken(PyObject* object)
{
    Token token;

    if (object == Py_None)
    {
        token.value = -1;
    }
    else if (PyInt_Check(object))
    {
        token.value = static_cast<int>(_PyInt_AsInt(object));
    }
    else
    {
        std::cerr << "parseToken(): unknown value : " << PyString_AsString(PyObject_Str(object)) << std::endl;
    }


    return token;
}

Type
PyGdbMiInterfaceImpl::parseType(PyObject* object)
{
    Type type = Type::NONE;

    if (PyString_Check(object) || PyUnicode_Check(object))
    {
        auto string = std::string(PyString_AsString(object));
        if (string == "result")
            type = Type::RESULT;
        else if (string == "notify")
            type = Type::NOTIFY;
        else if (string == "output")
            type = Type::OUTPUT;
        else if (string == "console")
            type = Type::CONSOLE;
        else
            std::cerr << "parseStream(): unknown value : " << PyString_AsString(PyObject_Str(object)) << std::endl;
    }

    return type;
}

boost::any
PyGdbMiInterfaceImpl::parseObject(PyObject *object)
{
    if (PyInt_Check(object))
        return _PyInt_AsInt(object);

    if (PyFloat_Check(object))
        return PyFloat_AsDouble(object);

    if (PyString_Check(object) || PyUnicode_Check(object))
        return PyString_AsString(object);

    if (PyList_Check(object))
    {
        auto n = PyList_Size(object);
        Payload::List list(n);

        for (auto i=0; i < n; i++)
        {
            list[i] = parseObject(PyList_GetItem(object, i));
        }

        return list;
    }

    if (PyDict_Check(object))
    {
        auto list = PyDict_Keys(object);
        auto n = PyList_Size(list);
        std::map<std::string, boost::any> map;

        for (auto i=0; i < n; i++)
        {
            auto key = PyList_GetItem(list, i);
            auto val = PyDict_GetItem(object, key);
            map[PyString_AsString(key)] = parseObject(val);
        }

        return map;
    }

    if (object == Py_None)
        return nullptr;

    std::stringstream errmsg;
    errmsg << "ParseObject(): un-handled type " << PyString_AsString(PyObject_Str(object));
    throw std::runtime_error(errmsg.str().c_str());
}

PyGdbMiInterface::PyGdbMiInterface(const std::string &filename) :
    m_impl(std::make_unique<PyGdbMiInterfaceImpl>(filename))
{
}

PyGdbMiInterface::~PyGdbMiInterface()
{
}

PyGdbMiResult
PyGdbMiInterface::executeCommand(const std::string &command)
{
    return m_impl->executeCommand(command);
}
