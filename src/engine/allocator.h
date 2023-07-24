#ifndef ALLOCATOR_H
#define ALLOCATOR_H

#ifndef DEFAULT_ALIGMENT
#	define DEFAULT_ALIGMENT 16
#endif

class IAllocator {
public:
	virtual ~IAllocator() {}

	virtual void* allocate(size_t size, size_t align) = 0;
	virtual void* reallocate(void* ptr, size_t size, size_t align) = 0;
	virtual void deallocate(void* ptr) = 0;
};

extern ENGINE_API IAllocator* g_allocator;

inline void* mem_alloc(size_t size, size_t align = DEFAULT_ALIGMENT)
{
	return (g_allocator->allocate(size, align));
}

inline void* mem_calloc(size_t size, size_t count, size_t align = DEFAULT_ALIGMENT)
{
	return (g_allocator->allocate(size * count, align));
}

template<typename T>
inline T* mem_talloc(size_t align = alignof(T))
{
	return (static_cast<T*>(g_allocator->allocate(sizeof(T), align)));
}

template<typename T>
inline T* mem_tcalloc(size_t count, size_t align = alignof(T))
{
	return (static_cast<T*>(g_allocator->allocate(sizeof(T) * count, align)));
}

template<typename T>
inline void mem_free(T*& ptr)
{
	g_allocator->deallocate(ptr);
	ptr = NULL;
}

template<typename T>
inline T* mem_trealloc(T* ptr, size_t count, size_t align = alignof(T))
{
	return (static_cast<T*>(g_allocator->reallocate(ptr, sizeof(T) * count, align)));
}

inline void* mem_realloc(void* ptr, size_t size, size_t align = DEFAULT_ALIGMENT)
{
	return (g_allocator->reallocate(ptr, size, align));
}

//#define MEM_NEW(T, ...) new(sizeof(T), alignof(T)) T(__VA_ARGS__)

#define SAFE_DELETE(PTR) do { \
	delete PTR; \
	PTR = nullptr; \
	} while (0)


// TODO: global define (BUILD_AS_DLL) or disable manualy in each module
#if 1
#define IMPLEMENT_ALLOCATOR \
	void*	operator	new		(size_t size)	{ return (mem_alloc(size)); } \
	void	operator	delete	(void* ptr)		{ mem_free(ptr); } \
	void*	operator	new[]	(size_t size)	{ return (mem_alloc(size)); } \
	void	operator	delete[](void* ptr)		{ mem_free(ptr); }

#else
#define IMPLEMENT_ALLOCATOR
#endif

#endif // !ALLOCATOR_H
