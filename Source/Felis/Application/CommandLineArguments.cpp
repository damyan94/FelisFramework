#include "Felis/stdafx.h"

#include "Felis/Application/CommandLineArguments.h"

namespace Felis
{
static const std::string s_ErrorString("<ERROR>");

CommandLineArguments::CommandLineArguments(int argC, char** argV)
	: m_ArgC(argC),
	  m_ArgV(argV),
	  m_ProgramName(argC > 0 && argV && argV[0] ? argV[0] : "")
{
	ParseArguments();
}

int CommandLineArguments::GetArgC() const
{
	return m_ArgC;
}

char** CommandLineArguments::GetArgV() const
{
	return m_ArgV;
}

const std::string& CommandLineArguments::GetProgramName() const
{
	return m_ProgramName;
}

bool CommandLineArguments::HasPositionalArgument(const std::string& arg) const
{
	return std::find(m_PositionalArgs.begin(), m_PositionalArgs.end(), arg) != m_PositionalArgs.end();
}

CommandLineArgumentError CommandLineArguments::GetPositionalArgument(size_t index, std::string& outValue) const
{
	ReturnIf(index >= m_PositionalArgs.size(), ECommandLineArgumentErrorCode::PositionalArgumentOutOfRange);

	outValue = m_PositionalArgs[index];
	return ECommandLineArgumentErrorCode::Success;
}

const PositionalArgumentsContainer& CommandLineArguments::GetPositionalArguments() const
{
	return m_PositionalArgs;
}

bool CommandLineArguments::HasArgument(const std::string& arg) const
{
	return m_ParsedArgs.contains(arg);
}

const std::string& CommandLineArguments::GetArgument(const std::string& arg) const
{
	AssertReturnIf(!HasArgument(arg), s_ErrorString);

	return m_ParsedArgs.at(arg);
}

bool CommandLineArguments::ParseArgumentVector(const std::string& value, std::vector<std::string>& outValues) const
{
	ReturnIf(value.empty(), false);

	std::string parsedValue;
	parsedValue.reserve(value.size());

	for (size_t i = 0; i < value.size(); i++)
	{
		const char character = value[i];

		if (character == '\\' && i + 1 < value.size() && (value[i + 1] == ',' || value[i + 1] == '\\'))
		{
			parsedValue += value[++i];
			continue;
		}

		if (character == ',')
		{
			ReturnIf(parsedValue.empty(), false);

			outValues.emplace_back(std::move(parsedValue));
			parsedValue.clear();
			continue;
		}

		parsedValue += character;
	}

	ReturnIf(parsedValue.empty(), false);

	outValues.emplace_back(std::move(parsedValue));
	return true;
}

void CommandLineArguments::ParseArguments()
{
	ReturnIf(!m_ArgV);

	bool parseOptions = true;
	for (int i = 1; i < m_ArgC; i++)
	{
		const char* rawArgument = m_ArgV[i];
		ContinueIf(!rawArgument);

		std::string_view argument(rawArgument);

		if (parseOptions && argument == "--")
		{
			parseOptions = false;
			continue;
		}

		if (!parseOptions || !argument.starts_with("--") || argument.size() <= 2)
		{
			m_PositionalArgs.emplace_back(argument);
			continue;
		}

		argument.remove_prefix(2);

		const auto separator = argument.find('=');
		const auto key		 = argument.substr(0, separator);

		if (key.empty())
		{
			m_PositionalArgs.emplace_back(rawArgument);
			continue;
		}

		const auto value = separator == std::string_view::npos ? std::string_view("1") : argument.substr(separator + 1);

		m_ParsedArgs.insert_or_assign(std::string(key), std::string(value));
	}
}

void CommandLineArguments::EnableArgumentValidation(bool enabled)
{
	m_IsArgumentValidationEnabled = enabled;
}

void CommandLineArguments::AddAllowedArgument(const std::string& arg)
{
	m_AllowedArgs.insert(arg);
}

void CommandLineArguments::RemoveAllowedArgument(const std::string& arg)
{
	ReturnIf(!m_AllowedArgs.contains(arg));

	m_AllowedArgs.erase(arg);
}

void CommandLineArguments::ClearAllowedArguments()
{
	m_AllowedArgs.clear();
}

CommandLineArgumentError CommandLineArguments::ValidateNamedArguments(
	NonAllowedArgumentsContainer& outUnexpectedArguments) const
{
	outUnexpectedArguments.clear();

	ReturnIf(!m_IsArgumentValidationEnabled, ECommandLineArgumentErrorCode::Success);

	for (const auto& [arg, _] : m_ParsedArgs)
	{
		if (!m_AllowedArgs.contains(arg))
		{
			outUnexpectedArguments.insert(arg);
		}
	}

	ReturnIf(!outUnexpectedArguments.empty(), ECommandLineArgumentErrorCode::UnexpectedArgument);

	return ECommandLineArgumentErrorCode::Success;
}
} // namespace Felis
