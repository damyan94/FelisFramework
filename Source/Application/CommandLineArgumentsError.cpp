#include "stdafx.h"

#include "Application/CommandLineArgumentsError.h"

template <>
const char* Error<ECommandLineArgumentErrorCode>::s_Type = "Command line argument error";

template <>
const ErrorRegistry<ECommandLineArgumentErrorCode> Error<ECommandLineArgumentErrorCode>::s_Registry({
	ErrorData{"Success"},
	ErrorData{"Argument not found"},
	ErrorData{"Invalid argument value"},
	ErrorData{"Argument value is out of range"},
	ErrorData{"Positional argument index is out of range"},
});
