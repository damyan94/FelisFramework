#pragma once

#include "Felis/Error/Error.h"

namespace Felis
{
enum class EFileErrorCode : int32_t
{
	Success = 0,
	AlreadyOpen,
	NotOpen,
	InvalidMode,
	OpenFailed,
	FileTooLarge,
	ReadFailed,
	WriteFailed,
	FlushFailed,
	CloseFailed,
	Count
};

using FileError = Error<EFileErrorCode>;
} // namespace Felis
