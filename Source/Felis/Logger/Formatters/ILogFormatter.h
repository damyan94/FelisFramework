#pragma once

#include "Felis/Logger/Log.h"

namespace Felis
{
// Interface for all log formatters
class ILogFormatter
{
public:
	explicit ILogFormatter(LogFlags flags = LogFlags{LogFlags::All});
	virtual ~ILogFormatter() = default;

	virtual std::string Format(const LogEntry& log) const = 0;

	void	 SetLogFlags(LogFlags flags);
	LogFlags GetLogFlags() const;

protected:
	LogFlags GetEffectiveLogFlags(const LogEntry& log) const;

private:
	std::atomic<uint8_t> m_Flags;
};

using ILogFormatterPtr = std::unique_ptr<ILogFormatter>;
} // namespace Felis
