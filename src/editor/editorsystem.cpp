#include "editor_shared.h"

class EditorSystem : public IEditorSystem
{
public:
	EditorSystem();
	~EditorSystem();

	void init();
	void shutdown();

	void update(float dt);
};

// Global instance of editor
static IEditorSystem* g_pEditorSystem;

EditorSystem::EditorSystem()
{
	g_pEditorSystem = this;
}

EditorSystem::~EditorSystem()
{
	g_pEditorSystem = nullptr;
}

void EditorSystem::init()
{
}

void EditorSystem::shutdown()
{
}

void EditorSystem::update(float dt)
{
}

// Internal singleton
IEditorSystem* getEditorSystem()
{
	return g_pEditorSystem;
}

// Dll export
extern "C" __declspec(dllexport) IEditorSystem* createEditorSystem()
{
	return new EditorSystem();
}

#include "engine/engine_api.h"
#include "engine/allocator.h"

IMPLEMENT_ALLOCATOR;