#pragma once

#if defined(_MSC_VER)
#include <intrin.h>
#endif

#define FELIS_STRINGIFY_IMPL(Value) #Value
#define FELIS_STRINGIFY(Value) FELIS_STRINGIFY_IMPL(Value)

#if defined(NDEBUG)
#define FELIS_DEBUG_BREAK() ((void)0)
#elif defined(_MSC_VER)
#define FELIS_DEBUG_BREAK() __debugbreak()
#elif defined(__clang__)
#define FELIS_DEBUG_BREAK() __builtin_debugtrap()
#elif defined(__GNUC__)
#define FELIS_DEBUG_BREAK() __builtin_trap()
#else
#define FELIS_DEBUG_BREAK() ((void)0)
#endif

// TODO Maybe use std::source_location::current
#define FELIS_DEBUG_BREAK_INFO(Reason)                                                                                 \
	"File: " __FILE__ "; Line: " FELIS_STRINGIFY(__LINE__) "; Reason: " FELIS_STRINGIFY(Reason)

#define Assert(Condition)                                                                                              \
	do                                                                                                                 \
	{                                                                                                                  \
		LogError(FELIS_DEBUG_BREAK_INFO(Condition));                                                                   \
		FELIS_DEBUG_BREAK();                                                                                           \
	} while (false)

#define ReturnIf(Condition, ...)                                                                                       \
	do                                                                                                                 \
	{                                                                                                                  \
		if (Condition)                                                                                                 \
		{                                                                                                              \
			return __VA_ARGS__;                                                                                        \
		}                                                                                                              \
	} while (false)

#define BreakIf(Condition)                                                                                             \
	if (Condition)                                                                                                     \
	break

#define ContinueIf(Condition)                                                                                          \
	if (Condition)                                                                                                     \
	continue

#define AssertReturnIf(Condition, ...)                                                                                 \
	do                                                                                                                 \
	{                                                                                                                  \
		if (Condition)                                                                                                 \
		{                                                                                                              \
			Assert(Condition);                                                                                         \
			return __VA_ARGS__;                                                                                        \
		}                                                                                                              \
	} while (false)

#define AssertBreakIf(Condition)                                                                                       \
	if (Condition)                                                                                                     \
	{                                                                                                                  \
		Assert(Condition);                                                                                             \
		break;                                                                                                         \
	}                                                                                                                  \
	else                                                                                                               \
		(void)0

#define AssertContinueIf(Condition)                                                                                    \
	if (Condition)                                                                                                     \
	{                                                                                                                  \
		Assert(Condition);                                                                                             \
		continue;                                                                                                      \
	}                                                                                                                  \
	else                                                                                                               \
		(void)0

#define SafeDelete(Pointer)                                                                                            \
	do                                                                                                                 \
	{                                                                                                                  \
		if (Pointer)                                                                                                   \
		{                                                                                                              \
			delete Pointer;                                                                                            \
			Pointer = nullptr;                                                                                         \
		}                                                                                                              \
	} while (false)

#define SafeDeleteArray(Pointer)                                                                                       \
	do                                                                                                                 \
	{                                                                                                                  \
		if (Pointer)                                                                                                   \
		{                                                                                                              \
			delete[] Pointer;                                                                                          \
			Pointer = nullptr;                                                                                         \
		}                                                                                                              \
	} while (false)

#define FELIS_DISABLE_COPY(ClassName)                                                                                  \
	ClassName(const ClassName&)			   = delete;                                                                   \
	ClassName& operator=(const ClassName&) = delete

#define FELIS_DISABLE_MOVE(ClassName)                                                                                  \
	ClassName(ClassName&&)			  = delete;                                                                        \
	ClassName& operator=(ClassName&&) = delete

#define FELIS_DEFAULT_COPY(ClassName)                                                                                  \
	ClassName(const ClassName&)			   = default;                                                                  \
	ClassName& operator=(const ClassName&) = default

#define FELIS_DEFAULT_MOVE(ClassName)                                                                                  \
	ClassName(ClassName&&)			  = default;                                                                       \
	ClassName& operator=(ClassName&&) = default

#define FELIS_DISABLE_COPY_AND_MOVE(ClassName)                                                                         \
	FELIS_DISABLE_COPY(ClassName);                                                                                     \
	FELIS_DISABLE_MOVE(ClassName)

#define FELIS_DEFAULT_COPY_AND_MOVE(ClassName)                                                                         \
	FELIS_DEFAULT_COPY(ClassName);                                                                                     \
	FELIS_DEFAULT_MOVE(ClassName)

#define FELIS_CREATE_STRONG_TYPE(Name, Type, DefaultValue, InvalidValue)                                               \
	class Name                                                                                                         \
	{                                                                                                                  \
	public:                                                                                                            \
		Name()	= default;                                                                                             \
		~Name() = default;                                                                                             \
		Name(Type initialValue)                                                                                        \
			: value(initialValue) {};                                                                                  \
                                                                                                                       \
		bool operator==(const Name& other) const                                                                       \
		{                                                                                                              \
			return value == other.value;                                                                               \
		}                                                                                                              \
		bool operator!=(const Name& other) const                                                                       \
		{                                                                                                              \
			return value != other.value;                                                                               \
		}                                                                                                              \
                                                                                                                       \
		bool IsValid() const                                                                                           \
		{                                                                                                              \
			return value != Name::Invalid;                                                                             \
		}                                                                                                              \
                                                                                                                       \
		inline static Type Default = DefaultValue;                                                                     \
		inline static Type Invalid = InvalidValue;                                                                     \
                                                                                                                       \
	public:                                                                                                            \
		Type value = DefaultValue;                                                                                     \
	};
