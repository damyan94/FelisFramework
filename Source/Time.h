#pragma once

using SystemClock = std::chrono::system_clock;
using Timepoint	  = SystemClock::time_point;
using Duration	  = SystemClock::duration;

class Time
{
public:
	Time();
	explicit Time(Timepoint tp);
	~Time() = default;

	Time(const Time& other)			   = default;
	Time& operator=(const Time& other) = default;
	Time(Time&& other)				   = default;
	Time& operator=(Time&& other)	   = default;

	bool	 operator<=>(const Time& other) const = default;
	Time	 operator+(Duration d) const;
	Time	 operator-(Duration d) const;
	Duration operator-(const Time& other) const;

	static Time Now();
	void		SetToNow();

	Timepoint	GetTimePoint() const;
	uint64_t	GetAs(EUnitOfTime unit) const;
	std::string GetString(ETimeStringFormat format) const;

	Duration GetElapsedTime() const;
	uint64_t GetElapsedTime(EUnitOfTime unit) const;

	static uint64_t ConvertTo(Duration tp, EUnitOfTime unit);

private:
	Timepoint m_TimePoint;
};
