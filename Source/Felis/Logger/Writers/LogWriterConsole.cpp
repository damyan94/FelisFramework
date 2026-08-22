#include "Felis/stdafx.h"

#include "Felis/Logger/Writers/LogWriterConsole.h"

namespace Felis
{

void LogWriterConsole::Write(const LogEntry& log, const class ILogFormatter& formatter)
{
	auto& out = log.Level > ELogLevel::Warning ? std::cout : std::cerr;
	out << formatter.Format(log);
}

void LogWriterConsole::Flush()
{
	std::flush(std::cout);
	std::flush(std::cerr);
}

} // namespace Felis
