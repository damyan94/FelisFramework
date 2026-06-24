#include "stdafx.h"

#include "Application/CommandLineArguments.h"

CommandLineArguments::CommandLineArguments(int argC, char** ArgV)
	: m_ArgC(argC),
	  m_ArgV(ArgV)
{
	ParseArguments();
}

bool CommandLineArguments::HasArgument(std::string_view arg) const
{
	return m_ParsedArgs.contains(arg);
}

const std::string_view& CommandLineArguments::GetArgument(std::string_view arg) const
{
	static const std::string_view err("ERROR");

	AssertReturnIf(!HasArgument(arg), err);

	return m_ParsedArgs.at(arg);
}

int CommandLineArguments::ParseArguments()
{
	for (int i = 0; i < m_ArgC; i++)
	{
		// TODO implement
	}

	return 0;
}
