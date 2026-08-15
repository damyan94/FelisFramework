#pragma once

#include "Felis/Application/Application.h"

// An example application
class ExampleApplication final : public Application
{
public:
	ExampleApplication(int argC, char** argV);
	~ExampleApplication() = default;

private:
	ApplicationError OnInit() override;
	ApplicationError OnRun() override;
	ApplicationError OnDeinit() override;
};
