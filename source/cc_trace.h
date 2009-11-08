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
// cc_trace.h
// A wrapper for the ugly gi.trace system.
//

#if !defined(__CC_TRACE_H__) || !defined(INCLUDE_GUARDS)
#define __CC_TRACE_H__

class CTrace
{
	void Copy (cmTrace_t tr);

public:
	float				fraction;
	edict_t				*ent;
	plane_t				plane;
	bool				allSolid;
	bool				startSolid;
	vec3f				EndPos;
	cmBspSurface_t		*surface;
	sint32					contents;

	// For vec3f
	CBaseEntity			*Ent;

	CTrace ();

	CTrace (vec3f &start, vec3f &mins, vec3f &maxs, vec3f &end, CBaseEntity *ignore, sint32 contentMask);
	CTrace (vec3f &start, vec3f &end, CBaseEntity *ignore, sint32 contentMask);
	CTrace (vec3f &start, vec3f &end, sint32 contentMask);

	void operator () (vec3f &start, vec3f &mins, vec3f &maxs, vec3f &end, CBaseEntity *ignore, sint32 contentMask); 
	void operator () (vec3f &start, vec3f &end, CBaseEntity *ignore, sint32 contentMask); 
	void operator () (vec3f &start, vec3f &end, sint32 contentMask); 
};

#else
FILE_WARNING
#endif