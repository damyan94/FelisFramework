#include "stdafx.h"

#include "ExampleApplication.h"

ExampleApplication::ExampleApplication(int argC, char** argV)
	: Felis::Application(argC, argV)
{
}

Felis::ApplicationError ExampleApplication::OnInit()
{
	auto& args = GetCommandLineArguments();

	Felis::Logger::GetGlobalLogger().SetLogPrefix("FelisExample");

	args.EnableArgumentValidation(true);
	args.AddAllowedArgument("help");
	args.AddAllowedArgument("mode");
	args.AddAllowedArgument("verbose");

	m_ShowHelp = args.HasArgument("help");
	ReturnIf(m_ShowHelp, Felis::EApplicationErrorCode::Success);

	Felis::NonAllowedArgumentsContainer unexpectedArguments;
	const auto							validationError = args.ValidateNamedArguments(unexpectedArguments);
	if (validationError)
	{
		for (const auto& argument : unexpectedArguments)
		{
			LogError("Unexpected argument: --", argument);
		}

		PrintUsage();
		return Felis::EApplicationErrorCode::InvalidCommandLineArguments;
	}

	if (args.GetPositionalArguments().size() != 1)
	{
		LogError("Expected exactly one input file. Use --help for usage.");
		return Felis::EApplicationErrorCode::InvalidCommandLineArguments;
	}

	m_InputPath = args.GetPositionalArguments().front();

	std::string mode;
	auto		error = args.GetOrDefault("mode", mode, std::string("lines"));
	if (error)
	{
		LogError(error);
		return Felis::EApplicationErrorCode::InvalidCommandLineArguments;
	}

	if (mode == "lines")
	{
		m_CountMode = ECountMode::Lines;
	}
	else if (mode == "words")
	{
		m_CountMode = ECountMode::Words;
	}
	else if (mode == "bytes")
	{
		m_CountMode = ECountMode::Bytes;
	}
	else
	{
		LogError("Invalid mode '", mode, "'. Expected lines, words or bytes.");
		return Felis::EApplicationErrorCode::InvalidCommandLineArguments;
	}

	error = args.GetOrDefault("verbose", m_Verbose, false);
	if (error)
	{
		LogError(error);
		return Felis::EApplicationErrorCode::InvalidCommandLineArguments;
	}

	if (m_Verbose)
	{
		Felis::Logger::GetGlobalLogger().SetLogLevel(Felis::ELogLevel::Info);
	}
	else
	{
		Felis::Logger::GetGlobalLogger().SetLogLevel(Felis::ELogLevel::Error);
	}

	return Felis::EApplicationErrorCode::Success;
}

Felis::ApplicationError ExampleApplication::OnRun()
{
	if (m_ShowHelp)
	{
		PrintUsage();
		return Felis::EApplicationErrorCode::Success;
	}

	std::ifstream input(m_InputPath, std::ios::binary);
	if (!input.is_open())
	{
		LogError("Failed to open input file: ", m_InputPath);
		return Felis::EApplicationErrorCode::RuntimeFailed;
	}

	uint64_t lineCount			= 0;
	uint64_t wordCount			= 0;
	uint64_t byteCount			= 0;
	bool	 previousWhitespace = true;
	bool	 endsWithNewline	= false;
	char	 character			= '\0';

	while (input.get(character))
	{
		++byteCount;

		endsWithNewline = character == '\n';
		if (endsWithNewline)
		{
			++lineCount;
		}

		const bool whitespace = std::isspace(static_cast<unsigned char>(character)) != 0;
		if (!whitespace && previousWhitespace)
		{
			++wordCount;
		}

		previousWhitespace = whitespace;
	}

	if (input.bad())
	{
		LogError("Failed while reading input file: ", m_InputPath);
		return Felis::EApplicationErrorCode::RuntimeFailed;
	}

	if (byteCount > 0 && !endsWithNewline)
	{
		++lineCount;
	}

	uint64_t	result = lineCount;
	const char* label  = "lines";

	switch (m_CountMode)
	{
	case ECountMode::Lines:
		result = lineCount;
		label  = "lines";
		break;

	case ECountMode::Words:
		result = wordCount;
		label  = "words";
		break;

	case ECountMode::Bytes:
		result = byteCount;
		label  = "bytes";
		break;
	}

	LogInfo("Processed ", byteCount, " bytes from ", m_InputPath);
	std::cout << result << ' ' << label << ": " << m_InputPath << '\n';

	return Felis::EApplicationErrorCode::Success;
}

void ExampleApplication::PrintUsage() const
{
	const auto& programName = GetCommandLineArguments().GetProgramName();

	std::cout << "Usage: " << (programName.empty() ? "FelisExample" : programName)
			  << " [--mode=lines|words|bytes] [--verbose] <file>\n"
			  << "\n"
			  << "Count lines, words or bytes in a text file.\n"
			  << "\n"
			  << "Options:\n"
			  << "  --mode=MODE  Select lines, words or bytes. Default: lines.\n"
			  << "  --verbose    Enable informational logging.\n"
			  << "  --help       Show this help text.\n"
			  << "  --           Stop parsing named arguments.\n";
}

Felis::ApplicationError ExampleApplication::OnDeinit()
{
	Felis::Logger::GetGlobalLogger().Flush();

	return Felis::EApplicationErrorCode::Success;
}
