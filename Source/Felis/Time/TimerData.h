#pragma once

#include "Felis/Time/Timer.h"

namespace Felis
{
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
} // namespace Felis
