#pragma once

#include <string>
#include <vector>
#include "PythonHelper.h"
#include "ClassDefinition.h"

namespace PythonWalker {
	std::optional<PyObject*> GetPyObjectDataContainer(std::string);
}

namespace PythonWalker::Module {

	std::vector<std::string> ParsePythonModuleString(std::string module);
	/* Loads python module into memory
	* @param moduleName - Python file path
	*/
	std::optional<PyObject*> Load(std::string moduleName);
	std::optional<PyObject*> Load(std::vector<std::string> moduleNames);

	std::optional<PyObject*> LoadDict(PyObject* pModule);
	template <typename T>
	std::optional<T> GetGlobal(PyObject* pModule, const char* variableName)
	{
		std::optional<PyObject*> dict = PythonWalker::Module::LoadDict(pModule);

		if (!dict) {
			return std::nullopt;
		}

		PyObject* pythonGlobalValue = PyDict_GetItemString(dict.value(), variableName);
		if (pythonGlobalValue == nullptr) {
			PyErr_Print();
			return std::nullopt;
		}
		return GetValueFromPyObject<T>(pythonGlobalValue);
	}
	template <typename T>
	std::optional<T> GetGlobal(std::string pModuleName, const char* variableName)
	{
		std::optional<PyObject*> pModule = PythonWalker::GetPyObjectDataContainer(pModuleName);
		if (!pModule) {
			return std::nullopt;
		}
		return GetGlobal<T>(pModule.value(), variableName);
	}
	template <typename T>
	void SetGlobal(PyObject* pModule, const char* variableName, T value)
	{
		std::optional<PyObject*> dict = PythonWalker::Module::LoadDict(pModule);
		if (dict) {
			PyDict_SetItemString(dict.value(), variableName, GetPyObjectFromValue(value));
		}
	}

	template <typename T>
	void SetGlobal(std::string pModuleName, const char* variableName, T value)
	{
		std::optional<PyObject*> pModule = PythonWalker::GetPyObjectDataContainer(pModuleName);
		if (pModule) {
			SetGlobal<T>(pModule.value(), variableName, value);
		}
	}
	/* Retrieves class definitions from the given module
	* @param moduleName - Name of the module to check
	*/
	std::vector<ClassDefinition> GetClassDefinitions(const char* moduleName);
	std::optional<PyObject*> GetPythonClass(const char* moduleName, const char* className);
	std::optional<PyObject*> GetPythonClass(PyObject* pModule, const char* className);
	bool MatchesModule(PyObject* pObject, const char* moduleName);

}

