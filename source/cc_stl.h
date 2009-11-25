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
Foundation, Inc., 59 Temple Place - Suite 330, Boston, MA  02111-1307, USA.
*/
/*
This source file is contained as part of CleanCode Quake2, a project maintained
by Paril, to 'clean up' and make Quake2 an easier source base to read and work with.

You may use any part of this code to help create your own bases and own mods off
this code if you wish. It is under the same license as Quake 2 source (as above),
therefore you are free to have to fun with it. All I ask is you email me so I can
list the mod on my page for CleanCode Quake2 to help get the word around. Thanks.
*/

//
// cc_stl.h
// 
//

#if !defined(__CC_STL_H__) || !defined(INCLUDE_GUARDS)
#define __CC_STL_H__

#include <string>
#include <vector>
#include <list>
#include <map>
#include <algorithm>

namespace std
{

// The following headers are required for all allocators.
#include <stddef.h>  // Required for size_t and ptrdiff_t and NULL
#include <new>       // Required for placement new and std::bad_alloc
#include <stdexcept> // Required for std::length_error

template <typename T> class generic_allocator
{
public:
	typedef T * pointer;
	typedef const T * const_pointer;
	typedef T& reference;
	typedef const T& const_reference;
	typedef T value_type;
	typedef size_t size_type;
	typedef ptrdiff_t difference_type;

	T * address(T& r) const
	{
		return &r;
	}

	const T * address(const T& s) const
	{
		return &s;
	}

	size_t max_size() const
	{
		return (static_cast<size_t>(0) - static_cast<size_t>(1)) / sizeof(T);
	}

	template <typename U> struct rebind
	{
		typedef generic_allocator<U> other;
	};

	bool operator!=(const generic_allocator& other) const
	{
		return !(*this == other);
	}

	void construct(T * const p, const T& t) const
	{
		void * const pv = static_cast<void *>(p);
		new (pv) T(t);
	}

	void destroy(T * const p) const
	{
		p->~T();
	}

	bool operator==(const generic_allocator& other) const
	{
		return true;
	}

	generic_allocator()
	{
	}

	generic_allocator(const generic_allocator&)
	{
	}

	template <typename U> generic_allocator(const generic_allocator<U>&)
	{
	}

	~generic_allocator()
	{
	}

	T * allocate(const size_t n) const
	{
		if (n == 0)
		{
			return NULL;
		}

		if (n > max_size())
			throw std::length_error("generic_allocator<T>::allocate() - Integer overflow.");

		void * const pv = _Mem_Alloc (n * sizeof(T), com_genericPool, 0, "null", 0);
		if (pv == NULL)
			throw std::bad_alloc();

		return static_cast<T *>(pv);
	}

	void deallocate(T * const p, const size_t n) const
	{
		QDelete p;
	}

	template <typename U> T * allocate(const size_t n, const U * /* const hint */) const
	{
		return allocate(n);
	}

private:
	generic_allocator& operator=(const generic_allocator&);
};

template <typename T> class level_allocator
{
public:
	typedef T * pointer;
	typedef const T * const_pointer;
	typedef T& reference;
	typedef const T& const_reference;
	typedef T value_type;
	typedef size_t size_type;
	typedef ptrdiff_t difference_type;

	T * address(T& r) const
	{
		return &r;
	}

	const T * address(const T& s) const
	{
		return &s;
	}

	size_t max_size() const
	{
		return (static_cast<size_t>(0) - static_cast<size_t>(1)) / sizeof(T);
	}

	template <typename U> struct rebind
	{
		typedef level_allocator<U> other;
	};

	bool operator!=(const level_allocator& other) const
	{
		return !(*this == other);
	}

	void construct(T * const p, const T& t) const
	{
		void * const pv = static_cast<void *>(p);
		new (pv) T(t);
	}

	void destroy(T * const p) const
	{
		p->~T();
	}

	bool operator==(const level_allocator& other) const
	{
		return true;
	}

	level_allocator()
	{
	}

	level_allocator(const level_allocator&)
	{
	}

	template <typename U> level_allocator(const level_allocator<U>&)
	{
	}

	~level_allocator()
	{
	}

	T * allocate(const size_t n) const
	{
		if (n == 0)
			return NULL;

		if (n > max_size())
			throw std::length_error("level_allocator<T>::allocate() - Integer overflow.");

		void * const pv = _Mem_Alloc (n * sizeof(T), com_levelPool, 0, "null", 0);
		if (pv == NULL)
			throw std::bad_alloc();

		return static_cast<T *>(pv);
	}

	void deallocate(T * const p, const size_t n) const
	{
		QDelete p;
	}

	template <typename U> T * allocate(const size_t n, const U * /* const hint */) const
	{
		return allocate(n);
	}

private:
	level_allocator& operator=(const level_allocator&);
};

template <typename T> class game_allocator
{
public:
	typedef T * pointer;
	typedef const T * const_pointer;
	typedef T& reference;
	typedef const T& const_reference;
	typedef T value_type;
	typedef size_t size_type;
	typedef ptrdiff_t difference_type;

	T * address(T& r) const
	{
		return &r;
	}

	const T * address(const T& s) const
	{
		return &s;
	}

	size_t max_size() const
	{
		return (static_cast<size_t>(0) - static_cast<size_t>(1)) / sizeof(T);
	}

	template <typename U> struct rebind
	{
		typedef game_allocator<U> other;
	};

	bool operator!=(const game_allocator& other) const
	{
		return !(*this == other);
	}

	void construct(T * const p, const T& t) const
	{
		void * const pv = static_cast<void *>(p);
		new (pv) T(t);
	}

	void destroy(T * const p) const
	{
		p->~T();
	}

	bool operator==(const game_allocator& other) const
	{
		return true;
	}

	game_allocator()
	{
	}

	game_allocator(const game_allocator&)
	{
	}

	template <typename U> game_allocator(const game_allocator<U>&)
	{
	}

	~game_allocator()
	{
	}

	T * allocate(const size_t n) const
	{
		if (n == 0)
			return NULL;

		if (n > max_size())
			throw std::length_error("game_allocator<T>::allocate() - Integer overflow.");

		void * const pv = _Mem_Alloc (n * sizeof(T), com_gamePool, 0, "null", 0);
		if (pv == NULL)
			throw std::bad_alloc();

		return static_cast<T *>(pv);
	}

	void deallocate(T * const p, const size_t n) const
	{
		QDelete p;
	}

	template <typename U> T * allocate(const size_t n, const U * /* const hint */) const
	{
		return allocate(n);
	}

private:
	game_allocator& operator=(const game_allocator&);
};

typedef basic_string<char, char_traits<char>, generic_allocator<char> >
	cc_string;

typedef basic_stringbuf<char, char_traits<char>, generic_allocator<char> >
	cc_stringbuf;

typedef basic_stringstream <char, char_traits<char>, generic_allocator<char> >
	cc_stringstream;

inline void FormatString (std::cc_string &str, const char *fmt, ...)
{
	va_list		argptr;
	static char	text[2048];

	va_start (argptr, fmt);
	vsnprintf_s (text, sizeof(text), 2048, fmt, argptr);
	va_end (argptr);

	str.assign (text);
};
};

#else
FILE_WARNING
#endif