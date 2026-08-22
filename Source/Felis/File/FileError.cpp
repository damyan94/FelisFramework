#include "Felis/stdafx.h"

#include "Felis/File/FileError.h"

namespace Felis
{
template <>
const char* Error<EFileErrorCode>::s_Type = "File error";

template <>
const ErrorRegistry<EFileErrorCode> Error<EFileErrorCode>::s_Registry({
	ErrorData{"Success"},
	ErrorData{"File is already open"},
	ErrorData{"File is not open"},
	ErrorData{"Operation is invalid for the file mode"},
	ErrorData{"Failed to open file"},
	ErrorData{"File is too large"},
	ErrorData{"Failed to read file"},
	ErrorData{"Failed to write file"},
	ErrorData{"Failed to flush file"},
	ErrorData{"Failed to close file"},
});
} // namespace Felis
