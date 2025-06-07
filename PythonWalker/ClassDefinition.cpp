#include "ClassDefinition.h"
#include "PythonWalker.h"

PythonWalker::ClassDefinition::ClassDefinition()
{
}
PythonWalker::ClassDefinition::ClassDefinition(const char* fullClassPath)
{
    std::string fullClassPathStr(fullClassPath);
    size_t lastDotOffset = fullClassPathStr.find_last_of(".");
    ClassName = fullClassPathStr.substr(lastDotOffset + 1).c_str();
    Module = fullClassPathStr.substr(0, lastDotOffset).c_str();
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
std::optional<PyObject*> PythonWalker::ClassDefinition::GetPythonClass() const
{
	return PythonWalker::Module::GetPythonClass(Module.c_str(), ClassName.c_str());
}
std::optional<PyObject*> PythonWalker::ClassDefinition::GetNewObject() const
{
    std::optional<PyObject*> pModule = PythonWalker::Module::Load(Module);
    if (!pModule) {
        std::nullopt;
    }
    return PythonWalker::CreateObject(pModule.value(), ClassName.c_str());
}
bool PythonWalker::ClassDefinition::IsValid() const
{
	return !Module.empty() && !ClassName.empty();
}
bool PythonWalker::ClassDefinition::IncludesModule(std::string moduleName)
{
    std::vector<std::string> modules = PythonWalker::Module::ParsePythonModuleString(Module);
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
std::optional<std::vector<PythonWalker::ClassDefinition>>
PythonWalker::ClassDefinition::FilterToDerivingFromClass(std::string className, std::vector<PythonWalker::ClassDefinition> defs)
{
    bool foundBaseClass = false;
    for (ClassDefinition def : defs) {
        if (def.ClassName == className) {
            std::optional< std::vector<PythonWalker::ClassDefinition>> resultingDefs = def.FilterToDerivedDefinitions(defs);
            if (!resultingDefs) {
                return std::nullopt;
            }
            defs = resultingDefs.value();
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
std::optional<std::vector<PythonWalker::ClassDefinition>>
PythonWalker::ClassDefinition::FilterToDerivedDefinitions(std::vector<PythonWalker::ClassDefinition> definitions) const
{
    std::optional<PyObject*> baseClass = GetPythonClass();
    if (!baseClass) {
        return std::nullopt;
    }
    std::vector<ClassDefinition> results;
    for (ClassDefinition def : definitions) {
        if (def.ClassName == ClassName) {
            continue;
        }
        PyObject* pClass = def.GetPythonClass().value();
        if (PyObject_IsSubclass(pClass, baseClass.value())) {
            results.push_back(def);
        }
    }
    return results;
}
