#pragma once

#include <iostream>
#include <vector>
#include <filesystem>

class PythonClassDefinition;

class PythonCodeGeneration
{
public:
	/* Appends code to a module on the given path
	* @param path - filesystem path where the resulting module should be
	* @param module - the module, formatted as 
	*/
	static void GeneratePythonClass(std::filesystem::path path, std::vector<std::string> module, std::string code);
	static void GeneratePythonClass(std::filesystem::path path, std::string module, std::string code);
	static void GeneratePythonClass(std::filesystem::path path, PythonClassDefinition def, std::string code);
	static void DeletePythonModule(std::filesystem::path path, std::vector<std::string> module, bool isDirectory = false);
	static void DeletePythonModule(std::filesystem::path path, std::string module, bool isDirectory = false);
	static void DeletePythonModule(std::filesystem::path path, PythonClassDefinition def, bool isDirectory = false);
};

