#include "Logging.h"

#include <fstream>
#include <cstdio>
#include <cstdlib>
#include "PythonWalker.h"

// TODO: error text https://stackoverflow.com/questions/1418015/how-to-get-python-exception-text
static std::string tempLogFileName = "TempLogFile";
static std::filesystem::path tempLogFilePath = std::filesystem::current_path() / tempLogFileName;
static std::filesystem::path currentLogPath;
static void SetPythonSystemVar(std::string sysVar, std::string newVal)
{
	PyRun_SimpleString("import sys");
	std::string command = sysVar + " = " + newVal;
	PyRun_SimpleString(command.c_str());
}
void PythonWalker::Logging::StartLoggingContext(std::filesystem::path path)
{
	if (GetCurrentLogFile().string().length() > 0) {
		return;
	}

	currentLogPath = path;
	if (currentLogPath == "") {
		currentLogPath = tempLogFilePath;
	}
	std::string openFileString = "open(r\"" + currentLogPath.string() + "\", 'a')";
	SetPythonSystemVar("sys.stdout", openFileString);
	SetPythonSystemVar("sys.stderr", openFileString);
}

void PythonWalker::Logging::CloseLoggingContext(bool deleteLogFile)
{
	SetPythonSystemVar("sys.stdout", "sys.__stdout__");
	SetPythonSystemVar("sys.stderr", "sys.__stderr__");
	if (deleteLogFile || currentLogPath == tempLogFilePath) {
		std::filesystem::remove(currentLogPath);
	}
	currentLogPath = "";
}

void PythonWalker::Logging::FlushLoggingContext()
{
	PyRun_SimpleString("sys.stdout.flush()");
	PyRun_SimpleString("sys.stderr.flush()");
}

std::filesystem::path PythonWalker::Logging::GetCurrentLogFile()
{
	return currentLogPath;
}
