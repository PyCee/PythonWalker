#include "CodeGeneration.h"
#include "ClassDefinition.h"
#include "PythonWalker.h"

#include <fstream>

std::filesystem::path PythonWalker::CodeGeneration::GetGeneratedFilePath(std::filesystem::path path, std::vector<std::string> modules)
{
	for (std::string module : modules) {
		path /= module;
	}
	return path;
}

std::filesystem::path PythonWalker::CodeGeneration::GeneratePythonClass(std::filesystem::path path, std::vector<std::string> modules, std::string code, bool append)
{
	path = GetGeneratedFilePath(path, modules);
	path += ".py";

	std::filesystem::create_directories(path.parent_path());
	std::ofstream newFile(path.string(), append ? std::ios::app : std::ios::out);
	newFile << code;
	newFile.close();
	PyImport_ReloadModule(PythonWalker::Module::Load(modules));
	return path;
}

std::filesystem::path PythonWalker::CodeGeneration::GeneratePythonClass(std::filesystem::path path, std::string module, std::string code, bool append)
{
	return GeneratePythonClass(path, PythonWalker::Module::ParsePythonModuleString(module), code, append);
}

std::filesystem::path PythonWalker::CodeGeneration::GeneratePythonClass(std::filesystem::path path, PythonWalker::ClassDefinition def, std::string code, bool append)
{
	return GeneratePythonClass(path, def.Module, code, append);
}

void PythonWalker::CodeGeneration::DeletePythonModule(std::filesystem::path path, std::vector<std::string> modules, bool isDirectory)
{
	path = GetGeneratedFilePath(path, modules);

	if (isDirectory) {
		path += "\\";
	}
	else {
		path += ".py";
	}
	try {
		std::filesystem::remove_all(path);
	}
	catch (std::filesystem::filesystem_error e) {
		// Catch error if the user has the fiule open when we're trying to delete
	}
}

void PythonWalker::CodeGeneration::DeletePythonModule(std::filesystem::path path, std::string module, bool isDirectory)
{
	DeletePythonModule(path, PythonWalker::Module::ParsePythonModuleString(module), isDirectory);
}

void PythonWalker::CodeGeneration::DeletePythonModule(std::filesystem::path path, PythonWalker::ClassDefinition def, bool isDirectory)
{
	std::vector<std::string> modules = PythonWalker::Module::ParsePythonModuleString(def.Module);

	// If we are clearing the directory, pop the last module since that's a specific filename
	if (isDirectory) {
		modules.pop_back();
	}
	DeletePythonModule(path, modules, isDirectory);
}
