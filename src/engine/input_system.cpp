#include <stdint.h>

#include "engine/allocator.h"
#include "engine/input_system.h"

IInputSystem* g_input_system = nullptr;

class SDLInputSystem : public IInputSystem {
public:
	SDLInputSystem(IAllocator* allocator);
	~SDLInputSystem();

	void init() override;
	void shutdown() override;

	void pushEvent(const inputEvent_t& event) override;

	bool isKeyPressed(int key) override;

private:
	IAllocator* m_allocator;

};

IInputSystem* IInputSystem::create(IAllocator* allocator) {
	return MEM_NEW(*allocator, SDLInputSystem, allocator);
}

SDLInputSystem::SDLInputSystem(IAllocator* allocator) :
	m_allocator(allocator) {
}

SDLInputSystem::~SDLInputSystem() {
}

void SDLInputSystem::init() {
}

void SDLInputSystem::shutdown() {
}

void SDLInputSystem::pushEvent(const inputEvent_t& event) {
}

bool SDLInputSystem::isKeyPressed(int key) {
	return false;
}
