#include "stdafx.h"

#include "Time/Timer.h"

#include "Time/TimerManager.h"

Timer::Timer()
	: m_Manager(&TimerManager::Instance())
{
}

Timer::~Timer()
{
	Stop();
}

Timer::Timer(Timer&& other)
	: m_Manager(other.m_Manager),
	  m_Id(std::exchange(other.m_Id, TimerId{}))
{
}

Timer& Timer::operator=(Timer&& other)
{
	ReturnIf(this == &other, *this);

	Stop();
	m_Manager = other.m_Manager;
	m_Id	  = std::exchange(other.m_Id, TimerId{});

	return *this;
}

void Timer::Start(ETimerType timerType, Duration interval, TimerCallback callback)
{
	AssertReturnIf((IsValid() || IsRunning() || IsPaused()) && "Attempting to create an already existent timer.");

	m_Id = m_Manager->Start(timerType, interval, callback);
}

void Timer::Pause()
{
	m_Manager->Pause(m_Id);
}

void Timer::Resume()
{
	m_Manager->Resume(m_Id);
}

void Timer::Stop()
{
	m_Manager->Stop(m_Id);
}

void Timer::Restart()
{
	m_Manager->Restart(m_Id);
}

bool Timer::IsRunning() const
{
	return m_Manager->IsRunning(m_Id);
}

bool Timer::IsPaused() const
{
	return m_Manager->IsPaused(m_Id);
}

bool Timer::IsValid() const
{
	return m_Manager->IsValid(m_Id);
}

ETimerType Timer::GetTimerType() const
{
	return m_Manager->GetTimerType(m_Id);
}

ETimerState Timer::GetTimerState() const
{
	return m_Manager->GetTimerState(m_Id);
}

Duration Timer::GetInterval() const
{
	return m_Manager->GetInterval(m_Id);
}

Duration Timer::GetRemaining() const
{
	return m_Manager->GetRemaining(m_Id);
}

Duration Timer::GetElapsed() const
{
	return m_Manager->GetElapsed(m_Id);
}

float Timer::GetProgress() const
{
	return m_Manager->GetProgress(m_Id);
}

TimerManager* Timer::GetManager()
{
	return m_Manager;
}

TimerId Timer::GetId() const
{
	return m_Id;
}
