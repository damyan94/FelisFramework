#pragma once

#include <cstdint>

namespace Constants
{
namespace Application
{
inline constexpr int DefaultFPS					 = 60;
inline constexpr int DefaultMicrosecondsPerFrame = 1000 * 1000 / 60;
} // namespace Application

namespace Time
{
inline constexpr int SecondsInMinute = 60;
inline constexpr int MinutesInHour	 = 60;
inline constexpr int HoursInDay		 = 24;
inline constexpr int DaysInYear		 = 365;

inline constexpr int SecondsInHour = SecondsInMinute * MinutesInHour;
inline constexpr int SecondsInDay  = SecondsInHour * HoursInDay;
inline constexpr int SecondsInYear = SecondsInDay * DaysInYear;

inline constexpr int MinutesInDay  = MinutesInHour * HoursInDay;
inline constexpr int MinutesInYear = MinutesInDay * DaysInYear;

inline constexpr int HoursInYear = HoursInDay * DaysInYear;
} // namespace Time
} // namespace Constants
