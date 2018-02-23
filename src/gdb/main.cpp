// clang++ `pkg-config --cflags python` `pkg-config --libs python` main.cpp -g3 -O0 -Wall

#include <stdio.h>
#include <Python.h>
#include <marshal.h>

size_t
load_file(const char *filename, char **buffer)
{
    size_t size = 0;

    FILE *fp = fopen(filename, "rb");
    fseek(fp, 0, SEEK_END);
    long flen = ftell(fp);
    fseek(fp, 0, SEEK_SET);

    *buffer = new char[flen];

    size = fread(*buffer, sizeof(char), flen, fp);

    return size;
}

void
import_module(const char *filename, char *modulename)
{
	size_t size;
    char *pByteCode;

	if((size = load_file(filename, &pByteCode)) > 0)
    {
        PyObject *pCode = PyMarshal_ReadObjectFromString(pByteCode+8, size-8);
        PyImport_ExecCodeModule(modulename, pCode);
    }

    delete [] pByteCode;
}

void
call_func(const char *modulename, char *funcname)
{
    PyObject *pModule, *pFunc, *pModulesDict, *pDict, *pInstance, *pValue;

    pModulesDict = PyImport_GetModuleDict();
    pModule = PyDict_GetItemString(pModulesDict, modulename);

    if (pModule)
    {
        pDict = PyModule_GetDict(pModule);

        pFunc = PyDict_GetItemString(pDict, funcname);
        if (PyCallable_Check(pFunc))
        {
            pValue = PyObject_CallFunction(pFunc, "");
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

void
call_class_func(const char *modulename, const char *classname, char *funcname)
{
    PyObject *pModule, *pClass, *pModulesDict, *pDict, *pInstance, *pValue;

    pModulesDict = PyImport_GetModuleDict();
    pModule = PyDict_GetItemString(pModulesDict, modulename);

    if (pModule)
    {
        pDict = PyModule_GetDict(pModule);

        pClass = PyDict_GetItemString(pDict, classname);
        if (PyCallable_Check(pClass))
        {
            pInstance = PyObject_CallObject(pClass, NULL);
            pValue = PyObject_CallMethod(pInstance, funcname, "(ii)", 12, 24);
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

int
main(int, char **)
{
	Py_Initialize();

    import_module("gdbcontroller.pyc", "gdbcontroller");
    import_module("example.pyc", "example");
    call_func("example", "main");

//    import_module("multiply.pyc", "multiply");
//    call_class_func("multiply", "Multiply", "multiply2");

	Py_Finalize();

    return 0;
}
