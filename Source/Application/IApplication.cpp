#include "stdafx.h"

#include "Application/IApplication.h"

#include "Utils/Logger/Formatters/LogFormatterConsole.h"
#include "Utils/Logger/Writers/LogWriterConsole.h"

IApplication::IApplication(int argC, char** argV)
	: m_Args(argC, argV),
	  m_ExitRequested(false)
{
}

int IApplication::RunApplication()
{
	int ec = 0;

	ec |= Init();

	while (!m_ExitRequested)
	{
		const auto dt = m_Time.Elapsed();
		m_Time.Reset();

		ec |= Update(dt);

		const uint64_t frameTargetTime = 1000000000 / m_Config.m_TargetFPS;
		const uint64_t sleepDuration   = frameTargetTime - m_Time.ElapsedAs(EUnitOfTime::Nanosecond);

		LogDebug("Sleep time this frame: ",
				 sleepDuration / 1000,
				 " us; Elapsed time this frame: ",
				 (frameTargetTime - sleepDuration) / 1000,
				 " us.");

		std::this_thread::sleep_for(Time::Duration(sleepDuration));
	}

	return ec;
}

const CommandLineArguments& IApplication::GetCommandLineArguments() const
{
	return m_Args;
}

const IApplicationConfig& IApplication::GetApplicationConfig() const
{
	return m_Config;
}

int IApplication::Init()
{
	int ec = 0;

	// TODO add framework initialization code
	ec |= InitFromCommandLineArguments();
	ec |= InitLogging();

	// Call user provided override of the OnInit method
	ec |= OnInit();

	return ec;
}

int IApplication::Update(Time::Duration dt)
{
	int ec = 0;

	// TODO add framework update code - timers, rendering (maybe), etc.

	// Call user provided override of the OnUpdate method
	ec |= OnUpdate(dt);

	return ec;
}

int IApplication::InitFromCommandLineArguments()
{
	m_Config.m_TargetFPS = m_Args.GetOrDefault<int>("fps", Constants::Application::DefaultFPS);

	return 0;
}

int IApplication::InitLogging() const
{
	g_Logger.SetLogPrefix("");
	g_Logger.SetLogLevel(ELogLevel::Debug);
	g_Logger.SetLogFlags(LogFlags::All);
	g_Logger.AddLogDestination(
		{ELogDestinationType::Console, std::make_unique<LogWriterConsole>(), std::make_unique<LogFormatterConsole>()});

	LogInfo("Logging initialized successfully.");

	return 0;
}
