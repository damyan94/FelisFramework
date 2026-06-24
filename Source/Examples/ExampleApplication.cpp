#include "stdafx.h"

#include "Examples/ExampleApplication.h"

ExampleApplication::ExampleApplication(int argC, char** argV)
	: IApplication(argC, argV)
{
}

int ExampleApplication::OnInit()
{
	const auto& cfg = GetApplicationConfig();

	LogDebug("Application initialized with FPS [", cfg.m_TargetFPS, "]");

	return 0;
}

int ExampleApplication::OnUpdate(Time::Duration /*dt*/)
{
	Test::TestLogger();

	return 0;
}
