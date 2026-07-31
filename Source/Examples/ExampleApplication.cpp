#include "stdafx.h"

#include "Examples/ExampleApplication.h"

#include "Tests/Utils/Logger/Logger.h"

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
	//Test::TestLogger();
}

void ExampleApplication::OnDeinit()
{
	LogDebug(">>> OnDeinit called");
}
