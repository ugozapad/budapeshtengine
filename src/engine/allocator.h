#ifndef ALLOCATOR_H
#define ALLOCATOR_H

#define DEFAUL_ALIGMENT 16

class IAllocator {
public:
	virtual ~IAllocator() {}

	virtual void* allocate(size_t size, size_t align) = 0;
	virtual void* reallocate(void* ptr, size_t size, size_t align) = 0;
	virtual void deallocate(void* ptr) = 0;
};

extern IAllocator* g_allocator;

//#define MEM_NEW(T, ...) new(sizeof(T), alignof(T)) T(__VA_ARGS__)

#define SAFE_DELETE(PTR) do { \
	delete PTR; \
	PTR = nullptr; \
	} while (0)

#define IMPLEMENT_ALLOCATOR \
	void* operator new(size_t size) \
	{ \
		return g_allocator->allocate(size, DEFAUL_ALIGMENT); \
	} \
	void operator delete(void* ptr) \
	{ \
		g_allocator->deallocate(ptr); \
	}


#endif // !ALLOCATOR_H
