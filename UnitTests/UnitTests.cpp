#include "pch.h"
#include "CppUnitTest.h"

#include <filesystem>

#include "../PythonWalker/PythonWalker.h"
#include "CodeGenStrings.h"
#include <fstream>

using namespace Microsoft::VisualStudio::CppUnitTestFramework;

//https://learn.microsoft.com/en-us/visualstudio/test/writing-unit-tests-for-c-cpp?view=vs-2022

template <typename T>
bool VectorContains(std::vector<T> vec, T val)
{
	return std::find(vec.begin(), vec.end(), val) != vec.end();
}
bool VectorContains(std::vector<const char *> vec, const char* val)
{
	for(const char* vectorValue : vec) {
		if (strcmp(vectorValue, val) == 0) {
			return true;
		}
	}
	return false;
}
class PyPosition : public PyWalkerObjectInstance {
public:
	using PyWalkerObjectInstance::PyWalkerObjectInstance;
	PyPosition() : PyWalkerObjectInstance("TestSnake", "Position") {};
	PyPosition(double x, double y) : PyPosition() {
		X = x;
		Y = y;
	};
	__PYW_TYPING_VAR(double, X)
	__PYW_TYPING_VAR(double, Y)
	__PYW_TYPING_METHOD(double, pythag)
};

class TestPythonClass : public PyWalkerObjectInstance {
public:
	using PyWalkerObjectInstance::PyWalkerObjectInstance;

	__PYW_TYPING_VAR(const char*, name)
	__PYW_TYPING_VAR(std::string, birthplace)
	__PYW_TYPING_VAR(long, length)
	__PYW_TYPING_VAR(int, age)
	__PYW_TYPING_VAR(double, terrariumWidth)
	__PYW_TYPING_VAR(double, terrariumDepth)
	__PYW_TYPING_VAR(float, terrariumHeight)
	__PYW_TYPING_VAR(bool, isCool)
		__PYW_TYPING_VAR(PyPosition, position)
		__PYW_TYPING_VAR(std::vector<const char*>, friends)


	__PYW_TYPING_VAR(int, fakeInt)

	__PYW_TYPING_METHOD(void, increaseAgeByOne)
	__PYW_TYPING_METHOD(void, issueFunction)
	__PYW_TYPING_METHOD(double, calculateTerrariumArea)
	__PYW_TYPING_METHOD(void, changeName, const char*, newName)
	__PYW_TYPING_METHOD(double, AddThreeParametersTogether, int, a, float, b, double, c)
	__PYW_TYPING_METHOD(double, AddNumbersTogether, std::vector<double>, numbers)
	__PYW_TYPING_METHOD(std::vector<const char *>, getFriends)
	__PYW_TYPING_METHOD(std::vector<int>, countToFive)
	
	__PYW_TYPING_METHOD(int, getGlobalVar)
	__PYW_TYPING_METHOD(void, setGlobalVar, int, value)
	__PYW_TYPING_METHOD(void, fakeFunction)
	__PYW_TYPING_METHOD(void, noParamFunction, int, fakeParam)
	__PYW_TYPING_METHOD(void, printMessage, std::string, msg)
	__PYW_TYPING_METHOD(double, GetPositionXIndirectly)
};
class TestRobotClass : public PyWalkerObjectInstance {
public:
	using PyWalkerObjectInstance::PyWalkerObjectInstance;

	TestRobotClass() : PyWalkerObjectInstance("GeneratedFiles.Robot", "Robot") {};
	__PYW_TYPING_VAR(int, age)
	__PYW_TYPING_METHOD(float, divide, float, dividend, float, divisor)
};

std::filesystem::path currentPath = std::filesystem::current_path();
std::string currentPathString = currentPath.make_preferred().string();
static PythonWalker pythonWalker = PythonWalker(std::vector<std::string>({
		currentPathString + "\\..\\..\\UnitTests\\UnitTestsPythonScripts\\",
		currentPathString
	}));

TestPythonClass testSnake;

TEST_MODULE_INITIALIZE(ClassInitialize)
{
	testSnake = TestPythonClass("TestSnake", "TestSnake");
}

namespace PythonInterfaceTestCases
{
	TEST_CLASS(PythonModuleInterfaceTestClass)
	{
	public:
		TEST_METHOD(LoadModule)
		{
			PyObject* module = PythonWalker::LoadModule("TestSnake");
			Assert::IsNotNull(module);
		}
		TEST_METHOD(GetModuleVariable)
		{
			PyObject* module = PythonWalker::LoadModule("TestSnake");
			int gloVal1=1, gloVal2=2;

			PythonWalker::SetModuleGlobal<int>(module, "gloVal", gloVal1);
			Assert::AreEqual(gloVal1, PythonWalker::GetModuleGlobal<int>(module,"gloVal"));

			PythonWalker::SetModuleGlobal<int>(module, "gloVal", gloVal2);
			Assert::AreEqual(gloVal2, PythonWalker::GetModuleGlobal<int>(module, "gloVal"));
		}
		/*
		TEST_METHOD(RunModuleFunction)
		{
			// Would this even be useful?
			Assert::IsTrue(true);
		}
		*/
	};
	TEST_CLASS(PythonObjectInterfaceTestClass)
	{
	public:
		TEST_METHOD(InitializeObject)
		{
			TestPythonClass testSnakeSuccess = TestPythonClass("TestSnake", "TestSnake");
			Assert::IsTrue(testSnakeSuccess.IsInitialized());
		}
		TEST_METHOD(InitializeObjectFromSubfolder)
		{
			TestPythonClass testSubfolderObject = TestPythonClass("Subfolder.TestOregano", "TestOregano");
			Assert::IsTrue(testSubfolderObject.IsInitialized());
		}
		TEST_METHOD(GetAndSetVariables)
		{
			int age1 = 1, age2 = 2;
			const char* name = "Ludwig";
			std::string birthplace = "Alaska";
			long length = 7;
			bool isCool = true;

			testSnake.age = age1;
			Assert::AreEqual(age1, testSnake.age);
			testSnake.age = age2;
			Assert::AreEqual(age2, testSnake.age);

			testSnake.name = name;
			Assert::AreEqual(name, testSnake.name);

			testSnake.birthplace = birthplace;
			Assert::AreEqual(birthplace, testSnake.birthplace);

			testSnake.length = length;
			Assert::AreEqual(length, testSnake.length);

			testSnake.isCool = false;
			Assert::AreEqual(false, testSnake.isCool);

			testSnake.isCool = true;
			Assert::AreEqual(true, testSnake.isCool);
		}
		TEST_METHOD(FunctionSideEffects)
		{
			testSnake.age = 1;
			testSnake.increaseAgeByOne();
			Assert::AreEqual(2, testSnake.age);
			testSnake.increaseAgeByOne();
			Assert::AreEqual(3, testSnake.age);
		}
		TEST_METHOD(ReturnValueFromFunction)
		{
			double width = 5.0, depth = 3.0, height = 2.0;
			double area = width * depth * height;
			testSnake.terrariumWidth = width;
			testSnake.terrariumDepth = depth;
			testSnake.terrariumHeight = (float)height;
			double pythonCalculatedArea = testSnake.calculateTerrariumArea();
			Assert::AreEqual(area, pythonCalculatedArea);
		}
		TEST_METHOD(ObjectIndependence)
		{
			TestPythonClass testSnake1 = TestPythonClass("TestSnake", "TestSnake");
			TestPythonClass testSnake2 = TestPythonClass("TestSnake", "TestSnake");
			int age1 = 1, age2 = 2;
			testSnake1.age = age1;
			testSnake2.age = age2;
			Assert::AreEqual(age1, (int)testSnake1.age);
			Assert::AreEqual(age2, (int)testSnake2.age);
			Assert::AreNotEqual(testSnake1.age, testSnake2.age);
		}
		TEST_METHOD(PassParametersIntoFunction)
		{
			const char* newName = "Ludwig";
			testSnake.changeName(newName);
			Assert::AreEqual(newName, testSnake.name);

			int a = 1;
			float b = 2.5;
			double c = 3.14159;
			double result = testSnake.AddThreeParametersTogether(a, b, c);
			Assert::AreEqual(a + b + c, result);
		}
		TEST_METHOD(GlobalIsSharedBetweenClassInstances)
		{
			TestPythonClass testSnake1 = TestPythonClass("TestSnake", "TestSnake");
			TestPythonClass testSnake2 = TestPythonClass("TestSnake", "TestSnake");
			int gloVal1=1, gloVal2=2;

			testSnake1.setGlobalVar(gloVal1);
			Assert::AreEqual(gloVal1, testSnake1.getGlobalVar());
			Assert::AreEqual(gloVal1, testSnake2.getGlobalVar());

			testSnake2.setGlobalVar(gloVal2);
			Assert::AreEqual(gloVal2, testSnake1.getGlobalVar());
			Assert::AreEqual(gloVal2, testSnake2.getGlobalVar());
		}
		TEST_METHOD(GetAndSetNestedObject)
		{
			double x = 1, y = 2;
			PyPosition pos(x, y);
			testSnake.position = pos;
			Assert::AreEqual(x, testSnake.position.X);
			Assert::AreEqual(y, testSnake.position.Y);
		}
		TEST_METHOD(SetNestedObjectPropagatesToPyObject)
		{
			double x = 1, y = 2;
			PyPosition pos(x, y);
			testSnake.position = pos;
			Assert::AreEqual(x, testSnake.GetPositionXIndirectly());

			pos.X = 0.5;
			Assert::AreNotEqual(pos.X, testSnake.GetPositionXIndirectly());
		}
		TEST_METHOD(IndirectObjectIndependence)
		{
			double x = 1, y = 2, x2 = 3.14;
			PyPosition pos(x, y);
			testSnake.position = pos;

			pos.X = x2;
			Assert::AreNotEqual(pos.X, testSnake.GetPositionXIndirectly());
		}
		TEST_METHOD(NestedObjectAssignmentKeepVariables)
		{
			double x = 1, y = 2;
			testSnake.position = PyPosition(x, y);
			Assert::AreEqual(testSnake.position.X, x);
			Assert::AreEqual(testSnake.position.Y, y);
		}
		TEST_METHOD(NestedObjectMethod)
		{
			double x = 1, y = 2;
			testSnake.position = PyPosition(x, y);
			double pythag = sqrt(x*x + y*y);
			double result = testSnake.position.pythag();
			Assert::AreEqual(pythag, result);
		}
		TEST_METHOD(KeepClassDefThroughAssignment)
		{
			double x = 1, y = 2;
			PyPosition pos(x, y);
			testSnake.position = pos;

			Assert::AreEqual(pos.ClassDef.ClassName, testSnake.position.ClassDef.ClassName,
				L"ClassName not kept through assignment");
			Assert::AreEqual(pos.ClassDef.Module, testSnake.position.ClassDef.Module,
				L"Module not kept through assignment");
		}
		TEST_METHOD(ObjectIndependenceAfterAssignment)
		{
			double x = 1, y = 2, x2 = 3.14, y2 = 4.28;
			PyPosition pos(x, y);
			testSnake.position = pos;

			pos.X = x2;
			pos.Y = y2;

			Assert::AreNotEqual(pos.X, testSnake.position.X);
			Assert::AreNotEqual(pos.Y, testSnake.position.Y);
		}
		TEST_METHOD(GetClassDefinitionFromPyObject)
		{
			PyPosition pos;
			PythonClassDefinition def(pos.PyObjectInstance);
			Assert::AreEqual(pos.ClassDef.Module, def.Module);
			Assert::AreEqual(pos.ClassDef.ClassName, def.ClassName);
		}
		TEST_METHOD(ListAsObjectVariable)
		{
			std::vector<const char*> friends = std::vector<const char*>{
				"Jake", "Sam"
			};
			testSnake.friends = friends;

			for (int i = 0; i < friends.size(); i++) {
				Assert::IsTrue(VectorContains(friends, testSnake.friends[i]));
			}
		}
		TEST_METHOD(ListElementCanBeChanged)
		{
			const char* newFriend = "Hector";
			const char* notSnakeFriend = "Ethan";
			testSnake.friends = std::vector<const char*>{
				"Jake", "Sam", notSnakeFriend
			};

			// Right now this runs the getter on friends, accesses the element const char *, and sets that.
			testSnake.friends[2] = newFriend;

			//TODO I thought ampersand on the getter return value fixed it, but now it reverts soon after...
			// Ampersand makes it set _friends, but not in python.
			//	 Setting _friends directly, not through setter
			//   Thus _friends is reset when we get it from python.
			// 
			// Ampersand is giving reference to underlying variable
			//		Probably need to do wrapper rather than props
			testSnake.Reload_friends();

			Assert::IsTrue(VectorContains(testSnake.friends, newFriend));
			Assert::IsTrue(!VectorContains(testSnake.friends, notSnakeFriend));
		}
		TEST_METHOD(ListAsParameter)
		{
			std::vector<double> numbers = std::vector<double>{
				1, 2, 4, 8, 16
			};
			double sum = 0;
			for (double i : numbers) sum += i;
			double result = testSnake.AddNumbersTogether(numbers);
			Assert::AreEqual(sum, result);
		}
		TEST_METHOD(ListAsReturnValue)
		{
			std::vector<int> countToFive = std::vector<int>{
				1, 2, 3, 4, 5
			};
			std::vector<int> result = testSnake.countToFive();
			for (int i = 0; i < countToFive.size(); i++) {
				Assert::AreEqual(countToFive[i], result[i]);
			}
		}
	};
	TEST_CLASS(PythonInterfaceExceptionTestClass)
	{
	public:
		TEST_METHOD(FakeVariableException)
		{
			auto func = [] {testSnake.fakeInt = 1; };
			Assert::ExpectException<PythonAttributeDNE>(func);
		}
		TEST_METHOD(FakeFunctionException)
		{
			auto func = [] {testSnake.fakeFunction(); };
			Assert::ExpectException<PythonMethodDNE>(func);
		}
		TEST_METHOD(FakeFunctionParametersException)
		{
			auto func = [] {testSnake.noParamFunction(4); };
			Assert::ExpectException<PythonMethodError>(func);
		}
		TEST_METHOD(FunctionIssue)
		{
			auto func = [] {testSnake.issueFunction(); };
			Assert::ExpectException<PythonMethodError>(func);
		}
		TEST_METHOD(ModuleDNE)
		{
			auto func = [] {TestPythonClass fakeModuleSnake = TestPythonClass("FakeModule", "TestSnake"); };
			Assert::ExpectException<PythonModuleDNE>(func);
		}
		TEST_METHOD(ModuleNotInitialized)
		{
			auto func = [] {PythonWalker::CreateObject(nullptr, "TestSnake"); };
			Assert::ExpectException<PythonModuleNotInitialized>(func);
		}
		TEST_METHOD(ObjectDNE)
		{
			auto func = [] {TestPythonClass throwAway = TestPythonClass("TestSnake", "FakeName"); };
			Assert::ExpectException<PythonObjectDNE>(func);
		}
	};
}
namespace PythonFileTestCases
{
	PythonClassDefinition PlantDefinition = PythonClassDefinition("TestPlant", "TestPlant");
	PythonClassDefinition DuplicatePlantDefinition = PythonClassDefinition("TestBasil", "TestPlant");
	PythonClassDefinition BasilDefinition = PythonClassDefinition("TestBasil", "TestBasil");
	PythonClassDefinition ThaiBasilDefinition = PythonClassDefinition("TestThaiBasil", "TestThaiBasil");
	PythonClassDefinition CatDefinition = PythonClassDefinition("TestPet", "TestCat");
	PythonClassDefinition DogDefinition = PythonClassDefinition("TestPet", "TestDog");
	PythonClassDefinition OreganoDefinition = PythonClassDefinition("Subfolder.TestOregano", "TestOregano");
	PythonClassDefinition MexicanOreganoDefinition = PythonClassDefinition("Subfolder.TestMexicanOregano", "TestMexicanOregano");
	PythonClassDefinition OreganoDefinitionWithoutSubfolder = PythonClassDefinition("TestOregano", "TestOregano");
	PythonClassDefinition RosemaryDefinition = PythonClassDefinition("Subfolder.Subfolder2.TestRosemary", "TestRosemary");

	PythonClassDefinition MushroomDefinition = PythonClassDefinition("Subfolder.TestMushroom", "TestMushroom");
	PythonClassDefinition OysterMushroomDefinition = PythonClassDefinition("TestOysterMushroom", "TestOysterMushroom");
	std::string NonExistantClassName = "NonExistantClassName";
	TEST_CLASS(PythonFileManagementTestClass)
	{
	public:
		TEST_METHOD(FindClasses)
		{
			std::vector<PythonClassDefinition> scripts = pythonWalker.GetScripts();
			Assert::IsTrue(VectorContains<PythonClassDefinition>(scripts, BasilDefinition));
		}
		TEST_METHOD(FindClassesNoDups)
		{
			std::vector<PythonClassDefinition> scripts = pythonWalker.GetScripts();
			Assert::IsTrue(VectorContains<PythonClassDefinition>(scripts, PlantDefinition));
			Assert::IsFalse(VectorContains<PythonClassDefinition>(scripts, DuplicatePlantDefinition));
		}
		TEST_METHOD(FindTwoClassesInOneModule)
		{
			std::vector<PythonClassDefinition> scripts = pythonWalker.GetScripts();
			Assert::IsTrue(VectorContains<PythonClassDefinition>(scripts, CatDefinition));
			Assert::IsTrue(VectorContains<PythonClassDefinition>(scripts, DogDefinition));
		}
		TEST_METHOD(FindClassesRecursive)
		{
			std::vector<PythonClassDefinition> scripts = pythonWalker.GetScripts(true);
			Assert::IsTrue(VectorContains<PythonClassDefinition>(scripts, BasilDefinition));
			Assert::IsTrue(VectorContains<PythonClassDefinition>(scripts, OreganoDefinition));
			Assert::IsTrue(VectorContains<PythonClassDefinition>(scripts, RosemaryDefinition));

			scripts = pythonWalker.GetScripts(false);
			Assert::IsTrue(VectorContains<PythonClassDefinition>(scripts, BasilDefinition));
			Assert::IsFalse(VectorContains<PythonClassDefinition>(scripts, OreganoDefinition));
			Assert::IsFalse(VectorContains<PythonClassDefinition>(scripts, RosemaryDefinition));
		}
		TEST_METHOD(RecursiveClassIncludesFullModule)
		{
			std::vector<PythonClassDefinition> scripts = pythonWalker.GetScripts(true);
			Assert::IsTrue(VectorContains<PythonClassDefinition>(scripts, OreganoDefinition));
			Assert::IsFalse(VectorContains<PythonClassDefinition>(scripts, OreganoDefinitionWithoutSubfolder));
		}
		TEST_METHOD(FindDerivedClasses)
		{
			std::vector<PythonClassDefinition> scripts = pythonWalker.GetScripts(false, PlantDefinition.ClassName);
			Assert::IsTrue(VectorContains<PythonClassDefinition>(scripts, BasilDefinition));
			Assert::IsFalse(VectorContains<PythonClassDefinition>(scripts, CatDefinition));

			scripts = pythonWalker.GetScripts(false, NonExistantClassName);
			Assert::IsTrue(scripts.empty());
		}
		TEST_METHOD(FindDerivedClassesWithBaseInSubfolder)
		{
			std::vector<PythonClassDefinition> scripts = pythonWalker.GetScripts(true, OreganoDefinition.ClassName);
			Assert::IsTrue(VectorContains<PythonClassDefinition>(scripts, MexicanOreganoDefinition));
		}
		TEST_METHOD(ExcludeBaseClassWhenSearchingForDerivedClasses)
		{
			std::vector<PythonClassDefinition> scripts = pythonWalker.GetScripts(false, PlantDefinition.ClassName);
			Assert::IsFalse(VectorContains<PythonClassDefinition>(scripts, PlantDefinition));
		}
		TEST_METHOD(FindIndirectDerrivedClasses)
		{
			std::vector<PythonClassDefinition> scripts = pythonWalker.GetScripts(false, PlantDefinition.ClassName);
			Assert::IsTrue(VectorContains<PythonClassDefinition>(scripts, ThaiBasilDefinition));
		}
		TEST_METHOD(ExcludeModule)
		{
			std::vector<std::string> excludedModules = {
				"Subfolder"
			};
			std::vector<PythonClassDefinition> scripts = pythonWalker.GetScripts(true, "", excludedModules);
			Assert::IsFalse(VectorContains<PythonClassDefinition>(scripts, MushroomDefinition));
			Assert::IsTrue(VectorContains<PythonClassDefinition>(scripts, OysterMushroomDefinition));
		}
		TEST_METHOD(DeriveFromExcludedModuleClass)
		{
			std::vector<std::string> excludedModules = {
				"Subfolder"
			};
			std::vector<PythonClassDefinition> scripts = pythonWalker.GetScripts(true, MushroomDefinition.ClassName, excludedModules);
			Assert::IsFalse(VectorContains<PythonClassDefinition>(scripts, MushroomDefinition));
			Assert::IsTrue(VectorContains<PythonClassDefinition>(scripts, OysterMushroomDefinition));
			Assert::IsFalse(VectorContains<PythonClassDefinition>(scripts, BasilDefinition));
		}
	};
}

namespace PythonCodeGenerationTestCases
{
	TEST_CLASS(PythonCodeGenerationTestClass)
	{
	public:
		TEST_METHOD_CLEANUP(CleanupCodeGenMethod)
		{
			PythonCodeGeneration::DeletePythonModule(currentPath, "GeneratedFiles", true);
		}
		PythonClassDefinition RobotDefinition = PythonClassDefinition("GeneratedFiles.Robot", "Robot");
		PythonClassDefinition RobotTwoDefinition = PythonClassDefinition("GeneratedFiles.Robot", "RobotTwo");
		TEST_METHOD(GenerateModuleAndClass)
		{
			PythonCodeGeneration::GeneratePythonClass(currentPath, RobotDefinition, RobotCodeString);
			std::vector<PythonClassDefinition> scripts = pythonWalker.GetScripts(true);
			Assert::IsTrue(VectorContains<PythonClassDefinition>(scripts, RobotDefinition));

			std::string result = PythonFileManagement::GetFileContents(currentPath / "GeneratedFiles\\Robot.py");
			Assert::AreEqual(RobotCodeString, result);
		}
		TEST_METHOD(DeleteScriptModule)
		{
			PythonCodeGeneration::GeneratePythonClass(currentPath, RobotDefinition, RobotCodeString);
			std::vector<PythonClassDefinition> scripts = pythonWalker.GetScripts(true);
			Assert::IsTrue(VectorContains<PythonClassDefinition>(scripts, RobotDefinition));

			PythonCodeGeneration::DeletePythonModule(currentPath, RobotDefinition);

			scripts = pythonWalker.GetScripts(true);
			Assert::IsFalse(VectorContains<PythonClassDefinition>(scripts, RobotDefinition));
		}
		TEST_METHOD(DeleteDirectoryModule)
		{
			PythonCodeGeneration::GeneratePythonClass(currentPath, RobotDefinition, RobotCodeString);
			std::vector<PythonClassDefinition> scripts = pythonWalker.GetScripts(true);
			Assert::IsTrue(VectorContains<PythonClassDefinition>(scripts, RobotDefinition));

			PythonCodeGeneration::DeletePythonModule(currentPath, "GeneratedFiles", true);

			scripts = pythonWalker.GetScripts(true);
			Assert::IsFalse(VectorContains<PythonClassDefinition>(scripts, RobotDefinition));
		}
		TEST_METHOD(ModuleKilledWhenDeleted)
		{
			PythonCodeGeneration::GeneratePythonClass(currentPath, RobotDefinition, RobotCodeString);
			std::vector<PythonClassDefinition> scripts = pythonWalker.GetScripts(true);
			Assert::IsTrue(VectorContains<PythonClassDefinition>(scripts, RobotDefinition));

			PythonCodeGeneration::DeletePythonModule(currentPath, RobotDefinition);

			scripts = pythonWalker.GetScripts(true);
			Assert::IsFalse(VectorContains<PythonClassDefinition>(scripts, RobotDefinition));
		}
		/*
		TEST_METHOD(ModuleResetWhenUpdated)
		{
			PythonCodeGeneration::GeneratePythonClass(currentPath, RobotDefinition, RobotCodeString);
			std::vector<PythonClassDefinition> scripts = pythonWalker.GetScripts(true);
			Assert::IsTrue(VectorContains<PythonClassDefinition>(scripts, RobotDefinition));

			// Doesn't appear to be a good way to remove module items during runtime, only add.
			//   Robot two string, which hasn't been added yet in this case, carries over from past runs.
			//Assert::IsFalse(VectorContains<PythonClassDefinition>(scripts, RobotTwoDefinition));

			PythonCodeGeneration::GeneratePythonClass(currentPath, RobotTwoDefinition, RobotTwoCodeString);
			scripts = pythonWalker.GetScripts(true);
			Assert::IsTrue(VectorContains<PythonClassDefinition>(scripts, RobotDefinition));
			Assert::IsTrue(VectorContains<PythonClassDefinition>(scripts, RobotTwoDefinition));
		}
		*/
		TEST_METHOD(GenerateModuleAndTwoClasses)
		{
			PythonCodeGeneration::GeneratePythonClass(currentPath, RobotDefinition, RobotCodeString);
			PythonCodeGeneration::GeneratePythonClass(currentPath, RobotTwoDefinition, RobotTwoCodeString);
			std::vector<PythonClassDefinition> scripts = pythonWalker.GetScripts(true);
			Assert::IsTrue(VectorContains<PythonClassDefinition>(scripts, RobotDefinition));
			Assert::IsTrue(VectorContains<PythonClassDefinition>(scripts, RobotTwoDefinition));

			std::string result = PythonFileManagement::GetFileContents(currentPath / "GeneratedFiles\\Robot.py");
			Assert::AreEqual(RobotCodeString + RobotTwoCodeString, result);
		}
		TEST_METHOD(RegenerateClassWithChange)
		{
			PythonCodeGeneration::GeneratePythonClass(currentPath, "GeneratedFiles.Robot", RobotCodeWithZeroErrorString);
			TestRobotClass robot = TestRobotClass("GeneratedFiles.Robot", "Robot");
			float expected, result;

			auto func = [&] {robot.divide(1.0, 0.0); };
			Assert::ExpectException<PythonMethodError>(func);
			
			PythonCodeGeneration::GeneratePythonClass(currentPath, "GeneratedFiles.Robot", RobotCodeWithZeroErrorFixedString);

			robot.RegenerateFromScript();

			expected = 0.0;
			result = robot.divide(1.0, 0.0);
			Assert::AreEqual(expected, result);
		}
		TEST_METHOD(RegenAfterReassignment)
		{
			PythonCodeGeneration::GeneratePythonClass(currentPath, "GeneratedFiles.Robot", RobotCodeWithZeroErrorString);
			TestRobotClass robot = TestRobotClass("GeneratedFiles.Robot", "Robot");
			TestRobotClass robot2 = TestRobotClass("GeneratedFiles.Robot", "Robot");
			int expected = 10;

			robot2.age = expected;
			robot = robot2;
			Assert::AreEqual(expected, robot.age);

			robot.RegenerateFromScript();

		}
		TEST_METHOD(KeepVariableValueThroughRegeneration)
		{
			PythonCodeGeneration::GeneratePythonClass(currentPath, "GeneratedFiles.Robot", RobotCodeWithZeroErrorString);
			TestRobotClass robot = TestRobotClass("GeneratedFiles.Robot", "Robot");
			int expected = 10;
			robot.age = expected;
			Assert::AreEqual(expected, robot.age);

			PythonCodeGeneration::GeneratePythonClass(currentPath, "GeneratedFiles.Robot", RobotCodeWithZeroErrorFixedString);

			robot.RegenerateFromScript();

			Assert::AreEqual(expected, robot.age, L"Variable value was not retained through code refresh");
		}
	};
}

namespace PythonLoggingTestCases
{
	TEST_CLASS(PythonLoggingTestClass)
	{
	public:
		std::filesystem::path LogDirectory = currentPath / "Logs";
		std::filesystem::path LogFilePath = LogDirectory / "LogFile.txt";
		std::string LoggingMessage = "My unit testing message";

		TEST_METHOD_CLEANUP(CleanupLogTesting)
		{
			PythonLogging::CloseLoggingContext();
			PythonFileManagement::ClearDirectory(LogDirectory);
		}
		TEST_METHOD(PrintToLogFile)
		{
			PythonLogging::StartLoggingContext(LogFilePath);
			testSnake.printMessage(LoggingMessage);
			PythonLogging::FlushLoggingContext();

			std::string result = PythonFileManagement::GetFileContents(LogFilePath);
			std::string expected = LoggingMessage + "\n";
			Assert::AreEqual(expected, result);
		}
		TEST_METHOD(ExceptionToLogFile)
		{
			PythonLogging::StartLoggingContext(LogFilePath);
			try {
				testSnake.issueFunction();
			}
			catch (PythonMethodError e) { }
			PythonLogging::FlushLoggingContext();

			std::string result = PythonFileManagement::GetFileContents(LogFilePath);
			Assert::IsTrue(result.find("TypeError") != std::string::npos);
		}
		TEST_METHOD(PrintToLogFileMiltipleTimes)
		{
			PythonLogging::StartLoggingContext(LogFilePath);
			testSnake.printMessage(LoggingMessage);
			PythonLogging::CloseLoggingContext();

			PythonLogging::StartLoggingContext(LogFilePath);
			testSnake.printMessage(LoggingMessage);
			PythonLogging::FlushLoggingContext();

			std::string result = PythonFileManagement::GetFileContents(LogFilePath);
			std::string expected = LoggingMessage + "\n" + LoggingMessage + "\n";
			Assert::AreEqual(expected, result);
		}
		TEST_METHOD(ClearLogFile)
		{
			PythonLogging::StartLoggingContext(LogFilePath);
			testSnake.printMessage(LoggingMessage);
			PythonLogging::CloseLoggingContext();

			std::string result = PythonFileManagement::GetFileContents(LogFilePath);
			std::string expected = LoggingMessage + "\n";
			Assert::AreEqual(expected, result);

			PythonFileManagement::ClearFile(LogFilePath);

			result = PythonFileManagement::GetFileContents(LogFilePath);
			expected = "";
			Assert::AreEqual(expected, result);
		}
	};
	TEST_CLASS(PythonReplayTestClass)
	{
	public:
		std::filesystem::path LogDirectory = currentPath / "Logs";
		std::filesystem::path LogFilePath = LogDirectory / "LogFileReplay.txt";
		TEST_METHOD_CLEANUP(CleanupReplayTesting)
		{
			PythonLogging::CloseLoggingContext();
			PythonFileManagement::ClearDirectory(LogDirectory);
		}
		TEST_METHOD(SupportReplay)
		{
			// Generate module with failing method
			PythonCodeGeneration::GeneratePythonClass(currentPath, "GeneratedFiles.Robot", RobotCodeWithZeroErrorString);
			TestRobotClass robot = TestRobotClass("GeneratedFiles.Robot", "Robot");
			float result;

			// Test with error
			PythonLogging::StartLoggingContext(LogFilePath);
			try {
				result = robot.divide(1.0, 0.0);
			}
			catch (PythonMethodError e) { }
			PythonLogging::FlushLoggingContext();

			std::string fileContents = PythonFileManagement::GetFileContents(LogFilePath);
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
			PythonCodeGeneration::GeneratePythonClass(currentPath, "GeneratedFiles.Robot", RobotCodeWithZeroErrorFixedString);
			
			/*
			robot = TestRobotClass("GeneratedFiles.Robot", "Robot");
			// TODO Run replay that would have errored, rather than calling the function again
			expected = 0.0;
			result = robot.divide(1.0, 0.0);
			Assert::AreEqual(expected, result);
			*/
		}
	};
}