using Microsoft.CodeAnalysis;
using Microsoft.CodeAnalysis.CSharp.Syntax;
using Microsoft.CodeAnalysis.Text;
using Python.Runtime;
using PythonWalker;
using System;
using System.Collections.Immutable;
using System.Globalization;
using System.Text;

namespace TestWalk
{
	[PythonMapping(DefaultModule = "TestSnake", DefaultClass = "TestSnake")]
	public partial class TestSnake
	{
		[PythonProperty]
		public int _Age { get; set; }
		[PythonProperty]
		public string _Name { get; set; }

		[PythonMethod]
		public partial void IncreaseAgeByOne();

		[PythonMethod]
		public partial int ReturnIntSeven();

		[PythonMethod]
		public partial void SetName(string name);

		[PythonMethod]
		public partial void HitPythonTypeError();
	}
	internal class ManualTesting
	{
		static void Main(string[] args)
		{
			PythonWalker.Core.Initialize("python312.dll");

			string currentPath = Environment.CurrentDirectory;
			List<string> scriptPaths =
			[
				currentPath,
				currentPath + @"\..\..\..\..\PythonWalkerTests\PythonScripts"
			];
			ScriptManager sm = new (scriptPaths);

			TestSnake testSnake = new();
		}
	}
}