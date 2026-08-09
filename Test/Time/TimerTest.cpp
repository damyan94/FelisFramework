#include "stdafx.h"

#include "Time/TimerTest.h"

#include <regex>

#include "TestReporter.h"

#include "Time/Duration.h"
#include "Time/Time.h"
#include "Time/Timer.h"
#include "Time/TimerManager.h"

// Compile-time checks for the ChronoDuration concept - not worth a runtime TEST_CHECK,
// since a failure here is a compile error, not a boolean result.
static_assert(ChronoDuration<std::chrono::seconds>);
static_assert(ChronoDuration<std::chrono::nanoseconds>);
static_assert(!ChronoDuration<int>);
static_assert(!ChronoDuration<double>);

namespace Test
{
namespace
{

// ---- Duration ----

void TestDuration_DefaultConstructedIsZero(TestReporter& r)
{
	Duration d;
	TEST_CHECK(r, d == Duration::Zero);
}

void TestDuration_ConstructsFromChronoDurationAndRoundTrips(TestReporter& r)
{
	Duration d(std::chrono::seconds(5));

	TEST_CHECK(r, d.As<Duration::Seconds>().count() == 5);
	TEST_CHECK(r, d.As<Duration::Milliseconds>().count() == 5000);
}

void TestDuration_AsEnumOverloadMatchesTemplateOverload(TestReporter& r)
{
	Duration d(std::chrono::minutes(90));

	TEST_CHECK(r, d.As(EUnitOfTime::Minute) == 90);
	TEST_CHECK(r, d.As(EUnitOfTime::Hour) == 1); // truncates: 90min = 1.5h
	TEST_CHECK(r, d.As(EUnitOfTime::Second) == 5400);
}

void TestDuration_LargeUnitsRoundTripExactly(TestReporter& r)
{
	Duration weeks	= Duration::Weeks(2);
	Duration months = Duration::Months(3);
	Duration years	= Duration::Years(1);

	TEST_CHECK(r, weeks.As(EUnitOfTime::Week) == 2);
	TEST_CHECK(r, months.As(EUnitOfTime::Month) == 3);
	TEST_CHECK(r, years.As(EUnitOfTime::Year) == 1);
}

void TestDuration_ArithmeticOperators(TestReporter& r)
{
	Duration a = Duration::Seconds(5);
	Duration b = Duration::Seconds(3);

	TEST_CHECK(r, (a + b) == Duration::Seconds(8));
	TEST_CHECK(r, (a - b) == Duration::Seconds(2));
	TEST_CHECK(r, (-a) == Duration::Seconds(-5));

	Duration c = a;
	c += b;
	TEST_CHECK(r, c == Duration::Seconds(8));

	c -= b;
	TEST_CHECK(r, c == Duration::Seconds(5));
}

void TestDuration_ComparisonOperators(TestReporter& r)
{
	Duration small = Duration::Milliseconds(100);
	Duration big   = Duration::Milliseconds(200);

	TEST_CHECK(r, small < big);
	TEST_CHECK(r, big > small);
	TEST_CHECK(r, small <= small);
	TEST_CHECK(r, small == Duration::Milliseconds(100));
	TEST_CHECK(r, small != big);
}

void TestDuration_ImplicitConversionToNanoseconds(TestReporter& r)
{
	Duration				 d	= Duration::Milliseconds(2);
	std::chrono::nanoseconds ns = d;

	TEST_CHECK(r, ns.count() == 2'000'000);
}

void TestDuration_ZeroIsActuallyZero(TestReporter& r)
{
	TEST_CHECK(r, Duration::Zero.As(EUnitOfTime::Nanosecond) == 0);
}

void TestDuration_StreamInsertionPrintsNanosecondValue(TestReporter& r)
{
	Duration d = Duration::Nanoseconds(1500);

	std::ostringstream oss;
	oss << d;

	// operator<< forwards to std::chrono::nanoseconds's own stream operator, which
	// appends the "ns" unit suffix after the raw count
	TEST_CHECK(r, oss.str() == "1500ns");
}

// ---- Time / DateTime (via BasicTime<ClockType>) ----

void TestTime_NowElapsedIsNearZero(TestReporter& r)
{
	Time t;

	TEST_CHECK(r, t.GetElapsed() < Duration::Milliseconds(50));
}

void TestTime_GetElapsedGrowsWithRealTime(TestReporter& r)
{
	Time t;
	std::this_thread::sleep_for(std::chrono::milliseconds(20));

	TEST_CHECK(r, t.GetElapsed() >= Duration::Milliseconds(15));
}

void TestTime_ResetRestartsElapsed(TestReporter& r)
{
	Time t;
	std::this_thread::sleep_for(std::chrono::milliseconds(20));
	t.Reset();

	TEST_CHECK(r, t.GetElapsed() < Duration::Milliseconds(15));
}

void TestTime_OperatorPlusMinusDurationShiftsTimepoint(TestReporter& r)
{
	Time now;

	Time future = now + Duration::Seconds(5);
	Time past	= now - Duration::Seconds(5);

	TEST_CHECK(r, future > now);
	TEST_CHECK(r, past < now);
}

void TestTime_OperatorPlusEqualsMinusEqualsMutateInPlace(TestReporter& r)
{
	Time t		  = Time::Now();
	Time original = t;

	t += Duration::Seconds(10);
	TEST_CHECK(r, t > original);

	t -= Duration::Seconds(10);
	TEST_CHECK(r, t == original);
}

void TestTime_SubtractingTwoTimepointsGivesExactDuration(TestReporter& r)
{
	Time a = Time::Now();
	Time b{a + Duration::Seconds(2)};

	// arithmetic on timepoints is exact (nanosecond precision), no real waiting involved
	TEST_CHECK(r, (b - a) == Duration::Seconds(2));
}

void TestTime_ComparisonOperatorsOrderCorrectly(TestReporter& r)
{
	Time a;
	Time b = a + Duration::Nanoseconds(1);

	TEST_CHECK(r, a < b);
	TEST_CHECK(r, b > a);
	TEST_CHECK(r, a == a);
	TEST_CHECK(r, a != b);
}

void TestTime_ExplicitTimepointConstructorRoundTrips(TestReporter& r)
{
	// Time/DateTime don't currently inherit this constructor from BasicTime (no
	// using-declaration in either derived class), so it's exercised directly against
	// the base template here rather than through Time itself.
	using SteadyBasicTime = BasicTime<std::chrono::steady_clock>;

	SteadyBasicTime::Timepoint tp = std::chrono::steady_clock::now();
	SteadyBasicTime			   bt(tp);

	TEST_CHECK(r, bt.GetTimepoint() == tp);
}

void TestDateTime_ToStringDefaultFormatMatchesShape(TestReporter& r)
{
	DateTime dt;

	// dd.mm.yyyy HH:MM:SS.ffffff
	static const std::regex pattern(R"(^\d{2}\.\d{2}\.\d{4} \d{2}:\d{2}:\d{2}.\d{6}$)");
	TEST_CHECK(r, std::regex_match(TimeFormat::ToString(dt, ETimeStringFormat::Default), pattern));
}

void TestDateTime_ToStringTimestampFormatMatchesShape(TestReporter& r)
{
	DateTime dt;

	// yyyymmddHHMMSS.ffffff
	static const std::regex pattern(R"(^\d{14}.\d{6}$)");
	TEST_CHECK(r, std::regex_match(TimeFormat::ToString(dt, ETimeStringFormat::Timestamp), pattern));
}

void TestDateTime_ToStringTimepointFormatMatchesShape(TestReporter& r)
{
	DateTime dt;

	// yyyy-mm-dd HH:MM:SS.ffffff (default chrono sys_time formatting, floored to microseconds)
	static const std::regex pattern(R"(^\d{4}-\d{2}-\d{2} \d{2}:\d{2}:\d{2}\.\d{6}$)");
	TEST_CHECK(r, std::regex_match(TimeFormat::ToString(dt, ETimeStringFormat::Timepoint), pattern));
}

// ---- Timer ----

void TestBasicStartStop(TestReporter& r)
{
	Timer t;

	TEST_CHECK(r, !t.IsValid());
	TEST_CHECK(r, !t.IsRunning());

	t.Start(ETimerType::Oneshot, Duration::Seconds(1), [] {});

	TEST_CHECK(r, t.IsValid());
	TEST_CHECK(r, t.IsRunning());
	TEST_CHECK(r, !t.IsPaused());

	t.Stop();

	TEST_CHECK(r, !t.IsValid());
	TEST_CHECK(r, !t.IsRunning());
}

void TestOneshotDoesNotFireEarly(TestReporter& r)
{
	auto& tm = TimerManager::Instance();

	int	  tickCount = 0;
	Timer t;
	t.Start(ETimerType::Oneshot, Duration::Seconds(1), [&tickCount] { ++tickCount; });

	tm.Update(Duration::Milliseconds(500));

	TEST_CHECK(r, tickCount == 0);
	TEST_CHECK(r, t.IsRunning());

	t.Stop();
}

void TestOneshotFiresOnceAndStops(TestReporter& r)
{
	auto& tm = TimerManager::Instance();

	int	  tickCount = 0;
	Timer t;
	t.Start(ETimerType::Oneshot, Duration::Seconds(1), [&tickCount] { ++tickCount; });

	tm.Update(Duration::Milliseconds(600));
	tm.Update(Duration::Milliseconds(600)); // crosses the 1s mark

	TEST_CHECK(r, tickCount == 1);
	TEST_CHECK(r, !t.IsRunning());
	TEST_CHECK(r, !t.IsValid());

	// further updates must not re-fire a stopped oneshot
	tm.Update(Duration::Seconds(5));
	TEST_CHECK(r, tickCount == 1);
}

void TestPulseFiresRepeatedly(TestReporter& r)
{
	auto& tm = TimerManager::Instance();

	int	  tickCount = 0;
	Timer t;
	t.Start(ETimerType::Pulse, Duration::Seconds(1), [&tickCount] { ++tickCount; });

	for (int i = 0; i < 3; ++i)
	{
		tm.Update(Duration::Seconds(1));
	}

	TEST_CHECK(r, tickCount == 3);
	TEST_CHECK(r, t.IsRunning()); // pulse timers keep going until explicitly stopped

	t.Stop();
}

void TestPauseStopsElapsedAccumulation(TestReporter& r)
{
	auto& tm = TimerManager::Instance();

	int	  tickCount = 0;
	Timer t;
	t.Start(ETimerType::Oneshot, Duration::Seconds(1), [&tickCount] { ++tickCount; });

	tm.Update(Duration::Milliseconds(500));
	t.Pause();

	TEST_CHECK(r, t.IsPaused());
	TEST_CHECK(r, !t.IsRunning());

	// time passes while paused, must not count towards elapsed
	tm.Update(Duration::Seconds(5));
	TEST_CHECK(r, tickCount == 0);
	TEST_CHECK(r, t.GetElapsed() == Duration::Milliseconds(500));

	t.Resume();
	TEST_CHECK(r, t.IsRunning());

	tm.Update(Duration::Milliseconds(500)); // now crosses the 1s mark
	TEST_CHECK(r, tickCount == 1);

	t.Stop();
}

void TestRestartResetsElapsed(TestReporter& r)
{
	auto& tm = TimerManager::Instance();

	int	  tickCount = 0;
	Timer t;
	t.Start(ETimerType::Oneshot, Duration::Seconds(1), [&tickCount] { ++tickCount; });

	tm.Update(Duration::Milliseconds(800));
	TEST_CHECK(r, t.GetElapsed() == Duration::Milliseconds(800));

	t.Restart();
	TEST_CHECK(r, t.GetElapsed() == Duration::Zero);
	TEST_CHECK(r, t.IsRunning());

	// only 800ms since restart, should not have fired yet
	tm.Update(Duration::Milliseconds(800));
	TEST_CHECK(r, tickCount == 0);

	t.Stop();
}

void TestGettersReportCorrectValues(TestReporter& r)
{
	auto& tm = TimerManager::Instance();

	Timer t;
	t.Start(ETimerType::Oneshot, Duration::Seconds(4), [] {});

	tm.Update(Duration::Seconds(1));

	TEST_CHECK(r, t.GetInterval() == Duration::Seconds(4));
	TEST_CHECK(r, t.GetElapsed() == Duration::Seconds(1));
	TEST_CHECK(r, t.GetRemaining() == Duration::Seconds(3));
	TEST_CHECK(r, t.GetTimerType() == ETimerType::Oneshot);
	TEST_CHECK(r, t.GetTimerState() == ETimerState::Running);

	float progress = t.GetProgress();
	TEST_CHECK(r, progress > 0.24f && progress < 0.26f); // ~0.25

	t.Stop();
}

void TestStaleIdIsInvalidatedAfterSlotReuse(TestReporter& r)
{
	auto& tm = TimerManager::Instance();

	Timer t1;
	t1.Start(ETimerType::Oneshot, Duration::Seconds(1), [] {});
	TimerId staleId = t1.GetId();

	t1.Stop();
	TEST_CHECK(r, !tm.IsValid(staleId));

	// starting a new timer is likely to reuse the freed slot (LIFO free list)
	Timer t2;
	t2.Start(ETimerType::Oneshot, Duration::Seconds(1), [] {});

	// the stale id from t1 must never be reported as valid again,
	// even if it aliases the same slot index with a new generation
	TEST_CHECK(r, !tm.IsValid(staleId));
	TEST_CHECK(r, tm.IsValid(t2.GetId()));

	t2.Stop();
}

void TestMoveConstructionTransfersOwnership(TestReporter& r)
{
	auto& tm = TimerManager::Instance();

	int tickCount = 0;

	{
		Timer t1;
		t1.Start(ETimerType::Oneshot, Duration::Seconds(1), [&tickCount] { ++tickCount; });
		TimerId id = t1.GetId();

		Timer t2(std::move(t1));

		TEST_CHECK(r, t2.GetId().Index == id.Index);
		TEST_CHECK(r, t2.GetId().Generation == id.Generation);
		TEST_CHECK(r, t2.IsRunning());

		// moved-from timer must no longer think it owns the timer
		TEST_CHECK(r, !t1.IsValid());

		tm.Update(Duration::Seconds(2)); // t1's destructor must not have stopped it
		TEST_CHECK(r, tickCount == 1);

		// t1 going out of scope here must not double-stop t2's (already-freed) slot
	}

	TEST_CHECK(r, tickCount == 1);
}

void TestMoveAssignmentStopsPreviousOwnedTimer(TestReporter& r)
{
	auto& tm = TimerManager::Instance();

	int tickCountA = 0;
	int tickCountB = 0;

	Timer a;
	a.Start(ETimerType::Oneshot, Duration::Seconds(1), [&tickCountA] { ++tickCountA; });

	Timer b;
	b.Start(ETimerType::Oneshot, Duration::Seconds(1), [&tickCountB] { ++tickCountB; });

	a = std::move(b);

	tm.Update(Duration::Seconds(2));

	// a's original timer must have been stopped by the move-assignment, so it never fires
	TEST_CHECK(r, tickCountA == 0);
	// a now owns what used to be b's timer, which should have fired
	TEST_CHECK(r, tickCountB == 1);
}

void TestOneshotStoppingItselfFromCallback(TestReporter& r)
{
	auto& tm = TimerManager::Instance();

	Timer* selfPtr = nullptr;
	Timer  t;
	t.Start(ETimerType::Oneshot,
			Duration::Seconds(1),
			[&selfPtr, &r]
			{
				// by the time a oneshot's callback runs, the manager should already
				// consider it stopped/invalid, even though Timer::Stop() was never called
				TEST_CHECK(r, selfPtr != nullptr);
				TEST_CHECK(r, !selfPtr->IsRunning());
			});
	selfPtr = &t;

	tm.Update(Duration::Seconds(1));
}

void TestMultipleIndependentTimersDoNotInterfere(TestReporter& r)
{
	auto& tm = TimerManager::Instance();

	int	  ticksFast = 0;
	int	  ticksSlow = 0;
	Timer fast;
	Timer slow;

	fast.Start(ETimerType::Pulse, Duration::Milliseconds(500), [&ticksFast] { ++ticksFast; });
	slow.Start(ETimerType::Pulse, Duration::Seconds(2), [&ticksSlow] { ++ticksSlow; });

	for (int i = 0; i < 4; ++i)
	{
		tm.Update(Duration::Milliseconds(500)); // 2s total
	}

	TEST_CHECK(r, ticksFast == 4);
	TEST_CHECK(r, ticksSlow == 1);

	fast.Stop();
	slow.Stop();
}

void TestManagerLevelQueriesOnFreeSlotAreSafe(TestReporter& r)
{
	auto&	tm = TimerManager::Instance();
	TimerId neverStarted;

	TEST_CHECK(r, !tm.IsValid(neverStarted));
	TEST_CHECK(r, !tm.IsRunning(neverStarted));
	TEST_CHECK(r, !tm.IsPaused(neverStarted));
}

} // namespace

void TestDuration()
{
	TestReporter r("DurationTest");

	TestDuration_DefaultConstructedIsZero(r);
	TestDuration_ConstructsFromChronoDurationAndRoundTrips(r);
	TestDuration_AsEnumOverloadMatchesTemplateOverload(r);
	TestDuration_LargeUnitsRoundTripExactly(r);
	TestDuration_ArithmeticOperators(r);
	TestDuration_ComparisonOperators(r);
	TestDuration_ImplicitConversionToNanoseconds(r);
	TestDuration_ZeroIsActuallyZero(r);
	TestDuration_StreamInsertionPrintsNanosecondValue(r);

	r.PrintSummary();
}

void TestTime()
{
	TestReporter r("TimeTest");

	TestTime_NowElapsedIsNearZero(r);
	TestTime_GetElapsedGrowsWithRealTime(r);
	TestTime_ResetRestartsElapsed(r);
	TestTime_OperatorPlusMinusDurationShiftsTimepoint(r);
	TestTime_OperatorPlusEqualsMinusEqualsMutateInPlace(r);
	TestTime_SubtractingTwoTimepointsGivesExactDuration(r);
	TestTime_ComparisonOperatorsOrderCorrectly(r);
	TestTime_ExplicitTimepointConstructorRoundTrips(r);

	TestDateTime_ToStringDefaultFormatMatchesShape(r);
	TestDateTime_ToStringTimestampFormatMatchesShape(r);
	TestDateTime_ToStringTimepointFormatMatchesShape(r);

	r.PrintSummary();
}

void TestTimers()
{
	TestReporter r("TimerTest");

	TestBasicStartStop(r);
	TestOneshotDoesNotFireEarly(r);
	TestOneshotFiresOnceAndStops(r);
	TestPulseFiresRepeatedly(r);
	TestPauseStopsElapsedAccumulation(r);
	TestRestartResetsElapsed(r);
	TestGettersReportCorrectValues(r);
	TestStaleIdIsInvalidatedAfterSlotReuse(r);
	TestMoveConstructionTransfersOwnership(r);
	TestMoveAssignmentStopsPreviousOwnedTimer(r);
	TestOneshotStoppingItselfFromCallback(r);
	TestMultipleIndependentTimersDoNotInterfere(r);
	TestManagerLevelQueriesOnFreeSlotAreSafe(r);

	r.PrintSummary();
}

} // namespace Test
