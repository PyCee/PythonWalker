using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using System.Threading.Tasks;

namespace PythonWalker.Exceptions
{
	public class InitializationFailed : Exception { }
	public class InvokeNonCallable : Exception { }
	public class PythonLoggingContextAlreadyOpen(string message) : Exception(message) { }
	public class PythonPropertyDoesNotExist(string message) : Exception(message) { }
	public class PythonMethodDoesNotExist(string message) : Exception(message) { }

	public class ScriptDirectoryDoesNotExist(string message) : Exception(message)
	{
	}
	public class PyObjectConversionFailureException : Exception
	{
		public PyObjectConversionFailureException()
		{ }

		public PyObjectConversionFailureException(string message)
			: base(message)
		{ }

		public PyObjectConversionFailureException(string message, Exception? innerException)
			: base(message, innerException)
		{ }
	}
}
