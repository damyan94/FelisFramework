#pragma once

#include "Time/Timer.h"
#include "Time/TimerData.h"

class TimerManager
{
public:
	TimerManager()	= default;
	~TimerManager() = default;

	DISABLE_COPY(TimerManager);
	DISABLE_MOVE(TimerManager);

	static TimerManager& Instance();

	TimerId Start(ETimerType timerType, Duration interval, TimerCallback callback);

	void Stop(TimerId id);
	void Pause(TimerId id);
	void Resume(TimerId id);
	void Restart(TimerId id);

	bool IsRunning(TimerId id);
	bool IsPaused(TimerId id);
	bool IsValid(TimerId id);

	void Update(Duration dt);

private:
	TimerData* GetTimerData(TimerId id);

private:
	std::vector<TimerData>	   m_Timers;
	std::vector<size_t>		   m_FreeIndices;
	std::vector<TimerCallback> m_CallbacksToExecute;
};
