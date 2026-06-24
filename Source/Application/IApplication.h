#pragma once

#include "Application/CommandLineArguments.h"

struct IApplicationConfig
{
	int m_TargetFPS = Constants::Application::DefaultFPS;
};

// An interface for the application main class
// Contains the abstract methods OnInit and OnUpdate which runs the main loop
class IApplication
{
public:
	IApplication(int argC, char** argV);
	virtual ~IApplication() = default;

	virtual int OnInit()						= 0;
	virtual int OnUpdate(Time::Duration /*dt*/) = 0;

	int RunApplication();

	const CommandLineArguments& GetCommandLineArguments() const;
	const IApplicationConfig&	GetApplicationConfig() const;

private:
	int Init();
	int Update(Time::Duration dt);

	int InitFromCommandLineArguments();

	int InitLogging() const;

private:
	CommandLineArguments m_Args;
	IApplicationConfig	 m_Config;

	bool m_ExitRequested;
	Time m_Time;
};
