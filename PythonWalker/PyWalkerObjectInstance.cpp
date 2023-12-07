#include "PyWalkerObjectInstance.h"
#include "PythonWalker.h"

PyWalkerObjectInstance::PyWalkerObjectInstance()
{
	ModuleName = "";
	ClassName = "";
	PyObjectInstance = nullptr;
}
PyWalkerObjectInstance::PyWalkerObjectInstance(const char* moduleName, const char* className)
{
	ModuleName = moduleName;
	ClassName = className;
	if (strlen(moduleName) == 0 || strlen(className) == 0) {
		PyObjectInstance = nullptr;
		return;
	}
	PyObject* pyModule = PythonWalker::LoadModule(moduleName);
	PyObjectInstance = PythonWalker::CreateObject(pyModule, className);
}
PyWalkerObjectInstance::PyWalkerObjectInstance(PyObject* pyObject) : PyWalkerObjectInstance() {
	PyObjectInstance = pyObject;
}
PyWalkerObjectInstance::~PyWalkerObjectInstance()
{

}
PyObject* PyWalkerObjectInstance::ExecuteFunctionInternal(const char* functionName, PyObject* keywords)
{
	return PythonWalker::ExecuteFunction(PyObjectInstance, functionName, keywords);
}
