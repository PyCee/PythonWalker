#include "PythonFileManagement.h"
#include <filesystem>
#include <iostream>
#include <fstream>
#include "PythonWalker.h"
#include "PythonHelper.h"


std::vector<PythonClassDefinition> PythonFileManagement::GetPythonClasses(const char* filepath, bool recursive,
    std::string extendedClassName, std::string modulePrepend)
{
    std::vector<PythonClassDefinition> results;
    for (const std::filesystem::directory_entry& entry : std::filesystem::directory_iterator(filepath)) {
        std::string stem = entry.path().stem().string();
        std::string moduleName = !modulePrepend.empty() ? modulePrepend + "." + stem : stem;

        if (entry.is_directory()) {
            if (recursive)
            {
                std::vector<PythonClassDefinition> recursiveResults = GetPythonClasses(entry.path().string().c_str(),
                    recursive, extendedClassName, moduleName);
                results.insert(results.end(), recursiveResults.begin(), recursiveResults.end());
            }
            continue;
        }
        if (!IsPythonScript(entry)) {
            continue;
        }
        std::vector<PythonClassDefinition> moduleResults = PythonWalker::GetClassDefinitionsFromModuleDict(moduleName.c_str());
        results.insert(results.end(), moduleResults.begin(), moduleResults.end());
    }
    return results;
}
bool PythonFileManagement::IsPythonScript(std::filesystem::directory_entry entry)
{
    return entry.path().extension() == ".py";
}

std::ofstream PythonFileManagement::CreateFile(std::filesystem::path path, bool immediatlyClose)
{
    std::filesystem::create_directories(path.parent_path());
    std::ofstream newFile(path.string(), std::ios::app);
    if (immediatlyClose) {
        newFile.close();
    }
    return newFile;
}
void PythonFileManagement::ClearDirectory(std::filesystem::path directory)
{
    for (const auto& entry : std::filesystem::directory_iterator(directory))
    {
        std::filesystem::remove_all(entry.path());
    }
}