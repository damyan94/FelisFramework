#include "stdafx.h"

#include "TestApplication.h"

#include "Application/ApplicationTest.h"
#include "Application/CommandLineArgumentsTest.h"
#include "Error/ErrorTest.h"
#include "Logger/LoggerTest.h"
#include "Time/DurationTest.h"
#include "Time/TimeTest.h"
#include "Time/TimerTest.h"

TestApplication::TestApplication(int argC, char** argV)
	: Application(argC, argV)
{
}

int TestApplication::GetFailures() const
{
	return m_Failures;
}

ApplicationError TestApplication::OnInit()
{
	return EApplicationErrorCode::Success;
}

ApplicationError TestApplication::OnRun()
{
	m_Failures = 0;
	m_Failures += Test::TestApplicationLifecycle();
	m_Failures += Test::TestCommandLineArguments();
	m_Failures += Test::TestError();
	m_Failures += Test::TestLogger();
	m_Failures += Test::TestDuration();
	m_Failures += Test::TestTime();
	m_Failures += Test::TestTimers();

	return EApplicationErrorCode::Success;
}

ApplicationError TestApplication::OnDeinit()
{
	return EApplicationErrorCode::Success;
}
