#include "stdafx.h"

#include "Application/IApplication.h"

IApplication::IApplication(int argC, char** argV)
	: m_Args(argC, argV)
{
}

bool IApplication::RunApplication()
{
	ReturnIf(!Init(), false);

	Run();
	Deinit();

	return true;
}

const CommandLineArguments& IApplication::GetCommandLineArguments() const
{
	return m_Args;
}

bool IApplication::Init()
{
	AssertReturnIf(!InitFromCommandLineArguments(), false);

	// Call user provided override of the OnInit method
	AssertReturnIf(!OnInit(), false);

	return true;
}

void IApplication::Run()
{
	// Call user provided override of the OnRun method
	OnRun();
}

void IApplication::Deinit()
{
	// Call user provided override of the OnDeinit method
	OnDeinit();
}

bool IApplication::InitFromCommandLineArguments()
{
	return true;
}
