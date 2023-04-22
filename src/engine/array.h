#ifndef ARRAY_H
#define ARRAY_H

#include <memory.h>

#include "engine/allocator.h"

#define STD_ARRAY

#ifdef STD_ARRAY
#include <vector>

template <typename T>
struct StlAllocator {
	typedef T value_type;

	StlAllocator() = default;

	template <class U>
	constexpr StlAllocator(const StlAllocator<U>&) noexcept {}

	T* allocate(std::size_t n) {
		return (T*)g_default_allocator->allocate(n * sizeof(T), alignof(T));
	}

	void deallocate(T* p, std::size_t n) noexcept {
		g_default_allocator->deallocate(p);
	}
};

template <typename T>
class Array : public std::vector<T, StlAllocator<T>> {
public:
	Array(IAllocator& allocator) : m_allocator(&allocator) {}

private:
	IAllocator* m_allocator;
};
#else
template <typename T>
class Array {
public:
	Array(IAllocator& allocator);
	~Array();

	const size_t size() { return m_size; }

	void set_capacity(size_t capacity);

	void push_back(T& t);
	void pop_back();

	T* begin() { return m_memory; }
	T* end() { return m_memory + m_size; }

	T& operator[](size_t i);
	//	const T& operator[](size_t i);

	void growMemory();
	void freeMemory();

private:
	IAllocator* m_allocator;
	T* m_memory;
	size_t m_size;
	size_t m_capacity;
};

template <typename T>
inline Array<T>::Array(IAllocator& allocator) :
	m_allocator(&allocator)
	, m_memory(nullptr)
	, m_size(0)
	, m_capacity(0)
{
}

template <typename T>
inline Array<T>::~Array() {
	freeMemory();
}

template<typename T>
inline void Array<T>::growMemory() {
	if (m_memory) {
		T* new_memory = (T*)m_allocator->allocate(sizeof(T) * m_capacity, alignof(T));
		// DMan: there is _aligned_realloc for this type of operations
		for (size_t i = 0; i < m_size; ++i)
			new_memory[i] = m_memory[i];
		m_allocator->deallocate(m_memory);
		m_memory = new_memory;
	}
	else {
		m_memory = (T*)m_allocator->allocate(sizeof(T) * m_capacity, alignof(T));
	}
}

template<typename T>
inline void Array<T>::freeMemory() {
	if (m_memory) {
		m_allocator->deallocate(m_memory);
		m_memory = nullptr;
	}
}

template<typename T>
inline void Array<T>::set_capacity(size_t capacity) {
	if (m_capacity < capacity) {
		m_capacity = capacity;
		Array<T>::growMemory();
	}
}

template<typename T>
inline void Array<T>::push_back(T& t) {
	size_t new_size = m_size + 1;
	if (new_size > m_capacity) {
		Array<T>::set_capacity(m_capacity + 1);
	}
	m_memory[m_size] = t;
	m_size = new_size;
}

template<typename T>
inline void Array<T>::pop_back() {
	if (m_size > 0)
		m_size--;
}

template<typename T>
inline T& Array<T>::operator[](size_t i) {
	return m_memory[i];
}

//template<typename T>
//inline const T& Array<T>::operator[](size_t i) {
//	return m_memory[i];
//}


#endif // STD_ARRAY

#endif // !ARRAY_H
