#include "stdafx.h"

#include "Application/IApplication.h"

#include "Utils/Logger/Formatters/LogFormatterConsole.h"
#include "Utils/Logger/Writers/LogWriterConsole.h"

IApplication::IApplication(int argC, char** argV)
	: m_Args(argC, argV),
	  m_ExitRequested(false)
{
}

void IApplication::OnDeinit()
{
}

bool IApplication::RunApplication()
{
	ReturnIf(!Init(), false);

	while (!m_ExitRequested)
	{
		const auto dt = m_Time.GetElapsed();
		m_Time.Reset();

		Update(dt);

		const auto frameTargetTime = Duration(Duration::Nanoseconds(1000000000) / m_Config.m_TargetFPS);
		const auto sleepDuration   = frameTargetTime - dt;

		LogDebug("Sleep time this frame: ",
				 sleepDuration.As<Duration::Microseconds>(),
				 "; Elapsed time this frame: ",
				 (frameTargetTime - sleepDuration).As<Duration::Microseconds>(),
				 ".");

		std::this_thread::sleep_for(sleepDuration.As<Duration::Nanoseconds>());
	}

	OnDeinit();

	return true;
}

const CommandLineArguments& IApplication::GetCommandLineArguments() const
{
	return m_Args;
}

const ApplicationConfig& IApplication::GetApplicationConfig() const
{
	return m_Config;
}

bool IApplication::Init()
{
	// TODO add framework initialization code
	AssertReturnIf(!InitFromCommandLineArguments(), false);
	AssertReturnIf(!InitLogging(), false);

	// Call user provided override of the OnInit method
	AssertReturnIf(!OnInit(), false);

	return true;
}

void IApplication::Update(Duration dt)
{
	// TODO add framework update code - timers, rendering (maybe), etc.

	// Call user provided override of the OnUpdate method
	OnUpdate(dt);
}

bool IApplication::InitFromCommandLineArguments()
{
	m_Config.m_TargetFPS = m_Args.GetOrDefault<int>("fps", Constants::Application::DefaultFPS);

	return true;
}

bool IApplication::InitLogging() const
{
	g_Logger.SetLogPrefix("");
	g_Logger.SetLogLevel(ELogLevel::Debug);
	g_Logger.SetLogFlags(LogFlags::All);
	g_Logger.AddLogDestination(
		{ELogDestinationType::Console, std::make_unique<LogWriterConsole>(), std::make_unique<LogFormatterConsole>()});

	LogInfo("Logging initialized successfully.");

	return true;
}
