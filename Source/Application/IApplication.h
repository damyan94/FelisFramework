#pragma once

#include "Application/CommandLineArguments.h"

struct ApplicationConfig
{
	int m_TargetFPS = Constants::Application::DefaultFPS;
};

// An interface for the application main class
// Initializes the application and runs the main loop
// Contains the OnInit and OnUpdate methods which can be overriden
class IApplication
{
public:
	IApplication(int argC, char** argV);
	virtual ~IApplication() = default;

	virtual bool OnInit()				   = 0;
	virtual void OnUpdate(Duration /*dt*/) = 0;
	virtual void OnDeinit();

	bool RunApplication();

	const CommandLineArguments& GetCommandLineArguments() const;
	const ApplicationConfig&	GetApplicationConfig() const;

private:
	bool Init();
	void Update(Duration dt);

	bool InitFromCommandLineArguments();

	bool InitLogging() const;

private:
	CommandLineArguments m_Args;
	ApplicationConfig	 m_Config;

	bool m_ExitRequested;
	Time m_Time;
};
