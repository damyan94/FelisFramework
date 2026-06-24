#include "stdafx.h"

#include "Examples/ExampleApplication.h"

// Entrypoint
// TODO change cmake file so we can build as either lib (.so or .a), test app or example app
int main(int argC, char** argV)
{
	ExampleApplication app(argC, argV);

	return app.RunApplication();
}
