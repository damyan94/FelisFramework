#pragma once

using ParsedArgumentsContainer = std::unordered_map<std::string, std::string>;

// Command line arguments utility
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
	T GetOrDefault(const std::string& arg, T def) const;

	template <typename T>
	T GetArgumentAs(const std::string& arg) const;

private:
	int ParseArguments();

private:
	int	   m_ArgC;
	char** m_ArgV;

	const std::string		 m_ProgramName;
	ParsedArgumentsContainer m_ParsedArgs;
};

template <typename T>
inline T CommandLineArguments::GetOrDefault(const std::string& arg, T def) const
{
	ReturnIf(!HasArgument(arg), def);

	return GetArgumentAs<T>(arg);
}

template <>
inline bool CommandLineArguments::GetArgumentAs(const std::string& arg) const
{
	ReturnIf(!HasArgument(arg), bool());

	return bool(std::stoi(GetArgument(arg)));
}

template <>
inline int CommandLineArguments::GetArgumentAs(const std::string& arg) const
{
	ReturnIf(!HasArgument(arg), int());

	return std::stoi(GetArgument(arg));
}

template <>
inline float CommandLineArguments::GetArgumentAs(const std::string& arg) const
{
	ReturnIf(!HasArgument(arg), float());

	return std::stof(GetArgument(arg));
}

template <>
inline double CommandLineArguments::GetArgumentAs(const std::string& arg) const
{
	ReturnIf(!HasArgument(arg), double());

	return std::stof(GetArgument(arg));
}
