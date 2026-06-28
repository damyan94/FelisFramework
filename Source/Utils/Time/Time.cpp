#include "stdafx.h"

#include "Utils/Time/Time.h"

namespace TimeFormat
{
std::string ToString(DateTime dateTime, ETimeStringFormat format)
{
	// https://en.cppreference.com/cpp/chrono/duration/formatter

	auto tp = floor<Duration::Microseconds>(dateTime.GetTimepoint());

	switch (format)
	{
	default:
	case ETimeStringFormat::Default: return std::format("{:%d.%m.%Y %H:%M:%S}", tp);
	case ETimeStringFormat::Timestamp: return std::format("{:%Y%m%d%H%M%S}", tp);
	case ETimeStringFormat::Timepoint: return std::format("{}", tp);
	}
}
} // namespace TimeFormat
