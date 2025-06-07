#pragma once

#include <iostream>
#include <format>
#include "PythonHelper.h"
#include "TypeConversions.h"
#include "ClassDefinition.h"

namespace PythonWalker {
	class ObjectInstance
	{
	public:
		ObjectInstance();
		ObjectInstance(const char* moduleName, const char* className);
		ObjectInstance(PyObject* pyObject);
		~ObjectInstance();

		bool IsInitialized() const { return PyObjectInstance != nullptr; }
		void RegenerateFromScript();
		void RegenerateFromScript(ClassDefinition);

		PyObject* PyObjectInstance;
		ClassDefinition ClassDef;

	public:

		template <typename T>
		std::optional<T> GetPythonVariable(const char* variableName) {
			if (PyObjectInstance == nullptr) {
				return std::nullopt;
			}

			PyObject* pyVariableName = PyUnicode_FromString(variableName);
			if (!PyObject_HasAttr(PyObjectInstance, pyVariableName)) {
				return std::nullopt;
			}
			PyObject* pyValue = PyObject_GetAttr(PyObjectInstance, pyVariableName);
			if (pyValue == NULL) {
				return std::nullopt;
			}
			return GetValueFromPyObject<T>(pyValue);
		}

		template <typename T>
		void SetPythonVariable(const char* variableName, T value) {
			PyObject* pyVariableName = PyUnicode_FromString(variableName);
			PyObject* pyValue = GetPyObjectFromValue(value);
			PyObject_SetAttr(PyObjectInstance, pyVariableName, pyValue);
		}
	};
}


/*
class PyWalkerObjectInstance;
template <typename T>
class PyVariableWrapper {
private:
	const char* variableName;
	T value;
	void (*refreshFunc)();
	PyWalkerObjectInstance* parent;
public:
	PyVariableWrapper(const char* name, T v, PyWalkerObjectInstance* p) {//}, void (refreshListFunc)()) {
		variableName = name;
		value = v;
		parent = p;
		//refreshFunc = refreshListFunc;
	}
	void operator =(T v) {
		// Setter
		value = v;
		parent->SetPythonVariable<T>(variableName, v);
		//refreshFunc();
	}
	operator T() const {
		// Getter
		value = parent->GetPythonVariable<T>(variableName);
		return value;
	}
};*/

