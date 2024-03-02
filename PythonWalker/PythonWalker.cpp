#include "PythonWalker.h"
#include <iostream>
#include <filesystem>
#include <format>

#define strdup _strdup

void PythonWalker::Initialize() {
    PyConfig config{};
    config.pathconfig_warnings = 0;
    PyConfig_InitPythonConfig(&config);
    Py_Initialize();
    if (!Py_IsInitialized())
    {
        throw PythonModuleNotInitialized();
    }
}
void PythonWalker::Destroy() {
    if (Py_IsInitialized())
    {
        Py_Finalize();
    }
}
PyObject* PythonWalker::CreateObject(PyObject* pModule, const char* className)
{
    PyObject* pythonClass = Module::GetPythonClass(pModule, className);

    // Creates an instance of the class
    PyObject* pythonClassInstance;
    if (PyCallable_Check(pythonClass)) {
        pythonClassInstance = PyObject_CallObject(pythonClass, nullptr);
        //Py_DECREF(pythonClass);
    }
    else {
        std::cout << "Cannot instantiate the Python class" << std::endl;
        //Py_DECREF(pythonClass);
        return nullptr;
    }
    return pythonClassInstance;
}

void PythonWalker::CopyPyObjectValues(PyObject* src, PyObject* dest)
{
    PyObject* dict = PyObject_GetAttrString(src, "__dict__");
    PyObject* pKey, * value = NULL;
    Py_ssize_t pos = 0;

    while (PyDict_Next(dict, &pos, &pKey, &value)) {
        const char* name = GetValueFromPyObject<const char*>(pKey);
        float val = GetValueFromPyObject<float>(value);
        PyObject_SetAttr(dest, pKey, value);
        //TODO does this handle complex objects?
    }
}


PyObject* PythonWalker::ExecuteFunction(PyObject* pythonObject, const char* functionName, PyObject* keywords)
{
    if (pythonObject == nullptr) {
        return nullptr;
    }
    PyObject* method = PyObject_GetAttrString(pythonObject, functionName);
    if (method == nullptr) {
        PyErr_Print();
        throw PythonMethodDNE(functionName, "", "");
    }

    PyObject* args = PyTuple_New(0);

    PyObject* value = PyObject_Call(method, args, keywords);
    if (value == nullptr) {
        PyErr_Print();
        throw PythonMethodError(functionName, "", "", "");
    }

    //Py_DECREF(args);
    //Py_DECREF(method);

    return value;
}


PyObject* PythonWalker::GetPyObjectDataContainer(std::string moduleName) {
    return PythonWalker::Module::Load(moduleName);
}
PyObject* PythonWalker::GetPyObjectDataContainer(PyObject* pyObject) {
    return pyObject;
}