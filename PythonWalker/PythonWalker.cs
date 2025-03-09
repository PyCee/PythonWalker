
using Python.Runtime;
using PythonWalker.Exceptions;
using System.Runtime.InteropServices;

namespace PythonWalker
{
	public static class Core
	{
		public static void Initialize(string pythonDLL)
		{
			Runtime.PythonDLL = pythonDLL;

			PythonEngine.Initialize();
			if (!PythonEngine.IsInitialized)
			{
				throw new InitializationFailed();
			}
		}
		public static void Destroy()
		{
			if(PythonEngine.IsInitialized)
			{
				PythonEngine.Shutdown();
			}
		}
		/// <summary>
		/// Converts a python object to the given C# type.
		/// </summary>
		/// <typeparam name="T">The conversion type.</typeparam>
		/// <param name="pythonObject">The python object that references the value we want to convert.</param>
		/// <returns>The converted value.</returns>
		public static T FromPyObjectToValue<T>(PyObject pythonObject)
		{
			return pythonObject.As<T>();
		}
	}
}
