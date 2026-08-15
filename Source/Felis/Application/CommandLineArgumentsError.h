#pragma once

#include "Felis/Error/Error.h"

enum class ECommandLineArgumentErrorCode : int32_t
{
	Success = 0,
	ArgumentNotFound,
	InvalidValue,
	ValueOutOfRange,
	PositionalArgumentOutOfRange,
	UnexpectedArgument,
	Count
};

using CommandLineArgumentError = Error<ECommandLineArgumentErrorCode>;
