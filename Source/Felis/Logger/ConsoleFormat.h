#pragma once

// Utilities for formatting output to the console
// TODO add iomanip functionality
class ConsoleFormat
{
public:
	ConsoleFormat();

	std::string EncloseString(const std::string& string) const;

	ConsoleFormat& Set(EConsoleFormat format);

private:
	std::string GetFormatString() const;
	std::string GetDefaultFormatString() const;

	friend std::ostream& operator<<(std::ostream& os, const ConsoleFormat& format);

private:
	std::string m_String;
	bool		m_HasAttributes;
};
