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
// g_misc.c

#include "g_local.h"

/*QUAKED func_group (0 0 0) ?
Used to group brushes together just for editor convenience.
*/

//=====================================================

void Use_Areaportal (edict_t *ent, edict_t *other, edict_t *activator)
{
	ent->count ^= 1;		// toggle state
//	gi.dprintf ("portalstate: %i = %i\n", ent->style, ent->count);
	gi.SetAreaPortalState (ent->style, (ent->count != 0));
}

/*QUAKED func_areaportal (0 0 0) ?

This is a non-visible object that divides the world into
areas that are seperated when this portal is not activated.
Usually enclosed in the middle of a door.
*/
void SP_func_areaportal (edict_t *ent)
{
	ent->use = Use_Areaportal;
	ent->count = 0;		// always start closed;
}

//=====================================================

/*
=================
debris
=================
*/
void debris_die (edict_t *self, edict_t *inflictor, edict_t *attacker, int damage, vec3_t point)
{
	G_FreeEdict (self);
}

void ThrowDebris (edict_t *self, char *modelname, float speed, vec3_t origin)
{
	edict_t	*chunk;
	vec3_t	v;

	chunk = G_Spawn();
	Vec3Copy (origin, chunk->state.origin);
	chunk->state.modelIndex = ModelIndex(modelname);
	v[0] = 100 * crandom();
	v[1] = 100 * crandom();
	v[2] = 100 + 100 * crandom();
	Vec3MA (self->velocity, speed, v, chunk->velocity);
	chunk->movetype = MOVETYPE_BOUNCE;
	chunk->solid = SOLID_NOT;
	chunk->avelocity[0] = random()*600;
	chunk->avelocity[1] = random()*600;
	chunk->avelocity[2] = random()*600;
	chunk->think = G_FreeEdict;
	chunk->nextthink = level.framenum + 50 + random()*50;
	chunk->state.frame = 0;
	chunk->flags = 0;
	chunk->classname = "debris";
	chunk->takedamage = true;
	chunk->die = debris_die;
	gi.linkentity (chunk);
}


void BecomeExplosion1 (edict_t *self)
{
#ifdef CLEANCTF_ENABLED
//ZOID
	//flags are important
	if (strcmp(self->classname, "item_flag_team1") == 0)
	{
		CTFResetFlag(CTF_TEAM1); // this will free self!
		BroadcastPrintf(PRINT_HIGH, "The %s flag has returned!\n",
			CTFTeamName(CTF_TEAM1));
		return;
	}
	if (strcmp(self->classname, "item_flag_team2") == 0)
	{
		CTFResetFlag(CTF_TEAM2); // this will free self!
		BroadcastPrintf(PRINT_HIGH, "The %s flag has returned!\n",
			CTFTeamName(CTF_TEAM2));
		return;
	}
	// techs are important too
	if (self->item && (self->item->Flags & ITEMFLAG_TECH))
	{
		CTFRespawnTech(self); // this frees self!
		return;
	}
//ZOID
#endif

	CTempEnt_Explosions::RocketExplosion (self->state.origin, self);
	G_FreeEdict (self);
}


void BecomeExplosion2 (edict_t *self)
{
	CTempEnt_Explosions::GrenadeExplosion (self->state.origin, self);
	G_FreeEdict (self);
}


/*QUAKED path_corner (.5 .3 0) (-8 -8 -8) (8 8 8) TELEPORT
Target: next path corner
Pathtarget: gets used when an entity that has
	this path_corner targeted touches it
*/

void path_corner_touch (edict_t *self, edict_t *other, plane_t *plane, cmBspSurface_t *surf)
{
	vec3_t		v;
	edict_t		*next;

	if (other->movetarget != self)
		return;
	
	if (other->enemy)
		return;

	if (self->pathtarget)
	{
		char *savetarget;

		savetarget = self->target;
		self->target = self->pathtarget;
		G_UseTargets (self, other);
		self->target = savetarget;
	}

	if (self->target)
		next = G_PickTarget(self->target);
	else
		next = NULL;

	if ((next) && (next->spawnflags & 1))
	{
		Vec3Copy (next->state.origin, v);
		v[2] += next->mins[2];
		v[2] -= other->mins[2];
		Vec3Copy (v, other->state.origin);
		next = G_PickTarget(next->target);
		other->state.event = EV_OTHER_TELEPORT;
	}

	other->goalentity = other->movetarget = next;

	if (self->wait)
	{
		if (other->Entity->EntityFlags & ENT_MONSTER)
		{
			// Backcompat
			(dynamic_cast<CMonsterEntity*>(other->Entity))->Monster->PauseTime = level.framenum + (self->wait * 10);
			(dynamic_cast<CMonsterEntity*>(other->Entity))->Monster->Stand();
		}
		return;
	}

	if (!other->movetarget)
	{
		if (other->Entity->EntityFlags & ENT_MONSTER)
		{
			(dynamic_cast<CMonsterEntity*>(other->Entity))->Monster->PauseTime = level.framenum + 100000000;
			(dynamic_cast<CMonsterEntity*>(other->Entity))->Monster->Stand ();
		}
	}
	else
	{
		Vec3Subtract (other->goalentity->state.origin, other->state.origin, v);
		other->ideal_yaw = VecToYaw (v);
	}
}

void SP_path_corner (edict_t *self)
{
	if (!self->targetname)
	{
		//gi.dprintf ("path_corner with no targetname at (%f %f %f)\n", self->state.origin[0], self->state.origin[1], self->state.origin[2]);
		MapPrint (MAPPRINT_ERROR, self, self->state.origin, "No targetname\n");
		G_FreeEdict (self);
		return;
	}

	self->solid = SOLID_TRIGGER;
	self->touch = path_corner_touch;
	Vec3Set (self->mins, -8, -8, -8);
	Vec3Set (self->maxs, 8, 8, 8);
	self->svFlags |= SVF_NOCLIENT;
	gi.linkentity (self);
}


/*QUAKED point_combat (0.5 0.3 0) (-8 -8 -8) (8 8 8) Hold
Makes this the target of a monster and it will head here
when first activated before going after the activator.  If
hold is selected, it will stay here.
*/
void point_combat_touch (edict_t *self, edict_t *other, plane_t *plane, cmBspSurface_t *surf)
{
	edict_t	*activator;

	if (other->movetarget != self)
		return;

	if (self->target)
	{
		other->target = self->target;
		other->goalentity = other->movetarget = G_PickTarget(other->target);
		if (!other->goalentity)
		{
			DebugPrintf("%s at (%f %f %f) target %s does not exist\n", self->classname, self->state.origin[0], self->state.origin[1], self->state.origin[2], self->target);
			other->movetarget = self;
		}
		self->target = NULL;
	}
	else if ((self->spawnflags & 1) && !(other->flags & (FL_SWIM|FL_FLY)))
	{
		if (other->Entity->EntityFlags & ENT_MONSTER)
		{
			(dynamic_cast<CMonsterEntity*>(other->Entity))->Monster->PauseTime = level.framenum + 100000000;
			(dynamic_cast<CMonsterEntity*>(other->Entity))->Monster->AIFlags |= AI_STAND_GROUND;
			(dynamic_cast<CMonsterEntity*>(other->Entity))->Monster->Stand ();
		}
	}

	if (other->movetarget == self)
	{
		other->target = NULL;
		other->movetarget = NULL;
		other->goalentity = other->enemy;

		if (other->Entity->EntityFlags & ENT_MONSTER)
			(dynamic_cast<CMonsterEntity*>(other->Entity))->Monster->AIFlags &= ~AI_COMBAT_POINT;
	}

	if (self->pathtarget)
	{
		char *savetarget;

		savetarget = self->target;
		self->target = self->pathtarget;
		if (other->enemy && other->enemy->client)
			activator = other->enemy;
		else if (other->oldenemy && other->oldenemy->client)
			activator = other->oldenemy;
		else if (other->activator && other->activator->client)
			activator = other->activator;
		else
			activator = other;
		G_UseTargets (self, activator);
		self->target = savetarget;
	}
}

void SP_point_combat (edict_t *self)
{
	if (game.mode & GAME_DEATHMATCH)
	{
		G_FreeEdict (self);
		return;
	}
	self->solid = SOLID_TRIGGER;
	self->touch = point_combat_touch;
	Vec3Set (self->mins, -8, -8, -16);
	Vec3Set (self->maxs, 8, 8, 16);
	self->svFlags = SVF_NOCLIENT;
	gi.linkentity (self);
};


/*QUAKED viewthing (0 .5 .8) (-8 -8 -8) (8 8 8)
Just for the debugging level.  Don't use
*/
void TH_viewthing(edict_t *ent)
{
	ent->state.frame = (ent->state.frame + 1) % 7;
	ent->nextthink = level.framenum + FRAMETIME;
}

void SP_viewthing(edict_t *ent)
{
	DebugPrintf ("viewthing spawned\n");

	ent->movetype = MOVETYPE_NONE;
	ent->solid = SOLID_BBOX;
	ent->state.renderFx = RF_FRAMELERP;
	Vec3Set (ent->mins, -16, -16, -24);
	Vec3Set (ent->maxs, 16, 16, 32);
	ent->state.modelIndex = ModelIndex ("models/objects/banner/tris.md2");
	gi.linkentity (ent);
	ent->nextthink = level.framenum + 5;
	ent->think = TH_viewthing;
	return;
}


/*QUAKED info_null (0 0.5 0) (-4 -4 -4) (4 4 4)
Used as a positional target for spotlights, etc.
*/
void SP_info_null (edict_t *self)
{
	G_FreeEdict (self);
};


/*QUAKED info_notnull (0 0.5 0) (-4 -4 -4) (4 4 4)
Used as a positional target for lightning.
*/
void SP_info_notnull (edict_t *self)
{
	Vec3Copy (self->state.origin, self->absMin);
	Vec3Copy (self->state.origin, self->absMax);
};


/*QUAKED light (0 1 0) (-8 -8 -8) (8 8 8) START_OFF
Non-displayed light.
Default light value is 300.
Default style is 0.
If targeted, will toggle between on and off.
Default _cone value is 10 (used to set size of light for spotlights)
*/

#define START_OFF	1

static void light_use (edict_t *self, edict_t *other, edict_t *activator)
{
	if (self->spawnflags & START_OFF)
	{
		ConfigString (CS_LIGHTS+self->style, "m");
		self->spawnflags &= ~START_OFF;
	}
	else
	{
		ConfigString (CS_LIGHTS+self->style, "a");
		self->spawnflags |= START_OFF;
	}
}

void SP_light (edict_t *self)
{
	// no targeted lights in deathmatch, because they cause global messages
	if (!self->targetname || (game.mode & GAME_DEATHMATCH))
	{
		G_FreeEdict (self);
		return;
	}

	if (self->style >= 32)
	{
		self->use = light_use;
		if (self->spawnflags & START_OFF)
			ConfigString (CS_LIGHTS+self->style, "a");
		else
			ConfigString (CS_LIGHTS+self->style, "m");
	}
}


/*QUAKED func_wall (0 .5 .8) ? TRIGGER_SPAWN TOGGLE START_ON ANIMATED ANIMATED_FAST
This is just a solid wall if not inhibited

TRIGGER_SPAWN	the wall will not be present until triggered
				it will then blink in to existance; it will
				kill anything that was in it's way

TOGGLE			only valid for TRIGGER_SPAWN walls
				this allows the wall to be turned on and off

START_ON		only valid for TRIGGER_SPAWN walls
				the wall will initially be present
*/

void func_wall_use (edict_t *self, edict_t *other, edict_t *activator)
{
	if (self->solid == SOLID_NOT)
	{
		self->solid = SOLID_BSP;
		self->svFlags &= ~SVF_NOCLIENT;
		KillBox (self);
	}
	else
	{
		self->solid = SOLID_NOT;
		self->svFlags |= SVF_NOCLIENT;
	}
	gi.linkentity (self);

	if (!(self->spawnflags & 2))
		self->use = NULL;
}

void SP_func_wall (edict_t *self)
{
	self->movetype = MOVETYPE_PUSH;
	SetModel (self, self->model);

	if (self->spawnflags & 8)
		self->state.effects |= EF_ANIM_ALL;
	if (self->spawnflags & 16)
		self->state.effects |= EF_ANIM_ALLFAST;

	// just a wall
	if ((self->spawnflags & 7) == 0)
	{
		self->solid = SOLID_BSP;
		gi.linkentity (self);
		return;
	}

	// it must be TRIGGER_SPAWN
	if (!(self->spawnflags & 1))
	{
//		gi.dprintf("func_wall missing TRIGGER_SPAWN\n");
		self->spawnflags |= 1;
	}

	// yell if the spawnflags are odd
	if (self->spawnflags & 4)
	{
		if (!(self->spawnflags & 2))
		{
			//gi.dprintf("func_wall START_ON without TOGGLE\n");
			MapPrint (MAPPRINT_WARNING, self, self->absMin, "Invalid spawnflags: START_ON without TOGGLE\n");
			self->spawnflags |= 2;
		}
	}

	self->use = func_wall_use;
	if (self->spawnflags & 4)
	{
		self->solid = SOLID_BSP;
	}
	else
	{
		self->solid = SOLID_NOT;
		self->svFlags |= SVF_NOCLIENT;
	}
	gi.linkentity (self);
}


/*QUAKED func_object (0 .5 .8) ? TRIGGER_SPAWN ANIMATED ANIMATED_FAST
This is solid bmodel that will fall if it's support it removed.
*/

void func_object_touch (edict_t *self, edict_t *other, plane_t *plane, cmBspSurface_t *surf)
{
	// only squash thing we fall on top of
	if (!plane)
		return;
	if (plane->normal[2] < 1.0)
		return;
	if (other->takedamage == false)
		return;
	T_Damage (other, self, self, vec3Origin, self->state.origin, vec3Origin, self->dmg, 1, 0, MOD_CRUSH);
}

void func_object_release (edict_t *self)
{
	self->movetype = MOVETYPE_TOSS;
	self->touch = func_object_touch;
}

void func_object_use (edict_t *self, edict_t *other, edict_t *activator)
{
	self->solid = SOLID_BSP;
	self->svFlags &= ~SVF_NOCLIENT;
	self->use = NULL;
	KillBox (self);
	func_object_release (self);
}

void SP_func_object (edict_t *self)
{
	SetModel (self, self->model);

	self->mins[0] += 1;
	self->mins[1] += 1;
	self->mins[2] += 1;
	self->maxs[0] -= 1;
	self->maxs[1] -= 1;
	self->maxs[2] -= 1;

	if (!self->dmg)
		self->dmg = 100;

	if (self->spawnflags == 0)
	{
		self->solid = SOLID_BSP;
		self->movetype = MOVETYPE_PUSH;
		self->think = func_object_release;
		self->nextthink = level.framenum + 2;
	}
	else
	{
		self->solid = SOLID_NOT;
		self->movetype = MOVETYPE_PUSH;
		self->use = func_object_use;
		self->svFlags |= SVF_NOCLIENT;
	}

	if (self->spawnflags & 2)
		self->state.effects |= EF_ANIM_ALL;
	if (self->spawnflags & 4)
		self->state.effects |= EF_ANIM_ALLFAST;

	self->clipMask = CONTENTS_MASK_MONSTERSOLID;

	gi.linkentity (self);
}


/*QUAKED func_explosive (0 .5 .8) ? Trigger_Spawn ANIMATED ANIMATED_FAST
Any brush that you want to explode or break apart.  If you want an
ex0plosion, set dmg and it will do a radius explosion of that amount
at the center of the bursh.

If targeted it will not be shootable.

health defaults to 100.

mass defaults to 75.  This determines how much debris is emitted when
it explodes.  You get one large chunk per 100 of mass (up to 8) and
one small chunk per 25 of mass (up to 16).  So 800 gives the most.
*/
void func_explosive_explode (edict_t *self, edict_t *inflictor, edict_t *attacker, int damage, vec3_t point)
{
	vec3_t	origin;
	vec3_t	chunkorigin;
	vec3_t	size;
	int		count;
	int		mass;

	// bmodel origins are (0 0 0), we need to adjust that here
	Vec3Scale (self->size, 0.5, size);
	Vec3Add (self->absMin, size, origin);
	Vec3Copy (origin, self->state.origin);

	self->takedamage = false;

	if (self->dmg)
		T_RadiusDamage (self, attacker, self->dmg, NULL, self->dmg+40, MOD_EXPLOSIVE);

	Vec3Subtract (self->state.origin, inflictor->state.origin, self->velocity);
	VectorNormalizef (self->velocity, self->velocity);
	Vec3Scale (self->velocity, 150, self->velocity);

	// start chunks towards the center
	Vec3Scale (size, 0.5, size);

	mass = self->mass;
	if (!mass)
		mass = 75;

	// big chunks
	if (mass >= 100)
	{
		count = mass / 100;
		if (count > 8)
			count = 8;
		while(count--)
		{
			chunkorigin[0] = origin[0] + crandom() * size[0];
			chunkorigin[1] = origin[1] + crandom() * size[1];
			chunkorigin[2] = origin[2] + crandom() * size[2];
			ThrowDebris (self, "models/objects/debris1/tris.md2", 1, chunkorigin);
		}
	}

	// small chunks
	count = mass / 25;
	if (count > 16)
		count = 16;
	while(count--)
	{
		chunkorigin[0] = origin[0] + crandom() * size[0];
		chunkorigin[1] = origin[1] + crandom() * size[1];
		chunkorigin[2] = origin[2] + crandom() * size[2];
		ThrowDebris (self, "models/objects/debris2/tris.md2", 2, chunkorigin);
	}

	G_UseTargets (self, attacker);

	if (self->dmg)
		BecomeExplosion1 (self);
	else
		G_FreeEdict (self);
}

void func_explosive_use(edict_t *self, edict_t *other, edict_t *activator)
{
	func_explosive_explode (self, self, other, self->health, vec3Origin);
}

void func_explosive_spawn (edict_t *self, edict_t *other, edict_t *activator)
{
	self->solid = SOLID_BSP;
	self->svFlags &= ~SVF_NOCLIENT;
	self->use = NULL;
	KillBox (self);
	gi.linkentity (self);
}

void SP_func_explosive (edict_t *self)
{
	if (game.mode & GAME_DEATHMATCH)
	{	// auto-remove for deathmatch
		G_FreeEdict (self);
		return;
	}

	self->movetype = MOVETYPE_PUSH;

	ModelIndex ("models/objects/debris1/tris.md2");
	ModelIndex ("models/objects/debris2/tris.md2");

	if (self->spawnflags & 1)
	{
		self->svFlags |= SVF_NOCLIENT;
		self->solid = SOLID_NOT;
		self->use = func_explosive_spawn;
	}
	else
	{
		self->solid = SOLID_BSP;
		if (self->targetname)
			self->use = func_explosive_use;
	}

	SetModel (self, self->model);

	if (self->spawnflags & 2)
		self->state.effects |= EF_ANIM_ALL;
	if (self->spawnflags & 4)
		self->state.effects |= EF_ANIM_ALLFAST;

	if (self->use != func_explosive_use)
	{
		if (!self->health)
			self->health = 100;
		self->die = func_explosive_explode;
		self->takedamage = true;
	}

	gi.linkentity (self);
}


#if 0
/*QUAKED misc_explobox (0 .5 .8) (-16 -16 0) (16 16 40)
Large exploding box.  You can override its mass (100),
health (80), and dmg (150).
*/

void barrel_touch (edict_t *self, edict_t *other, plane_t *plane, cmBspSurface_t *surf)

{
	float	ratio;
	vec3_t	v;

	if ((!other->groundentity) || (other->groundentity == self))
		return;

	ratio = (float)other->mass / (float)self->mass;

	Vec3Subtract (self->state.origin, other->state.origin, v);

	float Yaw = VecToYaw(v);
	Yaw = Yaw*M_PI*2 / 360;
	
	vec3_t move;
	move[0] = cosf(Yaw)*(20 * ratio);
	move[1] = sinf(Yaw)*(20 * ratio);
	move[2] = 0;

	vec3_t oldorg, neworg, end;
	Vec3Copy (self->state.origin, oldorg);
	Vec3Add (self->state.origin, move, neworg);

	int stepsize = 8;

	neworg[2] += stepsize;
	Vec3Copy (neworg, end);
	end[2] -= stepsize*2;

	CTrace trace;
	trace = CTrace (neworg, self->mins, self->maxs, end, self, CONTENTS_MASK_MONSTERSOLID);

	if (trace.allSolid)
		return;

	if (trace.startSolid)
	{
		neworg[2] -= stepsize;
		trace = CTrace (neworg, self->mins, self->maxs, end, self, CONTENTS_MASK_MONSTERSOLID);
		if (trace.allSolid || trace.startSolid)
			return;
	}

// check point traces down for dangling corners
	Vec3Copy (trace.endPos, self->state.origin);

	self->groundentity = trace.ent;
	self->groundentity_linkcount = trace.ent->linkCount;

	if (trace.fraction == 1.0)
		self->groundentity = NULL;

// the move is ok
	gi.linkentity (self);
	G_TouchTriggers (self);
}

void barrel_explode (edict_t *self)
{
	vec3_t	org;
	float	spd;
	vec3_t	save;

	T_RadiusDamage (self, self->activator, self->dmg, NULL, self->dmg+40, MOD_BARREL);

	Vec3Copy (self->state.origin, save);
	Vec3MA (self->absMin, 0.5, self->size, self->state.origin);

	// a few big chunks
	spd = 1.5 * (float)self->dmg / 200.0;
	org[0] = self->state.origin[0] + crandom() * self->size[0];
	org[1] = self->state.origin[1] + crandom() * self->size[1];
	org[2] = self->state.origin[2] + crandom() * self->size[2];
	ThrowDebris (self, "models/objects/debris1/tris.md2", spd, org);
	org[0] = self->state.origin[0] + crandom() * self->size[0];
	org[1] = self->state.origin[1] + crandom() * self->size[1];
	org[2] = self->state.origin[2] + crandom() * self->size[2];
	ThrowDebris (self, "models/objects/debris1/tris.md2", spd, org);

	// bottom corners
	spd = 1.75 * (float)self->dmg / 200.0;
	Vec3Copy (self->absMin, org);
	ThrowDebris (self, "models/objects/debris3/tris.md2", spd, org);
	Vec3Copy (self->absMin, org);
	org[0] += self->size[0];
	ThrowDebris (self, "models/objects/debris3/tris.md2", spd, org);
	Vec3Copy (self->absMin, org);
	org[1] += self->size[1];
	ThrowDebris (self, "models/objects/debris3/tris.md2", spd, org);
	Vec3Copy (self->absMin, org);
	org[0] += self->size[0];
	org[1] += self->size[1];
	ThrowDebris (self, "models/objects/debris3/tris.md2", spd, org);

	// a bunch of little chunks
	spd = 2 * self->dmg / 200;
	org[0] = self->state.origin[0] + crandom() * self->size[0];
	org[1] = self->state.origin[1] + crandom() * self->size[1];
	org[2] = self->state.origin[2] + crandom() * self->size[2];
	ThrowDebris (self, "models/objects/debris2/tris.md2", spd, org);
	org[0] = self->state.origin[0] + crandom() * self->size[0];
	org[1] = self->state.origin[1] + crandom() * self->size[1];
	org[2] = self->state.origin[2] + crandom() * self->size[2];
	ThrowDebris (self, "models/objects/debris2/tris.md2", spd, org);
	org[0] = self->state.origin[0] + crandom() * self->size[0];
	org[1] = self->state.origin[1] + crandom() * self->size[1];
	org[2] = self->state.origin[2] + crandom() * self->size[2];
	ThrowDebris (self, "models/objects/debris2/tris.md2", spd, org);
	org[0] = self->state.origin[0] + crandom() * self->size[0];
	org[1] = self->state.origin[1] + crandom() * self->size[1];
	org[2] = self->state.origin[2] + crandom() * self->size[2];
	ThrowDebris (self, "models/objects/debris2/tris.md2", spd, org);
	org[0] = self->state.origin[0] + crandom() * self->size[0];
	org[1] = self->state.origin[1] + crandom() * self->size[1];
	org[2] = self->state.origin[2] + crandom() * self->size[2];
	ThrowDebris (self, "models/objects/debris2/tris.md2", spd, org);
	org[0] = self->state.origin[0] + crandom() * self->size[0];
	org[1] = self->state.origin[1] + crandom() * self->size[1];
	org[2] = self->state.origin[2] + crandom() * self->size[2];
	ThrowDebris (self, "models/objects/debris2/tris.md2", spd, org);
	org[0] = self->state.origin[0] + crandom() * self->size[0];
	org[1] = self->state.origin[1] + crandom() * self->size[1];
	org[2] = self->state.origin[2] + crandom() * self->size[2];
	ThrowDebris (self, "models/objects/debris2/tris.md2", spd, org);
	org[0] = self->state.origin[0] + crandom() * self->size[0];
	org[1] = self->state.origin[1] + crandom() * self->size[1];
	org[2] = self->state.origin[2] + crandom() * self->size[2];
	ThrowDebris (self, "models/objects/debris2/tris.md2", spd, org);

	Vec3Copy (save, self->state.origin);
	if (self->groundentity)
		BecomeExplosion2 (self);
	else
		BecomeExplosion1 (self);
}

void barrel_delay (edict_t *self, edict_t *inflictor, edict_t *attacker, int damage, vec3_t point)
{
	self->takedamage = false;
	self->nextthink = level.framenum + 2;
	self->think = barrel_explode;
	self->activator = attacker;
}

void SP_misc_explobox_ (edict_t *self)
{
	if (game.mode & GAME_DEATHMATCH)
	{	// auto-remove for deathmatch
		G_FreeEdict (self);
		return;
	}

	ModelIndex ("models/objects/debris1/tris.md2");
	ModelIndex ("models/objects/debris2/tris.md2");
	ModelIndex ("models/objects/debris3/tris.md2");

	self->solid = SOLID_BBOX;
	self->movetype = MOVETYPE_STEP;

	self->model = "models/objects/barrels/tris.md2";
	self->state.modelIndex = ModelIndex (self->model);
	Vec3Set (self->mins, -16, -16, 0);
	Vec3Set (self->maxs, 16, 16, 40);

	if (!self->mass)
		self->mass = 400;
	if (!self->health)
		self->health = 10;
	if (!self->dmg)
		self->dmg = 150;

	self->die = barrel_delay;
	self->takedamage = true;

	self->touch = barrel_touch;

	gi.linkentity (self);
}
#endif

//
// miscellaneous specialty items
//

/*QUAKED misc_blackhole (1 .5 0) (-8 -8 -8) (8 8 8)
*/

void misc_blackhole_use (edict_t *ent, edict_t *other, edict_t *activator)
{
	G_FreeEdict (ent);
}

void misc_blackhole_think (edict_t *self)
{
	if (++self->state.frame < 19)
		self->nextthink = level.framenum + FRAMETIME;
	else
	{		
		self->state.frame = 0;
		self->nextthink = level.framenum + FRAMETIME;
	}
}

void SP_misc_blackhole (edict_t *ent)
{
	ent->movetype = MOVETYPE_NONE;
	ent->solid = SOLID_NOT;
	Vec3Set (ent->mins, -64, -64, 0);
	Vec3Set (ent->maxs, 64, 64, 8);
	ent->state.modelIndex = ModelIndex ("models/objects/black/tris.md2");
	ent->state.renderFx = RF_TRANSLUCENT;
	ent->use = misc_blackhole_use;
	ent->think = misc_blackhole_think;
	ent->nextthink = level.framenum + 2;
	gi.linkentity (ent);
}

/*QUAKED misc_eastertank (1 .5 0) (-32 -32 -16) (32 32 32)
*/

void misc_eastertank_think (edict_t *self)
{
	if (++self->state.frame < 293)
		self->nextthink = level.framenum + FRAMETIME;
	else
	{		
		self->state.frame = 254;
		self->nextthink = level.framenum + FRAMETIME;
	}
}

void SP_misc_eastertank (edict_t *ent)
{
	ent->movetype = MOVETYPE_NONE;
	ent->solid = SOLID_BBOX;
	Vec3Set (ent->mins, -32, -32, -16);
	Vec3Set (ent->maxs, 32, 32, 32);
	ent->state.modelIndex = ModelIndex ("models/monsters/tank/tris.md2");
	ent->state.frame = 254;
	ent->think = misc_eastertank_think;
	ent->nextthink = level.framenum + 2;
	gi.linkentity (ent);
}

/*QUAKED misc_easterchick (1 .5 0) (-32 -32 0) (32 32 32)
*/


void misc_easterchick_think (edict_t *self)
{
	if (++self->state.frame < 247)
		self->nextthink = level.framenum + FRAMETIME;
	else
	{		
		self->state.frame = 208;
		self->nextthink = level.framenum + FRAMETIME;
	}
}

void SP_misc_easterchick (edict_t *ent)
{
	ent->movetype = MOVETYPE_NONE;
	ent->solid = SOLID_BBOX;
	Vec3Set (ent->mins, -32, -32, 0);
	Vec3Set (ent->maxs, 32, 32, 32);
	ent->state.modelIndex = ModelIndex ("models/monsters/bitch/tris.md2");
	ent->state.frame = 208;
	ent->think = misc_easterchick_think;
	ent->nextthink = level.framenum + 2;
	gi.linkentity (ent);
}

/*QUAKED misc_easterchick2 (1 .5 0) (-32 -32 0) (32 32 32)
*/


void misc_easterchick2_think (edict_t *self)
{
	if (++self->state.frame < 287)
		self->nextthink = level.framenum + FRAMETIME;
	else
	{		
		self->state.frame = 248;
		self->nextthink = level.framenum + FRAMETIME;
	}
}

void SP_misc_easterchick2 (edict_t *ent)
{
	ent->movetype = MOVETYPE_NONE;
	ent->solid = SOLID_BBOX;
	Vec3Set (ent->mins, -32, -32, 0);
	Vec3Set (ent->maxs, 32, 32, 32);
	ent->state.modelIndex = ModelIndex ("models/monsters/bitch/tris.md2");
	ent->state.frame = 248;
	ent->think = misc_easterchick2_think;
	ent->nextthink = level.framenum + 2;
	gi.linkentity (ent);
}


/*QUAKED monster_commander_body (1 .5 0) (-32 -32 0) (32 32 48)
Not really a monster, this is the Tank Commander's decapitated body.
There should be a item_commander_head that has this as it's target.
*/

void commander_body_think (edict_t *self)
{
	if (++self->state.frame < 24)
		self->nextthink = level.framenum + FRAMETIME;
	else
		self->nextthink = 0;

	if (self->state.frame == 22)
		PlaySoundFrom (self, CHAN_BODY, SoundIndex ("tank/thud.wav"));
}

void commander_body_use (edict_t *self, edict_t *other, edict_t *activator)
{
	self->think = commander_body_think;
	self->nextthink = level.framenum + FRAMETIME;
	PlaySoundFrom (self, CHAN_BODY, SoundIndex ("tank/pain.wav"));
}

void commander_body_drop (edict_t *self)
{
	self->movetype = MOVETYPE_TOSS;
	self->state.origin[2] += 2;
}

void SP_monster_commander_body (edict_t *self)
{
	self->movetype = MOVETYPE_NONE;
	self->solid = SOLID_BBOX;
	self->model = "models/monsters/commandr/tris.md2";
	self->state.modelIndex = ModelIndex (self->model);
	Vec3Set (self->mins, -32, -32, 0);
	Vec3Set (self->maxs, 32, 32, 48);
	self->use = commander_body_use;
	self->takedamage = true;
	self->flags = FL_GODMODE;
	self->state.renderFx |= RF_FRAMELERP;
	gi.linkentity (self);

	SoundIndex ("tank/thud.wav");
	SoundIndex ("tank/pain.wav");

	self->think = commander_body_drop;
	self->nextthink = level.framenum + 5;
}


/*QUAKED misc_banner (1 .5 0) (-4 -4 -4) (4 4 4)
The origin is the bottom of the banner.
The banner is 128 tall.
*/
void misc_banner_think (edict_t *ent)
{
	ent->state.frame = (ent->state.frame + 1) % 16;
	ent->nextthink = level.framenum + FRAMETIME;
}

void SP_misc_banner (edict_t *ent)
{
	ent->movetype = MOVETYPE_NONE;
	ent->solid = SOLID_NOT;
	ent->state.modelIndex = ModelIndex ("models/objects/banner/tris.md2");
	ent->state.frame = rand() % 16;
	gi.linkentity (ent);

	ent->think = misc_banner_think;
	ent->nextthink = level.framenum + FRAMETIME;
}

/*QUAKED misc_deadsoldier (1 .5 0) (-16 -16 0) (16 16 16) ON_BACK ON_STOMACH BACK_DECAP FETAL_POS SIT_DECAP IMPALED
This is the dead player model. Comes in 6 exciting different poses!
*/
void misc_deadsoldier_die (edict_t *self, edict_t *inflictor, edict_t *attacker, int damage, vec3_t point)
{
//	int		n;

	if (self->health > -80)
		return;

	PlaySoundFrom (self, CHAN_BODY, SoundIndex ("misc/udeath.wav"));
	//for (n= 0; n < 4; n++)
		//ThrowGib (self, gMedia.Gib_SmallMeat, damage, GIB_ORGANIC);
	//ThrowHead (self, gMedia.Gib_Head[1], damage, GIB_ORGANIC);
}

void SP_misc_deadsoldier (edict_t *ent)
{
	if (game.mode & GAME_DEATHMATCH)
	{	// auto-remove for deathmatch
		G_FreeEdict (ent);
		return;
	}

	ent->movetype = MOVETYPE_NONE;
	ent->solid = SOLID_BBOX;
	ent->state.modelIndex=ModelIndex ("models/deadbods/dude/tris.md2");

	// Defaults to frame 0
	if (ent->spawnflags & 2)
		ent->state.frame = 1;
	else if (ent->spawnflags & 4)
		ent->state.frame = 2;
	else if (ent->spawnflags & 8)
		ent->state.frame = 3;
	else if (ent->spawnflags & 16)
		ent->state.frame = 4;
	else if (ent->spawnflags & 32)
		ent->state.frame = 5;
	else
		ent->state.frame = 0;

	Vec3Set (ent->mins, -16, -16, 0);
	Vec3Set (ent->maxs, 16, 16, 16);
	ent->deadflag = DEAD_DEAD;
	ent->takedamage = true;
	ent->svFlags |= SVF_MONSTER|SVF_DEADMONSTER;
	ent->die = misc_deadsoldier_die;

	gi.linkentity (ent);
}

/*QUAKED misc_viper (1 .5 0) (-16 -16 0) (16 16 32)
This is the Viper for the flyby bombing.
It is trigger_spawned, so you must have something use it for it to show up.
There must be a path for it to follow once it is activated.

"speed"		How fast the Viper should fly
*/

extern void train_use (edict_t *self, edict_t *other, edict_t *activator);
extern void func_train_find (edict_t *self);

void misc_viper_use  (edict_t *self, edict_t *other, edict_t *activator)
{
	self->svFlags &= ~SVF_NOCLIENT;
	self->use = train_use;
	train_use (self, other, activator);
}

void SP_misc_viper (edict_t *ent)
{
	if (!ent->target)
	{
		//gi.dprintf ("misc_viper without a target at (%f %f %f)\n", ent->absMin[0], ent->absMin[1], ent->absMin[2]);
		MapPrint (MAPPRINT_ERROR, ent, ent->state.origin, "No targetname\n");
		G_FreeEdict (ent);
		return;
	}

	if (!ent->speed)
		ent->speed = 300;

	ent->movetype = MOVETYPE_PUSH;
	ent->solid = SOLID_NOT;
	ent->state.modelIndex = ModelIndex ("models/ships/viper/tris.md2");
	Vec3Set (ent->mins, -16, -16, 0);
	Vec3Set (ent->maxs, 16, 16, 32);

	ent->think = func_train_find;
	ent->nextthink = level.framenum + FRAMETIME;
	ent->use = misc_viper_use;
	ent->svFlags |= SVF_NOCLIENT;
	ent->moveinfo.accel = ent->moveinfo.decel = ent->moveinfo.speed = ent->speed;

	gi.linkentity (ent);
}


/*QUAKED misc_bigviper (1 .5 0) (-176 -120 -24) (176 120 72) 
This is a large stationary viper as seen in Paul's intro
*/
void SP_misc_bigviper (edict_t *ent)
{
	ent->movetype = MOVETYPE_NONE;
	ent->solid = SOLID_BBOX;
	Vec3Set (ent->mins, -176, -120, -24);
	Vec3Set (ent->maxs, 176, 120, 72);
	ent->state.modelIndex = ModelIndex ("models/ships/bigviper/tris.md2");
	gi.linkentity (ent);
}


/*QUAKED misc_viper_bomb (1 0 0) (-8 -8 -8) (8 8 8)
"dmg"	how much boom should the bomb make?
*/
void misc_viper_bomb_touch (edict_t *self, edict_t *other, plane_t *plane, cmBspSurface_t *surf)
{
	G_UseTargets (self, self->activator);

	self->state.origin[2] = self->absMin[2] + 1;
	T_RadiusDamage (self, self, self->dmg, NULL, self->dmg+40, MOD_BOMB);
	BecomeExplosion2 (self);
}

void misc_viper_bomb_prethink (edict_t *self)
{
	vec3_t	v;
	float	diff;

	self->groundentity = NULL;

	diff = self->timestamp - level.framenum;
	if (diff < -1.0)
		diff = -1.0;

	Vec3Scale (self->moveinfo.dir, 1.0 + diff, v);
	v[2] = diff;

	diff = self->state.angles[2];
	VecToAngles (v, self->state.angles);
	self->state.angles[2] = diff + 10;
}

void misc_viper_bomb_use (edict_t *self, edict_t *other, edict_t *activator)
{
	edict_t	*viper;

	self->solid = SOLID_BBOX;
	self->svFlags &= ~SVF_NOCLIENT;
	self->state.effects |= EF_ROCKET;
	self->use = NULL;
	self->movetype = MOVETYPE_TOSS;
	self->prethink = misc_viper_bomb_prethink;
	self->touch = misc_viper_bomb_touch;
	self->activator = activator;

	viper = G_Find (NULL, FOFS(classname), "misc_viper");
	Vec3Scale (viper->moveinfo.dir, viper->moveinfo.speed, self->velocity);

	self->timestamp = level.framenum;
	Vec3Copy (viper->moveinfo.dir, self->moveinfo.dir);
}

void SP_misc_viper_bomb (edict_t *self)
{
	self->movetype = MOVETYPE_NONE;
	self->solid = SOLID_NOT;
	Vec3Set (self->mins, -8, -8, -8);
	Vec3Set (self->maxs, 8, 8, 8);

	self->state.modelIndex = ModelIndex ("models/objects/bomb/tris.md2");

	if (!self->dmg)
		self->dmg = 1000;

	self->use = misc_viper_bomb_use;
	self->svFlags |= SVF_NOCLIENT;

	gi.linkentity (self);
}


/*QUAKED misc_strogg_ship (1 .5 0) (-16 -16 0) (16 16 32)
This is a Storgg ship for the flybys.
It is trigger_spawned, so you must have something use it for it to show up.
There must be a path for it to follow once it is activated.

"speed"		How fast it should fly
*/

extern void train_use (edict_t *self, edict_t *other, edict_t *activator);
extern void func_train_find (edict_t *self);

void misc_strogg_ship_use  (edict_t *self, edict_t *other, edict_t *activator)
{
	self->svFlags &= ~SVF_NOCLIENT;
	self->use = train_use;
	train_use (self, other, activator);
}

void SP_misc_strogg_ship (edict_t *ent)
{
	if (!ent->target)
	{
		//gi.dprintf ("%s without a target at (%f %f %f)\n", ent->classname, ent->absMin[0], ent->absMin[1], ent->absMin[2]);
		MapPrint (MAPPRINT_ERROR, ent, ent->state.origin, "No target\n");
		G_FreeEdict (ent);
		return;
	}

	if (!ent->speed)
		ent->speed = 300;

	ent->movetype = MOVETYPE_PUSH;
	ent->solid = SOLID_NOT;
	ent->state.modelIndex = ModelIndex ("models/ships/strogg1/tris.md2");
	Vec3Set (ent->mins, -16, -16, 0);
	Vec3Set (ent->maxs, 16, 16, 32);

	ent->think = func_train_find;
	ent->nextthink = level.framenum + FRAMETIME;
	ent->use = misc_strogg_ship_use;
	ent->svFlags |= SVF_NOCLIENT;
	ent->moveinfo.accel = ent->moveinfo.decel = ent->moveinfo.speed = ent->speed;

	gi.linkentity (ent);
}


/*QUAKED misc_satellite_dish (1 .5 0) (-64 -64 0) (64 64 128)
*/
void misc_satellite_dish_think (edict_t *self)
{
	self->state.frame++;
	if (self->state.frame < 38)
		self->nextthink = level.framenum + FRAMETIME;
}

void misc_satellite_dish_use (edict_t *self, edict_t *other, edict_t *activator)
{
	self->state.frame = 0;
	self->think = misc_satellite_dish_think;
	self->nextthink = level.framenum + FRAMETIME;
}

void SP_misc_satellite_dish (edict_t *ent)
{
	ent->movetype = MOVETYPE_NONE;
	ent->solid = SOLID_BBOX;
	Vec3Set (ent->mins, -64, -64, 0);
	Vec3Set (ent->maxs, 64, 64, 128);
	ent->state.modelIndex = ModelIndex ("models/objects/satellite/tris.md2");
	ent->use = misc_satellite_dish_use;
	gi.linkentity (ent);
}


/*QUAKED light_mine1 (0 1 0) (-2 -2 -12) (2 2 12)
*/
void SP_light_mine1 (edict_t *ent)
{
	ent->movetype = MOVETYPE_NONE;
	ent->solid = SOLID_BBOX;
	ent->state.modelIndex = ModelIndex ("models/objects/minelite/light1/tris.md2");
	gi.linkentity (ent);
}


/*QUAKED light_mine2 (0 1 0) (-2 -2 -12) (2 2 12)
*/
void SP_light_mine2 (edict_t *ent)
{
	ent->movetype = MOVETYPE_NONE;
	ent->solid = SOLID_BBOX;
	ent->state.modelIndex = ModelIndex ("models/objects/minelite/light2/tris.md2");
	gi.linkentity (ent);
}


/*QUAKED misc_gib_arm (1 0 0) (-8 -8 -8) (8 8 8)
Intended for use with the target_spawner
*/
void SP_misc_gib_arm (edict_t *ent)
{
	ent->state.modelIndex = gMedia.Gib_Arm;
	ent->solid = SOLID_NOT;
	ent->state.effects |= EF_GIB;
	ent->takedamage = true;
//	ent->die = gib_die;
	ent->movetype = MOVETYPE_TOSS;
	ent->svFlags |= SVF_MONSTER;
	ent->deadflag = DEAD_DEAD;
	ent->avelocity[0] = random()*200;
	ent->avelocity[1] = random()*200;
	ent->avelocity[2] = random()*200;
	ent->think = G_FreeEdict;
	ent->nextthink = level.framenum + 300;
	gi.linkentity (ent);
}

/*QUAKED misc_gib_leg (1 0 0) (-8 -8 -8) (8 8 8)
Intended for use with the target_spawner
*/
void SP_misc_gib_leg (edict_t *ent)
{
	ent->state.modelIndex = gMedia.Gib_Leg;
	ent->solid = SOLID_NOT;
	ent->state.effects |= EF_GIB;
	ent->takedamage = true;
//	ent->die = gib_die;
	ent->movetype = MOVETYPE_TOSS;
	ent->svFlags |= SVF_MONSTER;
	ent->deadflag = DEAD_DEAD;
	ent->avelocity[0] = random()*200;
	ent->avelocity[1] = random()*200;
	ent->avelocity[2] = random()*200;
	ent->think = G_FreeEdict;
	ent->nextthink = level.framenum + 300;
	gi.linkentity (ent);
}

/*QUAKED misc_gib_head (1 0 0) (-8 -8 -8) (8 8 8)
Intended for use with the target_spawner
*/
void SP_misc_gib_head (edict_t *ent)
{
	ent->state.modelIndex = gMedia.Gib_Head[0];
	ent->solid = SOLID_NOT;
	ent->state.effects |= EF_GIB;
	ent->takedamage = true;
//	ent->die = gib_die;
	ent->movetype = MOVETYPE_TOSS;
	ent->svFlags |= SVF_MONSTER;
	ent->deadflag = DEAD_DEAD;
	ent->avelocity[0] = random()*200;
	ent->avelocity[1] = random()*200;
	ent->avelocity[2] = random()*200;
	ent->think = G_FreeEdict;
	ent->nextthink = level.framenum + 300;
	gi.linkentity (ent);
}

//=====================================================

/*QUAKED target_character (0 0 1) ?
used with target_string (must be on same "team")
"count" is position in the string (starts at 1)
*/

void SP_target_character (edict_t *self)
{
	self->movetype = MOVETYPE_PUSH;
	SetModel (self, self->model);
	self->solid = SOLID_BSP;
	self->state.frame = 12;
	gi.linkentity (self);
	return;
}


/*QUAKED target_string (0 0 1) (-8 -8 -8) (8 8 8)
*/

void target_string_use (edict_t *self, edict_t *other, edict_t *activator)
{
	size_t l = strlen(self->message);
	for (edict_t *e = self->teammaster; e; e = e->teamchain)
	{
		if (!e->count)
			continue;
		size_t n = e->count - 1;
		if (n > l)
		{
			e->state.frame = 12;
			continue;
		}

		char c = self->message[n];
		if (c >= '0' && c <= '9')
			e->state.frame = c - '0';
		else if (c == '-')
			e->state.frame = 10;
		else if (c == ':')
			e->state.frame = 11;
		else
			e->state.frame = 12;
	}
}

void SP_target_string (edict_t *self)
{
	if (!self->message)
		self->message = "";
	self->use = target_string_use;
}


/*QUAKED func_clock (0 0 1) (-8 -8 -8) (8 8 8) TIMER_UP TIMER_DOWN START_OFF MULTI_USE
target a target_string with this

The default is to be a time of day clock

TIMER_UP and TIMER_DOWN run for "count" seconds and the fire "pathtarget"
If START_OFF, this entity must be used before it starts

"style"		0 "xx"
			1 "xx:xx"
			2 "xx:xx:xx"
*/

#define CLOCK_MESSAGE_SIZE	16

// don't let field width of any clock messages change, or it
// could cause an overwrite after a game load

static void func_clock_reset (edict_t *self)
{
	self->activator = NULL;
	if (self->spawnflags & 1)
	{
		self->health = 0;
		self->wait = self->count;
	}
	else if (self->spawnflags & 2)
	{
		self->health = self->count;
		self->wait = 0;
	}
}

static void func_clock_format_countdown (edict_t *self)
{
	if (self->style == 0)
	{
		Q_snprintfz (self->message, CLOCK_MESSAGE_SIZE, "%2i", self->health);
		return;
	}

	if (self->style == 1)
	{
		Q_snprintfz(self->message, CLOCK_MESSAGE_SIZE, "%2i:%2i", self->health / 60, self->health % 60);
		if (self->message[3] == ' ')
			self->message[3] = '0';
		return;
	}

	if (self->style == 2)
	{
		Q_snprintfz(self->message, CLOCK_MESSAGE_SIZE, "%2i:%2i:%2i", self->health / 3600, (self->health - (self->health / 3600) * 3600) / 60, self->health % 60);
		if (self->message[3] == ' ')
			self->message[3] = '0';
		if (self->message[6] == ' ')
			self->message[6] = '0';
		return;
	}
}

void func_clock_think (edict_t *self)
{
	if (!self->enemy)
	{
		self->enemy = G_Find (NULL, FOFS(targetname), self->target);
		if (!self->enemy)
			return;
	}

	if (self->spawnflags & 1)
	{
		func_clock_format_countdown (self);
		self->health++;
	}
	else if (self->spawnflags & 2)
	{
		func_clock_format_countdown (self);
		self->health--;
	}
	else
	{
		struct tm	ltime;
		time_t		gmtime;

		time(&gmtime);
		localtime_s (&ltime, &gmtime);
		Q_snprintfz (self->message, CLOCK_MESSAGE_SIZE, "%2i:%2i:%2i", ltime.tm_hour, ltime.tm_min, ltime.tm_sec);
		if (self->message[3] == ' ')
			self->message[3] = '0';
		if (self->message[6] == ' ')
			self->message[6] = '0';
	}

	self->enemy->message = self->message;
	self->enemy->use (self->enemy, self, self);

	if (((self->spawnflags & 1) && (self->health > self->wait)) ||
		((self->spawnflags & 2) && (self->health < self->wait)))
	{
		if (self->pathtarget)
		{
			char *savetarget;
			char *savemessage;

			savetarget = self->target;
			savemessage = self->message;
			self->target = self->pathtarget;
			self->message = NULL;
			G_UseTargets (self, self->activator);
			self->target = savetarget;
			self->message = savemessage;
		}

		if (!(self->spawnflags & 8))
			return;

		func_clock_reset (self);

		if (self->spawnflags & 4)
			return;
	}

	self->nextthink = level.framenum + 10;
}

void func_clock_use (edict_t *self, edict_t *other, edict_t *activator)
{
	if (!(self->spawnflags & 8))
		self->use = NULL;
	if (self->activator)
		return;
	self->activator = activator;
	self->think (self);
}

void SP_func_clock (edict_t *self)
{
	if (!self->target)
	{
		//gi.dprintf("%s with no target at (%f %f %f)\n", self->classname, self->state.origin[0], self->state.origin[1], self->state.origin[2]);
		MapPrint (MAPPRINT_ERROR, self, self->absMin, "No target\n");
		G_FreeEdict (self);
		return;
	}

	if ((self->spawnflags & 2) && (!self->count))
	{
		//gi.dprintf("%s with no count at (%f %f %f)\n", self->classname, self->state.origin[0], self->state.origin[1], self->state.origin[2]);
		MapPrint (MAPPRINT_ERROR, self, self->absMin, "No count\n");
		G_FreeEdict (self);
		return;
	}

	if ((self->spawnflags & 1) && (!self->count))
		self->count = 60*60;;

	func_clock_reset (self);

	self->message = QNew (com_levelPool, 0) char[CLOCK_MESSAGE_SIZE];//(char*)gi.TagMalloc (CLOCK_MESSAGE_SIZE, TAG_LEVEL);

	self->think = func_clock_think;

	if (self->spawnflags & 4)
		self->use = func_clock_use;
	else
		self->nextthink = level.framenum + 10;
}

//=================================================================================

void SP_misc_model (edict_t *ent)
{
	G_FreeEdict (ent);
}

//=================================================================================

void teleporter_touch (edict_t *self, edict_t *other, plane_t *plane, cmBspSurface_t *surf)
{
	edict_t		*dest;

	dest = G_Find (NULL, FOFS(targetname), self->target);
	if (!dest)
	{
		DebugPrintf ("Couldn't find destination\n");
		return;
	}

	CPlayerEntity	*Player = NULL;
	if (other->Entity)
		Player = dynamic_cast<CPlayerEntity*>(other->Entity);

#ifdef CLEANCTF_ENABLED
	//ZOID
	if (Player)
		CGrapple::PlayerResetGrapple(Player);
	//ZOID
#endif

	// unlink to make sure it can't possibly interfere with KillBox
	gi.unlinkentity (other);

	Vec3Copy (dest->state.origin, other->state.origin);
	Vec3Copy (dest->state.origin, other->state.oldOrigin);
	other->state.origin[2] += 10;

	// clear the velocity and hold them in place briefly
	Vec3Clear (other->velocity);
	if (Player)
	{
		Player->Client.PlayerState.GetPMove()->pmTime = 160>>3;		// hold time
		Player->Client.PlayerState.GetPMove()->pmFlags |= PMF_TIME_TELEPORT;
	}

	// draw the teleport splash at source and on the player
	if (Player)
		other->state.event = EV_PLAYER_TELEPORT;
	else
		other->state.event = EV_OTHER_TELEPORT;

	// set angles
	if (other->client)
	{
		for (int i=0 ; i<3 ; i++)
			Player->Client.PlayerState.GetPMove()->deltaAngles[i] = ANGLE2SHORT(dest->state.angles[i] - Player->Client.resp.cmd_angles[i]);
	}

	Vec3Clear (other->state.angles);
	if (Player)
	{
		Player->Client.PlayerState.SetViewAngles (vec3Origin);
		Vec3Clear (Player->Client.v_angle);
	}

	// kill anything at the destination
	KillBox (other);

	gi.linkentity (other);
}

/*QUAKED misc_teleporter (1 0 0) (-32 -32 -24) (32 32 -16)
Stepping onto this disc will teleport players to the targeted misc_teleporter_dest object.
*/
void SP_misc_teleporter (edict_t *ent)
{
	edict_t		*trig;

	if (!ent->target)
	{
		//gi.dprintf ("teleporter without a target.\n");
		MapPrint (MAPPRINT_ERROR, ent, ent->state.origin, "No target\n");
		G_FreeEdict (ent);
		return;
	}

	ent->state.modelIndex = ModelIndex("models/objects/dmspot/tris.md2");
	ent->state.skinNum = 1;
	ent->state.effects = EF_TELEPORTER;
	ent->state.sound = SoundIndex ("world/amb10.wav");
	ent->solid = SOLID_BBOX;

	Vec3Set (ent->mins, -32, -32, -24);
	Vec3Set (ent->maxs, 32, 32, -16);
	gi.linkentity (ent);

	trig = G_Spawn ();
	trig->touch = teleporter_touch;
	trig->solid = SOLID_TRIGGER;
	trig->target = ent->target;
	trig->owner = ent;
	Vec3Copy (ent->state.origin, trig->state.origin);
	Vec3Set (trig->mins, -8, -8, 8);
	Vec3Set (trig->maxs, 8, 8, 24);
	gi.linkentity (trig);
	
}

/*QUAKED misc_teleporter_dest (1 0 0) (-32 -32 -24) (32 32 -16)
Point teleporters at these.
*/
void SP_misc_teleporter_dest (edict_t *ent)
{
	ent->state.modelIndex = ModelIndex("models/objects/dmspot/tris.md2");
	ent->state.skinNum = 0;
	ent->solid = SOLID_BBOX;
	Vec3Set (ent->mins, -32, -32, -24);
	Vec3Set (ent->maxs, 32, 32, -16);
	gi.linkentity (ent);
}
