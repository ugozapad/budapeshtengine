#ifndef INPUT_SYSTEM_H
#define INPUT_SYSTEM_H

class IAllocator;

enum keyboardKeys_t {
	KEY_Unknown = 0,

	// Alphabet
	KEY_A,
	KEY_B,
	KEY_C,
	KEY_D,
	KEY_E,
	KEY_F,
	KEY_G,
	KEY_H,
	KEY_I,
	KEY_J,
	KEY_K,
	KEY_L,
	KEY_M,
	KEY_N,
	KEY_O,
	KEY_P,
	KEY_Q,
	KEY_R,
	KEY_S,
	KEY_T,
	KEY_U,
	KEY_V,
	KEY_W,
	KEY_X,
	KEY_Y,
	KEY_Z,

	// Nums
	KEY_Num0,
	KEY_Num1,
	KEY_Num2,
	KEY_Num3,
	KEY_Num4,
	KEY_Num5,
	KEY_Num6,
	KEY_Num7,
	KEY_Num8,
	KEY_Num9,

	// F1-F12
	KEY_F1,
	KEY_F2,
	KEY_F3,
	KEY_F4,
	KEY_F5,
	KEY_F6,
	KEY_F7,
	KEY_F8,
	KEY_F9,
	KEY_F10,
	KEY_F11,
	KEY_F12,

	// Special keys
	KEY_Escape,
	KEY_Grave,
	KEY_Tab,
	KEY_Space,
	KEY_Enter,
	KEY_Backspace,
	KEY_Menu,
	KEY_Caps,

	KEY_LeftShift,
	KEY_LeftControl,
	KEY_LeftWindows,
	KEY_LeftAlt,

	KEY_RightAlt,
	KEY_RightWindows,
	KEY_RightControl,
	KEY_RightShift,

	// Special keys pt.2
	KEY_Insert,
	KEY_Delete,
	KEY_Home,
	KEY_End,
	KEY_PageUp,
	KEY_PageDown,

	// Special keys pt.3
	KEY_PrintScreen,
	KEY_ScrollLock,
	KEY_PauseBreak,

	// Arrows
	KEY_Left,
	KEY_Right,
	KEY_Up,
	KEY_Down,

	// Numpad
	KEY_Divide,
	KEY_Multiply,
	KEY_Minus,
	KEY_Plus,
	KEY_NumEnter,

	KEY_Numpad0,
	KEY_Numpad1,
	KEY_Numpad2,
	KEY_Numpad3,
	KEY_Numpad4,
	KEY_Numpad5,
	KEY_Numpad6,
	KEY_Numpad7,
	KEY_Numpad8,
	KEY_Numpad9,
	KEY_NumpadPeriod,

	// Max keys
	KEY_MAX
};

enum mouseButtons_t {
	MOUSE_BUTTON_None,
	MOUSE_BUTTON_Left,
	MOUSE_BUTTON_Middle,
	MOUSE_BUTTON_Right,
	MOUSE_BUTTON_1,
	MOUSE_BUTTON_2,

	MOUSE_BUTTON_MAX
};

enum inputEventType_t {
	INPUT_EVENT_NONE,
	INPUT_EVENT_KEY_DOWN,
	INPUT_EVENT_KEY_UP,
	INPUT_EVENT_KEY_PRESSING,

	INPUT_EVENT_MOUSE_BUTTON_DOWN,
	INPUT_EVENT_MOUSE_BUTTON_UP,
	INPUT_EVENT_MOUSE_BUTTON_PRESSING,

	INPUT_EVENT_MAX
};

struct inputEvent_t {
	inputEventType_t type;
	uint32_t scancode;
	uint32_t mousebutton;
};

class IInputSystem {
public:
	static IInputSystem* create(IAllocator* allocator);

public:
	virtual ~IInputSystem() {}

	virtual void init() = 0;
	virtual void shutdown() = 0;

	virtual void pushEvent(const inputEvent_t& event) = 0;

	virtual bool isKeyPressed(int key) = 0;
};

extern IInputSystem* g_input_system;

#endif // !INPUT_SYSTEM_H
