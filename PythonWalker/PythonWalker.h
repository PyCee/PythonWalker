#pragma once
#include <string>
#include <vector>
#include "PythonHelper.h"
#include "PyWalkerObjectInstance.h"
#include "PythonWalkerExceptions.h"
#include "PythonClassDefinition.h"
#include "PythonTypeConversions.h"
#include "PythonCodeGeneration.h"
#include "PythonLogging.h"
#include "PythonFileManagement.h"
/*
* https://docs.python.org/3/c-api/
* https://docs.python.org/3/extending/embedding.html
* https://stackoverflow.com/questions/39813301/creating-a-python-object-in-c-and-calling-its-method
* https://stackoverflow.com/questions/16777126/pyobject-callmethod-with-keyword-arguments
*/

class PythonWalker
{
private:
public:
	PythonWalker();
	PythonWalker(std::vector<std::string> additionalScriptsPaths);
	~PythonWalker();

	void TearDown();


	std::vector<std::string> ScriptsPaths;
	/* Searches for python classes and modules in the path given in the PythonWalker setup
	* @param recursive - Pass true if the search should dive into subfolders of the defined path
	* @param baseClassName - If defined, the result will be filtered down to classes that are derived from the given class
	*/
	std::vector<PythonClassDefinition> GetScripts(bool recursive = false, std::string baseClassName="",
		std::vector<std::string> excludeModules={});

	static std::vector<std::string> ParsePythonModuleString(std::string module);
	/* Loads python module into memory
	* @param moduleName - Python file path
	*/
	static PyObject* LoadModule(std::string moduleName);
	static PyObject* LoadModule(std::vector<std::string> moduleNames);

	static PyObject* LoadModuleDict(PyObject* pModule);
	template <typename T>
	static T GetModuleGlobal(PyObject* pModule, const char* variableName)
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
	static void SetModuleGlobal(PyObject* pModule, const char* variableName, T value)
	{
		PyObject* dict = PythonWalker::LoadModuleDict(pModule);
		PyDict_SetItemString(dict, variableName, GetPyObjectFromValue(value));
	}

	/* Retrieves class definitions from the given module
	* @param moduleName - Name of the module to check
	*/
	static std::vector<PythonClassDefinition> GetClassDefinitionsFromModuleDict(const char * moduleName);
	static PyObject* GetPythonClass(const char * moduleName, const char* className);
	static PyObject* GetPythonClass(PyObject* pModule, const char* className);
	static bool MatchesModule(PyObject* pObject, const char* moduleName);
	static PyObject* CreateObject(PyObject* module, const char* className);
	/*
	* @param pythonObject - The object to run the function on
	* @param functionName - Name of function in the python script to run
	* @param keywords - Keyword arguments to pass into the function
	*/
	static PyObject* ExecuteFunction(PyObject* pythonObject, const char* functionName, PyObject* keywords=nullptr);
};

