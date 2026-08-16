#include "stdafx.h"

#include "TestApplication.h"

int main(int argC, char** argV)
{
	TestApplication app(argC, argV);

	const auto ec = app.RunApplication();
	if (ec)
	{
		LogError(ec);
		return ec.GetErrorCodeInt();
	}

	return app.GetFailures() == 0 ? 0 : 1;
}
