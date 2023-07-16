#include "engine/allocator.h"

#include <malloc.h>

class DefaultAllocator : public IAllocator {
public:
	DefaultAllocator() {}
	~DefaultAllocator() {}

	void* allocate(size_t size, size_t align) override;
	void* reallocate(void* ptr, size_t size, size_t align) override;
	void deallocate(void* ptr) override;
};

static DefaultAllocator s_default_allocator_impl;
IAllocator* g_allocator = (IAllocator*)&s_default_allocator_impl;

void* DefaultAllocator::allocate(size_t size, size_t align) {
	return _aligned_malloc(size, align);
}

void* DefaultAllocator::reallocate(void* ptr, size_t size, size_t align) {
	return _aligned_realloc(ptr, size, align);
}

void DefaultAllocator::deallocate(void* ptr) {
	_aligned_free(ptr);
}

void* operator new(size_t size)
{
	return g_allocator->allocate(size, DEFAUL_ALIGMENT);
}
//
//void* operator new(size_t size, size_t aligment)
//{
//	return g_allocator->allocate(size, aligment);
//}

void operator delete(void* ptr)
{
	g_allocator->deallocate(ptr);
}

