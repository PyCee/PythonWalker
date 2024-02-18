#pragma once
#include <filesystem>

namespace PythonWalker {
	class Logging
	{
	public:
		static void StartLoggingContext(std::filesystem::path logPath);
		static void CloseLoggingContext();
		static void FlushLoggingContext();
	};
}
