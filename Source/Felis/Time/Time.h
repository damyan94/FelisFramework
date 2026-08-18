#pragma once

#include "Felis/Time/Duration.h"

template <typename T>
concept ChronoClock = std::chrono::is_clock_v<T>;

// Basic time class containing common logic
template <ChronoClock ClockType>
class BasicTime
{
public:
	using Clock		= ClockType;
	using Timepoint = Clock::time_point;

public:
	BasicTime();
	explicit BasicTime(Timepoint tp);

	FELIS_DEFAULT_COPY_AND_MOVE(BasicTime);

	auto operator<=>(const BasicTime& rhs) const = default;

	Duration operator-(const BasicTime& rhs) const;

	BasicTime operator+(const Duration& d) const;
	BasicTime operator-(const Duration& d) const;

	BasicTime& operator+=(const Duration& d);
	BasicTime& operator-=(const Duration& d);

	static BasicTime Now();

	void Reset();

	Duration		 GetElapsed() const;
	const Timepoint& GetTimepoint() const;

protected:
	Timepoint m_Start;
};

template <ChronoClock ClockType>
inline BasicTime<ClockType>::BasicTime()
	: m_Start(Clock::now())
{
}

template <ChronoClock ClockType>
inline BasicTime<ClockType>::BasicTime(Timepoint tp)
	: m_Start(tp)
{
}

template <ChronoClock ClockType>
inline Duration BasicTime<ClockType>::operator-(const BasicTime& rhs) const
{
	return m_Start - rhs.m_Start;
}

template <ChronoClock ClockType>
inline BasicTime<ClockType> BasicTime<ClockType>::operator+(const Duration& d) const
{
	return BasicTime<ClockType>(m_Start + d.As<Duration::Nanoseconds>());
}

template <ChronoClock ClockType>
inline BasicTime<ClockType> BasicTime<ClockType>::operator-(const Duration& d) const
{
	return BasicTime<ClockType>(m_Start - d.As<Duration::Nanoseconds>());
}

template <ChronoClock ClockType>
inline BasicTime<ClockType>& BasicTime<ClockType>::operator+=(const Duration& d)
{
	m_Start += d.As<Duration::Nanoseconds>();
	return *this;
}

template <ChronoClock ClockType>
inline BasicTime<ClockType>& BasicTime<ClockType>::operator-=(const Duration& d)
{
	m_Start -= d.As<Duration::Nanoseconds>();
	return *this;
}

template <ChronoClock ClockType>
inline BasicTime<ClockType> BasicTime<ClockType>::Now()
{
	return BasicTime<ClockType>();
}

template <ChronoClock ClockType>
inline void BasicTime<ClockType>::Reset()
{
	m_Start = Clock::now();
}

template <ChronoClock ClockType>
inline Duration BasicTime<ClockType>::GetElapsed() const
{
	return Duration(Clock::now() - m_Start);
}

template <ChronoClock ClockType>
inline const BasicTime<ClockType>::Timepoint& BasicTime<ClockType>::GetTimepoint() const
{
	return m_Start;
}

// Steady clock time utility
using Time = BasicTime<std::chrono::steady_clock>;

// System clock time utility
using DateTime = BasicTime<std::chrono::system_clock>;

// TODO move to separate file
namespace TimeFormat
{
std::string ToString(DateTime dateTime, ETimeStringFormat format = ETimeStringFormat::Default);
} // namespace TimeFormat

extern std::ostream& operator<<(std::ostream& os, DateTime dateTime);
