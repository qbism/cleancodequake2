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

/**
\fn	void Q_snprintfz(char *dest, size_t size, const char *fmt, ...)

\brief	Replacement for snprintf. Fills 'dest' up to 'size' with the format 'fmt'. 

\author	Paril
\date	26/05/2010

\param [in,out]	dest	If non-null, destination for the string. 
\param	size			The size. 
\param	fmt				Describes the format to use. 
**/
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

/**
\fn	void Q_strcatz(char *dst, const char *src, size_t dstSize)

\brief	Replacement for strcat. Concatenates 'src' into 'dst' up to 'dstSize' bytes.

\author	Paril
\date	26/05/2010

\param [in,out]	dst	If non-null, destination for the string. 
\param	src			Source for the string. 
\param	dstSize		Size of the destination. 
**/
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

/**
\fn	size_t Q_strncpyz(char *dest, const char *src, size_t size)

\brief	Replacement for strncpy. Copies 'src' into 'dest' up to 'size' bytes

\author	Paril
\date	26/05/2010

\param [in,out]	dest	If non-null, destination for the string to be copied into. 
\param	src				Source for the string. 
\param	size			The size. 

\return	. 
**/
size_t Q_strncpyz(char *dest, const char *src, size_t size)
{
	if (size)
	{
		while (--size && ((*dest++ = *src++) != 0)) ;
		*dest = '\0';
	}

	return size;
}


