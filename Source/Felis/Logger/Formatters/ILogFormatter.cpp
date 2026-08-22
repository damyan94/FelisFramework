#include "Felis/stdafx.h"

#include "Felis/Logger/Formatters/ILogFormatter.h"

namespace Felis
{
ILogFormatter::ILogFormatter(LogFlags flags)
	: m_Flags(flags.Flags)
{
}

void ILogFormatter::SetLogFlags(LogFlags flags)
{
	m_Flags.store(flags.Flags, std::memory_order_relaxed);
}

LogFlags ILogFormatter::GetLogFlags() const
{
	return {m_Flags.load(std::memory_order_relaxed)};
}

LogFlags ILogFormatter::GetEffectiveLogFlags(const LogEntry& log) const
{
	return log.Flags & GetLogFlags();
}
} // namespace Felis
