#include "ClassDefinition.h"
#include "PythonWalker.h"

PythonWalker::ClassDefinition::ClassDefinition()
{
}
PythonWalker::ClassDefinition::ClassDefinition(const char* module, const char* className)
	: Module(module), ClassName(className)
{
}
PythonWalker::ClassDefinition::ClassDefinition(PyObject* pyObject)
{
    Module = PyUnicode_AsUTF8(PyObject_GetAttrString(pyObject, "__module__"));
    PyObject* pyClass = PyObject_GetAttrString(pyObject, "__class__");
    ClassName = PyUnicode_AsUTF8(PyObject_GetAttrString(pyClass, "__name__"));
}
PyObject* PythonWalker::ClassDefinition::GetPythonClass() const
{
	return PythonWalker::GetPythonClass(Module.c_str(), ClassName.c_str());
}
PyObject* PythonWalker::ClassDefinition::GetNewObject() const
{
    PyObject* pModule = PythonWalker::LoadModule(Module);
    return PythonWalker::CreateObject(pModule, ClassName.c_str());
}
bool PythonWalker::ClassDefinition::IsValid() const
{
	return !Module.empty() && !ClassName.empty();
}
bool PythonWalker::ClassDefinition::IncludesModule(std::string moduleName)
{
    std::vector<std::string> modules = PythonWalker::ParsePythonModuleString(Module);
    return std::find(modules.begin(), modules.end(), moduleName) != modules.end();
}
std::vector<PythonWalker::ClassDefinition>
PythonWalker::ClassDefinition::FilterOutModule(std::string moduleName, std::vector<PythonWalker::ClassDefinition> defs)
{
    std::vector<ClassDefinition> results;
    for (ClassDefinition def : defs) {
        if (!def.IncludesModule(moduleName)) {
            results.push_back(def);
        }
    }
    return results;
}
std::vector<PythonWalker::ClassDefinition>
PythonWalker::ClassDefinition::FilterToDerivingFromClass(std::string className, std::vector<PythonWalker::ClassDefinition> defs)
{
    bool foundBaseClass = false;
    for (ClassDefinition def : defs) {
        if (def.ClassName == className) {
            defs = def.FilterToDerivedDefinitions(defs);
            foundBaseClass = true;
            break;
        }
    }
    if (!foundBaseClass)
    {
        defs.clear();
    }
    return defs;
}
std::vector<PythonWalker::ClassDefinition>
PythonWalker::ClassDefinition::FilterToDerivedDefinitions(std::vector<PythonWalker::ClassDefinition> definitions) const
{
    PyObject* baseClass = GetPythonClass();
    std::vector<ClassDefinition> results;
    for (ClassDefinition def : definitions) {
        if (def.ClassName == ClassName) {
            continue;
        }
        PyObject* pClass = def.GetPythonClass();
        if (PyObject_IsSubclass(pClass, baseClass)) {
            results.push_back(def);
        }
    }
    return results;
}
