#pragma once

// System clock time utility
class DateTime
{
public:
	using Clock		= std::chrono::system_clock;
	using Timepoint = Clock::time_point;
	using Duration	= Clock::duration;

public:
	DateTime();
	explicit DateTime(Timepoint tp);

	auto operator<=>(const DateTime& rhs) const = default;

	static DateTime Now();

	void SetToNow();

	Timepoint GetTimepoint() const;

	int64_t ToUnixSeconds() const;
	int64_t ToUnixMilliseconds() const;

	std::string ToString(ETimeStringFormat format = ETimeStringFormat::Default) const;

private:
	Timepoint m_Time;
};
