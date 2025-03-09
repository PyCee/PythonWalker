using Microsoft.CodeAnalysis;
using Microsoft.CodeAnalysis.CSharp;
using Microsoft.CodeAnalysis.CSharp.Syntax;
using Microsoft.CodeAnalysis.Text;
using System;
using System.Collections.Generic;
using System.Collections.Immutable;
using System.Diagnostics;
using System.Globalization;
using System.Linq;
using System.Reflection;
using System.Text;
using System.Xml.Linq;

namespace PythonMappingGeneration
{
	public class StringBuilderCodeGenerator
	{
		private string result = string.Empty;
		private int numIndents = 0;
		public StringBuilderCodeGenerator()
		{
		}
		public override string ToString() { return result; }

		public void Append(string text, bool indent = false)
		{
			result += (indent ? new string('\t', numIndents) : "") + text;
		}
		public void AppendLine(string text)	{ Append(text + '\n', true); }
		public void IncreaseIndent() { numIndents++; }
		public void DecreaseIndents() { numIndents--; }
	}
		public class PythonMappingClassRepresentation
	{
		static readonly Dictionary<SyntaxKind, string> attributeStringMapping = new Dictionary<SyntaxKind, string>{
			{SyntaxKind.MethodDeclaration, "PythonMethod"},
			{SyntaxKind.PropertyDeclaration, "PythonProperty"}
			};
		string ClassName;
		ClassDeclarationSyntax ClassSyntax;
		List<MethodDeclarationSyntax> PyMethods;
		List<PropertyDeclarationSyntax> PyProperties;
		List<PropertyDeclarationSyntax> PyNestedProperties;

		public PythonMappingClassRepresentation(GeneratorAttributeSyntaxContext context)
		{
			if(context.TargetNode is not ClassDeclarationSyntax classSyntax){
				return;
			}
			INamedTypeSymbol classSymbol = context.SemanticModel.GetDeclaredSymbol(classSyntax);

			ClassSyntax = classSyntax;
			ClassName = classSymbol.Name;

			PopulateMemberLists(classSyntax.Members);

		}
		/// <summary>
		/// Populates the primary member lists for methods and properties
		/// </summary>
		/// <param name="memberSyntaxList">The classes member declaration syntax list</param>
		public void PopulateMemberLists(SyntaxList<MemberDeclarationSyntax> memberSyntaxList)
		{
			PyMethods = new();
			PyProperties = new();
			PyNestedProperties = new();

			foreach (MemberDeclarationSyntax memberSyntax in memberSyntaxList)
			{
				SyntaxKind kind = memberSyntax.Kind();
				attributeStringMapping.TryGetValue(kind, out string attributeName);
				
				switch (kind)
				{
					case SyntaxKind.MethodDeclaration:
						if (SyntaxNodeHasAttribute(memberSyntax, attributeName))
						{
							PyMethods.Add(memberSyntax as MethodDeclarationSyntax);
						}
						break;
					case SyntaxKind.PropertyDeclaration:
						if (SyntaxNodeHasAttribute(memberSyntax, attributeName))
						{
							PyProperties.Add(memberSyntax as PropertyDeclarationSyntax);
						} else if (SyntaxNodeHasAttribute(memberSyntax, "NestedPythonProperty"))
						{
							PyNestedProperties.Add(memberSyntax as PropertyDeclarationSyntax);
						}
						break;
					default:
						break;
				}
			}
		}
		/// <summary>
		/// Checks if the given member's AttributeLists includes an attribute of the given name
		/// </summary>
		/// <param name="memberSyntax">Member declaration</param>
		/// <param name="attributeString">String of the member attribute's name. Ex: "PythonMethod", "PythonProperty"</param>
		/// <returns></returns>
		private bool SyntaxNodeHasAttribute(MemberDeclarationSyntax memberSyntax, string attributeString)
		{
			if(string.IsNullOrEmpty(attributeString)) return false;
			return memberSyntax.AttributeLists.Any(attributeListSyntax => 
				attributeListSyntax.Attributes.Any(attributeSyntax => attributeSyntax.ToFullString() == attributeString));
		}
		private AttributeSyntax GetAttribute(ClassDeclarationSyntax classSyntax, string attributeString)
		{
			if (string.IsNullOrEmpty(attributeString)) return null;
			foreach (var attributeListSyntax in classSyntax.AttributeLists)
			{
				foreach (AttributeSyntax attributeSyntax in attributeListSyntax.Attributes) {
					if(attributeSyntax.ToFullString().Contains(attributeString))
					{
						return attributeSyntax;
					}
				}
			}
			return null;
		}

		public string GetFileName()
		{
			return $"{ClassName}.g.cs";
		}
		public string GetSourceCode(Compilation compilation, SourceProductionContext context)
		{
			SemanticModel semanticModel = compilation.GetSemanticModel(ClassSyntax.SyntaxTree);

			StringBuilderCodeGenerator result = new();
			INamedTypeSymbol classSymbol = semanticModel.GetDeclaredSymbol(ClassSyntax);
			string namespaceName = classSymbol.ContainingNamespace.Name;
			result.AppendLine("using Python.Runtime;");
			result.AppendLine("using PythonWalker;");
			if (!string.IsNullOrEmpty(namespaceName))
			{
				result.AppendLine($"namespace {namespaceName} {{");
				result.IncreaseIndent();
			}
			string accessibilityModifier = SyntaxFacts.GetText(classSymbol.DeclaredAccessibility);
			result.AppendLine($"{accessibilityModifier} partial class {ClassName} : ClassMapping {{");
			result.IncreaseIndent();

			GenerateConstructors(result, semanticModel);

			GenerateProperties(result, semanticModel);

			GenerateMethods(result, semanticModel);

			GenerateCodeGenerator(result, semanticModel);
			result.DecreaseIndents();
			result.AppendLine("}");
			if (!string.IsNullOrEmpty(namespaceName))
			{
				result.DecreaseIndents();
				result.AppendLine("}");
			}
			return result.ToString();
		}
		private void GenerateConstructors(StringBuilderCodeGenerator result, SemanticModel semanticModel)
		{
			AttributeSyntax mappingAttribute = GetAttribute(ClassSyntax, "PythonMapping");
			string defaultModule = "";
			string defaultClass = "";
			if (mappingAttribute?.ArgumentList != null)
			{
				foreach (AttributeArgumentSyntax argumentSyntax in mappingAttribute.ArgumentList.Arguments)
				{
					string name = "";
					if (argumentSyntax.NameColon != null)
					{
						name = argumentSyntax.NameColon.Name.ToString();
					}
					else if (argumentSyntax.NameEquals != null)
					{
						name = argumentSyntax.NameEquals.Name.ToString();
					}
					else
					{
						continue;
					}
					string expression = argumentSyntax.Expression.ToString();
					switch (name)
					{
						case "DefaultModule":
							defaultModule = expression;
							break;
						case "DefaultClass":
							defaultClass = expression;
							break;
						default:
							break;
					}
				}
			}
			if (string.IsNullOrEmpty(defaultClass))
			{
				defaultClass = $"\"{ClassName}\"";
			}
			result.AppendLine($"public static readonly string DefaultClassName = {defaultClass};");
			if (!string.IsNullOrEmpty(defaultModule))
			{
				result.AppendLine($"public static readonly string DefaultModuleName = {defaultModule};");
				result.AppendLine($"public {ClassName}() : base({defaultModule}, {defaultClass}) {{ }}");
			}

			result.AppendLine($"public {ClassName}(string defaultModule, string defaultClass) : base(defaultModule, defaultClass) {{ }}");
			result.AppendLine($"public {ClassName}(ClassMapping classMapping) : base(classMapping) {{ }}");
			result.AppendLine($"public {ClassName}(PyObject otherPyObject) : base(otherPyObject) {{ }}");
			result.AppendLine($"public {ClassName}({ClassName} copyObject) : base(copyObject) {{");

			foreach (PropertyDeclarationSyntax propertySyntax in PyNestedProperties)
			{
				IPropertySymbol propertySymbol = semanticModel.GetDeclaredSymbol(propertySyntax);
				string propertyName = propertySymbol.Name;

				result.IncreaseIndent();
				result.AppendLine($"{propertyName} = new(copyObject.{propertyName});");
				result.DecreaseIndents();
			}
			result.AppendLine("}");
			result.AppendLine("public override void Initialize() {");
			foreach (PropertyDeclarationSyntax propertySyntax in PyNestedProperties)
			{
				IPropertySymbol propertySymbol = semanticModel.GetDeclaredSymbol(propertySyntax);
				string propertyName = propertySymbol.Name;
				result.IncreaseIndent();
				result.AppendLine($"if({propertyName} == null) {{ {propertyName} = new(); }}");
				result.DecreaseIndents();
			}
			result.AppendLine("}");
		}
		private void GenerateProperties(StringBuilderCodeGenerator result, SemanticModel semanticModel)
		{
			foreach (PropertyDeclarationSyntax propertySyntax in PyProperties)
			{
				IPropertySymbol propertySymbol = semanticModel.GetDeclaredSymbol(propertySyntax);
				string propertyName = propertySymbol.Name;
				// Remove the first character since it'll be an underscore since we can't do partial properties until C# 13.0
				propertyName = propertyName.Substring(1);
				ITypeSymbol propertyTypeSymbol = propertySymbol.Type;
				string propertyType = propertyTypeSymbol.ToDisplayString();
				result.AppendLine($"public {propertyType} {propertyName}" + " {");

				result.IncreaseIndent();
				result.AppendLine($"get {{ return GetMappedPythonProperty<{propertyType}>(\"{propertyName}\"); }}");
				result.AppendLine($"set {{ SetMappedPythonProperty(\"{propertyName}\", value); }}");

				result.DecreaseIndents();
				result.AppendLine("}");
			}
		}
		private void GenerateMethods(StringBuilderCodeGenerator result, SemanticModel semanticModel)
		{
			foreach (MethodDeclarationSyntax methodSyntax in PyMethods)
			{
				IMethodSymbol methodSymbol = semanticModel.GetDeclaredSymbol(methodSyntax);
				string methodName = methodSymbol.Name;
				string methodAccessibilityModifier = SyntaxFacts.GetText(methodSymbol.DeclaredAccessibility);
				ITypeSymbol returnTypeSymbol = methodSymbol.ReturnType;
				string returnType = returnTypeSymbol.ToDisplayString();
				bool returnsValue = returnType != "void";

				List<string> parameterStrings = new List<string>();
				List<string> passInStrings = new List<string>();
				foreach (IParameterSymbol parameterSymbol in methodSymbol.Parameters)
				{
					string parameterType = parameterSymbol.Type.ToDisplayString();
					string parameterName = parameterSymbol.Name;
					parameterStrings.Add(parameterType + " " + parameterName);
					passInStrings.Add(parameterName);
				}

				result.AppendLine($"{methodAccessibilityModifier} partial {returnType} {methodName}(" + string.Join(", ", parameterStrings) + ") {");
				result.IncreaseIndent();
				string returnString = returnsValue ? "return " : string.Empty;
				string templateReturnType = (returnsValue) ? returnType : "object";
				string parametersString = parameterStrings.Count > 0 ? ", " + string.Join(", ", passInStrings) : string.Empty;
				result.AppendLine($"{returnString}RunMethod<{templateReturnType}>(\"{methodName}\"{parametersString});");
				result.DecreaseIndents();
				result.AppendLine("}");
			}
		}

		private void GeneratePythonMethodPass(StringBuilderCodeGenerator result, string methodDeclaration)
		{
			result.AppendLine(methodDeclaration);
			result.IncreaseIndent();
			result.AppendLine("pass");
			result.DecreaseIndents();
		}
		private void GenerateCodeGenerator(StringBuilderCodeGenerator result, SemanticModel semanticModel)
		{
			result.AppendLine("public static new string GetPythonBaseCode() {");
			result.IncreaseIndent();

			StringBuilderCodeGenerator pythonCode = new();
			pythonCode.AppendLine($"class {ClassName}(object):");
			pythonCode.IncreaseIndent();
			GeneratePythonMethodPass(pythonCode, "def __init__(self):");

			foreach (MethodDeclarationSyntax methodSyntax in PyMethods)
			{
				IMethodSymbol methodSymbol = semanticModel.GetDeclaredSymbol(methodSyntax);
				string methodName = methodSymbol.Name;
				List<string> passInStrings = new();
				foreach (IParameterSymbol parameterSymbol in methodSymbol.Parameters)
				{
					passInStrings.Add(parameterSymbol.Name);
				}
				string parametersString = passInStrings.Count > 0 ? ", " + string.Join(", ", passInStrings) : string.Empty;
				GeneratePythonMethodPass(pythonCode, $"def {methodName}(self{parametersString}):");
			}
			pythonCode.DecreaseIndents();

			result.AppendLine("return @\"" + pythonCode.ToString() + "\";");
			result.DecreaseIndents();
			result.AppendLine("}");
		}

	}
	[Generator]
	public class PythonMappingGeneration : IIncrementalGenerator
	{
		static string logMessage = "";
		private void Log(string message)
		{
			logMessage += "// " + message + "\n";
		}
		private void OutputLog(SourceProductionContext context)
		{
			if (logMessage.Length == 0)
			{
				return;
			}
			context.AddSource(
					$"MappingGeneration_Log.g.cs",
					logMessage
				);
		}
		public void Initialize(IncrementalGeneratorInitializationContext context)
		{
			//Debugger.Launch();
			IncrementalValuesProvider<PythonMappingClassRepresentation> classRepresentations = context.SyntaxProvider
			.ForAttributeWithMetadataName(
				"PythonWalker.PythonMappingAttribute",
				//"PythonMapping"
				predicate: (s, _) => IsSyntaxTargetForGeneration(s),
				transform: (ctx, _) => GetTargetForGeneration(ctx));

			IncrementalValueProvider<(Compilation, ImmutableArray<PythonMappingClassRepresentation>)> compliationAndClasses
				= context.CompilationProvider.Combine(classRepresentations.Collect());

			// TODO consider into using RegisterImplementationSourceOutput to not affect semantic meaning of the rest of the code
			context.RegisterSourceOutput(compliationAndClasses,
				(spc, source) => Execute(source.Item1, source.Item2, spc));

		}
		public bool IsSyntaxTargetForGeneration(SyntaxNode syntaxNode)
		{
			bool isClass = syntaxNode is ClassDeclarationSyntax;
			return isClass;
		}
		public PythonMappingClassRepresentation GetTargetForGeneration(GeneratorAttributeSyntaxContext context)
		{
			return new PythonMappingClassRepresentation(context);
		}
		/// <summary>
		/// 
		/// </summary>
		/// <param name="compilation"></param>
		/// <param name="classes"></param>
		/// <param name="context"></param>
		public void Execute(Compilation compilation,
			ImmutableArray<PythonMappingClassRepresentation> classes,
			SourceProductionContext context)
		{
			foreach (PythonMappingClassRepresentation classRepresentation in classes)
			{
				context.AddSource(
					classRepresentation.GetFileName(),
					classRepresentation.GetSourceCode(compilation, context)
				);
			}
			OutputLog(context);
		}
	}
}
