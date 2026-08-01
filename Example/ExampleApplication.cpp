#include "stdafx.h"

#include "ExampleApplication.h"

#include "Time/Timer.h"
#include "Time/TimerManager.h"

ExampleApplication::ExampleApplication(int argC, char** argV)
	: IApplication(argC, argV)
{
}

bool ExampleApplication::OnInit()
{
	LogDebug(">>> Running ", m_Args.GetProgramName(), "; ", m_Args.GetArgC());
	LogDebug(">>> OnInit called");

	return true;
}

void ExampleApplication::OnRun()
{
	LogDebug(">>> OnRun called");

	Timer t;
	int	  ticks = 0;
	t.Start(ETimerType::Pulse,
			Duration::Seconds(2),
			[&ticks]
			{
				LogInfo("Timer tick ", ticks);
				ticks++;
			});

	while (ticks <= 5)
	{
		TimerManager::Instance().Update(Duration::Milliseconds(16));
		std::this_thread::sleep_for(Duration::Milliseconds(16));
	}
}

void ExampleApplication::OnDeinit()
{
	LogDebug(">>> OnDeinit called");
}
