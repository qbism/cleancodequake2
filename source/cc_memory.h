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

//
// cc_memory.h
//

#if !defined(CC_GUARD_MEMORY_H) || !INCLUDE_GUARDS
#define CC_GUARD_MEMORY_H

#include <exception>

/**
\typedef	sint16 EMemoryTag

\brief	Defines an alias representing the memory tags.
**/
typedef sint16 EMemoryTag;

/**
\enum	

\brief	Values that represent memory tags. 
**/
enum
{
	TAG_GENERIC = -1,
	TAG_GAME	= 765,
	TAG_LEVEL,
	TAG_ENTITY
};

char						*Mem_TagStrDup (const char *in, const sint32 tagNum);

inline char *Mem_StrDup(const char *in)
{
	return Mem_TagStrDup(in, TAG_GENERIC);
}

void *operator new(size_t Size, const sint32 TagNum);
void *operator new[](size_t Size, const sint32 TagNum);
void operator delete(void *Pointer, const sint32 TagNum);
void operator delete[](void *Pointer, const sint32 TagNum);

void Mem_FreeTag (const sint32 TagNum);

#define QNew(TagNum)	new((TagNum))
#define QDelete	delete

inline void Mem_Zero (void *ptr, size_t size)
{
	memset (ptr, 0, size);
}

#else
FILE_WARNING
#endif

