#pragma once

#if defined(_MSC_VER)
#include <intrin.h>
#endif

#define _Stringify_(X) #X
#define _Stringify(X) _Stringify_(X)

#if defined(NDEBUG)
#define _DebugBreak ((void)0)
#elif defined(_MSC_VER)
#define _DebugBreak __debugbreak()
#elif defined(__clang__)
#define _DebugBreak __builtin_debugtrap()
#elif defined(__GNUC__)
#define _DebugBreak __builtin_trap()
#else
#define _DebugBreak ((void)0)
#endif

// TODO Maybe use std::source_location::current
#define _DebugBreakInfo(_Reason) "File: " __FILE__ "; Line: " _Stringify(__LINE__) "; Reason: " _Stringify(_Reason)

#define Assert(...)                                                                                                    \
	LogError(_DebugBreakInfo(__VA_ARGS__));                                                                            \
	_DebugBreak

#define ReturnIf(_Condition, ...)                                                                                      \
	if (_Condition)                                                                                                    \
	return __VA_ARGS__

#define BreakIf(_Condition)                                                                                            \
	if (_Condition)                                                                                                    \
	break

#define ContinueIf(_Condition)                                                                                         \
	if (_Condition)                                                                                                    \
	continue

#define AssertReturnIf(_Condition, ...)                                                                                \
	do                                                                                                                 \
	{                                                                                                                  \
		if (_Condition)                                                                                                \
		{                                                                                                              \
			Assert(_Condition);                                                                                        \
			return __VA_ARGS__;                                                                                        \
		}                                                                                                              \
	} while (false)

#define AssertBreakIf(_Condition)                                                                                      \
	do                                                                                                                 \
	{                                                                                                                  \
		if (_Condition)                                                                                                \
		{                                                                                                              \
			Assert(_Condition);                                                                                        \
			break;                                                                                                     \
		}                                                                                                              \
	} while (false)

#define AssertContinueIf(_Condition)                                                                                   \
	do                                                                                                                 \
	{                                                                                                                  \
		if (_Condition)                                                                                                \
		{                                                                                                              \
			Assert(_Condition);                                                                                        \
			continue;                                                                                                  \
		}                                                                                                              \
	} while (false)

#define SafeDelete(_Pointer)                                                                                           \
	do                                                                                                                 \
	{                                                                                                                  \
		if (_Pointer)                                                                                                  \
		{                                                                                                              \
			delete _Pointer;                                                                                           \
			_Pointer = nullptr;                                                                                        \
		}                                                                                                              \
	} while (false)

#define SafeDeleteArray(_Pointer)                                                                                      \
	do                                                                                                                 \
	{                                                                                                                  \
		if (_Pointer)                                                                                                  \
		{                                                                                                              \
			delete[] _Pointer;                                                                                         \
			_Pointer = nullptr;                                                                                        \
		}                                                                                                              \
	} while (false)

#define DISABLE_COPY(Class)                                                                                            \
	Class(const Class&)			   = delete;                                                                           \
	Class& operator=(const Class&) = delete

#define DISABLE_MOVE(Class)                                                                                            \
	Class(Class&&)			  = delete;                                                                                \
	Class& operator=(Class&&) = delete

#define DEFAULT_COPY(Class)                                                                                            \
	Class(const Class&)			   = default;                                                                          \
	Class& operator=(const Class&) = default

#define DEFAULT_MOVE(Class)                                                                                            \
	Class(Class&&)			  = default;                                                                               \
	Class& operator=(Class&&) = default

#define DISABLE_COPY_AND_MOVE(Class)                                                                                   \
	DISABLE_COPY(Class);                                                                                               \
	DISABLE_MOVE(Class)

#define DEFAULT_COPY_AND_MOVE(Class)                                                                                   \
	DEFAULT_COPY(Class);                                                                                               \
	DEFAULT_MOVE(Class)

#define CREATE_STRONG_TYPE(_Name, _Type, _DefaultValue, _InvalidValue)                                                 \
	class _Name                                                                                                        \
	{                                                                                                                  \
	public:                                                                                                            \
		_Name()	 = default;                                                                                            \
		~_Name() = default;                                                                                            \
		_Name(_Type initialValue)                                                                                      \
			: value(initialValue) {};                                                                                  \
                                                                                                                       \
		bool operator==(const _Name& other) const                                                                      \
		{                                                                                                              \
			return value == other.value;                                                                               \
		}                                                                                                              \
		bool operator!=(const _Name& other) const                                                                      \
		{                                                                                                              \
			return value != other.value;                                                                               \
		}                                                                                                              \
                                                                                                                       \
		bool IsValid() const                                                                                           \
		{                                                                                                              \
			return value != _Name::Invalid;                                                                            \
		}                                                                                                              \
                                                                                                                       \
		inline static _Type Default = _DefaultValue;                                                                   \
		inline static _Type Invalid = _InvalidValue;                                                                   \
                                                                                                                       \
	public:                                                                                                            \
		_Type value = _DefaultValue;                                                                                   \
	};
