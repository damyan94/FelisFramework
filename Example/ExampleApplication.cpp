#include "stdafx.h"

#include "ExampleApplication.h"

#include "Felis/Time/Timer.h"
#include "Felis/Time/TimerManager.h"

#include "Felis/Error/Error.h"
#include "Felis/Error/FelisError.h"

enum class MyErrorEnum
{
	a,
	b,
	c,
	Count
};

template <>
const char* Error<MyErrorEnum>::s_Type = "My error type";

template <>
const ErrorRegistry<MyErrorEnum> Error<MyErrorEnum>::s_Registry({
	ErrorData{"a"},
	ErrorData{"b"},
	//ErrorData{"c"},
});

ExampleApplication::ExampleApplication(int argC, char** argV)
	: Application(argC, argV)
{
}

ApplicationError ExampleApplication::OnInit()
{
	const auto& args = GetCommandLineArguments();

	LogDebug(">>> Running ", args.GetProgramName(), "; ", args.GetArgC());
	LogDebug(">>> OnInit called");

	Error e(EFelisErrorCode::Unknown);
	LogError(e.GetErrorData().Text, "; ", TimeFormat::ToString(e.GetTimestamp()));

	LogDebug(TimeFormat::ToString(DateTime::Now(), ETimeStringFormat::Timepoint));

	LogDebug(e);

	e = Error(EFelisErrorCode::Success);
	LogDebug(e);

	Error err(MyErrorEnum::c);
	LogDebug(err);

	return EApplicationErrorCode::Success;
}

ApplicationError ExampleApplication::OnRun()
{
	LogDebug(">>> OnRun called");

	auto& tm = TimerManager::Instance();

	Timer t1, t2;
	int	  ticks = 0;

	LogInfo("t1 started pulse with interval of 1s");
	t1.Start(ETimerType::Pulse,
			 Duration::Seconds(1),
			 [&ticks, &t1, &t2]
			 {
				 LogDebug("t1 tick ", ticks);
				 ticks++;

				 if (ticks == 3)
				 {
					 LogInfo("t1 stopped");
					 t1.Stop();
					 LogInfo("t2 started oneshot with interval of 3s");
					 t2.Start(ETimerType::Oneshot, Duration::Seconds(3), [] { LogDebug("t2 tick"); });
				 }
				 else if (ticks == 4)
				 {
					 t2.Pause();
				 }
			 });

	bool started = false;
	while (ticks <= 5)
	{
		if (ticks == 3 && !started)
		{
			started = true;
			LogInfo("t1 started with interval of 5s");
			t1.Start(ETimerType::Pulse,
					 Duration::Seconds(5),
					 [&ticks]
					 {
						 LogDebug("t1 tick ", ticks);
						 ticks++;
					 });
		}

		tm.Update();

		std::this_thread::sleep_for(Duration::Milliseconds(16));
	}

	return EApplicationErrorCode::Success;
}

ApplicationError ExampleApplication::OnDeinit()
{
	LogDebug(">>> OnDeinit called");

	return EApplicationErrorCode::Success;
}
