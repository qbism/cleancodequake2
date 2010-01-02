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

#if !defined(CC_GUARD_OPTIONS_H) || !INCLUDE_GUARDS
#define CC_GUARD_OPTIONS_H

// Define this to change options based on game mode
#define GAME_ORIGINAL_QUAKE2				1
#define GAME_ROGUE							2
#define GAME_XATRIX							4
#define CC_GAME_MODE						GAME_XATRIX

// Global macros
// Monsters won't attack allies and will try not to hurt allies with shots
#define MONSTERS_ARENT_STUPID				1

// Use the Rogue AI instead of regular AI
#define MONSTER_USE_ROGUE_AI				1

// Makes pathfinding work.
#define MONSTERS_USE_PATHFINDING			1

// "Extended game imports" refers to all deprecated imports.
// This is actually a bad thing, even though it sounds good!
#define USE_EXTENDED_GAME_IMPORTS			0

// Define this if you don't want old function calls to warn you about deprecation.
// THIS IS AT YOUR OWN RISK! CLEANCODE HAS PERFECTLY FUNCTIONAL ALTERNATIVES.
#define _NO_DEPRECATING_OLD_FUNCTIONS		0

#if !defined(WIN32) || _NO_DEPRECATING_OLD_FUNCTIONS
	#define _CC_INSECURE_DEPRECATE(_Replacement)
#else
	#define _CC_INSECURE_DEPRECATE(_Replacement) _CRT_DEPRECATE_TEXT("CleanCode has a better replacement for this function. Consider using " #_Replacement " instead.\nTo disable deprecation, use _NO_DEPRECATING_OLD_FUNCTIONS in cc_options.h.")
#endif

// This is a simple macro to disable deprecation for everything inside the macro.
// This is only used internally; using this in your code could cause big problems.
#if defined(WIN32) && !_NO_DEPRECATING_OLD_FUNCTIONS
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
#define CC_USE_EXCEPTION_HANDLER			1

// Define this to allow GAME_CTF
#define CLEANCTF_ENABLED					1

// Don't touch this.
#define GAME_IS_BEING_COMPILED_NOT_ENGINE_GO_AWAY

// Define this if you want monsters to not be able to go through monstersolids
#define MONSTERS_HIT_MONSTERSOLID			0

// Define this if you intend to use quaternions
#define SHARED_ALLOW_QUATERNIONS			0

// Define this if you intend to use 3x3 matrix'
#define SHARED_ALLOW_3x3_MATRIX				0

// Define this if you intend to use 4x4 matrix'
#define SHARED_ALLOW_4x4_MATRIX				0

// Define this to enable some custom asserts that we place in some places
#define ALLOW_ASSERTS						1

// Notes regarding CC_ASSERT_EXPR:
// Msg must only be one string; the expr is shown and
// therefore not required to be repeated in msg. Arguments
// will cause this function to fail (obviously)
// see cc_utils.cpp
#define _CC_ASSERT_EXPR(expr, msg) AssertExpression(!!(expr), (msg))

// Define this if you want to use include guards.
#define INCLUDE_GUARDS						1

// Generally keep this off if it's not CleanCode
#define NO_VERSION_CHECKING					0

#ifdef INCLUDE_GUARDS
	#define CC_STR2(str) #str
	#define CC_STR1(str) CC_STR2(str)
	#define CC_LOC __FILE__ "("CC_STR1(__LINE__)") : warning: "

#if defined(_WIN32)
	#define FILE_WARNING __pragma(message(CC_LOC"file included more than once!"))
#else
	#define FILE_WARNING
#endif
#endif

// Monster-related preprocessor flags
#define INFANTRY_DOES_REVERSE_GUN_ATTACK	1
#define FLYER_KNOWS_HOW_TO_DODGE			2
#define SUPERTANK_USES_GRENADES				4
#define MUTANT_JUMPS_UNSTUPIDLY				8

// Change this to change some global flags
#define MONSTER_SPECIFIC_FLAGS (INFANTRY_DOES_REVERSE_GUN_ATTACK | FLYER_KNOWS_HOW_TO_DODGE | SUPERTANK_USES_GRENADES | MUTANT_JUMPS_UNSTUPIDLY)

// Enable the example Ammo Regen tech
#define AMMO_REGEN_TECH						1

// Enable Rogue entities and weapons
#define ROGUE_FEATURES						0

// Enable Xatrix entities and weapons
#define XATRIX_FEATURES						0

#if CC_GAME_MODE == GAME_ORIGINAL_QUAKE2
#undef MONSTERS_ARENT_STUPID
#define MONSTERS_ARENT_STUPID 0
#undef MONSTER_USE_ROGUE_AI
#define MONSTER_USE_ROGUE_AI 0
#undef MONSTERS_USE_PATHFINDING
#define MONSTERS_USE_PATHFINDING 0
#undef MONSTER_SPECIFIC_FLAGS
#define MONSTER_SPECIFIC_FLAGS 0
#undef AMMO_REGEN_TECH
#define AMMO_REGEN_TECH 0
#undef MONSTERS_HIT_MONSTERSOLID
#define MONSTERS_HIT_MONSTERSOLID 1
#else
#if (CC_GAME_MODE & GAME_ROGUE) && (CC_GAME_MODE & GAME_XATRIX)
#undef MONSTERS_ARENT_STUPID
#define MONSTERS_ARENT_STUPID 0
#undef MONSTER_USE_ROGUE_AI
#define MONSTER_USE_ROGUE_AI 1
#undef MONSTERS_USE_PATHFINDING
#define MONSTERS_USE_PATHFINDING 0
#undef MONSTER_SPECIFIC_FLAGS
#define MONSTER_SPECIFIC_FLAGS 0
#undef AMMO_REGEN_TECH
#define AMMO_REGEN_TECH 0
#undef MONSTERS_HIT_MONSTERSOLID
#define MONSTERS_HIT_MONSTERSOLID 1
#undef ROGUE_FEATURES
#define ROGUE_FEATURES 1
#undef XATRIX__FEATURES
#define XATRIX_FEATURES 1
#elif (CC_GAME_MODE & GAME_ROGUE)
#undef MONSTERS_ARENT_STUPID
#define MONSTERS_ARENT_STUPID 0
#undef MONSTER_USE_ROGUE_AI
#define MONSTER_USE_ROGUE_AI 1
#undef MONSTERS_USE_PATHFINDING
#define MONSTERS_USE_PATHFINDING 0
#undef MONSTER_SPECIFIC_FLAGS
#define MONSTER_SPECIFIC_FLAGS 0
#undef AMMO_REGEN_TECH
#define AMMO_REGEN_TECH 0
#undef MONSTERS_HIT_MONSTERSOLID
#define MONSTERS_HIT_MONSTERSOLID 1
#undef ROGUE_FEATURES
#define ROGUE_FEATURES 1
#else
#undef MONSTERS_ARENT_STUPID
#define MONSTERS_ARENT_STUPID 0
#undef MONSTER_USE_ROGUE_AI
#define MONSTER_USE_ROGUE_AI 0
#undef MONSTERS_USE_PATHFINDING
#define MONSTERS_USE_PATHFINDING 0
#undef MONSTER_SPECIFIC_FLAGS
#define MONSTER_SPECIFIC_FLAGS 0
#undef AMMO_REGEN_TECH
#define AMMO_REGEN_TECH 0
#undef MONSTERS_HIT_MONSTERSOLID
#define MONSTERS_HIT_MONSTERSOLID 1
#undef XATRIX_FEATURES
#define XATRIX_FEATURES 1
#endif
#endif

#else
FILE_WARNING
#endif