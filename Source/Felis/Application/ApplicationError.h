#pragma once

#include "Felis/Error/Error.h"

namespace Felis
{
enum class EApplicationErrorCode : int32_t
{
	Success = 0,
	InvalidCommandLineArguments,
	InitializationFailed,
	RuntimeFailed,
	DeinitializationFailed,
	Count
};

using ApplicationError = Error<EApplicationErrorCode>;
} // namespace Felis
