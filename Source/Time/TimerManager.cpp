#include "stdafx.h"

#include "Time/TimerManager.h"

TimerManager& TimerManager::Instance()
{
	static TimerManager timerManager;

	return timerManager;
}

TimerId TimerManager::Start(ETimerType timerType, Duration interval, TimerCallback callback)
{
	TimerData* timerData = nullptr;
	size_t	   index;

	if (!m_FreeIndices.empty())
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
	else
	{
		index = m_Timers.size();
		m_Timers.emplace_back();

		timerData				 = &m_Timers.back();
		timerData->Id.Generation = 1;
	}

	timerData->Id.Index	  = index;
	timerData->TimerType  = timerType;
	timerData->TimerState = ETimerState::Running;
	timerData->Interval	  = interval;
	timerData->Elapsed	  = Duration::Nanoseconds(0);
	timerData->Callback	  = std::move(callback);

	return timerData->Id;
}

void TimerManager::Stop(TimerId id)
{
	TimerData* timerData = GetTimerData(id);
	ReturnIf(!timerData);

	timerData->TimerType  = ETimerType::Oneshot;
	timerData->TimerState = ETimerState::Free;
	timerData->Interval	  = Duration::Nanoseconds(0);
	timerData->Elapsed	  = Duration::Nanoseconds(0);
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
	TimerData* timerData = GetTimerData(id);
	ReturnIf(!timerData);

	timerData->TimerState = ETimerState::Running;
	timerData->Elapsed	  = Duration::Nanoseconds(0);
}

bool TimerManager::IsRunning(TimerId id)
{
	TimerData* timerData = GetTimerData(id);
	ReturnIf(!timerData, false);

	return timerData->TimerState == ETimerState::Running;
}

bool TimerManager::IsPaused(TimerId id)
{
	TimerData* timerData = GetTimerData(id);
	ReturnIf(!timerData, false);

	return timerData->TimerState == ETimerState::Paused;
}

bool TimerManager::IsValid(TimerId id)
{
	return GetTimerData(id) != nullptr;
}

void TimerManager::Update(Duration dt)
{
	for (auto& timerData : m_Timers)
	{
		ContinueIf(timerData.TimerState != ETimerState::Running);

		timerData.Elapsed += dt;

		ContinueIf(timerData.Elapsed < timerData.Interval);

		if (timerData.Callback)
		{
			m_CallbacksToExecute.emplace_back(timerData.Callback);
		}

		if (timerData.TimerType == ETimerType::Pulse)
		{
			timerData.Elapsed -= timerData.Interval;
		}
		else
		{
			Stop(timerData.Id);
		}
	}

	for (auto& callback : m_CallbacksToExecute)
	{
		callback();
	}

	m_CallbacksToExecute.clear();
}

TimerData* TimerManager::GetTimerData(TimerId id)
{
	ReturnIf(id.Index >= m_Timers.size(), nullptr);

	auto& timerData = m_Timers[id.Index];

	ReturnIf(timerData.Id.Generation != id.Generation, nullptr);
	ReturnIf(timerData.TimerState == ETimerState::Free, nullptr);

	return &timerData;
}
