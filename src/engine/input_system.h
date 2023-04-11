#ifndef INPUT_SYSTEM_H
#define INPUT_SYSTEM_H

class IAllocator;

class IInputSystem {
public:
	static IInputSystem* create(IAllocator* allocator);

public:
	virtual ~IInputSystem() {}

	virtual void init() = 0;
	virtual void shutdown() = 0;
};

extern IInputSystem* g_input_system;

#endif // !INPUT_SYSTEM_H
