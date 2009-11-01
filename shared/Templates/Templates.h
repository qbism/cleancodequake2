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
template<typename TType>
static inline TType Align(const TType &Number, const TType &Alignment)
{
	return (TType)(((intptr)Number + Alignment-1) & ~(Alignment-1));
}

template<typename TType>
static inline TType Min(const TType &A, const TType &B)
{
	return (A<=B) ? A : B;
}
template<>
static inline float Min(const float &A, const float &B)
{
	return (A<=B) ? A : B;
}
template<>
static inline int Min(const int &A, const int &B)
{
	return (A<=B) ? A : B;
}

template<typename TType>
static inline TType Max(const TType &A, const TType &B)
{
	return (A>=B) ? A : B;
}
template<>
static inline float Max(const float &A, const float &B)
{
	return (A>=B) ? A : B;
}
template<>
static inline int Max(const int &A, const int &B)
{
	return (A>=B) ? A : B;
}

template<typename TType>
static inline TType Clamp(const TType &V, const TType &L, const TType &H)
{
	return (V<L) ? L : (V>H) ? H : V;
}
template<>
static inline float Clamp(const float &V, const float &L, const float &H)
{
	return (V<L) ? L : (V>H) ? H : V;
}
template<>
static inline int Clamp(const int &V, const int &L, const int &H)
{
	return (V<L) ? L : (V>H) ? H : V;
}

template<typename TType>
static inline TType Bound(const TType &V, const TType &L, const TType &H)
{
	return (V>=H) ? V : (L<V) ? V : (L>H) ? H : L;
}
template<>
static inline float Bound(const float &V, const float &L, const float &H)
{
	return (V>=H) ? V : (L<V) ? V : (L>H) ? H : L;
}
template<>
static inline int Bound(const int &V, const int &L, const int &H)
{
	return (V>=H) ? V : (L<V) ? V : (L>H) ? H : L;
}

template<typename TType>
static inline bool IsPowOfTwo(const TType &Value)
{
	return (bool)(Value > 0 && (Value & (Value-1)) == 0);
}
template<>
static inline bool IsPowOfTwo(const int &Value)
{
	return (bool)(Value > 0 && (Value & (Value-1)) == 0);
}
template<>
static inline bool IsPowOfTwo(const uint32 &Value)
{
	return (bool)(Value > 0 && (Value & (Value-1)) == 0);
}

#ifndef ArrayCount
# define ArrayCount(a) (sizeof(a)/sizeof((a)[0]))
#endif

/*
==============================================================================

	STRING RELATED FUNCTIONS

==============================================================================
*/

void	Q_snprintfz(char *dest, size_t size, const char *fmt, ...);
void	Q_strcatz(char *dst, const char *src, size_t dstSize);
size_t	Q_strncpyz(char *dest, const char *src, size_t size);

char	*Q_strlwr(char *s);
char	*Q_strupr(char *s);

#ifdef id386
int __cdecl Q_tolower (int c);
#else // id386
inline int Q_tolower(int chr)
{
	return tolower(chr);
}
#endif // id386
inline int Q_toupper(int chr)
{
	return toupper(chr);
}

// ===========================================================================

int		Q_WildcardMatch (const char *filter, const char *string, int ignoreCase);
std::cc_string	Q_VarArgs (char *format, ...);

#ifndef GAME_IS_BEING_COMPILED_NOT_ENGINE_GO_AWAY
#include "TArray.h"
#include "TAutoPtr.h"
#include "TMap.h"
#include "TString.h"
#endif

