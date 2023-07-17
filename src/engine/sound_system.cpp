#include "pch.h"
#include "engine/sound_system.h"

ISoundSystem* g_pSoundSystem = NULL;

ISoundSystem* ISoundSystem::create(IAllocator* allocator, const char* sSoundSystemName)
{
	ISoundSystem* pSoundSystem = NULL;
	char buff[_MAX_PATH]; snprintf(buff, sizeof(buff), "%s.dll", sSoundSystemName);
	HMODULE hSoundSystemLib = LoadLibrary(buff);
	if (hSoundSystemLib != NULL)
	{
		createSoundSystem_t createSoundSystem_ = (createSoundSystem_t)GetProcAddress(hSoundSystemLib, "createSoundSystem");
		if (createSoundSystem_ != NULL)
		{
			pSoundSystem = createSoundSystem_(allocator);
		}
		// Dima : Do we need to close it?
		//CloseHandle(hSoundSystemLib);
	}
	ASSERT(pSoundSystem);
	return (pSoundSystem);
}
