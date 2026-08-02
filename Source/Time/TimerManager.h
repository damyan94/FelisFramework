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

	bool IsRunning(TimerId id) const;
	bool IsPaused(TimerId id) const;
	bool IsValid(TimerId id) const;

	ETimerType	GetTimerType(TimerId id) const;
	ETimerState GetTimerState(TimerId id) const;
	Duration	GetInterval(TimerId id) const;
	Duration	GetRemaining(TimerId id) const;
	Duration	GetElapsed(TimerId id) const;
	float		GetProgress(TimerId id) const;

	void Update();
	void Update(Duration dt);

private:
	TimerData*		 GetTimerData(TimerId id);
	const TimerData* GetTimerData(TimerId id) const;

	void ExecuteCallbacks();
	void OnTimerTick(TimerData& timerData);

private:
	Time					   m_Clock;
	std::vector<TimerData>	   m_Timers;
	std::vector<size_t>		   m_FreeIndices;
	std::vector<TimerCallback> m_CallbacksToExecute;
};
