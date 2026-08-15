#pragma once

#include "Application/CommandLineArgumentsError.h"

using ParsedArgumentsContainer	   = std::unordered_map<std::string, std::string>;
using PositionalArgumentsContainer = std::vector<std::string>;

// Command line arguments utility
// Supports --flag, --key=value, positional arguments and --
class CommandLineArguments
{
public:
	explicit CommandLineArguments(int argC, char** argV);
	~CommandLineArguments() = default;

	int	   GetArgC() const;
	char** GetArgV() const;

	const std::string& GetProgramName() const;

	bool			   HasArgument(const std::string& arg) const;
	const std::string& GetArgument(const std::string& arg) const;

	template <typename T>
	CommandLineArgumentError GetOrDefault(const std::string& arg, T& outValue, const T& def) const;

	template <typename T>
	CommandLineArgumentError GetArgumentAs(const std::string& arg, T& outValue) const;

	template <typename T>
	CommandLineArgumentError GetArgumentVectorAs(const std::string& arg, std::vector<T>& outValues) const;

	bool								HasPositionalArgument(const std::string& arg) const;
	CommandLineArgumentError			GetPositionalArgument(size_t index, std::string& outValue) const;
	const PositionalArgumentsContainer& GetPositionalArguments() const;

private:
	void ParseArguments();
	bool ParseArgumentVector(const std::string& value, std::vector<std::string>& outValues) const;

	template <typename T>
	CommandLineArgumentError ParseValueAs(const std::string& value, T& outValue) const;

	template <typename T>
	CommandLineArgumentError ParseNumber(const std::string& value, T& outValue) const;

	template <typename>
	static constexpr bool s_IsSupportedArgumentType = false;

private:
	int	   m_ArgC;
	char** m_ArgV;

	const std::string			 m_ProgramName;
	ParsedArgumentsContainer	 m_ParsedArgs;
	PositionalArgumentsContainer m_PositionalArgs;
};

template <typename T>
inline CommandLineArgumentError CommandLineArguments::GetOrDefault(const std::string& arg,
																   T&				  outValue,
																   const T&			  def) const
{
	if (!HasArgument(arg))
	{
		outValue = def;
		return ECommandLineArgumentErrorCode::Success;
	}

	return GetArgumentAs<T>(arg, outValue);
}

template <typename T>
inline CommandLineArgumentError CommandLineArguments::GetArgumentAs(const std::string& arg, T& outValue) const
{
	ReturnIf(!HasArgument(arg), ECommandLineArgumentErrorCode::ArgumentNotFound);

	return ParseValueAs<T>(GetArgument(arg), outValue);
}

template <typename T>
inline CommandLineArgumentError CommandLineArguments::GetArgumentVectorAs(const std::string& arg,
																		  std::vector<T>&	 outValues) const
{
	ReturnIf(!HasArgument(arg), ECommandLineArgumentErrorCode::ArgumentNotFound);

	std::vector<std::string> values;
	ReturnIf(!ParseArgumentVector(GetArgument(arg), values), ECommandLineArgumentErrorCode::InvalidValue);

	std::vector<T> parsedValues;
	parsedValues.reserve(values.size());

	for (const auto& value : values)
	{
		T parsedValue{};

		const auto error = ParseValueAs<T>(value, parsedValue);
		ReturnIf(error, error);

		parsedValues.emplace_back(std::move(parsedValue));
	}

	outValues = std::move(parsedValues);
	return ECommandLineArgumentErrorCode::Success;
}

template <typename T>
inline CommandLineArgumentError CommandLineArguments::ParseValueAs(const std::string&, T&) const
{
	static_assert(s_IsSupportedArgumentType<T>, "Unsupported command-line argument type");
	return ECommandLineArgumentErrorCode::InvalidValue;
}

template <>
inline CommandLineArgumentError CommandLineArguments::ParseValueAs(const std::string& value, bool& outValue) const
{
	if (value == "1" || value == "true")
	{
		outValue = true;
		return ECommandLineArgumentErrorCode::Success;
	}

	if (value == "0" || value == "false")
	{
		outValue = false;
		return ECommandLineArgumentErrorCode::Success;
	}

	return ECommandLineArgumentErrorCode::InvalidValue;
}

template <>
inline CommandLineArgumentError CommandLineArguments::ParseValueAs(const std::string& value, int& outValue) const
{
	return ParseNumber(value, outValue);
}

template <>
inline CommandLineArgumentError CommandLineArguments::ParseValueAs(const std::string& value, float& outValue) const
{
	return ParseNumber(value, outValue);
}

template <>
inline CommandLineArgumentError CommandLineArguments::ParseValueAs(const std::string& value, double& outValue) const
{
	return ParseNumber(value, outValue);
}

template <>
inline CommandLineArgumentError CommandLineArguments::ParseValueAs(const std::string& value,
																   std::string&		  outValue) const
{
	outValue = value;
	return ECommandLineArgumentErrorCode::Success;
}

template <typename T>
inline CommandLineArgumentError CommandLineArguments::ParseNumber(const std::string& value, T& outValue) const
{
	T parsedValue{};

	const auto* begin  = value.data();
	const auto* end	   = begin + value.size();
	const auto	result = std::from_chars(begin, end, parsedValue);

	ReturnIf(result.ec == std::errc::result_out_of_range, ECommandLineArgumentErrorCode::ValueOutOfRange);
	ReturnIf(result.ec != std::errc() || result.ptr != end, ECommandLineArgumentErrorCode::InvalidValue);

	outValue = parsedValue;
	return ECommandLineArgumentErrorCode::Success;
}
