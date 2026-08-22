#include "Felis/stdafx.h"

#include "Felis/Logger/Writers/LogWriterFile.h"

#include "Felis/Logger/Formatters/ILogFormatter.h"

namespace Felis
{
LogWriterFile::LogWriterFile(std::filesystem::path path, EFileMode mode)
	: m_File(std::move(path), mode)
{
}

FileError LogWriterFile::Open()
{
	std::scoped_lock lock(m_Mutex);
	ReturnIf(m_File.GetMode() == EFileMode::Read, EFileErrorCode::InvalidMode);
	return m_File.Open();
}

FileError LogWriterFile::Close()
{
	std::scoped_lock lock(m_Mutex);
	return m_File.Close();
}

bool LogWriterFile::IsOpen() const
{
	std::scoped_lock lock(m_Mutex);
	return m_File.IsOpen();
}

bool LogWriterFile::Write(const LogEntry& log, const class ILogFormatter& formatter)
{
	const std::string formattedLog = formatter.Format(log);

	std::scoped_lock lock(m_Mutex);
	return !m_File.Write(formattedLog);
}

bool LogWriterFile::Flush()
{
	std::scoped_lock lock(m_Mutex);
	return !m_File.Flush();
}
} // namespace Felis
