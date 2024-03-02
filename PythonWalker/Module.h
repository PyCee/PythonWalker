#pragma once

#include <string>
#include <vector>
#include "PythonHelper.h"
#include "ClassDefinition.h"
#include "Exceptions.h"

namespace PythonWalker {
	PyObject* GetPyObjectDataContainer(std::string pModuleName);
}

namespace PythonWalker::Module {

	std::vector<std::string> ParsePythonModuleString(std::string module);
	/* Loads python module into memory
	* @param moduleName - Python file path
	*/
	PyObject* Load(std::string moduleName);
	PyObject* Load(std::vector<std::string> moduleNames);

	PyObject* LoadDict(PyObject* pModule);
	template <typename T>
	T GetGlobal(PyObject* pModule, const char* variableName)
	{
		PyObject* dict = PythonWalker::Module::LoadDict(pModule);

		PyObject* pythonGlobalValue = PyDict_GetItemString(dict, variableName);
		if (pythonGlobalValue == nullptr) {
			PyErr_Print();
			throw PythonWalker::PythonObjectDNE(variableName);
		}
		return GetValueFromPyObject<T>(pythonGlobalValue);
	}
	template <typename T>
	T GetGlobal(std::string pModuleName, const char* variableName)
	{
		PyObject* pModule = PythonWalker::GetPyObjectDataContainer(pModuleName);
		return GetGlobal<T>(pModule, variableName);
	}
	template <typename T>
	void SetGlobal(PyObject* pModule, const char* variableName, T value)
	{
		PyObject* dict = PythonWalker::Module::LoadDict(pModule);
		PyDict_SetItemString(dict, variableName, GetPyObjectFromValue(value));
	}

	template <typename T>
	void SetGlobal(std::string pModuleName, const char* variableName, T value)
	{
		PyObject* pModule = PythonWalker::GetPyObjectDataContainer(pModuleName);
		return SetGlobal<T>(pModule, variableName, value);
	}
	/* Retrieves class definitions from the given module
	* @param moduleName - Name of the module to check
	*/
	std::vector<ClassDefinition> GetClassDefinitions(const char* moduleName);
	PyObject* GetPythonClass(const char* moduleName, const char* className);
	PyObject* GetPythonClass(PyObject* pModule, const char* className);
	bool MatchesModule(PyObject* pObject, const char* moduleName);

}

