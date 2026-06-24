#pragma once

// uint8_t flags for a log message
struct LogFlags
{
	enum Type : uint8_t
	{
		Color  = 1 << 0,
		Time   = 1 << 1,
		Level  = 1 << 2,
		Prefix = 1 << 3,
		All	   = Color | Time | Level | Prefix
	};

	LogFlags& SetFlag(uint8_t type, bool value);
	void	  ClearFlags();
	bool	  IsFlagSet(uint8_t type) const;

	uint8_t Flags = 0;
};

// A log message
struct LogEntry
{
	LogFlags	Flags;
	DateTime	Time;
	ELogLevel	Level;
	std::string Prefix;
	std::string Message;
};
