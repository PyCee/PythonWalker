
using Newtonsoft.Json.Linq;
using Python.Runtime;
using PythonWalker;
using PythonWalker.Exceptions;
using System.IO;
using System.Text;
using System.Text.RegularExpressions;

namespace PythonWalkerTests
{

	[Collection(nameof(PythonFixtureCollection))]
	public class ModuleInterface
	{
		[Fact]
		public void LoadModule()
		{
			PythonModule module = new("TestModule");
			Assert.NotNull(module.ToPython());
		}
		[Fact]
		public void GetModuleGlobal()
		{
		}

		[Fact]
		public void SetModuleGlobal()
		{
		}
		[Fact]
		public void ModifyModuleGlobalViaFunction()
		{
		}

		[Fact]
		public void RunModuleFunctionWithReturn()
		{
		}

		[Fact]
		public void RunModuleFunctionWithParameters()
		{
		}
		/*
		TODO
		Module interface.
		attribute for module globals and module functions.
		Extend the PythonModule class (maybe rename to ModuleMapping?)
		Maybe a shared common class between ClassMapping and ModuleMapping for general
		"has a PyObject, has functions for getattr and setattr and invoke, and script watcher"
		*/
	}

	[Collection(nameof(PythonFixtureCollection))]
	public class ObjectInterface
	{
		[Fact]
		public void InitializeObject()
		{
			TestSnake testSnake = new();
			Assert.True(testSnake.IsInitialized());
		}
		[Fact]
		public void InitializeObjectFromSubmodule()
		{
			TestSnake testSnake = new("Subfolder.TestOregano", "TestOregano");
			Assert.True(testSnake.IsInitialized());
		}
		[Fact]
		public void InitializeObjectOverrideClass()
		{
			TestSnake testSnake = new("TestBasil", "TestBasil");
			Assert.True(testSnake.IsInitialized());
		}
		[Fact]
		public void GetAndSetClassDefinition()
		{
			ClassDefinition original = new("TestPlant", "TestPlant");
			PyObject obj = original.GetPythonObject();
			ClassDefinition result = new(obj);
			Assert.Equal(original, result);
		}
	}
	[Collection(nameof(PythonFixtureCollection))]
	public class PropertyRetrieval(PythonFixture pyFixtureReference)
	{
		readonly TestSnake testSnake = pyFixtureReference.testSnake;

		[Theory]
		[InlineData(1)]
		[InlineData(0)]
		[InlineData(-1)]
		[InlineData(int.MaxValue)]
		[InlineData(int.MinValue)]
		public void SetAndGetInt(int value)
		{
			testSnake.Age = value;
			Assert.Equal(value, testSnake.Age);
		}
		[Theory]
		[InlineData("Snakels")]
		[InlineData("")]
		[InlineData("123")]
		[InlineData("true")]
		[InlineData("1.5f")]
		public void SetAndGetString(string value)
		{
			testSnake.Name = value;
			Assert.Equal(value, testSnake.Name);
		}
		[Theory]
		[InlineData(7)]
		[InlineData(long.MaxValue)]
		[InlineData(long.MinValue)]
		public void SetAndGetLong(long value)
		{
			testSnake.Length = value;
			Assert.Equal(value, testSnake.Length);
		}
		[Theory]
		[InlineData(true)]
		[InlineData(false)]
		public void SetAndGetBool(bool value)
		{
			testSnake.IsCool = value;
			Assert.Equal(value, testSnake.IsCool);
		}
		[Theory]
		[InlineData(1.0f)]
		[InlineData(3.25f)]
		[InlineData(0.0f)]
		[InlineData(-1.0f)]
		[InlineData(float.MaxValue)]
		[InlineData(float.MinValue)]
		public void SetAndGetFloat(float value)
		{
			testSnake.TerrariumDepth = value;
			Assert.Equal(value, testSnake.TerrariumDepth);
		}

		[Theory]
		[InlineData(1.0)]
		[InlineData(3.25)]
		[InlineData(0.0)]
		[InlineData(-1.0)]
		[InlineData(double.MaxValue)]
		[InlineData(double.MinValue)]
		public void SetAndGetDouble(double value)
		{
			testSnake.TerrariumWidth = value;
			Assert.Equal(value, testSnake.TerrariumWidth);
		}

		[Fact]
		public void SetAndGetList()
		{
			List<string> friends =
			[
				"Elle", "Issac", "John"
			];
			testSnake.Friends = friends;

			foreach(string friend in friends)
			{
				Assert.Contains(friend, testSnake.Friends);
			}
		}
		[Fact]
		public void ListElementCanBeAppended()
		{
			testSnake.Friends =
			[
				"Elle", "Issac", "John"
			];
			string snakeFriend = "Hector";

			testSnake.Friends.Add(snakeFriend);
			Assert.Contains(snakeFriend, testSnake.Friends);
		}

		[Fact]
		public void ListElementCanBeShortened()
		{
			string notSnakeFriend = "Noodle";
			testSnake.Friends =
			[
				"Elle", "Issac", "John", notSnakeFriend
			];
			Assert.Contains(notSnakeFriend, testSnake.Friends);
			testSnake.Friends.Remove(notSnakeFriend);
			Assert.DoesNotContain(notSnakeFriend, testSnake.Friends);
		}
		[Fact]
		public void ListElementCanBeModified()
		{
			string snakeFriend = "Hector";
			string notSnakeFriend = "Noodle";
			testSnake.Friends =
			[
				"Elle", "Issac", "John", notSnakeFriend
			];

			Assert.DoesNotContain(snakeFriend, testSnake.Friends);
			Assert.Contains(notSnakeFriend, testSnake.Friends);

			int index = testSnake.Friends.IndexOf(notSnakeFriend);
			testSnake.Friends[index] = snakeFriend;
			Assert.Contains(snakeFriend, testSnake.Friends);
			Assert.DoesNotContain(notSnakeFriend, testSnake.Friends);
		}

		[Fact]
		public void ListAsParameter()
		{
			List<int> numbers =
			[
				1, 2, 4, 8, 16, 32
			];
			int expected = 0;
			foreach (int number in numbers) { expected += number; }
			int result = testSnake.AddNumbersTogether(numbers);
			Assert.Equal(expected, result);
		}

		[Fact]
		public void ListAsReturnValue()
		{
			int[] expected =
			[
				1, 2, 3, 4, 5
			];
			int[] result = testSnake.CountToFive();
			Assert.Equal(expected, result);
		}

		[Fact]
		public void ObjectIndependence()
		{
			TestSnake testSnake1 = new();
			TestSnake testSnake2 = new();
			int age1 = 1, age2 = 2;
			testSnake1.Age = age1;
			testSnake2.Age = age2;
			Assert.Equal(age1, testSnake1.Age);
			Assert.Equal(age2, testSnake2.Age);
			Assert.NotEqual(testSnake1.Age, testSnake2.Age);
		}

		[Fact]
		public void GlobalIsSharedBetweenObjectInstances()
		{
			TestSnake testSnake1 = new();
			TestSnake testSnake2 = new();
			int gloVal1 = 1, gloVal2 = 2;

			testSnake1.SetGlobalVar(gloVal1);
			Assert.Equal(gloVal1, testSnake1.GetGlobalVar());
			Assert.Equal(gloVal1, testSnake2.GetGlobalVar());

			testSnake2.SetGlobalVar(gloVal2);
			Assert.Equal(gloVal2, testSnake1.GetGlobalVar());
			Assert.Equal(gloVal2, testSnake2.GetGlobalVar());
		}
		[Fact]
		public void GetClassDefinitionFromPyObject()
		{
			PyPosition pos = new();
			ClassDefinition def = new(pos.ToPython());
			Assert.Equal(pos.ClassDef, def);
		}
		[Fact]
		public void KeepClassDefThroughAssignment()
		{
			PyPosition pos = new();
			testSnake.Position = new(pos);
			Assert.Equal(pos.ClassDef, testSnake.Position.ClassDef);
		}

		[Fact]
		public void SetAndGetNestedObjects()
		{
			float x = 1f, y = 2f;
			PyPosition pos = new(x, y);

			testSnake.Position = pos;

			Assert.Equal(x, testSnake.Position.X);
			Assert.Equal(y, testSnake.Position.Y);
		}
		[Fact]
		public void SetAndGetNestedObjectsCopiedByValueNotReference()
		{
			float x = 1f, y = 2f, x2 = 3.14159f, y2 = 4.28f;
			PyPosition pos = new(x, y);
			testSnake.Position = new(pos);

			pos.X = x2;
			pos.Y = y2;

			Assert.NotEqual(pos.X, testSnake.Position.X);
			Assert.NotEqual(pos.Y, testSnake.Position.Y);
			Assert.Equal(x, testSnake.Position.X);
			Assert.Equal(y, testSnake.Position.Y);
			Assert.Equal(x2, pos.X);
			Assert.Equal(y2, pos.Y);
		}
		[Fact]
		public void NestedObjectsNotNull()
		{
			TestSnake testSnake1 = new();

			Assert.NotNull(testSnake1.Position);
		}
		[Fact]
		public void CopyObjectNestedObjectsCopiedByValueNotReference()
		{
			TestSnake testSnake1 = new();
			TestSnake testSnake2;
			float x = 1f, y = 2f, x2 = 3.14159f, y2 = 4.28f;

			testSnake1.Position = new(x, y);
			testSnake2 = new(testSnake1);

			testSnake1.Position.X = x2;
			testSnake1.Position.Y = y2;

			Assert.NotEqual(testSnake1.Position.X, testSnake2.Position.X);
			Assert.NotEqual(testSnake1.Position.Y, testSnake2.Position.Y);
			Assert.Equal(x, testSnake2.Position.X);
			Assert.Equal(y, testSnake2.Position.Y);
			Assert.Equal(x2, testSnake1.Position.X);
			Assert.Equal(y2, testSnake1.Position.Y);
		}
		[Fact]
		public void NestedObjectMethod()
		{
			float x = 1f, y = 2f;
			testSnake.Position = new(x, y);

			float expected = (float)Math.Sqrt(x * x + y * y);
			float result = testSnake.Position.Pythag();
			Assert.Equal(expected, result);
		}
	}
	[Collection(nameof(PythonFixtureCollection))]
	public class ObjectMethods(PythonFixture pyFixtureReference)
	{
		readonly TestSnake testSnake = pyFixtureReference.testSnake;

		[Fact]
		public void ReturnValue()
		{
			int expected = 7;
			int result = testSnake.ReturnIntSeven();
			Assert.Equal(expected, result);
		}

		[Fact]
		public void Parameter()
		{
			string newName = "New Snakey Name";
			testSnake.SetName(newName);
			Assert.Equal(newName, testSnake.Name);
		}
		[Fact]
		public void MultipleParameters()
		{
			int a = 1, b = 2, c = 4;
			int expected = a + b + c;
			int result = testSnake.AddThreeParametersTogether(a, b, c);
			Assert.Equal(expected, result);
		}
		[Fact]
		public void MethodWithNoDefinitionRuns()
		{
			testSnake.MethodWithPass();
		}

	}
	[Collection(nameof(PythonFixtureCollection))]
	public class Exceptions(PythonFixture pyFixtureReference)
	{
		private readonly TestSnake testSnake = pyFixtureReference.testSnake;
		[Fact]
		public void InvokeNonCallable()
		{
			PythonModule mod = new("TestModule");
			Assert.Throws<InvokeNonCallable>(() => mod.Invoke("helloWorld"));
		}

		[Fact]
		public void FakeMethod()
		{
			Assert.Throws<PythonMethodDoesNotExist>(() => testSnake.MethodThatDoesntExist());
		}
		[Fact]
		public void FakeMethodParameters()
		{
			Assert.Throws<Python.Runtime.PythonException>(() => testSnake.NoParamFunction(4));
		}
	}

	[Collection(nameof(PythonFixtureCollection))]
	public class FileManagement(PythonFixture pyFixtureReference)
	{
		readonly ScriptManager scriptManager = pyFixtureReference.scriptManager;

		const string PlantDefinitionModule = "TestPlant";
		const string PlantDefinitionClass = "TestPlant";
		readonly ClassDefinition PlantDefinition = new(PlantDefinitionModule, PlantDefinitionClass);
		readonly ClassDefinition DuplicatePlantDefinitionFromSecondModule = new("TestBasil", PlantDefinitionClass);
		const string BasilDefinitionModule = "TestBasil";
		const string BasilDefinitionClass = "TestBasil";
		readonly ClassDefinition BasilDefinition = new(BasilDefinitionModule, BasilDefinitionClass);
		readonly ClassDefinition ThaiBasilDefinition = new("TestThaiBasil", "TestThaiBasil");
		const string PetDefinitionModule = "TestPet";
		const string CatDefinitionClass = "TestCat";
		const string DogDefinitionClass = "TestDog";
		readonly ClassDefinition CatDefinition = new(PetDefinitionModule, CatDefinitionClass);
		readonly ClassDefinition DogDefinition = new(PetDefinitionModule, DogDefinitionClass);
		readonly ClassDefinition OreganoDefinition = new("Subfolder.TestOregano", "TestOregano");
		readonly ClassDefinition MexicanOreganoDefinition = new("Subfolder.TestMexicanOregano", "TestMexicanOregano");
		readonly ClassDefinition OreganoDefinitionWithoutSubfolder = new("TestOregano", "TestOregano");
		readonly ClassDefinition RosemaryDefinition = new("Subfolder.Subfolder2.TestRosemary", "TestRosemary");

		readonly ClassDefinition MushroomDefinition = new("Subfolder.TestMushroom", "TestMushroom");
		readonly ClassDefinition OysterMushroomDefinition = new("TestOysterMushroom", "TestOysterMushroom");
		//readonly string NonExistantClassName = "NonExistantClassName";


		[Theory]
		[InlineData("TestBasil", "TestBasil")]
		[InlineData("TestBasil.py", "TestBasil")]
		[InlineData("PythonScripts\\TestBasil.py", "PythonScripts.TestBasil")]
		[InlineData("PythonScripts\\TestBasil", "PythonScripts.TestBasil")]
		[InlineData("PythonScripts\\TestBasil\\", "PythonScripts.TestBasil")]
		public void GetModuleNameFromString(string relativePath, string expected)
		{
			string result = PythonModule.GetModuleNameFromRelativePath(relativePath);
			Assert.Equal(expected, result);
		}
		[Theory]
		[InlineData(PlantDefinitionModule, PlantDefinitionClass)]
		[InlineData(BasilDefinitionModule, BasilDefinitionClass)]
		[InlineData(PetDefinitionModule, CatDefinitionClass)]
		[InlineData(PetDefinitionModule, DogDefinitionClass)]
		public void GetClassNamesFromModule(string moduleName, string expectedContainingClassName)
		{
			PythonModule mod = new(moduleName);
			List<string> classNames = mod.GetClassNamesFromModule();
			Assert.Contains(expectedContainingClassName, classNames);
		}
		[Theory]
		[InlineData("TestBasil", "")]
		[InlineData("PythonScripts.TestBasil", "PythonScripts")]
		[InlineData("PythonScripts.Subfolder.TestOregano", "PythonScripts.Subfolder")]
		public void TrimLastModule(string module, string expected)
		{
			string result = PythonModule.TrimLastModule(module);
			Assert.Equal(expected, result);
		}
		[Fact]
		public void FindsClassDefinitions()
		{
			List<ClassDefinition> defs = scriptManager.GetScripts();
			Assert.Contains(PlantDefinition, defs);
		}
		[Fact]
		public void FindClassesNoImportDuplicates()
		{
			List<ClassDefinition> defs = scriptManager.GetScripts();
			Assert.Contains(PlantDefinition, defs);
			Assert.DoesNotContain(DuplicatePlantDefinitionFromSecondModule, defs);
		}
		[Fact]
		public void FindTwoClassesInOneModule()
		{
			List<ClassDefinition> defs = scriptManager.GetScripts();
			Assert.Contains(CatDefinition, defs);
			Assert.Contains(DogDefinition, defs);
		}

		[Fact]
		public void FindClassesInSubdirectories()
		{
			List<ClassDefinition> defs = scriptManager.GetScripts();
			Assert.Contains(OreganoDefinition, defs);
			Assert.Contains(RosemaryDefinition, defs);
		}

		[Fact]
		public void SubdirectoryDefinitionsContainFullModule()
		{
			List<ClassDefinition> defs = scriptManager.GetScripts();
			Assert.Contains(OreganoDefinition, defs);
			Assert.DoesNotContain(OreganoDefinitionWithoutSubfolder, defs);
		}

		[Fact]
		public void FindDerivedClasses()
		{
			List<ClassDefinition> defs = scriptManager.GetScripts(PlantDefinition);
			Assert.Contains(BasilDefinition, defs);
			Assert.DoesNotContain(CatDefinition, defs);
		}

		[Fact]
		public void FindDerrivedClassesWithBaseInSubfolder()
		{
			List<ClassDefinition> defs = scriptManager.GetScripts(OreganoDefinition);
			Assert.Contains(MexicanOreganoDefinition, defs);
		}

		[Fact]
		public void ExcludeBaseClassWhenSearchingForDerivedClasses()
		{
			List<ClassDefinition> defs = scriptManager.GetScripts(PlantDefinition);
			Assert.DoesNotContain(PlantDefinition, defs);
		}

		[Fact]
		public void FindIndirectDerrivedClasses()
		{
			List<ClassDefinition> defs = scriptManager.GetScripts(BasilDefinition);
			Assert.Contains(ThaiBasilDefinition, defs);

			defs = scriptManager.GetScripts(PlantDefinition);
			Assert.Contains(BasilDefinition, defs);
			Assert.Contains(ThaiBasilDefinition, defs);
		}

		[Fact]
		public void ExcludeModule()
		{
			List<string> excludedModules = ["Subfolder"];
			List<ClassDefinition> defs = scriptManager.GetScripts(null, excludedModules);
			Assert.Contains(OysterMushroomDefinition, defs);
			Assert.DoesNotContain(MushroomDefinition, defs);

		}
		[Fact]
		public void CanDeriveFromExcludedModuleClass()
		{
			List<string> excludedModules = ["Subfolder"];
			List<ClassDefinition> defs = scriptManager.GetScripts(MushroomDefinition, excludedModules);
			Assert.Contains(OysterMushroomDefinition, defs);
			Assert.DoesNotContain(MushroomDefinition, defs);
		}
	}

	[Collection(nameof(PythonFixtureCollection))]
	public class CodeGeneration : IDisposable
	{
		private readonly string RobotDirectory;
		private readonly string RobotFilePath;
		private readonly string BaseGeneratedDirectory;
		private readonly string RobotSubclassName = "RobotSubclass";
		private readonly string RobotSubclassTwoName = "RobotSubclassTwo";
		private readonly string RobotSubclassModule;
		public CodeGeneration()
		{
			RobotFilePath = PythonModule.GetRelativePathFromModuleName(Robot.DefaultModuleName);
			RobotDirectory = Directory.GetParent(RobotFilePath)!.ToString();
			BaseGeneratedDirectory = PythonModule.GetAbsolutePath(Robot.DefaultModuleName, true);

			string subclassModule = PythonModule.GetModuleNameFromRelativePath(RobotFilePath);
			RobotSubclassModule = PythonModule.TrimLastModule(subclassModule) + ".SubclassModule";
			Dispose();
		}
		public void Dispose()
		{
			if (Directory.Exists(RobotDirectory))
			{
				Directory.Delete(RobotDirectory, true);
			}
			GC.SuppressFinalize(this);
		}
		[Fact]
		public void EnsureDirectoryExists()
		{
			Assert.False(Directory.Exists(RobotDirectory), "Path does not exist before test");
			ScriptManager.EnsurePathExists(RobotDirectory);
			Assert.True(Directory.Exists(RobotDirectory), "Path exists after ensure");

			ScriptManager.EnsurePathExists(RobotDirectory);
			Assert.True(Directory.Exists(RobotDirectory), "Path exists after second ensure");
		}
		[Fact]
		public void EnsureFileExists()
		{
			Assert.False(File.Exists(RobotFilePath), "Path does not exist before test");
			ScriptManager.EnsurePathExists(RobotFilePath);
			Assert.True(File.Exists(RobotFilePath), "Path exists after ensure");

			ScriptManager.EnsurePathExists(RobotFilePath);
			Assert.True(File.Exists(RobotFilePath), "Path exists after second ensure");
		}
		[Fact]
		public void GenerateFromBaseCode()
		{
			Robot robot;
			Assert.Throws<Python.Runtime.PythonException>(() => robot = new());

			PythonGeneration.GenerateScript(Robot.DefaultModuleName, Robot.GetPythonBaseCode());
			robot = new(); // Check this doesn't throw an exception
			robot.Beep();
		}
		[Fact]
		public void FileContentsHaveNotChanged()
		{
			PythonGeneration.GenerateScript(RobotSubclassModule, RobotGeneration.RobotSubclassCodeBrokenString);
			string originalContents = File.ReadAllText(PythonModule.GetAbsolutePath(RobotSubclassModule));
			bool hasChanged = ScriptManager.HasScriptChanged(RobotSubclassModule, originalContents);
			Assert.False(hasChanged);
		}
		[Fact]
		public void FileContentsHaveChanged()
		{
			PythonGeneration.GenerateScript(RobotSubclassModule, RobotGeneration.RobotSubclassCodeBrokenString);
			string originalContents = File.ReadAllText(PythonModule.GetAbsolutePath(RobotSubclassModule));
			PythonGeneration.GenerateScript(RobotSubclassModule, RobotGeneration.RobotSubclassCodeWorkingString);
			bool hasChanged = ScriptManager.HasScriptChanged(RobotSubclassModule, originalContents);
			Assert.True(hasChanged);
		}
		[Fact]
		public void GenerateSubclass()
		{
			PythonGeneration.GenerateScript(RobotSubclassModule, RobotGeneration.RobotSubclassCodeWorkingString);
			_ = new Robot(RobotSubclassModule, RobotSubclassName);
		}
		[Fact]
		public void AccessBaseClassMethodThroughSubclass()
		{
			PythonGeneration.GenerateScript(Robot.DefaultModuleName, Robot.GetPythonBaseCode());
			PythonGeneration.GenerateScript(RobotSubclassModule, RobotGeneration.RobotSubclassCodeWorkingString);
			Robot robot = new(RobotSubclassModule, RobotSubclassName);
			robot.Beep();

		}
		[Fact]
		public void UpdateSubclass()
		{
			PythonGeneration.GenerateScript(RobotSubclassModule, RobotGeneration.RobotSubclassCodeBrokenString);
			Robot robot = new(RobotSubclassModule, RobotSubclassName);
			float result = robot.Divide(7.0f, 2.0f);
			float brokenExpected = -1.0f;
			Assert.Equal(brokenExpected, result);

			PythonGeneration.GenerateScript(RobotSubclassModule, RobotGeneration.RobotSubclassCodeWorkingString);
			robot = new(RobotSubclassModule, RobotSubclassName);
			result = robot.Divide(7.0f, 2.0f);
			float workingExpected = 3.5f;
			Assert.Equal(workingExpected, result);
		}

		[Fact]
		public void ObjectUpdatesAutomatically()
		{
			float brokenExpected = -1.0f, workingExpected = 3.5f;
			PythonGeneration.GenerateScript(Robot.DefaultModuleName, Robot.GetPythonBaseCode());
			PythonGeneration.GenerateScript(RobotSubclassModule, RobotGeneration.RobotSubclassCodeBrokenString);
			Robot robot = new(RobotSubclassModule, RobotSubclassName);
			float result = robot.Divide(7.0f, 2.0f);
			Assert.Equal(brokenExpected, result);

			PythonGeneration.GenerateScript(RobotSubclassModule, RobotGeneration.RobotSubclassCodeWorkingString);
			// The object should update to use the new code automatically.
			// Updates on next method call.
			result = robot.Divide(7.0f, 2.0f);
			Assert.Equal(workingExpected, result);

			PythonGeneration.GenerateScript(RobotSubclassModule, RobotGeneration.RobotSubclassCodeBrokenString);
			// Same test, back to broken.
			result = robot.Divide(7.0f, 2.0f);
			Assert.Equal(brokenExpected, result);
		}
		[Fact]
		public void TwoClassesOneModule()
		{
			PythonGeneration.GenerateScript(RobotSubclassModule, RobotGeneration.RobotSubclassCodeWorkingString);
			PythonGeneration.GenerateScript(RobotSubclassModule, RobotGeneration.RobotTwoCodeString, true);
			Robot robot = new(RobotSubclassModule, RobotSubclassName);
			float result = robot.Divide(7.0f, 2.0f);
			float expected = 3.5f;
			Assert.Equal(expected, result);

			Robot robotTwo = new(RobotSubclassModule, RobotSubclassTwoName);
			result = robotTwo.Divide(7.0f, 2.0f);
			float expectedTwo = 2.0f;
			Assert.Equal(expectedTwo, result);
		}
		[Fact]
		public void SecondGenerationAppends()
		{
			PythonGeneration.GenerateScript(RobotSubclassModule, RobotGeneration.RobotSubclassCodeWorkingString);
			PythonGeneration.GenerateScript(RobotSubclassModule, RobotGeneration.RobotTwoCodeString, true);

			string result = File.ReadAllText(PythonModule.GetAbsolutePath(RobotSubclassModule));
			string expected = RobotGeneration.RobotSubclassCodeWorkingString + RobotGeneration.RobotTwoCodeString;
			Assert.Equal(expected, result);
		}
		[Fact]
		public void SecondGenerationOverwritesWithoutAppend()
		{
			PythonGeneration.GenerateScript(RobotSubclassModule, RobotGeneration.RobotSubclassCodeWorkingString);
			PythonGeneration.GenerateScript(RobotSubclassModule, RobotGeneration.RobotTwoCodeString);

			string result = File.ReadAllText(PythonModule.GetAbsolutePath(RobotSubclassModule));
			string expected = RobotGeneration.RobotTwoCodeString;
			Assert.Equal(expected, result);
		}
		/*
		[Fact]
		public void SecondGenerationRemovesRobotDefinition()
		{
			PythonGeneration.GenerateScript(RobotSubclassModule, RobotGeneration.RobotSubclassCodeWorkingString);

			// Overwrite the old code with the new
			PythonGeneration.GenerateScript(RobotSubclassModule, RobotGeneration.RobotTwoCodeString);

			// Doesn't appear to be a good way to remove module items during runtime, only add.
			//   Robot working string, which was overwritten, carries over.
			Robot robot;
			Assert.Throws<Python.Runtime.PythonException>(() => robot = new(RobotSubclassModule, "RobotSubclass"));
		}
		*/
	}

	[Collection(nameof(PythonFixtureCollection))]
	public class Logging : IDisposable
	{
		static readonly string LogDirectory = Path.Combine(Environment.CurrentDirectory, "Logs");
		static readonly string LogFilePath = Path.Combine(LogDirectory, "LogFile.txt");
		static readonly string TestLoggingMessage = "My unit testing message!";

		private readonly TestSnake testSnake;
		public Logging(PythonFixture pyFixtureReference)
		{
			testSnake = pyFixtureReference.testSnake;
			Dispose();
		}
		public void Dispose()
		{
			PythonWalker.Logging.CloseLoggingContext(true);
			if (Directory.Exists(LogDirectory))
			{
				Directory.Delete(LogDirectory, true);
			}
			GC.SuppressFinalize(this);
		}
		[Fact]
		public void StartingContextCreatesFile()
		{
			Assert.False(File.Exists(LogFilePath));
			PythonWalker.Logging.StartLoggingContext(LogFilePath);
			Assert.True(File.Exists(LogFilePath));
		}
		[Fact]
		public void PrintToLogFile()
		{
			PythonWalker.Logging.StartLoggingContext(LogFilePath);
			Assert.True(PythonWalker.Logging.IsLogging());
			Assert.Equal(LogFilePath, PythonWalker.Logging.GetCurrentLogFile());

			PythonEngine.Eval($"""print("{TestLoggingMessage}")""");

			string result = PythonWalker.Logging.ReadCurrentContentsOfLogFile();
			Assert.Equal(TestLoggingMessage, result);
		}
		[Fact]
		public void PrintToLogFileMultipleTimesAppends()
		{
			PythonWalker.Logging.StartLoggingContext(LogFilePath);
			PythonEngine.Eval($"""print("{TestLoggingMessage}")""");
			PythonWalker.Logging.CloseLoggingContext();

			PythonWalker.Logging.StartLoggingContext(LogFilePath);
			PythonEngine.Eval($"""print("{TestLoggingMessage}")""");

			string resultString = PythonWalker.Logging.ReadCurrentContentsOfLogFile();
			int numberOfPrints = Regex.Matches(resultString, TestLoggingMessage).Count;
			int expectedNumberOfPrints = 2;
			Assert.Equal(expectedNumberOfPrints, numberOfPrints);
		}
		[Fact]
		public void ErrorToLogFile()
		{
			PythonWalker.Logging.StartLoggingContext(LogFilePath);
			Assert.Throws<Python.Runtime.PythonException>(() => testSnake.HitPythonTypeError());

			string result = PythonWalker.Logging.ReadCurrentContentsOfLogFile();
			Assert.Contains(nameof(Python.Runtime.PythonException), result);
		}
		[Fact]
		public void ClearLogFileOnClose()
		{
			PythonWalker.Logging.StartLoggingContext(LogFilePath);
			Assert.True(File.Exists(LogFilePath));
			PythonWalker.Logging.CloseLoggingContext(true);
			Assert.False(File.Exists(LogFilePath));
		}
		[Fact]
		public void LogToTemporaryFile()
		{
			PythonWalker.Logging.StartLoggingContext();
			Assert.True(PythonWalker.Logging.IsLogging());

			PythonEngine.Eval($"""print("{TestLoggingMessage}")""");

			string result = PythonWalker.Logging.ReadCurrentContentsOfLogFile();
			Assert.Equal(TestLoggingMessage, result);

			string tempLogPath = PythonWalker.Logging.GetCurrentLogFile();
			PythonWalker.Logging.CloseLoggingContext();
			Assert.False(File.Exists(tempLogPath));
		}

		[Fact]
		public void StartLoggingContextWhileOneIsOpen()
		{
			PythonWalker.Logging.StartLoggingContext(LogFilePath);
			Assert.True(PythonWalker.Logging.IsLogging());
			Assert.Throws<PythonLoggingContextAlreadyOpen>(() => PythonWalker.Logging.StartLoggingContext(LogFilePath));
		}
	}

	[Collection(nameof(PythonFixtureCollection))]
	public class Replay
	{
		[Fact]
		public void ReplayTest()
		{
			/* Copied the C++ rough test outline until we get to this
			 * 
			 *
			// Generate module with failing method
			PythonWalker::CodeGeneration::GeneratePythonClass(currentPath, "GeneratedFiles.Robot", RobotCodeWithZeroErrorString);
			TestRobotClass robot = TestRobotClass("GeneratedFiles.Robot", "Robot");
			float result;

			// Test with error
			PythonWalker::Logging::StartLoggingContext(LogFilePath);
			try {
				result = robot.divide(1.0, 0.0);
			}
			catch (PythonWalker::PythonMethodError e) { }
			PythonWalker::Logging::FlushLoggingContext();

			std::string fileContents = PythonWalker::ScriptManager::GetFileContents(LogFilePath);
			std::wstring fileContentsOutput = std::wstring(fileContents.begin(), fileContents.end());
			Assert::IsTrue(fileContents.find("ZeroDivisionError") != std::string::npos, fileContentsOutput.c_str());

			//TODO maybe save state before executing function, be able to save that off
			// State should also include variables that aren't in C++
			// State should include module/class called, variables within class, globals within module (and imported?)
			// Replay should setup all the variables and run the function
			// Should be runnable by the user so they can breakpoint easily


			// TODO Test replay with error
			// TODO Generate module with working method
			// 
			PythonWalker::CodeGeneration::GeneratePythonClass(currentPath, "GeneratedFiles.Robot", RobotCodeWithZeroErrorFixedString);
			
			
			robot = TestRobotClass("GeneratedFiles.Robot", "Robot");
			// TODO Run replay that would have errored, rather than calling the function again
			expected = 0.0;
			result = robot.divide(1.0, 0.0);
			Assert::AreEqual(expected, result);
			
			*
			*/
		}
	}

}