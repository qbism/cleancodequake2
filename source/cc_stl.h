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

#if !defined(CC_GUARD_STL_H) || !INCLUDE_GUARDS
#define CC_GUARD_STL_H

#include <string>
#include <sstream>
#include <vector>
#include <list>
#include <map>
#include <algorithm>

// The following headers are required for all allocators.
#include <stddef.h>  // Required for size_t and ptrdiff_t and NULL
#include <new>       // Required for placement new and std::bad_alloc
#include <stdexcept> // Required for std::length_error

// In the name of code reusage!
#define CREATE_TEMPLATE_POOL_ALLOCATOR(className, pool) \
template <typename T> class className \
{ \
public: \
	typedef T * pointer; \
	typedef const T * const_pointer; \
	typedef T& reference; \
	typedef const T& const_reference; \
	typedef T value_type; \
	typedef size_t size_type; \
	typedef ptrdiff_t difference_type; \
	 \
	T * address(T& r) const \
	{ \
		return &r; \
	} \
	\
	const T * address(const T& s) const \
	{ \
		return &s; \
	} \
	\
	size_t max_size() const \
	{ \
		return (static_cast<size_t>(0) - static_cast<size_t>(1)) / sizeof(T); \
	} \
	\
	template <typename U> struct rebind \
	{ \
		typedef className<U> other; \
	}; \
	\
	bool operator!=(const className& Other) const \
	{ \
		return !(*this == Other); \
	} \
	\
	void construct(T * const p, const T& t) const \
	{ \
		void * const pv = static_cast<void *>(p); \
		new (pv) T(t); \
	} \
	\
	void destroy(T * const p) const \
	{ \
		p->~T(); \
	} \
	\
	bool operator==(const className& Other) const \
	{ \
		return true; \
	} \
	\
	className() \
	{ \
	} \
	\
	className(const className&) \
	{ \
	} \
	\
	template <typename U> className(const className<U>&) \
	{ \
	} \
	\
	~className() \
	{ \
	} \
	\
	T * allocate(const size_t n) const \
	{ \
		if (n == 0) \
		{ \
			return NULL; \
		} \
		\
		if (n > max_size()) \
			throw std::length_error("className<T>::allocate() - Integer overflow."); \
		\
		void * const pv = CC_Mem_Alloc (n * sizeof(T), pool, 0, "null", 0, false); \
		if (pv == NULL) \
			throw std::bad_alloc(); \
		\
		return static_cast<T *>(pv); \
	} \
	\
	void deallocate(T * const p, const size_t n) const \
	{ \
		QDelete p; \
	} \
	\
	template <typename U> T * allocate(const size_t n, const U * /* const hint */) const \
	{ \
		return allocate(n); \
	} \
	\
private: \
	className& operator=(const className&); \
};

CREATE_TEMPLATE_POOL_ALLOCATOR (generic_allocator, com_genericPool);
CREATE_TEMPLATE_POOL_ALLOCATOR (level_allocator, com_levelPool);
CREATE_TEMPLATE_POOL_ALLOCATOR (game_allocator, com_gamePool);
CREATE_TEMPLATE_POOL_ALLOCATOR (item_allocator, com_itemPool);
CREATE_TEMPLATE_POOL_ALLOCATOR (command_allocator, com_commandPool);
CREATE_TEMPLATE_POOL_ALLOCATOR (filesystem_allocator, com_fileSysPool);
CREATE_TEMPLATE_POOL_ALLOCATOR (write_allocator, com_writePool);
CREATE_TEMPLATE_POOL_ALLOCATOR (index_allocator, com_indexPool);
CREATE_TEMPLATE_POOL_ALLOCATOR (entity_allocator, com_entityPool);
CREATE_TEMPLATE_POOL_ALLOCATOR (test_allocator, com_testPool);

typedef std::basic_string<char, std::char_traits<char>, test_allocator<char> >
	cc_string;

typedef std::basic_stringbuf<char, std::char_traits<char>, generic_allocator<char> >
	cc_stringbuf;

typedef std::basic_stringstream <char, std::char_traits<char>, generic_allocator<char> >
	cc_stringstream;

inline void FormatString (cc_string &str, const char *fmt, ...)
{
	va_list		argptr;
	static char	text[2048];

	va_start (argptr, fmt);
	vsnprintf (text, sizeof(text), fmt, argptr);
	va_end (argptr);

	str.assign (text);
};

#else
FILE_WARNING
#endif

