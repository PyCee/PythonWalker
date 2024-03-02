#include "pch.h"
#include "CppUnitTest.h"

#include <filesystem>

#include "../PythonWalker/PythonWalker.h"
#include "CodeGenStrings.h"
#include <fstream>
#include <list>

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
class PyPosition : public PythonWalker::ObjectInstance {
public:
	using PythonWalker::ObjectInstance::ObjectInstance;
	PyPosition() : PythonWalker::ObjectInstance("TestSnake", "Position") {};
	PyPosition(double x, double y) : PyPosition() {
		X = x;
		Y = y;
	};
	__PYW_TYPING_VAR(double, X)
	__PYW_TYPING_VAR(double, Y)
		__PYW_TYPING_CLASS_METHOD(double, pythag)
};

class TestPythonClass : public PythonWalker::ObjectInstance {
public:
	using PythonWalker::ObjectInstance::ObjectInstance;
	TestPythonClass() : PythonWalker::ObjectInstance("TestSnake", "TestSnake") {};

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

	__PYW_TYPING_CLASS_METHOD(void, increaseAgeByOne)
		__PYW_TYPING_CLASS_METHOD(void, functionThatHitsTypeError)
		__PYW_TYPING_CLASS_METHOD(double, calculateTerrariumArea)
		__PYW_TYPING_CLASS_METHOD(void, changeName, const char*, newName)
		__PYW_TYPING_CLASS_METHOD(double, AddThreeParametersTogether, int, a, float, b, double, c)

	// Inherently checks that we can overload function parameters like this,
	//   which end up calling into the same python function.
		__PYW_TYPING_CLASS_METHOD(double, AddNumbersTogether, std::vector<double>, numbers)
		__PYW_TYPING_CLASS_METHOD(double, AddNumbersTogether, std::list<double>, numbers)
		__PYW_TYPING_CLASS_METHOD(std::vector<const char *>, getFriends)
		__PYW_TYPING_CLASS_METHOD(std::vector<int>, countToFive)
	
		__PYW_TYPING_CLASS_METHOD(int, getGlobalVar)
		__PYW_TYPING_CLASS_METHOD(void, setGlobalVar, int, value)
		__PYW_TYPING_CLASS_METHOD(void, fakeFunction)
		__PYW_TYPING_CLASS_METHOD(void, noParamFunction, int, fakeParam)
		__PYW_TYPING_CLASS_METHOD(void, printMessage, std::string, msg)
		__PYW_TYPING_CLASS_METHOD(double, GetPositionXIndirectly)
};
class TestRobotClass : public PythonWalker::ObjectInstance {
public:
	using PythonWalker::ObjectInstance::ObjectInstance;

	TestRobotClass() : PythonWalker::ObjectInstance("GeneratedFiles.Robot", "Robot") {};
	__PYW_TYPING_VAR(int, age)
	__PYW_TYPING_CLASS_METHOD(float, divide, float, dividend, float, divisor)
};

/* Python typing function that multiplies an internal global Python variable by two */
__PYW_TYPING_METHOD("TestModule", void, moduleFunctionMultplyGlobalByTwo)
/* Python typing function that returns a string */
__PYW_TYPING_METHOD("TestModule", std::string, moduleFunctionReturnHelloWorld)
/* Python typing function that takes two numbers and returns the result of them multiplied together */
__PYW_TYPING_METHOD("TestModule", int, moduleFunctionMultply, int, num1, int, num2)

std::filesystem::path currentPath = std::filesystem::current_path();
std::string currentPathString = currentPath.make_preferred().string();
static PythonWalker::ScriptManager scriptManager;

TEST_MODULE_INITIALIZE(ClassInitialize)
{
	PythonWalker::Initialize();
	scriptManager = PythonWalker::ScriptManager(std::vector<std::string>({
		currentPathString + "\\..\\..\\UnitTests\\UnitTestsPythonScripts\\",
		currentPathString
		}));
}

namespace PythonInterfaceTestCases
{
	TEST_CLASS(PythonModuleInterfaceTestClass)
	{
	public:

		TestPythonClass testSnake;
		TEST_METHOD(LoadModule)
		{
			PyObject* module = PythonWalker::Module::Load("TestModule");
			Assert::IsNotNull(module);
		}
		TEST_METHOD(GetModuleGlobal)
		{
			PyObject* module = PythonWalker::Module::Load("TestModule");
			std::string helloWorld = "HelloWorld";
			std::string globalValue = PythonWalker::Module::GetGlobal<std::string>(module, "helloWorld");

			Assert::AreEqual(helloWorld, globalValue);
		}
		TEST_METHOD(GetAndSetModuleGlobal)
		{
			PyObject* module = PythonWalker::Module::Load("TestModule");
			int gloVar1 = 1, gloVar2 = 2, gloVar3 = 3;

			PythonWalker::Module::SetGlobal<int>(module, "gloVar", gloVar1);
			Assert::AreEqual(gloVar1, PythonWalker::Module::GetGlobal<int>(module, "gloVar"));

			PythonWalker::Module::SetGlobal<int>(module, "gloVar", gloVar2);
			Assert::AreEqual(gloVar2, PythonWalker::Module::GetGlobal<int>(module, "gloVar"));

			PythonWalker::Module::SetGlobal<int>("TestModule", "gloVar", gloVar3);
			Assert::AreEqual(gloVar3, PythonWalker::Module::GetGlobal<int>("TestModule", "gloVar"));
		}
		
		TEST_METHOD(ModifyModuleGlobalViaFunction)
		{
			int gloVar = 2;
			int expected = gloVar * 2;
			PyObject* module = PythonWalker::Module::Load("TestModule");

			PythonWalker::Module::SetGlobal<int>(module, "gloVar", gloVar);
			Assert::AreEqual(gloVar, PythonWalker::Module::GetGlobal<int>(module, "gloVar"));

			moduleFunctionMultplyGlobalByTwo();

			int result = PythonWalker::Module::GetGlobal<int>(module, "gloVar");
			Assert::AreEqual(expected, result);
		}

		TEST_METHOD(RunModuleFunctionWithReturn)
		{
			std::string expected = "HelloWorld";
			std::string result = moduleFunctionReturnHelloWorld();
			Assert::AreEqual(expected, result);
		}

		TEST_METHOD(RunModuleFunctionWithParameters)
		{
			int num1 = 2, num2 = 3, expected = num1 * num2;
			int result = moduleFunctionMultply(num1, num2);
			Assert::AreEqual(expected, result);
		}
	};
	TEST_CLASS(PythonObjectInterfaceTestClass)
	{
	public:
		TestPythonClass testSnake;
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
			PythonWalker::ClassDefinition def(pos.PyObjectInstance);
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
			// Overwrite old entry with new entry
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
		TEST_METHOD(VectorAsParameter)
		{
			std::vector<double> vectorNumbers = std::vector<double>{
				1, 2, 4, 8, 16
			};
			double sum = 0;
			for (double i : vectorNumbers) sum += i;
			double result = testSnake.AddNumbersTogether(vectorNumbers);
			Assert::AreEqual(sum, result);
		}
		TEST_METHOD(ListAsParameter)
		{
			std::list<double> listNumbers = std::list<double>{
				1, 2, 4, 8, 16, 32
			};
			double sum = 0;
			for (double i : listNumbers) sum += i;
			double result = testSnake.AddNumbersTogether(listNumbers);
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
		TestPythonClass testSnake;
		TEST_METHOD(FakeVariableException)
		{
			auto func = [this] {testSnake.fakeInt = 1; };
			Assert::ExpectException<PythonWalker::PythonAttributeDNE>(func);
		}
		TEST_METHOD(FakeFunctionException)
		{
			auto func = [this] {testSnake.fakeFunction(); };
			Assert::ExpectException<PythonWalker::PythonMethodDNE>(func);
		}
		TEST_METHOD(FakeFunctionParametersException)
		{
			auto func = [this] {testSnake.noParamFunction(4); };
			Assert::ExpectException<PythonWalker::PythonMethodError>(func);
		}
		TEST_METHOD(FunctionIssue)
		{
			auto func = [this] {testSnake.functionThatHitsTypeError(); };
			Assert::ExpectException<PythonWalker::PythonMethodError>(func);
		}
		TEST_METHOD(ModuleDNE)
		{
			auto func = [] {TestPythonClass fakeModuleSnake = TestPythonClass("FakeModule", "TestSnake"); };
			Assert::ExpectException<PythonWalker::PythonModuleDNE>(func);
		}
		TEST_METHOD(ModuleNotInitialized)
		{
			auto func = [] {PythonWalker::CreateObject(nullptr, "TestSnake"); };
			Assert::ExpectException<PythonWalker::PythonModuleNotInitialized>(func);
		}
		TEST_METHOD(ObjectDNE)
		{
			auto func = [] {TestPythonClass throwAway = TestPythonClass("TestSnake", "FakeName"); };
			Assert::ExpectException<PythonWalker::PythonObjectDNE>(func);
		}
	};
}
namespace PythonFileTestCases
{
	PythonWalker::ClassDefinition PlantDefinition = PythonWalker::ClassDefinition("TestPlant", "TestPlant");
	PythonWalker::ClassDefinition DuplicatePlantDefinition = PythonWalker::ClassDefinition("TestBasil", "TestPlant");
	PythonWalker::ClassDefinition BasilDefinition = PythonWalker::ClassDefinition("TestBasil", "TestBasil");
	PythonWalker::ClassDefinition ThaiBasilDefinition = PythonWalker::ClassDefinition("TestThaiBasil", "TestThaiBasil");
	PythonWalker::ClassDefinition CatDefinition = PythonWalker::ClassDefinition("TestPet", "TestCat");
	PythonWalker::ClassDefinition DogDefinition = PythonWalker::ClassDefinition("TestPet", "TestDog");
	PythonWalker::ClassDefinition OreganoDefinition = PythonWalker::ClassDefinition("Subfolder.TestOregano", "TestOregano");
	PythonWalker::ClassDefinition MexicanOreganoDefinition = PythonWalker::ClassDefinition("Subfolder.TestMexicanOregano", "TestMexicanOregano");
	PythonWalker::ClassDefinition OreganoDefinitionWithoutSubfolder = PythonWalker::ClassDefinition("TestOregano", "TestOregano");
	PythonWalker::ClassDefinition RosemaryDefinition = PythonWalker::ClassDefinition("Subfolder.Subfolder2.TestRosemary", "TestRosemary");

	PythonWalker::ClassDefinition MushroomDefinition = PythonWalker::ClassDefinition("Subfolder.TestMushroom", "TestMushroom");
	PythonWalker::ClassDefinition OysterMushroomDefinition = PythonWalker::ClassDefinition("TestOysterMushroom", "TestOysterMushroom");
	std::string NonExistantClassName = "NonExistantClassName";
	TEST_CLASS(PythonFileManagementTestClass)
	{
	public:
		TEST_METHOD(FindClasses)
		{
			std::vector<PythonWalker::ClassDefinition> scripts = scriptManager.GetScripts();
			Assert::IsTrue(VectorContains<PythonWalker::ClassDefinition>(scripts, BasilDefinition));
		}
		TEST_METHOD(FindClassesNoDups)
		{
			std::vector<PythonWalker::ClassDefinition> scripts = scriptManager.GetScripts();
			Assert::IsTrue(VectorContains<PythonWalker::ClassDefinition>(scripts, PlantDefinition));
			Assert::IsFalse(VectorContains<PythonWalker::ClassDefinition>(scripts, DuplicatePlantDefinition));
		}
		TEST_METHOD(FindTwoClassesInOneModule)
		{
			std::vector<PythonWalker::ClassDefinition> scripts = scriptManager.GetScripts();
			Assert::IsTrue(VectorContains<PythonWalker::ClassDefinition>(scripts, CatDefinition));
			Assert::IsTrue(VectorContains<PythonWalker::ClassDefinition>(scripts, DogDefinition));
		}
		TEST_METHOD(FindClassesRecursive)
		{
			std::vector<PythonWalker::ClassDefinition> scripts = scriptManager.GetScripts(true);
			Assert::IsTrue(VectorContains<PythonWalker::ClassDefinition>(scripts, BasilDefinition));
			Assert::IsTrue(VectorContains<PythonWalker::ClassDefinition>(scripts, OreganoDefinition));
			Assert::IsTrue(VectorContains<PythonWalker::ClassDefinition>(scripts, RosemaryDefinition));

			scripts = scriptManager.GetScripts(false);
			Assert::IsTrue(VectorContains<PythonWalker::ClassDefinition>(scripts, BasilDefinition));
			Assert::IsFalse(VectorContains<PythonWalker::ClassDefinition>(scripts, OreganoDefinition));
			Assert::IsFalse(VectorContains<PythonWalker::ClassDefinition>(scripts, RosemaryDefinition));
		}
		TEST_METHOD(RecursiveClassIncludesFullModule)
		{
			std::vector<PythonWalker::ClassDefinition> scripts = scriptManager.GetScripts(true);
			Assert::IsTrue(VectorContains<PythonWalker::ClassDefinition>(scripts, OreganoDefinition));
			Assert::IsFalse(VectorContains<PythonWalker::ClassDefinition>(scripts, OreganoDefinitionWithoutSubfolder));
		}
		TEST_METHOD(FindDerivedClasses)
		{
			std::vector<PythonWalker::ClassDefinition> scripts = scriptManager.GetScripts(false, PlantDefinition.ClassName);
			Assert::IsTrue(VectorContains<PythonWalker::ClassDefinition>(scripts, BasilDefinition));
			Assert::IsFalse(VectorContains<PythonWalker::ClassDefinition>(scripts, CatDefinition));

			scripts = scriptManager.GetScripts(false, NonExistantClassName);
			Assert::IsTrue(scripts.empty());
		}
		TEST_METHOD(FindDerivedClassesWithBaseInSubfolder)
		{
			std::vector<PythonWalker::ClassDefinition> scripts = scriptManager.GetScripts(true, OreganoDefinition.ClassName);
			Assert::IsTrue(VectorContains<PythonWalker::ClassDefinition>(scripts, MexicanOreganoDefinition));
		}
		TEST_METHOD(ExcludeBaseClassWhenSearchingForDerivedClasses)
		{
			std::vector<PythonWalker::ClassDefinition> scripts = scriptManager.GetScripts(false, PlantDefinition.ClassName);
			Assert::IsFalse(VectorContains<PythonWalker::ClassDefinition>(scripts, PlantDefinition));
		}
		TEST_METHOD(FindIndirectDerrivedClasses)
		{
			std::vector<PythonWalker::ClassDefinition> scripts = scriptManager.GetScripts(false, PlantDefinition.ClassName);
			Assert::IsTrue(VectorContains<PythonWalker::ClassDefinition>(scripts, ThaiBasilDefinition));
		}
		TEST_METHOD(ExcludeModule)
		{
			std::vector<std::string> excludedModules = {
				"Subfolder"
			};
			std::vector<PythonWalker::ClassDefinition> scripts = scriptManager.GetScripts(true, "", excludedModules);
			Assert::IsFalse(VectorContains<PythonWalker::ClassDefinition>(scripts, MushroomDefinition));
			Assert::IsTrue(VectorContains<PythonWalker::ClassDefinition>(scripts, OysterMushroomDefinition));
		}
		TEST_METHOD(DeriveFromExcludedModuleClass)
		{
			std::vector<std::string> excludedModules = {
				"Subfolder"
			};
			std::vector<PythonWalker::ClassDefinition> scripts = scriptManager.GetScripts(true, MushroomDefinition.ClassName, excludedModules);
			Assert::IsFalse(VectorContains<PythonWalker::ClassDefinition>(scripts, MushroomDefinition));
			Assert::IsTrue(VectorContains<PythonWalker::ClassDefinition>(scripts, OysterMushroomDefinition));
			Assert::IsFalse(VectorContains<PythonWalker::ClassDefinition>(scripts, BasilDefinition));
		}
	};
}

namespace CodeGenerationTestCases
{
	TEST_CLASS(CodeGenerationTestClass)
	{
	public:
		TEST_METHOD_CLEANUP(CleanupCodeGenMethod)
		{
			PythonWalker::CodeGeneration::DeletePythonModule(currentPath, "GeneratedFiles", true);
		}
		PythonWalker::ClassDefinition RobotDefinition = PythonWalker::ClassDefinition("GeneratedFiles.Robot", "Robot");
		PythonWalker::ClassDefinition RobotTwoDefinition = PythonWalker::ClassDefinition("GeneratedFiles.Robot", "RobotTwo");
		TEST_METHOD(GenerateModuleAndClass)
		{
			std::filesystem::path path = PythonWalker::CodeGeneration::GeneratePythonClass(currentPath, RobotDefinition, RobotCodeString);
			std::vector<PythonWalker::ClassDefinition> scripts = scriptManager.GetScripts(true);
			Assert::IsTrue(VectorContains<PythonWalker::ClassDefinition>(scripts, RobotDefinition));

			std::string result = PythonWalker::ScriptManager::GetFileContents(path);
			Assert::AreEqual(RobotCodeString, result);
		}
		TEST_METHOD(DeleteScriptModule)
		{
			PythonWalker::CodeGeneration::GeneratePythonClass(currentPath, RobotDefinition, RobotCodeString);
			std::vector<PythonWalker::ClassDefinition> scripts = scriptManager.GetScripts(true);
			Assert::IsTrue(VectorContains<PythonWalker::ClassDefinition>(scripts, RobotDefinition));

			PythonWalker::CodeGeneration::DeletePythonModule(currentPath, RobotDefinition);

			scripts = scriptManager.GetScripts(true);
			Assert::IsFalse(VectorContains<PythonWalker::ClassDefinition>(scripts, RobotDefinition));
		}
		TEST_METHOD(DeleteDirectoryModule)
		{
			PythonWalker::CodeGeneration::GeneratePythonClass(currentPath, RobotDefinition, RobotCodeString);
			std::vector<PythonWalker::ClassDefinition> scripts = scriptManager.GetScripts(true);
			Assert::IsTrue(VectorContains<PythonWalker::ClassDefinition>(scripts, RobotDefinition));

			PythonWalker::CodeGeneration::DeletePythonModule(currentPath, RobotDefinition, true);

			scripts = scriptManager.GetScripts(true);
			Assert::IsFalse(VectorContains<PythonWalker::ClassDefinition>(scripts, RobotDefinition));
		}
		TEST_METHOD(ModuleKilledWhenDeleted)
		{
			PythonWalker::CodeGeneration::GeneratePythonClass(currentPath, RobotDefinition, RobotCodeString);
			std::vector<PythonWalker::ClassDefinition> scripts = scriptManager.GetScripts(true);
			Assert::IsTrue(VectorContains<PythonWalker::ClassDefinition>(scripts, RobotDefinition));

			PythonWalker::CodeGeneration::DeletePythonModule(currentPath, RobotDefinition);

			scripts = scriptManager.GetScripts(true);
			Assert::IsFalse(VectorContains<PythonWalker::ClassDefinition>(scripts, RobotDefinition));
		}
		/*
		TEST_METHOD(ModuleResetWhenUpdated)
		{
			PythonWalker::CodeGeneration::GeneratePythonClass(currentPath, RobotDefinition, RobotCodeString);
			std::vector<PythonWalker::ClassDefinition> scripts = pythonWalker.GetScripts(true);
			Assert::IsTrue(VectorContains<PythonWalker::ClassDefinition>(scripts, RobotDefinition));

			// Doesn't appear to be a good way to remove module items during runtime, only add.
			//   Robot two string, which hasn't been added yet in this case, carries over from past runs.
			//Assert::IsFalse(VectorContains<PythonWalker::ClassDefinition>(scripts, RobotTwoDefinition));

			PythonWalker::CodeGeneration::GeneratePythonClass(currentPath, RobotTwoDefinition, RobotTwoCodeString);
			scripts = pythonWalker.GetScripts(true);
			Assert::IsTrue(VectorContains<PythonWalker::ClassDefinition>(scripts, RobotDefinition));
			Assert::IsTrue(VectorContains<PythonWalker::ClassDefinition>(scripts, RobotTwoDefinition));
		}
		*/
		TEST_METHOD(GenerateModuleAndTwoClasses)
		{
			std::filesystem::path filePath = PythonWalker::CodeGeneration::GeneratePythonClass(currentPath, RobotDefinition, RobotCodeString);
			std::filesystem::path filePathTwo = PythonWalker::CodeGeneration::GeneratePythonClass(currentPath, RobotTwoDefinition, RobotTwoCodeString, true);
			
			Assert::IsTrue(filePath == filePathTwo);
			
			std::vector<PythonWalker::ClassDefinition> scripts = scriptManager.GetScripts(true);
			Assert::IsTrue(VectorContains<PythonWalker::ClassDefinition>(scripts, RobotDefinition));
			Assert::IsTrue(VectorContains<PythonWalker::ClassDefinition>(scripts, RobotTwoDefinition));

			std::string result = PythonWalker::ScriptManager::GetFileContents(filePath);
			Assert::AreEqual(RobotCodeString + RobotTwoCodeString, result);
		}
		TEST_METHOD(OverwriteFileWithoutAppending)
		{
			std::filesystem::path filePath = PythonWalker::CodeGeneration::GeneratePythonClass(currentPath, RobotDefinition, RobotCodeString);
			std::string firstGeneration = PythonWalker::ScriptManager::GetFileContents(filePath);
			Assert::AreEqual(firstGeneration, RobotCodeString);

			PythonWalker::CodeGeneration::GeneratePythonClass(currentPath, RobotTwoDefinition, RobotTwoCodeString);
			std::string secondGeneration = PythonWalker::ScriptManager::GetFileContents(filePath);

			Assert::AreNotEqual(firstGeneration, secondGeneration);
			Assert::AreEqual(secondGeneration, RobotTwoCodeString);
		}
		TEST_METHOD(RegenerateClassWithChange)
		{
			PythonWalker::CodeGeneration::GeneratePythonClass(currentPath, RobotDefinition, RobotCodeWithZeroErrorString);
			TestRobotClass robot = RobotDefinition.GetNewObject();
			float expected = 0.0, result;

			// Run the function and make sure it errors
			auto func = [&] {robot.divide(1.0, 0.0); };
			Assert::ExpectException<PythonWalker::PythonMethodError>(func);
			
			PythonWalker::CodeGeneration::GeneratePythonClass(currentPath, RobotDefinition, RobotCodeWithZeroErrorFixedString);

			robot.RegenerateFromScript();

			// Run the function without errors after regenerating it
			result = robot.divide(1.0, 0.0);
			Assert::AreEqual(expected, result);
		}
		TEST_METHOD(DetectFileChange)
		{
			std::filesystem::path filePath = PythonWalker::CodeGeneration::GeneratePythonClass(currentPath, RobotDefinition, RobotCodeString);

			std::string fileContents = PythonWalker::ScriptManager::GetFileContents(filePath);

			Assert::IsFalse(PythonWalker::ScriptManager::HasFileChanged(filePath, fileContents));

			PythonWalker::CodeGeneration::GeneratePythonClass(currentPath, RobotDefinition, RobotTwoCodeString);

			Assert::IsTrue(PythonWalker::ScriptManager::HasFileChanged(filePath, fileContents));
		}
		TEST_METHOD(AttemptToDeleteOpenFile)
		{
			std::filesystem::path filePath = PythonWalker::CodeGeneration::GeneratePythonClass(currentPath, RobotDefinition, RobotCodeString);

			std::ofstream newFile(filePath.string(), std::ios::app);

			// Check that this doesn't throw an error by trying to delete while the file is open
			PythonWalker::CodeGeneration::DeletePythonModule(currentPath, RobotDefinition);

			newFile.close();

			// Confirm that we didn't delete the file
			std::string fileContents = PythonWalker::ScriptManager::GetFileContents(filePath);
			Assert::IsTrue(fileContents == RobotCodeString);
		}
		TEST_METHOD(GenerateToOpenFile)
		{
			std::filesystem::path filePath = PythonWalker::CodeGeneration::GeneratePythonClass(currentPath, RobotDefinition, RobotCodeString);

			std::ofstream newFile(filePath.string(), std::ios::app);

			PythonWalker::CodeGeneration::GeneratePythonClass(currentPath, RobotDefinition, RobotTwoCodeString);
			std::string fileContents = PythonWalker::ScriptManager::GetFileContents(filePath);
			Assert::AreEqual(RobotTwoCodeString, fileContents, L"Overwrote file contents when file was open");

			PythonWalker::CodeGeneration::GeneratePythonClass(currentPath, RobotDefinition, RobotCodeString, true);
			fileContents = PythonWalker::ScriptManager::GetFileContents(filePath);
			Assert::AreEqual(RobotTwoCodeString + RobotCodeString, fileContents, L"Appended to file contents when file was open");

			newFile.close();
		}
		TEST_METHOD(KeepVariableValueThroughRegeneration)
		{
			PythonWalker::CodeGeneration::GeneratePythonClass(currentPath, RobotDefinition, RobotCodeWithZeroErrorString);
			TestRobotClass robot = RobotDefinition.GetNewObject();
			int expected = 10;
			robot.age = expected;
			Assert::AreEqual(expected, robot.age);

			robot.RegenerateFromScript();

			Assert::AreEqual(expected, robot.age);
		}
		
		TEST_METHOD(KeepVariableValueThroughSourceUpdate)
		{
			PythonWalker::CodeGeneration::GeneratePythonClass(currentPath, RobotDefinition, RobotCodeWithZeroErrorString);
			TestRobotClass robot = RobotDefinition.GetNewObject();
			int expected = 10;
			robot.age = expected;
			Assert::AreEqual(expected, robot.age);

			PythonWalker::CodeGeneration::GeneratePythonClass(currentPath, RobotDefinition, RobotCodeWithZeroErrorFixedString);

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
		TestPythonClass testSnake;
		std::filesystem::path LogDirectory = currentPath / "Logs";
		std::filesystem::path LogFilePath = LogDirectory / "LogFile.txt";
		std::string LoggingMessage = "My unit testing message";

		TEST_METHOD_CLEANUP(CleanupLogTesting)
		{
			PythonWalker::Logging::CloseLoggingContext(true);
			PythonWalker::ScriptManager::ClearDirectory(LogDirectory);
		}
		TEST_METHOD(PrintToLogFile)
		{
			// Make sure the log file starts not created created
			Assert::IsFalse(std::filesystem::exists(LogFilePath));

			PythonWalker::Logging::StartLoggingContext(LogFilePath);

			// Make sure the log file was created
			Assert::IsTrue(std::filesystem::exists(LogFilePath));

			testSnake.printMessage(LoggingMessage);
			PythonWalker::Logging::FlushLoggingContext();

			std::string result = PythonWalker::ScriptManager::GetFileContents(LogFilePath);
			std::string expected = LoggingMessage + "\n";
			Assert::AreEqual(expected, result);
		}
		TEST_METHOD(ExceptionToLogFile)
		{
			PythonWalker::Logging::StartLoggingContext(LogFilePath);
			try {
				testSnake.functionThatHitsTypeError();
			}
			catch (PythonWalker::PythonMethodError e) { }
			PythonWalker::Logging::FlushLoggingContext();

			std::string result = PythonWalker::ScriptManager::GetFileContents(LogFilePath);

			// Check that the log file returned a type error
			Assert::IsTrue(result.find("TypeError") != std::string::npos);
		}
		TEST_METHOD(PrintToLogFileMiltipleTimes)
		{
			PythonWalker::Logging::StartLoggingContext(LogFilePath);
			testSnake.printMessage(LoggingMessage);
			PythonWalker::Logging::CloseLoggingContext(false);

			PythonWalker::Logging::StartLoggingContext(LogFilePath);
			testSnake.printMessage(LoggingMessage);
			PythonWalker::Logging::FlushLoggingContext();

			std::string result = PythonWalker::ScriptManager::GetFileContents(LogFilePath);
			std::string expected = LoggingMessage + "\n" + LoggingMessage + "\n";
			Assert::AreEqual(expected, result);
		}
		TEST_METHOD(ClearLogFile)
		{
			PythonWalker::Logging::StartLoggingContext(LogFilePath);
			testSnake.printMessage(LoggingMessage);
			PythonWalker::Logging::FlushLoggingContext();

			std::string result = PythonWalker::ScriptManager::GetFileContents(LogFilePath);
			std::string expected = LoggingMessage + "\n";
			Assert::AreEqual(expected, result);

			PythonWalker::Logging::CloseLoggingContext(true);

			Assert::IsFalse(std::filesystem::exists(LogFilePath));
		}
		TEST_METHOD(LogToTemporaryFile)
		{
			PythonWalker::Logging::StartLoggingContext();
			std::filesystem::path tempLogPath = PythonWalker::Logging::GetCurrentLogFile();

			testSnake.printMessage(LoggingMessage);

			PythonWalker::Logging::FlushLoggingContext();

			std::string result = PythonWalker::ScriptManager::GetFileContents(tempLogPath);

			// Confirm we logged the message when we didn't pass in a file
			std::string expected = LoggingMessage + "\n";
			Assert::AreEqual(expected, result);

			PythonWalker::Logging::CloseLoggingContext();

			// Confirm the temp file is deleted after closing the logging context
			Assert::IsFalse(std::filesystem::exists(tempLogPath));
		}
		TEST_METHOD(StartLoggingContextWhileOneIsOpen)
		{
			PythonWalker::Logging::StartLoggingContext(LogFilePath);

			// Start a new logging context while the old one is still open
			auto func = [] {PythonWalker::Logging::StartLoggingContext(); };
			Assert::ExpectException<PythonWalker::PythonLoggingContextAlreadyOpen>(func);
		}
	};
	TEST_CLASS(PythonReplayTestClass)
	{
	public:
		std::filesystem::path LogDirectory = currentPath / "Logs";
		std::filesystem::path LogFilePath = LogDirectory / "LogFileReplay.txt";
		TEST_METHOD_CLEANUP(CleanupReplayTesting)
		{
			PythonWalker::Logging::CloseLoggingContext();
			PythonWalker::ScriptManager::ClearDirectory(LogDirectory);
		}
		TEST_METHOD(SupportReplay)
		{
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