/*
Copyright (C) 1997-2001 Id Software, Inc.

This program is free software; you can redistribute it and/or
modify it under the terms of the GNU General Public License
as published by the Free Software Foundation; either version 2
of the License, or (at your option) any later version.

This program is distributed in the hope that it will be useful,
but WITHOUT ANY WARRANTY; without even the implied warranty of
MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.

See the GNU General Public License for more details.

You should have received a copy of the GNU General Public License
along with this program; if not, write to the Free Software
Foundation, Inc., 59 Temple Place - Suite 330, Boston, MA 02111-1307, USA.
*/

// TArray.h

/*
==============================================================================

	dynArray
 
==============================================================================
*/

// Try to maintain a minimum alignment of 16B
#define ARRAY_GRANULARITY ((sizeof(TType)==1) ? 16 : 8)

template<typename TType, const sint32 TGranularity = ARRAY_GRANULARITY>
class dynArray
{
protected:
	/**
	 * Data
	 */
	sint32 m_maxElems;
	sint32 m_numElems;
	TType *m_data;

public:
	/**
	 * Constructors
	 */
	dynArray()
	: m_data(NULL)
	, m_numElems(0)
	, m_maxElems(0)
	{
	}

protected:
	dynArray(const sint32 Num)
	: m_data(NULL)
	, m_maxElems(0)
	, m_numElems(Num)
	{
		ReAllocate(m_numElems);
	}

	dynArray(const size_t Num)
	: m_data(NULL)
	, m_maxElems(0)
	, m_numElems(Num)
	{
		ReAllocate(m_numElems);
	}

public:
	dynArray(const dynArray<TType,TGranularity> &Source)
	: m_data(NULL)
	, m_numElems(0)
	, m_maxElems(0)
	{
		Copy(Source);
	}

	/**
	 * Destructors
	 */
	~dynArray()
	{
		assert(m_numElems >= 0);
		assert(m_maxElems >= m_numElems);

		Empty();
	}

	/**
	 * Operators
	 */
	inline const bool operator !=(const dynArray<TType,TGranularity> &Array)
	{
		if (m_numElems == Array.m_numElems)
		{
			for (sint32 i=0 ; i<m_numElems ; i++)
			{
				if (!((*this)[i] == Array[i]))
					return true;
			}

			return false;
		}

		return true;
	}

	inline dynArray<TType> &operator +=(const dynArray<TType,TGranularity> &Source)
	{
		Append(Source);
		return *this;
	}

	inline dynArray<TType> &operator =(const dynArray<TType,TGranularity> &Source)
	{
		Copy(Source);
		return *this;
	}

	inline const bool operator ==(const dynArray<TType,TGranularity> &Array) const
	{
		if (m_numElems == Array.m_numElems)
		{
			for (sint32 i=0 ; i<m_numElems ; i++)
			{
				if (!((*this)[i] == Array[i]))
					return false;
			}

			return true;
		}

		return false;
	}

	inline const TType &operator [](const sint32 Index) const
	{
		assert(Index >= 0);
		assert(Index < m_numElems || (Index == 0 && m_numElems == 0));
		assert(m_maxElems >= m_numElems);

		return m_data[Index];
	}
	inline TType &operator [](const sint32 Index)
	{
		assert(Index >= 0);
		assert(Index < m_numElems || (Index == 0 && m_numElems == 0));
		assert(m_maxElems >= m_numElems);

		return m_data[Index];
	}

	/**
	 * Item functions
	 */
	sint32 AddItem(const TType &Item)
	{
		new(*this) TType(Item);
		return m_numElems-1;
	}

	sint32 AddUniqueItem(const TType &Item)
	{
		for (sint32 i=0 ; i<m_numElems ; i++)
		{
			if ((*this)[i] == Item)
				return i;
		}

		return AddItem(Item);
	}

	inline const bool ContainsItem(const TType &Item) const
	{
		return (FindItemIndex(Item) != -1);
	}

	const bool FindItem(const TType &Item, sint32 &OutIndex) const
	{
		for (OutIndex=0 ; OutIndex<m_numElems ; OutIndex++)
		{
			if ((*this)[OutIndex] == Item)
				return true;
		}

		OutIndex = -1;
		return false;
	}

	const sint32 FindItemIndex(const TType &Item) const
	{
		for (sint32 Result=0 ; Result<m_numElems ; Result++)
		{
			if ((*this)[Result] == Item)
				return Result;
		}

		return -1;
	}

	sint32 InsertItem(const TType &Item, const sint32 Index)
	{
		new(*this, Index) TType(Item);
		return Index;
	}

	void RemoveItem(const TType &Item)
	{
		for (sint32 i=0 ; i<m_numElems ; i++)
		{
			if ((*this)[i] == Item)
				Remove(i--);
		}
	}

	inline void SwapItems(const sint32 Index1, const sint32 Index2)
	{
		Swap(Index1, Index2);
	}

	/**
	 * Array functions
	 */
	sint32 Add(const sint32 Num, const bool bZeroFill = false)
	{
		const sint32 Result = m_numElems;

		m_numElems += Num;
		if (m_numElems > m_maxElems)
		{
			ReAllocate(m_numElems);
		}

		if (bZeroFill)
		{
			memset((uint8*)m_data + Result*GetElemSize(), 0, Num*GetElemSize());
		}

		return Result;
	}

	void Append(const dynArray<TType,TGranularity> &Array)
	{
		if (this != &Array && Array.m_numElems > 0)
		{
			Reserve(m_numElems + Array.m_numElems);

			// Construct if needed
			if (dataType<TType>::NeedsCtor)
			{
				for (sint32 i=0 ; i<Array.m_numElems ; i++)
				{
					::new(*this) TType(Array[i]);
				}
			}
			else
			{
				// Otherwise just copy
				memcpy((uint8*)m_data+GetUsedSize(), Array.GetData(), Array.GetUsedSize());
				m_numElems += Array.m_numElems;
			}
		}
	}

	void Copy(const dynArray<TType,TGranularity> &Source)
	{
		if (this != &Source)
		{
			if (Source.m_numElems > 0)
			{
				// This will empty our array, and make sure enough space is allocated for the replacement
				Empty(Source.m_numElems);

				// Construct if necessary
				if (dataType<TType>::NeedsCtor)
				{
					for (sint32 i=0 ; i<Source.m_numElems ; i++)
					{
						new(*this) TType(Source[i]);
					}
				}
				else
				{
					// Otherwise just copy
					memcpy(m_data, Source.GetData(), Source.GetUsedSize());
					m_numElems = Source.m_numElems;
				}
			}
			else
			{
				Empty();
			}
		}
	}

	void Empty(const sint32 Slack=0)
	{
		if (dataType<TType>::NeedsCtor)
		{
			for (sint32 i=0 ; i<m_numElems ; i++)
				(&(*this)[i])->~TType();
		}

		m_numElems = 0;
		if (m_maxElems != Slack)
		{
			ReAllocate(Slack);
		}
	}

	void Insert(const sint32 Index, const sint32 Num=1, const bool bZeroFill = false)
	{
		assert(Num >= 1);
		assert(m_numElems >= 0);
		assert(m_maxElems >= m_numElems);
		assert(Index >= 0);
		assert(Index <= m_numElems);

		const sint32 OldNumElems = m_numElems;

		m_numElems += Num;
		if (m_numElems > m_maxElems)
		{
			ReAllocate(m_numElems);
		}

		memmove((uint8*)m_data + (Index+Num)*GetElemSize(), (uint8*)m_data + Index*GetElemSize(), (OldNumElems-Index)*GetElemSize());

		if (bZeroFill)
		{
			memset((uint8*)m_data + Index*GetElemSize(), 0, Num*GetElemSize());
		}
	}

	inline const bool IsValidIndex(const sint32 Index) const
	{
		return (Index >= 0 && Index < m_numElems);
	}

	inline TType &Top() { return Last(); }
	inline const TType &Top() const { return Last(); }

	inline TType &Last(const sint32 Offset=0) { return ((TType*)m_data)[m_numElems-Offset-1]; }
	inline const TType &Last(const sint32 Offset=0) const { return ((TType*)m_data)[m_numElems-Offset-1]; }

	TType Pop()
	{
		TType Result = ((TType*)m_data)[m_numElems-1];
		Remove(m_numElems-1);
		return Result;
	}

	inline sint32 Push(const TType &Item)
	{
		return AddItem(Item);
	}

	void Remove(const sint32 Start, const sint32 Count=1)
	{
		assert(Count >= 1 || (Count == 0 && m_numElems == 0));
		assert(Start <= m_numElems && (Start+Count) <= m_numElems);

		if (Count > 0)
		{
			if (dataType<TType>::NeedsCtor)
			{
				for (sint32 i=Start ; i<Start+Count ; i++)
					(&(*this)[i])->~TType();
			}

			memmove((uint8*)m_data + Start*GetElemSize(), (uint8*)m_data + (Start+Count)*GetElemSize(), (m_numElems-Start-Count) * GetElemSize());
			m_numElems -= Count;

			if (m_numElems > 0)
			{
				// Resize if possible
				if (m_numElems < m_maxElems)
				{
					ReAllocate(m_numElems);
				}
			}
			else
			{
				ReAllocate(0, false);
			}

			assert(m_numElems >= 0);
		}
	}

	void Reserve(const sint32 Num)
	{
		if (Num > m_maxElems)
		{
			ReAllocate(Num);
		}
	}

	void Shrink()
	{
		if (m_maxElems != m_numElems)
		{
			ReAllocate(m_numElems, false);
		}
	}

	void Swap(const sint32 Index1, const sint32 Index2)
	{
		assert(Index1 >= 0 && Index1 < m_numElems);
		assert(Index2 >= 0 && Index2 < m_numElems);

		if (Index1 != Index2)
		{
			const size_t Size = GetElemSize();
			void *Temp = dAlloca(Size);
			memcpy(Temp, (uint8*)m_data+Index1*GetElemSize(), Size);
			memcpy((uint8*)m_data+Index1*GetElemSize(), (uint8*)m_data+Index2*GetElemSize(), Size);
			memcpy((uint8*)m_data+Index2*GetElemSize(), Temp, Size);
		}
	}

	inline const size_t GetAllocSize() const { return m_maxElems * sizeof(TType); }
	inline const size_t GetElemSize() const { return sizeof(TType); }
	inline const size_t GetUsedSize() const { return m_numElems * sizeof(TType); }

	inline void *GetData() { return m_data; }
	inline const void *GetData() const{ return m_data; }

	inline TType *GetTypedData() { return m_data; }
	inline const TType *GetTypedData() const { return m_data; }

	inline const sint32 GetNum() const { return m_numElems; }
	inline const sint32 GetLastNum() const { return GetNum()>0 ? GetNum()-1 : 0; }

	class iterator
	{
	protected:
		dynArray<TType,TGranularity> &m_array;
		sint32 m_position;

		iterator() {}
		inline iterator operator =(const iterator &) {}

	public:
		iterator(dynArray<TType,TGranularity> &inArray, const sint32 StartingPosition=0)
		: m_array(inArray)
		, m_position(StartingPosition) {}

		~iterator() {}

		inline void operator++() { ++m_position; } // ++Prefix
		inline void operator++(sint32) { m_position++; } // Postfix++;
		inline TType &operator *() { return m_array[m_position]; }
		inline TType &operator->() { return m_array[m_position]; }
		inline operator bool() const { return m_array.IsValidIndex(m_position); }

		inline TType &Value() { return m_array[m_position]; }
	};
	class reverseIterator : public iterator
	{
	public:
		reverseIterator(dynArray<TType,TGranularity> &inArray)
		: iterator(inArray, inArray.GetLastNum()) {}

		~reverseIterator() {}

		inline void operator++() { --m_position; } // ++Prefix
		inline void operator++(sint32) { m_position--; } // Postfix++;
	};

	class constIterator
	{
	protected:
		dynArray<TType,TGranularity> &m_array;
		sint32 m_position;

		constIterator() {}
		inline constIterator operator =(const constIterator &) {}

	public:
		constIterator(const dynArray<TType,TGranularity> &inArray, const sint32 StartingPosition=0)
		: m_array(inArray)
		, m_position(StartingPosition) {}

		~constIterator() {}

		inline void operator++() { ++m_position; } // ++Prefix
		inline void operator++(sint32) { m_position++; } // Postfix++;
		inline const TType &operator *() const { return m_array[m_position]; }
		inline const TType &operator->() const { return m_array[m_position]; }
		inline operator bool() const { return m_array.IsValidIndex(m_position); }

		inline const TType &Value() const { return m_array[m_position]; }
	};
	class constReverseIterator : public constIterator
	{
	public:
		constReverseIterator(const dynArray<TType,TGranularity> &inArray)
		: constIterator(inArray, inArray.GetLastNum()) {}

		~constReverseIterator() {}

		inline void operator++() { --m_position; } // ++Prefix
		inline void operator++(sint32) { m_position--; } // Postfix++;
	};

protected:
	void ReAllocate(const sint32 NewMaxElems, const bool bAlign=true)
	{
		sint32 NewValue = NewMaxElems;
		if (bAlign && NewMaxElems > 0)
		{
			// FIXME: Test these other algos once there's a LOT more usage of this class
			//
			//const sint32 NewValue = Align<sint32>(NewMaxElems+TGranularity, TGranularity);
			//
			//const sint32 NewValue = Align<sint32>(NewMaxElems+(TGranularity-1), TGranularity);
			//
			//const sint32 NewValue = Align<sint32>(NewMaxElems+((TGranularity*2)-1), TGranularity);
			//
			//const sint32 NewValue = Align<sint32>(NewMaxElems+(TGranularity*2), TGranularity);
			NewValue = Align<sint32>(NewMaxElems, TGranularity);
		}

		if (m_maxElems == NewValue && m_data)
			return;

		if (m_data || NewValue)
		{
			extern void *_Mem_ReAlloc(void *ptr, size_t newSize, const char *fileName, const sint32 fileLine);
			m_data = (char*)_Mem_ReAlloc(m_data, NewValue * sizeof(TType), __FILE__, __LINE__);
			m_maxElems = NewValue;
		}
	}
};

// dynArray operator new/delete
template<class TType, const sint32 TGranularity>
static inline void *operator new(size_t Size, dynArray<TType,TGranularity> &Array)
{
	assert(Size == sizeof(TType));
	return &Array[Array.Add(1)];
}

template<class TType, const sint32 TGranularity>
static inline void *operator new(size_t Size, dynArray<TType,TGranularity> &Array, const sint32 Index)
{
	assert(Size == sizeof(TType));

	Array.Insert(Index, 1);
	return &Array[Index];
}

template<class TType, const sint32 TGranularity>
static inline void operator delete(void *MemPointer, dynArray<TType,TGranularity> &Array, const sint32 Index)
{
	assert(Size == sizeof(TType));
	Array.Remove(Index);
}

template<class TType>
static inline void *operator new(size_t Size, dynArray<TType> &List)
{
	assert(Size == sizeof(TType));
	return &List[List->Add(1)];
}
