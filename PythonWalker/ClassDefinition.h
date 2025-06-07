#pragma once
#include <iostream>
#include <vector>
#include <optional>
#include "PythonHelper.h"

namespace PythonWalker {
	class ClassDefinition
	{
	public:
		ClassDefinition();
		ClassDefinition(const char* fullClassPath);
		ClassDefinition(const char* module, const char* className);
		ClassDefinition(PyObject* pyObject);

		bool operator == (ClassDefinition& def) const
		{
			return Module == def.Module && ClassName == def.ClassName;
		}
		std::string Module;
		std::string ClassName;
		std::optional<PyObject*> GetPythonClass() const;
		std::optional<PyObject*> GetNewObject() const;
		bool IsValid() const;
		bool IncludesModule(std::string moduleName);
		static std::vector<ClassDefinition> FilterOutModule(std::string moduleName, std::vector<ClassDefinition> defs);
		static std::optional<std::vector<ClassDefinition>> FilterToDerivingFromClass(std::string className, std::vector<ClassDefinition> defs);
		std::optional<std::vector<ClassDefinition>> FilterToDerivedDefinitions(std::vector<ClassDefinition> definitions) const;
	};
}