#include <stdint.h>

#include "engine/engine_api.h"
#include "engine/allocator.h"
#include "engine/engine.h"

#include "editor_shared.h"

#include <imgui.h>
#include <imgui_impl_sdl2.h>
#include <imgui_impl_opengl3.h>

class EditorSystem : public IEditorSystem
{
public:
	EditorSystem();
	~EditorSystem();

	void init();
	void shutdown();

	void pollEvents(SDL_Event& event);

	void update(float dt);
	void render();
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
	// Setup Dear ImGui context
	IMGUI_CHECKVERSION();
	ImGui::CreateContext();
	ImGuiIO& io = ImGui::GetIO(); (void)io;
	io.ConfigFlags |= ImGuiConfigFlags_NavEnableKeyboard;     // Enable Keyboard Controls
	io.ConfigFlags |= ImGuiConfigFlags_NavEnableGamepad;      // Enable Gamepad Controls
	io.ConfigFlags |= ImGuiConfigFlags_DockingEnable;         // Enable Docking
	io.ConfigFlags |= ImGuiConfigFlags_ViewportsEnable;       // Enable Multi-Viewport / Platform Windows
	//io.ConfigViewportsNoAutoMerge = true;
	//io.ConfigViewportsNoTaskBarIcon = true;

	// Setup Dear ImGui style
	ImGui::StyleColorsDark();
	//ImGui::StyleColorsLight();

	// When viewports are enabled we tweak WindowRounding/WindowBg so platform windows can look identical to regular ones.
	ImGuiStyle& style = ImGui::GetStyle();
	if (io.ConfigFlags & ImGuiConfigFlags_ViewportsEnable)
	{
		style.WindowRounding = 0.0f;
		style.Colors[ImGuiCol_WindowBg].w = 1.0f;
	}

	const char* glsl_version = "#version 130";

	// Setup Platform/Renderer backends
	ImGui_ImplSDL2_InitForOpenGL(g_engine->getRenderWindow(), SDL_GL_GetCurrentContext());
	ImGui_ImplOpenGL3_Init(glsl_version);
}

void EditorSystem::shutdown()
{
	// Cleanup
	ImGui_ImplOpenGL3_Shutdown();
	ImGui_ImplSDL2_Shutdown();
	ImGui::DestroyContext();
}

void EditorSystem::pollEvents(SDL_Event& event)
{
	ImGui_ImplSDL2_ProcessEvent(&event);
}

void EditorSystem::update(float dt)
{
	// Start the Dear ImGui frame
	ImGui_ImplOpenGL3_NewFrame();
	ImGui_ImplSDL2_NewFrame();
	ImGui::NewFrame();

	// Draw ...
	//ImGui::ShowDemoWindow((bool*)true);
}

void EditorSystem::render()
{
	ImGuiIO& io = ImGui::GetIO();

	// Rendering
	ImGui::Render();
	ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());

	// Update and Render additional Platform Windows
	// (Platform functions may change the current OpenGL context, so we save/restore it to make it easier to paste this code elsewhere.
	//  For this specific demo app we could also call SDL_GL_MakeCurrent(window, gl_context) directly)
	if (io.ConfigFlags & ImGuiConfigFlags_ViewportsEnable)
	{
		SDL_Window* backup_current_window = SDL_GL_GetCurrentWindow();
		SDL_GLContext backup_current_context = SDL_GL_GetCurrentContext();
		ImGui::UpdatePlatformWindows();
		ImGui::RenderPlatformWindowsDefault();
		SDL_GL_MakeCurrent(backup_current_window, backup_current_context);
	}
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

IMPLEMENT_ALLOCATOR;