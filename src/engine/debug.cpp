#include "engine/debug.h"
#include <stdarg.h>
#include <string.h>
#include <stdio.h>

#include <Windows.h>

#include "SDL.h"

void assertBackend(const char* msg, ...) {
	char buffer[2048];
	va_list args;
	va_start(args, msg);
	vsnprintf(buffer, sizeof(buffer), msg, args);
	va_end(args);

	strcat(buffer, "\n\nPress abort for exit, retry for step into, ignore for continue");

	int i = MessageBoxA(NULL, buffer, "Ooops... It's crash time", MB_ICONERROR | MB_ABORTRETRYIGNORE);
	if (i == IDABORT)
		ExitProcess(-1);
	else if (i == IDRETRY)
		__debugbreak();
	else if (i == IDIGNORE)
		return;

//	SDL_ShowSimpleMessageBox(SDL_MESSAGEBOX_ERROR, "Ooops... It's crash time", buffer, nullptr);
}
