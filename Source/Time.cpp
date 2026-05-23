#include "stdafx.h"

#include "Time.h"

Time::Time()
{
	SetToNow();
}

Time::Time(Timepoint tp)
	: m_TimePoint(tp)
{
}

Time Time::operator+(Duration d) const
{
	return Time(m_TimePoint + d);
}

Time Time::operator-(Duration d) const
{
	return Time(m_TimePoint - d);
}

Duration Time::operator-(const Time& other) const
{
	return m_TimePoint - other.m_TimePoint;
}

Time Time::Now()
{
	return Time();
}

void Time::SetToNow()
{
	m_TimePoint = SystemClock::now();
}

Timepoint Time::GetTimePoint() const
{
	return m_TimePoint;
}

uint64_t Time::GetAs(EUnitOfTime unit) const
{
	return ConvertTo(m_TimePoint.time_since_epoch(), unit);
}

std::string Time::GetString(ETimeStringFormat format) const
{
	switch (format)
	{
	case ETimeStringFormat::Default: return std::format("{:%d.%m.%Y %H:%M:%OS}", m_TimePoint);
	case ETimeStringFormat::Timestamp: return std::format("{:%Y%m%d%H%M%OS}", m_TimePoint);
	case ETimeStringFormat::Timepoint: return std::format("{}", m_TimePoint);

	default:
		Assert("Invalid time string format. Returning default format.");
		return std::format("{:%d.%m.%Y %H:%M:%OS}", m_TimePoint);
	}
}

// NOTE: Uses system_clock - not safe for precise interval measurement
Duration Time::GetElapsedTime() const
{
	return SystemClock::now() - m_TimePoint;
}

// NOTE: Uses system_clock - not safe for precise interval measurement
uint64_t Time::GetElapsedTime(EUnitOfTime unit) const
{
	return ConvertTo(GetElapsedTime(), unit);
}

uint64_t Time::ConvertTo(Duration tp, EUnitOfTime unit)
{
	using namespace std::chrono;

	switch (unit)
	{
	case EUnitOfTime::Nanosecond: return duration_cast<nanoseconds>(tp).count();
	case EUnitOfTime::Microsecond: return duration_cast<microseconds>(tp).count();
	case EUnitOfTime::Millisecond: return duration_cast<milliseconds>(tp).count();
	case EUnitOfTime::Second: return duration_cast<seconds>(tp).count();
	case EUnitOfTime::Minute: return duration_cast<minutes>(tp).count();
	case EUnitOfTime::Hour: return duration_cast<hours>(tp).count();
	case EUnitOfTime::Day: return duration_cast<days>(tp).count();
	case EUnitOfTime::Week: return duration_cast<weeks>(tp).count();
	case EUnitOfTime::Month: return duration_cast<months>(tp).count();
	case EUnitOfTime::Year: return duration_cast<years>(tp).count();

	default: Assert("Invalid unit of time. Returning nanoseconds."); return duration_cast<nanoseconds>(tp).count();
	}
}
