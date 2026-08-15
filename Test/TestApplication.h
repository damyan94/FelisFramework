#pragma once

#include "Felis/Application/Application.h"

class TestApplication final : public Application
{
public:
	TestApplication(int argC, char** argV);

private:
	ApplicationError OnInit() override;
	ApplicationError OnRun() override;
	ApplicationError OnDeinit() override;
};
