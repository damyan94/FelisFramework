#include "stdafx.h"

#include "Time.h"
#include "Utils/Time/Time.h"

using namespace std::chrono;

Time::Time()
	: m_Start(Clock::now())
{
}

Time::Time(Timepoint tp)
	: m_Start(tp)
{
}

Time Time::Now()
{
	return Time(Clock::now());
}

void Time::Reset()
{
	m_Start = Clock::now();
}

Time::Duration Time::Elapsed() const
{
	return Clock::now() - m_Start;
}

uint64_t Time::ElapsedAs(EUnitOfTime unit) const
{
	const auto d = Elapsed();

	switch (unit)
	{
	case EUnitOfTime::Nanosecond: return duration_cast<nanoseconds>(d).count();
	case EUnitOfTime::Microsecond: return duration_cast<microseconds>(d).count();
	case EUnitOfTime::Millisecond: return duration_cast<milliseconds>(d).count();
	case EUnitOfTime::Second: return duration_cast<seconds>(d).count();
	case EUnitOfTime::Minute: return duration_cast<minutes>(d).count();
	case EUnitOfTime::Hour: return duration_cast<hours>(d).count();
	case EUnitOfTime::Day: return duration_cast<days>(d).count();
	}

	return 0;
}

template <typename TDuration>
uint64_t Time::ElapsedAs() const
{
	return std::chrono::duration_cast<TDuration>(Elapsed()).count();
}

bool Time::Expired(Duration timeout) const
{
	return Elapsed() >= timeout;
}
