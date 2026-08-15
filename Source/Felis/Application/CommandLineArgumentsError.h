#pragma once

#include "Felis/Error/Error.h"

enum class ECommandLineArgumentErrorCode : int32_t
{
	Success = 0,
	ArgumentNotFound,
	InvalidValue,
	ValueOutOfRange,
	PositionalArgumentOutOfRange,
	Count
};

using CommandLineArgumentError = Error<ECommandLineArgumentErrorCode>;
