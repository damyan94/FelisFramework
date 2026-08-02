#include "stdafx.h"

#include "Logger/LoggerTest.h"
#include "Time/TimerTest.h"

int main(int argC, char** argV)
{
	Test::TestLogger();
	Test::TestTimers();

	return 0;
}
