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

namespace std
{
		// TEMPLATE CLASS allocator
template<class _Ty>
	class level_allocator
		: public _Allocator_base<_Ty>
	{	// generic allocator for objects of class _Ty
public:
	typedef _Allocator_base<_Ty> _Mybase;
	typedef typename _Mybase::value_type value_type;
	typedef value_type _FARQ *pointer;
	typedef value_type _FARQ& reference;
	typedef const value_type _FARQ *const_pointer;
	typedef const value_type _FARQ& const_reference;

	typedef _SIZT size_type;
	typedef _PDFT difference_type;

	template<class _Other>
		struct rebind
		{	// convert an allocator<_Ty> to an allocator <_Other>
		typedef level_allocator<_Other> other;
		};

	pointer address(reference _Val) const
		{	// return address of mutable _Val
		return (&_Val);
		}

	const_pointer address(const_reference _Val) const
		{	// return address of nonmutable _Val
		return (&_Val);
		}

	level_allocator() _THROW0()
		{	// construct default allocator (do nothing)
		}

	level_allocator(const level_allocator<_Ty>&) _THROW0()
		{	// construct by copying (do nothing)
		}

	template<class _Other>
		level_allocator(const level_allocator<_Other>&) _THROW0()
		{	// construct from a related allocator (do nothing)
		}

	template<class _Other>
		level_allocator<_Ty>& operator=(const level_allocator<_Other>&)
		{	// assign from a related allocator (do nothing)
		return (*this);
		}

	void deallocate(pointer _Ptr, size_type)
		{	// deallocate object at _Ptr, ignore size
		delete(_Ptr);
		}

	pointer allocate(size_type _Count)
		{	// allocate array of _Count elements
		// check for integer overflow
		if (_Count <= 0)
			_Count = 0;
		else if (((_SIZT)(-1) / _Count) < sizeof (_Ty))
			_THROW_NCEE(bad_alloc, NULL);

			// allocate storage for _Count elements of type _Ty
		return ((_Ty _FARQ *) _Mem_Alloc (_Count * sizeof (_Ty), com_levelPool, 0, "STL Container", 0));
			//return (_Allocate(_Count, (pointer)0));
		}

	pointer allocate(size_type _Count, const void _FARQ *)
		{	// allocate array of _Count elements, ignore hint
		return (allocate(_Count));
		}

	void construct(pointer _Ptr, const _Ty& _Val)
		{	// construct object at _Ptr with value _Val
		_Construct(_Ptr, _Val);
		}

	void destroy(pointer _Ptr)
		{	// destroy object at _Ptr
		_Destroy(_Ptr);
		}

	_SIZT max_size() const _THROW0()
		{	// estimate maximum array size
		_SIZT _Count = (_SIZT)(-1) / sizeof (_Ty);
		return (0 < _Count ? _Count : 1);
		}
	};

		// TEMPLATE CLASS allocator
template<class _Ty>
	class game_allocator
		: public _Allocator_base<_Ty>
	{	// generic allocator for objects of class _Ty
public:
	typedef _Allocator_base<_Ty> _Mybase;
	typedef typename _Mybase::value_type value_type;
	typedef value_type _FARQ *pointer;
	typedef value_type _FARQ& reference;
	typedef const value_type _FARQ *const_pointer;
	typedef const value_type _FARQ& const_reference;

	typedef _SIZT size_type;
	typedef _PDFT difference_type;

	template<class _Other>
		struct rebind
		{	// convert an allocator<_Ty> to an allocator <_Other>
		typedef game_allocator<_Other> other;
		};

	pointer address(reference _Val) const
		{	// return address of mutable _Val
		return (&_Val);
		}

	const_pointer address(const_reference _Val) const
		{	// return address of nonmutable _Val
		return (&_Val);
		}

	game_allocator() _THROW0()
		{	// construct default allocator (do nothing)
		}

	game_allocator(const game_allocator<_Ty>&) _THROW0()
		{	// construct by copying (do nothing)
		}

	template<class _Other>
		game_allocator(const game_allocator<_Other>&) _THROW0()
		{	// construct from a related allocator (do nothing)
		}

	template<class _Other>
		game_allocator<_Ty>& operator=(const game_allocator<_Other>&)
		{	// assign from a related allocator (do nothing)
		return (*this);
		}

	void deallocate(pointer _Ptr, size_type)
		{	// deallocate object at _Ptr, ignore size
		delete(_Ptr);
		}

	pointer allocate(size_type _Count)
		{	// allocate array of _Count elements
		// check for integer overflow
		if (_Count <= 0)
			_Count = 0;
		else if (((_SIZT)(-1) / _Count) < sizeof (_Ty))
			_THROW_NCEE(bad_alloc, NULL);

			// allocate storage for _Count elements of type _Ty
		return ((_Ty _FARQ *) _Mem_Alloc (_Count * sizeof (_Ty), com_gamePool, 0, "STL Container", 0));
			//return (_Allocate(_Count, (pointer)0));
		}

	pointer allocate(size_type _Count, const void _FARQ *)
		{	// allocate array of _Count elements, ignore hint
		return (allocate(_Count));
		}

	void construct(pointer _Ptr, const _Ty& _Val)
		{	// construct object at _Ptr with value _Val
		_Construct(_Ptr, _Val);
		}

	void destroy(pointer _Ptr)
		{	// destroy object at _Ptr
		_Destroy(_Ptr);
		}

	_SIZT max_size() const _THROW0()
		{	// estimate maximum array size
		_SIZT _Count = (_SIZT)(-1) / sizeof (_Ty);
		return (0 < _Count ? _Count : 1);
		}
	};

		// TEMPLATE CLASS allocator
template<class _Ty>
	class generic_allocator
		: public _Allocator_base<_Ty>
	{	// generic allocator for objects of class _Ty
public:
	typedef _Allocator_base<_Ty> _Mybase;
	typedef typename _Mybase::value_type value_type;
	typedef value_type _FARQ *pointer;
	typedef value_type _FARQ& reference;
	typedef const value_type _FARQ *const_pointer;
	typedef const value_type _FARQ& const_reference;

	typedef _SIZT size_type;
	typedef _PDFT difference_type;

	template<class _Other>
		struct rebind
		{	// convert an allocator<_Ty> to an allocator <_Other>
		typedef generic_allocator<_Other> other;
		};

	pointer address(reference _Val) const
		{	// return address of mutable _Val
		return (&_Val);
		}

	const_pointer address(const_reference _Val) const
		{	// return address of nonmutable _Val
		return (&_Val);
		}

	generic_allocator() _THROW0()
		{	// construct default allocator (do nothing)
		}

	generic_allocator(const generic_allocator<_Ty>&) _THROW0()
		{	// construct by copying (do nothing)
		}

	template<class _Other>
		generic_allocator(const generic_allocator<_Other>&) _THROW0()
		{	// construct from a related allocator (do nothing)
		}

	template<class _Other>
		generic_allocator<_Ty>& operator=(const generic_allocator<_Other>&)
		{	// assign from a related allocator (do nothing)
		return (*this);
		}

	void deallocate(pointer _Ptr, size_type)
		{	// deallocate object at _Ptr, ignore size
		delete(_Ptr);
		}

	pointer allocate(size_type _Count)
		{	// allocate array of _Count elements
		// check for integer overflow
		if (_Count <= 0)
			_Count = 0;
		else if (((_SIZT)(-1) / _Count) < sizeof (_Ty))
			_THROW_NCEE(bad_alloc, NULL);

			// allocate storage for _Count elements of type _Ty
		return ((_Ty _FARQ *) _Mem_Alloc (_Count * sizeof (_Ty), com_genericPool, 0, "STL Container", 0));
			//return (_Allocate(_Count, (pointer)0));
		}

	pointer allocate(size_type _Count, const void _FARQ *)
		{	// allocate array of _Count elements, ignore hint
		return (allocate(_Count));
		}

	void construct(pointer _Ptr, const _Ty& _Val)
		{	// construct object at _Ptr with value _Val
		_Construct(_Ptr, _Val);
		}

	void destroy(pointer _Ptr)
		{	// destroy object at _Ptr
		_Destroy(_Ptr);
		}

	_SIZT max_size() const _THROW0()
		{	// estimate maximum array size
		_SIZT _Count = (_SIZT)(-1) / sizeof (_Ty);
		return (0 < _Count ? _Count : 1);
		}
	};

typedef basic_string<char, char_traits<char>, generic_allocator<char> >
	cc_string;
};

#else
FILE_WARNING
#endif