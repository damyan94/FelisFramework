#include "Felis/stdafx.h"

#include "Felis/File/File.h"

namespace Felis
{
namespace
{

template <typename Container>
FileError ReadFileContents(std::fstream& stream, Container& contents)
{
	stream.clear();
	stream.seekg(0, std::ios::end);
	ReturnIf(!stream, EFileErrorCode::ReadFailed);

	const auto endPosition = stream.tellg();
	ReturnIf(endPosition == std::streampos(-1), EFileErrorCode::ReadFailed);

	const auto fileSize = static_cast<std::streamoff>(endPosition);
	ReturnIf(fileSize < 0, EFileErrorCode::ReadFailed);

	Container readContents;
	ReturnIf(static_cast<uintmax_t>(fileSize) > static_cast<uintmax_t>(readContents.max_size()),
			 EFileErrorCode::FileTooLarge);
	ReturnIf(fileSize > std::numeric_limits<std::streamsize>::max(), EFileErrorCode::FileTooLarge);

	readContents.resize(static_cast<size_t>(fileSize));

	stream.seekg(0, std::ios::beg);
	ReturnIf(!stream, EFileErrorCode::ReadFailed);

	if (!readContents.empty())
	{
		stream.read(reinterpret_cast<char*>(readContents.data()), static_cast<std::streamsize>(readContents.size()));
		ReturnIf(!stream, EFileErrorCode::ReadFailed);
	}

	contents = std::move(readContents);
	return EFileErrorCode::Success;
}

FileError WriteFileContents(std::fstream& stream, const char* contents, size_t size)
{
	ReturnIf(size > static_cast<size_t>(std::numeric_limits<std::streamsize>::max()), EFileErrorCode::FileTooLarge);

	if (size > 0)
	{
		stream.write(contents, static_cast<std::streamsize>(size));
		ReturnIf(!stream, EFileErrorCode::WriteFailed);
	}

	return EFileErrorCode::Success;
}

FileError FinishWrite(File& file)
{
	auto error = file.Flush();
	ReturnIf(error, error);

	return file.Close();
}

} // namespace

File::File(std::filesystem::path path, EFileMode mode)
	: m_Path(std::move(path)),
	  m_Mode(mode)
{
}

File::~File()
{
	if (m_Stream.is_open())
	{
		m_Stream.close();
	}
}

FileError File::Open()
{
	ReturnIf(m_Stream.is_open(), EFileErrorCode::AlreadyOpen);

	std::ios::openmode openMode = std::ios::binary;
	switch (m_Mode)
	{
	case EFileMode::Read: openMode |= std::ios::in; break;
	case EFileMode::Write: openMode |= std::ios::out | std::ios::trunc; break;
	case EFileMode::Append: openMode |= std::ios::out | std::ios::app; break;
	default: return EFileErrorCode::InvalidMode;
	}

	m_Stream.clear();
	m_Stream.open(m_Path, openMode);
	ReturnIf(!m_Stream.is_open(), EFileErrorCode::OpenFailed);

	return EFileErrorCode::Success;
}

FileError File::Close()
{
	ReturnIf(!m_Stream.is_open(), EFileErrorCode::NotOpen);

	m_Stream.close();
	ReturnIf(m_Stream.fail(), EFileErrorCode::CloseFailed);

	return EFileErrorCode::Success;
}

FileError File::Flush()
{
	ReturnIf(!m_Stream.is_open(), EFileErrorCode::NotOpen);
	ReturnIf(m_Mode == EFileMode::Read, EFileErrorCode::InvalidMode);

	m_Stream.flush();
	ReturnIf(!m_Stream, EFileErrorCode::FlushFailed);

	return EFileErrorCode::Success;
}

FileError File::ReadAllText(std::string& contents)
{
	ReturnIf(!m_Stream.is_open(), EFileErrorCode::NotOpen);
	ReturnIf(m_Mode != EFileMode::Read, EFileErrorCode::InvalidMode);

	return ReadFileContents(m_Stream, contents);
}

FileError File::ReadAllBinary(std::vector<std::byte>& contents)
{
	ReturnIf(!m_Stream.is_open(), EFileErrorCode::NotOpen);
	ReturnIf(m_Mode != EFileMode::Read, EFileErrorCode::InvalidMode);

	return ReadFileContents(m_Stream, contents);
}

FileError File::Write(std::string_view contents)
{
	ReturnIf(!m_Stream.is_open(), EFileErrorCode::NotOpen);
	ReturnIf(m_Mode == EFileMode::Read, EFileErrorCode::InvalidMode);

	return WriteFileContents(m_Stream, contents.data(), contents.size());
}

FileError File::Write(std::span<const std::byte> contents)
{
	ReturnIf(!m_Stream.is_open(), EFileErrorCode::NotOpen);
	ReturnIf(m_Mode == EFileMode::Read, EFileErrorCode::InvalidMode);

	return WriteFileContents(m_Stream, reinterpret_cast<const char*>(contents.data()), contents.size());
}

bool File::IsOpen() const
{
	return m_Stream.is_open();
}

const std::filesystem::path& File::GetPath() const
{
	return m_Path;
}

EFileMode File::GetMode() const
{
	return m_Mode;
}

FileError ReadTextFile(const std::filesystem::path& path, std::string& contents)
{
	File file(path, EFileMode::Read);

	auto error = file.Open();
	ReturnIf(error, error);

	return file.ReadAllText(contents);
}

FileError ReadBinaryFile(const std::filesystem::path& path, std::vector<std::byte>& contents)
{
	File file(path, EFileMode::Read);

	auto error = file.Open();
	ReturnIf(error, error);

	return file.ReadAllBinary(contents);
}

FileError WriteTextFile(const std::filesystem::path& path, std::string_view contents, EFileMode mode)
{
	ReturnIf(mode == EFileMode::Read, EFileErrorCode::InvalidMode);

	File file(path, mode);

	auto error = file.Open();
	ReturnIf(error, error);

	error = file.Write(contents);
	ReturnIf(error, error);

	return FinishWrite(file);
}

FileError WriteBinaryFile(const std::filesystem::path& path, std::span<const std::byte> contents, EFileMode mode)
{
	ReturnIf(mode == EFileMode::Read, EFileErrorCode::InvalidMode);

	File file(path, mode);

	auto error = file.Open();
	ReturnIf(error, error);

	error = file.Write(contents);
	ReturnIf(error, error);

	return FinishWrite(file);
}
} // namespace Felis
