#include "stdafx.h"

#include "Utils/Logger/ConsoleFormat.h"

static const std::string s_StartSequence("\033[");
static const std::string s_EndSequence("m");

static constexpr std::string_view GetFormat(EConsoleFormat format)
{
	switch (format)
	{
	case EConsoleFormat::ResetAll: return "0";
	case EConsoleFormat::ResetIntensity: return "22";
	case EConsoleFormat::ResetUnderline: return "24";
	case EConsoleFormat::BlackFg: return "30";
	case EConsoleFormat::RedFg: return "31";
	case EConsoleFormat::GreenFg: return "32";
	case EConsoleFormat::YellowFg: return "33";
	case EConsoleFormat::BlueFg: return "34";
	case EConsoleFormat::MagentaFg: return "35";
	case EConsoleFormat::CyanFg: return "36";
	case EConsoleFormat::GrayFg: return "37";
	case EConsoleFormat::DefaultFg: return "39";
	case EConsoleFormat::BlackBg: return "40";
	case EConsoleFormat::RedBg: return "41";
	case EConsoleFormat::GreenBg: return "42";
	case EConsoleFormat::YellowBg: return "43";
	case EConsoleFormat::BlueBg: return "44";
	case EConsoleFormat::MagentaBg: return "45";
	case EConsoleFormat::CyanBg: return "46";
	case EConsoleFormat::GrayBg: return "47";
	case EConsoleFormat::DefaultBg: return "49";
	case EConsoleFormat::Bold: return "1";
	case EConsoleFormat::Faint: return "2";
	case EConsoleFormat::Italic: return "3";
	case EConsoleFormat::Underlined: return "4";
	case EConsoleFormat::Blink: return "5";
	case EConsoleFormat::Highlight: return "7";
	case EConsoleFormat::CrossedOut: return "9";
	case EConsoleFormat::DoublyUnderlined: return "21";
	case EConsoleFormat::Overlined: return "53";
	default: return "0";
	}
}

ConsoleFormat::ConsoleFormat()
	: m_String(s_StartSequence),
	  m_HasAttributes(false)
{
}

std::string ConsoleFormat::EncloseString(const std::string& string) const
{
	return GetFormatString() + string + GetDefaultFormatString();
}

ConsoleFormat& ConsoleFormat::Set(EConsoleFormat format)
{
	if (m_HasAttributes)
	{
		m_String += ";";
	}

	m_String += GetFormat(format);
	m_HasAttributes = true;

	return *this;
}

std::string ConsoleFormat::GetFormatString() const
{
	return m_String + s_EndSequence;
}

std::string ConsoleFormat::GetDefaultFormatString() const
{
	return s_StartSequence + "0" + s_EndSequence;
}

std::ostream& operator<<(std::ostream& os, const ConsoleFormat& format)
{
	return os << format.m_String << s_EndSequence;
}
