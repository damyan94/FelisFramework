#include "stdafx.h"

#include "TestReporter.h"

TestReporter::TestReporter(std::string suiteName)
	: m_SuiteName(std::move(suiteName))
{
}

void TestReporter::Check(bool condition, const char* conditionStr, const char* func, int line)
{
	++m_Checks;

	if (!condition)
	{
		++m_Failures;
		LogError(m_SuiteName, ": CHECK FAILED: ", conditionStr, " (", func, ", line ", line, ")");
	}
}

void TestReporter::PrintSummary() const
{
	const auto passed = m_Checks - m_Failures;
	const auto status = m_Failures == 0 ? "[PASSED]" : "[FAILED]";

	LogInfo(m_SuiteName, ": ", m_Failures, " failed; ", passed, " passed; ", m_Checks, " total; ", status);
}

int TestReporter::GetChecks() const
{
	return m_Checks;
}

int TestReporter::GetFailures() const
{
	return m_Failures;
}
