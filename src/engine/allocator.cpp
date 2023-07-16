#include "pch.h"
#include "engine/allocator.h"

IAllocator* g_default_allocator = nullptr;

class DefaultAllocator : public IAllocator {
public:
	DefaultAllocator() {}
	~DefaultAllocator() {}

	void* allocate(size_t size, size_t align) override;
	void* reallocate(void* ptr, size_t size, size_t align) override;
	void deallocate(void* ptr) override;
};

void* DefaultAllocator::allocate(size_t size, size_t align) {
	return _aligned_malloc(size, align);
}

void* DefaultAllocator::reallocate(void* ptr, size_t size, size_t align) {
	return _aligned_realloc(ptr, size, align);
}

void DefaultAllocator::deallocate(void* ptr) {
	_aligned_free(ptr);
}

IAllocator* createDefaultAllocator()
{
	static char memoryBuffer[sizeof(DefaultAllocator)];
	return new(memoryBuffer) DefaultAllocator();
}
