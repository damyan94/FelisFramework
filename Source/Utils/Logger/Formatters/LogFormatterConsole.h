#pragma once

#include "ILogFormatter.h"

class LogFormatterConsole final : public ILogFormatter
{
public:
	LogFormatterConsole()		 = default;
	~LogFormatterConsole() final = default;

	std::string Format(const LogEntry& log) const final;

protected:
	void FormatTime(const Time& time, std::stringstream& stream) const;
	void FormatLevel(ELogLevel level, std::stringstream& stream) const;
	void FormatPrefix(const std::string& prefix, std::stringstream& stream) const;
	void FormatMessage(const std::string& message, std::stringstream& stream) const;
};
