#include "Felis/stdafx.h"

#include "Felis/Logger/Logger.h"

#include "Felis/Logger/Formatters/LogFormatterConsole.h"
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
		{ELogDestinationType::Console, std::make_unique<LogWriterConsole>(), std::make_unique<LogFormatterConsole>()});

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

void Logger::Flush()
{
	for (auto& dest : m_Destinations)
	{
		dest.Writer->Flush();
	}
}

void Logger::Log(const LogEntry& log)
{
	for (auto& dest : m_Destinations)
	{
		dest.Writer->Write(log, *dest.Formatter);
	}
}

} // namespace Felis
