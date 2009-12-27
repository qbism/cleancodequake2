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
// cc_utils.cpp
// g_utils, CleanCode style
//

#include "cc_local.h"

void G_ProjectSource (const vec3f &point, const vec3f &distance, const vec3f &forward, const vec3f &right, vec3f &result)
{
	result.Set (point.X + forward.X * distance.X + right.X * distance.Y,
				point.Y + forward.Y * distance.X + right.Y * distance.Y,
				point.Z + forward.Z * distance.X + right.Z * distance.Y + distance.Z);
}


CBaseEntity *FindRadius (CBaseEntity *From, vec3f &org, sint32 Radius, uint32 EntityFlags, bool CheckNonSolid)
{
	for (edict_t *from = (!From) ? g_edicts : (From->gameEntity + 1); from < &g_edicts[Game.GetNumEdicts()]; from++)
	{
		if (!from->Entity)
			continue;
		CBaseEntity *Entity = from->Entity;
		if (!Entity->GetInUse())
			continue;
		if (CheckNonSolid && (Entity->GetSolid() == SOLID_NOT))
			continue;
		if (!(Entity->EntityFlags & EntityFlags))
			continue;

		if ((sint32)(org - (Entity->State.GetOrigin() + (Entity->GetMins()+ Entity->GetMaxs()) * 0.5)).LengthFast() > Radius)
			continue;
		return from->Entity;
	}

	return NULL;
}

/*
=============
G_PickTarget

Searches all active entities for the next one that holds
the matching string at fieldofs (use the FOFS() macro) in the structure.

Searches beginning at the edict after from, or the beginning if NULL
NULL will be returned if the end of the list is reached.

=============
*/

CBaseEntity *CC_PickTarget (char *targetname)
{
	static TTargetList choices;
	choices.clear();

	if (!targetname)
		return NULL;

	CMapEntity *ent = NULL;
	while(1)
	{
		ent = CC_Find<CMapEntity, ENT_MAP, EntityMemberOffset(CMapEntity,TargetName)> (ent, targetname);
		if (!ent)
			break;

		choices.push_back (ent);
	}

	if (!choices.size())
	{
		DebugPrintf("G_PickTarget: target %s not found\n", targetname);
		return NULL;
	}

	return choices[irandom(choices.size())];
}

TTargetList CC_GetTargets (char *targetname)
{
	static TTargetList choices;
	choices.clear();

	if (!targetname)
		return choices;

	CMapEntity *ent = NULL;
	while(1)
	{
		ent = CC_Find<CMapEntity, ENT_MAP, EntityMemberOffset(CMapEntity,TargetName)> (ent, targetname);
		if (!ent)
			break;

		choices.push_back (ent);
	}

	if (!choices.size())
	{
		DebugPrintf("G_PickTarget: target %s not found\n", targetname);
		return choices;
	}

	return choices;
}

void G_SetMovedir (vec3f &angles, vec3f &movedir)
{
	if (angles.Y == -1)
		movedir.Set (0, 0, 1);
	else if (angles.Y == -2)
		movedir.Set (0, 0, -1);
	else
		angles.ToVectors (&movedir, NULL, NULL);

	angles.Clear();
}

/*
============
G_TouchTriggers

============
*/
void	G_TouchTriggers (CBaseEntity *ent)
{
	static edict_t		*touch[MAX_CS_EDICTS];
	memset(touch, 0, sizeof(touch));

	// dead things don't activate triggers!
	if (ent->EntityFlags & ENT_HURTABLE)
	{
		CHurtableEntity *Hurt = entity_cast<CHurtableEntity>(ent);
		if ((Hurt->CanTakeDamage) && (Hurt->Health <= 0))
			return;
	}

	sint32 num = BoxEdicts (ent->GetAbsMin(), ent->GetAbsMax(), touch, MAX_CS_EDICTS, true);

	// be careful, it is possible to have an entity in this
	// list removed before we get to it (killtriggered)
	for (sint32 i = 0; i < num; i++)
	{
		edict_t *hit = touch[i];
		CBaseEntity *Entity = hit->Entity;

		if (!Entity || !Entity->GetInUse())
			continue;

		if (Entity->EntityFlags & ENT_TOUCHABLE)
		{
			(entity_cast<CTouchableEntity>(Entity))->Touch (ent, NULL, NULL);
			continue;
		}

		if (!ent->GetInUse())
			break;
	}
}

/*
==============================================================================

Kill box

==============================================================================
*/

/*
=================
KillBox

Kills all entities that would touch the proposed new positioning
of ent.  Ent should be unlinked before calling this!
=================
*/


// Calls the callback for each member of the team in "ent"
void CForEachTeamChainCallback::Query (CBaseEntity *Master)
{
	for (CBaseEntity *e = Master->Team.Master; e; e = e->Team.Chain)
		Callback (e);
}

void CForEachPlayerCallback::Query (bool MustBeInUse)
{
	for (uint8 i = 1; i <= game.MaxClients; i++)
	{
		CPlayerEntity *Player = entity_cast<CPlayerEntity>(g_edicts[i].Entity);

		if (MustBeInUse && (!Player->GetInUse() || Player->Client.Persistent.State != SVCS_SPAWNED))
			continue;

		Index = i;
		Callback (Player);
	}
}

/*
=============
range

returns the range catagorization of an entity reletive to self
0	melee range, will become hostile even if back is turned
1	visibility and infront, or visibility and show hostile
2	infront and show hostile
3	only triggered by damage
=============
*/
ERangeType Range (CBaseEntity *self, CBaseEntity *other)
{
	return Range(self->State.GetOrigin(), other->State.GetOrigin());
}

/*
=============
visible

returns 1 if the entity is visible to self, even if not infront ()
=============
*/
bool IsVisible (CBaseEntity *self, CBaseEntity *other)
{
	vec3f start = self->State.GetOrigin() + vec3f(0, 0, self->ViewHeight),
		  end = other->State.GetOrigin() + vec3f(0, 0, other->ViewHeight);
	return (CTrace (start, end,
		self, CONTENTS_MASK_OPAQUE).fraction == 1.0);
}

/*
=============
infront

returns 1 if the entity is in front (in sight) of self
=============
*/
bool IsInFront (CBaseEntity *self, CBaseEntity *other)
{	
	vec3f forward;
	self->State.GetAngles().ToVectors (&forward, NULL, NULL);
	return ((other->State.GetOrigin() - self->State.GetOrigin()).GetNormalized().Dot (forward) > 0.3);
}

#include <crtdbg.h>

void AssertExpression (const bool expr, const char *msg)
{
#if ALLOW_ASSERTS
	if (!expr)
	{
		// On Win32, open up the Crt debug report thingy
#if defined(WIN32)
#if defined(_DEBUG)
		if (_CrtDbgReport(_CRT_ASSERT, __FILE__, __LINE__, NULL, msg) == 1)
			_CrtDbgBreak(); // Call break if we told it to break
#else
		assert (0);
#endif
#else
		// If you hit this, you're on non-Windows.
		// Check msg for more information.
		assert (0);
#endif
		// Print it to the console
		DebugPrintf ("Assertion failed: %s\n", msg);
	}
#endif
}