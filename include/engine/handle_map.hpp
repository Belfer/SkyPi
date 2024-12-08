#pragma once

#include <engine/byte_types.hpp>
#include <engine/list.hpp>

#define INDEX_MASK 0xFFFF
#define NEW_OBJECT_ID_ADD 0x10000

template <typename TVal>
class HandleMap
{
public:
	explicit HandleMap(SizeType capacity = 100)
		: m_indices(capacity)
		, m_objects(capacity)
	{
		m_count = 0;
		for (int i = 0; i < capacity; i++)
		{
			Index index;
			index.key = i;
			index.next = i + 1;

			m_indices.Add(index);
		}

		enqueue = 0;
		dequeue = capacity - 1;
	}

	inline bool Contains(u32 key)
	{
		Index& index = m_indices[key & INDEX_MASK];
		return index.key == key && index.index != USHRT_MAX;
	}

	inline T& LookUp(u32 key)
	{
		return m_handles[m_handles[key & INDEX_MASK].index];
	}

	inline u32 Insert(const T& value)
	{
		Handle& handle = m_handles[m_dequeue];
		m_dequeue = handle.next;
		handle.key += NEW_OBJECT_ID_ADD;
		handle.index = m_count++;
		T& obj = m_handles[handle.index];
		obj = value;
		return handle.id;
	}

	inline void Remove(u32 key)
	{
		Handle& handle = m_handles[key & INDEX_MASK];

		T& obj = m_handles[handle.index];
		obj = m_handles[--m_count];
		m_handles[handle.id & INDEX_MASK].index = handle.index;

		handle.index = USHRT_MAX;
		m_handles[m_enqueue].next = key & INDEX_MASK;
		m_enqueue = key & INDEX_MASK;
	}

private:
	struct Index
	{
		u32 key = 0;
		u16 index = 0;
		u16 next = 0;
	};

	struct Object
	{
		u32 key = 0;
		T value;
	};

	u16 m_enqueue;
	u16 m_dequeue;
	u32 m_count;
	List<Index> m_indices;
	List<Object> m_objects;
};