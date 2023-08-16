#ifndef INPUT_SYSTEM_H
#define INPUT_SYSTEM_H

class IAllocator;

typedef uint32_t	scanCode_t;
typedef uint8_t		mouseButton_t;
enum mouseButton_
{
	mouseButton_Left	,
	mouseButton_Middle	,
	mouseButton_Right	,

	mouseButton_LastNamed = mouseButton_Right
};
#define GetNamelessMouseButtonIndex(buttonNum) mouseButton_t(mouseButton_LastNamed + buttonNum)

class IInputSystem {
public:
	ENGINE_API static IInputSystem* Create();

public:
	virtual ~IInputSystem() {}

	virtual void Init() = 0;
	virtual void Shutdown() = 0;

	virtual void OnKeyDown		(scanCode_t scancode) = 0;
	virtual void OnKeyUp		(scanCode_t scancode) = 0;
	virtual void OnMouseKeyDown	(mouseButton_t mousebutton) = 0;
	virtual void OnMouseKeyUp	(mouseButton_t mousebutton) = 0;
	virtual void OnMouseMove	(int32_t x, int32_t y) = 0;
	virtual void OnMouseWheel	(int32_t x, int32_t y) = 0;

	virtual bool IsKeyPressed	(scanCode_t scancode) const = 0;
};

extern ENGINE_API IInputSystem* g_input_system;

#endif // !INPUT_SYSTEM_H
