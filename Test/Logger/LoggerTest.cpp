#include "stdafx.h"

#include "Logger/LoggerTest.h"

#include "TestReporter.h"

#include "Logger/Logger.h"

#include "Logger/Formatters/LogFormatterConsole.h"
#include "Logger/Writers/LogWriterConsole.h"

#include <regex>

namespace
{

// Captures every LogEntry written to it via a real ILogFormatter, so
// integration tests can assert on captured LogEntry fields and/or the
// real formatted line without printing anywhere. Thread-safe.
class LogWriterMemory final : public ILogWriter
{
public:
	void Write(const LogEntry& log, const ILogFormatter& formatter) override
	{
		std::lock_guard<std::mutex> lock(m_Mutex);
		m_Entries.push_back(log);
		m_FormattedLines.push_back(formatter.Format(log));
	}

	void Flush() override
	{
		std::lock_guard<std::mutex> lock(m_Mutex);
		++m_FlushCount;
	}

	const std::vector<LogEntry>& GetEntries() const
	{
		return m_Entries;
	}

	const std::vector<std::string>& GetFormattedLines() const
	{
		return m_FormattedLines;
	}

	int GetFlushCount() const
	{
		return m_FlushCount;
	}

private:
	mutable std::mutex		 m_Mutex;
	std::vector<LogEntry>	 m_Entries;
	std::vector<std::string> m_FormattedLines;
	int						 m_FlushCount = 0;
};

// Builds a Logger wired to a fresh LogWriterMemory + the real
// LogFormatterConsole, and hands back a non-owning pointer to the
// writer so tests can inspect what was captured.
Logger MakeMemoryLogger(LogWriterMemory*& outWriter, const std::string& prefix = "", ELogLevel level = ELogLevel::Debug)
{
	Logger logger;
	logger.SetLogPrefix(prefix);
	logger.SetLogLevel(level);
	logger.SetLogFlags(LogFlags::All);

	auto writer = std::make_unique<LogWriterMemory>();
	outWriter	= writer.get();

	logger.AddLogDestination(
		{ELogDestinationType::Console, std::move(writer), std::make_unique<LogFormatterConsole>()});

	return logger;
}

// Logs one message at every level, in Critical -> Debug order.
void LogAllLevels(Logger& logger, const std::string& tag = "")
{
	const std::string s = tag.empty() ? "" : " [" + tag + "]";
	logger.Log(ELogLevel::Critical, "Critical", s);
	logger.Log(ELogLevel::Error, "Error", s);
	logger.Log(ELogLevel::Warning, "Warning", s);
	logger.Log(ELogLevel::Info, "Info", s);
	logger.Log(ELogLevel::Debug, "Debug", s);
}

// Redirects std::cout/std::cerr to in-memory buffers for the duration
// of its scope, so LogWriterConsole's actual stream output can be
// asserted on instead of just eyeballed. Restores the real buffers on
// destruction, even if a check fails mid-scope.
class StreamCapture
{
public:
	StreamCapture()
		: m_OldCoutBuf(std::cout.rdbuf(m_CoutCapture.rdbuf())),
		  m_OldCerrBuf(std::cerr.rdbuf(m_CerrCapture.rdbuf()))
	{
	}

	~StreamCapture()
	{
		std::cout.rdbuf(m_OldCoutBuf);
		std::cerr.rdbuf(m_OldCerrBuf);
	}

	DISABLE_COPY(StreamCapture);
	DISABLE_MOVE(StreamCapture);

	std::string GetCout() const
	{
		return m_CoutCapture.str();
	}

	std::string GetCerr() const
	{
		return m_CerrCapture.str();
	}

private:
	std::ostringstream m_CoutCapture;
	std::ostringstream m_CerrCapture;
	std::streambuf*	   m_OldCoutBuf;
	std::streambuf*	   m_OldCerrBuf;
};

} // namespace

namespace Test
{
namespace
{

// LogFormatterConsole - pure unit tests, no Logger/writer involved.
// Each case isolates a single flag to lock down the exact contract
// of "<COLOR> [TIME] [LEVEL] [PREFIX] MESSAGE <RESET>".

void TestFormatter_ExactColorCodePerLevel(TestReporter& r)
{
	struct LevelColor
	{
		ELogLevel	level;
		const char* code; // matches the Set() calls in LogFormatterConsole::Format
	};

	const LevelColor cases[] = {
		{ELogLevel::Critical, "35;1"}, // MagentaFg + Bold
		{ELogLevel::Error, "31;1"},	   // RedFg + Bold
		{ELogLevel::Warning, "33;1"},  // YellowFg + Bold
		{ELogLevel::Info, "37;1"},	   // GrayFg + Bold
		{ELogLevel::Debug, "36;1"},	   // CyanFg + Bold
	};

	LogFormatterConsole formatter;

	for (const auto& c : cases)
	{
		LogFlags flags;
		flags.Flags = LogFlags::Color; // isolate color only

		const LogEntry	  entry{flags, DateTime::Now(), c.level, "", "msg"};
		const std::string result   = formatter.Format(entry);
		const std::string expected = std::string("\033[") + c.code + "m" + "msg\n" + "\033[0m";

		TEST_CHECK(r, result == expected);
	}
}

void TestFormatter_ColorOffProducesNoEscapeCodes(TestReporter& r)
{
	LogFormatterConsole formatter;

	LogFlags flags;
	flags.Flags = LogFlags::All;
	flags.SetFlag(LogFlags::Color, false);

	const LogEntry	  entry{flags, DateTime::Now(), ELogLevel::Critical, "Pfx", "msg"};
	const std::string result = formatter.Format(entry);

	TEST_CHECK(r, result.find("\033[") == std::string::npos);
}

void TestFormatter_TimeOnlyMatchesExpectedShape(TestReporter& r)
{
	LogFormatterConsole formatter;

	LogFlags flags;
	flags.Flags = LogFlags::Time;

	const LogEntry	  entry{flags, DateTime::Now(), ELogLevel::Info, "", "msg"};
	const std::string result = formatter.Format(entry);

	// dd.mm.yyyy HH:MM:SS, matching TimeFormat::ETimeStringFormat::Default
	static const std::regex pattern(R"(^\[\d{2}\.\d{2}\.\d{4} \d{2}:\d{2}:\d{2}\.\d{6}\] msg\n$)");
	TEST_CHECK(r, std::regex_match(result, pattern));
}

void TestFormatter_LevelOnlyProducesExpectedTag(TestReporter& r)
{
	LogFormatterConsole formatter;

	struct LevelTag
	{
		ELogLevel	level;
		const char* tag;
	};

	const LevelTag cases[] = {
		{ELogLevel::Critical, "CRT"},
		{ELogLevel::Error, "ERR"},
		{ELogLevel::Warning, "WRN"},
		{ELogLevel::Info, "INF"},
		{ELogLevel::Debug, "DBG"},
	};

	LogFlags flags;
	flags.Flags = LogFlags::Level;

	for (const auto& c : cases)
	{
		const LogEntry	  entry{flags, DateTime::Now(), c.level, "", "msg"};
		const std::string result   = formatter.Format(entry);
		const std::string expected = std::string("[") + c.tag + "] msg\n";

		TEST_CHECK(r, result == expected);
	}
}

void TestFormatter_PrefixOnlyIncludedWhenNonEmpty(TestReporter& r)
{
	LogFormatterConsole formatter;

	LogFlags flags;
	flags.Flags = LogFlags::Prefix;

	const LogEntry withPrefix{flags, DateTime::Now(), ELogLevel::Info, "MyPrefix", "msg"};
	TEST_CHECK(r, formatter.Format(withPrefix) == "[MyPrefix] msg\n");

	const LogEntry emptyPrefix{flags, DateTime::Now(), ELogLevel::Info, "", "msg"};
	TEST_CHECK(r, formatter.Format(emptyPrefix) == "msg\n");
}

void TestFormatter_NoFlagsProducesBareMessage(TestReporter& r)
{
	LogFormatterConsole formatter;

	LogFlags flags;
	flags.ClearFlags();

	const LogEntry entry{flags, DateTime::Now(), ELogLevel::Critical, "Pfx", "bare"};
	TEST_CHECK(r, formatter.Format(entry) == "bare\n");
}

void TestFormatter_AllFlagsComposeInOrder(TestReporter& r)
{
	LogFormatterConsole formatter;

	LogFlags flags;
	flags.Flags = LogFlags::All;

	const LogEntry	  entry{flags, DateTime::Now(), ELogLevel::Error, "Net", "connection lost"};
	const std::string result = formatter.Format(entry);

	// Order per LogFormatterConsole::Format: color, time, level, prefix, message, then reset.
	static const std::regex pattern(
		R"(^\x1B\[31;1m\[\d{2}\.\d{2}\.\d{4} \d{2}:\d{2}:\d{2}\.\d{6}\] \[ERR\] \[Net\] connection lost\n\x1B\[0m$)");
	TEST_CHECK(r, std::regex_match(result, pattern));
}

// LogWriterConsole - unit tests via stream capture.

void TestWriter_RoutesAboveWarningToCoutAtOrBelowToCerr(TestReporter& r)
{
	LogWriterConsole	writer;
	LogFormatterConsole formatter;

	LogFlags flags;
	flags.Flags = LogFlags::All;

	struct Case
	{
		ELogLevel	level;
		bool		expectCout; // true -> cout, false -> cerr
		const char* message;
	};

	const Case cases[] = {
		{ELogLevel::Critical, false, "crit-msg"},
		{ELogLevel::Error, false, "error-msg"},
		{ELogLevel::Warning, false, "warn-msg"},
		{ELogLevel::Info, true, "info-msg"},
		{ELogLevel::Debug, true, "debug-msg"},
	};

	for (const auto& c : cases)
	{
		StreamCapture capture;

		const LogEntry entry{flags, DateTime::Now(), c.level, "", c.message};
		writer.Write(entry, formatter);

		if (c.expectCout)
		{
			TEST_CHECK(r, capture.GetCout().find(c.message) != std::string::npos);
			TEST_CHECK(r, capture.GetCerr().find(c.message) == std::string::npos);
		}
		else
		{
			TEST_CHECK(r, capture.GetCerr().find(c.message) != std::string::npos);
			TEST_CHECK(r, capture.GetCout().find(c.message) == std::string::npos);
		}
	}
}

void TestWriter_FlushDoesNotCrashOrWriteAnything(TestReporter& r)
{
	LogWriterConsole writer;
	StreamCapture	 capture;

	writer.Flush();

	TEST_CHECK(r, capture.GetCout().empty());
	TEST_CHECK(r, capture.GetCerr().empty());
}

// Logger - integration tests, real formatter, memory-captured writer.

// Only messages with level <= the configured level should be captured.
// Levels are Critical..Debug in increasing verbosity (per IsLogLevelEnabled),
// so setting the level to array index i should let exactly i+1 messages through.
void TestLogLevelFiltering(TestReporter& r)
{
	const ELogLevel levels[] = {
		ELogLevel::Critical,
		ELogLevel::Error,
		ELogLevel::Warning,
		ELogLevel::Info,
		ELogLevel::Debug,
	};

	for (int i = 0; i < 5; ++i)
	{
		LogWriterMemory* writer = nullptr;
		Logger			 logger = MakeMemoryLogger(writer, "LevelTest", levels[i]);

		LogAllLevels(logger);

		TEST_CHECK(r, (int)writer->GetEntries().size() == i + 1);

		if (!writer->GetEntries().empty())
		{
			TEST_CHECK(r, writer->GetEntries().front().Level == ELogLevel::Critical);
		}

		for (const auto& entry : writer->GetEntries())
		{
			TEST_CHECK(r, logger.IsLogLevelEnabled(entry.Level));
		}
	}
}

// Level filtering must be unaffected by which flags are set.
void TestLevelFlagInteraction(TestReporter& r)
{
	LogWriterMemory* writer = nullptr;
	Logger			 logger = MakeMemoryLogger(writer, "InteractionTest", ELogLevel::Error);

	logger.SetLogFlags(LogFlags::All);
	LogAllLevels(logger);
	TEST_CHECK(r, writer->GetEntries().size() == 2); // Critical, Error only

	LogFlags none;
	none.ClearFlags();
	logger.SetLogFlags(none);
	LogAllLevels(logger);
	TEST_CHECK(r, writer->GetEntries().size() == 4); // +2 more, still nothing below Error
}

// Stream-style Log() and format-string LogFmt() should produce the
// expected message content across a range of argument types.
void TestLogMethodVariants(TestReporter& r)
{
	LogWriterMemory* writer = nullptr;
	Logger			 logger = MakeMemoryLogger(writer, "MethodTest");

	logger.Log(ELogLevel::Info, "int=", 42, " str=", "hello");
	TEST_CHECK(r, writer->GetEntries().back().Message == "int=42 str=hello");

	logger.Log(ELogLevel::Info, "single string argument");
	TEST_CHECK(r, writer->GetEntries().back().Message == "single string argument");

	logger.LogFmt(ELogLevel::Info, "x={0} y={1} x={0}", 10, 20);
	TEST_CHECK(r, writer->GetEntries().back().Message == "x=10 y=20 x=10");

	logger.LogFmt(ELogLevel::Info, "int={} uint={} hex={:x} oct={:o}", -1, 255u, 255, 255);
	TEST_CHECK(r, writer->GetEntries().back().Message == "int=-1 uint=255 hex=ff oct=377");

	logger.LogFmt(ELogLevel::Info, "fixed={:.3f}", 3.14159);
	TEST_CHECK(r, writer->GetEntries().back().Message == "fixed=3.142");

	logger.LogFmt(ELogLevel::Info, "str='{}' width={:>10}", "test", "right");
	TEST_CHECK(r, writer->GetEntries().back().Message == "str='test' width=     right");

	logger.LogFmt(ELogLevel::Info, "true={} false={}", true, false);
	TEST_CHECK(r, writer->GetEntries().back().Message == "true=true false=false");

	logger.Log(ELogLevel::Info, "");
	TEST_CHECK(r, writer->GetEntries().back().Message.empty());

	std::string longMsg(300, 'x');
	logger.Log(ELogLevel::Info, longMsg);
	TEST_CHECK(r, writer->GetEntries().back().Message.size() == 300);
	TEST_CHECK(r, writer->GetEntries().back().Message == longMsg);

	logger.Log(ELogLevel::Info, "line1\nline2\nline3");
	TEST_CHECK(r, writer->GetEntries().back().Message == "line1\nline2\nline3");

	const std::string unicode = "Привет, свет! こんにちは";
	logger.Log(ELogLevel::Info, unicode);
	TEST_CHECK(r, writer->GetEntries().back().Message == unicode);
}

// GetLogPrefix() reflects whatever was last set, and end-to-end through
// the real formatter, the prefix bracket appears iff non-empty.
void TestPrefixVariations(TestReporter& r)
{
	LogWriterMemory* writer = nullptr;
	Logger			 logger = MakeMemoryLogger(writer);

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
		TEST_CHECK(r, logger.GetLogPrefix() == p);

		logger.Log(ELogLevel::Info, "msg");
		const std::string& line = writer->GetFormattedLines().back();

		if (std::string(p).empty())
		{
			TEST_CHECK(r, line.find("] msg\n") != std::string::npos); // no prefix bracket before message
		}
		else
		{
			TEST_CHECK(r, line.find(std::string("[") + p + "] msg\n") != std::string::npos);
		}
	}
}

// Adding, removing, and re-adding destinations should route writes
// correctly, and adding the same destination type twice should
// replace rather than duplicate.
void TestDestinationManagement(TestReporter& r)
{
	Logger logger;
	logger.SetLogPrefix("DestTest");
	logger.SetLogLevel(ELogLevel::Debug);
	logger.SetLogFlags(LogFlags::All);

	auto			 writerAOwned = std::make_unique<LogWriterMemory>();
	LogWriterMemory* writerA	  = writerAOwned.get();
	logger.AddLogDestination(
		{ELogDestinationType::Console, std::move(writerAOwned), std::make_unique<LogFormatterConsole>()});

	logger.Log(ELogLevel::Info, "msg1");
	TEST_CHECK(r, writerA->GetEntries().size() == 1);

	logger.RemoveLogDestination(ELogDestinationType::Console);
	logger.Log(ELogLevel::Info, "msg2 - should not be captured anywhere");
	TEST_CHECK(r, writerA->GetEntries().size() == 1); // unchanged, no longer attached

	auto			 writerBOwned = std::make_unique<LogWriterMemory>();
	LogWriterMemory* writerB	  = writerBOwned.get();
	logger.AddLogDestination(
		{ELogDestinationType::Console, std::move(writerBOwned), std::make_unique<LogFormatterConsole>()});

	logger.Log(ELogLevel::Info, "msg3");
	TEST_CHECK(r, writerB->GetEntries().size() == 1);
	TEST_CHECK(r, writerA->GetEntries().size() == 1); // still untouched

	// adding another destination of the same type should replace writerB, not add a second one
	auto			 writerCOwned = std::make_unique<LogWriterMemory>();
	LogWriterMemory* writerC	  = writerCOwned.get();
	logger.AddLogDestination(
		{ELogDestinationType::Console, std::move(writerCOwned), std::make_unique<LogFormatterConsole>()});

	logger.Log(ELogLevel::Info, "msg4");
	TEST_CHECK(r, writerC->GetEntries().size() == 1);
	TEST_CHECK(r, writerB->GetEntries().size() == 1); // did not receive msg4 - proves replacement, not duplication
}

// Flush() should reach every attached writer's Flush(), and must not
// crash with zero destinations attached.
void TestFlush(TestReporter& r)
{
	LogWriterMemory* writer = nullptr;
	Logger			 logger = MakeMemoryLogger(writer, "FlushTest");

	logger.Flush();
	TEST_CHECK(r, writer->GetFlushCount() == 1);

	logger.Flush();
	TEST_CHECK(r, writer->GetFlushCount() == 2);

	logger.RemoveLogDestination(ELogDestinationType::Console);
	logger.Flush();								 // must not crash with no destinations
	TEST_CHECK(r, writer->GetFlushCount() == 2); // unchanged, no longer attached
}

// 4 threads logging concurrently: every message must be captured
// exactly once, with none lost or corrupted.
void TestConcurrency(TestReporter& r)
{
	LogWriterMemory* writer = nullptr;
	Logger			 logger = MakeMemoryLogger(writer, "ThreadTest");

	constexpr int threadCount		= 4;
	constexpr int messagesPerThread = 25;

	std::vector<std::thread> threads;
	threads.reserve(threadCount);

	for (int t = 0; t < threadCount; ++t)
	{
		threads.emplace_back(
			[&logger, t]()
			{
				for (int i = 0; i < messagesPerThread; ++i)
				{
					logger.LogFmt(ELogLevel::Info, "thread={} msg={}", t, i);
				}
			});
	}

	for (auto& th : threads)
	{
		th.join();
	}

	TEST_CHECK(r, (int)writer->GetEntries().size() == threadCount * messagesPerThread);

	std::map<std::string, int> counts;
	for (const auto& entry : writer->GetEntries())
	{
		++counts[entry.Message];
	}

	bool allExactlyOnce = true;
	for (int t = 0; t < threadCount; ++t)
	{
		for (int i = 0; i < messagesPerThread; ++i)
		{
			const std::string expected = std::format("thread={} msg={}", t, i);
			if (counts[expected] != 1)
			{
				allExactlyOnce = false;
			}
		}
	}
	TEST_CHECK(r, allExactlyOnce);
}

// 100 messages logged in a tight loop, single-threaded: all captured,
// in order, with correct content.
void TestRapidFire(TestReporter& r)
{
	LogWriterMemory* writer = nullptr;
	Logger			 logger = MakeMemoryLogger(writer, "RapidTest");

	for (int i = 0; i < 100; ++i)
	{
		logger.Log(ELogLevel::Debug, "msg ", i);
	}

	TEST_CHECK(r, writer->GetEntries().size() == 100);

	bool inOrder = true;
	for (int i = 0; i < 100; ++i)
	{
		if (writer->GetEntries()[i].Message != "msg " + std::to_string(i))
		{
			inOrder = false;
			break;
		}
	}
	TEST_CHECK(r, inOrder);
}

// LogCritical/LogError/LogWarning/LogInfo/LogDebug macros should route
// through Logger::GetGlobalLogger(). Temporarily swaps its console
// destination for a memory one, then restores the original console
// destination exactly as CreateGlobalLogger() set it up.
void TestGlobalLoggerMacros(TestReporter& r)
{
	auto& gLogger = Logger::GetGlobalLogger();

	const ELogLevel prevLevel = gLogger.GetLogLevel();
	const LogFlags	prevFlags = gLogger.GetLogFlags();

	gLogger.SetLogLevel(ELogLevel::Debug);
	gLogger.SetLogFlags(LogFlags::All);

	auto			 writerOwned = std::make_unique<LogWriterMemory>();
	LogWriterMemory* writer		 = writerOwned.get();
	gLogger.AddLogDestination(
		{ELogDestinationType::Console, std::move(writerOwned), std::make_unique<LogFormatterConsole>()});

	LogCritical("macro Critical");
	LogError("macro Error");
	LogWarning("macro Warning");
	LogInfo("macro Info");
	LogDebug("macro Debug");

	TEST_CHECK(r, writer->GetEntries().size() == 5);
	TEST_CHECK(r, writer->GetEntries()[0].Level == ELogLevel::Critical);
	TEST_CHECK(r, writer->GetEntries()[0].Message == "macro Critical");
	TEST_CHECK(r, writer->GetEntries()[4].Level == ELogLevel::Debug);
	TEST_CHECK(r, writer->GetEntries()[4].Message == "macro Debug");

	// restore the global logger's real console destination and prior settings
	gLogger.AddLogDestination(
		{ELogDestinationType::Console, std::make_unique<LogWriterConsole>(), std::make_unique<LogFormatterConsole>()});
	gLogger.SetLogLevel(prevLevel);
	gLogger.SetLogFlags(prevFlags);
}

// Bonus: prints real colored output for a human to glance at. Nothing
// here is asserted - the equivalent contract is already locked down
// exactly by TestFormatter_ExactColorCodePerLevel above.
void PrintVisualSample()
{
	Logger logger;
	logger.SetLogPrefix("VisualSample");
	logger.SetLogLevel(ELogLevel::Debug);
	logger.SetLogFlags(LogFlags::All);
	logger.AddLogDestination(
		{ELogDestinationType::Console, std::make_unique<LogWriterConsole>(), std::make_unique<LogFormatterConsole>()});

	logger.Log(ELogLevel::Debug, "");
	LogAllLevels(logger);
}

} // namespace

void TestLogger()
{
	TestReporter r("LoggerTest");

	TestFormatter_ExactColorCodePerLevel(r);
	TestFormatter_ColorOffProducesNoEscapeCodes(r);
	TestFormatter_TimeOnlyMatchesExpectedShape(r);
	TestFormatter_LevelOnlyProducesExpectedTag(r);
	TestFormatter_PrefixOnlyIncludedWhenNonEmpty(r);
	TestFormatter_NoFlagsProducesBareMessage(r);
	TestFormatter_AllFlagsComposeInOrder(r);

	TestWriter_RoutesAboveWarningToCoutAtOrBelowToCerr(r);
	TestWriter_FlushDoesNotCrashOrWriteAnything(r);

	TestLogLevelFiltering(r);
	TestLevelFlagInteraction(r);
	TestLogMethodVariants(r);
	TestPrefixVariations(r);
	TestDestinationManagement(r);
	TestFlush(r);
	TestConcurrency(r);
	TestRapidFire(r);
	TestGlobalLoggerMacros(r);

	r.PrintSummary();

	//PrintVisualSample();
}

} // namespace Test
