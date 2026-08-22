#include "Felis/stdafx.h"

#include "Felis/Time/Timer.h"

#include "Felis/Time/TimerManager.h"

namespace Felis
{

Timer::Timer()
{
}

Timer::~Timer()
{
	Stop();
}

Timer::Timer(Timer&& other)
	: m_Id(std::exchange(other.m_Id, TimerId{}))
{
}

Timer& Timer::operator=(Timer&& other)
{
	ReturnIf(this == &other, *this);

	Stop();
	m_Id = std::exchange(other.m_Id, TimerId{});

	return *this;
}

void Timer::Start(ETimerType timerType, Duration interval, TimerCallback callback)
{
	AssertReturnIf((IsValid() || IsRunning() || IsPaused()) && "Attempting to create an already existent timer.");

	m_Id = TimerManager::Instance().Start(timerType, interval, callback);
}

void Timer::Pause()
{
	TimerManager::Instance().Pause(m_Id);
}

void Timer::Resume()
{
	TimerManager::Instance().Resume(m_Id);
}

void Timer::Stop()
{
	TimerManager::Instance().Stop(m_Id);
}

void Timer::Restart()
{
	TimerManager::Instance().Restart(m_Id);
}

bool Timer::IsRunning() const
{
	return TimerManager::Instance().IsRunning(m_Id);
}

bool Timer::IsPaused() const
{
	return TimerManager::Instance().IsPaused(m_Id);
}

bool Timer::IsValid() const
{
	return TimerManager::Instance().IsValid(m_Id);
}

ETimerType Timer::GetTimerType() const
{
	return TimerManager::Instance().GetTimerType(m_Id);
}

ETimerState Timer::GetTimerState() const
{
	return TimerManager::Instance().GetTimerState(m_Id);
}

Duration Timer::GetInterval() const
{
	return TimerManager::Instance().GetInterval(m_Id);
}

Duration Timer::GetRemaining() const
{
	return TimerManager::Instance().GetRemaining(m_Id);
}

Duration Timer::GetElapsed() const
{
	return TimerManager::Instance().GetElapsed(m_Id);
}

float Timer::GetProgress() const
{
	return TimerManager::Instance().GetProgress(m_Id);
}

TimerId Timer::GetId() const
{
	return m_Id;
}

} // namespace Felis
