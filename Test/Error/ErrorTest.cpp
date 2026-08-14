#include "stdafx.h"

#include "Error/ErrorTest.h"

#include "TestReporter.h"

#include "Error/Error.h"
#include "Error/ErrorRegistry.h"

// Test-only error enum + registry specialization, isolated from EFelisErrorCode/FelisError.cpp
// so these tests don't depend on (or break alongside) the production error set.
namespace
{

enum class ETestErrorCode : int32_t
{
	Alpha = 0,
	Beta,
	Gamma,
	Count
};

} // namespace

template <>
const char* Error<ETestErrorCode>::s_Type = "TestError";

template <>
const ErrorRegistry<ETestErrorCode> Error<ETestErrorCode>::s_Registry({
	ErrorData{"Alpha"},
	ErrorData{"Beta"},
	ErrorData{"Gamma"},
});

namespace Test
{
namespace
{

static const char* s_ErrorNotRegisteredText = "<ERROR TEXT NOT DEFINED IN ERROR REGISTRY>";

// Error<T>

void TestErrorStoresCode(TestReporter& r)
{
	Error<ETestErrorCode> error(ETestErrorCode::Beta);

	TEST_CHECK(r, error.GetErrorCode() == ETestErrorCode::Beta);
	TEST_CHECK(r, error.GetErrorCodeInt() == (int)ETestErrorCode::Beta);
}

void TestExplicitIntConversionMatchesGetErrorCodeInt(TestReporter& r)
{
	Error<ETestErrorCode> error(ETestErrorCode::Gamma);

	TEST_CHECK(r, static_cast<int>(error) == error.GetErrorCodeInt());
}

void TestErrorDataForValidCodes(TestReporter& r)
{
	Error<ETestErrorCode> alpha(ETestErrorCode::Alpha);
	Error<ETestErrorCode> beta(ETestErrorCode::Beta);
	Error<ETestErrorCode> gamma(ETestErrorCode::Gamma);

	TEST_CHECK(r, std::string(alpha.GetErrorData().Text) == "Alpha");
	TEST_CHECK(r, std::string(beta.GetErrorData().Text) == "Beta");
	TEST_CHECK(r, std::string(gamma.GetErrorData().Text) == "Gamma");
}

void TestErrorDataFallbackForOutOfRangeCode(TestReporter& r)
{
	// Simulates a corrupted/invalid error code beyond Count
	auto				  invalidCode = static_cast<ETestErrorCode>((int)ETestErrorCode::Count + 5);
	Error<ETestErrorCode> error(invalidCode);

	TEST_CHECK(r, std::string(error.GetErrorData().Text) == s_ErrorNotRegisteredText);
}

void TestErrorDataFallbackForNegativeCode(TestReporter& r)
{
	// Covers the explicit negative-value check in ErrorRegistry::GetErrorData
	auto				  negativeCode = static_cast<ETestErrorCode>(-1);
	Error<ETestErrorCode> error(negativeCode);

	TEST_CHECK(r, std::string(error.GetErrorData().Text) == s_ErrorNotRegisteredText);
}

void TestHasErrorReflectsSuccessSentinel(TestReporter& r)
{
	// HasError() relies on enum value 0 meaning success - Alpha plays that role here
	Error<ETestErrorCode> success(ETestErrorCode::Alpha);
	Error<ETestErrorCode> failure(ETestErrorCode::Beta);

	TEST_CHECK(r, !success.HasError());
	TEST_CHECK(r, failure.HasError());
}

void TestOperatorBoolMatchesHasError(TestReporter& r)
{
	Error<ETestErrorCode> success(ETestErrorCode::Alpha);
	Error<ETestErrorCode> failure(ETestErrorCode::Gamma);

	TEST_CHECK(r, static_cast<bool>(success) == success.HasError());
	TEST_CHECK(r, static_cast<bool>(failure) == failure.HasError());
	TEST_CHECK(r, !static_cast<bool>(success));
	TEST_CHECK(r, static_cast<bool>(failure));
}

void TestGetTypeReturnsRegisteredType(TestReporter& r)
{
	Error<ETestErrorCode> error(ETestErrorCode::Beta);

	TEST_CHECK(r, std::string(error.GetType()) == "TestError");
}

void TestStreamInsertionFormatsTypeCodeAndDescription(TestReporter& r)
{
	Error<ETestErrorCode> error(ETestErrorCode::Gamma);

	std::ostringstream oss;
	oss << error;
	const std::string result = oss.str();

	TEST_CHECK(r, result.find("TestError") != std::string::npos);
	TEST_CHECK(r, result.find(std::to_string(error.GetErrorCodeInt())) != std::string::npos);
	TEST_CHECK(r, result.find("Gamma") != std::string::npos);
}

// ErrorRegistry<T>, constructed directly rather than through Error<T>

void TestRegistryMapsIndicesDirectly(TestReporter& r)
{
	ErrorRegistry<ETestErrorCode> registry({
		ErrorData{"First"},
		ErrorData{"Second"},
		ErrorData{"Third"},
	});

	TEST_CHECK(r, std::string(registry.GetErrorData(ETestErrorCode::Alpha).Text) == "First");
	TEST_CHECK(r, std::string(registry.GetErrorData(ETestErrorCode::Beta).Text) == "Second");
	TEST_CHECK(r, std::string(registry.GetErrorData(ETestErrorCode::Gamma).Text) == "Third");
}

void TestRegistryFallbackForOutOfRangeCode(TestReporter& r)
{
	ErrorRegistry<ETestErrorCode> registry({
		ErrorData{"First"},
		ErrorData{"Second"},
		ErrorData{"Third"},
	});

	auto invalidCode = static_cast<ETestErrorCode>(100);

	TEST_CHECK(r, std::string(registry.GetErrorData(invalidCode).Text) == s_ErrorNotRegisteredText);
}

void TestRegistryFallbackForNegativeCode(TestReporter& r)
{
	// Covers the explicit negative-value check, exercised directly against ErrorRegistry rather than through Error<T>
	ErrorRegistry<ETestErrorCode> registry({
		ErrorData{"First"},
		ErrorData{"Second"},
		ErrorData{"Third"},
	});

	auto negativeCode = static_cast<ETestErrorCode>(-1);

	TEST_CHECK(r, std::string(registry.GetErrorData(negativeCode).Text) == s_ErrorNotRegisteredText);
}

} // namespace

void TestError()
{
	TestReporter r("ErrorTest");

	TestErrorStoresCode(r);
	TestExplicitIntConversionMatchesGetErrorCodeInt(r);
	TestErrorDataForValidCodes(r);
	TestErrorDataFallbackForOutOfRangeCode(r);
	TestErrorDataFallbackForNegativeCode(r);
	TestHasErrorReflectsSuccessSentinel(r);
	TestOperatorBoolMatchesHasError(r);
	TestGetTypeReturnsRegisteredType(r);
	TestStreamInsertionFormatsTypeCodeAndDescription(r);
	TestRegistryMapsIndicesDirectly(r);
	TestRegistryFallbackForOutOfRangeCode(r);
	TestRegistryFallbackForNegativeCode(r);

	r.PrintSummary();
}

} // namespace Test
