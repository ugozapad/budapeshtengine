#define WIN32_LEAN_AND_MEAN
#include <Windows.h>

#include <stdint.h>
#include <stdio.h>
#include <string.h>
#include <assert.h>
#include <stdarg.h>
#include <time.h>

#include <malloc.h>
#include <memory.h>
#include <new>

#include <algorithm>

#define SDL_MAIN_HANDLED
#include <SDL.h>
#include <SDL_scancode.h>

#include <glm/glm.hpp>

// Own Headers
#include "engine/engine_api.h"
#include "engine/allocator.h"
#include "engine/debug.h"
#include "engine/array.h"
#include "engine/object.h"
#include "engine/logger.h"
