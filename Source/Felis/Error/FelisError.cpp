#include "Felis/stdafx.h"

#include "Felis/Error/FelisError.h"

#include "Felis/Error/Error.h"

template <>
const char* Error<EFelisErrorCode>::s_Type = "FelisFramework error";

template <>
const ErrorRegistry<EFelisErrorCode> Error<EFelisErrorCode>::s_Registry({
	ErrorData{"Success"},
	ErrorData{"Unknown"},
	ErrorData{"Generic"},
});
