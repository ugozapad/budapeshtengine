#ifndef ARRAY_H
#define ARRAY_H

#include "engine/debug.h"
#include "engine/allocator.h"

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
	inline Array() :
		m_memory(nullptr)
		, m_size(0)
		, m_capacity(0)
	{
	}
	inline ~Array()
	{
		if (m_memory) {
			mem_free(m_memory);
		}
	}

	iterator insert(const_iterator pos, const_reference value)
	{
		ASSERT(pos >= begin() || pos <= end());
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
	inline bool empty			() const { return !m_size; }
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
	inline void erase	(iterator pos)
	{
		ASSERT(pos >= begin() || pos <= end());
		size_type i = size_type(end() - pos);
		for (size_type j = i; j < (m_size - 1); ++j)
			m_memory[j] = m_memory[j + 1];
		m_size--;
	}
	inline void erase	(iterator first, iterator last)
	{
		// TODO : Need to implement this !!!
	}
	inline void shrink_to_fit()
	{
		if (m_capacity > m_size)
			realloc_buffer(m_size);
	}

	inline reference at(size_type i)
	{
		ASSERT(i < size());
		return m_memory[i];
	}
	inline const_reference at(size_type i) const
	{
		ASSERT(i < size());
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
	void realloc_buffer(size_type size) {
		if (m_memory) {
			m_memory = mem_trealloc<value_type>(m_memory, size);
		}
		else {
			m_memory = mem_tcalloc<value_type>(size);
		}

		m_capacity = size;
		if (m_capacity < m_size)
			m_size = m_capacity;
	}

private:
	value_type* m_memory;
	size_type m_size;
	size_type m_capacity;
};

#endif // !ARRAY_H
