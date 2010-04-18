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
// cc_memory.cpp
// 
//

#include "cc_local.h"

_CC_DISABLE_DEPRECATION

#define MAGIC_NUMBER (('E'<<24)+('N'<<16)+('E'<<8)+'G')

struct MemHeader
{
	void		*Address;
	uint32		Magic;
	sint32		TagNum;
	size_t		Size, RealSize;
};

static void *Mem_TagAlloc (size_t Size, const sint32 TagNum)
{
/*	void *Mem = (TagNum == TAG_GENERIC) ? malloc(Size + sizeof(int)) : gi.TagMalloc(Size + sizeof(int), TagNum);

	Mem_Zero (Mem, Size + sizeof(int));

	// Set the first byte to 255 if we're Generic memory, otherwise 0.
	*(int*)Mem = (TagNum == TAG_GENERIC) ? MAGIC_NUMBER : 0;

	return (void*)((uint8*)Mem + sizeof(int));*/
	size_t RealSize = Size + sizeof(MemHeader);
	MemHeader *Mem = (TagNum == TAG_GENERIC) ? (MemHeader*)malloc(RealSize) : (MemHeader*)gi.TagMalloc(RealSize, TagNum);

	Mem_Zero (Mem, RealSize);

	Mem->Magic = MAGIC_NUMBER;
	Mem->TagNum = TagNum;
	Mem->Size = Size;
	Mem->RealSize = RealSize;
	Mem->Address = ((void*)(Mem + 1));

	return Mem->Address;
}

static void Mem_TagFree (void *Pointer)
{
/*	if (Pointer == NULL)
		return;

	uint8 *realMem = (uint8*)Pointer - sizeof(int);
	
	if (*(int*)realMem == MAGIC_NUMBER)
		free (realMem);
	else
		gi.TagFree (realMem);*/
	MemHeader *Header = (((MemHeader*)Pointer)-1);

	if (Header->Magic != MAGIC_NUMBER)
		assert (0);

	if (Header->TagNum == TAG_GENERIC)
		free (Header);
	else
		gi.TagFree (Header);
}

void Mem_FreeTag (const sint32 TagNum)
{
	gi.FreeTags (TagNum);
}
_CC_ENABLE_DEPRECATION

#ifdef WIN32
#pragma warning(disable : 4290)
#endif

void *operator new(size_t Size, const sint32 TagNum)
{
	return Mem_TagAlloc(Size, TagNum);
}

void *operator new[](size_t Size, const sint32 TagNum)
{
	return Mem_TagAlloc(Size, TagNum);
}

void operator delete(void *Pointer, const sint32 TagNum)
{
	if (Pointer == NULL)
	{
		_CC_ASSERT_EXPR (0, "Attempted to free NULL");
		return;
	}

	Mem_TagFree (Pointer);
	TagNum;
}

void operator delete[](void *Pointer, const sint32 TagNum)
{
	if (Pointer == NULL)
	{
		_CC_ASSERT_EXPR (0, "Attempted to free NULL");
		return;
	}

	Mem_TagFree (Pointer);
	TagNum;
}

void *operator new (size_t Size) throw (std::bad_alloc)
{
	return Mem_TagAlloc(Size, TAG_GENERIC);
}

void operator delete (void *Pointer) throw ()
{
	Mem_TagFree (Pointer);
}

/*
================
Mem_TagStrDup
================
*/
char *Mem_TagStrDup (const char *in, const sint32 tagNum)
{
	char	*out = QNew(tagNum) char[(size_t)(strlen (in) + 1)];
	Q_strncpyz (out, in, (size_t)(strlen (in) + 1));

	return out;
}
