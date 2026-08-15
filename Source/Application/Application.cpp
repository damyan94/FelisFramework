#include "stdafx.h"

#include "Application/Application.h"

Application::Application(int argC, char** argV)
	: m_Args(argC, argV)
{
}

ApplicationError Application::RunApplication()
{
	const auto initError = Init();
	ReturnIf(initError, initError);

	const auto runError	   = Run();
	const auto deinitError = Deinit();

	ReturnIf(runError, runError);

	return deinitError;
}

const CommandLineArguments& Application::GetCommandLineArguments() const
{
	return m_Args;
}

ApplicationError Application::Init()
{
	return OnInit();
}

ApplicationError Application::Run()
{
	return OnRun();
}

ApplicationError Application::Deinit()
{
	return OnDeinit();
}
