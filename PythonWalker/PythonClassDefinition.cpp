#include "PythonClassDefinition.h"
#include "PythonWalker.h"

PythonClassDefinition::PythonClassDefinition()
{
}
PythonClassDefinition::PythonClassDefinition(const char* module, const char* className)
	: Module(module), ClassName(className)
{
}
PythonClassDefinition::PythonClassDefinition(PyObject* pyObject)
{
    Module = PyUnicode_AsUTF8(PyObject_GetAttrString(pyObject, "__module__"));
    PyObject* pyClass = PyObject_GetAttrString(pyObject, "__class__");
    ClassName = PyUnicode_AsUTF8(PyObject_GetAttrString(pyClass, "__name__"));
}
PyObject* PythonClassDefinition::GetPythonClass() const
{
	return PythonWalker::GetPythonClass(Module.c_str(), ClassName.c_str());
}
PyObject* PythonClassDefinition::GetNewObject() const
{
    PyObject* pModule = PythonWalker::LoadModule(Module);
    return PythonWalker::CreateObject(pModule, ClassName.c_str());
}
bool PythonClassDefinition::IsValid() const
{
	return !Module.empty() && !ClassName.empty();
}
bool PythonClassDefinition::IncludesModule(std::string moduleName)
{
    std::vector<std::string> modules = PythonWalker::ParsePythonModuleString(Module);
    return std::find(modules.begin(), modules.end(), moduleName) != modules.end();
}
std::vector<PythonClassDefinition> PythonClassDefinition::FilterOutModule(std::string moduleName, std::vector<PythonClassDefinition> defs)
{
    std::vector<PythonClassDefinition> results;
    for (PythonClassDefinition def : defs) {
        if (!def.IncludesModule(moduleName)) {
            results.push_back(def);
        }
    }
    return results;
}
std::vector<PythonClassDefinition> PythonClassDefinition::FilterToDerivingFromClass(std::string className,
    std::vector<PythonClassDefinition> defs)
{
    bool foundBaseClass = false;
    for (PythonClassDefinition def : defs) {
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
std::vector<PythonClassDefinition> PythonClassDefinition::FilterToDerivedDefinitions(std::vector<PythonClassDefinition> definitions) const
{
    PyObject* baseClass = GetPythonClass();
    std::vector<PythonClassDefinition> results;
    for (PythonClassDefinition def : definitions) {
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
