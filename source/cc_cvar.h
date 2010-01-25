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
// cc_cvar.h
// A small wrapper for cvars that make managing and using cvars a bit easier.
//

#if !defined(CC_GUARD_CVAR_H) || !INCLUDE_GUARDS
#define CC_GUARD_CVAR_H

// I know this grabs string and converts and that EGL has floatval/intval, but
// this is just to make sure, because engines that don't might cause problems
class CCvar
{
	cVar_t			*cVar;

	char			*mainValue;
	float			floatVal;
	sint32			intVal;

	// Updates the cvar values above. Private because it won't be called from anywhere else but here.
	void Update ();

public:
	// Constructors, to make it a bit easier (using 'new')
	CCvar ();
	CCvar (const char *cvarName, const char *defaultValue, ECvarFlags flags = 0);
	CCvar (const char *cvarName, sint32 defaultValue, ECvarFlags flags = 0);
	CCvar (const char *cvarName, float defaultValue, ECvarFlags flags = 0);
	// I didn't list any destructors, but if any are needed throw them here (you can't really delete cvars from the list from game anyway)

	void Register (const char *cvarName, const char *defaultValue, ECvarFlags flags = 0);
	void Register (const char *cvarName, sint32 defaultValue, ECvarFlags flags = 0);
	void Register (const char *cvarName, float defaultValue, ECvarFlags flags = 0);

	// Let me know if these defs cause any problems with types.
	float		Float ();
	sint32		Integer();
	char		*String();
	bool		Boolean (bool MustBeOne = false);

	void Set (const char *value, bool Force = false);
	void Set (sint32 value, bool Force = false);
	void Set (float value, bool Force = false);

	bool Modified ();
};

CC_ENUM (uint16, ECvars)
{
	CV_DEATHMATCH,
	CV_COOP,
	CV_DMFLAGS,
	CV_SKILL,
	CV_FRAG_LIMIT,
	CV_TIME_LIMIT,
	CV_PASSWORD,
	CV_SPECTATOR_PASSWORD,
	CV_NEEDPASS,
	CV_MAXCLIENTS,
	CV_MAXSPECTATORS,
	CV_MAXENTITIES,
	CV_SELECT_EMPTY,
	CV_DEDICATED,
	CV_DEVELOPER,
	CV_FILTERBAN,
	CV_GRAVITY,
	CV_ROLLSPEED,
	CV_ROLLANGLE,
	CV_GUN_X,
	CV_GUN_Y,
	CV_GUN_Z,
	CV_RUN_PITCH,
	CV_RUN_ROLL,
	CV_BOB_UP,
	CV_BOB_PITCH,
	CV_BOB_ROLL,
	CV_CHEATS,
	CV_FLOOD_MSGS,
	CV_FLOOD_PER_SECOND,
	CV_FLOOD_DELAY,
	CV_MAPLIST,
	CV_MAP_DEBUG,
	CV_AIRACCELERATE,
	CV_CC_TECHFLAGS,

#if CLEANCTF_ENABLED
	CV_CTF,
	CV_CTF_FORCEJOIN,
	CV_COMPETITION,
	CV_MATCH_LOCK,
	CV_ELECT_PERCENTAGE,
	CV_MATCH_TIME,
	CV_MATCH_SETUP_TIME,
	CV_MATCH_START_TIME,
	CV_ADMIN_PASSWORD,
	CV_WARP_LIST,
	CV_CAPTURE_LIMIT,
	CV_INSTANT_WEAPONS,
#endif

#if MONSTERS_USE_PATHFINDING
	CV_PATH_NODE_DEBUG,
#endif

	CV_TOTAL_CVARS
};

struct cvarDefaultValue_s
{
	ECvars		cvarIndex;
	const char	*cvarName;
	const char	*cvarDefaultValue;
	ECvarFlags	cvarFlags;
};

extern CCvar	CvarList[CV_TOTAL_CVARS];

void Cvar_Register ();

#else
FILE_WARNING
#endif

