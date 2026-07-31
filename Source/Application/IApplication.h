#pragma once

#include "Application/CommandLineArguments.h"

// An interface for the application main class
// Initializes, runs and deinitializes the application
// Contains the OnInit, OnRun and OnDeinit methods which can be overriden
class IApplication
{
public:
	IApplication(int argC, char** argV);
	virtual ~IApplication() = default;

	bool RunApplication();

protected:
	virtual bool OnInit()	= 0;
	virtual void OnRun()	= 0;
	virtual void OnDeinit() = 0;

	const CommandLineArguments& GetCommandLineArguments() const;

private:
	bool Init();
	void Run();
	void Deinit();

	bool InitFromCommandLineArguments();

protected:
	CommandLineArguments m_Args;
};
