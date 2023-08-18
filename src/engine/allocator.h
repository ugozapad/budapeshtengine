#ifndef ALLOCATOR_H
#define ALLOCATOR_H

#ifndef DEFAULT_ALIGMENT
#	define DEFAULT_ALIGMENT 16
#endif

#include <new>

class IAllocator {
public:
	virtual ~IAllocator() {}

	virtual void* Allocate(size_t size, size_t align) = 0;
	virtual void* Reallocate(void* ptr, size_t size, size_t align) = 0;
	virtual void Deallocate(void* ptr) = 0;
};

extern ENGINE_API IAllocator* g_allocator;

inline void* mem_alloc(size_t size, size_t align = DEFAULT_ALIGMENT)
{
	return (g_allocator->Allocate(size, align));
}

inline void* mem_calloc(size_t size, size_t count, size_t align = DEFAULT_ALIGMENT)
{
	return (g_allocator->Allocate(size * count, align));
}

template<typename T>
inline T* mem_talloc(size_t align = alignof(T))
{
	return (static_cast<T*>(g_allocator->Allocate(sizeof(T), align)));
}

template<typename T>
inline T* mem_tcalloc(size_t count, size_t align = alignof(T))
{
	return (static_cast<T*>(g_allocator->Allocate(sizeof(T) * count, align)));
}

template<typename T>
inline void mem_free(T*& ptr)
{
	g_allocator->Deallocate(ptr);
	ptr = nullptr;
}

template<typename T>
inline T* mem_trealloc(T* ptr, size_t count, size_t align = alignof(T))
{
	return (static_cast<T*>(g_allocator->Reallocate(ptr, sizeof(T) * count, align)));
}

inline void* mem_realloc(void* ptr, size_t size, size_t align = DEFAULT_ALIGMENT)
{
	return (g_allocator->Reallocate(ptr, size, align));
}

template <typename T, typename... Args>
inline T* mem_new(Args&&... args)
{
	T* mem = (T*)g_allocator->Allocate(sizeof(T), alignof(T));
	return new(mem) T(std::forward<Args>(args)...);
}

template <typename T>
inline void mem_delete(T*& ptr)
{
	if (ptr)
	{
		ptr->~T();
		g_allocator->Deallocate(ptr);
	}
}

#define SAFE_DELETE(PTR) do { \
	mem_free(PTR); \
	PTR = nullptr; \
	} while (0)

#endif // !ALLOCATOR_H
