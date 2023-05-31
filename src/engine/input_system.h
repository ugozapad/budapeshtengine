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
#define getNamelessMouseButtonIndex(buttonNum) mouseButton_t(mouseButton_LastNamed + buttonNum)

class IInputSystem {
public:
	static IInputSystem* create(IAllocator* allocator);

public:
	virtual ~IInputSystem() {}

	virtual void init() = 0;
	virtual void shutdown() = 0;

	virtual void onKeyDown		(scanCode_t scancode) = 0;
	virtual void onKeyUp		(scanCode_t scancode) = 0;
	virtual void onMouseKeyDown	(mouseButton_t mousebutton) = 0;
	virtual void onMouseKeyUp	(mouseButton_t mousebutton) = 0;
	virtual void onMouseMove	(int32_t x, int32_t y) = 0;
	virtual void onMouseWheel	(int32_t x, int32_t y) = 0;

	virtual bool isKeyPressed	(scanCode_t scancode) const = 0;
};

extern IInputSystem* g_input_system;

#endif // !INPUT_SYSTEM_H
