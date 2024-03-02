// RunTest.cpp : This file contains the 'main' function. Program execution begins and ends there.
//

#include <iostream>
#include "../PythonWalker/PythonWalker.h"
#include "../UnitTests/CodeGenStrings.h"

class TestPyClass : public PythonWalker::ObjectInstance {
public:
    using PythonWalker::ObjectInstance::ObjectInstance;
    __PYW_TYPING_METHOD(void, printMessage, std::string, msg)
        __PYW_TYPING_METHOD(void, issueFunction)
};

//https://w3.pppl.gov/~hammett/comp/python/LLNLDistribution11/CXX/Doc/cxx.htm


/* An executable meant to play around with features outside of unit testing */
int main()
{
    std::cout << "Walking the python..." << std::endl;

    std::filesystem::path currentPath = std::filesystem::current_path();
    std::string currentPathString = currentPath.make_preferred().string();

    std::vector<std::string> paths = std::vector<std::string>({
        currentPathString + "\\..\\UnitTests\\UnitTestsPythonScripts",
        currentPathString
        });
    PythonWalker::Initialize();
    PythonWalker::ScriptManager scriptManager(paths);

    int gloVal = 3;
    PyObject* module = PythonWalker::Module::Load("TestModule");


    std::filesystem::path LogFilePath = currentPath / "LogFile.txt";

    PythonWalker::Logging::StartLoggingContext(LogFilePath);


    //PythonWalker::Module::SetGlobal<int>(module, "gloVal", gloVal);
    std::cout << PythonWalker::Module::GetGlobal<std::string>(module, "helloWorld") << std::endl;

    std::cout << PythonWalker::Module::GetGlobal<int>(module, "gloVar") << std::endl;
    PythonWalker::ExecuteFunction(module, "moduleFunctionMultplyGlobalByTwo");
    PythonWalker::ExecuteFunction(module, "moduleFunctionMultplyGlobalByTwo");
    //Assert::AreEqual(gloVal * 2, PythonWalker::Module::GetGlobal<int>(module, "gloVal"));
    std::cout << PythonWalker::Module::GetGlobal<int>(module, "gloVar") << std::endl;

    PythonWalker::Logging::FlushLoggingContext();

    std::string result = PythonWalker::GetFileContents(LogFilePath);
    std::cout << result << std::endl;

    PythonWalker::Logging::CloseLoggingContext();
    std::filesystem::remove(LogFilePath);



    //TODO better handling python errors
    //  1. Logging - Done
    //  2. Replaying back with object and environment variables set for debugging in user environment
    //      Would have to generate a script that the user can use to step through an issue
    // 
    //TODO Maybe support standalone module functions
    //  Not super important, but feels like I might as well
    //  Should do to support user writing functions without a whole class interface

    PythonWalker::CodeGeneration::DeletePythonModule(currentPath, "GeneratedFiles", true);

    std::cout << "Done walking the python." << std::endl;
}

// Run program: Ctrl + F5 or Debug > Start Without Debugging menu
// Debug program: F5 or Debug > Start Debugging menu

// Tips for Getting Started: 
//   1. Use the Solution Explorer window to add/manage files
//   2. Use the Team Explorer window to connect to source control
//   3. Use the Output window to see build output and other messages
//   4. Use the Error List window to view errors
//   5. Go to Project > Add New Item to create new code files, or Project > Add Existing Item to add existing code files to the project
//   6. In the future, to open this project again, go to File > Open > Project and select the .sln file
