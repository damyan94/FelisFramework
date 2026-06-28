#include "stdafx.h"

#include "Utils/Logger/Logger.h"

Logger g_Logger;

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
	return level <= m_Level;
}

LogFlags Logger::GetLogFlags() const
{
	return m_Flags;
}

void Logger::AddLogDestination(LogDestination&& destination)
{
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

#ifdef FELIS_RUN_TESTS

#include "Utils/Logger/Formatters/LogFormatterConsole.h"
#include "Utils/Logger/Writers/LogWriterConsole.h"

// ============================================================
// Helpers
// ============================================================

namespace
{

// Creates a fresh logger with a console destination and all flags on.
Logger MakeLogger(const std::string& prefix, ELogLevel level = ELogLevel::Debug)
{
	Logger logger;
	logger.SetLogPrefix(prefix);
	logger.SetLogLevel(level);
	logger.SetLogFlags(LogFlags::All);
	logger.AddLogDestination(
		{ELogDestinationType::Console, std::make_unique<LogWriterConsole>(), std::make_unique<LogFormatterConsole>()});
	return logger;
}

void PrintSection(Logger& logger, const std::string& title)
{
	logger.SetLogLevel(ELogLevel::Debug);
	logger.SetLogFlags(LogFlags::All);
	logger.Log(ELogLevel::Debug, "");
	logger.Log(ELogLevel::Debug, "========================================");
	logger.Log(ELogLevel::Debug, "  ", title);
	logger.Log(ELogLevel::Debug, "========================================");
}

void PrintSubSection(Logger& logger, const std::string& title)
{
	logger.Log(ELogLevel::Debug, "--- ", title, " ---");
}

// Log one message at every level.
void LogAllLevels(Logger& logger, const std::string& tag = "")
{
	const std::string s = tag.empty() ? "" : " [" + tag + "]";
	logger.Log(ELogLevel::Critical, "Critical", s);
	logger.Log(ELogLevel::Error, "Error", s);
	logger.Log(ELogLevel::Warning, "Warning", s);
	logger.Log(ELogLevel::Info, "Info", s);
	logger.Log(ELogLevel::Debug, "Debug", s);
}

} // anonymous namespace

// ============================================================
// Test cases
// ============================================================

namespace Test
{

void TestLogger()
{
	// --------------------------------------------------------
	// 1. LOG LEVEL FILTERING
	//    For each level L, only messages with level <= L should appear.
	//    Expected: Critical always visible; Debug only when level = Debug.
	// --------------------------------------------------------
	{
		Logger logger = MakeLogger("LevelTest");
		PrintSection(logger, "1. LOG LEVEL FILTERING");

		const ELogLevel levels[] = {
			ELogLevel::Critical,
			ELogLevel::Error,
			ELogLevel::Warning,
			ELogLevel::Info,
			ELogLevel::Debug,
		};
		const char* levelNames[] = {"Critical", "Error", "Warning", "Info", "Debug"};

		for (int i = 0; i < 5; ++i)
		{
			logger.SetLogLevel(ELogLevel::Debug); // ensure the annotation is visible
			logger.SetLogFlags(LogFlags::All);
			PrintSubSection(logger, std::string("Log level = ") + levelNames[i]);

			logger.SetLogLevel(levels[i]);
			LogAllLevels(logger);
		}
	}

	// --------------------------------------------------------
	// 2. FLAG TESTS — one flag disabled at a time
	//    Each sub-section removes exactly one flag; all other
	//    fields should still appear normally.
	// --------------------------------------------------------
	{
		Logger logger = MakeLogger("FlagTest");
		PrintSection(logger, "2. FLAG TESTS (one flag off at a time)");

		struct FlagCase
		{
			LogFlags::Type flag;
			const char*	   description;
		};

		const FlagCase cases[] = {
			{LogFlags::Color, "Color  OFF — no ANSI codes, plain text"},
			{LogFlags::Time, "Time   OFF — timestamp absent"},
			{LogFlags::Level, "Level  OFF — level tag absent"},
			{LogFlags::Prefix, "Prefix OFF — prefix tag absent"},
		};

		for (const auto& c : cases)
		{
			logger.SetLogLevel(ELogLevel::Debug);

			LogFlags flags;
			flags.Flags = LogFlags::All;
			flags.SetFlag(c.flag, false);
			logger.SetLogFlags(flags);

			PrintSubSection(logger, c.description);
			LogAllLevels(logger);
		}

		// All flags off
		{
			logger.SetLogLevel(ELogLevel::Debug);
			LogFlags none;
			none.ClearFlags();
			logger.SetLogFlags(none);
			PrintSubSection(logger, "ALL flags OFF — bare message only");
			LogAllLevels(logger);
		}

		// All flags on (restore)
		{
			logger.SetLogLevel(ELogLevel::Debug);
			logger.SetLogFlags(LogFlags::All);
			PrintSubSection(logger, "ALL flags ON — full output restored");
			LogAllLevels(logger);
		}
	}

	// --------------------------------------------------------
	// 3. LOG METHOD VARIANTS
	//    stream-style Log() with multiple args, LogFmt() with
	//    format strings, integer args, float args.
	// --------------------------------------------------------
	{
		Logger logger = MakeLogger("MethodTest");
		PrintSection(logger, "3. LOG METHOD VARIANTS");

		PrintSubSection(logger, "Stream — multiple heterogeneous args");
		logger.Log(ELogLevel::Info, "int=", 42, " float=", 3.14f, " str=", "hello");

		PrintSubSection(logger, "Stream — single string");
		logger.Log(ELogLevel::Info, "single string argument");

		PrintSubSection(logger, "LogFmt — positional");
		logger.LogFmt(ELogLevel::Info, "x={0} y={1} x={0}", 10, 20);

		PrintSubSection(logger, "LogFmt — integer types");
		logger.LogFmt(ELogLevel::Info, "int={} uint={} hex={:x} oct={:o}", -1, 255u, 255, 255);

		PrintSubSection(logger, "LogFmt — float formatting");
		logger.LogFmt(ELogLevel::Info, "default={} fixed={:.3f} sci={:e}", 3.14159, 3.14159, 3.14159);

		PrintSubSection(logger, "LogFmt — string types");
		logger.LogFmt(ELogLevel::Info, "str='{}' width={:>10}", "test", "right");

		PrintSubSection(logger, "LogFmt — boolean");
		logger.LogFmt(ELogLevel::Info, "true={} false={}", true, false);

		PrintSubSection(logger, "Stream — empty message");
		logger.Log(ELogLevel::Info, "");

		PrintSubSection(logger, "Stream — long message (300 chars)");
		logger.Log(ELogLevel::Info, std::string(300, 'x'));

		PrintSubSection(logger, "Stream — message with newlines inside");
		logger.Log(ELogLevel::Info, "line1\nline2\nline3");

		PrintSubSection(logger, "Stream — unicode");
		logger.Log(ELogLevel::Info, "Привет, свет! こんにちは");

		PrintSubSection(logger, "Stream — special chars");
		logger.Log(ELogLevel::Info, "tabs:\there  null-ish: end");
	}

	// --------------------------------------------------------
	// 4. PREFIX VARIATIONS
	//    Empty prefix, short, long, special characters.
	// --------------------------------------------------------
	{
		Logger logger = MakeLogger("PrefixTest");
		PrintSection(logger, "4. PREFIX VARIATIONS");

		const char* prefixes[] = {
			"",
			"A",
			"Renderer",
			"Very Long Prefix Name That Is Quite Verbose",
			"[Special/Chars]",
		};

		for (const auto* p : prefixes)
		{
			logger.SetLogPrefix(p);
			logger.LogFmt(ELogLevel::Info, "prefix='{}'", p);
		}

		logger.SetLogPrefix("PrefixTest"); // restore
	}

	// --------------------------------------------------------
	// 5. DESTINATION MANAGEMENT
	//    Add, remove, re-add destinations; verify the logger
	//    survives having no destinations (no crash).
	// --------------------------------------------------------
	{
		Logger logger = MakeLogger("DestTest");
		PrintSection(logger, "5. DESTINATION MANAGEMENT");

		PrintSubSection(logger, "Normal output — one console destination");
		logger.Log(ELogLevel::Info, "should appear");

		PrintSubSection(logger, "Remove console destination — nothing should print after this");
		logger.RemoveLogDestination(ELogDestinationType::Console);
		logger.Log(ELogLevel::Info, "should NOT appear");
		logger.Log(ELogLevel::Critical, "should NOT appear");

		PrintSubSection(logger, "Re-add console destination — output resumes");
		logger.AddLogDestination({ELogDestinationType::Console,
								  std::make_unique<LogWriterConsole>(),
								  std::make_unique<LogFormatterConsole>()});
		logger.Log(ELogLevel::Info, "should appear again");

		PrintSubSection(logger, "Add same destination type again — replaces, no duplicate");
		logger.AddLogDestination({ELogDestinationType::Console,
								  std::make_unique<LogWriterConsole>(),
								  std::make_unique<LogFormatterConsole>()});
		logger.Log(ELogLevel::Info, "one line, not two");
	}

	// --------------------------------------------------------
	// 6. LEVEL + FLAG INTERACTION
	//    Flags should not affect which messages are filtered by
	//    level — a suppressed level stays suppressed regardless
	//    of which flags are on.
	// --------------------------------------------------------
	{
		Logger logger = MakeLogger("InteractionTest");
		PrintSection(logger, "6. LEVEL + FLAG INTERACTION");

		logger.SetLogLevel(ELogLevel::Error); // only Critical + Error pass

		PrintSubSection(logger, "Level=Error, all flags on");
		logger.SetLogFlags(LogFlags::All);
		LogAllLevels(logger); // expect: Critical, Error only

		PrintSubSection(logger, "Level=Error, all flags off");
		LogFlags none;
		none.ClearFlags();
		logger.SetLogFlags(none);
		LogAllLevels(logger); // expect: Critical, Error only (bare, no decoration)

		/*PrintSubSection(logger, "Level=Off — nothing should print");
		logger.SetLogLevel(ELogLevel::Off);
		logger.SetLogFlags(LogFlags::All);
		LogAllLevels(logger);  // expect: silence*/
	}

	// --------------------------------------------------------
	// 7. FLUSH
	//    Just verifies Flush() doesn't crash with zero, one,
	//    and after-remove destinations.
	// --------------------------------------------------------
	{
		Logger logger = MakeLogger("FlushTest");
		PrintSection(logger, "7. FLUSH");

		PrintSubSection(logger, "Flush with one destination");
		logger.Log(ELogLevel::Info, "before flush");
		logger.Flush();
		logger.Log(ELogLevel::Info, "after flush");

		PrintSubSection(logger, "Flush with no destinations");
		logger.RemoveLogDestination(ELogDestinationType::Console);
		logger.Flush(); // must not crash

		logger.AddLogDestination({ELogDestinationType::Console,
								  std::make_unique<LogWriterConsole>(),
								  std::make_unique<LogFormatterConsole>()});
		logger.Log(ELogLevel::Info, "destination restored after flush test");
	}

	// --------------------------------------------------------
	// 8. CONCURRENCY
	//    4 threads logging simultaneously; each message must
	//    appear whole — no interleaved characters.
	//    Visual check: each line should be complete and parseable.
	// --------------------------------------------------------
	{
		Logger logger = MakeLogger("ThreadTest");
		PrintSection(logger, "8. CONCURRENCY (4 threads x 25 messages)");

		std::vector<std::thread> threads;
		threads.reserve(4);

		for (int t = 0; t < 4; ++t)
		{
			threads.emplace_back(
				[&logger, t]()
				{
					for (int i = 0; i < 25; ++i)
						logger.LogFmt(ELogLevel::Info, "thread={} msg={}", t, i);
				});
		}

		for (auto& th : threads)
			th.join();

		logger.Log(ELogLevel::Debug, "all threads joined");
	}

	// --------------------------------------------------------
	// 9. RAPID FIRE
	//    100 messages in a tight loop — stress test for any
	//    internal locking or buffering issues.
	// --------------------------------------------------------
	{
		Logger logger = MakeLogger("RapidTest");
		PrintSection(logger, "9. RAPID FIRE (100 messages)");

		const Time start;

		for (int i = 0; i < 100; ++i)
			logger.LogFmt(ELogLevel::Debug, "msg {}", i);

		const uint64_t ms = start.GetElapsed().As(EUnitOfTime::Millisecond);
		const uint64_t us = start.GetElapsed().As(EUnitOfTime::Microsecond);

		logger.LogFmt(ELogLevel::Info, "rapid fire done — 100 messages in {}ms ({}us)", ms, us);
	}

	// --------------------------------------------------------
	// 10. GLOBAL LOGGER MACROS
	//     Verify the convenience macros route through g_Logger.
	// --------------------------------------------------------
	{
		PrintSection(g_Logger, "10. GLOBAL LOGGER MACROS");

		const ELogLevel prev = g_Logger.GetLogLevel();
		g_Logger.SetLogLevel(ELogLevel::Debug);
		g_Logger.SetLogFlags(LogFlags::All);

		LogCritical("macro Critical");
		LogError("macro Error");
		LogWarning("macro Warning");
		LogInfo("macro Info");
		LogDebug("macro Debug");

		g_Logger.SetLogLevel(prev);
	}

	// --------------------------------------------------------
	// Done
	// --------------------------------------------------------
	{
		Logger logger = MakeLogger("TestLogger");
		PrintSection(logger, "ALL TESTS COMPLETE");
	}
}

} // namespace Test

#endif // FELIS_RUN_TESTS
