#pragma once

#include <cstdint>

/**
 * General requirements for enums:
 * - enums should be signed
 * - enums should start with an invalid value of -1
 * - enums should end with a count value
 */
template <typename EnumType>
constexpr inline bool IsEnumValueValid(EnumType value)
{
	return value > EnumType::Invalid && value < EnumType::Count;
}

enum class ELogDestinationType : uint8_t
{
	Console,
	LOG,
	CSV,
	JSON,
	XML
};

enum class ELogLevel : uint8_t
{
	Critical, // We cannot recover from this, application needs to close
	Error,	  // We can recover from this, no need to close application
	Warning,  // This might be or might become a problem in the future
	Info,	  // Just FYI
	Debug	  // Spam
};

enum class EConsoleFormat : uint8_t
{
	ResetAll,
	ResetIntensity,
	ResetUnderline,

	BlackFg,
	RedFg,
	GreenFg,
	YellowFg,
	BlueFg,
	MagentaFg,
	CyanFg,
	GrayFg,
	DefaultFg,

	BlackBg,
	RedBg,
	GreenBg,
	YellowBg,
	BlueBg,
	MagentaBg,
	CyanBg,
	GrayBg,
	DefaultBg,

	Bold,  //Brighter
	Faint, //Dimmer
	Italic,
	Underlined,
	Blink,
	Highlight, //Swap fg and bg
	CrossedOut,
	DoublyUnderlined,
	Overlined
};

enum class EUnitOfTime : uint8_t
{
	Nanosecond,
	Microsecond,
	Millisecond,
	Second,
	Minute,
	Hour,
	Day
};

enum class ETimeStringFormat : uint8_t
{
	Default,
	Timestamp,
	Timepoint
};
