#pragma once

#include <cstdint>

template <typename T>
concept ErrorCodeEnum = std::is_enum_v<T> && requires {
	{ T::Count } -> std::same_as<T>;
	requires(static_cast<std::underlying_type_t<T>>(T::Count) > 0);
};

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
	Day,
	Week,
	Month,
	Year,
	Count
};

enum class ETimeStringFormat : uint8_t
{
	Default,   //dd.mm.yy HH:MM:SS
	Timestamp, //yyyymmddHHMMSS
	Timepoint
};
