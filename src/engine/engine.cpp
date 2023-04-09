#include "engine/allocator.h"
#include "engine/filesystem.h"
#include "engine/engine.h"

#include <stdio.h>

Engine::Engine() :
    m_render_window(nullptr)
{
}

Engine::~Engine() {
}

void Engine::init(int width, int height, bool fullscreen) {
    if (SDL_Init(SDL_INIT_EVERYTHING ^ SDL_INIT_SENSOR) != 0) {
        printf("Failed to initialize SDL2. Error core: %s\n", SDL_GetError());
    }

    // create filesystem
    g_file_system = IFileSystem::createPhysFS();

    // Initialize OpenGL context
    SDL_GL_SetAttribute(SDL_GL_CONTEXT_FLAGS, SDL_GL_CONTEXT_FORWARD_COMPATIBLE_FLAG);
    SDL_GL_SetAttribute(SDL_GL_CONTEXT_PROFILE_MASK, SDL_GL_CONTEXT_PROFILE_CORE);
    SDL_GL_SetAttribute(SDL_GL_CONTEXT_MAJOR_VERSION, 3);
    SDL_GL_SetAttribute(SDL_GL_CONTEXT_MINOR_VERSION, 3);
    SDL_GL_SetAttribute(SDL_GL_DOUBLEBUFFER, 1);

    Uint32 window_flags = SDL_WINDOW_OPENGL | SDL_WINDOW_SHOWN;
    if (fullscreen)
        window_flags |= SDL_WINDOW_FULLSCREEN;

    // Create window
    m_render_window = SDL_CreateWindow("Engine", SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED, width, height, SDL_WINDOW_OPENGL | SDL_WINDOW_SHOWN);
    if (!m_render_window) {
        printf("Failed to create render window. Error core: %s\n", SDL_GetError());
    }
}

void Engine::shutdown() {
    if (m_render_window) {
        SDL_DestroyWindow(m_render_window);
        m_render_window = nullptr;
    }

    IFileSystem::destroy(g_file_system);

    SDL_Quit();
}

SDL_Window* Engine::getRenderWindow() {
    return m_render_window;
}
