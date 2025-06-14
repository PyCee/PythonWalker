#pragma once
#include <string>
#include <vector>
#include "PythonHelper.h"
#include "ObjectInstance.h"
#include "ClassDefinition.h"
#include "TypeConversions.h"
#include "CodeGeneration.h"
#include "Logging.h"
#include "ScriptManager.h"
#include "TypingsGeneration.h"
#include "Module.h"
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

	std::optional<PyObject*> CreateObject(PyObject* module, const char* className);
	void CopyPyObjectValues(PyObject* src, PyObject* dest);
	/*
	* @param pythonObject - The object to run the function on
	* @param functionName - Name of function in the python script to run
	* @param keywords - Keyword arguments to pass into the function
	*/
	std::optional<PyObject*> ExecuteFunction(PyObject* pythonObject, const char* functionName, PyObject* keywords = nullptr);

	std::optional<PyObject*> GetPyObjectDataContainer(std::string moduleName);
	std::optional<PyObject*> GetPyObjectDataContainer(PyObject* pyObject);
}

static PyObject* GetPyObjectFromValue(PythonWalker::ObjectInstance value)
{
	std::optional<PyObject*> dupObject = value.ClassDef.GetNewObject();
	PythonWalker::CopyPyObjectValues(value.PyObjectInstance, dupObject.value());
	return dupObject.value();
}