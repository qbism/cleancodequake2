/*
Copyright (C) 1997-2001 Id Software, Inc.

This program is free software; you can redistribute it and/or
modify it under the terms of the GNU General Public License
as published by the Free Software Foundation; either version 2
of the License, or (at your option) any later version.

This program is distributed in the hope that it will be useful,
but WITHOUT ANY WARRANTY; without even the implied warranty of
MERCHANTABILITY or v

See the GNU General Public License for more details.

You should have received a copy of the GNU General Public License
along with this program; if not, write to the Free Software
Foundation, Inc., 59 Temple Place - Suite 330, Boston, MA 02111-1307, USA.
*/

// Platform.h

// =========================================================================
// Windows
//
#ifdef WIN32

#define VS_UNKNOWN	0
#define VS_5		1
#define VS_6		2
#define VS_7		3
#define VS_71		4
#define VS_8		5
#define VS_9		6
#define VS_10		7

#define VS_UNKNOWN_STR	"Unknown Visual Studio"
#define VS_5_STR		"Visual C++ 5.0"
#define VS_6_STR		"Visual C++ 6.0"
#define VS_7_STR		"Visual C++ 7.0"
#define VS_71_STR		"Visual C++ 7.1"
#define VS_8_STR		"Visual Studio 2005"
#define VS_9_STR		"Visual Studio 2008"
#define VS_10_STR		"Visual Studio 2010 BETA"

#if (_MSC_VER >= 1600) // 2010
	#define MSVS_VERSION			VS_10
	#define MSVS_VERSION_STRING		VS_10_STR
	#define HAS__CPP0x				1
#elif (_MSC_VER >= 1500) // 2008
	#define MSVS_VERSION			VS_9
	#define MSVS_VERSION_STRING		VS_9_STR
#elif (_MSC_VER >= 1400) // 2005
	#define MSVS_VERSION			VS_8
	#define MSVS_VERSION_STRING		VS_8_STR
#elif (_MSC_VER >= 1310) // C++ 7.1
	#define MSVS_VERSION			VS_7
	#define MSVS_VERSION_STRING		VS_7_STR
#elif (_MSC_VER >= 1300) // C++ 7.0
	#define MSVS_VERSION			VS_71
	#define MSVS_VERSION_STRING		VS_71_STR
#elif (_MSC_VER >= 1200) // C++ 6.0
	#define MSVS_VERSION			VS_6
	#define MSVS_VERSION_STRING		VS_6_STR
#elif (_MSC_VER >= 1100) // C++ 5.0
	#define MSVS_VERSION			VS_5
	#define MSVS_VERSION_STRING		VS_5_STR
#else
	#define MSVS_VERSION			VS_UNKNOWN
	#define MSVS_VERSION_STRING		VS_UNKNOWN_STR
#endif

#define COMPILERSTRING MSVS_VERSION_STRING

#define CC_ENUM(type,name) \
	typedef type name; \
	enum

// unknown pragmas are SUPPOSED to be ignored, but....
# pragma warning(disable : 4244)	// 'conversion' conversion from 'type1' to 'type2', possible loss of data

// Off by default, force on for level 3 warning level
# pragma warning(3 : 4056)	// overflow in floating point constant arithmetic
# pragma warning(3 : 4254)	// 'operator' : conversion from 'type1' to 'type2', possible loss of data
// Paril, for CleanCode
# pragma warning (disable : 4100) // unreferenced formal parameter
# pragma warning (disable : 4389) // signed/unsigned mismatch
# pragma warning (disable : 4127) // conditional expression is constant

# pragma intrinsic(memcmp)

# define HAVE___INLINE 1
# define HAVE___FASTCALL 1
# define HAVE__SNPRINTF 1
# define HAVE__STRICMP 1
# define HAVE__VSNPRINTF 1
# define HAVE__CDECL 1

# define BUILDSTRING		"Win32"

# ifdef _M_IX86
#   define CPUSTRING		"x86"
# elif defined _M_ALPHA
#  define CPUSTRING		"AXP"
# elif _M_IA64
#  define CPUSTRING		"x64"
# endif

typedef	signed char			sint8;
typedef signed __int16		sint16;
typedef signed __int32		sint32;
typedef signed __int64		sint64;
typedef unsigned char		uint8;
typedef unsigned __int16	uint16;
typedef unsigned __int32	uint32;
typedef unsigned __int64	uint64;

# define strdup _strdup
# define itoa _itoa

# define __declspec_naked __declspec(naked)
# define cc_thiscall __thiscall

// =========================================================================
// Generic Unix
//
#elif defined __unix__
#  define GL_FORCEFINISH

#  define HAVE_INLINE 1
#  define HAVE_STRCASECMP 1

# ifdef _GNU_SOURCE
#  define HAVE_SINCOSF 1
# endif

#  define __declspec
#  define __declspec_naked
#  define cc_thiscall thiscall 

//
// Linux
//
# if defined __linux__
#  include <stdint.h>

#  define BUILDSTRING		"Linux"

#  ifdef __i386__
#   define CPUSTRING		"i386"
#  elif defined(__alpha__)
#   define CPUSTRING		"AXP"
#  endif

//
// FreeBSD
//
# elif defined __FreeBSD__
#  include <machine/param.h>
#  if __FreeBSD_version < 500000
#   include <inttypes.h>
#  else
#   include <stdint.h>
#  endif

#  define BUILDSTRING		"FreeBSD"

#  ifdef __i386__
#   define CPUSTRING		"i386"
#  elif defined(__alpha__)
#   define CPUSTRING		"AXP"
#  endif

# endif

typedef int16_t				sint16;
typedef int32_t				sint32;
typedef int64_t				int64;
typedef uint16_t			uint16;
typedef uint32_t			uint32;
typedef uint64_t			uint64;

#endif	// __unix__

// =========================================================================

#ifndef HAVE__CDECL
# define __cdecl
#endif

#ifndef HAVE___FASTCALL
# define __fastcall
#endif

#ifdef HAVE___INLINE
# ifndef inline
#  define inline __inline
# endif
#elif !defined(HAVE_INLINE)
# ifndef inline
#  define inline
# endif
#endif

#ifdef HAVE__SNPRINTF
# ifndef snprintf 
#  define snprintf _snprintf
# endif
#endif

#ifdef HAVE_STRCASECMP
# ifndef Q_stricmp
#  define Q_stricmp(s1,s2) strcasecmp ((s1), (s2))
# endif
# ifndef Q_strnicmp
#  define Q_strnicmp(s1,s2,n) strncasecmp ((s1), (s2), (n))
# endif
#endif

#ifdef HAVE__STRICMP
# ifndef Q_stricmp
inline sint32 Q_stricmp (const char *s1, const char *s2)
{
	return _stricmp (s1, s2);
}
# endif
# ifndef Q_strnicmp
inline sint32 Q_strnicmp (const char *s1, const char *s2, size_t n)
{
	return _strnicmp (s1, s2, n);
}
# endif
#define stricmp _stricmp
#endif

#ifdef HAVE__VSNPRINTF
# ifndef vsnprintf
#  define vsnprintf _vsnprintf
# endif
#endif

// =========================================================================

#if (defined(_M_IX86) || defined(__i386__)) && !defined(C_ONLY) && !defined(__unix__) // FIXME: make this work with unix
# define id386
#else
# ifdef id386
#  undef id386
# endif
#endif

#ifndef BUILDSTRING
# define BUILDSTRING	"Unknown"
#endif

#ifndef CPUSTRING
# define CPUSTRING		"Unknown"
#endif

#ifndef COMPILERSTRING
# define COMPILERSTRING	"Unknown"
#endif
