#pragma once

#include "Felis/Logger/Formatters/ILogFormatter.h"

namespace Felis
{
// Formats a text line in the form
// <COLOR> [TIMESTAMP] [LEVEL] [PREFIX] MESSAGE <DEFAULT_COLOR>
// according to the flags enabled by both the log entry and this formatter
class LogFormatterText final : public ILogFormatter
{
public:
	explicit LogFormatterText(LogFlags flags = LogFlags{LogFlags::All});
	~LogFormatterText() = default;

	std::string Format(const LogEntry& log) const override;

private:
	void FormatTime(const DateTime& time, std::stringstream& stream) const;
	void FormatLevel(ELogLevel level, std::stringstream& stream) const;
	void FormatPrefix(const std::string& prefix, std::stringstream& stream) const;
	void FormatMessage(const std::string& message, std::stringstream& stream) const;
};
} // namespace Felis
