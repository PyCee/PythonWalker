using Microsoft.CodeAnalysis;
using Microsoft.CodeAnalysis.CSharp;
using PythonMappingGeneration;

namespace UnitTests
{
	public class CodeGenerationUnitTests
	{
		[Fact]
		public void CodeGenerationShouldWork()
		{
			string currentPath = Environment.CurrentDirectory;
			//string testGenerationClassFilePath = currentPath + @"\..\..\..\HelperFiles\TestGenerationClass.cs";
			string testGenerationClassFilePath = currentPath + @"\..\..\..\..\TestWalk\ManualTesting.cs";
			string codeToTest = File.ReadAllText(testGenerationClassFilePath);
			Compilation inputCompilation = CSharpCompilation.Create("TestProject", [CSharpSyntaxTree.ParseText(codeToTest)]);

			PythonMappingGeneration.PythonMappingGeneration gen = new();

			GeneratorDriver driver = CSharpGeneratorDriver.Create(gen);

			driver = driver.RunGenerators(inputCompilation);
			GeneratorDriverRunResult result = driver.GetRunResult();
		}
	}
}