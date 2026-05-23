#include "stdafx.h"

#include "Utils/Logger/Formatters/LogFormatterConsole.h"
#include "Utils/Logger/Writers/LogWriterConsole.h"

void InitLogging()
{
	g_Logger.SetLogPrefix("");
	g_Logger.SetLogLevel(ELogLevel::Debug);
	g_Logger.SetLogFlags(LogFlags::All);
	g_Logger.AddLogDestination(
		{ELogDestinationType::Console, std::make_unique<LogWriterConsole>(), std::make_unique<LogFormatterConsole>()});

	LogInfo("Logging initialized successfully.");
}

int main(int /*argC*/, char** /*argV*/)
{
	InitLogging();

	Test::TestLogger();

	return 0;
}
