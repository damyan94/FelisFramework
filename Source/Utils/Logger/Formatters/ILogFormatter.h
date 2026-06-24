#pragma once

#include "Utils/Logger/Log.h"

// Interface for all log formatters
class ILogFormatter
{
public:
	ILogFormatter()			 = default;
	virtual ~ILogFormatter() = default;

	virtual std::string Format(const LogEntry& log) const = 0;
};

using ILogFormatterPtr = std::unique_ptr<ILogFormatter>;
