#ifndef IEDITORSYSTEM_H
#define IEDITORSYSTEM_H

class IEditorSystem
{
public:
	virtual ~IEditorSystem();

	virtual void init() = 0;
	virtual void shutdown() = 0;

	virtual void update(float dt) = 0;
};

inline IEditorSystem::~IEditorSystem()
{
}

typedef IEditorSystem* (*createEditorSystem_t)();

#endif // !IEDITORSYSTEM_H
