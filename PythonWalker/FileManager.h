#pragma once
#include <vector>
#include <string>
#include <filesystem>
#include "ClassDefinition.h"

namespace PythonWalker {
	class ScriptManager {
	public:
		
		ScriptManager();
		ScriptManager(std::vector<std::string> additionalScriptsPaths);
		~ScriptManager();

		std::vector<std::string> ScriptPaths;
		/* Searches for python classes and modules in the path given in the PythonWalker setup
		* @param recursive - Pass true if the search should dive into subfolders of the defined path
		* @param baseClassName - If defined, the result will be filtered down to classes that are derived from the given class
		*/
		std::vector<ClassDefinition> GetScripts(bool recursive = false, std::string baseClassName = "",
			std::vector<std::string> excludeModules = {});
	};

	std::vector<ClassDefinition> GetPythonClasses(const char* filepath, bool recursive = false, std::string extendedClassName = "", std::string modulePrepend = "");
	bool IsPythonScript(std::filesystem::directory_entry entry);
	std::ofstream CreateFile(std::filesystem::path path, bool immediatlyClose = false);
	void EraseFile(std::filesystem::path path);
	void ClearFile(std::filesystem::path path);
	std::string GetFileContents(std::filesystem::path filePath);
	bool HasFileChanged(std::filesystem::path filePath, std::string pastFileContents);
	void ClearDirectory(std::filesystem::path directoryPath);
}

