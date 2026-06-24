#pragma once

#include "Utils/Logger/Writers/ILogWriter.h"

// Writes a line formatted with a ILogFormatter to the console
class LogWriterConsole final : public ILogWriter
{
public:
	LogWriterConsole()		  = default;
	~LogWriterConsole() final = default;

	void Write(const LogEntry& log, const class ILogFormatter& formatter) final;
	void Flush() final;
};
