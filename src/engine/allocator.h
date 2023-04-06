#ifndef ALLOCATOR_H
#define ALLOCATOR_H

#include <new>

class IAllocator {
public:
	virtual ~IAllocator() {}

	virtual void* allocate(size_t size, size_t align) = 0;
	virtual void deallocate(void* ptr) = 0;
};

extern IAllocator* g_default_allocator;

IAllocator* createDefaultAllocator();

#define MEM_NEW(a, T, ...) (new ((a).allocate(sizeof(T), alignof(T))) T(__VA_ARGS__))
#define MEM_DELETE(a, T, p)	if (p) {(p)->~T(); (a).deallocate(p);}

#endif // !ALLOCATOR_H
