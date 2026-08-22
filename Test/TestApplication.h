#pragma once

#include "Felis/Application/Application.h"

class TestApplication final : public Felis::Application
{
public:
	TestApplication(int argC, char** argV);
	int GetFailures() const;

private:
	Felis::ApplicationError OnInit() override;
	Felis::ApplicationError OnRun() override;
	Felis::ApplicationError OnDeinit() override;

private:
	int m_Failures = 0;
};
