#include "Felis/stdafx.h"

#include "Felis/Time/Time.h"

#define FELIS_USE_CHRONO_FORMATTER

namespace TimeFormat
{
std::string ToString(DateTime dateTime, ETimeStringFormat format)
{
#ifdef FELIS_USE_CHRONO_FORMATTER
	// https://en.cppreference.com/cpp/chrono/duration/formatter

	auto tp = floor<Duration::Microseconds>(dateTime.GetTimepoint());

	// TODO this formatting is quite slow, find alternatives
	switch (format)
	{
	default:
	case ETimeStringFormat::Default: return std::format("{:%d.%m.%Y %H:%M:%S}", tp);
	case ETimeStringFormat::Timestamp: return std::format("{:%Y%m%d%H%M%S}", tp);
	case ETimeStringFormat::Timepoint: return std::format("{}", tp);
	}

#else
	auto tp			= dateTime.GetTimepoint();
	auto time_t_val = std::chrono::system_clock::to_time_t(tp);

	std::tm tm_buf;
	localtime_r(&time_t_val, &tm_buf);

	char		buffer[32];
	const char* fmt_str = nullptr;

	switch (format)
	{
	default:
	case ETimeStringFormat::Default: fmt_str = "%d.%m.%Y %H:%M:%S"; break;
	case ETimeStringFormat::Timestamp: fmt_str = "%Y%m%d%H%M%S"; break;
	case ETimeStringFormat::Timepoint: fmt_str = "%H:%M:%S"; break;
	}

	std::strftime(buffer, sizeof(buffer), fmt_str, &tm_buf);

	return std::string(buffer);

#endif
}
} // namespace TimeFormat

std::ostream& operator<<(std::ostream& os, DateTime dateTime)
{
	os << TimeFormat::ToString(dateTime, ETimeStringFormat::Default);

	return os;
}
