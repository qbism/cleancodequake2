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
// Events.h
// A C#-style delegate event class
//

#if !defined(CC_GUARD_EVENTS_H) || !INCLUDE_GUARDS
#define CC_GUARD_EVENTS_H

template <typename T,
	typename Arg1 = bool,
	typename Arg2 = bool,
	typename Arg3 = bool,
	typename Arg4 = bool,
	typename Arg5 = bool,
	typename Arg6 = bool,
	typename Arg7 = bool,
	typename Arg8 = bool
	>
class Event
{
protected:
	std::vector<T> functions;

public:
	void operator += (T functor) { functions.push_back(functor); }
	void operator -= (T functor) { functions.remove(functor); }

	void operator() ()
	{
		for (size_t i = 0; i < functions.size(); ++i)
			functions[i]();
	}

	void operator() (Arg1 arg1)
	{
		for (size_t i = 0; i < functions.size(); ++i)
			functions[i](arg1);
	}
	
	void operator() (Arg1 arg1, Arg2 arg2)
	{
		for (size_t i = 0; i < functions.size(); ++i)
			functions[i](arg1, arg2);
	}

	void operator() (Arg1 arg1, Arg2 arg2, Arg3 arg3)
	{
		for (size_t i = 0; i < functions.size(); ++i)
			functions[i](arg1, arg2, arg3);
	}
	
	void operator() (Arg1 arg1, Arg2 arg2, Arg3 arg3, Arg4 arg4)
	{
		for (size_t i = 0; i < functions.size(); ++i)
			functions[i](arg1, arg2, arg3, arg4);
	}

	void operator() (Arg1 arg1, Arg2 arg2, Arg3 arg3, Arg4 arg4, Arg5 arg5)
	{
		for (size_t i = 0; i < functions.size(); ++i)
			functions[i](arg1, arg2, arg3, arg4, arg5);
	}

	void operator() (Arg1 arg1, Arg2 arg2, Arg3 arg3, Arg4 arg4, Arg5 arg5, Arg6 arg6)
	{
		for (size_t i = 0; i < functions.size(); ++i)
			functions[i](arg1, arg2, arg3, arg4, arg5, arg6);
	}

	void operator() (Arg1 arg1, Arg2 arg2, Arg3 arg3, Arg4 arg4, Arg5 arg5, Arg6 arg6, Arg7 arg7)
	{
		for (size_t i = 0; i < functions.size(); ++i)
			functions[i](arg1, arg2, arg3, arg4, arg5, arg6, arg7);
	}

	void operator() (Arg1 arg1, Arg2 arg2, Arg3 arg3, Arg4 arg4, Arg5 arg5, Arg6 arg6, Arg7 arg7, Arg8 arg8)
	{
		for (size_t i = 0; i < functions.size(); ++i)
			functions[i](arg1, arg2, arg3, arg4, arg5, arg6, arg7, arg8);
	}
};

#else
FILE_WARNING
#endif