#pragma once
#include <vector>
#include <string>
#include <filesystem>
#include "PythonClassDefinition.h"

class PythonFileManagement
{
public:
	static std::vector<PythonClassDefinition> GetPythonClasses(const char* filepath, bool recursive = false, std::string extendedClassName="", std::string modulePrepend="");
	static bool IsPythonScript(std::filesystem::directory_entry entry);
	static std::ofstream CreateFile(std::filesystem::path path, bool immediatlyClose=false);
	static void ClearFile(std::filesystem::path path);
	static std::string GetFileContents(std::filesystem::path filePath);
	static void ClearDirectory(std::filesystem::path directoryPath);
};

