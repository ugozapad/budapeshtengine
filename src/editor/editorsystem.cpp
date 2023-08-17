#include <stdint.h>

#define WIN32_LEAN_AND_MEAN
#include <Windows.h>
#include <commdlg.h>
#include <shobjidl.h>

#include "engine/engine_api.h"
#include "engine/allocator.h"
#include "engine/engine.h"
#include "engine/camera.h"
#include "engine/objectfactory.h"
#include "editor_shared.h"

#include <imgui.h>
#include <imgui_impl_sdl2.h>
#include <imgui_impl_opengl3.h>

class EditorSystem : public IEditorSystem
{
public:
	EditorSystem();
	~EditorSystem();

	void Init();
	void Shutdown();

	void PollEvents(SDL_Event& event);

	void Update(float dt);
	void Render();

	void renderMenu();

	void openLevelDialog();

private:
	// Windows
	bool m_bHelpWindowOpened;
	bool m_bAboutWindowOpened;
	bool m_bCameraDebugWindowOpened;
	bool m_bDebugConsoleWindowOpened;
	bool m_bEntityCreationWindowOpened;
};

// Global instance of editor
static IEditorSystem* g_pEditorSystem;

EditorSystem::EditorSystem()
{
	g_pEditorSystem = this;
	m_bHelpWindowOpened = false;
	m_bAboutWindowOpened = false;
	m_bCameraDebugWindowOpened = false;
	m_bDebugConsoleWindowOpened = false;
	m_bEntityCreationWindowOpened = false;
}

EditorSystem::~EditorSystem()
{
	g_pEditorSystem = nullptr;
}

void EditorSystem::Init()
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
	ImGui_ImplSDL2_InitForOpenGL(g_engine->GetRenderWindow(), SDL_GL_GetCurrentContext());
	ImGui_ImplOpenGL3_Init(glsl_version);
}

void EditorSystem::Shutdown()
{
	// Cleanup
	ImGui_ImplOpenGL3_Shutdown();
	ImGui_ImplSDL2_Shutdown();
	ImGui::DestroyContext();
}

void EditorSystem::PollEvents(SDL_Event& event)
{
	ImGui_ImplSDL2_ProcessEvent(&event);
}

void EditorSystem::Update(float dt)
{
	// Start the Dear ImGui frame
	ImGui_ImplOpenGL3_NewFrame();
	ImGui_ImplSDL2_NewFrame();
	ImGui::NewFrame();

	// Draw ...
	renderMenu();

	//ImGui::ShowDemoWindow((bool*)true);
}

void EditorSystem::Render()
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

void EditorSystem::renderMenu()
{
	if (ImGui::BeginMainMenuBar())
	{
		if (ImGui::BeginMenu("File"))
		{
			ImGui::MenuItem("New");
			if (ImGui::MenuItem("Open...")) openLevelDialog();
			ImGui::MenuItem("Save");
			ImGui::MenuItem("Save As...");
			ImGui::Separator();
			if (ImGui::MenuItem("Exit")) g_engine->RequestExit();
			ImGui::EndMenu();
		}

		if (ImGui::BeginMenu("Edit"))
		{
			ImGui::MenuItem("Undo");
			ImGui::MenuItem("Redo");
			ImGui::Separator();
			ImGui::MenuItem("Copy");
			ImGui::MenuItem("Cut");
			ImGui::MenuItem("Paste");
			ImGui::Separator();
			ImGui::MenuItem("Delete");
			ImGui::EndMenu();
		}

		if (ImGui::BeginMenu("Tools"))
		{
			if (ImGui::MenuItem("Entity Creation"))
				m_bEntityCreationWindowOpened = true;

			ImGui::EndMenu();
		}

		if (ImGui::BeginMenu("Debug"))
		{
			if (ImGui::MenuItem("Camera"))
				m_bCameraDebugWindowOpened = true;
			if (ImGui::MenuItem("Console"))
				m_bDebugConsoleWindowOpened = true;
			ImGui::EndMenu();
		}

		if (ImGui::BeginMenu("Help"))
		{
			if (ImGui::MenuItem("Help"))
				m_bHelpWindowOpened = true;
			ImGui::Separator();
			if (ImGui::MenuItem("About"))
				m_bAboutWindowOpened = true;
			ImGui::EndMenu();
		}

		ImGui::EndMainMenuBar();
	}

	if (m_bAboutWindowOpened)
	{
		if (ImGui::Begin("About", &m_bAboutWindowOpened))
		{
			ImGui::Text("Budapest Engine 0.01");
			ImGui::Text("Build Timestamp: %s", __TIMESTAMP__);
		}
		ImGui::End();
	}

	if (m_bHelpWindowOpened)
	{
		if (ImGui::Begin("Help", &m_bHelpWindowOpened))
		{
			ImGui::Text("No help");
		}
		ImGui::End();
	}

	if (m_bCameraDebugWindowOpened)
	{
		if (ImGui::Begin("Camera Debug", &m_bCameraDebugWindowOpened))
		{
		//	ImGui::InputFloat3("Position", reinterpret_cast<float*>(&g_camera.GetPosition()));
		//	ImGui::InputFloat3("Direction", reinterpret_cast<float*>(&g_camera.GetDirection()));
		}
		ImGui::End();
	}

	if (m_bEntityCreationWindowOpened)
	{
		if (ImGui::Begin("Entity Creation", &m_bEntityCreationWindowOpened))
		{
			Array<objectCreationInfo_t> objects;
			g_object_factory->GetObjectCreationInfos(objects);

			for (auto it : objects)
			{
				ImGui::Text("%s", it.classname);
			}
		}

		ImGui::End();
	}

#if 0
	{
		static char tmp[256];
		if (m_bDebugConsoleWindowOpened)
		{
			if (ImGui::Begin(
				"Debug Console",
				&m_bDebugConsoleWindowOpened
			))
			{
				if (ImGui::BeginChild(
					"LogFrame",
					ImVec2(0.f, ImGui::GetWindowSize().y - 64.f),
					true
				))
				{
					Logger::iterator I = Logger::begin(), E = Logger::end();
					for (; I != E; ++I)
						ImGui::TextUnformatted((*I).c_str());
					ImGui::SetScrollHereY();
				}
				ImGui::EndChild();

				if (ImGui::InputTextEx(
					"##CommandInput",
					"Type command here and press Enter",
					tmp,
					sizeof(tmp),
					ImVec2(-1.f, 0.f),
					ImGuiInputTextFlags_EnterReturnsTrue
				))
				{
					Logger::msg("> %s", tmp);
					CmdArgs args(tmp);
					if (g_cmdManager.hasCmd(args.getArgv(0)))
						g_cmdManager.executeNow(tmp);
					else
						Logger::msg("! Unreconized command %s", tmp);
					tmp[0] = '\0';
				}
			}
			ImGui::End();
		}
		else tmp[0] = '\0';
	}
#endif
}

void EditorSystem::openLevelDialog()
{
	HRESULT hr = CoInitializeEx(NULL, COINIT_APARTMENTTHREADED |
		COINIT_DISABLE_OLE1DDE);
	if (SUCCEEDED(hr))
	{
		IFileOpenDialog *pFileOpen;

		// Create the FileOpenDialog object.
		hr = CoCreateInstance(CLSID_FileOpenDialog, NULL, CLSCTX_ALL,
			IID_IFileOpenDialog, reinterpret_cast<void**>(&pFileOpen));

		if (SUCCEEDED(hr))
		{
			// Show the Open dialog box.
			hr = pFileOpen->Show(NULL);

			// Get the file name from the dialog box.
			if (SUCCEEDED(hr))
			{
				IShellItem *pItem;
				hr = pFileOpen->GetResult(&pItem);
				if (SUCCEEDED(hr))
				{
					PWSTR pszFilePath;
					hr = pItem->GetDisplayName(SIGDN_FILESYSPATH, &pszFilePath);

					// Display the file name to the user.
					if (SUCCEEDED(hr))
					{
						MessageBoxW(NULL, pszFilePath, L"File Path", MB_OK);
						CoTaskMemFree(pszFilePath);
					}
					pItem->Release();
				}
			}
			pFileOpen->Release();
		}

		CoUninitialize();
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
