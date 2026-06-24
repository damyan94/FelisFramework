#pragma once

// Command line arguments utility
class CommandLineArguments
{
public:
	CommandLineArguments(int argC, char** ArgV);
	~CommandLineArguments() = default;

	bool					HasArgument(std::string_view arg) const;
	const std::string_view& GetArgument(std::string_view arg) const;

	template <typename T>
	T GetOrDefault(std::string_view arg, T def) const;

	template <typename T>
	T GetArgumentAs(std::string_view arg) const;

private:
	int ParseArguments();

private:
	int	   m_ArgC;
	char** m_ArgV;

	std::unordered_map<std::string_view, std::string_view> m_ParsedArgs;
};

template <typename T>
inline T CommandLineArguments::GetOrDefault(std::string_view arg, T def) const
{
	ReturnIf(HasArgument(arg), GetArgumentAs<T>(arg));

	return def;
}

template <>
inline int CommandLineArguments::GetArgumentAs(std::string_view arg) const
{
	ReturnIf(HasArgument(arg), std::stoi(GetArgument(arg).data()));

	return int();
}

template <>
inline bool CommandLineArguments::GetArgumentAs(std::string_view arg) const
{
	ReturnIf(HasArgument(arg), bool(std::stoi(GetArgument(arg).data())));

	return bool();
}
