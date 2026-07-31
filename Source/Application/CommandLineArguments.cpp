#include "stdafx.h"

#include "Application/CommandLineArguments.h"

static const std::string s_ErrorString("<ERROR>");

CommandLineArguments::CommandLineArguments(int argC, char** argV)
	: m_ArgC(argC),
	  m_ArgV(argV),
	  m_ProgramName(argV[0])
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

bool CommandLineArguments::HasArgument(const std::string& arg) const
{
	return m_ParsedArgs.contains(arg);
}

const std::string& CommandLineArguments::GetArgument(const std::string& arg) const
{
	AssertReturnIf(!HasArgument(arg), s_ErrorString);

	return m_ParsedArgs.at(arg);
}

int CommandLineArguments::ParseArguments()
{
	for (int i = 1; i < m_ArgC; i++)
	{
		// TODO implement
	}

	return 0;
}
