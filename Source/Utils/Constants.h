#pragma once

#include <cstdint>

namespace Constants
{
inline constexpr int64_t TimeSecondsInMinute = 60;
inline constexpr int64_t TimeMinutesInHour	 = 60;
inline constexpr int64_t TimeHoursInDay		 = 24;
inline constexpr int64_t TimeDaysInYear		 = 365;

inline constexpr int64_t TimeSecondsInHour = TimeSecondsInMinute * TimeMinutesInHour;
inline constexpr int64_t TimeSecondsInDay  = TimeSecondsInHour * TimeHoursInDay;
inline constexpr int64_t TimeSecondsInYear = TimeSecondsInDay * TimeDaysInYear;

inline constexpr int64_t TimeMinutesInDay  = TimeMinutesInHour * TimeHoursInDay;
inline constexpr int64_t TimeMinutesInYear = TimeMinutesInDay * TimeDaysInYear;

inline constexpr int64_t TimeHoursInYear = TimeHoursInDay * TimeDaysInYear;
} // namespace Constants
