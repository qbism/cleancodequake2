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
// g_utils.c -- misc utility functions for game module

#include "g_local.h"

void G_ProjectSource (vec3_t point, vec3_t distance, vec3_t forward, vec3_t right, vec3_t result)
{
	result[0] = point[0] + forward[0] * distance[0] + right[0] * distance[1];
	result[1] = point[1] + forward[1] * distance[0] + right[1] * distance[1];
	result[2] = point[2] + forward[2] * distance[0] + right[2] * distance[1] + distance[2];
}

void G_ProjectSource (vec3f point, vec3f distance, vec3f forward, vec3f right, vec3f &result)
{
	result.Set (point.X + forward.X * distance.X + right.X * distance.Y,
				point.Y + forward.Y * distance.X + right.Y * distance.Y,
				point.Z + forward.Z * distance.X + right.Z * distance.Y + distance.Z);
}


/*
=============
G_Find

Searches all active entities for the next one that holds
the matching string at fieldofs (use the FOFS() macro) in the structure.

Searches beginning at the edict after from, or the beginning if NULL
NULL will be returned if the end of the list is reached.

=============
*/
edict_t *G_Find (edict_t *from, int fieldofs, char *match)
{
	char	*s;

	if (!from)
		from = g_edicts;
	else
		from++;

	for ( ; from < &g_edicts[globals.numEdicts] ; from++)
	{
		if (!from->inUse)
			continue;
		s = *(char **) ((byte *)from + fieldofs);
		if (!s)
			continue;
		if (!Q_stricmp (s, match))
			return from;
	}

	return NULL;
}


/*
=================
findradius

Returns entities that have origins within a spherical area

findradius (origin, radius)
=================
*/
edict_t *findradius (edict_t *from, vec3_t org, float rad)
{
	vec3_t	eorg;
	int		j;

	if (!from)
		from = g_edicts;
	else
		from++;
	for ( ; from < &g_edicts[globals.numEdicts]; from++)
	{
		if (!from->inUse)
			continue;
		if (from->solid == SOLID_NOT)
			continue;
		for (j=0 ; j<3 ; j++)
			eorg[j] = org[j] - (from->state.origin[j] + (from->mins[j] + from->maxs[j])*0.5);
		if (Vec3Length(eorg) > rad)
			continue;
		return from;
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
#define MAXCHOICES	8

edict_t *G_PickTarget (char *targetname)
{
	edict_t	*ent = NULL;
	int		num_choices = 0;
	edict_t	*choice[MAXCHOICES];

	if (!targetname)
	{
		//gi.dprintf("G_PickTarget called with NULL targetname\n");
		return NULL;
	}

	while(1)
	{
		ent = G_Find (ent, FOFS(targetname), targetname);
		if (!ent)
			break;
		choice[num_choices++] = ent;
		if (num_choices == MAXCHOICES)
			break;
	}

	if (!num_choices)
	{
		DebugPrintf("G_PickTarget: target %s not found\n", targetname);
		return NULL;
	}

	return choice[rand() % num_choices];
}



void Think_Delay (edict_t *ent)
{
	G_UseTargets (ent, ent->activator);
	G_FreeEdict (ent);
}

/*
==============================
G_UseTargets

the global "activator" should be set to the entity that initiated the firing.

If self.delay is set, a DelayedUse entity will be created that will actually
do the SUB_UseTargets after that many seconds have passed.

Centerprints any self.message to the activator.

Search for (string)targetname in all entities that
match (string)self.target and call their .use function

==============================
*/
void G_UseTargets (edict_t *ent, edict_t *activator)
{
	edict_t		*t;

//
// check for a delay
//
	if (ent->delay)
	{
	// create a temp object to fire at a later time
		t = G_Spawn();
		t->classname = "DelayedUse";

		// Paril: for compatibility
		t->nextthink = level.framenum + (ent->delay * 10);
		t->think = Think_Delay;
		t->activator = activator;
		if (!activator)
			DebugPrintf ("Think_Delay with no activator\n");
		t->message = ent->message;
		t->target = ent->target;
		t->killtarget = ent->killtarget;
		return;
	}
	
	
//
// print the message
//
	if ((ent->message) && !(activator->svFlags & SVF_MONSTER))
	{
		ClientPrintf (activator, PRINT_CENTER, "%s", ent->message);
		if (ent->noise_index)
			PlaySoundFrom (activator, CHAN_AUTO, ent->noise_index);
		else
			PlaySoundFrom (activator, CHAN_AUTO, SoundIndex ("misc/talk1.wav"));
	}

//
// kill killtargets
//
	if (ent->killtarget)
	{
		t = NULL;
		while ((t = G_Find (t, FOFS(targetname), ent->killtarget)) != NULL)
		{
			G_FreeEdict (t);
			if (!ent->inUse)
			{
				DebugPrintf("entity was removed while using killtargets\n");
				return;
			}
		}
	}

//
// fire targets
//
	if (ent->target)
	{
		t = NULL;
		while ((t = G_Find (t, FOFS(targetname), ent->target)) != NULL)
		{
			// doors fire area portals in a specific way
			if (!Q_stricmp(t->classname, "func_areaportal") &&
				(!Q_stricmp(ent->classname, "func_door") || !Q_stricmp(ent->classname, "func_door_rotating")))
				continue;

			if (t == ent)
				DebugPrintf ("WARNING: Entity used itself.\n");
			else if (t->use)
				t->use (t, ent, activator);
			if (!ent->inUse)
			{
				DebugPrintf("entity was removed while using targets\n");
				return;
			}
		}
	}
}

vec3_t VEC_UP		= {0, -1, 0};
vec3_t MOVEDIR_UP	= {0, 0, 1};
vec3_t VEC_DOWN		= {0, -2, 0};
vec3_t MOVEDIR_DOWN	= {0, 0, -1};

void G_SetMovedir (vec3_t angles, vec3_t movedir)
{
	if (angles[1] == VEC_UP[1])
		Vec3Copy (MOVEDIR_UP, movedir);
	else if (angles[1] == VEC_DOWN[1])
		Vec3Copy (MOVEDIR_DOWN, movedir);
	else
		Angles_Vectors (angles, movedir, NULL, NULL);

	Vec3Clear (angles);
}

void G_InitEdict (edict_t *e)
{
	e->inUse = true;
	e->classname = "noclass";
	e->gravity = 1.0;
	e->state.number = e - g_edicts;
}

/*
=================
G_Spawn

Either finds a free edict, or allocates a new one.
Try to avoid reusing an entity that was recently freed, because it
can cause the client to think the entity morphed into something else
instead of being removed and recreated, which can cause interpolated
angles and bad trails.
=================
*/
edict_t *G_Spawn (void)
{
	int			i;
	edict_t		*e;

	e = &g_edicts[game.maxclients+1];
	for ( i=game.maxclients+1 ; i<globals.numEdicts ; i++, e++)
	{
		// the first couple seconds of server time can involve a lot of
		// freeing and allocating, so relax the replacement policy
		if (!e->inUse && ( e->freetime < 20 || level.framenum - e->freetime > 5 ) )
		{
			G_InitEdict (e);
			//DebugPrintf ("Entity %i reused\n", i);
			return e;
		}
	}
	
	if (i == game.maxentities)
		gi.error ("ED_Alloc: no free edicts");
		
	//DebugPrintf ("Entity %i allocated\n", i);
	globals.numEdicts++;
	G_InitEdict (e);
	return e;
}

/*
=================
G_FreeEdict

Marks the edict as free
=================
*/
void G_FreeEdict (edict_t *ed)
{
	gi.unlinkentity (ed);		// unlink from world

	// Paril, hack
	CBaseEntity *Entity = ed->Entity;
	memset (ed, 0, sizeof(*ed));
	ed->Entity = Entity;
	ed->classname = "freed";
	ed->freetime = level.framenum;
	ed->inUse = false;
}


/*
============
G_TouchTriggers

============
*/
void	G_TouchTriggers (edict_t *ent)
{
	int			i, num;
	edict_t		*touch[MAX_CS_EDICTS], *hit;

	// dead things don't activate triggers!
	if ((ent->client || (ent->svFlags & SVF_MONSTER)) && (ent->health <= 0))
		return;

	num = gi.BoxEdicts (ent->absMin, ent->absMax, touch, MAX_CS_EDICTS, AREA_TRIGGERS);

	// be careful, it is possible to have an entity in this
	// list removed before we get to it (killtriggered)
	for (i=0 ; i<num ; i++)
	{
		hit = touch[i];
		if (!hit->inUse)
			continue;

		if (ent->Entity && hit->Entity && (hit->Entity->EntityFlags & ENT_TOUCHABLE))
		{
			(dynamic_cast<CTouchableEntity*>(hit->Entity))->Touch (ent->Entity, NULL, NULL);
			continue;
		}
		if (!hit->touch)
			continue;
		hit->touch (hit, ent, NULL, NULL);
	}
}

/*
============
G_TouchSolids

Call after linking a new trigger in during gameplay
to force all entities it covers to immediately touch it
============
*/
void	G_TouchSolids (edict_t *ent)
{
	int			i, num;
	edict_t		*touch[MAX_CS_EDICTS], *hit;

	num = gi.BoxEdicts (ent->absMin, ent->absMax, touch, MAX_CS_EDICTS, AREA_SOLID);

	// be careful, it is possible to have an entity in this
	// list removed before we get to it (killtriggered)
	for (i=0 ; i<num ; i++)
	{
		hit = touch[i];
		if (!hit->inUse)
			continue;
		if (ent->touch)
			ent->touch (hit, ent, NULL, NULL);
		if (!ent->inUse)
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
bool KillBox (edict_t *ent)
{
	CTrace		tr;

	while (1)
	{
		tr = CTrace (ent->state.origin, ent->mins, ent->maxs, ent->state.origin, NULL, CONTENTS_MASK_PLAYERSOLID);
		if (!tr.ent)
			break;

		// nail it
		T_Damage (tr.ent, ent, ent, vec3Origin, ent->state.origin, vec3Origin, 100000, 0, DAMAGE_NO_PROTECTION, MOD_TELEFRAG);

		// if we didn't kill it, fail
		if (tr.ent->solid)
			return false;
	}

	return true;		// all clear
}
