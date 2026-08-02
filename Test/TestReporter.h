#pragma once

// Minimal shared pass/fail counter + logging for hand-rolled test suites.
// Not a framework - just removes the s_Checks/s_Failures/macro duplication
// that would otherwise get copy-pasted into every TestXxx.cpp file.
class TestReporter
{
public:
	explicit TestReporter(std::string suiteName);

	void Check(bool condition, const char* conditionStr, const char* func, int line);
	void PrintSummary() const;
	int	 GetChecks() const;
	int	 GetFailures() const;

private:
	std::string m_SuiteName;
	int			m_Checks   = 0;
	int			m_Failures = 0;
};

// Usage: TEST_CHECK(reporter, someCondition);
#define TEST_CHECK(reporter, cond) (reporter).Check((cond), #cond, __FUNCTION__, __LINE__)
