#pragma once

#include "ILogWriter.h"

class LogWriterConsole final : public ILogWriter
{
public:
	LogWriterConsole()		  = default;
	~LogWriterConsole() final = default;

	void Write(const LogEntry& log, const class ILogFormatter& formatter) final;
	void Flush() final;
};
