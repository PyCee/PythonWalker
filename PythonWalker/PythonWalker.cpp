#include "PythonWalker.h"
#include <iostream>
#include <filesystem>
#include "PythonFileManagement.h"
#include <format>

#define strdup _strdup

PythonWalker::PythonWalker()
{
}

PythonWalker::PythonWalker(std::vector<std::string> additionalScriptsPaths)
{
    TearDown();

    PyConfig config{};
    config.pathconfig_warnings = 0;
    PyConfig_InitPythonConfig(&config);
    Py_Initialize();
    if (!Py_IsInitialized())
    {
        throw PythonModuleNotInitialized();
    }

    PyObject* sysPath = PySys_GetObject((char*)"path");
    for (std::string scriptsPath : additionalScriptsPaths) {
        if (scriptsPath.empty()) {
            continue;
        }

        // escape characters
        //scriptsPath.replace(scriptsPath.begin(), scriptsPath.end(), "\\\\", "/");
        //scriptsPath = std::quoted(scriptsPath);

        ScriptsPaths.push_back(scriptsPath);

        const char* path = scriptsPath.c_str();
        PyObject* programName = PyUnicode_FromString(path);
        PyList_Append(sysPath, programName);
        //Py_DECREF(programName);
    }
}

PythonWalker::~PythonWalker()
{
    TearDown();
}


void PythonWalker::TearDown()
{
    if (Py_IsInitialized())
    {
        Py_Finalize();
    }
}

std::vector<PythonClassDefinition> PythonWalker::GetScripts(bool recursive, std::string baseClassName,
    std::vector<std::string> excludeModules)
{
    std::vector<PythonClassDefinition> results;
    for (std::string scriptPath : ScriptsPaths) {
        std::vector<PythonClassDefinition> pathResults = PythonFileManagement::GetPythonClasses(scriptPath.c_str(), recursive);
        results.insert(results.end(), pathResults.begin(), pathResults.end());
    }
    if (!baseClassName.empty())
    {
        results = PythonClassDefinition::FilterToDerivingFromClass(baseClassName, results);
    }
    if (excludeModules.size() > 0) {
        for (std::string excludeModule : excludeModules) {
            results = PythonClassDefinition::FilterOutModule(excludeModule, results);
        }
    }

    return results;
}

std::vector<std::string> PythonWalker::ParsePythonModuleString(std::string module)
{
    std::vector<std::string> results;
    std::string delimiter = ".";
    size_t pos = 0;
    std::string token;
    while ((pos = module.find(delimiter)) != std::string::npos) {
        token = module.substr(0, pos);
        results.push_back(token);
        module.erase(0, pos + delimiter.length());
    }
    results.push_back(module);
    return results;
}

PyObject* PythonWalker::LoadModule(std::string moduleName)
{
    PyObject* pModule = PyImport_ImportModule(moduleName.c_str());
    if (pModule == nullptr) {
        PyErr_Print();
        throw PythonModuleDNE(moduleName);
    }

    return pModule;
}
PyObject* PythonWalker::LoadModule(std::vector<std::string> moduleNames)
{
    std::string resultingName;
    for (std::string moduleName : moduleNames) {
        if (!resultingName.empty()) {
            resultingName += ".";
        }
        resultingName += moduleName;
    }

    return LoadModule(resultingName);
}
PyObject* PythonWalker::LoadModuleDict(PyObject* pModule)
{
    PyObject* dict = PyModule_GetDict(pModule);
    if (dict == nullptr) {
        PyErr_Print();
        throw PythonModuleDictionaryFailed();
    }
    return dict;
}

PyObject* PythonWalker::GetPythonClass(const char* moduleName, const char* className)
{
    PyObject* pModule = PythonWalker::LoadModule(moduleName);
    return PythonWalker::GetPythonClass(pModule, className);
}
PyObject* PythonWalker::GetPythonClass(PyObject* pModule, const char* className)
{
    if (pModule == nullptr) {
        throw PythonModuleNotInitialized();
    }

    PyObject* dict = LoadModuleDict(pModule);

    // Builds the name of a callable class
    PyObject* pythonClass = PyDict_GetItemString(dict, className);
    if (pythonClass == nullptr) {
        PyErr_Print();
        throw PythonObjectDNE(className);
    }
    //Py_DECREF(dict);
    return pythonClass;
}

// https://stackoverflow.com/questions/25314774/python-c-api-iterate-through-classes-in-module
// https://stackoverflow.com/questions/58075398/how-to-list-all-function-names-of-a-python-module-in-c
// https://stackoverflow.com/questions/5520580/how-do-you-get-all-classes-defined-in-a-module-but-not-imported
std::vector<PythonClassDefinition> PythonWalker::GetClassDefinitionsFromModuleDict(const char* moduleName)
{
    std::vector<PythonClassDefinition> results;
    // At this point we have a python module, find classes within it
    PyObject* module = PythonWalker::LoadModule(moduleName);
    PyObject* dict = PyModule_GetDict(module);
    PyObject* pKey, * value = NULL;
    Py_ssize_t pos = 0;

    while (PyDict_Next(dict, &pos, &pKey, &value)) {
        const char* className = PyUnicode_AsUTF8(pKey);
        if (PyType_Check(value)) {

            // Make sure this class is actually from the working module, not just imported from another
            if (!PythonWalker::MatchesModule(value, moduleName)) {
                continue;
            }
            const char* className = PyUnicode_AsUTF8(pKey);
            results.push_back(PythonClassDefinition(moduleName, className));
        }
    }
    //Py_DECREF(dict);
    //Py_DECREF(module);
    return results;
}

bool PythonWalker::MatchesModule(PyObject* pObject, const char* moduleName)
{
    PyObject* moduleAttribute = PyObject_GetAttrString(pObject, "__module__");
    const char* tempModuleAttribute = PyUnicode_AsUTF8(moduleAttribute);
    return strcmp(moduleName, tempModuleAttribute) == 0;
}

PyObject* PythonWalker::CreateObject(PyObject* pModule, const char* className)
{
    PyObject* pythonClass = GetPythonClass(pModule, className);

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