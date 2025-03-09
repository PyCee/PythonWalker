using Python.Runtime;
using PythonWalker.Exceptions;
using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using System.Text.RegularExpressions;
using System.Threading.Tasks;

namespace PythonWalker
{
	public static class Logging
	{
		private static string CurrentLoggingPath = "";
		private static string TempLogFileName = "TemporaryLogFile.txt";
		private static string TempLogFilePath = Path.Combine(Environment.CurrentDirectory, TempLogFileName);
		public static void StartLoggingContext(string? path = "")
		{
			if (!string.IsNullOrEmpty(CurrentLoggingPath)){
				throw new PythonLoggingContextAlreadyOpen($"Trying to open '{path}' while '{CurrentLoggingPath}' is open");
			}

			if (string.IsNullOrEmpty(path))
			{
				path = TempLogFilePath;
			}
			CurrentLoggingPath = path;

			EnsureLogFileExists();

			dynamic sys = Py.Import("sys");

			PythonEngine.RunSimpleString("import sys");
			string openFileCommand = $"sys.stdout = sys.stderr = open(r'{CurrentLoggingPath}', 'a', buffering=1)";
			PythonEngine.RunSimpleString(openFileCommand);

		}
		private static void EnsureLogFileExists()
		{
			string parentDir = Directory.GetParent(CurrentLoggingPath)!.ToString();
			if (!Directory.Exists(parentDir))
			{
				Directory.CreateDirectory(parentDir);
			}
			if (!File.Exists(CurrentLoggingPath))
			{
				FileStream fs = File.Create(CurrentLoggingPath);
				fs.Close();
			}
		}
		public static void CloseLoggingContext(bool deleteLogFile = false)
		{
			if (!IsLogging())
			{
				return;
			}

			SetPythonSystemVar("sys.stdout", "sys.__stdout__");
			SetPythonSystemVar("sys.stderr", "sys.__stderr__");

			if (deleteLogFile || CurrentLoggingPath == TempLogFilePath)
			{
				File.Delete(CurrentLoggingPath);
			}
			CurrentLoggingPath = "";
		}
		public static void FlushLoggingContext()
		{
			PythonEngine.RunSimpleString("sys.stdout.flush()");
			PythonEngine.RunSimpleString("sys.stderr.flush()");
		}
		public static string ReadCurrentContentsOfLogFile()
		{
			// By default C# io doesn't allow file access sharing.
			// We want to read from the log here even if it's still open for logging.
			FileStreamOptions options = new()
			{
				Share = FileShare.ReadWrite
			};
			using StreamReader sr = new(CurrentLoggingPath, options);
			string result = sr.ReadToEnd();
			sr.Close();
			return result.Trim('\n').Trim('\r');
		}
		public static void LogException(Exception exception)
		{
			if (!IsLogging())
			{
				return;
			}
			string exceptionString = exception.ToString();
			string command = "sys.stderr.write(r'''" + exceptionString + "''')";
			PythonEngine.RunSimpleString(command);
		}
		public static string GetCurrentLogFile()
		{
			return CurrentLoggingPath;
		}
		public static bool IsLogging()
		{
			return !string.IsNullOrEmpty(CurrentLoggingPath);
		}
		private static void SetPythonSystemVar(string sysVar, string newVal)
		{
			PythonEngine.RunSimpleString("import sys");
			string command = sysVar + " = " + newVal;
			PythonEngine.RunSimpleString(command);
		}
	}
}
