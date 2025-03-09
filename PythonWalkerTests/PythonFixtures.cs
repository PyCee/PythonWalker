using Python.Runtime;
using PythonWalker;
using System;
using System.Collections.Generic;
using System.Globalization;
using System.Linq;
using System.Text;
using System.Threading.Tasks;

namespace PythonWalkerTests
{

	public class PythonFixture : IDisposable
	{
		public TestSnake testSnake;
		public ScriptManager scriptManager;
		public PythonFixture()
		{
			PythonWalker.Core.Initialize("python312.dll");

			string currentPath = Environment.CurrentDirectory;
			List<string> scriptPaths =
			[
				currentPath,
				currentPath + @"\..\..\..\PythonScripts"
			];
			scriptManager = new(scriptPaths);
			testSnake = new();
		}
		public void Dispose()
		{
			GC.SuppressFinalize(this);
			//PythonWalker.PythonWalker.Destroy();
		}
	}

	[CollectionDefinition(nameof(PythonFixtureCollection))]
	public class PythonFixtureCollection : ICollectionFixture<PythonFixture> { }



	[PythonMapping(DefaultModule = "TestSnake", DefaultClass = "TestSnake")]
	public partial class TestSnake
	{
		
		// Can't make a partial property like this until C# 13
		[PythonProperty]
		public /* partial */ int _Age { get; set; }

		[PythonProperty]
		public string _Name { get; set; }

		[PythonProperty]
		public long _Length { get; set; }

		[PythonProperty]
		public bool _IsCool { get; set; }
		[PythonProperty]
		public float _TerrariumDepth { get; set; }
		[PythonProperty]
		public double _TerrariumWidth { get; set; }


		[NestedPythonProperty]
		public PyPosition Position { get; set; }

		[PythonProperty]
		public List<string> _Friends { get; set; }

		[PythonProperty]
		public int _PropertyThatDoesntExist { get; set; }
		[PythonMethod]
		public partial void MethodThatDoesntExist();

		[PythonMethod]
		public partial void NoParamFunction(int fakeParam);

		[PythonMethod]
		public partial void IncreaseAgeByOne();

		[PythonMethod]
		public partial int ReturnIntSeven();

		[PythonMethod]
		public partial void SetName(string name);

		[PythonMethod]
		public partial int AddThreeParametersTogether(int a, int b, int c);
		[PythonMethod]
		public partial int GetGlobalVar();
		[PythonMethod]
		public partial void SetGlobalVar(int value);

		[PythonMethod]
		public partial int AddNumbersTogether(List<int> numbers);
		[PythonMethod]
		public partial int[] CountToFive();
		[PythonMethod]
		public partial void PythonPrintMessage(string message);
		[PythonMethod]
		public partial void HitPythonTypeError();
		[PythonMethod]
		public partial void MethodWithPass();
	}

	[PythonMapping(DefaultModule ="TestSnake", DefaultClass ="Position")]
	public partial class PyPosition
	{
		public PyPosition(float x, float y): this()
		{
			X = x; Y = y;
		}

		[PythonProperty]
		public float _X { get; set; }
		[PythonProperty]
		public float _Y { get; set; }

		[PythonMethod]
		public partial float Pythag();

	}
	[PythonMapping(DefaultModule = "Generated.BaseRobotClass")]
	public partial class Robot
	{
		[PythonMethod]
		public partial float Divide(float divisor, float dividend);
		[PythonMethod]
		public partial void Beep();
	}
	public static class RobotGeneration
	{

		public static readonly string RobotTwoCodeString = @"
from Generated.BaseRobotClass import Robot
class RobotSubclassTwo(Robot) :
	def Divide(self, dividend, divisor):
		return 2
";

		public static readonly string RobotSubclassCodeBrokenString = @"
from Generated.BaseRobotClass import Robot
class RobotSubclass(Robot) :
	def __init__(self):
		self.age = 0
	def Divide(self, dividend, divisor):
		return -1
";
		public static readonly string RobotSubclassCodeWorkingString = @"
from Generated.BaseRobotClass import Robot
class RobotSubclass(Robot) :
	def __init__(self):
		self.age = 0
	def Divide(self, dividend, divisor):
		if divisor == 0:
			return 0.0
		return dividend / divisor
";
	}

}
