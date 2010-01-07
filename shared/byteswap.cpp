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
// byteswap.c
//

#include "../source/cc_options.h"
#include "shared.h"

float (*LittleFloat) (float f);
sint32 (*LittleLong) (sint32 l);
sint16 (*LittleShort) (sint16 s);
float (*BigFloat) (float f);
sint32 (*BigLong) (sint32 l);
sint16 (*BigShort) (sint16 s);

/*
===============
_FloatSwap
===============
*/
static float _FloatSwap (float f)
{
	union
	{
		uint8	b[4];
		float	f;
	} in, out;
	
	in.f = f;

	out.b[0] = in.b[3];
	out.b[1] = in.b[2];
	out.b[2] = in.b[1];
	out.b[3] = in.b[0];
	
	return out.f;
}


/*
===============
_FloatNoSwap
===============
*/
static float _FloatNoSwap (float f)
{
	return f;
}


/*
===============
_LongSwap
===============
*/
static sint32 _LongSwap (sint32 l)
{
	union
	{
		uint8	b[4];
		sint32		l;
	} in, out;

	in.l = l;

	out.b[0] = in.b[3];
	out.b[1] = in.b[2];
	out.b[2] = in.b[1];
	out.b[3] = in.b[0];

	return out.l;
}


/*
===============
_LongNoSwap
===============
*/
static sint32 _LongNoSwap (sint32 l)
{
	return l;
}


/*
===============
_ShortSwap
===============
*/
static sint16 _ShortSwap (sint16 s)
{
	union
	{
		uint8	b[2];
		sint16	s;
	} in, out;

	in.s = s;

	out.b[0] = in.b[1];
	out.b[1] = in.b[0];

	return out.s;
}


/*
===============
_ShortNoSwap
===============
*/
static sint16 _ShortNoSwap (sint16 s)
{
	return s;
}


/*
===============
Swap_Init
===============
*/
void Swap_Init ()
{
	uint8	swapTest[2] = { 1, 0 };

	if (*(sint16 *)swapTest == 1)
	{
		// Little endian
		LittleFloat = _FloatNoSwap;
		LittleLong = _LongNoSwap;
		LittleShort = _ShortNoSwap;
		BigFloat = _FloatSwap;
		BigLong = _LongSwap;
		BigShort = _ShortSwap;
	}
	else
	{
		// Big endian
		LittleFloat = _FloatSwap;
		LittleLong = _LongSwap;
		LittleShort = _ShortSwap;
		BigFloat = _FloatNoSwap;
		BigLong = _LongNoSwap;
		BigShort = _ShortNoSwap;
	}
}

