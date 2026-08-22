#pragma once

#include "Felis/Logger/Log.h"

namespace Felis
{
// Interface for all log writers
// Writers should be thread safe
class ILogWriter
{
public:
	ILogWriter()		  = default;
	virtual ~ILogWriter() = default;

	virtual bool Write(const LogEntry& log, const class ILogFormatter& formatter) = 0;
	virtual bool Flush();
};

using ILogWriterPtr = std::unique_ptr<ILogWriter>;
} // namespace Felis
