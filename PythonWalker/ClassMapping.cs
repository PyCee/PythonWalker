using Python.Runtime;
using PythonWalker.Exceptions;
using System;
using System.Collections;
using System.Collections.Generic;
using System.Globalization;
using System.IO;
using System.Linq;
using System.Runtime.CompilerServices;
using System.Text;
using System.Threading.Tasks;

namespace PythonWalker
{
	/// <summary>
	/// The base type for a C# class that maps to a python class.
	/// This class probably shouldn't be directly referenced.
	/// See <see cref="PythonMappingAttribute" /> for usage.
	/// </summary>
	public abstract class ClassMapping
	{
		/// <summary>
		/// Reference to the underlying python object.
		/// </summary>
		protected PyObject pyObject;
		/// <summary>
		/// The script definition of the underlying python object.
		/// </summary>
		public ClassDefinition ClassDef;
		private string ModuleAbsolutePath;

		/// <summary>
		/// Watches the underlying python script for changes.
		/// https://learn.microsoft.com/en-us/dotnet/api/system.io.filesystemwatcher?view=net-8.0
		/// </summary>
		private readonly FileSystemWatcher ScriptWatcher;
		/// <summary>
		/// Flag for when the system detects a source code change.
		/// </summary>
		private bool SourceCodeIsOutOfDate;

		public ClassMapping()
		{
			pyObject = PyObject.None;
			ClassDef = new();
			ModuleAbsolutePath = string.Empty;
			ScriptWatcher = new();
			SourceCodeIsOutOfDate = false;

			Initialize();
		}
		public ClassMapping(string moduleName, string className) : this(new ClassDefinition(moduleName, className))
		{
		}
		public ClassMapping(ClassMapping otherClass) : this(otherClass.pyObject)
		{
		}
		public ClassMapping(ClassDefinition classDef) : this()
		{
			CreateFromClassDefinition(classDef);
		}
		public ClassMapping(PyObject otherPyObject) : this()
		{
			CopyFromPyObject(otherPyObject);
		}
		/// <summary>
		/// Creates a new instance of a python object from the class defined in the given class definition.
		/// </summary>
		/// <param name="classDef">Definition for the class this object should map to.</param>
		private void CreateFromClassDefinition(ClassDefinition classDef)
		{
			ClassDef = classDef;
			pyObject = ClassDef.GetPythonObject();
			ModuleAbsolutePath = PythonModule.GetAbsolutePath(ClassDef.ModuleName);
			SourceCodeIsOutOfDate = false;

			ScriptWatcher.Path = Path.GetDirectoryName(ModuleAbsolutePath)!;
			ScriptWatcher.Filter = Path.GetFileName(ModuleAbsolutePath);
			if (!ScriptWatcher.EnableRaisingEvents)
			{
				ScriptWatcher.NotifyFilter = NotifyFilters.LastWrite;
				ScriptWatcher.Changed += OnScriptChanged;
				ScriptWatcher.EnableRaisingEvents = true;
			}

			Initialize();
		}
		/// <summary>
		/// Event handler for when the underlying python script is changed.
		/// </summary>
		/// <param name="sender"></param>
		/// <param name="eventArgs"></param>
		private void OnScriptChanged(object sender, FileSystemEventArgs eventArgs)
		{
			if (eventArgs.ChangeType != WatcherChangeTypes.Changed || !File.Exists(eventArgs.FullPath))
			{
				return;
			}
			SourceCodeIsOutOfDate = true;
		}
		/// <summary>
		/// Duplicates the given python object into this one.
		/// </summary>
		/// <param name="otherPyObject">Object to duplicate.</param>
		private void CopyFromPyObject(PyObject otherPyObject)
		{
			CreateFromClassDefinition(new ClassDefinition(otherPyObject));

			PyObject dict = otherPyObject.GetAttr("__dict__");
			PyIter iter = dict.GetIterator();
			while (iter.MoveNext())
			{
				if (iter.Current == null)
				{
					continue;
				}
				string propertyName = iter.Current.ToString() ?? "";
				PyObject value = otherPyObject.GetAttr(propertyName);
				pyObject.SetAttr(propertyName, value);
			}
		}

		public abstract void Initialize();
		public static string GetPythonBaseCode() { return ""; }
		/// <summary>
		/// Retrieves the underlying python object.
		/// Mimics the "ToPython" method defined for standard C# types, for our mapping object.
		/// </summary>
		/// <returns>The underlying python object.</returns>
		public PyObject ToPython() => pyObject;

		public bool IsInitialized() { return pyObject != null; }
		/// <summary>
		/// Retrieves a property from the underlying python object.
		/// </summary>
		/// <typeparam name="T">Type of the property.</typeparam>
		/// <param name="propertyName">Name of the property on the underlying python object.</param>
		/// <returns>The properties value.</returns>
		/// <exception cref="PythonPropertyDoesNotExist"></exception>
		/// <exception cref="PyObjectConversionFailureException"></exception>
		public T GetMappedPythonProperty<T>(string propertyName)
		{
			if (!pyObject.HasAttr(propertyName))
			{
				throw new PythonPropertyDoesNotExist($"Property '{propertyName}' does not exist on {ClassDef}");
			}
			try
			{
				return Core.FromPyObjectToValue<T>(pyObject.GetAttr(propertyName));
			}
			catch (System.InvalidCastException e)
			{
				string message = $"Failed to retrieve property '{propertyName}' within {ClassDef}";
				throw new PyObjectConversionFailureException(message, e);
			}
		}
		/// <summary>
		/// Sets the given property on the underlying python object to the given value.
		/// </summary>
		/// <param name="propertyName">Name of the corresponding property on the python object. Case-sensitive.</param>
		/// <param name="propertyValue">Value to set the property to.</param>
		public void SetMappedPythonProperty(string propertyName, object propertyValue)
		{
			pyObject.SetAttr(propertyName, propertyValue.ToPython());
		}
		/// <summary>
		/// Run a python method on this object with the given method name, parameters, and return value.
		/// Will refresh the python object if the source code is out of date from the linked python script.
		/// </summary>
		/// <typeparam name="T">
		/// The return value of the python object.
		/// This may be any type that has a direct mapping between python and C#.
		/// The corresponding python code should return an object of this type.
		/// </typeparam>
		/// <param name="methodName">Name of the method within the corresponding python class. Case-sensitive.</param>
		/// <param name="parameters">The parameters to be passed into the python method.</param>
		/// <returns>The result of the python method call.</returns>
		/// <exception cref="PythonMethodDoesNotExist"></exception>
		/// <exception cref="Exception"></exception>
		/// <exception cref="PyObjectConversionFailureException"></exception>
		protected T RunMethod<T>(string methodName, params object?[] parameters)
		{
			RefreshPythonSourceIfNeeded();

			PyObject[] pyParameters = [];
			if (parameters != null)
			{
				pyParameters = new PyObject[parameters.Length];
				for(int i = 0; i < parameters.Length; i++)
				{
					pyParameters[i] = parameters[i].ToPython();
				}
			}
			if (!pyObject.HasAttr(methodName))
			{
				throw new PythonMethodDoesNotExist($"Attribute '{methodName}' does not exist on {ClassDef}");
			}
			PyObject methodObject = pyObject.GetAttr(methodName);

			if (!methodObject.IsCallable())
			{
				throw new Exception();
			}
			PyObject returnedPyObject;
			try
			{
				returnedPyObject = methodObject.Invoke(pyParameters);
			}
			catch(Python.Runtime.PythonException e)
			{
				Logging.LogException(e);
				throw;
			}

			try
			{
				return Core.FromPyObjectToValue<T>(returnedPyObject);
			}
			catch (System.InvalidCastException e)
			{
				string message = $"Failed to convert return value of method '{methodName}' within class '{ClassDef}'";
				throw new PyObjectConversionFailureException(message, e.InnerException);
			}
		}
		/// <summary>
		/// Checks if the python source code for this object is out of date, and refreshes the object if needed.
		/// </summary>
		private void RefreshPythonSourceIfNeeded()
		{
			if (!ShouldRefreshPythonSource())
			{
				return;
			}
			RefreshPythonSource();
		}
		/// <summary>
		/// Checks if the python source code that this object is based on is out of date.
		/// </summary>
		/// <returns>True if the source for this object is out of date.</returns>
		public bool ShouldRefreshPythonSource()
		{
			return SourceCodeIsOutOfDate;
		}
		/// <summary>
		/// Refresh the python code driving this object.
		/// </summary>
		private void RefreshPythonSource()
		{
			CopyFromPyObject(pyObject);
		}
		
	}
}
