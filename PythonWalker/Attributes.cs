using System;
using System.Collections.Generic;
using System.ComponentModel.DataAnnotations;
using System.Diagnostics;
using System.Linq;
using System.Text;
using System.Threading.Tasks;

namespace PythonWalker
{
	/// <summary>
	/// Attribute to apply to a C# class that maps to a python class.
	/// </summary>
	[System.AttributeUsage(System.AttributeTargets.Class)]
	public class PythonMappingAttribute(string defaultModule, string defaultClass) : Attribute
	{
		public PythonMappingAttribute() : this("","")
		{
		}

		public string DefaultModule { get; set; } = defaultModule;
		public string DefaultClass { get; set; } = defaultClass;
	}
	/// <summary>
	/// Attribute to apply to a property for python mapping code generation.
	/// The first character will be trimmed off for the generated property. Recommend beginning with an underscore.
	/// Once trimmed, the name must match the mapped python property exactly.
	/// Must be a property with an empty body.
	/// Parent class must have the [PythonMapping] attribute.
	/// Example: public int _Age { get; set; }
	/// </summary>
	[System.AttributeUsage(System.AttributeTargets.Property)]
	public class PythonPropertyAttribute : ValidationAttribute
	{
	}
	/// <summary>
	/// Attribute to apply to a property for python mapping code generation.
	/// The first character will be trimmed off for the generated property. Recommend beginning with an underscore.
	/// Once trimmed, the name must match the mapped python property exactly.
	/// Must be a property with an empty body.
	/// Parent class must have the [PythonMapping] attribute.
	/// Example: public int _Age { get; set; }
	/// </summary>
	[System.AttributeUsage(System.AttributeTargets.Property)]
	public class NestedPythonPropertyAttribute : ValidationAttribute
	{
	}
	/// <summary>
	/// Attribute to apply to a method for python mapping code generation.
	/// Must match name of mapped python method exactly.
	/// Parent class must have the [PythonMapping] attribute.
	/// Must be defined with the 'partial' keyword without a method body.
	/// Example: public partial int CalculateAge();
	/// </summary>
	[System.AttributeUsage(System.AttributeTargets.Method)]
	public class PythonMethodAttribute : Attribute
	{

	}
}
