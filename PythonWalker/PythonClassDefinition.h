#pragma once
#include <iostream>
#include <vector>
#include "PythonHelper.h"

class PythonClassDefinition
{
public:
	PythonClassDefinition();
	PythonClassDefinition(const char* module, const char* className);
	PythonClassDefinition(PyObject* pyObject);

	bool operator == (PythonClassDefinition& def) const
	{
		return Module == def.Module && ClassName == def.ClassName;
	}
	std::string Module;
	std::string ClassName;
	PyObject* GetPythonClass() const;
	PyObject* GetNewObject() const;
	bool IsValid() const;
	bool IncludesModule(std::string moduleName);
	static std::vector<PythonClassDefinition> FilterOutModule(std::string moduleName, std::vector<PythonClassDefinition> defs);
	static std::vector<PythonClassDefinition> FilterToDerivingFromClass(std::string className, std::vector<PythonClassDefinition> defs);
	std::vector<PythonClassDefinition> FilterToDerivedDefinitions(std::vector<PythonClassDefinition> definitions) const;
};

