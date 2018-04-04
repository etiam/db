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
#include <regex>
#include <QFile>
#include <boost/format.hpp>
#include <Python.h>
#include <marshal.h>

#include "pyGdbMiInterface.h"

namespace
{

#pragma GCC diagnostic push
#pragma GCC diagnostic ignored "-Wunused-function"
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
#pragma GCC diagnostic pop

}

template<typename Iterable>
class enumerate_object
{
    private:
        Iterable _iter;
        std::size_t _size;
        decltype(std::begin(_iter)) _begin;
        const decltype(std::end(_iter)) _end;

    public:
        enumerate_object(Iterable iter):
            _iter(iter),
            _size(0),
            _begin(std::begin(iter)),
            _end(std::end(iter))
        {}

        const enumerate_object& begin() const { return *this; }
        const enumerate_object& end()   const { return *this; }

        bool operator!=(const enumerate_object&) const
        {
            return _begin != _end;
        }

        void operator++()
        {
            ++_begin;
            ++_size;
        }

        auto operator*() const
            -> std::pair<std::size_t, decltype(*_begin)>
        {
            return { _size, *_begin };
        }
};

template<typename Iterable>
auto enumerate(Iterable&& iter)
    -> enumerate_object<Iterable>
{
    return { std::forward<Iterable>(iter) };
}

std::ostream & operator <<(std::ostream &out, Payload::Dict dict);
std::ostream & operator <<(std::ostream &out, Payload::List dict);

std::ostream &
operator <<(std::ostream &out, String string)
{
    out << "'" << std::regex_replace(string.string, std::regex("(\n)"),"\\n") << "'";
    return out;
}

std::ostream &
operator <<(std::ostream &out, Message message)
{
    switch (message.type)
    {
    case Message::Type::STRING:
        out << message.string;
        break;

    case Message::Type::NONE:
        out << "None";
        break;
    }

    return out;
}

std::ostream &
operator <<(std::ostream &out, Payload::List list)
{
    out << "[";
    for (auto&& a : enumerate(list))
    {
        auto index = std::get<0>(a);
        auto item = std::get<1>(a);

        try
        {
            out << "'" << boost::any_cast<char *>(item) << "'";
        }
        catch (boost::bad_any_cast &)
        {
        }

        try
        {
            out << boost::any_cast<Payload::Dict>(item);
        }
        catch (boost::bad_any_cast &)
        {
        }

        try
        {
            out << boost::any_cast<Payload::List>(item);
        }
        catch (boost::bad_any_cast &)
        {
        }

        if (index < list.size()-1)
            out << ", ";
    }
    out << "]";

    return out;
}

std::ostream &
operator <<(std::ostream &out, Payload::Dict dict)
{
    out << "{";
    for (auto&& a : enumerate(dict))
    {
        auto index = std::get<0>(a);
        auto item = std::get<1>(a);

        out << "'" << item.first << "': ";

        try
        {
            out << "'" << boost::any_cast<char *>(item.second) << "'";
        }
        catch (boost::bad_any_cast &)
        {
        }

        try
        {
            out << boost::any_cast<Payload::Dict>(item.second);
        }
        catch (boost::bad_any_cast &)
        {
        }

        try
        {
            out << boost::any_cast<Payload::List>(item.second);
        }
        catch (boost::bad_any_cast &)
        {
        }

        if (index < dict.size()-1)
            out << ", ";
    }
    out << "}";
    return out;
}

std::ostream &
operator <<(std::ostream &out, Payload payload)
{
    switch (payload.type)
    {
    case Payload::Type::STRING:
        out << payload.string;
        break;

    case Payload::Type::DICT:
        out << payload.dict;
        break;

    case Payload::Type::NONE:
        out << "None";
        break;
    }

    return out;
}

std::ostream &
operator <<(std::ostream &out, Stream stream)
{
    switch (stream)
    {
    case Stream::STDOUT:
        out << "'stdout'";
        break;

    case Stream::NONE:
        out << "None";
        break;

    default:
        out << "unknown";
        break;
    }
    return out;
}

std::ostream &
operator <<(std::ostream &out, Token token)
{
    switch (token)
    {
    case Token::NONE:
        out << "None";
        break;

    default:
        out << "unknown";
        break;
    }
    return out;
}

std::ostream &
operator <<(std::ostream &out, Type type)
{
    switch (type)
    {
    case Type::RESULT:
        out << "'result'";
        break;

    case Type::NOTIFY:
        out << "'notify'";
        break;

    case Type::OUTPUT:
        out << "'output'";
        break;

    case Type::CONSOLE:
        out << "'console'";
        break;

    case Type::NONE:
        out << "None";
        break;

    default:
        out << "unknown";
        break;
    }
    return out;
}

std::ostream &
operator<<(std::ostream &stream, const GdbMiResult &result)
{
    stream << "{'token': "   << result.token << ", "
           <<  "'message': " << result.message << ", "
           <<  "'type': "    << result.type << ", "
           <<  "'payload': " << result.payload << ", "
           <<  "'stream': "  << result.stream << "}";
    return stream;
}

PyGdbMiInterface::PyGdbMiInterface(const std::string &filename)
{
    Py_Initialize();

    m_moduleDict = PyImport_GetModuleDict();

    auto testmodule = importModule(":/pyc/parsetest.pyc", "parsetest");
    for (auto n=0; n < 32; ++n)
    {
        auto result = callFunction(testmodule, "getn", Py_BuildValue("(i)", n));
        if (result)
        {
            auto r = parseResult(result);
            std::cout << r << std::endl;
        }
    }

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

PyGdbMiInterface::~PyGdbMiInterface()
{
    Py_Finalize();
}

void
PyGdbMiInterface::executeCommand(const std::string &command)
{
    auto value = PyObject_CallMethod(m_gdbMiInstance, (char*)"write", (char*)"(s)", command.c_str());

    Py_ssize_t n = PyList_Size(value);
    (void)n;
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
PyGdbMiInterface::callFunction(PyObject *module, const std::string &functionname, PyObject *args)
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

GdbMiResult
PyGdbMiInterface::parseResult(PyObject *object)
{
    // must be a dictionary type
    if (!PyDict_Check(object))
    {
        std::stringstream errmsg;
        errmsg << "ParseMessage(): invalid data : " << PyString_AsString(PyObject_Str(object));
        throw std::runtime_error(errmsg.str().c_str());
    }

    GdbMiResult result;

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
PyGdbMiInterface::parseMessage(PyObject *object)
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
PyGdbMiInterface::parsePayload(PyObject* object)
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
PyGdbMiInterface::parseStream(PyObject* object)
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
PyGdbMiInterface::parseToken(PyObject* object)
{
    Token token = Token::NONE;

    if (object != Py_None)
    {
        std::cerr << "parseToken(): unknown value : " << PyString_AsString(PyObject_Str(object)) << std::endl;
    }

    return token;
}

Type
PyGdbMiInterface::parseType(PyObject* object)
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
PyGdbMiInterface::parseObject(PyObject *object)
{
    if (PyLong_Check(object))
        return PyLong_AsLong(object);

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
