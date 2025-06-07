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
	std::optional<PyObject*> newObject = ClassDef.GetNewObject();
	PyObjectInstance = newObject.value();
}
PythonWalker::ObjectInstance::ObjectInstance(PyObject* pyObject) : PythonWalker::ObjectInstance() {
	ClassDef = ClassDefinition(pyObject);
	std::optional<PyObject*> newObject = ClassDef.GetNewObject();
	PyObjectInstance = newObject.value();
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
	std::optional<PyObject*> obj = classDef.GetNewObject();
	PyObjectInstance = obj.value();
	PythonWalker::CopyPyObjectValues(regenPyObject, PyObjectInstance);
}
