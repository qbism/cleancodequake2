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
// cc_options.h
// Compiler options
//

#if !defined(__CC_OPTIONS_H__) || !defined(INCLUDE_GUARDS)
#define __CC_OPTIONS_H__

// Global macros
// Monsters won't attack allies and will try not to hurt allies with shots
#define MONSTERS_ARENT_STUPID

// Use the Rogue AI instead of regular AI
#define MONSTER_USE_ROGUE_AI

// Makes pathfinding work.
#define MONSTERS_USE_PATHFINDING

// "Extended game imports" refers to all deprecated imports.
// This is actually a bad thing, even though it sounds good!
//#define USE_EXTENDED_GAME_IMPORTS

// Define this if you don't want old function calls to warn you about deprecation.
// THIS IS AT YOUR OWN RISK! CLEANCODE HAS PERFECTLY FUNCTIONAL ALTERNATIVES.
//#define _NO_DEPRECATING_OLD_FUNCTIONS

#if !defined(WIN32) || defined(_NO_DEPRECATING_OLD_FUNCTIONS)
	#define _CC_INSECURE_DEPRECATE(_Replacement)
#else
	#define _CC_INSECURE_DEPRECATE(_Replacement) _CRT_DEPRECATE_TEXT("CleanCode has a better replacement for this function. Consider using " #_Replacement " instead.\nTo disable deprecation, use _NO_DEPRECATING_OLD_FUNCTIONS in cc_options.h.")
#endif

// This is a simple macro to disable deprecation for everything inside the macro.
// This is only used internally; using this in your code could cause big problems.
#if WIN32 && !defined(_NO_DEPRECATING_OLD_FUNCTIONS)
	#define _CC_DISABLE_DEPRECATION		__pragma(warning(push)) \
										__pragma(warning(disable:4996))

	#define _CC_ENABLE_DEPRECATION		__pragma(warning(pop))
#else
	#define _CC_DISABLE_DEPRECATION
	#define _CC_ENABLE_DEPRECATION
#endif

// Don't touch this
#define _CRT_SECURE_NO_WARNINGS

// Define this to use the CleanCode exception handler
#define CC_USE_EXCEPTION_HANDLER

// Define this to allow GAME_CTF
#define CLEANCTF_ENABLED

// Don't touch this.
#define GAME_IS_BEING_COMPILED_NOT_ENGINE_GO_AWAY

// Define this if you want monsters to not be able to go through monstersolids
//#define MONSTERS_HIT_MONSTERSOLID

// Define this if you intend to use quaternions
//#define SHARED_ALLOW_QUATERNIONS

// Define this if you intend to use 3x3 matrix'
//#define SHARED_ALLOW_3x3_MATRIX

// Define this if you intend to use 4x4 matrix'
//#define SHARED_ALLOW_4x4_MATRIX

// Define this to enable some custom asserts that we place in some places
#define ALLOW_ASSERTS

// Notes regarding CC_ASSERT_EXPR:
// Msg must only be one string; the expr is shown and
// therefore not required to be repeated in msg. Arguments
// will cause this function to fail (obviously)
#if defined(ALLOW_ASSERTS)
	#ifndef _DEBUG
		#define _CC_ASSERT_EXPR(expr, msg)
	#else
		// see cc_utils.h
		#define _CC_ASSERT_EXPR(expr, msg) AssertExpression(!!(expr), (msg))
	#endif
#endif

// Define this if you want to use the sin/cos tables.
#define SINCOS_TABLES

// Define this if you want to use include guards.
#define INCLUDE_GUARDS

#ifdef INCLUDE_GUARDS
	#define __STR2__(str) #str
	#define __STR1__(str) __STR2__(str)
	#define __LOC__ __FILE__ "("__STR1__(__LINE__)") : warning: "

	#define FILE_WARNING __pragma(message(__LOC__"file included more than once!"))
#endif

// Define this to get rid of TR1-specific headers
//#define _CC_NO_TR1

#else
FILE_WARNING
#endif