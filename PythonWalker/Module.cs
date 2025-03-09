using Microsoft.CodeAnalysis.Operations;
using Python.Runtime;
using System;
using System.Collections.Generic;
using System.Linq;
using System.Runtime.CompilerServices;
using System.Text;
using System.Threading.Tasks;

namespace PythonWalker
{
	public class PythonModule
	{
		/// <summary>
		/// Caches the full path to each module in the user's filesystem.
		/// </summary>
		private static readonly Dictionary<string, string> ModulePaths = [];
		public string ModuleName;
		public PyObject pyObject;
		public PythonModule(): this("") { }
		public PythonModule(string moduleName) 
		{
			ModuleName = moduleName;
			pyObject = Py.Import(ModuleName);
			string path = pyObject.GetAttr("__file__").ToString()!;
			ModulePaths.TryAdd(moduleName, path);
		}
		public PyObject ToPython() => pyObject;
		/// <summary>
		/// Invokes the given callable from the module.
		/// </summary>
		/// <param name="callableName">The name of the callable.</param>
		/// <returns>The python object returned by the callable.</returns>
		/// <exception cref="Exceptions.InvokeNonCallable"></exception>
		public PyObject Invoke(string callableName)
		{
			PyObject callable = pyObject.GetAttr(callableName);
			if (!callable.IsCallable())
			{
				throw new Exceptions.InvokeNonCallable();
			}
			return callable.Invoke();
		}
		/// <summary>
		/// Retrieves the underlying python classes defined in this module.
		/// </summary>
		/// <param name="requiredBaseClass">If given, this method will only return classes that inherit from this class.</param>
		/// <returns>The classes found in this module.</returns>
		public List<string> GetClassNamesFromModule(ClassDefinition? requiredBaseClass = null)
		{
			List<string> results = [];
			PyObject dict = pyObject.GetAttr("__dict__");
			
			PyIter iter = dict.GetIterator();
			while (iter.MoveNext())
			{
				PyObject dictElement = dict.GetItem(iter.Current);
				if (!PyType.IsType(dictElement) ||
					!MatchesModule(dictElement))
				{
					continue;
				}
				string className = iter.Current.ToString() ?? "";
				ClassDefinition def = new(ModuleName, className);
				if(requiredBaseClass != null)
				{
					if(!dictElement.IsSubclass(requiredBaseClass.GetPythonClass()) || def.Equals(requiredBaseClass)) { continue; }
				}

				results.Add(className);
			}
			return results;
		}
		/// <summary>
		/// Checks if the given python object is defined within this module.
		/// This will return false for objects imported into this module.
		/// </summary>
		/// <param name="testObject">The python object to check.</param>
		/// <returns>True if the python object is found within this object. False otherwise.</returns>
		public bool MatchesModule(PyObject testObject)
		{
			return ClassDefinition.GetModuleName(testObject) == ModuleName;
		}
		#region Static Methods
		/// <summary>
		/// Reloads the module from its corresponding python source.
		/// </summary>
		/// <param name="moduleName">Module to reload.</param>
		public static void Reload(string moduleName)
		{ 
			PythonEngine.RunSimpleString("from importlib import reload");
			PythonEngine.RunSimpleString($"import {moduleName}");
			PythonEngine.RunSimpleString($"reload({moduleName})");
		}
		/*
		private static void DeletePyCache(string moduleName)
		{
			string path = GetAbsolutePath(moduleName);
			string parentPath = Directory.GetParent(path)!.ToString();
			string pyCacheDirectory = Path.Combine(parentPath, "__pycache__");
			if (Directory.Exists(pyCacheDirectory))
			{
				Directory.Delete(pyCacheDirectory, true);
			}
		}
		*/
		/// <summary>
		/// Converts the given module string into a list of the individual module pieces.
		/// </summary>
		/// <param name="module">A python module. Example: "Test.TestTwo.TestThree"</param>
		/// <returns>A list of the module pieces.</returns>
		public static List<string> ParseModuleStringList(string module)
		{
			return [.. module.Split(".")];
		}
		/// <summary>
		/// 
		/// </summary>
		/// <param name="moduleNames">A list of module pieces.</param>
		/// <returns>A python module. Example: "Test.TestTwo.TestThree"</returns>
		public static string ConstructModuleStringList(List<string> moduleNames)
		{
			return string.Join(".", moduleNames);
		}
		public static string GetModuleNameFromRelativePath(string relativePath)
		{
			return Path.ChangeExtension(relativePath, null).Replace(Path.DirectorySeparatorChar, '.').Trim('.');
		}
		public static string GetRelativePathFromModuleName(string moduleName)
		{
			string filePath = moduleName.Replace('.', Path.DirectorySeparatorChar);
			return Path.ChangeExtension(filePath, "py");
		}
		public static string GetAbsolutePath(string moduleName, bool justBase = false)
		{
			if (!ModulePaths.TryGetValue(moduleName, out string? result))
			{
				string moduleRelativePath = GetRelativePathFromModuleName(moduleName);
				result = Path.GetFullPath(moduleRelativePath);
			}
			if (justBase)
			{
				List<string> modules = ParseModuleStringList(moduleName);
				DirectoryInfo? directory = new(result);
				for (int i = 0; i < modules.Count; i++)
				{
					directory = directory?.Parent;
				}
				if (directory == null)
				{
					return "";
				}
				result = directory.FullName;
			}
			return result;
		}
		public static string TrimLastModule(string moduleName)
		{
			List<string> modules = ParseModuleStringList(moduleName)[0..^1];
			return ConstructModuleStringList(modules);
		}
		#endregion
	}
}
