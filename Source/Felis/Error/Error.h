#pragma once

#include "Felis/Error/ErrorRegistry.h"

// A generic error class that can accept custom error codes via a template enum class
//
// The user needs to instantiate the static s_Type and s_Registry with a string and std::array
// that holds additional error data like error texts and others respectively, or else - a linker error
//
// Check FelisError.h and FelisError.cpp for reference
template <ErrorCodeEnum T>
class Error
{
public:
	Error(T errorCode);
	~Error() = default;

	explicit operator int() const;
	explicit operator bool() const;

	T				 GetErrorCode() const;
	int				 GetErrorCodeInt() const;
	DateTime		 GetTimestamp() const;
	const ErrorData& GetErrorData() const;

	// This relies that an enum value of 0 represents success
	bool HasError() const;

	const char* GetType() const;

private:
	T		 m_ErrorCode;
	DateTime m_Timestamp;

	static const char* s_Type;

	static const ErrorRegistry<T> s_Registry;
};

template <ErrorCodeEnum T>
Error<T>::Error(T errorCode)
	: m_ErrorCode(errorCode)
{
}

template <ErrorCodeEnum T>
inline Error<T>::operator int() const
{
	return GetErrorCodeInt();
}

template <ErrorCodeEnum T>
inline Error<T>::operator bool() const
{
	return HasError();
}

template <ErrorCodeEnum T>
T Error<T>::GetErrorCode() const
{
	return m_ErrorCode;
}

template <ErrorCodeEnum T>
int Error<T>::GetErrorCodeInt() const
{
	return (int)m_ErrorCode;
}

template <ErrorCodeEnum T>
DateTime Error<T>::GetTimestamp() const
{
	return m_Timestamp;
}

template <ErrorCodeEnum T>
const ErrorData& Error<T>::GetErrorData() const
{
	return s_Registry.GetErrorData(m_ErrorCode);
}

template <ErrorCodeEnum T>
inline bool Error<T>::HasError() const
{
	return m_ErrorCode != T(0);
}

template <ErrorCodeEnum T>
inline const char* Error<T>::GetType() const
{
	return s_Type;
}

template <ErrorCodeEnum T>
inline std::ostream& operator<<(std::ostream& os, const Error<T>& error)
{
	return os << error.GetType() << ": code " << error.GetErrorCodeInt()
			  << "; description: " << error.GetErrorData().Text;
}
