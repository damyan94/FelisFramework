#include "stdafx.h"

#include "File/FileTest.h"

#include "Felis/File/File.h"

namespace Test
{
namespace
{

class ScopedTestFile final
{
public:
	explicit ScopedTestFile(const char* name)
		: m_Path(std::string("FelisFileTest_") + name + ".tmp")
	{
		std::error_code error;
		std::filesystem::remove(m_Path, error);
	}

	~ScopedTestFile()
	{
		std::error_code error;
		std::filesystem::remove(m_Path, error);
	}

	FELIS_DISABLE_COPY_AND_MOVE(ScopedTestFile);

	const std::filesystem::path& GetPath() const
	{
		return m_Path;
	}

private:
	std::filesystem::path m_Path;
};

void TestTextFileRoundTrip(TestReporter& r)
{
	ScopedTestFile file("TextRoundTrip");
	const std::string expected = "First line\nSecond line\n";

	const auto writeError = WriteTextFile(file.GetPath(), expected);
	TEST_CHECK(r, !writeError);

	std::string actual;
	const auto readError = ReadTextFile(file.GetPath(), actual);

	TEST_CHECK(r, !readError);
	TEST_CHECK(r, actual == expected);
}

void TestTextFileAppend(TestReporter& r)
{
	ScopedTestFile file("TextAppend");

	TEST_CHECK(r, !WriteTextFile(file.GetPath(), "First"));
	TEST_CHECK(r, !WriteTextFile(file.GetPath(), "Second", EFileMode::Append));

	std::string contents;
	TEST_CHECK(r, !ReadTextFile(file.GetPath(), contents));
	TEST_CHECK(r, contents == "FirstSecond");
}

void TestEmptyTextFile(TestReporter& r)
{
	ScopedTestFile file("EmptyText");

	TEST_CHECK(r, !WriteTextFile(file.GetPath(), ""));

	std::string contents = "Unchanged";
	TEST_CHECK(r, !ReadTextFile(file.GetPath(), contents));
	TEST_CHECK(r, contents.empty());
}

void TestBinaryFileRoundTrip(TestReporter& r)
{
	ScopedTestFile file("BinaryRoundTrip");

	const std::vector<std::byte> expected = {
		std::byte{0x00},
		std::byte{0x01},
		std::byte{0x7f},
		std::byte{0xff},
	};

	TEST_CHECK(r, !WriteBinaryFile(file.GetPath(), expected));

	std::vector<std::byte> actual;
	TEST_CHECK(r, !ReadBinaryFile(file.GetPath(), actual));
	TEST_CHECK(r, actual == expected);
}

void TestFailedReadPreservesOutput(TestReporter& r)
{
	ScopedTestFile file("Missing");
	std::string contents = "Unchanged";

	const auto error = ReadTextFile(file.GetPath(), contents);

	TEST_CHECK(r, error.GetErrorCode() == EFileErrorCode::OpenFailed);
	TEST_CHECK(r, contents == "Unchanged");
}

void TestFileStoresPathModeAndOpenState(TestReporter& r)
{
	ScopedTestFile testFile("State");
	File		   file(testFile.GetPath(), EFileMode::Write);

	TEST_CHECK(r, file.GetPath() == testFile.GetPath());
	TEST_CHECK(r, file.GetMode() == EFileMode::Write);
	TEST_CHECK(r, !file.IsOpen());

	TEST_CHECK(r, !file.Open());
	TEST_CHECK(r, file.IsOpen());
	TEST_CHECK(r, !file.Write("First"));
	TEST_CHECK(r, !file.Write("Second"));
	TEST_CHECK(r, !file.Flush());
	TEST_CHECK(r, !file.Close());
	TEST_CHECK(r, !file.IsOpen());

	std::string contents;
	TEST_CHECK(r, !ReadTextFile(testFile.GetPath(), contents));
	TEST_CHECK(r, contents == "FirstSecond");
}

void TestFileRejectsOperationsForInvalidStateOrMode(TestReporter& r)
{
	ScopedTestFile testFile("InvalidState");
	File		   unopened(testFile.GetPath(), EFileMode::Read);
	std::string	   contents = "Unchanged";

	TEST_CHECK(r, static_cast<bool>(unopened.ReadAllText(contents)));
	TEST_CHECK(r, contents == "Unchanged");

	TEST_CHECK(r, !WriteTextFile(testFile.GetPath(), "Contents"));

	File reader(testFile.GetPath(), EFileMode::Read);
	TEST_CHECK(r, !reader.Open());
	TEST_CHECK(r, static_cast<bool>(reader.Write("Invalid")));
	TEST_CHECK(r, !reader.Close());
}

} // namespace

int TestFile()
{
	TestReporter r("FileTest");
	TestFileStoresPathModeAndOpenState(r);
	TestFileRejectsOperationsForInvalidStateOrMode(r);

	TestTextFileRoundTrip(r);
	TestTextFileAppend(r);
	TestEmptyTextFile(r);
	TestBinaryFileRoundTrip(r);
	TestFailedReadPreservesOutput(r);

	r.PrintSummary();
	return r.GetFailures();
}

} // namespace Test
