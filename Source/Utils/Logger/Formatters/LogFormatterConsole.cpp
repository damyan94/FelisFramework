#include "stdafx.h"

#include "Utils/Logger/Formatters/LogFormatterConsole.h"

#include "Utils/Logger/ConsoleFormat.h"

std::string LogFormatterConsole::Format(const LogEntry& log) const
{
	std::stringstream ss;

	const bool coloredOutput = log.Flags.IsFlagSet(LogFlags::Color);
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

	if (log.Flags.IsFlagSet(LogFlags::Time))
	{
		FormatTime(log.Time, ss);
	}

	if (log.Flags.IsFlagSet(LogFlags::Level))
	{
		FormatLevel(log.Level, ss);
	}

	if (log.Flags.IsFlagSet(LogFlags::Prefix))
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

void LogFormatterConsole::FormatTime(const DateTime& time, std::stringstream& stream) const
{
	stream << '[' << time.ToString(ETimeStringFormat::Default) << "] ";
}

void LogFormatterConsole::FormatLevel(ELogLevel level, std::stringstream& stream) const
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

void LogFormatterConsole::FormatPrefix(const std::string& prefix, std::stringstream& stream) const
{
	if (!prefix.empty())
	{
		stream << '[' << prefix << "] ";
	}
}

void LogFormatterConsole::FormatMessage(const std::string& message, std::stringstream& stream) const
{
	stream << message << '\n';
}
