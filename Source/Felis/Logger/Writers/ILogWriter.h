#pragma once

#include "Felis/Logger/Log.h"

// Interface for all log writers
// Writers should be thread safe
class ILogWriter
{
public:
	ILogWriter()		  = default;
	virtual ~ILogWriter() = default;

	virtual void		Write(const LogEntry& log, const class ILogFormatter& formatter) = 0;
	virtual inline void Flush() {};
};

using ILogWriterPtr = std::unique_ptr<ILogWriter>;
