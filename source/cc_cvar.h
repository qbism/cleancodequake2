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

#if !defined(__CC_CVAR_H__) || !defined(INCLUDE_GUARDS)
#define __CC_CVAR_H__

// I know this grabs string and converts and that EGL has floatval/intval, but
// this is just to make sure, because engines that don't might cause problems
class CCvar
{
	cVar_t			*cVar;

	char			*mainValue;
	float			floatVal;
	sint32				intVal;

	// Updates the cvar values above. Private because it won't be called from anywhere else but here.
	void Update ();

public:
	// Constructors, to make it a bit easier (using 'new')
	CCvar ();
	CCvar (const char *cvarName, const char *defaultValue, sint32 flags = 0);
	CCvar (const char *cvarName, sint32 defaultValue, sint32 flags = 0);
	CCvar (const char *cvarName, float defaultValue, sint32 flags = 0);
	// I didn't list any deconstructors, but if any are needed throw them here (you can't really delete cvars from the list from game anyway)

	void Register (const char *cvarName, const char *defaultValue, sint32 flags = 0);
	void Register (const char *cvarName, sint32 defaultValue, sint32 flags = 0);
	void Register (const char *cvarName, float defaultValue, sint32 flags = 0);

	// Let me know if these defs cause any problems with types.
	float		Float ();
	sint32			Integer();
	char		*String();
	bool		Boolean (bool MustBeOne = false);

	void Set (const char *value, bool Force = false);
	void Set (sint32 value, bool Force = false);
	void Set (float value, bool Force = false);

	bool Modified ();
};

#else
FILE_WARNING
#endif