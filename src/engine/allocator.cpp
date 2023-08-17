#include "pch.h"
#include "engine/allocator.h"

class DefaultAllocator : public IAllocator {
public:
	DefaultAllocator() {}
	~DefaultAllocator() {}

	void* Allocate(size_t size, size_t align) override;
	void* Reallocate(void* ptr, size_t size, size_t align) override;
	void Deallocate(void* ptr) override;
};

static DefaultAllocator s_default_allocator_impl;
ENGINE_API IAllocator* g_allocator = (IAllocator*)&s_default_allocator_impl;

void* DefaultAllocator::Allocate(size_t size, size_t align) {
	return _aligned_malloc(size, align);
}

void* DefaultAllocator::Reallocate(void* ptr, size_t size, size_t align) {
	return _aligned_realloc(ptr, size, align);
}

void DefaultAllocator::Deallocate(void* ptr) {
	_aligned_free(ptr);
}
