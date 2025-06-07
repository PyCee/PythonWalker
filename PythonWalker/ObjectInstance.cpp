#include "ObjectInstance.h"
#include "PythonWalker.h"

PythonWalker::ObjectInstance::ObjectInstance()
{
	PyObjectInstance = nullptr;
}
PythonWalker::ObjectInstance::ObjectInstance(const char* moduleName, const char* className)
{
	PythonWalker::Initialize();
	if (strlen(moduleName) == 0 || strlen(className) == 0) {
		PyObjectInstance = nullptr;
		return;
	}
	ClassDef = ClassDefinition(moduleName, className);
	PyObjectInstance = ClassDef.GetNewObject();
}
PythonWalker::ObjectInstance::ObjectInstance(PyObject* pyObject) : PythonWalker::ObjectInstance() {
	ClassDef = ClassDefinition(pyObject);
	PyObjectInstance = ClassDef.GetNewObject();
	PythonWalker::CopyPyObjectValues(pyObject, PyObjectInstance);
}
PythonWalker::ObjectInstance::~ObjectInstance() {}

void PythonWalker::ObjectInstance::RegenerateFromScript()
{
	RegenerateFromScript(ClassDef);
}
void PythonWalker::ObjectInstance::RegenerateFromScript(ClassDefinition classDef)
{
	PyObject* regenPyObject = PyObjectInstance;
	PyObjectInstance = classDef.GetNewObject();
	PythonWalker::CopyPyObjectValues(regenPyObject, PyObjectInstance);
}
