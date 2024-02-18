#pragma once

#include <iostream>
#include <format>
#include "PythonHelper.h"
#include "Exceptions.h"
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

		template <typename T>
		T ExecuteFunction(const char* functionName, PyObject* keywords = nullptr) {
			PyObject* result = ExecuteFunctionInternal(functionName, keywords);
			return GetValueFromPyObject<T>(result);
		}
		bool IsInitialized() const { return PyObjectInstance != nullptr; }
		void RegenerateFromScript();

		PyObject* PyObjectInstance;
		ClassDefinition ClassDef;


	protected:

	private:
		PyObject* ExecuteFunctionInternal(const char* functionName, PyObject* keywords = nullptr);
	public:

		template <typename T>
		T GetPythonVariable(const char* variableName) {
			if (PyObjectInstance == nullptr) {
				throw PythonObjectNotInitialized();
			}

			PyObject* pyVariableName = PyUnicode_FromString(variableName);
			if (!PyObject_HasAttr(PyObjectInstance, pyVariableName)) {
				throw PythonAttributeDNE(variableName, ClassDef.ClassName, ClassDef.Module);
			}
			PyObject* pyValue = PyObject_GetAttr(PyObjectInstance, pyVariableName);
			if (pyValue == NULL) {
				throw PythonAttributeDNE(variableName, ClassDef.ClassName, ClassDef.Module);
			}
			return GetValueFromPyObject<T>(pyValue);
		}

		template <typename T>
		void SetPythonVariable(const char* variableName, T value) {
			if (PyObjectInstance == nullptr) {
				throw PythonObjectNotInitialized();
			}

			PyObject* pyVariableName = PyUnicode_FromString(variableName);
			if (!PyObject_HasAttr(PyObjectInstance, pyVariableName)) {
				throw PythonAttributeDNE(variableName, ClassDef.ClassName, ClassDef.Module);
			}
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

