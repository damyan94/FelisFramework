#pragma once

#include "Felis/Application/Application.h"

// A small text-statistics command-line application
class ExampleApplication final : public Application
{
public:
	ExampleApplication(int argC, char** argV);
	~ExampleApplication() = default;

private:
	enum class ECountMode
	{
		Lines,
		Words,
		Bytes
	};

	ApplicationError OnInit() override;
	ApplicationError OnRun() override;
	ApplicationError OnDeinit() override;

	void PrintUsage() const;

private:
	std::string m_InputPath;
	ECountMode	m_CountMode = ECountMode::Lines;
	bool		m_ShowHelp	= false;
	bool		m_Verbose	= false;
};
