#include "stdafx.h"

#include "Examples/ExampleApplication.h"

ExampleApplication::ExampleApplication(int argC, char** argV)
	: IApplication(argC, argV)
{
}

bool ExampleApplication::OnInit()
{
	const auto& cfg = GetApplicationConfig();

	LogDebug("Application initialized with FPS [", cfg.m_TargetFPS, "]");

	return true;
}

void ExampleApplication::OnUpdate(Duration /*dt*/)
{
	Test::TestLogger();
}
