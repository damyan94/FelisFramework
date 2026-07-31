#pragma once

#include "Utils/Logger/Writers/ILogWriter.h"

// Writes a line formatted with an ILogFormatter to the console
class LogWriterConsole final : public ILogWriter
{
public:
	LogWriterConsole()	= default;
	~LogWriterConsole() = default;

	void Write(const LogEntry& log, const class ILogFormatter& formatter);
	void Flush();
};
