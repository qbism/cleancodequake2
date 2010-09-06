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

// TTypes.h

#ifndef NULL
# define NULL ((void *)0)
#endif

#ifndef CONCAT
# define CONCAT(x,y) x##y
#endif

#ifndef BIT
# define BIT(x) (1U<<(x))
#endif

#ifndef BOOL
typedef sint32 BOOL;
#endif

#ifndef FALSE
# define FALSE 0
#endif

#ifndef TRUE
# define TRUE 1
#endif

// ===========================================================================

#ifdef _WIN64
typedef unsigned __int64	intptr;
#elif defined(__LP32__)
typedef uint32		intptr;
#else
typedef unsigned long		intptr;
#endif
