#include "stdafx.h"

#include "Application/ApplicationTest.h"

#include "TestReporter.h"

#include "Application/Application.h"

namespace Test
{
namespace
{

class LifecycleApplication final : public Application
{
public:
	LifecycleApplication(EApplicationErrorCode initResult,
						 EApplicationErrorCode runResult,
						 EApplicationErrorCode deinitResult)
		: Application(0, nullptr),
		  m_InitResult(initResult),
		  m_RunResult(runResult),
		  m_DeinitResult(deinitResult)
	{
	}

	bool InitCalled() const
	{
		return m_InitCalled;
	}

	bool RunCalled() const
	{
		return m_RunCalled;
	}

	bool DeinitCalled() const
	{
		return m_DeinitCalled;
	}

private:
	ApplicationError OnInit() override
	{
		m_InitCalled = true;
		return m_InitResult;
	}

	ApplicationError OnRun() override
	{
		m_RunCalled = true;
		return m_RunResult;
	}

	ApplicationError OnDeinit() override
	{
		m_DeinitCalled = true;
		return m_DeinitResult;
	}

private:
	EApplicationErrorCode m_InitResult;
	EApplicationErrorCode m_RunResult;
	EApplicationErrorCode m_DeinitResult;

	bool m_InitCalled	= false;
	bool m_RunCalled	= false;
	bool m_DeinitCalled = false;
};

void TestSuccessfulLifecycle(TestReporter& r)
{
	LifecycleApplication app(
		EApplicationErrorCode::Success, EApplicationErrorCode::Success, EApplicationErrorCode::Success);

	const auto error = app.RunApplication();

	TEST_CHECK(r, !error);
	TEST_CHECK(r, app.InitCalled());
	TEST_CHECK(r, app.RunCalled());
	TEST_CHECK(r, app.DeinitCalled());
}

void TestInitializationFailure(TestReporter& r)
{
	LifecycleApplication app(
		EApplicationErrorCode::InitializationFailed, EApplicationErrorCode::Success, EApplicationErrorCode::Success);

	const auto error = app.RunApplication();

	TEST_CHECK(r, error.GetErrorCode() == EApplicationErrorCode::InitializationFailed);
	TEST_CHECK(r, app.InitCalled());
	TEST_CHECK(r, !app.RunCalled());
	TEST_CHECK(r, !app.DeinitCalled());
}

void TestRuntimeFailureStillDeinitializes(TestReporter& r)
{
	LifecycleApplication app(
		EApplicationErrorCode::Success, EApplicationErrorCode::RuntimeFailed, EApplicationErrorCode::Success);

	const auto error = app.RunApplication();

	TEST_CHECK(r, error.GetErrorCode() == EApplicationErrorCode::RuntimeFailed);
	TEST_CHECK(r, app.InitCalled());
	TEST_CHECK(r, app.RunCalled());
	TEST_CHECK(r, app.DeinitCalled());
}

void TestDeinitializationFailure(TestReporter& r)
{
	LifecycleApplication app(
		EApplicationErrorCode::Success, EApplicationErrorCode::Success, EApplicationErrorCode::DeinitializationFailed);

	const auto error = app.RunApplication();

	TEST_CHECK(r, error.GetErrorCode() == EApplicationErrorCode::DeinitializationFailed);
	TEST_CHECK(r, app.InitCalled());
	TEST_CHECK(r, app.RunCalled());
	TEST_CHECK(r, app.DeinitCalled());
}

void TestRuntimeFailureTakesPriority(TestReporter& r)
{
	LifecycleApplication app(EApplicationErrorCode::Success,
							 EApplicationErrorCode::RuntimeFailed,
							 EApplicationErrorCode::DeinitializationFailed);

	const auto error = app.RunApplication();

	TEST_CHECK(r, error.GetErrorCode() == EApplicationErrorCode::RuntimeFailed);
	TEST_CHECK(r, app.DeinitCalled());
}

} // namespace

void TestApplicationLifecycle()
{
	TestReporter r("ApplicationTest");

	TestSuccessfulLifecycle(r);
	TestInitializationFailure(r);
	TestRuntimeFailureStillDeinitializes(r);
	TestDeinitializationFailure(r);
	TestRuntimeFailureTakesPriority(r);

	r.PrintSummary();
}

} // namespace Test
