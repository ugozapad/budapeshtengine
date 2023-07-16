#define WIN32_LEAN_AND_MEAN
#include <Windows.h>

#include <stdarg.h>
#include <string.h>
#include <stdio.h>

#include <malloc.h>
#include <memory.h>
#include <new>

#define SDL_MAIN_HANDLED
#include <SDL.h>
#include <SDL_scancode.h>

#include <glm/glm.hpp>

// Own Headers
#include "engine/allocator.h"
#include "engine/debug.h"
#include "engine/array.h"
#include "engine/object.h"