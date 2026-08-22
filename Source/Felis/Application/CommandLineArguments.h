#pragma once

#include "Felis/Application/CommandLineArgumentsError.h"

namespace Felis
{
using AllowedArgumentsContainer	   = std::set<std::string>;
using NonAllowedArgumentsContainer = std::set<std::string>;
using ParsedArgumentsContainer	   = std::unordered_map<std::string, std::string>;
using PositionalArgumentsContainer = std::vector<std::string>;

template <typename T>
concept CommandLineArgumentValue =
	std::same_as<std::remove_cvref_t<T>, bool> || std::same_as<std::remove_cvref_t<T>, int> ||
	std::same_as<std::remove_cvref_t<T>, float> || std::same_as<std::remove_cvref_t<T>, double> ||
	std::same_as<std::remove_cvref_t<T>, std::string>;

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

	bool								HasPositionalArgument(const std::string& arg) const;
	CommandLineArgumentError			GetPositionalArgument(size_t index, std::string& outValue) const;
	const PositionalArgumentsContainer& GetPositionalArguments() const;

	bool			   HasArgument(const std::string& arg) const;
	const std::string& GetArgument(const std::string& arg) const;

	template <CommandLineArgumentValue T>
	CommandLineArgumentError GetOrDefault(const std::string& arg, T& outValue, const T& def) const;

	template <CommandLineArgumentValue T>
	CommandLineArgumentError GetArgumentAs(const std::string& arg, T& outValue) const;

	template <CommandLineArgumentValue T>
	CommandLineArgumentError GetArgumentVectorAs(const std::string& arg, std::vector<T>& outValues) const;

	// Allowed names omit the leading "--"
	void EnableArgumentValidation(bool enabled);
	void AddAllowedArgument(const std::string& arg);
	void RemoveAllowedArgument(const std::string& arg);
	void ClearAllowedArguments();

	CommandLineArgumentError ValidateNamedArguments(NonAllowedArgumentsContainer& outUnexpectedArguments) const;

private:
	void ParseArguments();
	bool ParseArgumentVector(const std::string& value, std::vector<std::string>& outValues) const;

	template <CommandLineArgumentValue T>
	CommandLineArgumentError ParseValueAs(const std::string& value, T& outValue) const;

	template <typename T>
		requires std::same_as<T, int> || std::same_as<T, float> || std::same_as<T, double>
	CommandLineArgumentError ParseNumber(const std::string& value, T& outValue) const;

private:
	int	   m_ArgC;
	char** m_ArgV;

	const std::string			 m_ProgramName;
	PositionalArgumentsContainer m_PositionalArgs;
	ParsedArgumentsContainer	 m_ParsedArgs;
	AllowedArgumentsContainer	 m_AllowedArgs;
	bool						 m_IsArgumentValidationEnabled = false;
};

template <CommandLineArgumentValue T>
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

template <CommandLineArgumentValue T>
inline CommandLineArgumentError CommandLineArguments::GetArgumentAs(const std::string& arg, T& outValue) const
{
	ReturnIf(!HasArgument(arg), ECommandLineArgumentErrorCode::ArgumentNotFound);

	return ParseValueAs<T>(GetArgument(arg), outValue);
}

template <CommandLineArgumentValue T>
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
	requires std::same_as<T, int> || std::same_as<T, float> || std::same_as<T, double>
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
} // namespace Felis
