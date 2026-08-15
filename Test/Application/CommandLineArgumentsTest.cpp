#include "stdafx.h"

#include "Application/CommandLineArgumentsTest.h"

#include "Felis/Application/CommandLineArguments.h"

namespace Test
{
namespace
{

void TestProgramAndRawArguments(TestReporter& r)
{
	char  program[] = "felis-app";
	char  option[]	= "--verbose";
	char* argV[]	= {program, option};
	int	  argC		= static_cast<int>(sizeof(argV) / sizeof(argV[0]));

	CommandLineArguments args(argC, argV);

	TEST_CHECK(r, args.GetArgC() == argC);
	TEST_CHECK(r, args.GetArgV() == argV);
	TEST_CHECK(r, args.GetProgramName() == "felis-app");
}

void TestFlagAndKeyValueArguments(TestReporter& r)
{
	char  program[] = "felis-app";
	char  verbose[] = "--verbose";
	char  port[]	= "--port=8080";
	char  empty[]	= "--empty=";
	char* argV[]	= {program, verbose, port, empty};
	int	  argC		= static_cast<int>(sizeof(argV) / sizeof(argV[0]));

	CommandLineArguments args(argC, argV);

	TEST_CHECK(r, args.HasArgument("verbose"));
	TEST_CHECK(r, args.GetArgument("verbose") == "1");

	bool	   verboseValue = false;
	const auto verboseError = args.GetArgumentAs("verbose", verboseValue);
	TEST_CHECK(r, !verboseError);
	TEST_CHECK(r, verboseValue);

	TEST_CHECK(r, args.HasArgument("port"));
	TEST_CHECK(r, args.GetArgument("port") == "8080");

	int		   portValue = 0;
	const auto portError = args.GetArgumentAs("port", portValue);
	TEST_CHECK(r, !portError);
	TEST_CHECK(r, portValue == 8080);

	TEST_CHECK(r, args.HasArgument("empty"));
	TEST_CHECK(r, args.GetArgument("empty").empty());
}

void TestPositionalArgumentsAndSeparator(TestReporter& r)
{
	char  program[]		= "felis-app";
	char  input[]		= "input.txt";
	char  mode[]		= "--mode=fast";
	char  separator[]	= "--";
	char  escaped[]		= "--literal";
	char  shortOption[] = "-x";
	char* argV[]		= {program, input, mode, separator, escaped, shortOption};
	int	  argC			= static_cast<int>(sizeof(argV) / sizeof(argV[0]));

	CommandLineArguments args(argC, argV);

	TEST_CHECK(r, args.GetArgument("mode") == "fast");
	TEST_CHECK(r, args.HasPositionalArgument("input.txt"));
	TEST_CHECK(r, args.HasPositionalArgument("--literal"));
	TEST_CHECK(r, args.HasPositionalArgument("-x"));
	TEST_CHECK(r, !args.HasPositionalArgument("missing"));

	const auto& positional = args.GetPositionalArguments();
	TEST_CHECK(r, positional.size() == 3);

	std::string positionalValue;
	const auto	firstError = args.GetPositionalArgument(0, positionalValue);
	TEST_CHECK(r, !firstError);
	TEST_CHECK(r, positionalValue == "input.txt");

	const auto secondError = args.GetPositionalArgument(1, positionalValue);
	TEST_CHECK(r, !secondError);
	TEST_CHECK(r, positionalValue == "--literal");

	const auto thirdError = args.GetPositionalArgument(2, positionalValue);
	TEST_CHECK(r, !thirdError);
	TEST_CHECK(r, positionalValue == "-x");

	positionalValue		  = "unchanged";
	const auto rangeError = args.GetPositionalArgument(50, positionalValue);
	TEST_CHECK(r, rangeError.GetErrorCode() == ECommandLineArgumentErrorCode::PositionalArgumentOutOfRange);
	TEST_CHECK(r, positionalValue == "unchanged");
}

void TestDuplicateArgumentUsesLastValue(TestReporter& r)
{
	char  program[] = "felis-app";
	char  first[]	= "--mode=debug";
	char  second[]	= "--mode=release";
	char* argV[]	= {program, first, second};
	int	  argC		= static_cast<int>(sizeof(argV) / sizeof(argV[0]));

	CommandLineArguments args(argC, argV);

	TEST_CHECK(r, args.GetArgument("mode") == "release");
}

void TestTypedArgumentsAndDefaults(TestReporter& r)
{
	char  program[]	 = "felis-app";
	char  count[]	 = "--count=42";
	char  scale[]	 = "--scale=1.5";
	char  ratio[]	 = "--ratio=1.23456789012345";
	char  disabled[] = "--disabled=false";
	char* argV[]	 = {program, count, scale, ratio, disabled};
	int	  argC		 = static_cast<int>(sizeof(argV) / sizeof(argV[0]));

	CommandLineArguments args(argC, argV);

	int		   countValue = 0;
	const auto countError = args.GetArgumentAs("count", countValue);
	TEST_CHECK(r, !countError);
	TEST_CHECK(r, countValue == 42);

	float	   scaleValue = 0.0f;
	const auto scaleError = args.GetArgumentAs("scale", scaleValue);
	TEST_CHECK(r, !scaleError);
	TEST_CHECK(r, std::abs(scaleValue - 1.5f) < 0.0001f);

	double	   ratioValue = 0.0;
	const auto ratioError = args.GetArgumentAs("ratio", ratioValue);
	TEST_CHECK(r, !ratioError);
	TEST_CHECK(r, std::abs(ratioValue - 1.23456789012345) < 0.000000000001);

	bool	   disabledValue = true;
	const auto disabledError = args.GetArgumentAs("disabled", disabledValue);
	TEST_CHECK(r, !disabledError);
	TEST_CHECK(r, !disabledValue);

	int		   defaultValue = 0;
	const auto defaultError = args.GetOrDefault("missing", defaultValue, 7);
	TEST_CHECK(r, !defaultError);
	TEST_CHECK(r, defaultValue == 7);
}

void TestTypedArgumentErrors(TestReporter& r)
{
	char  program[]		= "felis-app";
	char  invalid[]		= "--invalid=42abc";
	char  outOfRange[]	= "--range=999999999999999999999999";
	char  invalidBool[] = "--enabled=yes";
	char* argV[]		= {program, invalid, outOfRange, invalidBool};
	int	  argC			= static_cast<int>(sizeof(argV) / sizeof(argV[0]));

	CommandLineArguments args(argC, argV);

	int		   missingValue = 17;
	const auto missingError = args.GetArgumentAs("missing", missingValue);
	TEST_CHECK(r, missingError.GetErrorCode() == ECommandLineArgumentErrorCode::ArgumentNotFound);
	TEST_CHECK(r, missingValue == 17);

	int		   invalidValue = 23;
	const auto invalidError = args.GetArgumentAs("invalid", invalidValue);
	TEST_CHECK(r, invalidError.GetErrorCode() == ECommandLineArgumentErrorCode::InvalidValue);
	TEST_CHECK(r, invalidValue == 23);

	int		   rangeValue = 29;
	const auto rangeError = args.GetArgumentAs("range", rangeValue);
	TEST_CHECK(r, rangeError.GetErrorCode() == ECommandLineArgumentErrorCode::ValueOutOfRange);
	TEST_CHECK(r, rangeValue == 29);

	bool	   boolValue = true;
	const auto boolError = args.GetArgumentAs("enabled", boolValue);
	TEST_CHECK(r, boolError.GetErrorCode() == ECommandLineArgumentErrorCode::InvalidValue);
	TEST_CHECK(r, boolValue);

	int		   defaultValue = 31;
	const auto defaultError = args.GetOrDefault("invalid", defaultValue, 7);
	TEST_CHECK(r, defaultError.GetErrorCode() == ECommandLineArgumentErrorCode::InvalidValue);
	TEST_CHECK(r, defaultValue == 31);
}

void TestArgumentVectors(TestReporter& r)
{
	char  program[]		  = "felis-app";
	char  input[]		  = "--input=input1.txt,input\\,special.txt,input1.txt";
	char  values[]		  = "--values=1,2,3";
	char  backslashes[]	  = "--paths=folder\\\\name,other";
	char  empty[]		  = "--empty=";
	char  emptyElement[]  = "--empty-element=1,,3";
	char  invalidValues[] = "--invalid-values=1,nope,3";
	char* argV[]		  = {program, input, values, backslashes, empty, emptyElement, invalidValues};
	int	  argC			  = static_cast<int>(sizeof(argV) / sizeof(argV[0]));

	CommandLineArguments args(argC, argV);

	std::vector<std::string> inputs;
	const auto				 inputError = args.GetArgumentVectorAs("input", inputs);
	TEST_CHECK(r, !inputError);
	TEST_CHECK(r, inputs.size() == 3);
	TEST_CHECK(r, inputs[0] == "input1.txt");
	TEST_CHECK(r, inputs[1] == "input,special.txt");
	TEST_CHECK(r, inputs[2] == "input1.txt");

	std::vector<int> parsedValues;
	const auto		 valuesError = args.GetArgumentVectorAs("values", parsedValues);
	TEST_CHECK(r, !valuesError);
	TEST_CHECK(r, parsedValues.size() == 3);
	TEST_CHECK(r, parsedValues[0] == 1);
	TEST_CHECK(r, parsedValues[1] == 2);
	TEST_CHECK(r, parsedValues[2] == 3);

	std::vector<std::string> paths;
	const auto				 pathsError = args.GetArgumentVectorAs("paths", paths);
	TEST_CHECK(r, !pathsError);
	TEST_CHECK(r, paths.size() == 2);
	TEST_CHECK(r, paths[0] == "folder\\name");
	TEST_CHECK(r, paths[1] == "other");

	std::vector<int> unchangedValues{7};
	const auto		 emptyError = args.GetArgumentVectorAs("empty", unchangedValues);
	TEST_CHECK(r, emptyError.GetErrorCode() == ECommandLineArgumentErrorCode::InvalidValue);
	TEST_CHECK(r, unchangedValues.size() == 1 && unchangedValues[0] == 7);

	const auto emptyElementError = args.GetArgumentVectorAs("empty-element", unchangedValues);
	TEST_CHECK(r, emptyElementError.GetErrorCode() == ECommandLineArgumentErrorCode::InvalidValue);
	TEST_CHECK(r, unchangedValues.size() == 1 && unchangedValues[0] == 7);

	const auto invalidValuesError = args.GetArgumentVectorAs("invalid-values", unchangedValues);
	TEST_CHECK(r, invalidValuesError.GetErrorCode() == ECommandLineArgumentErrorCode::InvalidValue);
	TEST_CHECK(r, unchangedValues.size() == 1 && unchangedValues[0] == 7);

	const auto missingError = args.GetArgumentVectorAs("missing", unchangedValues);
	TEST_CHECK(r, missingError.GetErrorCode() == ECommandLineArgumentErrorCode::ArgumentNotFound);
	TEST_CHECK(r, unchangedValues.size() == 1 && unchangedValues[0] == 7);
}

void TestNamedArgumentValidation(TestReporter& r)
{
	char  program[]	   = "felis-app";
	char  mode[]	   = "--mode=lines";
	char  zeta[]	   = "--zeta";
	char  alpha[]	   = "--alpha=value";
	char  separator[]  = "--";
	char  positional[] = "--not-an-option";
	char* argV[]	   = {program, mode, zeta, alpha, separator, positional};
	int	  argC		   = static_cast<int>(sizeof(argV) / sizeof(argV[0]));

	CommandLineArguments args(argC, argV);

	NonAllowedArgumentsContainer unexpectedArguments{"stale"};
	const auto					 disabledError = args.ValidateNamedArguments(unexpectedArguments);
	TEST_CHECK(r, !disabledError);
	TEST_CHECK(r, unexpectedArguments.empty());

	args.EnableArgumentValidation(true);
	args.AddAllowedArgument("mode");
	args.AddAllowedArgument("help");

	const auto validationError = args.ValidateNamedArguments(unexpectedArguments);
	TEST_CHECK(r, validationError.GetErrorCode() == ECommandLineArgumentErrorCode::UnexpectedArgument);
	TEST_CHECK(r, std::string(validationError.GetErrorData().Text) == "Unexpected command-line argument");
	TEST_CHECK(r, unexpectedArguments.size() == 2);
	TEST_CHECK(r, unexpectedArguments.contains("alpha"));
	TEST_CHECK(r, unexpectedArguments.contains("zeta"));
	TEST_CHECK(r, args.HasPositionalArgument("--not-an-option"));

	args.AddAllowedArgument("mode");
	args.AddAllowedArgument("alpha");
	args.AddAllowedArgument("zeta");

	const auto allowedError = args.ValidateNamedArguments(unexpectedArguments);
	TEST_CHECK(r, !allowedError);
	TEST_CHECK(r, unexpectedArguments.empty());

	args.ClearAllowedArguments();

	const auto emptyAllowedError = args.ValidateNamedArguments(unexpectedArguments);
	TEST_CHECK(r, emptyAllowedError.GetErrorCode() == ECommandLineArgumentErrorCode::UnexpectedArgument);
	TEST_CHECK(r, unexpectedArguments.size() == 3);
	TEST_CHECK(r, unexpectedArguments.contains("alpha"));
	TEST_CHECK(r, unexpectedArguments.contains("zeta"));
	TEST_CHECK(r, unexpectedArguments.contains("mode"));

	args.EnableArgumentValidation(false);

	const auto reDisabledError = args.ValidateNamedArguments(unexpectedArguments);
	TEST_CHECK(r, !reDisabledError);
	TEST_CHECK(r, unexpectedArguments.empty());
}

void TestEmptyArgumentVector(TestReporter& r)
{
	CommandLineArguments args(0, nullptr);

	TEST_CHECK(r, args.GetArgC() == 0);
	TEST_CHECK(r, args.GetArgV() == nullptr);
	TEST_CHECK(r, args.GetProgramName().empty());
	TEST_CHECK(r, args.GetPositionalArguments().empty());
}

} // namespace

void TestCommandLineArguments()
{
	TestReporter r("CommandLineArgumentsTest");

	TestProgramAndRawArguments(r);
	TestFlagAndKeyValueArguments(r);
	TestPositionalArgumentsAndSeparator(r);
	TestDuplicateArgumentUsesLastValue(r);
	TestTypedArgumentsAndDefaults(r);
	TestTypedArgumentErrors(r);
	TestArgumentVectors(r);
	TestNamedArgumentValidation(r);
	TestEmptyArgumentVector(r);

	r.PrintSummary();
}

} // namespace Test
