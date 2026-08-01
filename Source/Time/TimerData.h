#pragma once

#include "Time/Timer.h"

struct TimerData
{
	TimerId		  Id;
	ETimerType	  TimerType	 = ETimerType::Oneshot;
	ETimerState	  TimerState = ETimerState::Free;
	Duration	  Interval;
	Duration	  Elapsed;
	TimerCallback Callback;
};
