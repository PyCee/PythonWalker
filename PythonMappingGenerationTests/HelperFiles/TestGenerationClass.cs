using PythonWalker;

namespace UnitTests.HelperFiles
{
	[PythonMapping(DefaultModule = "TestSnake", DefaultClass = "TestSnake")]
	public partial class TestGenerationClass
	{
		[PythonProperty]
		public int _Age { get; set; }

		[PythonProperty]
		public int _Name { get; set; }

		[PythonMethod]
		public partial int MethodName(int intParameter, bool boolParameter, string stringParameter);
	}
}


namespace UnitTests.HelperFiles
{
	public partial class TestGenerationClass
	{

		public partial int MethodName(int intParameter, bool boolParameter, string stringParameter)
		{
			return 0;
		}
	}
}