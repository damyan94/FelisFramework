#include "stdafx.h"

#include "Log.h"

LogFlags& LogFlags::SetFlag(uint8_t flag, bool value)
{
	Flags = (value) ? (Flags | flag) : (Flags & ~flag);
	return *this;
}

void LogFlags::ClearFlags()
{
	Flags = 0;
}

bool LogFlags::IsFlagSet(uint8_t flag) const
{
	return Flags & flag;
}
