using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using System.Threading.Tasks;

namespace PythonWalker
{
	public static class PythonGeneration
	{
		/// <summary>
		/// Generates a python script, creating the file and directories if needed.
		/// </summary>
		/// <param name="module">The module that defines the script to generate.</param>
		/// <param name="code">The code that will make up the script.</param>
		/// <param name="append">
		///		Pass in true if the code should be appended to the end of the script.
		///		Otherwise this will overwrite any script contents.
		///	</param>
		public static void GenerateScript(string module, string code, bool append=false)
		{
			string path = PythonModule.GetAbsolutePath(module);
			ScriptManager.EnsurePathExists(path);

			if (string.IsNullOrEmpty(code))
			{
				return;
			}

			if (append)
			{
				File.AppendAllText(path, code);
			} else
			{
				File.WriteAllText(path, code);
			}

			PythonModule.Reload(module);
		}
	}
}
