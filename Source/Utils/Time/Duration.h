#pragma once

// A duration of time, a time interval if you will
class Duration
{
public:
	using Nanoseconds  = std::chrono::nanoseconds;
	using Milliseconds = std::chrono::milliseconds;
	using Microseconds = std::chrono::microseconds;
	using Seconds	   = std::chrono::seconds;
	using Minutes	   = std::chrono::minutes;
	using Hours		   = std::chrono::hours;
	using Days		   = std::chrono::days;
	using Weeks		   = std::chrono::weeks;
	using Months	   = std::chrono::months;
	using Years		   = std::chrono::years;

	friend std::ostream& operator<<(std::ostream& out, Duration dt);

public:
	Duration() = default;

	template <typename Rep, typename Period>
	Duration(std::chrono::duration<Rep, Period> d);

	// Would this work?
	operator std::chrono::nanoseconds() const;

	auto operator<=>(const Duration& rhs) const = default;

	Duration operator+(const Duration& rhs) const;
	Duration operator-(const Duration& rhs) const;

	Duration  operator-() const;
	Duration& operator+=(const Duration& rhs);
	Duration& operator-=(const Duration& rhs);

	template <typename T>
	T As() const;

	int64_t As(EUnitOfTime unit) const;

private:
	Nanoseconds m_Value;
};

template <typename Rep, typename Period>
inline Duration::Duration(std::chrono::duration<Rep, Period> d)
	: m_Value(std::chrono::duration_cast<Nanoseconds>(d))
{
}

template <typename T>
inline T Duration::As() const
{
	return std::chrono::duration_cast<T>(m_Value);
}
