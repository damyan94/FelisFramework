#pragma once

#include "Felis/Logger/Writers/ILogWriter.h"

namespace Felis
{
// Writes a line formatted with an ILogFormatter to the console
class LogWriterConsole final : public ILogWriter
{
public:
	LogWriterConsole()	= default;
	~LogWriterConsole() = default;

	void Write(const LogEntry& log, const class ILogFormatter& formatter);
	void Flush();
};
} // namespace Felis
