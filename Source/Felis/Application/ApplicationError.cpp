#include "Felis/stdafx.h"

#include "Felis/Application/ApplicationError.h"

namespace Felis
{
template <>
const char* Error<EApplicationErrorCode>::s_Type = "Application error";

template <>
const ErrorRegistry<EApplicationErrorCode> Error<EApplicationErrorCode>::s_Registry({
	ErrorData{"Success"},
	ErrorData{"Invalid command line arguments"},
	ErrorData{"Application initialization failed"},
	ErrorData{"Application runtime failed"},
	ErrorData{"Application deinitialization failed"},
});
} // namespace Felis
