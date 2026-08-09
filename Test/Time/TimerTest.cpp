#include "stdafx.h"

#include "Time/TimerTest.h"

#include "TestReporter.h"

#include "Time/Timer.h"
#include "Time/TimerManager.h"

namespace Test
{
namespace
{

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
