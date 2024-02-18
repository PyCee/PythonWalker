#pragma once

#include "PythonHelper.h"
#include <vector>
#include <list>


// https://stackoverflow.com/questions/6953969/can-static-assert-check-if-a-type-is-a-vector
// https://stackoverflow.com/questions/58337609/c-template-specialization-in-subtemplates

namespace PythonWalker {

	class ObjectInstance;

	//TODO simplify the vector/list comparisons
	template <typename C> struct is_vector : std::false_type {};
	template <typename T, typename A> struct is_vector< std::vector<T, A> > : std::true_type {};
	template <typename C> inline constexpr bool is_vector_v = is_vector<C>::value;

	template <typename C> struct is_list : std::false_type {};
	template <typename T, typename A> struct is_list< std::list<T, A> > : std::true_type {};
	template <typename C> inline constexpr bool is_list_v = is_list<C>::value;

	template <typename T> static T GetValueFromIterablePyObject(PyObject* pyObject) {
		T result = {};
		Py_ssize_t length = PyTuple_Size(pyObject);
		for (int i = 0; i < length; i++) {
			result.push_back(GetValueFromPyObject<typename T::value_type>(PyTuple_GET_ITEM(pyObject, i)));
		}
		return result;
	}

	template <typename T> static T GetValueFromPyObject(PyObject* pyObject)
	{
		if constexpr (std::is_base_of_v<ObjectInstance, T>) {
			return T(pyObject);
		}
		if constexpr (is_vector_v<T> || is_list_v<T>) {
			return GetValueFromIterablePyObject<T>(pyObject);
		}
		return T();
	}
	/* __GEN_GET_PRIMITIVE_VALUE_FROM_PYOBJECT creates a templated function named GetValueFromPyObject.
	*  Macro takes the return type and function to convert a PyObject* into that type.
	*/
	#define __GEN_GET_PRIMITIVE_VALUE_FROM_PYOBJECT(TYPE, CONVERSION_FUN) \
		template <>	static TYPE GetValueFromPyObject<TYPE>(PyObject* pyObject) { return (TYPE)CONVERSION_FUN(pyObject); }

	__GEN_GET_PRIMITIVE_VALUE_FROM_PYOBJECT(int, PyLong_AsLong)
	__GEN_GET_PRIMITIVE_VALUE_FROM_PYOBJECT(long, PyLong_AsLong)
	__GEN_GET_PRIMITIVE_VALUE_FROM_PYOBJECT(float, PyFloat_AsDouble)
	__GEN_GET_PRIMITIVE_VALUE_FROM_PYOBJECT(double, PyFloat_AsDouble)
	__GEN_GET_PRIMITIVE_VALUE_FROM_PYOBJECT(const char*, PyUnicode_AsUTF8)
	__GEN_GET_PRIMITIVE_VALUE_FROM_PYOBJECT(std::string, PyUnicode_AsUTF8)
	__GEN_GET_PRIMITIVE_VALUE_FROM_PYOBJECT(bool, PyObject_IsTrue)

	template <>	static void GetValueFromPyObject<void>(PyObject* pyObject) { return; }
	template <>	static PyObject* GetValueFromPyObject<PyObject*>(PyObject* pyObject) { return pyObject; }



	/* __GEN_GET_PYOBJECT_FROM_PRIMITIVE_VALUE creates a templated function named GetPyObjectFromValue.
	*  Macro takes a variable and converts into a corresponding PyObject* for that type.
	*/
	#define __GEN_GET_PYOBJECT_FROM_PRIMITIVE_VALUE(TYPE, CONVERSION_STRING) \
		static PyObject* GetPyObjectFromValue(TYPE value) { return Py_BuildValue((CONVERSION_STRING), value); }


	__GEN_GET_PYOBJECT_FROM_PRIMITIVE_VALUE(int, "i")
	__GEN_GET_PYOBJECT_FROM_PRIMITIVE_VALUE(long, "l")
	__GEN_GET_PYOBJECT_FROM_PRIMITIVE_VALUE(float, "d")
	__GEN_GET_PYOBJECT_FROM_PRIMITIVE_VALUE(double, "d")
	__GEN_GET_PYOBJECT_FROM_PRIMITIVE_VALUE(const char*, "s")
	__GEN_GET_PYOBJECT_FROM_PRIMITIVE_VALUE(bool, "i")

	static PyObject* GetPyObjectFromValue(std::string value) { return GetPyObjectFromValue(value.c_str()); }
	static PyObject* GetPyObjectFromValue(PyObject* value) { return value; }

	template<typename T>
	static PyObject* GetPyObjectFromIterableValue(T values) {
		PyObject* tuple = PyTuple_New(values.size());
		int index = 0;
		for (typename T::value_type const& iter : values) {
			PyTuple_SET_ITEM(tuple, index, GetPyObjectFromValue(iter));
			index++;
		}
		return tuple;
	}

	template <typename T>
	static PyObject* GetPyObjectFromValue(std::vector<T> values) {
		return GetPyObjectFromIterableValue(values);
	}
	template <typename T>
	static PyObject* GetPyObjectFromValue(std::list<T> values) {
		return GetPyObjectFromIterableValue(values);
	}

}