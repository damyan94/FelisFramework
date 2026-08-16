#include "stdafx.h"

#include "Time/TimeTest.h"

#include "Felis/Time/Time.h"

#include <regex>

namespace Test
{
namespace
{

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

} // namespace

int TestTime()
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
	return r.GetFailures();
}

} // namespace Test
