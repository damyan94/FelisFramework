#include "stdafx.h"

#include "Utils/Time/Duration.h"

std::ostream& operator<<(std::ostream& out, Duration dt)
{
	out << dt.m_Value;
	return out;
}

Duration::operator std::chrono::nanoseconds() const
{
	return m_Value;
}

Duration Duration::operator+(const Duration& rhs) const
{
	return Duration(m_Value + rhs.m_Value);
}

Duration Duration::operator-(const Duration& rhs) const
{
	return Duration(m_Value - rhs.m_Value);
}

Duration Duration::operator-() const
{
	return Duration(-m_Value);
}

Duration& Duration::operator+=(const Duration& rhs)
{
	m_Value += rhs.m_Value;
	return *this;
}

Duration& Duration::operator-=(const Duration& rhs)
{
	m_Value -= rhs.m_Value;
	return *this;
}

int64_t Duration::As(EUnitOfTime unit) const
{
	switch (unit)
	{
	case EUnitOfTime::Nanosecond: return As<Nanoseconds>().count();
	case EUnitOfTime::Microsecond: return As<Microseconds>().count();
	case EUnitOfTime::Millisecond: return As<Milliseconds>().count();
	case EUnitOfTime::Second: return As<Seconds>().count();
	case EUnitOfTime::Minute: return As<Minutes>().count();
	case EUnitOfTime::Hour: return As<Hours>().count();
	case EUnitOfTime::Day: return As<Days>().count();
	case EUnitOfTime::Week: return As<Weeks>().count();
	case EUnitOfTime::Month: return As<Months>().count();
	case EUnitOfTime::Year: return As<Years>().count();
	default: AssertReturnIf(true, 0);
	}
}
