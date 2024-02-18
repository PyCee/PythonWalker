#pragma once
#include <string>
#include <vector>
#include "PythonHelper.h"
#include "ObjectInstance.h"
#include "Exceptions.h"
#include "ClassDefinition.h"
#include "TypeConversions.h"
#include "CodeGeneration.h"
#include "Logging.h"
#include "FileManagement.h"
#include "TypingsGeneration.h"
/*
* https://docs.python.org/3/c-api/
* https://docs.python.org/3/extending/embedding.html
* https://stackoverflow.com/questions/39813301/creating-a-python-object-in-c-and-calling-its-method
* https://stackoverflow.com/questions/16777126/pyobject-callmethod-with-keyword-arguments
* https://docs.python.org/3/reference/datamodel.html
*/

namespace PythonWalker {
	void Initialize();
	void Destroy();


	std::vector<std::string> ParsePythonModuleString(std::string module);
	/* Loads python module into memory
	* @param moduleName - Python file path
	*/
	PyObject* LoadModule(std::string moduleName);
	PyObject* LoadModule(std::vector<std::string> moduleNames);

	PyObject* LoadModuleDict(PyObject* pModule);
	template <typename T>
	T GetModuleGlobal(PyObject* pModule, const char* variableName)
	{
		PyObject* dict = PythonWalker::LoadModuleDict(pModule);

		PyObject* pythonGlobalValue = PyDict_GetItemString(dict, variableName);
		if (pythonGlobalValue == nullptr) {
			PyErr_Print();
			throw PythonObjectDNE(variableName);
		}
		return GetValueFromPyObject<T>(pythonGlobalValue);
	}
	template <typename T>
	void SetModuleGlobal(PyObject* pModule, const char* variableName, T value)
	{
		PyObject* dict = PythonWalker::LoadModuleDict(pModule);
		PyDict_SetItemString(dict, variableName, GetPyObjectFromValue(value));
	}

	/* Retrieves class definitions from the given module
	* @param moduleName - Name of the module to check
	*/
	std::vector<ClassDefinition> GetClassDefinitionsFromModuleDict(const char* moduleName);
	PyObject* GetPythonClass(const char* moduleName, const char* className);
	PyObject* GetPythonClass(PyObject* pModule, const char* className);
	bool MatchesModule(PyObject* pObject, const char* moduleName);
	PyObject* CreateObject(PyObject* module, const char* className);
	void CopyPyObjectValues(PyObject* src, PyObject* dest);
	/*
	* @param pythonObject - The object to run the function on
	* @param functionName - Name of function in the python script to run
	* @param keywords - Keyword arguments to pass into the function
	*/
	PyObject* ExecuteFunction(PyObject* pythonObject, const char* functionName, PyObject* keywords = nullptr);
}

static PyObject* GetPyObjectFromValue(PythonWalker::ObjectInstance value)
{
	PyObject* dupObject = value.ClassDef.GetNewObject();
	PythonWalker::CopyPyObjectValues(value.PyObjectInstance, dupObject);
	return dupObject;
}