#pragma once

#include "Felis/File/File.h"
#include "Felis/Logger/Writers/ILogWriter.h"

namespace Felis
{
// Writes formatted log entries to a persistently open file
class LogWriterFile final : public ILogWriter
{
public:
	explicit LogWriterFile(std::filesystem::path path, EFileMode mode = EFileMode::Append);
	~LogWriterFile() = default;

	FELIS_DISABLE_COPY_AND_MOVE(LogWriterFile);

	FileError Open();
	FileError Close();
	bool	  IsOpen() const;

	bool Write(const LogEntry& log, const class ILogFormatter& formatter) override;
	bool Flush() override;

private:
	mutable std::mutex m_Mutex;
	File			   m_File;
};
} // namespace Felis
