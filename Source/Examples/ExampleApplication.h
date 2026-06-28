#pragma once

#include "Application/IApplication.h"

// An example application
class ExampleApplication final : public IApplication
{
public:
	ExampleApplication(int argC, char** argV);
	~ExampleApplication() = default;

	bool OnInit();
	void OnUpdate(Duration dt);

private:
};
