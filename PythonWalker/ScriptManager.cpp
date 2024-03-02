#include "ScriptManager.h"
#include <filesystem>
#include <iostream>
#include <fstream>
#include "PythonWalker.h"
#include "PythonHelper.h"


PythonWalker::ScriptManager::ScriptManager() { }
PythonWalker::ScriptManager::ScriptManager(std::vector<std::string> additionalScriptsPaths) {
    PyObject* sysPath = PySys_GetObject((char*)"path");
    for (std::string scriptsPath : additionalScriptsPaths) {
        if (scriptsPath.empty()) {
            continue;
        }

        // escape characters
        //scriptsPath.replace(scriptsPath.begin(), scriptsPath.end(), "\\\\", "/");
        //scriptsPath = std::quoted(scriptsPath);

        ScriptPaths.push_back(scriptsPath);

        const char* path = scriptsPath.c_str();
        PyObject* programName = PyUnicode_FromString(path);
        PyList_Append(sysPath, programName);
        //Py_DECREF(programName);
    }
}
PythonWalker::ScriptManager::~ScriptManager() {

}

std::vector<PythonWalker::ClassDefinition> PythonWalker::ScriptManager::GetScripts(bool recursive, std::string baseClassName,
    std::vector<std::string> excludeModules)
{
    std::vector<ClassDefinition> results;
    for (std::string scriptPath : ScriptPaths) {
        std::vector<ClassDefinition> pathResults = GetPythonClasses(scriptPath.c_str(), recursive);
        results.insert(results.end(), pathResults.begin(), pathResults.end());
    }
    if (!baseClassName.empty())
    {
        results = ClassDefinition::FilterToDerivingFromClass(baseClassName, results);
    }
    if (excludeModules.size() > 0) {
        for (std::string excludeModule : excludeModules) {
            results = ClassDefinition::FilterOutModule(excludeModule, results);
        }
    }

    return results;
}

std::vector<PythonWalker::ClassDefinition> PythonWalker::ScriptManager::GetPythonClasses(const char* filepath, bool recursive,
    std::string extendedClassName, std::string modulePrepend)
{
    std::vector<ClassDefinition> results;
    for (const std::filesystem::directory_entry& entry : std::filesystem::directory_iterator(filepath)) {
        std::string stem = entry.path().stem().string();
        std::string moduleName = !modulePrepend.empty() ? modulePrepend + "." + stem : stem;

        if (entry.is_directory()) {
            if (recursive)
            {
                std::vector<PythonWalker::ClassDefinition> recursiveResults = GetPythonClasses(entry.path().string().c_str(),
                    recursive, extendedClassName, moduleName);
                results.insert(results.end(), recursiveResults.begin(), recursiveResults.end());
            }
            continue;
        }
        if (!IsPythonScript(entry)) {
            continue;
        }
        std::vector<PythonWalker::ClassDefinition> moduleResults = PythonWalker::Module::GetClassDefinitions(moduleName.c_str());
        results.insert(results.end(), moduleResults.begin(), moduleResults.end());
    }
    return results;
}
bool PythonWalker::ScriptManager::IsPythonScript(std::filesystem::directory_entry entry)
{
    return entry.path().extension() == ".py";
}

std::ofstream PythonWalker::ScriptManager::CreateFile(std::filesystem::path path, bool immediatlyClose)
{
    std::filesystem::create_directories(path.parent_path());
    std::ofstream newFile(path.string(), std::ios::app);
    if (immediatlyClose) {
        newFile.close();
    }
    return newFile;
}
std::string PythonWalker::ScriptManager::GetFileContents(std::filesystem::path filePath)
{
    std::ifstream file(filePath.string());
    std::stringstream buffer;
    buffer << file.rdbuf();
    return buffer.str();
}
bool PythonWalker::ScriptManager::HasFileChanged(std::filesystem::path filePath, std::string pastFileContents)
{
    std::string newFileContents = GetFileContents(filePath);
    return newFileContents != pastFileContents;
}
void PythonWalker::ScriptManager::ClearDirectory(std::filesystem::path directory)
{
    for (const auto& entry : std::filesystem::directory_iterator(directory))
    {
        std::filesystem::remove_all(entry.path());
    }
}
