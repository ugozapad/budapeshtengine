#ifndef GAMELIB_H
#define GAMELIB_H

#ifdef GAME_EXPORTS
#define GAME_API __declspec(dllexport)
#else
#define GAME_API __declspec(dllimport)
#endif // !GAME_EXPORTS

extern "C" GAME_API void gameLibInit();
extern "C" GAME_API void gameLibShutdown();

typedef void(*gameLibInit_t)();
typedef void(*gameLibShutdown_t)();

#endif