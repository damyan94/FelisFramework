#pragma once

#include "Error/Error.h"

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
