#include "Felis/stdafx.h"

#include "Felis/Time/TimerManager.h"

namespace Felis
{

TimerManager& TimerManager::Instance()
{
	static TimerManager timerManager;

	return timerManager;
}

TimerId TimerManager::Start(ETimerType timerType, Duration interval, TimerCallback callback)
{
	if (interval < Timer::MinDuration || interval > Timer::MaxDuration)
	{
		LogWarning("Timer interval ",
				   interval,
				   " is not in the range of [",
				   Timer::MinDuration,
				   " - ",
				   Timer::MaxDuration,
				   "]; clamping it.");
		interval = std::clamp(interval, Timer::MinDuration, Timer::MaxDuration);
	}

	TimerData* timerData = nullptr;
	size_t	   index;

	if (m_FreeIndices.empty())
	{
		index = m_Timers.size();
		m_Timers.emplace_back();

		timerData				 = &m_Timers.back();
		timerData->Id.Generation = 1;
	}
	else
	{
		index = m_FreeIndices.back();
		m_FreeIndices.pop_back();

		timerData = &m_Timers[index];
		++timerData->Id.Generation;

		if (timerData->Id.Generation == 0)
		{
			++timerData->Id.Generation;
		}
	}

	timerData->Id.Index	  = index;
	timerData->TimerType  = timerType;
	timerData->TimerState = ETimerState::Running;
	timerData->Interval	  = interval;
	timerData->Elapsed	  = Duration::Zero;
	timerData->Callback	  = std::move(callback);

	return timerData->Id;
}

void TimerManager::Stop(TimerId id)
{
	TimerData* timerData = GetTimerData(id);
	ReturnIf(!timerData);

	timerData->TimerType  = ETimerType::Oneshot;
	timerData->TimerState = ETimerState::Free;
	timerData->Interval	  = Duration::Zero;
	timerData->Elapsed	  = Duration::Zero;
	timerData->Callback	  = nullptr;

	m_FreeIndices.push_back(id.Index);
}

void TimerManager::Pause(TimerId id)
{
	TimerData* timerData = GetTimerData(id);
	ReturnIf(!timerData);

	ReturnIf(timerData->TimerState == ETimerState::Paused);

	timerData->TimerState = ETimerState::Paused;
}

void TimerManager::Resume(TimerId id)
{
	TimerData* timerData = GetTimerData(id);
	ReturnIf(!timerData);

	ReturnIf(timerData->TimerState != ETimerState::Paused);

	timerData->TimerState = ETimerState::Running;
}

void TimerManager::Restart(TimerId id)
{
	auto* timerData = GetTimerData(id);
	ReturnIf(!timerData);

	timerData->TimerState = ETimerState::Running;
	timerData->Elapsed	  = Duration::Zero;
}

bool TimerManager::IsRunning(TimerId id) const
{
	const TimerData* timerData = GetTimerData(id);
	ReturnIf(!timerData, false);

	return timerData->TimerState == ETimerState::Running;
}

bool TimerManager::IsPaused(TimerId id) const
{
	const TimerData* timerData = GetTimerData(id);
	ReturnIf(!timerData, false);

	return timerData->TimerState == ETimerState::Paused;
}

bool TimerManager::IsValid(TimerId id) const
{
	return GetTimerData(id) != nullptr;
}

ETimerType TimerManager::GetTimerType(TimerId id) const
{
	const TimerData* timerData = GetTimerData(id);
	AssertReturnIf(!timerData, ETimerType::Oneshot);

	return timerData->TimerType;
}

ETimerState TimerManager::GetTimerState(TimerId id) const
{
	const TimerData* timerData = GetTimerData(id);
	ReturnIf(!timerData, ETimerState::Free);

	return timerData->TimerState;
}

Duration TimerManager::GetInterval(TimerId id) const
{
	const TimerData* timerData = GetTimerData(id);
	ReturnIf(!timerData, Duration::Zero);

	return timerData->Interval;
}

Duration TimerManager::GetRemaining(TimerId id) const
{
	const TimerData* timerData = GetTimerData(id);
	ReturnIf(!timerData, Duration::Zero);

	return timerData->Interval - timerData->Elapsed;
}

Duration TimerManager::GetElapsed(TimerId id) const
{
	const TimerData* timerData = GetTimerData(id);
	ReturnIf(!timerData, Duration::Zero);

	return timerData->Elapsed;
}

float TimerManager::GetProgress(TimerId id) const
{
	const TimerData* timerData = GetTimerData(id);
	ReturnIf(!timerData, 0.0f);

	const float progress =
		(float)timerData->Elapsed.As(EUnitOfTime::Nanosecond) / (float)timerData->Interval.As(EUnitOfTime::Nanosecond);

	return std::clamp(progress, 0.0f, 1.0f);
}

void TimerManager::Update()
{
	const auto dt = m_Clock.GetElapsed();
	m_Clock.Reset();
	Update(dt);
}

void TimerManager::Update(Duration dt)
{
	for (auto& timerData : m_Timers)
	{
		ContinueIf(timerData.TimerState != ETimerState::Running);

		timerData.Elapsed += dt;

		ContinueIf(timerData.Elapsed < timerData.Interval);

		OnTimerTick(timerData);
	}

	ExecuteCallbacks();
}

TimerData* TimerManager::GetTimerData(TimerId id)
{
	ReturnIf(id.Index >= m_Timers.size(), nullptr);

	auto& timerData = m_Timers[id.Index];

	ReturnIf(timerData.Id.Generation != id.Generation, nullptr);
	ReturnIf(timerData.TimerState == ETimerState::Free, nullptr);

	return &timerData;
}

const TimerData* TimerManager::GetTimerData(TimerId id) const
{
	ReturnIf(id.Index >= m_Timers.size(), nullptr);

	const auto& timerData = m_Timers[id.Index];

	ReturnIf(timerData.Id.Generation != id.Generation, nullptr);
	ReturnIf(timerData.TimerState == ETimerState::Free, nullptr);

	return &timerData;
}

void TimerManager::ExecuteCallbacks()
{
	for (auto& callback : m_CallbacksToExecute)
	{
		callback();
	}

	m_CallbacksToExecute.clear();
}

void TimerManager::OnTimerTick(TimerData& timerData)
{
	if (timerData.Callback)
	{
		m_CallbacksToExecute.emplace_back(timerData.Callback);
	}

	if (timerData.TimerType == ETimerType::Oneshot)
	{
		Stop(timerData.Id);
		return;
	}

	timerData.Elapsed = timerData.Elapsed.As<Duration::Nanoseconds>() % timerData.Interval.As<Duration::Nanoseconds>();
}

} // namespace Felis
