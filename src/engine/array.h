#ifndef ARRAY_H
#define ARRAY_H

#include <memory.h>
#include <stdexcept>

#include "engine/debug.h"
#include "engine/allocator.h"

//#define STD_ARRAY

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
	typedef size_t				size_type;
	typedef T					value_type;
	typedef value_type&			reference;
	typedef value_type const&	const_reference;
	typedef value_type*			iterator;
	typedef value_type const*	const_iterator;

public:
	Array(IAllocator& allocator);
	~Array();

	iterator insert(const_iterator pos, const_reference value)
	{
		if (pos < begin() || pos > end())
			throw std::out_of_range("pos < begin() || pos > end()");
		// calculate index
		size_type idx = size_type(end() - pos);
		// check the capacity
		size_type const new_size = m_size + 1;
		if (new_size > m_capacity)
			reserve(m_capacity + 1);
		// shift elements
		for (size_type i = m_size; i > idx; --i)
			m_memory[i] = m_memory[i - 1];
		// assign value
		m_memory[idx] = value;
		m_size = new_size;
		return (m_memory + idx);
	}

	inline size_type size		() const { return m_size; }
	inline bool empty		() const { return !m_size; }
	inline size_type capacity	() const { return m_capacity; }

	inline void reserve(size_type capacity)
	{
		if (m_capacity < capacity)
			realloc_buffer(capacity);
	}
	inline void resize(size_type size)
	{
		if (size > m_capacity)
			reserve(size - m_capacity);
		m_size = size;
	}
	void resize(size_type size, const_reference value)
	{
		if (size > m_capacity)
		{
			reserve(size - m_capacity);
			for (size_type i = m_size; i < m_capacity; ++i)
				m_memory[i] = value;
		}
		m_size = size;
	}
	inline void clear	() { m_size = 0; }
	inline void erase	(const_iterator pos)
	{
		if (pos < begin() || pos > end())
			throw std::out_of_range("pos < begin() || pos > end()");
		size_type i = size_type(end() - pos);
		for (size_type j = i; j < (m_size - 1); ++j)
			m_memory[j] = m_memory[j + 1];
		m_size--;
	}
	inline void shrink_to_fit()
	{
		if (m_capacity > m_size)
			realloc_buffer(m_size);
	}

	inline reference at(size_type i)
	{
		if (i > size()) throw std::out_of_range("i > size()");
		return m_memory[i];
	}
	inline const_reference at(size_type i) const
	{
		if (i > size()) throw std::out_of_range("i > size()");
		return m_memory[i];
	}

	inline void push_back	(const_reference value) { insert(end(), value); }
	inline void pop_back	() { if (m_size > 0) m_size--; }

	inline reference		back	()			{ return m_memory[0]; }
	inline const_reference	back	() const	{ return m_memory[0]; }
	inline reference		front	()			{ return m_memory[m_size - 1]; }
	inline const_reference	front	() const	{ return m_memory[m_size - 1]; }

	inline value_type*		data	()			{ return m_memory; }
	inline value_type const*data	() const	{ return m_memory; }

	inline iterator			begin	()			{ return m_memory; }
	inline const_iterator	begin	() const	{ return m_memory; }
	inline iterator			end		()			{ return m_memory + m_size; }
	inline const_iterator	end		() const	{ return m_memory + m_size; }

	inline reference		operator[](size_type i)		{ return m_memory[i]; }
	inline const_reference	operator[](size_type i) const	{ return m_memory[i]; }

private:
	void realloc_buffer(size_type);

private:
	IAllocator* m_allocator;
	value_type* m_memory;
	size_type m_size;
	size_type m_capacity;
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
	if (m_memory) {
		m_allocator->deallocate(m_memory);
		m_memory = nullptr;
	}
}

template<typename T>
void Array<T>::realloc_buffer(size_type size) {
	if (m_memory) {
		m_memory = (value_type*)m_allocator->reallocate(m_memory, sizeof(value_type) * size, alignof(value_type));
	}
	else {
		m_memory = (value_type*)m_allocator->allocate(sizeof(value_type) * size, alignof(value_type));
	}

	m_capacity = size;
	if (m_capacity < m_size)
		m_size = m_capacity;
}


#endif // STD_ARRAY

#endif // !ARRAY_H
