// RunTest.cpp : This file contains the 'main' function. Program execution begins and ends there.
//

#include <iostream>
#include "../PythonWalker/PythonWalker.h"
#include "../UnitTests/CodeGenStrings.h"

class TestPyClass : public PythonWalker::ObjectInstance {
public:
    using PythonWalker::ObjectInstance::ObjectInstance;
    __PYW_TYPING_CLASS_METHOD(void, printMessage, std::string, msg)
        __PYW_TYPING_CLASS_METHOD(void, issueFunction)
};


class PyPosition : public PythonWalker::ObjectInstance {
public:
    using PythonWalker::ObjectInstance::ObjectInstance;
    PyPosition() : PythonWalker::ObjectInstance("TestSnake", "Position") {};
    PyPosition(double x, double y) : PyPosition() {
        X = x;
        Y = y;
    };
    __PYW_TYPING_VAR(double, X)
        __PYW_TYPING_VAR(double, Y)
        __PYW_TYPING_CLASS_METHOD(double, pythag)
};

class TestPythonClass : public PythonWalker::ObjectInstance {
public:
    using PythonWalker::ObjectInstance::ObjectInstance;
    TestPythonClass() : PythonWalker::ObjectInstance("TestSnake", "TestSnake") {};

    __PYW_TYPING_VAR(const char*, name)
        __PYW_TYPING_VAR(std::string, birthplace)
        __PYW_TYPING_VAR(long, length)
        __PYW_TYPING_VAR(int, age)
        __PYW_TYPING_VAR(double, terrariumWidth)
        __PYW_TYPING_VAR(double, terrariumDepth)
        __PYW_TYPING_VAR(float, terrariumHeight)
        __PYW_TYPING_VAR(bool, isCool)
        __PYW_TYPING_VAR(PyPosition, position)
        __PYW_TYPING_VAR(std::vector<const char*>, friends)

        __PYW_TYPING_VAR(int, fakeInt)

        __PYW_TYPING_CLASS_METHOD(void, increaseAgeByOne)
        __PYW_TYPING_CLASS_METHOD(void, functionThatHitsTypeError)
        __PYW_TYPING_CLASS_METHOD(double, calculateTerrariumArea)
        __PYW_TYPING_CLASS_METHOD(void, changeName, const char*, newName)
        __PYW_TYPING_CLASS_METHOD(double, AddThreeParametersTogether, int, a, float, b, double, c)

        // Inherently checks that we can overload function parameters like this,
        //   which end up calling into the same python function.
        __PYW_TYPING_CLASS_METHOD(double, AddNumbersTogether, std::vector<double>, numbers)
        __PYW_TYPING_CLASS_METHOD(double, AddNumbersTogether, std::list<double>, numbers)
        __PYW_TYPING_CLASS_METHOD(std::vector<const char*>, getFriends)
        __PYW_TYPING_CLASS_METHOD(std::vector<int>, countToFiveVector)

        __PYW_TYPING_CLASS_METHOD(int, getGlobalVar)
        __PYW_TYPING_CLASS_METHOD(void, setGlobalVar, int, value)
        __PYW_TYPING_CLASS_METHOD(void, fakeFunction)
        __PYW_TYPING_CLASS_METHOD(void, noParamFunction, int, fakeParam)
        __PYW_TYPING_CLASS_METHOD(void, printMessage, std::string, msg)
        __PYW_TYPING_CLASS_METHOD(double, GetPositionXIndirectly)
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
    /*
    int gloVal = 3;
    PyObject* module = PythonWalker::Module::Load("TestModule");

    PythonWalker::Logging::StartLoggingContext();

    std::cout << PythonWalker::Module::GetGlobal<std::string>(module, "helloWorld") << std::endl;

    std::cout << PythonWalker::Module::GetGlobal<int>(module, "gloVar") << std::endl;
    PythonWalker::ExecuteFunction(module, "moduleFunctionMultplyGlobalByTwo");
    PythonWalker::ExecuteFunction(module, "moduleFunctionMultplyGlobalByTwo");
    std::cout << PythonWalker::Module::GetGlobal<int>(module, "gloVar") << std::endl;

    PythonWalker::Logging::FlushLoggingContext();

    std::string result = PythonWalker::ScriptManager::GetFileContents(PythonWalker::Logging::GetCurrentLogFile());
    std::cout << result << std::endl;

    PythonWalker::Logging::CloseLoggingContext();
    */
    std::filesystem::path LogDirectory = currentPath / "Logs";
    std::filesystem::path LogFilePath = LogDirectory / "LogFile.txt";
    std::string LoggingMessage = "My unit testing message";
    TestPythonClass testSnake;
    bool test = (std::filesystem::exists(LogFilePath));

    PythonWalker::Logging::StartLoggingContext(LogFilePath);

    // Make sure the log file was created
    bool test2 = (std::filesystem::exists(LogFilePath));

    testSnake.printMessage(LoggingMessage);
    PythonWalker::Logging::FlushLoggingContext();

    std::string result = PythonWalker::ScriptManager::GetFileContents(LogFilePath);
    std::string expected = LoggingMessage + "\n";
    
    //TODO better handling python errors
    //  1. Logging - Done
    //  2. Replaying back with object and environment variables set for debugging in user environment
    //      Would have to generate a script that the user can use to step through an issue

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
