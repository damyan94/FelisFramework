#pragma once

#include "Felis/File/FileError.h"

namespace Felis
{
enum class EFileMode : uint8_t
{
	Read,
	Write,
	Append
};

class File final
{
public:
	File(std::filesystem::path path, EFileMode mode);
	~File();

	FELIS_DISABLE_COPY(File);
	FELIS_DEFAULT_MOVE(File);

	FileError Open();
	FileError Close();
	FileError Flush();

	FileError ReadAllText(std::string& contents);
	FileError ReadAllBinary(std::vector<std::byte>& contents);

	FileError Write(std::string_view contents);
	FileError Write(std::span<const std::byte> contents);

	bool IsOpen() const;

	const std::filesystem::path& GetPath() const;
	EFileMode					 GetMode() const;

private:
	std::filesystem::path m_Path;
	EFileMode			  m_Mode;
	std::fstream		  m_Stream;
};

// Text is treated as an exact sequence of bytes. No encoding or newline conversion is performed
FileError ReadTextFile(const std::filesystem::path& path, std::string& contents);
FileError ReadBinaryFile(const std::filesystem::path& path, std::vector<std::byte>& contents);

FileError WriteTextFile(const std::filesystem::path& path,
						std::string_view			 contents,
						EFileMode					 mode = EFileMode::Write);

FileError WriteBinaryFile(const std::filesystem::path& path,
						  std::span<const std::byte>   contents,
						  EFileMode					   mode = EFileMode::Write);
} // namespace Felis
