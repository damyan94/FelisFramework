#include "Felis/stdafx.h"

#include "Felis/Logger/Writers/LogWriterConsole.h"

namespace Felis
{
bool LogWriterConsole::Write(const LogEntry& log, const class ILogFormatter& formatter)
{
	auto& out = log.Level > ELogLevel::Warning ? std::cout : std::cerr;
	out << formatter.Format(log);
	return static_cast<bool>(out);
}

bool LogWriterConsole::Flush()
{
	std::flush(std::cout);
	std::flush(std::cerr);
	return static_cast<bool>(std::cout) && static_cast<bool>(std::cerr);
}
} // namespace Felis
