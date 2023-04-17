#ifndef ARRAY_H
#define ARRAY_H

#include <memory.h>

#include "engine/allocator.h"

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
,	m_memory(nullptr)
,	m_size(0)
,	m_capacity(0)
{
}

template <typename T>
inline Array<T>::~Array() {
	freeMemory();
}

template<typename T>
inline void Array<T>::growMemory() {
	if (m_memory) {
		T* new_memory = (T*)m_allocator->allocate(sizeof(T), alignof(T));
		if (m_size > 0) {
			memcpy(new_memory, m_memory, sizeof(T)*m_size);
		}
		m_allocator->deallocate(m_memory);
		m_memory = new_memory;
	} else {
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
	if (m_capacity <= 0 || m_capacity < 0) {
		size_t new_capacity = m_capacity + 1;
		Array<T>::set_capacity(new_capacity);
	}

	m_memory[m_size++] = t;
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


#endif // !ARRAY_H
