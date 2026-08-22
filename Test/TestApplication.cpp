#include "stdafx.h"

#include "TestApplication.h"

#include "Application/ApplicationTest.h"
#include "Application/CommandLineArgumentsTest.h"
#include "Error/ErrorTest.h"
#include "File/FileTest.h"
#include "Logger/LogWriterFileTest.h"
#include "Logger/LoggerTest.h"
#include "Time/DurationTest.h"
#include "Time/TimeTest.h"
#include "Time/TimerTest.h"

TestApplication::TestApplication(int argC, char** argV)
	: Felis::Application(argC, argV)
{
}

int TestApplication::GetFailures() const
{
	return m_Failures;
}

Felis::ApplicationError TestApplication::OnInit()
{
	return Felis::EApplicationErrorCode::Success;
}

Felis::ApplicationError TestApplication::OnRun()
{
	m_Failures = 0;
	m_Failures += Test::TestApplicationLifecycle();
	m_Failures += Test::TestCommandLineArguments();
	m_Failures += Test::TestError();
	m_Failures += Test::TestFile();
	m_Failures += Test::TestLogWriterFile();
	m_Failures += Test::TestLogger();
	m_Failures += Test::TestDuration();
	m_Failures += Test::TestTime();
	m_Failures += Test::TestTimers();

	return Felis::EApplicationErrorCode::Success;
}

Felis::ApplicationError TestApplication::OnDeinit()
{
	return Felis::EApplicationErrorCode::Success;
}
