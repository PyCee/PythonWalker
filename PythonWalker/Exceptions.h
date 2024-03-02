#pragma once

#include <iostream>
#include <format>

namespace PythonWalker{
	class PythonObjectDNE : public std::runtime_error
	{
	public:	PythonObjectDNE(std::string className) :
		std::runtime_error(std::format("Class %s does not exist", className)) {}
	};
	class PythonObjectNotInitialized : public std::runtime_error
	{
	public:	PythonObjectNotInitialized() :
		std::runtime_error("Attempted to use object that was not initialized") {}
	};
	class PythonAttributeDNE : public std::runtime_error
	{
	public:	PythonAttributeDNE(std::string attributeName, std::string className, std::string moduleName) :
			std::runtime_error(std::format("Attribute %s does not exist on class %s in module %s", attributeName, className, moduleName)) {}
	};
	class PythonMethodDNE : public std::runtime_error
	{
	public:	PythonMethodDNE(std::string methodName, std::string className, std::string moduleName) :
			std::runtime_error(std::format("Method %s does not exist on class %s in module %s", methodName, className, moduleName)) {}
	};
	class PythonMethodError : public std::runtime_error
	{
	public:	PythonMethodError(std::string paramName, std::string className, std::string moduleName, std::string errorMsg) :
			std::runtime_error(std::format("Method %s on class %s in module %s failed to execute: %s", paramName, className, moduleName, errorMsg)) {}
	};
	class PythonModuleDNE : public std::runtime_error
	{
	public:	PythonModuleDNE(std::string moduleName) :
			std::runtime_error(std::format("Module %s does not exist", moduleName)) {}
	};
	class PythonModuleNotInitialized : public std::runtime_error
	{
	public:	PythonModuleNotInitialized() :
		std::runtime_error("Module has not been initialized") {}
	};
	class PythonModuleDictionaryFailed : public std::runtime_error
	{
	public:	PythonModuleDictionaryFailed() :
		std::runtime_error("Module dictionary could not be created") {}
	};
	class PythonLoggingContextAlreadyOpen : public std::runtime_error
	{
	public:	PythonLoggingContextAlreadyOpen(std::string newLoggingFilePath, std::string oldLoggingFilePath) :
		std::runtime_error(std::format("Cannot start logging to file %s when already logging to file %s", newLoggingFilePath, oldLoggingFilePath)) {}
	};
}