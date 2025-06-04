#pragma once

//Return a list/vector object that overloads the & []= to call the refresh

#define __GEN_PYTHON_VARIABLE_FORCE_SET(TYPE, VARIABLE_NAME)			\
	void Reload_##VARIABLE_NAME(){SetPythonVariable<TYPE>(#VARIABLE_NAME, __GEN_PYTHON_VARIABLE_INTERNAL_NAME(VARIABLE_NAME));}
#define __GEN_PYTHON_VARIABLE_INTERNAL_NAME(VARIABLE_NAME) _##VARIABLE_NAME
#define __GEN_PYTHON_VARIABLE_GETTER_NAME(VARIABLE_NAME) Get_##VARIABLE_NAME
#define __GEN_PYTHON_VARIABLE_SETTER_NAME(VARIABLE_NAME) Set_##VARIABLE_NAME
#define __GEN_PYTHON_VARIABLE_GETTER(TYPE, VARIABLE_NAME)				\
	TYPE& __GEN_PYTHON_VARIABLE_GETTER_NAME(VARIABLE_NAME)() {			\
		__GEN_PYTHON_VARIABLE_INTERNAL_NAME(VARIABLE_NAME) = GetPythonVariable<TYPE>(#VARIABLE_NAME); \
		return __GEN_PYTHON_VARIABLE_INTERNAL_NAME(VARIABLE_NAME);		\
	}			
#define __GEN_PYTHON_VARIABLE_SETTER(TYPE, VARIABLE_NAME) \
	void __GEN_PYTHON_VARIABLE_SETTER_NAME(VARIABLE_NAME)(TYPE value) { \
		__GEN_PYTHON_VARIABLE_INTERNAL_NAME(VARIABLE_NAME) = value;		\
		SetPythonVariable<TYPE>(#VARIABLE_NAME, value);					\
	}

	/* Macro for generating a variable within a class that is accessible between python and C++ */
#define __PYW_TYPING_VAR(TYPE, VARIABLE_NAME)						\
	private: TYPE __GEN_PYTHON_VARIABLE_INTERNAL_NAME(VARIABLE_NAME) = {};	\
	public:																\
	__GEN_PYTHON_VARIABLE_FORCE_SET(TYPE, VARIABLE_NAME)				\
	__GEN_PYTHON_VARIABLE_GETTER(TYPE, VARIABLE_NAME)					\
	__GEN_PYTHON_VARIABLE_SETTER(TYPE, VARIABLE_NAME)					\
	__declspec(property(get = __GEN_PYTHON_VARIABLE_GETTER_NAME(VARIABLE_NAME), put = __GEN_PYTHON_VARIABLE_SETTER_NAME(VARIABLE_NAME))) TYPE VARIABLE_NAME;


	/*
#define __GEN_PYTHON_VARIABLE_GETTER_WRAPPER(TYPE, VARIABLE_NAME)				\
	PyVariableWrapper<TYPE> __GEN_PYTHON_VARIABLE_GETTER_NAME(VARIABLE_NAME)() {			\
		__GEN_PYTHON_VARIABLE_INTERNAL_NAME(VARIABLE_NAME) = GetPythonVariable<TYPE>(#VARIABLE_NAME); \
		return PyVariableWrapper<TYPE>(#VARIABLE_NAME,__GEN_PYTHON_VARIABLE_INTERNAL_NAME(VARIABLE_NAME),this);\
	}
#define __GEN_PYTHON_VARIABLE_SETTER_WRAPPER(TYPE, VARIABLE_NAME) \
	void __GEN_PYTHON_VARIABLE_SETTER_NAME(VARIABLE_NAME)(TYPE value) { \
		__GEN_PYTHON_VARIABLE_INTERNAL_NAME(VARIABLE_NAME) = value;		\
		SetPythonVariable<TYPE>(#VARIABLE_NAME, value);					\
	}

#define __GEN_PYTHON_VARIABLE(TYPE, VARIABLE_NAME)						\
	private: TYPE __GEN_PYTHON_VARIABLE_INTERNAL_NAME(VARIABLE_NAME) = {};	\
	public:																\
	__GEN_PYTHON_VARIABLE_GETTER_WRAPPER(TYPE, VARIABLE_NAME)					\
	__GEN_PYTHON_VARIABLE_SETTER_WRAPPER(TYPE, VARIABLE_NAME)					\
	__declspec(property(get = __GEN_PYTHON_VARIABLE_GETTER_NAME(VARIABLE_NAME), put = __GEN_PYTHON_VARIABLE_SETTER_NAME(VARIABLE_NAME))) PyVariableWrapper<TYPE> VARIABLE_NAME;
	*/


	// https://stackoverflow.com/questions/11920577/casting-all-parameters-passed-in-macro-using-va-args
	// Test macros at https://godbolt.org/
	// https://learn.microsoft.com/en-us/cpp/preprocessor/preprocessor-experimental-overview?view=msvc-160

	/* This counts the number of args */
#define __GEN_PYTHON_NARGS_SEQ(_1,_2,_3,_4,_5,_6,_7,_8,N,...) N
#define __GEN_PYTHON_NARGS(...) __GEN_PYTHON_NARGS_(__VA_ARGS__)
#define __GEN_PYTHON_NARGS_(...) __GEN_PYTHON_NARGS__(__VA_ARGS__)
#define __GEN_PYTHON_NARGS__(...) __GEN_PYTHON_NARGS_SEQ(__VA_ARGS__##_, 8, 7, 6, 5, 4, 3, 2, 1)

/* This will let macros expand before concating them */
#define __GEN_PYTHON_PRIMITIVE_CAT(x, y) x ## y
#define __GEN_PYTHON_CAT(x, y) __GEN_PYTHON_PRIMITIVE_CAT(x, y)

#define __GEN_PYTHON_really_unparen(...) __VA_ARGS__
#define __GEN_PYTHON_invoke(expr) expr
#define __GEN_PYTHON_unparen(args)  __GEN_PYTHON_invoke(__GEN_PYTHON_really_unparen args)

/* This will call a macro on each argument passed in */
#define APPLY(macro, ...) __GEN_PYTHON_CAT(APPLY_, __GEN_PYTHON_NARGS(__VA_ARGS__))(macro, __VA_ARGS__)
#define APPLY_1(m, x1) m(x1)
#define APPLY_2(m, x1, x2) m(x1), m(x2)
#define APPLY_3(m, x1, x2, x3) m(x1), m(x2), m(x3)
#define APPLY_4(m, x1, x2, x3, x4) m(x1), m(x2), m(x3), m(x4)
#define APPLY_5(m, x1, x2, x3, x4, x5) m(x1), m(x2), m(x3), m(x4), m(x5)
#define APPLY_6(m, x1, x2, x3, x4, x5, x6) m(x1), m(x2), m(x3), m(x4), m(x5), m(x6)
#define APPLY_7(m, x1, x2, x3, x4, x5, x6, x7) m(x1), m(x2), m(x3), m(x4), m(x5), m(x6), m(x7)
#define APPLY_8(m, x1, x2, x3, x4, x5, x6, x7, x8) m(x1), m(x2), m(x3), m(x4), m(x5), m(x6), m(x7), m(x8)

#define DOUBLE_APPLY_(...) 
#define DOUBLE_APPLY_1(...) 
#define DOUBLE_APPLY_2(m, separator, x1, x2) m(x1, x2)
#define DOUBLE_APPLY_4(m, separator, x1, x2, x3, x4) m(x1, x2) __GEN_PYTHON_unparen(separator)  m(x3, x4)
#define DOUBLE_APPLY_6(m, separator, x1, x2, x3, x4, x5, x6) m(x1, x2) __GEN_PYTHON_unparen(separator) m(x3, x4) __GEN_PYTHON_unparen(separator) m(x5, x6)
#define DOUBLE_APPLY_8(m, separator, x1, x2, x3, x4, x5, x6, x7, x8) m(x1, x2) __GEN_PYTHON_unparen(separator) m(x3, x4) __GEN_PYTHON_unparen(separator) m(x5, x6) __GEN_PYTHON_unparen(separator) m(x7, x8)
#define DOUBLE_APPLY(macro, separator, ...) __GEN_PYTHON_CAT(DOUBLE_APPLY_, __GEN_PYTHON_NARGS(__VA_ARGS__))(macro, separator, __VA_ARGS__)

#define __GEN_PYTHON_FUNCTION_KEYWORD_ENTRY(TYPE, VARIABLE_NAME)    \
    PyDict_SetItemString(keywords, #VARIABLE_NAME, PythonWalker::GetPyObjectFromValue(VARIABLE_NAME));

#define __GEN_PYTHON_FUNCTION_KEYWORD_PARAMETER(TYPE, VARIABLE_NAME) TYPE VARIABLE_NAME

#define __GEN_PYTHON_NOTHING ()
#define __GEN_PYTHON_FUNCTION_PARENTHESIS_COMMA (,)

#define __GEN_PYTHON_FUNCTION_SPACER_void 1, 
#define __GEN_PYTHON_FUNCTION_IF_NOT_VOID(type, result) __GEN_PYTHON_FUNCTION_IF_NOT_VOID_(__GEN_PYTHON_FUNCTION_SPACER_##type, result)
#define __GEN_PYTHON_FUNCTION_IF_NOT_VOID_(...) __GEN_PYTHON_FUNCTION_IF_NOT_VOID__(__VA_ARGS__)
#define __GEN_PYTHON_FUNCTION_IF_NOT_VOID__(...) __GEN_PYTHON_FUNCTION_GET_SECOND_PARAM(__VA_ARGS__)
#define __GEN_PYTHON_FUNCTION_GET_SECOND_PARAM(x1, x2, ...) x2


namespace PythonWalker {
	PyObject* GetPyObjectDataContainer(std::string moduleName);
	PyObject* GetPyObjectDataContainer(PyObject* pyObject);
}

/* Macro for generating a function that calls into an equivalent python method
	Example usage: __PYW_TYPING_METHOD(moduleName, int, addTogether, int, num1, int, num2)
	@param PYOBJECT - The object containing the method. Should be either the class instance or the module, depending on method scope. May be the name of the module
	@param RETURN_TYPE - The return type of the method.
	@param FUNCTION_NAME - The resulting name of the function. Must match the name in python.
	@param ... - Defines the parameters for the method. Must match the method parameter names in python. Pass in like
*/
#define __PYW_TYPING_METHOD(PYOBJECT, RETURN_TYPE, FUNCTION_NAME, ...)												\
	RETURN_TYPE FUNCTION_NAME(																		\
		DOUBLE_APPLY(__GEN_PYTHON_FUNCTION_KEYWORD_PARAMETER, __GEN_PYTHON_FUNCTION_PARENTHESIS_COMMA, __VA_ARGS__) ) {	\
		PyObject* container = PythonWalker::GetPyObjectDataContainer(PYOBJECT); \
		PyObject* keywords = PyDict_New();																	\
		DOUBLE_APPLY(__GEN_PYTHON_FUNCTION_KEYWORD_ENTRY, __GEN_PYTHON_NOTHING, __VA_ARGS__)								\
		__GEN_PYTHON_FUNCTION_IF_NOT_VOID(RETURN_TYPE, PyObject* result = )								\
			PythonWalker::ExecuteFunction(container, #FUNCTION_NAME, keywords);														\
		Py_DECREF(keywords);																				\
		return __GEN_PYTHON_FUNCTION_IF_NOT_VOID(RETURN_TYPE, PythonWalker::GetValueFromPyObject<RETURN_TYPE>(result););										\
	}

/* Macro for generating a function within a class, for mirroring a python class. Similar to __PYW_TYPING_METHOD.

*/
#define __PYW_TYPING_CLASS_METHOD(RETURN_TYPE, FUNCTION_NAME, ...) __PYW_TYPING_METHOD(PyObjectInstance, RETURN_TYPE, FUNCTION_NAME, __VA_ARGS__)

