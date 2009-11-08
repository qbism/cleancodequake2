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

namespace std
{

// The following headers are required for all allocators.
#include <stddef.h>  // Required for size_t and ptrdiff_t and NULL
#include <new>       // Required for placement new and std::bad_alloc
#include <stdexcept> // Required for std::length_error

template <typename T> class generic_allocator
{
public:
	// The following will be the same for virtually all allocators.
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
		// The following has been carefully written to be independent of
		// the definition of size_t and to avoid signed/unsigned warnings.
		return (static_cast<size_t>(0) - static_cast<size_t>(1)) / sizeof(T);
	}

	// The following must be the same for all allocators.
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

	void destroy(T * const p) const; // Defined below.

	// Returns true if and only if storage allocated from *this
	// can be deallocated from other, and vice versa.
	// Always returns true for stateless allocators.
	bool operator==(const generic_allocator& other) const
	{
		return true;
	}

	// Default constructor, copy constructor, rebinding constructor, and destructor.
	// Empty for stateless allocators.
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

	// The following will be different for each allocator.
	T * allocate(const size_t n) const
	{
		// generic_allocator prints a diagnostic message to demonstrate
		// what it's doing. Real allocators won't do this.
		// std::cout << "Allocating " << n << (n == 1 ? " object" : "objects")
		//     << " of size " << sizeof(T) << "." << std::endl;

		// The return value of allocate(0) is unspecified.
		// generic_allocator returns NULL in order to avoid depending
		// on malloc(0)'s implementation-defined behavior
		// (the implementation can define malloc(0) to return NULL,
		// in which case the bad_alloc check below would fire).
		// All allocators can return NULL in this case.
		if (n == 0)
		{
			return NULL;
		}

		// All allocators should contain an integer overflow check.
		// The Standardization Committee recommends that std::length_error
		// be thrown in the case of integer overflow.
		if (n > max_size())
		{
			throw std::length_error("generic_allocator<T>::allocate() - Integer overflow.");
		}

		// generic_allocator wraps malloc().
		void * const pv = _Mem_Alloc (n * sizeof(T), com_genericPool, 0, "null", 0);

		// Allocators should throw std::bad_alloc in the case of memory allocation failure.
		if (pv == NULL)
		{
			throw std::bad_alloc();
		}

		return static_cast<T *>(pv);
	}

	void deallocate(T * const p, const size_t n) const
	{
		// generic_allocator prints a diagnostic message to demonstrate
		// what it's doing. Real allocators won't do this.
//		std::cout << "Deallocating " << n << (n == 1 ? " object" : "objects")
//			<< " of size " << sizeof(T) << "." << std::endl;

		// generic_allocator wraps free().
		QDelete p;
	}

	// The following will be the same for all allocators that ignore hints.
	template <typename U> T * allocate(const size_t n, const U * /* const hint */) const
	{
		return allocate(n);
	}


	// Allocators are not required to be assignable, so
	// all allocators should have a private unimplemented
	// assignment operator. Note that this will trigger the
	// off-by-default (enabled under /Wall) warning C4626
	// "assignment operator could not be generated because a
	// base class assignment operator is inaccessible" within
	// the STL headers, but that warning is useless.
private:
	generic_allocator& operator=(const generic_allocator&);
};

// A compiler bug causes it to believe that p->~T() doesn't reference p.
// The definition of destroy() must be the same for all allocators.
template <typename T> void generic_allocator<T>::destroy(T * const p) const
{
	p->~T();
}

template <typename T> class level_allocator
{
public:
	// The following will be the same for virtually all allocators.
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
		// The following has been carefully written to be independent of
		// the definition of size_t and to avoid signed/unsigned warnings.
		return (static_cast<size_t>(0) - static_cast<size_t>(1)) / sizeof(T);
	}

	// The following must be the same for all allocators.
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

	void destroy(T * const p) const; // Defined below.

	// Returns true if and only if storage allocated from *this
	// can be deallocated from other, and vice versa.
	// Always returns true for stateless allocators.
	bool operator==(const level_allocator& other) const
	{
		return true;
	}

	// Default constructor, copy constructor, rebinding constructor, and destructor.
	// Empty for stateless allocators.
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

	// The following will be different for each allocator.
	T * allocate(const size_t n) const
	{
		// level_allocator prints a diagnostic message to demonstrate
		// what it's doing. Real allocators won't do this.
		// std::cout << "Allocating " << n << (n == 1 ? " object" : "objects")
		//     << " of size " << sizeof(T) << "." << std::endl;

		// The return value of allocate(0) is unspecified.
		// level_allocator returns NULL in order to avoid depending
		// on malloc(0)'s implementation-defined behavior
		// (the implementation can define malloc(0) to return NULL,
		// in which case the bad_alloc check below would fire).
		// All allocators can return NULL in this case.
		if (n == 0)
		{
			return NULL;
		}

		// All allocators should contain an integer overflow check.
		// The Standardization Committee recommends that std::length_error
		// be thrown in the case of integer overflow.
		if (n > max_size())
		{
			throw std::length_error("level_allocator<T>::allocate() - Integer overflow.");
		}

		// level_allocator wraps malloc().
		void * const pv = _Mem_Alloc (n * sizeof(T), com_levelPool, 0, "null", 0);

		// Allocators should throw std::bad_alloc in the case of memory allocation failure.
		if (pv == NULL)
		{
			throw std::bad_alloc();
		}

		return static_cast<T *>(pv);
	}

	void deallocate(T * const p, const size_t n) const
	{
		// level_allocator prints a diagnostic message to demonstrate
		// what it's doing. Real allocators won't do this.
//		std::cout << "Deallocating " << n << (n == 1 ? " object" : "objects")
//			<< " of size " << sizeof(T) << "." << std::endl;

		// level_allocator wraps free().
		QDelete p;
	}

	// The following will be the same for all allocators that ignore hints.
	template <typename U> T * allocate(const size_t n, const U * /* const hint */) const
	{
		return allocate(n);
	}


	// Allocators are not required to be assignable, so
	// all allocators should have a private unimplemented
	// assignment operator. Note that this will trigger the
	// off-by-default (enabled under /Wall) warning C4626
	// "assignment operator could not be generated because a
	// base class assignment operator is inaccessible" within
	// the STL headers, but that warning is useless.
private:
	level_allocator& operator=(const level_allocator&);
};

// A compiler bug causes it to believe that p->~T() doesn't reference p.
// The definition of destroy() must be the same for all allocators.
template <typename T> void level_allocator<T>::destroy(T * const p) const
{
	p->~T();
}

template <typename T> class game_allocator
{
public:
	// The following will be the same for virtually all allocators.
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
		// The following has been carefully written to be independent of
		// the definition of size_t and to avoid signed/unsigned warnings.
		return (static_cast<size_t>(0) - static_cast<size_t>(1)) / sizeof(T);
	}

	// The following must be the same for all allocators.
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

	void destroy(T * const p) const; // Defined below.

	// Returns true if and only if storage allocated from *this
	// can be deallocated from other, and vice versa.
	// Always returns true for stateless allocators.
	bool operator==(const game_allocator& other) const
	{
		return true;
	}

	// Default constructor, copy constructor, rebinding constructor, and destructor.
	// Empty for stateless allocators.
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

	// The following will be different for each allocator.
	T * allocate(const size_t n) const
	{
		// game_allocator prints a diagnostic message to demonstrate
		// what it's doing. Real allocators won't do this.
		// std::cout << "Allocating " << n << (n == 1 ? " object" : "objects")
		//     << " of size " << sizeof(T) << "." << std::endl;

		// The return value of allocate(0) is unspecified.
		// game_allocator returns NULL in order to avoid depending
		// on malloc(0)'s implementation-defined behavior
		// (the implementation can define malloc(0) to return NULL,
		// in which case the bad_alloc check below would fire).
		// All allocators can return NULL in this case.
		if (n == 0)
		{
			return NULL;
		}

		// All allocators should contain an integer overflow check.
		// The Standardization Committee recommends that std::length_error
		// be thrown in the case of integer overflow.
		if (n > max_size())
		{
			throw std::length_error("game_allocator<T>::allocate() - Integer overflow.");
		}

		// game_allocator wraps malloc().
		void * const pv = _Mem_Alloc (n * sizeof(T), com_gamePool, 0, "null", 0);

		// Allocators should throw std::bad_alloc in the case of memory allocation failure.
		if (pv == NULL)
		{
			throw std::bad_alloc();
		}

		return static_cast<T *>(pv);
	}

	void deallocate(T * const p, const size_t n) const
	{
		// game_allocator prints a diagnostic message to demonstrate
		// what it's doing. Real allocators won't do this.
//		std::cout << "Deallocating " << n << (n == 1 ? " object" : "objects")
//			<< " of size " << sizeof(T) << "." << std::endl;

		// game_allocator wraps free().
		QDelete p;
	}

	// The following will be the same for all allocators that ignore hints.
	template <typename U> T * allocate(const size_t n, const U * /* const hint */) const
	{
		return allocate(n);
	}


	// Allocators are not required to be assignable, so
	// all allocators should have a private unimplemented
	// assignment operator. Note that this will trigger the
	// off-by-default (enabled under /Wall) warning C4626
	// "assignment operator could not be generated because a
	// base class assignment operator is inaccessible" within
	// the STL headers, but that warning is useless.
private:
	game_allocator& operator=(const game_allocator&);
};

// A compiler bug causes it to believe that p->~T() doesn't reference p.
// The definition of destroy() must be the same for all allocators.
template <typename T> void game_allocator<T>::destroy(T * const p) const
{
	p->~T();
}

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