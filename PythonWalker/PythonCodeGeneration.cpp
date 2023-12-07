#include "PythonCodeGeneration.h"
#include "PythonClassDefinition.h"
#include "PythonWalker.h"

#include <fstream>


void PythonCodeGeneration::GeneratePythonClass(std::filesystem::path path, std::vector<std::string> modules, std::string code)
{
	for (std::string module : modules) {
		path /= module;
	}

	std::filesystem::create_directories(path.parent_path());
	std::ofstream newFile(path.string() + ".py", std::ios::app);
	newFile << code;
	newFile.close();
	PyImport_ReloadModule(PythonWalker::LoadModule(modules));
}

void PythonCodeGeneration::GeneratePythonClass(std::filesystem::path path, std::string module, std::string code)
{
	GeneratePythonClass(path, PythonWalker::ParsePythonModuleString(module), code);
}

void PythonCodeGeneration::GeneratePythonClass(std::filesystem::path path, PythonClassDefinition def, std::string code)
{
	GeneratePythonClass(path, def.Module, code);
}

void PythonCodeGeneration::DeletePythonModule(std::filesystem::path path, std::vector<std::string> modules, bool isDirectory)
{
	for (std::string module : modules) {
		path /= module;
	}
	if (isDirectory) {
		path += "\\";
	}
	else {
		path += ".py";
	}
	std::filesystem::remove_all(path);
}

void PythonCodeGeneration::DeletePythonModule(std::filesystem::path path, std::string module, bool isDirectory)
{
	DeletePythonModule(path, PythonWalker::ParsePythonModuleString(module), isDirectory);
}

void PythonCodeGeneration::DeletePythonModule(std::filesystem::path path, PythonClassDefinition def, bool isDirectory)
{
	DeletePythonModule(path, def.Module, isDirectory);
}
