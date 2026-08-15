#pragma once

#include "Felis/Time/Timer.h"

// Contains information about a given Timer, identified by its TimerId
struct TimerData
{
	TimerId		  Id;
	ETimerType	  TimerType	 = ETimerType::Oneshot;
	ETimerState	  TimerState = ETimerState::Free;
	Duration	  Interval;
	Duration	  Elapsed;
	TimerCallback Callback;
};
