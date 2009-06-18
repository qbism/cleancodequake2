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
// cc_trace.cpp
// A wrapper for the ugly gi.trace system.
//

#include "cc_local.h"

void CTrace::Copy (cmTrace_t tr)
{
	fraction = tr.fraction;
	ent = tr.ent;
	plane = tr.plane;
	allSolid = (tr.allSolid != 0);
	startSolid = (tr.startSolid != 0);
	Vec3Copy (tr.endPos, endPos);
	surface = tr.surface;
	contents = tr.contents;
};

CTrace::CTrace ()
{
	fraction = 0;
	ent = NULL;
	memset (&plane, 0, sizeof(plane));
	allSolid = false;
	startSolid = false;
	Vec3Copy (vec3Origin, endPos);
	surface = NULL;
	contents = -1;
}

_CC_DISABLE_DEPRECATION
(
// Constructor easyness
CTrace::CTrace (vec3_t start, vec3_t mins, vec3_t maxs, vec3_t end, edict_t *ignore, int contentMask)
{
	cmTrace_t temp = gi.trace(start, mins, maxs, end, ignore, contentMask);
	Copy(temp);
};

CTrace::CTrace (vec3_t start, vec3_t end, edict_t *ignore, int contentMask)
{
	cmTrace_t temp = gi.trace(start, vec3Origin, vec3Origin, end, ignore, contentMask);
	Copy(temp);
};

CTrace::CTrace (vec3_t start, vec3_t end, int contentMask)
{
	cmTrace_t temp = gi.trace(start, vec3Origin, vec3Origin, end, NULL, contentMask);
	Copy(temp);
}
)