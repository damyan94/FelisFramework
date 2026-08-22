#include "Felis/stdafx.h"

#include "Felis/Logger/Formatters/LogFormatterText.h"

#include "Felis/Logger/ConsoleFormat.h"

namespace Felis
{
LogFormatterText::LogFormatterText(LogFlags flags)
	: ILogFormatter(flags)
{
}

std::string LogFormatterText::Format(const LogEntry& log) const
{
	std::stringstream ss;
	const LogFlags	  flags = GetEffectiveLogFlags(log);

	const bool coloredOutput = flags.IsFlagSet(LogFlags::Color);
	if (coloredOutput)
	{
		switch (log.Level)
		{
		case ELogLevel::Critical: ss << ConsoleFormat().Set(EConsoleFormat::MagentaFg).Set(EConsoleFormat::Bold); break;
		case ELogLevel::Error: ss << ConsoleFormat().Set(EConsoleFormat::RedFg).Set(EConsoleFormat::Bold); break;
		case ELogLevel::Warning: ss << ConsoleFormat().Set(EConsoleFormat::YellowFg).Set(EConsoleFormat::Bold); break;
		case ELogLevel::Info: ss << ConsoleFormat().Set(EConsoleFormat::GrayFg).Set(EConsoleFormat::Bold); break;
		case ELogLevel::Debug: ss << ConsoleFormat().Set(EConsoleFormat::CyanFg).Set(EConsoleFormat::Bold); break;
		default: break;
		}
	}

	if (flags.IsFlagSet(LogFlags::Time))
	{
		FormatTime(log.Time, ss);
	}

	if (flags.IsFlagSet(LogFlags::Level))
	{
		FormatLevel(log.Level, ss);
	}

	if (flags.IsFlagSet(LogFlags::Prefix))
	{
		FormatPrefix(log.Prefix, ss);
	}

	FormatMessage(log.Message, ss);

	if (coloredOutput)
	{
		ss << ConsoleFormat().Set(EConsoleFormat::ResetAll);
	}

	return ss.str();
}

void LogFormatterText::FormatTime(const DateTime& time, std::stringstream& stream) const
{
	stream << '[' << time << "] ";
}

void LogFormatterText::FormatLevel(ELogLevel level, std::stringstream& stream) const
{
	std::string_view levelStringView;
	switch (level)
	{
	case ELogLevel::Critical: levelStringView = "CRT"; break;
	case ELogLevel::Error: levelStringView = "ERR"; break;
	case ELogLevel::Warning: levelStringView = "WRN"; break;
	case ELogLevel::Info: levelStringView = "INF"; break;
	case ELogLevel::Debug: levelStringView = "DBG"; break;
	default: break;
	}

	stream << '[' << levelStringView << "] ";
}

void LogFormatterText::FormatPrefix(const std::string& prefix, std::stringstream& stream) const
{
	if (!prefix.empty())
	{
		stream << '[' << prefix << "] ";
	}
}

void LogFormatterText::FormatMessage(const std::string& message, std::stringstream& stream) const
{
	stream << message << '\n';
}
} // namespace Felis
