#pragma once

// Steady clock time utility
class Time
{
public:
	using Clock		= std::chrono::steady_clock;
	using Timepoint = Clock::time_point;
	using Duration	= Clock::duration;

public:
	Time();
	explicit Time(Timepoint tp);

	auto operator<=>(const Time& rhs) const = default;

	static Time Now();

	void Reset();

	Duration Elapsed() const;
	uint64_t ElapsedAs(EUnitOfTime unit) const;

	template <typename TDuration = std::chrono::milliseconds>
	uint64_t ElapsedAs() const;

	bool Expired(Duration timeout) const;

private:
	Timepoint m_Start;
};
