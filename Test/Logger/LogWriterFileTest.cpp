#include "stdafx.h"

#include "Logger/LogWriterFileTest.h"

#include "Felis/File/File.h"
#include "Felis/Logger/Formatters/LogFormatterText.h"
#include "Felis/Logger/Logger.h"
#include "Felis/Logger/Writers/LogWriterFile.h"

namespace Test
{
namespace
{

class ScopedLogFile final
{
public:
	explicit ScopedLogFile(const char* name)
		: m_Path(std::string("FelisLogWriterFileTest_") + name + ".tmp")
	{
		std::error_code error;
		std::filesystem::remove(m_Path, error);
	}

	~ScopedLogFile()
	{
		std::error_code error;
		std::filesystem::remove(m_Path, error);
	}

	FELIS_DISABLE_COPY_AND_MOVE(ScopedLogFile);

	const std::filesystem::path& GetPath() const
	{
		return m_Path;
	}

private:
	std::filesystem::path m_Path;
};

void TestFormatterMaskIntersectionAndRuntimeChanges(TestReporter& r)
{
	LogFormatterText formatter(LogFlags{LogFlags::Plain});
	const LogEntry	 entry{LogFlags{LogFlags::All}, DateTime::Now(), ELogLevel::Info, "Mask", "message"};

	const std::string plain = formatter.Format(entry);
	TEST_CHECK(r, plain.find("\033[") == std::string::npos);
	TEST_CHECK(r, plain.find("[INF] [Mask] message\n") != std::string::npos);

	formatter.SetLogFlags(LogFlags{LogFlags::Level});
	TEST_CHECK(r, formatter.GetLogFlags().Flags == LogFlags::Level);
	TEST_CHECK(r, formatter.Format(entry) == "[INF] message\n");

	formatter.SetLogFlags(LogFlags{LogFlags::All});
	TEST_CHECK(r, formatter.Format(entry).find("\033[") != std::string::npos);

	const LogEntry prefixOnly{LogFlags{LogFlags::Prefix}, DateTime::Now(), ELogLevel::Info, "Mask", "message"};
	TEST_CHECK(r, formatter.Format(prefixOnly) == "[Mask] message\n");
}

void TestFileDestinationAndLoggerResults(TestReporter& r)
{
	ScopedLogFile testFile("Logger");
	Logger		  logger;

	logger.SetLogPrefix("File");
	logger.SetLogLevel(ELogLevel::Debug);
	logger.SetLogFlags(LogFlags::All);

	auto  writer	 = std::make_unique<LogWriterFile>(testFile.GetPath());
	auto* writerView = writer.get();
	TEST_CHECK(r, !writerView->Open());

	logger.AddLogDestination(
		{ELogDestinationType::LOG, std::move(writer), std::make_unique<LogFormatterText>(LogFlags{LogFlags::Plain})});

	TEST_CHECK(r, logger.Log(ELogLevel::Info, "message"));
	TEST_CHECK(r, logger.Flush());

	const auto destinationFlags = logger.GetLogDestinationFlags(ELogDestinationType::LOG);
	TEST_CHECK(r, destinationFlags.has_value());
	TEST_CHECK(r, destinationFlags && destinationFlags->Flags == LogFlags::Plain);
	TEST_CHECK(r, logger.SetLogDestinationFlags(ELogDestinationType::LOG, LogFlags{LogFlags::Level}));
	TEST_CHECK(r, !logger.SetLogDestinationFlags(ELogDestinationType::JSON, LogFlags{LogFlags::All}));
	TEST_CHECK(r, !logger.GetLogDestinationFlags(ELogDestinationType::JSON).has_value());
	TEST_CHECK(r, !writerView->Close());

	std::string contents;
	TEST_CHECK(r, !ReadTextFile(testFile.GetPath(), contents));
	TEST_CHECK(r, contents.find("\033[") == std::string::npos);
	TEST_CHECK(r, contents.find("[INF] [File] message\n") != std::string::npos);
}

void TestAppendMode(TestReporter& r)
{
	ScopedLogFile	 testFile("Append");
	LogFormatterText formatter(LogFlags{});

	{
		LogWriterFile writer(testFile.GetPath());
		TEST_CHECK(r, !writer.Open());
		TEST_CHECK(r, writer.Write({LogFlags{}, DateTime::Now(), ELogLevel::Info, "", "First"}, formatter));
		TEST_CHECK(r, !writer.Close());
	}

	{
		LogWriterFile writer(testFile.GetPath());
		TEST_CHECK(r, !writer.Open());
		TEST_CHECK(r, writer.Write({LogFlags{}, DateTime::Now(), ELogLevel::Info, "", "Second"}, formatter));
		TEST_CHECK(r, !writer.Close());
	}

	std::string contents;
	TEST_CHECK(r, !ReadTextFile(testFile.GetPath(), contents));
	TEST_CHECK(r, contents == "First\nSecond\n");
}

void TestUnopenedWriterReportsFailure(TestReporter& r)
{
	ScopedLogFile testFile("Unopened");
	Logger		  logger;

	logger.SetLogLevel(ELogLevel::Debug);
	logger.AddLogDestination({ELogDestinationType::LOG,
							  std::make_unique<LogWriterFile>(testFile.GetPath()),
							  std::make_unique<LogFormatterText>(LogFlags{})});

	TEST_CHECK(r, !logger.Log(ELogLevel::Info, "message"));
	TEST_CHECK(r, !logger.Flush());
}

void TestConcurrentWritesRemainComplete(TestReporter& r)
{
	ScopedLogFile	 testFile("Concurrent");
	LogWriterFile	 writer(testFile.GetPath());
	LogFormatterText formatter(LogFlags{});
	std::atomic<int> failures = 0;

	TEST_CHECK(r, !writer.Open());

	constexpr int			 threadCount	 = 4;
	constexpr int			 writesPerThread = 25;
	std::vector<std::thread> threads;
	threads.reserve(threadCount);

	for (int threadIndex = 0; threadIndex < threadCount; ++threadIndex)
	{
		threads.emplace_back(
			[&writer, &formatter, &failures, threadIndex]()
			{
				for (int writeIndex = 0; writeIndex < writesPerThread; ++writeIndex)
				{
					const std::string message = std::format("thread={} write={}", threadIndex, writeIndex);
					if (!writer.Write({LogFlags{}, DateTime::Now(), ELogLevel::Info, "", message}, formatter))
					{
						++failures;
					}
				}
			});
	}

	for (auto& thread : threads)
	{
		thread.join();
	}

	TEST_CHECK(r, failures == 0);
	TEST_CHECK(r, writer.Flush());
	TEST_CHECK(r, !writer.Close());

	std::string contents;
	TEST_CHECK(r, !ReadTextFile(testFile.GetPath(), contents));

	std::stringstream stream(contents);
	std::string		  line;
	int				  lineCount		= 0;
	bool			  completeLines = true;
	while (std::getline(stream, line))
	{
		++lineCount;
		completeLines = completeLines && line.starts_with("thread=") && line.find(" write=") != std::string::npos;
	}

	TEST_CHECK(r, lineCount == threadCount * writesPerThread);
	TEST_CHECK(r, completeLines);
}

} // namespace

int TestLogWriterFile()
{
	TestReporter r("LogWriterFileTest");

	TestFormatterMaskIntersectionAndRuntimeChanges(r);
	TestFileDestinationAndLoggerResults(r);
	TestAppendMode(r);
	TestUnopenedWriterReportsFailure(r);
	TestConcurrentWritesRemainComplete(r);

	r.PrintSummary();
	return r.GetFailures();
}

} // namespace Test
