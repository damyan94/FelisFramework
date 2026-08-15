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

ApplicationError TestApplication::OnInit()
{
	return EApplicationErrorCode::Success;
}

ApplicationError TestApplication::OnRun()
{
	Test::TestApplicationLifecycle();
	Test::TestCommandLineArguments();
	Test::TestError();
	Test::TestLogger();

	Test::TestDuration();
	Test::TestTime();
	Test::TestTimers();

	return EApplicationErrorCode::Success;
}

ApplicationError TestApplication::OnDeinit()
{
	return EApplicationErrorCode::Success;
}
