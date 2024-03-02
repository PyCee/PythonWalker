#pragma once
#include <filesystem>

namespace PythonWalker {
	class Logging
	{
	public:
		/* Redirects standard python outputs to the given log file. Creates file if it doesn't exist.
			If no file is passed in, will use a standard temporary file that is deleted when the logging context is closed.
			@param logPath - Path of the logging file.
			@return Returns the path to the log file
		*/
		static void StartLoggingContext(std::filesystem::path logPath="");
		/* Resets python standard outputs to their defaults. If using a temporary log file, deletes the file */
		static void CloseLoggingContext();
		static void FlushLoggingContext();
		static std::filesystem::path GetCurrentLogFile();
	};
}
