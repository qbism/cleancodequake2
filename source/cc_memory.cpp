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

CC_DISABLE_DEPRECATION

#define HEADER_MAGIC_CONSTANT (('E'<<24)+('N'<<16)+('E'<<8)+'G')

struct MemSentinel
{
	uint32				Magic;
	void				*Header;

	bool Check (void *CheckHeader)
	{
		return (Magic == HEADER_MAGIC_CONSTANT && Header == CheckHeader);
	};
};

struct MemHeader
{
	MemSentinel			SentinelHeader;
	void				*Address;
	sint32				TagNum;
	size_t				Size, RealSize;

	bool Check ()
	{
		return (SentinelHeader.Check(this) && ((MemSentinel*)(((byte*)this) + RealSize - sizeof(MemSentinel)))->Check(this));
	};
};

static void *Mem_TagAlloc (size_t Size, const sint32 TagNum)
{
	size_t RealSize = Size + sizeof(MemHeader) + sizeof(MemSentinel);
	MemHeader *Mem = (MemHeader*)((TagNum == TAG_GENERIC) ? malloc(RealSize) : gi.TagMalloc(RealSize, TagNum));
	MemSentinel *Footer = (MemSentinel*)(((byte*)Mem) + RealSize - sizeof(MemSentinel));

	Mem->SentinelHeader.Header = Footer->Header = Mem;
	Mem->TagNum = TagNum;
	Mem->Size = Size;
	Footer->Magic = Mem->SentinelHeader.Magic = HEADER_MAGIC_CONSTANT;
	Mem->RealSize = RealSize;
	Mem->Address = (((byte*)Mem) + sizeof(MemHeader));
	Mem_Zero (Mem->Address, Size);

	return Mem->Address;
}

static void Mem_TagFree (void *Pointer)
{
	MemHeader *Header = (MemHeader*)(((byte*)Pointer) - sizeof(MemHeader));

	if (!Header->Check())
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
CC_ENABLE_DEPRECATION

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
		CC_ASSERT_EXPR (0, "Attempted to free NULL");
		return;
	}

	Mem_TagFree (Pointer);
	TagNum;
}

void operator delete[](void *Pointer, const sint32 TagNum)
{
	if (Pointer == NULL)
	{
		CC_ASSERT_EXPR (0, "Attempted to free NULL");
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
