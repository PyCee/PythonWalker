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
	PyObjectInstance = nullptr;
	std::optional<PyObject*> newObject = ClassDef.GetNewObject();
	if (newObject) {
		PyObjectInstance = newObject.value();
	}
}
PythonWalker::ObjectInstance::ObjectInstance(PyObject* pyObject) : PythonWalker::ObjectInstance() {
	ClassDef = ClassDefinition(pyObject);
	std::optional<PyObject*> newObject = ClassDef.GetNewObject();
	if (newObject) {
		PyObjectInstance = newObject.value();
		PythonWalker::CopyPyObjectValues(pyObject, PyObjectInstance);
	}
}
PythonWalker::ObjectInstance::~ObjectInstance() {}

bool PythonWalker::ObjectInstance::RegenerateFromScript()
{
	return RegenerateFromScript(ClassDef);
}
bool PythonWalker::ObjectInstance::RegenerateFromScript(ClassDefinition classDef)
{
	std::optional<PyObject*> obj = classDef.GetNewObject();
	if (obj) {
		PyObject* regenPyObject = PyObjectInstance;
		PyObjectInstance = obj.value();
		PythonWalker::CopyPyObjectValues(regenPyObject, PyObjectInstance);
		ClassDef = classDef;
		return true;
	}
	return false;
}
