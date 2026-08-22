#include "stdafx.h"

#include "Time/DurationTest.h"

#include "Felis/Time/Duration.h"

#include <regex>

// Compile-time checks for the ChronoDuration concept - not worth a runtime TEST_CHECK,
// since a failure here is a compile error, not a boolean result.
static_assert(Felis::ChronoDuration<std::chrono::seconds>);
static_assert(Felis::ChronoDuration<std::chrono::nanoseconds>);
static_assert(!Felis::ChronoDuration<int>);
static_assert(!Felis::ChronoDuration<double>);

namespace Test
{
namespace
{

void TestDefaultConstructedIsZero(TestReporter& r)
{
	Duration d;
	TEST_CHECK(r, d == Duration::Zero);
}

void TestConstructsFromChronoDurationAndRoundTrips(TestReporter& r)
{
	Duration d(std::chrono::seconds(5));

	TEST_CHECK(r, d.As<Duration::Seconds>().count() == 5);
	TEST_CHECK(r, d.As<Duration::Milliseconds>().count() == 5000);
}

void TestAsEnumOverloadMatchesTemplateOverload(TestReporter& r)
{
	Duration d(std::chrono::minutes(90));

	TEST_CHECK(r, d.As(EUnitOfTime::Minute) == 90);
	TEST_CHECK(r, d.As(EUnitOfTime::Hour) == 1); // truncates: 90min = 1.5h
	TEST_CHECK(r, d.As(EUnitOfTime::Second) == 5400);
}

void TestLargeUnitsRoundTripExactly(TestReporter& r)
{
	Duration weeks	= Duration::Weeks(2);
	Duration months = Duration::Months(3);
	Duration years	= Duration::Years(1);

	TEST_CHECK(r, weeks.As(EUnitOfTime::Week) == 2);
	TEST_CHECK(r, months.As(EUnitOfTime::Month) == 3);
	TEST_CHECK(r, years.As(EUnitOfTime::Year) == 1);
}

void TestArithmeticOperators(TestReporter& r)
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

void TestComparisonOperators(TestReporter& r)
{
	Duration small = Duration::Milliseconds(100);
	Duration big   = Duration::Milliseconds(200);

	TEST_CHECK(r, small < big);
	TEST_CHECK(r, big > small);
	TEST_CHECK(r, small <= small);
	TEST_CHECK(r, small == Duration::Milliseconds(100));
	TEST_CHECK(r, small != big);
}

void TestImplicitConversionToNanoseconds(TestReporter& r)
{
	Duration				 d	= Duration::Milliseconds(2);
	std::chrono::nanoseconds ns = d;

	TEST_CHECK(r, ns.count() == 2'000'000);
}

void TestZeroIsActuallyZero(TestReporter& r)
{
	TEST_CHECK(r, Duration::Zero.As(EUnitOfTime::Nanosecond) == 0);
}

void TestStreamInsertionPrintsNanosecondValue(TestReporter& r)
{
	Duration d = Duration::Nanoseconds(1500);

	std::ostringstream oss;
	oss << d;

	// operator<< forwards to std::chrono::nanoseconds's own stream operator, which
	// appends the "ns" unit suffix after the raw count
	TEST_CHECK(r, oss.str() == "1500ns");
}

} // namespace

int TestDuration()
{
	TestReporter r("DurationTest");

	TestDefaultConstructedIsZero(r);
	TestConstructsFromChronoDurationAndRoundTrips(r);
	TestAsEnumOverloadMatchesTemplateOverload(r);
	TestLargeUnitsRoundTripExactly(r);
	TestArithmeticOperators(r);
	TestComparisonOperators(r);
	TestImplicitConversionToNanoseconds(r);
	TestZeroIsActuallyZero(r);
	TestStreamInsertionPrintsNanosecondValue(r);

	r.PrintSummary();
	return r.GetFailures();
}

} // namespace Test
