
#ifndef SOUND_EXPORTS
#	define SOUND_API __declspec(dllimport)
#else
#	define SOUND_API __declspec(dllexport)
#endif

#define WIN32_LEAN_AND_MEAN
#include <Windows.h>

#include <string.h>
#include <stdint.h>
#include <new>

#include <openal/alc.h>
#include <openal/al.h>

#pragma comment(lib, "openal32.lib")

#include "../engine/engine_api.h"
#include "../engine/allocator.h"
#include "../engine/sound_system.h"
#include "../engine/array.h"
#include "../engine/debug.h"
#include "../sound/sound_system_oal.h"
#include "../sound/sound_file.h"
#include "../sound/sound.h"
