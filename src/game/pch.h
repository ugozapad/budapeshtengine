#ifndef PCH_H
#define PCH_H

#define WIN32_LEAN_AND_MEAN
#include <Windows.h>

#include <string.h>
#include <stdint.h>
#include <new>

#define SDL_MAIN_HANDLED
#include <SDL.h>
#include <SDL_scancode.h>

#include <glm/glm.hpp>

#include "../engine/engine_api.h"
#include "../engine/allocator.h"
#include "../engine/sound_system.h"
#include "../engine/array.h"
#include "../engine/debug.h"
#include "../engine/logger.h"

#endif // !PCH_H
