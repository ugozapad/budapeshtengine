#include "engine/debug.h"
#include <stdarg.h>
#include <string.h>
#include <stdio.h>

#include <Windows.h>

#include "SDL.h"

void assertBackend(const char* expression, const char* file, int line, const char* function, bool* ignore, const char* fmt, ...)
{
	if (ignore && *ignore) return;

	char buffer[4096];
	char msg[2048];

	if (fmt)
	{
		va_list p;
		va_start(p, fmt);
		vsnprintf(msg, sizeof(msg), fmt, p);
		va_end(p);

		sprintf(
			buffer,
			"Assertion Failed: %s\n"
			"File: %s\n"
			"Line: %d\n"
			"Function: %s\n"
			"\n"
			"Message: %s\n",
			"<no expression>",
			file,
			line,
			function,
			msg
		);
	}
	else
	{
		sprintf(
			buffer,
			"Assertion Failed: %s\n"
			"File: %s\n"
			"Line: %d\n"
			"Function: %s\n",
			"<no expression>",
			file,
			line,
			function
		);
	}

	UINT msgBoxType = MB_ICONERROR;
	if (ignore)
	{
		msgBoxType |= MB_ABORTRETRYIGNORE;
		strcat(buffer, "\n\nPress abort for exit, retry for step into, ignore for continue.");
	}
	else
	{
		msgBoxType |= MB_OK;
		strcat(buffer, "\n\nPress OK for exit.");
	}

	printf("%s(%d) - %s : %s\n", file, line, function, fmt ? msg : expression);

	switch (MessageBoxA(NULL, buffer, "Ooops... It's crash time", msgBoxType))
	{
	case IDIGNORE:
	{
		*ignore = true;
		return;
	}break;
	case IDRETRY:
	case IDOK:
	{
		if (IsDebuggerPresent())
			DebugBreak();
	}break;
	default: __assume(0);
	}

	ExitProcess(-1);
}

