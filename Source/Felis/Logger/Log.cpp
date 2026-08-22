#include "Felis/stdafx.h"

#include "Felis/Logger/Log.h"

namespace Felis
{

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

} // namespace Felis
