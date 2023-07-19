#ifndef ENGINE_API_H
#define ENGINE_API_H

#ifndef ENGINE_EXPORTS
#	define ENGINE_API __declspec(dllimport)
#else
#	define ENGINE_API __declspec(dllexport)
#endif

#endif