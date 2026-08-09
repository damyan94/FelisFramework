#include "stdafx.h"

#include "Error/ErrorTest.h"
#include "Logger/LoggerTest.h"
#include "Time/TimerTest.h"

int main(int argC, char** argV)
{
	Test::TestError();
	Test::TestLogger();

	Test::TestDuration();
	Test::TestTime();
	Test::TestTimers();

	return 0;
}
