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

	bool Write(const LogEntry& log, const class ILogFormatter& formatter) override;
	bool Flush() override;
};
} // namespace Felis
