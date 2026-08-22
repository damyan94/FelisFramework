#pragma once

#include "Felis/Logger/Log.h"

#include "Felis/Logger/Formatters/ILogFormatter.h"
#include "Felis/Logger/Writers/ILogWriter.h"

namespace Felis
{
// A destination for logging stuff
// Has a Type so we can log only some stuff, an ILogWriter and an ILogFormatter
struct LogDestination
{
	ELogDestinationType Type;

	// TODO maybe store these centrally in a registry
	ILogWriterPtr	 Writer;
	ILogFormatterPtr Formatter;
};

using LogDestinations = std::vector<LogDestination>;

// Formats and writes logs to LogDestinations using their ILogFormatters and ILogWriters
class Logger
{
public:
	Logger()  = default;
	~Logger() = default;

	FELIS_DISABLE_COPY(Logger);
	FELIS_DEFAULT_MOVE(Logger);

	static Logger& GetGlobalLogger();

	void SetEnabled(bool enabled);
	void SetLogPrefix(const std::string& prefix);
	void SetLogLevel(ELogLevel level);
	void SetLogFlags(uint8_t flags);
	void SetLogFlags(LogFlags flags);

	bool			   IsEnabled() const;
	const std::string& GetLogPrefix() const;
	ELogLevel		   GetLogLevel() const;
	bool			   IsLogLevelEnabled(ELogLevel level) const;
	LogFlags		   GetLogFlags() const;

	void					AddLogDestination(LogDestination&& destination);
	void					RemoveLogDestination(ELogDestinationType type);
	bool					SetLogDestinationFlags(ELogDestinationType type, LogFlags flags);
	std::optional<LogFlags> GetLogDestinationFlags(ELogDestinationType type) const;

	bool Flush();

	template <typename... Args>
	inline bool Log(ELogLevel level, Args&&... args);

	template <typename... Args>
	inline bool LogFmt(ELogLevel level, std::format_string<Args...> fmt, Args&&... args);

private:
	bool Log(const LogEntry& log);

private:
	bool			m_IsEnabled = true;
	std::string		m_Prefix;
	ELogLevel		m_Level = ELogLevel::Warning;
	LogFlags		m_Flags = {LogFlags::Time | LogFlags::Level | LogFlags::Prefix};
	LogDestinations m_Destinations;
};

template <typename... Args>
inline bool Logger::Log(ELogLevel level, Args&&... args)
{
	ReturnIf(!IsLogLevelEnabled(level), true);

	std::stringstream ss;
	(ss << ... << args);

	return Log({m_Flags, DateTime::Now(), level, m_Prefix, ss.str()});
}

template <typename... Args>
inline bool Logger::LogFmt(ELogLevel level, std::format_string<Args...> fmt, Args&&... args)
{
	ReturnIf(!IsLogLevelEnabled(level), true);

	return Log({m_Flags, DateTime::Now(), level, m_Prefix, std::format(fmt, std::forward<Args>(args)...)});
}
} // namespace Felis

#define FELIS_LOG_GLOBAL(Level, ...) ::Felis::Logger::GetGlobalLogger().Log(Level, __VA_ARGS__)
#define FELIS_LOG_CONSOLE(Level, ...) FELIS_LOG_GLOBAL(Level, __VA_ARGS__)

#define LogCritical(...) FELIS_LOG_GLOBAL(::Felis::ELogLevel::Critical, __VA_ARGS__)
#define LogError(...) FELIS_LOG_GLOBAL(::Felis::ELogLevel::Error, __VA_ARGS__)
#define LogWarning(...) FELIS_LOG_GLOBAL(::Felis::ELogLevel::Warning, __VA_ARGS__)
#define LogInfo(...) FELIS_LOG_GLOBAL(::Felis::ELogLevel::Info, __VA_ARGS__)
#define LogDebug(...) FELIS_LOG_GLOBAL(::Felis::ELogLevel::Debug, __VA_ARGS__)
