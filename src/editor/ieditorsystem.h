#ifndef IEDITORSYSTEM_H
#define IEDITORSYSTEM_H

#include <SDL.h>

class IEditorSystem
{
public:
	virtual ~IEditorSystem();

	virtual void init() = 0;
	virtual void shutdown() = 0;

	// #TODO: Make InputListenter or SystemMessageListener
	virtual void pollEvents(SDL_Event& event) = 0;

	virtual void update(float dt) = 0;
	virtual void render() = 0;
};

inline IEditorSystem::~IEditorSystem()
{
}

typedef IEditorSystem* (*createEditorSystem_t)();

#endif // !IEDITORSYSTEM_H