using Microsoft.CodeAnalysis;
using Python.Runtime;
using PythonWalker.Exceptions;
using System;
using System.Collections.Generic;
using System.IO;
using System.Linq;
using System.Text;
using System.Text.RegularExpressions;
using System.Threading.Tasks;
using System.Xml;

namespace PythonWalker
{
	public class ScriptManager
	{
		public List<string> ScriptPaths;
		public ScriptManager(List<string> scriptPaths)
		{
			ScriptPaths = [];

			dynamic sys = Py.Import("sys");
			foreach (string scriptPath in scriptPaths)
			{
				string fullScriptPath = Path.GetFullPath(scriptPath);
				if (!Directory.Exists(fullScriptPath))
				{
					throw new ScriptDirectoryDoesNotExist($"Directory '{fullScriptPath}' does not exist");
				}
				ScriptPaths.Add(fullScriptPath);
				sys.path.append(fullScriptPath);
			}
		}
		public List<ClassDefinition> GetScripts(ClassDefinition? requiredBaseClass = null, List<string>? excludeModules = null)
		{
			List<ClassDefinition> results = [];
			foreach (string scriptPath in ScriptPaths)
			{
				List<ClassDefinition> classDefinitions = GetPythonClassDefinitions(scriptPath, requiredBaseClass, excludeModules);
				results.AddRange(classDefinitions);
			}
			return results;
		}
		public static List<ClassDefinition> GetPythonClassDefinitions(string scriptPath, ClassDefinition? requiredBaseClass = null, List<string>? excludeModules = null)
		{
			List<ClassDefinition> results = [];

			IEnumerable<string> files = Directory.EnumerateFiles(scriptPath, "*.py", SearchOption.AllDirectories);
			foreach (string file in files)
			{
				string relativePath = Path.GetRelativePath(scriptPath, file);
				string moduleName = PythonModule.GetModuleNameFromRelativePath(relativePath);
				if(excludeModules?.Any(excludedModule => moduleName.Contains(excludedModule)) ?? false)
				{
					continue;
				}

				List<string> classNames = new PythonModule(moduleName).GetClassNamesFromModule(requiredBaseClass);

				List<ClassDefinition> fileDefs = classNames.Select(className => new ClassDefinition(moduleName, className)).ToList();
				results.AddRange(fileDefs);
			}
			return results;
		}
		/// <summary>
		/// Constructs the given path if it does not exist.
		/// </summary>
		/// <param name="path">The path to construct. Supports directory paths and file paths.</param>
		/// <returns>True if the path needed to be constructed. False if the path already existed.</returns>
		public static bool EnsurePathExists(string path)
		{
			if (!Path.Exists(path))
			{
				bool isFile = Path.GetExtension(path) != string.Empty;
                if (!isFile)
				{
					Directory.CreateDirectory(path);
				}
				else
				{
					string directory = Path.GetDirectoryName(path)!;
					Directory.CreateDirectory(directory);
					FileStream access = File.Create(path);
					access.Close();
				}
				return true;
			}
			return false;
		}
		public static bool HasScriptChanged(string module, string oldCode)
		{
			string path = PythonModule.GetAbsolutePath(module);
			string contents = File.ReadAllText(path);
			return contents != oldCode;
		}
	}
}
