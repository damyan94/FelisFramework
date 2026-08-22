#include "Felis/stdafx.h"

#include "Felis/Logger/Logger.h"

#include "Felis/Logger/Formatters/LogFormatterText.h"
#include "Felis/Logger/Writers/LogWriterConsole.h"

namespace Felis
{
static Logger CreateGlobalLogger()
{
	Logger logger;
	logger.SetLogPrefix("");
	logger.SetLogLevel(ELogLevel::Debug);
	logger.SetLogFlags(LogFlags::All);
	logger.AddLogDestination(
		{ELogDestinationType::Console, std::make_unique<LogWriterConsole>(), std::make_unique<LogFormatterText>()});

	return logger;
}

Logger& Logger::GetGlobalLogger()
{
	static Logger gLogger{CreateGlobalLogger()};

	return gLogger;
}

void Logger::SetEnabled(bool enabled)
{
	m_IsEnabled = enabled;
}

void Logger::SetLogPrefix(const std::string& prefix)
{
	m_Prefix = prefix;
}

void Logger::SetLogLevel(ELogLevel level)
{
	m_Level = level;
}

void Logger::SetLogFlags(uint8_t flags)
{
	m_Flags.Flags = flags;
}

void Logger::SetLogFlags(LogFlags flags)
{
	m_Flags = flags;
}

bool Logger::IsEnabled() const
{
	return m_IsEnabled;
}

const std::string& Logger::GetLogPrefix() const
{
	return m_Prefix;
}

ELogLevel Logger::GetLogLevel() const
{
	return m_Level;
}

bool Logger::IsLogLevelEnabled(ELogLevel level) const
{
	return m_IsEnabled && level <= m_Level;
}

LogFlags Logger::GetLogFlags() const
{
	return m_Flags;
}

void Logger::AddLogDestination(LogDestination&& destination)
{
	ReturnIf(!destination.Formatter || !destination.Writer);

	RemoveLogDestination(destination.Type);
	m_Destinations.emplace_back(std::move(destination));
}

void Logger::RemoveLogDestination(ELogDestinationType type)
{
	std::erase_if(m_Destinations, [type](const LogDestination& destination) { return destination.Type == type; });
}

bool Logger::SetLogDestinationFlags(ELogDestinationType type, LogFlags flags)
{
	for (auto& destination : m_Destinations)
	{
		if (destination.Type == type)
		{
			destination.Formatter->SetLogFlags(flags);
			return true;
		}
	}

	return false;
}

std::optional<LogFlags> Logger::GetLogDestinationFlags(ELogDestinationType type) const
{
	for (const auto& destination : m_Destinations)
	{
		if (destination.Type == type)
		{
			return destination.Formatter->GetLogFlags();
		}
	}

	return std::nullopt;
}

bool Logger::Flush()
{
	bool success = true;
	for (auto& destination : m_Destinations)
	{
		if (!destination.Writer->Flush())
		{
			success = false;
		}
	}

	return success;
}

bool Logger::Log(const LogEntry& log)
{
	bool success = true;
	for (auto& destination : m_Destinations)
	{
		if (!destination.Writer->Write(log, *destination.Formatter))
		{
			success = false;
		}
	}

	return success;
}
} // namespace Felis
