#ifndef IEDITORSYSTEM_H
#define IEDITORSYSTEM_H

#include <SDL.h>

class IEditorSystem
{
public:
	virtual ~IEditorSystem();

	virtual void Init() = 0;
	virtual void Shutdown() = 0;

	// #TODO: Make InputListenter or SystemMessageListener
	virtual void PollEvents(SDL_Event& event) = 0;

	virtual void Update(float dt) = 0;
	virtual void Render() = 0;
};

inline IEditorSystem::~IEditorSystem()
{
}

typedef IEditorSystem* (*createEditorSystem_t)();

#endif // !IEDITORSYSTEM_H
