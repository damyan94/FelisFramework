#pragma once

#include "Felis/Application/Application.h"

class TestApplication final : public Application
{
public:
	TestApplication(int argC, char** argV);
	int GetFailures() const;

private:
	ApplicationError OnInit() override;
	ApplicationError OnRun() override;
	ApplicationError OnDeinit() override;

private:
	int m_Failures = 0;
};
