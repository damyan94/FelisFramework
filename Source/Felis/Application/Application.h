#pragma once

#include "Felis/Application/ApplicationError.h"
#include "Felis/Application/CommandLineArguments.h"

namespace Felis
{
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

	CommandLineArguments&		GetCommandLineArguments();
	const CommandLineArguments& GetCommandLineArguments() const;

private:
	ApplicationError Init();
	ApplicationError Run();
	ApplicationError Deinit();

private:
	CommandLineArguments m_Args;
};
} // namespace Felis
