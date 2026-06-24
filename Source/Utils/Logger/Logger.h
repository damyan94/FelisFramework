#pragma once

#include "Utils/Logger/Log.h"

#include "Utils/Logger/Formatters/ILogFormatter.h"
#include "Utils/Logger/Writers/ILogWriter.h"

// A destination for logging stuff
// Has a Type so we can log only some stuff, a ILogWriter and a ILogFormatter
struct LogDestination
{
	ELogDestinationType Type;
	ILogWriterPtr		Writer;
	ILogFormatterPtr	Formatter;
};

using LogDestinations = std::vector<LogDestination>;

// A class that formats and writes logs to LogDestinations using their ILogFormatters and ILogWriters
class Logger
{
public:
	Logger()  = default;
	~Logger() = default;

	DISABLE_COPY(Logger);
	DEFAULT_MOVE(Logger);

	void SetLogPrefix(const std::string& prefix);
	void SetLogLevel(ELogLevel level);
	void SetLogFlags(uint8_t flags);
	void SetLogFlags(LogFlags flags);

	const std::string& GetLogPrefix() const;
	ELogLevel		   GetLogLevel() const;
	bool			   IsLogLevelEnabled(ELogLevel level) const;
	LogFlags		   GetLogFlags() const;

	void AddLogDestination(LogDestination&& destination);
	void RemoveLogDestination(ELogDestinationType type);

	void Flush();

	template <typename... Args>
	inline void Log(ELogLevel level, Args&&... args);

	template <typename... Args>
	inline void LogFmt(ELogLevel level, std::format_string<Args...> fmt, Args&&... args);

private:
	void Log(const LogEntry& log);

private:
	std::string		m_Prefix;
	ELogLevel		m_Level = ELogLevel::Warning;
	LogFlags		m_Flags = {LogFlags::Time | LogFlags::Level | LogFlags::Prefix};
	LogDestinations m_Destinations;
};

template <typename... Args>
inline void Logger::Log(ELogLevel level, Args&&... args)
{
	ReturnIf(!IsLogLevelEnabled(level));

	std::stringstream ss;
	(ss << ... << args);

	Log({m_Flags, DateTime::Now(), level, m_Prefix, ss.str()});
}

template <typename... Args>
inline void Logger::LogFmt(ELogLevel level, std::format_string<Args...> fmt, Args&&... args)
{
	ReturnIf(!IsLogLevelEnabled(level));

	Log({m_Flags, DateTime::Now(), level, m_Prefix, std::format(fmt, std::forward<Args>(args)...)});
}

// TODO Move to separate class, file, project
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
