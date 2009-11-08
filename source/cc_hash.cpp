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
// cc_hash.cpp
// Duh
//

#include "cc_local.h"

/*
================
Com_HashGeneric
hashSize MUST be a power of two!

From EGL, used to make table lookups
a bit faster.
================
*/
uint32 Com_HashGeneric (const char *name, const sint32 hashSize)
{
	uint32 hashValue = 0;
	for ( ; *name ; )
	{
		sint32 ch = Q_tolower(*(name++));
		hashValue = hashValue * 33 + ch;
	}

	return (hashValue + (hashValue >> 5)) & (hashSize-1);
}

uint32 Com_HashGeneric (const std::cc_string &name, const sint32 hashSize)
{
	uint32 hashValue = 0;
	uint32 i = 0;
	for ( ; name[i] ; i++)
	{
		sint32 ch = Q_tolower(name[i]);
		hashValue = hashValue * 33 + ch;
	}

	return (hashValue + (hashValue >> 5)) & (hashSize-1);
}

//-----------------------------------------------------------------------------
// MurmurHash2, by Austin Appleby

// Note - This code makes a few assumptions about how your machine behaves -

// 1. We can read a 4-uint8 value from any address without crashing
// 2. sizeof(sint32) == 4

// And it has a few limitations -

// 1. It will not work incrementally.
// 2. It will not produce the same results on little-endian and big-endian
//    machines.

uint32 MurmurHash2 (const char *text, sint32 len, uint32 seed)
{
	// 'm' and 'r' are mixing constants generated offline.
	// They're not really 'magic', they just happen to work well.
	static const uint32 m = 0x5bd1e995;
	static const sint32 r = 24;

	// Initialize the hash to a 'random' value
	uint32 h = seed ^ len;
	const char *data = text;

	// Mix 4 bytes at a time into the hash
	while(len >= 4)
	{
		uint32 k = *(uint32 *)data;

		k *= m; 
		k ^= k >> r; 
		k *= m; 
		
		h *= m; 
		h ^= k;

		data += 4;
		len -= 4;
	}
	
	// Handle the last few bytes of the input array
	switch(len)
	{
	case 3: h ^= data[2] << 16;
	case 2: h ^= data[1] << 8;
	case 1: h ^= data[0];
	        h *= m;
	};

	// Do a few final mixes of the hash to ensure the last few
	// bytes are well-incorporated.
	h ^= h >> 13;
	h *= m;
	h ^= h >> 15;

	return h;
} 
