#pragma once

#include "Utils/Logger/Formatters/ILogFormatter.h"

// Formats a line to be printed on the console in the format
// <COLOR> [TIMESTAMP] [LEVEL] [PREFIX] MESSAGE <DEFAULT_COLOR>
class LogFormatterConsole final : public ILogFormatter
{
public:
	LogFormatterConsole()		 = default;
	~LogFormatterConsole() final = default;

	std::string Format(const LogEntry& log) const final;

protected:
	void FormatTime(const DateTime& time, std::stringstream& stream) const;
	void FormatLevel(ELogLevel level, std::stringstream& stream) const;
	void FormatPrefix(const std::string& prefix, std::stringstream& stream) const;
	void FormatMessage(const std::string& message, std::stringstream& stream) const;
};
