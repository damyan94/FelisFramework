#include "stdafx.h"

#include "ExampleApplication.h"

int main(int argC, char** argV)
{
	ExampleApplication app(argC, argV);

	const auto ec = app.RunApplication();
	if (ec)
	{
		LogError(ec);
	}

	return ec.GetErrorCodeInt();
}
