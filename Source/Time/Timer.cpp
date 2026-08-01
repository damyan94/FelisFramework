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

void Timer::Start(ETimerType timerType, Duration interval, TimerCallback callback)
{
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
