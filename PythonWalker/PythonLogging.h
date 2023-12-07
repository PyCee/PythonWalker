#pragma once
#include <filesystem>
class PythonLogging
{
public:
	static void StartLoggingContext(std::filesystem::path logPath);
	static void CloseLoggingContext();
	static void FlushLoggingContext();
	
};

