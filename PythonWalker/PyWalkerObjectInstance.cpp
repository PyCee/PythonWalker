#include "PyWalkerObjectInstance.h"
#include "PythonWalker.h"

PyWalkerObjectInstance::PyWalkerObjectInstance()
{
	PyObjectInstance = nullptr;
}
PyWalkerObjectInstance::PyWalkerObjectInstance(const char* moduleName, const char* className)
{
	if (strlen(moduleName) == 0 || strlen(className) == 0) {
		PyObjectInstance = nullptr;
		return;
	}
	ClassDef = PythonClassDefinition(moduleName, className);
	PyObjectInstance = ClassDef.GetNewObject();
}
PyWalkerObjectInstance::PyWalkerObjectInstance(PyObject* pyObject) : PyWalkerObjectInstance() {
	ClassDef = PythonClassDefinition(pyObject);
	PyObjectInstance = ClassDef.GetNewObject();
	PythonWalker::CopyPyObjectValues(pyObject, PyObjectInstance);
}
PyWalkerObjectInstance::~PyWalkerObjectInstance() {}

void PyWalkerObjectInstance::RegenerateFromScript()
{
	PyObject* regenPyObject = PyObjectInstance;
	PyObjectInstance = ClassDef.GetNewObject();
	PythonWalker::CopyPyObjectValues(regenPyObject, PyObjectInstance);
}
PyObject* PyWalkerObjectInstance::ExecuteFunctionInternal(const char* functionName, PyObject* keywords)
{
	return PythonWalker::ExecuteFunction(PyObjectInstance, functionName, keywords);
}
