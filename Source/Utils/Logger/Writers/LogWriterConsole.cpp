#include "stdafx.h"

#include "Utils/Logger/Writers/LogWriterConsole.h"

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
