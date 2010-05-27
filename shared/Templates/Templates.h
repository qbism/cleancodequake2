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

// Templates.h

#include "TTypes.h"

/*
==============================================================================

	Template functions
 
==============================================================================
*/

/**
\fn	template<typename TType> inline TType Align(const TType &Number, const TType &Alignment)

\brief	Aligns 'Number' to 'Alignment'. 

\author	Paril
\date	27/05/2010

\param	Number		Number to be aligned. 
\param	Alignment	The alignment value. 

\return	. 
**/
template<typename TType>
inline TType Align(const TType &Number, const TType &Alignment)
{
	return (TType)(((intptr)Number + Alignment-1) & ~(Alignment-1));
}

/**
\fn	template<typename TType> inline TType Min(const TType A, const TType B)

\brief	Gets the smaller value of 'A' and 'B'.

\author	Paril
\date	27/05/2010

\param	A	First value. 
\param	B	Second value. 

\return	'A' if A <= B, otherwise B. 
**/
template<typename TType>
inline TType Min(const TType A, const TType B)
{
	return (A<=B) ? A : B;
}

/**
\fn	template<typename TType> inline TType Max(const TType A, const TType B)

\brief	Gets the larger value of 'A' and 'B'

\author	Paril
\date	27/05/2010

\param	A	First value. 
\param	B	Second value. 

\return	'A' if A >= B, otherwise B. 
**/
template<typename TType>
inline TType Max(const TType A, const TType B)
{
	return (A>=B) ? A : B;
}

/**
\fn	template<typename TType> inline TType Clamp(const TType V, const TType L, const TType H)

\brief	Clamps the value 'V' between the low 'L' and high 'H'

\author	Paril
\date	27/05/2010

\param	V	The value to be clamped.
\param	L	The low value. 
\param	H	The high value. 

\return	'V' if 'V' is > 'L' and < 'H', otherwise L or H. 
**/
template<typename TType>
inline TType Clamp(const TType V, const TType L, const TType H)
{
	return (V<L) ? L : (V>H) ? H : V;
}

/**
\fn	template<typename TType> inline bool IsPowOfTwo(const TType Value)

\brief	Query if 'Value' is a power of two.

\author	Paril
\date	27/05/2010

\param	Value	The value. 

\return	true if 'Value' is power of two, false if not. 
**/
template<typename TType>
inline bool IsPowOfTwo(const TType Value)
{
	return (bool)(Value > 0 && (Value & (Value-1)) == 0);
}
