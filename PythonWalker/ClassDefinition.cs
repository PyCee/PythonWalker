using Python.Runtime;
using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using System.Threading.Tasks;

namespace PythonWalker
{
	[PythonMapping]
	public class ClassDefinition
	{
		public string ModuleName;
		public string ClassName;
		public ClassDefinition(): this("", "")
		{

		}
		public ClassDefinition(string module, string className)
		{
			ModuleName = module;
			ClassName = className;
		}
		public ClassDefinition(PyObject pyObject)
		{
			ModuleName = GetModuleName(pyObject);
			PyObject classRef = pyObject.GetAttr("__class__");
			ClassName = classRef.GetAttr("__name__").ToString() ?? "";
		}
		public static string GetModuleName(PyObject pyObject)
		{
			return pyObject.GetAttr("__module__").ToString() ?? "";
		}
		public override bool Equals(object? obj)
		{
			return obj is ClassDefinition classDef && 
				ModuleName.Equals(classDef.ModuleName) && ClassName.Equals(classDef.ClassName);
		}
		public override int GetHashCode()
		{
			return HashCode.Combine(ModuleName, ClassName);
		}

		public static bool operator ==(ClassDefinition? left, ClassDefinition? right)
		{
			if (ReferenceEquals(left, right))
				return true;

			if (left is null || right is null)
				return false;

			return left.ModuleName == right.ModuleName && left.ClassName == right.ClassName;
		}
		public static bool operator !=(ClassDefinition? left, ClassDefinition? right)
		{
			return !(left == right);
		}
		public override string ToString()
		{
			return ModuleName + "." + ClassName;
		}
		public bool IsValid()
		{
			return !string.IsNullOrEmpty(ModuleName) && !string.IsNullOrEmpty(ClassName);
		}
		public PyObject GetPythonObject()
		{
			PythonModule mod = new (ModuleName);
			return mod.Invoke(ClassName);
		}
		public PyObject GetPythonClass()
		{
			PythonModule mod = new (ModuleName);
			return mod.pyObject.GetAttr(ClassName);
		}
	}
}
