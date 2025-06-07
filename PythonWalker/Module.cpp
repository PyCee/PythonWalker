#include "Module.h"
#include "Exceptions.h"

std::vector<std::string> PythonWalker::Module::ParsePythonModuleString(std::string module)
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

PyObject* PythonWalker::Module::Load(std::string moduleName)
{
    PyObject* pModule = PyImport_ImportModule(moduleName.c_str());
    if (pModule == nullptr) {
        PyErr_Print();
        throw PythonModuleDNE(moduleName);
    }

    return pModule;
}
PyObject* PythonWalker::Module::Load(std::vector<std::string> moduleNames)
{
    std::string resultingName;
    for (std::string moduleName : moduleNames) {
        if (!resultingName.empty()) {
            resultingName += ".";
        }
        resultingName += moduleName;
    }

    return Load(resultingName);
}
PyObject* PythonWalker::Module::LoadDict(PyObject* pModule)
{
    PyObject* dict = PyModule_GetDict(pModule);
    if (dict == nullptr) {
        PyErr_Print();
        throw PythonModuleDictionaryFailed();
    }
    return dict;
}
// https://stackoverflow.com/questions/25314774/python-c-api-iterate-through-classes-in-module
// https://stackoverflow.com/questions/58075398/how-to-list-all-function-names-of-a-python-module-in-c
// https://stackoverflow.com/questions/5520580/how-do-you-get-all-classes-defined-in-a-module-but-not-imported
std::vector<PythonWalker::ClassDefinition> PythonWalker::Module::GetClassDefinitions(const char* moduleName)
{
    std::vector<ClassDefinition> results;
    // At this point we have a python module, find classes within it
    PyObject* module = Load(moduleName);
    PyObject* dict = PyModule_GetDict(module);
    PyObject* pKey, * value = NULL;
    Py_ssize_t pos = 0;

    while (PyDict_Next(dict, &pos, &pKey, &value)) {
        if (PyType_Check(value)) {

            // Make sure this class is actually from the working module, not just imported from another
            if (!MatchesModule(value, moduleName)) {
                continue;
            }
            const char* className = PyUnicode_AsUTF8(pKey);
            results.push_back(ClassDefinition(moduleName, className));
        }
    }
    //Py_DECREF(dict);
    //Py_DECREF(module);
    return results;
}

PyObject* PythonWalker::Module::GetPythonClass(const char* moduleName, const char* className)
{
    PyObject* pModule = Load(moduleName);
    return GetPythonClass(pModule, className);
}
PyObject* PythonWalker::Module::GetPythonClass(PyObject* pModule, const char* className)
{
    if (pModule == nullptr) {
        throw PythonModuleNotInitialized();
    }

    PyObject* dict = LoadDict(pModule);

    // Builds the name of a callable class
    PyObject* pythonClass = PyDict_GetItemString(dict, className);
    if (pythonClass == nullptr) {
        PyErr_Print();
        throw PythonObjectDNE(className);
    }
    //Py_DECREF(dict);
    return pythonClass;
}


bool PythonWalker::Module::MatchesModule(PyObject* pObject, const char* moduleName)
{
    ClassDefinition def(pObject);
    return strcmp(moduleName, def.Module.c_str()) == 0;
}
