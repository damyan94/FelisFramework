#pragma once

#include "Log.h"
#include "Utils/Logger/Formatters/ILogFormatter.h"
#include "Utils/Logger/Writers/ILogWriter.h"

struct LogDestination
{
	ELogDestinationType Type;
	ILogWriterPtr		Writer;
	ILogFormatterPtr	Formatter;
};

using LogDestinations = std::vector<LogDestination>;

class Logger
{
public:
	Logger()  = default;
	~Logger() = default;

	DISABLE_COPY(Logger);
	DEFAULT_MOVE(Logger);

	template <typename... Args>
	inline void Log(ELogLevel level, Args&&... args);

	template <typename... Args>
	inline void LogFmt(ELogLevel level, std::format_string<Args...> fmt, Args&&... args);

	void			   SetLogPrefix(const std::string& prefix);
	const std::string& GetLogPrefix() const;

	void	  SetLogLevel(ELogLevel level);
	ELogLevel GetLogLevel() const;
	bool	  IsLogLevelEnabled(ELogLevel level) const;

	void	 SetLogFlags(uint8_t flags);
	void	 SetLogFlags(LogFlags flags);
	LogFlags GetLogFlags() const;

	void AddLogDestination(LogDestination&& destination);
	void RemoveLogDestination(ELogDestinationType type);

	void Flush();

private:
	void Log(const LogEntry& log);

private:
	std::string		m_Prefix;
	LogFlags		m_Flags = {LogFlags::Time | LogFlags::Level | LogFlags::Prefix};
	ELogLevel		m_Level = ELogLevel::Warning;
	LogDestinations m_Destinations;
};

template <typename... Args>
inline void Logger::Log(ELogLevel level, Args&&... args)
{
	if (!IsLogLevelEnabled(level))
	{
		return;
	}

	std::stringstream ss;
	(ss << ... << args);

	Log({m_Flags, Time::Now(), level, m_Prefix, ss.str()});
}

template <typename... Args>
inline void Logger::LogFmt(ELogLevel level, std::format_string<Args...> fmt, Args&&... args)
{
	if (!IsLogLevelEnabled(level))
	{
		return;
	}

	Log({m_Flags, Time::Now(), level, m_Prefix, std::format(fmt, std::forward<Args>(args)...)});
}

//TODO Move to separate class, file, project
#ifdef FELIS_RUN_TESTS
namespace Test
{
extern void TestLogger();
}
#endif // FELIS_RUN_TESTS

extern Logger g_Logger;

#define FELIS_LOG_CONSOLE(Level, ...) g_Logger.Log(Level, __VA_ARGS__)

#define LogCritical(...) FELIS_LOG_CONSOLE(ELogLevel::Critical, __VA_ARGS__)
#define LogError(...) FELIS_LOG_CONSOLE(ELogLevel::Error, __VA_ARGS__)
#define LogWarning(...) FELIS_LOG_CONSOLE(ELogLevel::Warning, __VA_ARGS__)
#define LogInfo(...) FELIS_LOG_CONSOLE(ELogLevel::Info, __VA_ARGS__)
#define LogDebug(...) FELIS_LOG_CONSOLE(ELogLevel::Debug, __VA_ARGS__)
