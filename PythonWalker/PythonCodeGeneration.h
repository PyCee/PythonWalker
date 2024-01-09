#pragma once

#include <iostream>
#include <vector>
#include <filesystem>

class PythonClassDefinition;

class PythonCodeGeneration
{
public:

	static std::filesystem::path GetGeneratedFilePath(std::filesystem::path path, std::vector<std::string> module);
	static std::filesystem::path GeneratePythonClass(std::filesystem::path path, std::vector<std::string> module, std::string code, bool append=false);
	static std::filesystem::path GeneratePythonClass(std::filesystem::path path, std::string module, std::string code, bool append=false);
	static std::filesystem::path GeneratePythonClass(std::filesystem::path path, PythonClassDefinition def, std::string code, bool append=false);
	static void DeletePythonModule(std::filesystem::path path, std::vector<std::string> module, bool isDirectory = false);
	static void DeletePythonModule(std::filesystem::path path, std::string module, bool isDirectory = false);
	static void DeletePythonModule(std::filesystem::path path, PythonClassDefinition def, bool isDirectory = false);
};

