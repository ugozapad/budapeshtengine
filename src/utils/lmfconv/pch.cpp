#include "pch.h"

void print(const char* fmt, ...)
{
	static char buffer[1024];
	va_list args;
	va_start(args, fmt);
	vsnprintf(buffer, sizeof(buffer), fmt, args);
	va_end(args);

#ifdef _DEBUG
	OutputDebugStringA(buffer);
#endif

	fwrite(buffer, strlen(buffer), 1, stdout);
}
