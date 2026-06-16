#include "stdafx.h"

#include "Utils/Time/DateTime.h"

using namespace std::chrono;

DateTime::DateTime()
	: m_Time(Clock::now())
{
}

DateTime::DateTime(Timepoint tp)
	: m_Time(tp)
{
}

DateTime DateTime::Now()
{
	return DateTime(Clock::now());
}

void DateTime::SetToNow()
{
	m_Time = Clock::now();
}

DateTime::Timepoint DateTime::GetTimepoint() const
{
	return m_Time;
}

int64_t DateTime::ToUnixSeconds() const
{
	return duration_cast<seconds>(m_Time.time_since_epoch()).count();
}

int64_t DateTime::ToUnixMilliseconds() const
{
	return duration_cast<milliseconds>(m_Time.time_since_epoch()).count();
}

std::string DateTime::ToString(ETimeStringFormat format) const
{
	auto tp = floor<microseconds>(m_Time);

	switch (format)
	{
	default:
	case ETimeStringFormat::Default: return std::format("{:%d.%m.%Y %H:%M:%S}", tp);
	case ETimeStringFormat::Timestamp: return std::format("{:%Y%m%d%H%M%S}", tp);
	case ETimeStringFormat::Timepoint: return std::format("{}", m_Time);
	}
}
