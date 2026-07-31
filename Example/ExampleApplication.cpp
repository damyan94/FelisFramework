#include "stdafx.h"

#include "ExampleApplication.h"

ExampleApplication::ExampleApplication(int argC, char** argV)
	: IApplication(argC, argV)
{
}

bool ExampleApplication::OnInit()
{
	LogDebug(">>> Running ", m_Args.GetProgramName(), "; ", m_Args.GetArgC());
	LogDebug(">>> OnInit called");

	return true;
}

void ExampleApplication::OnRun()
{
	LogDebug(">>> OnRun called");
}

void ExampleApplication::OnDeinit()
{
	LogDebug(">>> OnDeinit called");
}
