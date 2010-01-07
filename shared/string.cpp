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
// string.c
//

#include "../source/cc_local.h"

/*
============================================================================

	LIBRARY REPLACEMENT FUNCTIONS

============================================================================
*/

/*
===============
Q_snprintfz
===============
*/
void Q_snprintfz (char *dest, size_t size, const char *fmt, ...)
{
	if (size) {
		va_list		argptr;

		va_start (argptr, fmt);
		vsnprintf (dest, size, fmt, argptr);
		va_end (argptr);

		dest[size-1] = '\0';
	}
}

/*
===============
Q_strcatz
===============
*/
void Q_strcatz (char *dst, const char *src, size_t dstSize)
{
	size_t len = strlen (dst);
	if (len >= dstSize)
	{
		DebugPrintf ("Q_strcatz: already overflowed");
		return;
	}

	Q_strncpyz (dst + len, src, dstSize - len);
}


/*
===============
Q_strncpyz
===============
*/
size_t Q_strncpyz(char *dest, const char *src, size_t size)
{
	if (size)
	{
		while (--size && ((*dest++ = *src++) != 0)) ;
		*dest = '\0';
	}

	return size;
}

/*
===============
Q_tolower

by R1CH
===============
*/
#if defined(id386) && ((!defined(MSVS_VERSION) && defined(CC_STDC_CONFORMANCE)) || !defined(CC_STDC_CONFORMANCE))
__declspec(naked) sint32 __cdecl Q_tolower (sint32 c)
{
	__asm {
			mov eax, [esp+4]		;get character
			cmp	eax, 5Ah
			ja  short finish1

			cmp	eax, 41h
			jb  short finish1

			or  eax, 00100000b		;to lower (-32)
		finish1:
			ret	
	}
}
#endif // id386

// =========================================================================

/*
============
Q_WildcardMatch

from Q2ICE
============
*/
sint32 Q_WildcardMatch (const char *filter, const char *string, sint32 ignoreCase)
{
	switch (*filter) {
	case '\0':	return !*string;
	case '*':	return Q_WildcardMatch (filter + 1, string, ignoreCase) || *string && Q_WildcardMatch (filter, string + 1, ignoreCase);
	case '?':	return *string && Q_WildcardMatch (filter + 1, string + 1, ignoreCase);
	default:	return (*filter == *string || (ignoreCase && toupper (*filter) == toupper (*string))) && Q_WildcardMatch (filter + 1, string + 1, ignoreCase);
	}
}


/*
============
Q_VarArgs

Does a varargs printf into a temp buffer, so I don't need to have
varargs versions of all text functions.
============
*/
std::cc_string Q_VarArgs (char *format, ...)
{
	va_list		argptr;
	static char	string[1024];

	va_start (argptr, format);
	vsnprintf (string, 1024, format, argptr);
	va_end (argptr);

	return string;
}

