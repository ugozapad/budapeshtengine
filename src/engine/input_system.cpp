#include "pch.h"
#include "engine/allocator.h"
#include "engine/input_system.h"

ENGINE_API IInputSystem* g_input_system = nullptr;

typedef uint32_t keyStateBitfield_t;
typedef uint32_t mouseButtonsStateBitfield_t; // i'm very sceptical about idea that someone could have more than 8 mbuttons but whatever.

#define SUPPORTED_MOUSEBUTTONS_NUM	sizeof(mouseButtonsStateBitfield_t)
#define SUPPORTED_SCANCODES_NUM		512		// taken from SDL_Scancode::SDL_NUM_SCANCODES
#if (SUPPORTED_SCANCODES_NUM != 512)		// actually we support any power-of-two value
#	error "SUPPORTED_SCANCODES_NUM must be 512"
#endif
#define BITS_IN_KEYSTATE			(sizeof(keyStateBitfield_t) * 8)
#define KEYSTATES_SIZE				(SUPPORTED_SCANCODES_NUM / BITS_IN_KEYSTATE)

class SDLInputSystem : public IInputSystem {
	struct Point
	{
		int32_t		x;
		int32_t		y;
	};
public:
	SDLInputSystem();
	~SDLInputSystem();

	void Init() override;
	void Shutdown() override;

	void OnKeyDown		(scanCode_t scancode) override;
	void OnKeyUp		(scanCode_t scancode) override;
	void OnMouseKeyDown	(mouseButton_t mousebutton) override;
	void OnMouseKeyUp	(mouseButton_t mousebutton) override;
	void OnMouseMove	(int32_t x, int32_t y) override;
	void OnMouseWheel	(int32_t x, int32_t y) override;

	bool IsKeyPressed	(scanCode_t scancode) const override;

private:
	IAllocator*					m_allocator;
	keyStateBitfield_t			m_keyState			[KEYSTATES_SIZE];
	mouseButtonsStateBitfield_t	m_mouseButtonBitfield;

	Point						m_mousePosition;
	Point						m_wheelPosition;
};

IInputSystem* IInputSystem::Create()
{
	return mem_new<SDLInputSystem>();
}

SDLInputSystem::SDLInputSystem() :
	m_mouseButtonBitfield(0), m_mousePosition({ 0, 0 }), 
	m_wheelPosition({ 0, 0 })
{
	for(size_t I = 0; I < KEYSTATES_SIZE; ++I)
		m_keyState[I] = 0;
}

SDLInputSystem::~SDLInputSystem() {
}

void SDLInputSystem::Init() {
}

void SDLInputSystem::Shutdown() {
}

#define getScanCodeStateIndex(scanCode) uint32_t(floorf(float(scanCode / KEYSTATES_SIZE)))
#define getScanCodeMask(scanCode, stateIndex) (1U << (scanCode - (BITS_IN_KEYSTATE * stateIndex)))

void SDLInputSystem::OnKeyDown(scanCode_t scancode)
{
	if (scancode < SUPPORTED_SCANCODES_NUM)
	{
		uint32_t keyStateIdx = getScanCodeStateIndex(scancode);
		keyStateBitfield_t bitfield = m_keyState[keyStateIdx];
		bitfield |= getScanCodeMask(scancode, keyStateIdx);
		m_keyState[keyStateIdx] = bitfield;
	}
}

void SDLInputSystem::OnKeyUp(scanCode_t scancode)
{
	if (scancode < SUPPORTED_SCANCODES_NUM)
	{
		uint32_t keyStateIdx = getScanCodeStateIndex(scancode);
		keyStateBitfield_t bitfield = m_keyState[keyStateIdx];
		bitfield &= ~getScanCodeMask(scancode, keyStateIdx);
		m_keyState[keyStateIdx] = bitfield;
	}
}

void SDLInputSystem::OnMouseKeyDown(mouseButton_t mousebutton)
{
	if (mousebutton < SUPPORTED_MOUSEBUTTONS_NUM)
		m_mouseButtonBitfield |= (1U << mousebutton);
}

void SDLInputSystem::OnMouseKeyUp(mouseButton_t mousebutton)
{
	if (mousebutton < SUPPORTED_MOUSEBUTTONS_NUM)
		m_mouseButtonBitfield &= ~(1U << mousebutton);
}

void SDLInputSystem::OnMouseMove(int32_t x, int32_t y)
{
	m_mousePosition.x = x;
	m_mousePosition.y = y;
}

void SDLInputSystem::OnMouseWheel(int32_t x, int32_t y)
{
	m_wheelPosition.x = x;
	m_wheelPosition.y = y;
}

bool SDLInputSystem::IsKeyPressed(scanCode_t scancode) const {
	uint32_t keyStateIdx = getScanCodeStateIndex(scancode);
	return (m_keyState[keyStateIdx] & getScanCodeMask(scancode, keyStateIdx));
}
