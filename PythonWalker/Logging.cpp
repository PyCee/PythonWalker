#include "Logging.h"

#include <fstream>
#include "PythonWalker.h"
#include <cstdio>
#include <cstdlib>
#include "FileManager.h"

// TODO: error text https://stackoverflow.com/questions/1418015/how-to-get-python-exception-text

static void SetPythonSystemVar(std::string sysVar, std::string newVal)
{
	PyRun_SimpleString("import sys");
	std::string command = sysVar + " = " + newVal;
	PyRun_SimpleString(command.c_str());
}
void PythonWalker::Logging::StartLoggingContext(std::filesystem::path path)
{
	std::string openFileString = "open(r\"" + path.string() + "\", 'a')";
	SetPythonSystemVar("sys.stdout", openFileString);
	SetPythonSystemVar("sys.stderr", openFileString);
}

void PythonWalker::Logging::CloseLoggingContext()
{
	SetPythonSystemVar("sys.stdout", "sys.__stdout__");
	SetPythonSystemVar("sys.stderr", "sys.__stderr__");
}

void PythonWalker::Logging::FlushLoggingContext()
{
	PyRun_SimpleString("sys.stdout.flush()");
	PyRun_SimpleString("sys.stderr.flush()");
}
