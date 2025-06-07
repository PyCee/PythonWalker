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
}
void PythonWalker::Destroy() {
    if (Py_IsInitialized())
    {
        Py_Finalize();
    }
}
std::optional<PyObject*> PythonWalker::CreateObject(PyObject* pModule, const char* className)
{
    std::optional<PyObject*> pythonClass = Module::GetPythonClass(pModule, className);

    if (!pythonClass) {
        return std::nullopt;
    }

    // Creates an instance of the class
    PyObject* pythonClassInstance;
    if (PyCallable_Check(pythonClass.value())) {
        pythonClassInstance = PyObject_CallObject(pythonClass.value(), nullptr);
        //Py_DECREF(pythonClass);
    }
    else {
        std::cout << "Cannot instantiate the Python class" << std::endl;
        //Py_DECREF(pythonClass);
        return std::nullopt;
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


std::optional<PyObject*> PythonWalker::ExecuteFunction(PyObject* pythonObject, const char* functionName, PyObject* keywords)
{
    if (pythonObject == nullptr) {
        return nullptr;
    }
    PyObject* method = PyObject_GetAttrString(pythonObject, functionName);
    if (method == nullptr) {
        PyErr_Print();
        return std::nullopt;
    }

    PyObject* args = PyTuple_New(0);

    PyObject* value = PyObject_Call(method, args, keywords);
    if (value == nullptr) {
        PyErr_Print();
        return std::nullopt;
    }

    //Py_DECREF(args);
    //Py_DECREF(method);

    return value;
}


std::optional<PyObject*> PythonWalker::GetPyObjectDataContainer(std::string moduleName) {
    return PythonWalker::Module::Load(moduleName);
}
std::optional<PyObject*> PythonWalker::GetPyObjectDataContainer(PyObject* pyObject) {
    return pyObject;
}