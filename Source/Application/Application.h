#pragma once

#include "Application/ApplicationError.h"
#include "Application/CommandLineArguments.h"

// Base class for an application
// Initializes, runs and deinitializes the application
class Application
{
public:
	Application(int argC, char** argV);
	virtual ~Application() = default;

	[[nodiscard]] ApplicationError RunApplication();

protected:
	virtual ApplicationError OnInit()	= 0;
	virtual ApplicationError OnRun()	= 0;
	virtual ApplicationError OnDeinit() = 0;

	const CommandLineArguments& GetCommandLineArguments() const;

private:
	ApplicationError Init();
	ApplicationError Run();
	ApplicationError Deinit();

private:
	CommandLineArguments m_Args;
};
