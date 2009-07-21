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
// g_phys.c

#include "g_local.h"

/*


pushmove objects do not obey gravity, and do not interact with each other or trigger fields, but block normal movement and push normal objects when they move.

onground is set for toss objects when they come to a complete rest.  it is set for steping or walking objects 

doors, plats, etc are SOLID_BSP, and MOVETYPE_PUSH
bonus items are SOLID_TRIGGER touch, and MOVETYPE_TOSS
corpses are SOLID_NOT and MOVETYPE_TOSS
crates are SOLID_BBOX and MOVETYPE_TOSS
walking monsters are SOLID_SLIDEBOX and MOVETYPE_STEP
flying/floating monsters are SOLID_SLIDEBOX and MOVETYPE_FLY

solid_edge items only clip against bsp models.

*/


/*
============
SV_TestEntityPosition

============
*/
edict_t	*SV_TestEntityPosition (edict_t *ent)
{
	CTrace	trace;
	int			mask;

	if (ent->clipMask)
		mask = ent->clipMask;
	else
		mask = CONTENTS_MASK_SOLID;
	trace = CTrace (ent->state.origin, ent->mins, ent->maxs, ent->state.origin, ent, mask);
	
	if (trace.startSolid)
		return g_edicts;
		
	return NULL;
}

/*
=============
SV_RunThink

Runs thinking code for this frame if necessary
=============
*/
bool SV_RunThink (edict_t *ent)
{
	int32	thinktime = ent->nextthink;
	if (thinktime <= 0)
		return true;
	if (thinktime > level.framenum)
		return true;
	
	ent->nextthink = 0;
	if (ent->think)
		ent->think (ent);

	return false;
}

/*
==================
SV_Impact

Two entities have touched, so run their touch functions
==================
*/
void SV_Impact (edict_t *e1, CTrace *trace)
{
	edict_t		*e2;
//	plane_t	backplane;

	e2 = trace->ent;

	if (e1->touch && e1->solid != SOLID_NOT)
		e1->touch (e1, e2, &trace->plane, trace->surface);
	
	if (e2->touch && e2->solid != SOLID_NOT)
		e2->touch (e2, e1, NULL, NULL);
}


/*
==================
ClipVelocity

Slide off of the impacting object
returns the blocked flags (1 = floor, 2 = step / wall)
==================
*/
#define STOP_EPSILON	0.1

int ClipVelocity (vec3_t in, vec3_t normal, vec3_t out, float overbounce)
{
	float	backoff;
	float	change;
	int		i, blocked;
	
	blocked = 0;
	if (normal[2] > 0)
		blocked |= 1;		// floor
	if (!normal[2])
		blocked |= 2;		// step
	
	backoff = Dot3Product (in, normal) * overbounce;

	for (i=0 ; i<3 ; i++)
	{
		change = normal[i]*backoff;
		out[i] = in[i] - change;
		if (out[i] > -STOP_EPSILON && out[i] < STOP_EPSILON)
			out[i] = 0;
	}

	return blocked;
}


/*
============
SV_FlyMove

The basic solid body movement clip that slides along multiple planes
Returns the clipflags if the velocity was modified (hit something solid)
1 = floor
2 = wall / step
4 = dead stop
============
*/
#define MAX_CLIP_PLANES	5
int SV_FlyMove (edict_t *ent, float time, int mask)
{
	edict_t		*hit;
	int			bumpcount, numbumps;
	vec3_t		dir;
	float		d;
	int			numplanes;
	vec3_t		planes[MAX_CLIP_PLANES];
	vec3_t		primal_velocity, original_velocity, new_velocity;
	int			i, j;
	CTrace	trace;
	vec3_t		end;
	float		time_left;
	int			blocked;
	
	numbumps = 4;
	
	blocked = 0;
	Vec3Copy (ent->velocity, original_velocity);
	Vec3Copy (ent->velocity, primal_velocity);
	numplanes = 0;
	
	time_left = time;

	ent->groundentity = NULL;
	for (bumpcount=0 ; bumpcount<numbumps ; bumpcount++)
	{
		for (i=0 ; i<3 ; i++)
			end[i] = ent->state.origin[i] + time_left * ent->velocity[i];

		trace = CTrace (ent->state.origin, ent->mins, ent->maxs, end, ent, mask);

		if (trace.allSolid)
		{	// entity is trapped in another solid
			Vec3Copy (vec3Origin, ent->velocity);
			return 3;
		}

		if (trace.fraction > 0)
		{	// actually covered some distance
			Vec3Copy (trace.endPos, ent->state.origin);
			Vec3Copy (ent->velocity, original_velocity);
			numplanes = 0;
		}

		if (trace.fraction == 1)
			 break;		// moved the entire distance

		hit = trace.ent;

		if (trace.plane.normal[2] > 0.7)
		{
			blocked |= 1;		// floor
			if ( hit->solid == SOLID_BSP)
			{
				ent->groundentity = hit;
				ent->groundentity_linkcount = hit->linkCount;
			}
		}
		if (!trace.plane.normal[2])
		{
			blocked |= 2;		// step
		}

//
// run the impact function
//
		SV_Impact (ent, &trace);
		if (!ent->inUse)
			break;		// removed by the impact function

		
		time_left -= time_left * trace.fraction;
		
	// cliped to another plane
		if (numplanes >= MAX_CLIP_PLANES)
		{	// this shouldn't really happen
			Vec3Copy (vec3Origin, ent->velocity);
			return 3;
		}

		Vec3Copy (trace.plane.normal, planes[numplanes]);
		numplanes++;

//
// modify original_velocity so it parallels all of the clip planes
//
		for (i=0 ; i<numplanes ; i++)
		{
			ClipVelocity (original_velocity, planes[i], new_velocity, 1);

			for (j=0 ; j<numplanes ; j++)
				if ((j != i) && !Vec3Compare (planes[i], planes[j]))
				{
					if (Dot3Product (new_velocity, planes[j]) < 0)
						break;	// not ok
				}
			if (j == numplanes)
				break;
		}
		
		if (i != numplanes)
		{	// go along this plane
			Vec3Copy (new_velocity, ent->velocity);
		}
		else
		{	// go along the crease
			if (numplanes != 2)
			{
//				gi.dprintf ("clip velocity, numplanes == %i\n",numplanes);
				Vec3Copy (vec3Origin, ent->velocity);
				return 7;
			}
			CrossProduct (planes[0], planes[1], dir);
			d = Dot3Product (dir, ent->velocity);
			Vec3Scale (dir, d, ent->velocity);
		}

//
// if original velocity is against the original velocity, stop dead
// to avoid tiny occilations in sloping corners
//
		if (Dot3Product (ent->velocity, primal_velocity) <= 0)
		{
			Vec3Copy (vec3Origin, ent->velocity);
			return blocked;
		}
	}

	return blocked;
}


/*
============
SV_AddGravity

============
*/
void SV_AddGravity (edict_t *ent)
{
	ent->velocity[2] -= ent->gravity * sv_gravity->Float() * 0.1f;
}

/*
===============================================================================

PUSHMOVE

===============================================================================
*/

/*
============
SV_PushEntity

Does not change the entities velocity at all
============
*/
CTrace SV_PushEntity (edict_t *ent, vec3_t push)
{
	CTrace	trace;
	vec3_t		start;
	vec3_t		end;
	int			mask;

	Vec3Copy (ent->state.origin, start);
	Vec3Add (start, push, end);

retry:
	if (ent->clipMask)
		mask = ent->clipMask;
	else
		mask = CONTENTS_MASK_SOLID;

	trace = CTrace (start, ent->mins, ent->maxs, end, ent, mask);
	
	Vec3Copy (trace.endPos, ent->state.origin);
	gi.linkentity (ent);

	if (trace.fraction != 1.0)
	{
		SV_Impact (ent, &trace);

		// if the pushed entity went away and the pusher is still there
		if (!trace.ent->inUse && ent->inUse)
		{
			// move the pusher back and try again
			Vec3Copy (start, ent->state.origin);
			gi.linkentity (ent);
			goto retry;
		}
	}

	if (ent->inUse)
		G_TouchTriggers (ent);

	return trace;
}					


typedef struct
{
	edict_t	*ent;
	vec3_t	origin;
	vec3_t	angles;
	float	deltayaw;
} pushed_t;
pushed_t	pushed[MAX_CS_EDICTS], *pushed_p;

edict_t	*obstacle;

/*
============
SV_Push

Objects need to be moved back on a failed push,
otherwise riders would continue to slide.
============
*/
bool SV_Push (edict_t *pusher, vec3_t move, vec3_t amove)
{
	int			i, e;
	edict_t		*check, *block;
	vec3_t		mins, maxs;
	pushed_t	*p;
	vec3_t		org, org2, move2, forward, right, up;

	// clamp the move to 1/8 units, so the position will
	// be accurate for client side prediction
	for (i=0 ; i<3 ; i++)
	{
		float	temp;
		temp = move[i]*8.0;
		if (temp > 0.0)
			temp += 0.5;
		else
			temp -= 0.5;
		move[i] = 0.125 * (int)temp;
	}

	// find the bounding box
	for (i=0 ; i<3 ; i++)
	{
		mins[i] = pusher->absMin[i] + move[i];
		maxs[i] = pusher->absMax[i] + move[i];
	}

// we need this for pushing things later
	Vec3Subtract (vec3Origin, amove, org);
	Angles_Vectors (org, forward, right, up);

// save the pusher's original position
	pushed_p->ent = pusher;
	Vec3Copy (pusher->state.origin, pushed_p->origin);
	Vec3Copy (pusher->state.angles, pushed_p->angles);
	if (pusher->client)
		pushed_p->deltayaw = pusher->client->playerState.pMove.deltaAngles[YAW];
	pushed_p++;

// move the pusher to it's final position
	Vec3Add (pusher->state.origin, move, pusher->state.origin);
	Vec3Add (pusher->state.angles, amove, pusher->state.angles);
	gi.linkentity (pusher);

// see if any solid entities are inside the final position
	check = g_edicts+1;
	for (e = 1; e < globals.numEdicts; e++, check++)
	{
		if (!check->inUse)
			continue;
		if (check->movetype == MOVETYPE_PUSH
		|| check->movetype == MOVETYPE_STOP
		|| check->movetype == MOVETYPE_NONE
		|| check->movetype == MOVETYPE_NOCLIP)
			continue;

		if (!check->area.prev)
			continue;		// not linked in anywhere

	// if the entity is standing on the pusher, it will definitely be moved
		if (check->groundentity != pusher)
		{
			// see if the ent needs to be tested
			if (check->absMin[0] >= maxs[0]
			|| check->absMin[1] >= maxs[1]
			|| check->absMin[2] >= maxs[2]
			|| check->absMax[0] <= mins[0]
			|| check->absMax[1] <= mins[1]
			|| check->absMax[2] <= mins[2])
				continue;

			// see if the ent's bbox is inside the pusher's final position
			if (!SV_TestEntityPosition (check))
				continue;
		}

		if ((pusher->movetype == MOVETYPE_PUSH) || (check->groundentity == pusher))
		{
			// move this entity
			pushed_p->ent = check;
			Vec3Copy (check->state.origin, pushed_p->origin);
			Vec3Copy (check->state.angles, pushed_p->angles);

			// try moving the contacted entity 
			Vec3Add (check->state.origin, move, check->state.origin);
			if (check->client)
			{
				check->client->playerState.pMove.deltaAngles[YAW] += amove[YAW];

				//r1: dead-body-on-lift / other random view distortion fix
				pushed_p->deltayaw = pushed_p->ent->client->playerState.pMove.deltaAngles[YAW];
			}

			else
				check->state.angles[YAW] += amove[YAW];

			pushed_p++;

			// figure movement due to the pusher's amove
			Vec3Subtract (check->state.origin, pusher->state.origin, org);
			org2[0] = Dot3Product (org, forward);
			org2[1] = -Dot3Product (org, right);
			org2[2] = Dot3Product (org, up);
			Vec3Subtract (org2, org, move2);
			Vec3Add (check->state.origin, move2, check->state.origin);

			// may have pushed them off an edge
			if (check->groundentity != pusher)
				check->groundentity = NULL;

			block = SV_TestEntityPosition (check);
			if (!block)
			{	// pushed ok
				gi.linkentity (check);
				// impact?
				continue;
			}

			// if it is ok to leave in the old position, do it
			// this is only relevent for riding entities, not pushed
			// FIXME: this doesn't acount for rotation
			Vec3Subtract (check->state.origin, move, check->state.origin);
			block = SV_TestEntityPosition (check);
			if (!block)
			{
				pushed_p--;
				continue;
			}
		}
		
		// save off the obstacle so we can call the block function
		obstacle = check;

		// move back any entities we already moved
		// go backwards, so if the same entity was pushed
		// twice, it goes back to the original position
		for (p=pushed_p-1 ; p>=pushed ; p--)
		{
			Vec3Copy (p->origin, p->ent->state.origin);
			Vec3Copy (p->angles, p->ent->state.angles);
			if (p->ent->client)
			{
				p->ent->client->playerState.pMove.deltaAngles[YAW] = p->deltayaw;
			}
			gi.linkentity (p->ent);
		}
		return false;
	}

//FIXME: is there a better way to handle this?
	// see if anything we moved has touched a trigger
	for (p=pushed_p-1 ; p>=pushed ; p--)
		G_TouchTriggers (p->ent);

	return true;
}

/*
================
SV_Physics_Pusher

Bmodel objects don't interact with each other, but
push all box objects
================
*/
void SV_Physics_Pusher (edict_t *ent)
{
	vec3_t		move, amove;
	edict_t		*part, *mv;

	// if not a team captain, so movement will be handled elsewhere
	if ( ent->flags & FL_TEAMSLAVE)
		return;

	// make sure all team slaves can move before commiting
	// any moves or calling any think functions
	// if the move is blocked, all moved objects will be backed out
//retry:
	pushed_p = pushed;
	for (part = ent ; part ; part=part->teamchain)
	{
		if (part->velocity[0] || part->velocity[1] || part->velocity[2] ||
			part->avelocity[0] || part->avelocity[1] || part->avelocity[2]
			)
		{	// object is moving
			Vec3Scale (part->velocity, 0.1f, move);
			Vec3Scale (part->avelocity, 0.1f, amove);

			if (!SV_Push (part, move, amove))
				break;	// move was blocked
		}
	}
	if (pushed_p > &pushed[MAX_CS_EDICTS])
		gi.error ("pushed_p > &pushed[MAX_CS_EDICTS], memory corrupted");

	if (part)
	{
		// the move failed, bump all nextthink times and back out moves
		for (mv = ent ; mv ; mv=mv->teamchain)
		{
			if (mv->nextthink > 0)
				mv->nextthink += FRAMETIME;
		}

		// if the pusher has a "blocked" function, call it
		// otherwise, just stay in place until the obstacle is gone
		if (part->blocked)
			part->blocked (part, obstacle);
#if 0
		// if the pushed entity went away and the pusher is still there
		if (!obstacle->inUse && part->inUse)
			goto retry;
#endif
	}
	else
	{
		// the move succeeded, so call all think functions
		for (part = ent ; part ; part=part->teamchain)
		{
			SV_RunThink (part);
		}
	}
}

//==================================================================

/*
=============
SV_Physics_None

Non moving objects can only think
=============
*/
void SV_Physics_None (edict_t *ent)
{
// regular thinking
	SV_RunThink (ent);
}

/*
=============
SV_Physics_Noclip

A moving object that doesn't obey physics
=============
*/
void SV_Physics_Noclip (edict_t *ent)
{
// regular thinking
	if (!SV_RunThink (ent))
		return;
	
	Vec3MA (ent->state.angles, 0.1f, ent->avelocity, ent->state.angles);
	Vec3MA (ent->state.origin, 0.1f, ent->velocity, ent->state.origin);

	gi.linkentity (ent);
}

/*
==============================================================================

TOSS / BOUNCE

==============================================================================
*/

/*
=============
SV_Physics_Toss

Toss, bounce, and fly movement.  When onground, do nothing.
=============
*/
void SV_Physics_Toss (edict_t *ent)
{
	CTrace	trace;
	vec3_t		move;
	float		backoff;
	edict_t		*slave;
	bool		wasinwater;
	bool		isinwater;
	vec3_t		old_origin;

// regular thinking
	SV_RunThink (ent);

	// if not a team captain, so movement will be handled elsewhere
	if ( ent->flags & FL_TEAMSLAVE)
		return;

	if (ent->velocity[2] > 0)
		ent->groundentity = NULL;

// check for the groundentity going away
	if (ent->groundentity)
		if (!ent->groundentity->inUse)
			ent->groundentity = NULL;

// if onground, return without moving
	if ( ent->groundentity )
		return;

	Vec3Copy (ent->state.origin, old_origin);

// add gravity
	if (ent->movetype != MOVETYPE_FLY
	&& ent->movetype != MOVETYPE_FLYMISSILE)
		SV_AddGravity (ent);

// move angles
	Vec3MA (ent->state.angles, 0.1f, ent->avelocity, ent->state.angles);

// move origin
	Vec3Scale (ent->velocity, 0.1f, move);
	trace = SV_PushEntity (ent, move);
	if (!ent->inUse)
		return;

	if (trace.fraction < 1)
	{
		if (ent->movetype == MOVETYPE_BOUNCE)
			backoff = 1.5;
		else
			backoff = 1;

		ClipVelocity (ent->velocity, trace.plane.normal, ent->velocity, backoff);

		// stop if on ground
		if (trace.plane.normal[2] > 0.9)
		{		
			if (ent->velocity[2] < 60 || ent->movetype != MOVETYPE_BOUNCE )
			{
				ent->groundentity = trace.ent;
				ent->groundentity_linkcount = trace.ent->linkCount;
				Vec3Copy (vec3Origin, ent->velocity);
				Vec3Copy (vec3Origin, ent->avelocity);
			}
		}

//		if (ent->touch)
//			ent->touch (ent, trace.ent, &trace.plane, trace.surface);
	}
	
// check for water transition
	wasinwater = (ent->watertype & CONTENTS_MASK_WATER) ? true : false;
	ent->watertype = gi.pointcontents (ent->state.origin);
	isinwater = (ent->watertype & CONTENTS_MASK_WATER) ? true : false;

	if (isinwater)
		ent->waterlevel = 1;
	else
		ent->waterlevel = 0;

	if (!wasinwater && isinwater)
		PlaySoundAt (old_origin, g_edicts, CHAN_AUTO, SoundIndex("misc/h2ohit1.wav"));
	else if (wasinwater && !isinwater)
		PlaySoundAt (ent->state.origin, g_edicts, CHAN_AUTO, SoundIndex("misc/h2ohit1.wav"));

// move teamslaves
	for (slave = ent->teamchain; slave; slave = slave->teamchain)
	{
		Vec3Copy (ent->state.origin, slave->state.origin);
		gi.linkentity (slave);
	}
}

/*
===============================================================================

STEPPING MOVEMENT

===============================================================================
*/

/*
=============
SV_Physics_Step

Monsters freefall when they don't have a ground entity, otherwise
all movement is done with discrete steps.

This is also used for objects that have become still on the ground, but
will fall if the floor is pulled out from under them.
FIXME: is this true?
=============
*/

//FIXME: hacked in for E3 demo
#define sv_stopspeed		100
#define sv_friction			6
#define sv_waterfriction	1

void SV_AddRotationalFriction (edict_t *ent)
{
	int		n;
	float	adjustment;

	Vec3MA (ent->state.angles, 0.1f, ent->avelocity, ent->state.angles);
	adjustment = 0.1f * sv_stopspeed * sv_friction;
	for (n = 0; n < 3; n++)
	{
		if (ent->avelocity[n] > 0)
		{
			ent->avelocity[n] -= adjustment;
			if (ent->avelocity[n] < 0)
				ent->avelocity[n] = 0;
		}
		else
		{
			ent->avelocity[n] += adjustment;
			if (ent->avelocity[n] > 0)
				ent->avelocity[n] = 0;
		}
	}
}

void SV_Physics_Step (edict_t *ent)
{
	bool	wasonground;
	bool	hitsound = false;
	float		*vel;
	float		speed, newspeed, control;
	float		friction;
	edict_t		*groundentity;
	int			mask;

	// airborn monsters should always check for ground
	if (!ent->groundentity && ent->Monster)
		ent->Monster->CheckGround ();

	groundentity = ent->groundentity;

	if (groundentity)
		wasonground = true;
	else
		wasonground = false;
		
	if (ent->avelocity[0] || ent->avelocity[1] || ent->avelocity[2])
		SV_AddRotationalFriction (ent);

	// add gravity except:
	//   flying monsters
	//   swimming monsters who are in the water
	if (! wasonground)
		if (!(ent->flags & FL_FLY))
			if (!((ent->flags & FL_SWIM) && (ent->waterlevel > 2)))
			{
				if (ent->velocity[2] < sv_gravity->Float()*-0.1)
					hitsound = true;
				if (ent->waterlevel == 0)
					SV_AddGravity (ent);
			}

	// friction for flying monsters that have been given vertical velocity
	if ((ent->flags & FL_FLY) && (ent->velocity[2] != 0))
	{
		speed = Q_fabs(ent->velocity[2]);
		control = speed < sv_stopspeed ? sv_stopspeed : speed;
		friction = sv_friction/3;
		newspeed = speed - (0.1f * control * friction);
		if (newspeed < 0)
			newspeed = 0;
		newspeed /= speed;
		ent->velocity[2] *= newspeed;
	}

	// friction for flying monsters that have been given vertical velocity
	if ((ent->flags & FL_SWIM) && (ent->velocity[2] != 0))
	{
		speed = Q_fabs(ent->velocity[2]);
		control = speed < sv_stopspeed ? sv_stopspeed : speed;
		newspeed = speed - (0.1f * control * sv_waterfriction * ent->waterlevel);
		if (newspeed < 0)
			newspeed = 0;
		newspeed /= speed;
		ent->velocity[2] *= newspeed;
	}

	if (ent->velocity[2] || ent->velocity[1] || ent->velocity[0])
	{
		// apply friction
		// let dead monsters who aren't completely onground slide
		if ((wasonground) || (ent->flags & (FL_SWIM|FL_FLY)) && !(ent->health <= 0.0 && (ent->Monster && !ent->Monster->CheckBottom())))
		{
			vel = ent->velocity;
			speed = sqrtf(vel[0]*vel[0] +vel[1]*vel[1]);
			if (speed)
			{
				friction = sv_friction;

				control = speed < sv_stopspeed ? sv_stopspeed : speed;
				newspeed = speed - 0.1f*control*friction;

				if (newspeed < 0)
					newspeed = 0;
				newspeed /= speed;

				vel[0] *= newspeed;
				vel[1] *= newspeed;
			}
		}

		if (ent->svFlags & SVF_MONSTER)
			mask = CONTENTS_MASK_MONSTERSOLID;
		else
			mask = CONTENTS_MASK_SOLID;
		SV_FlyMove (ent, 0.1f, mask);

		gi.linkentity (ent);
		G_TouchTriggers (ent);
		if (!ent->inUse)
			return;

		if (ent->groundentity && !wasonground && hitsound)
			PlaySoundFrom (ent, CHAN_AUTO, SoundIndex("world/land.wav"));
	}

// regular thinking
	SV_RunThink (ent);
}

//============================================================================
/*
================
G_RunEntity

================
*/
void G_RunEntity (edict_t *ent)
{
	if (ent->prethink)
		ent->prethink (ent);

	switch ( (int)ent->movetype)
	{
	case MOVETYPE_PUSH:
	case MOVETYPE_STOP:
		SV_Physics_Pusher (ent);
		break;
	case MOVETYPE_NONE:
		SV_Physics_None (ent);
		break;
	case MOVETYPE_NOCLIP:
		SV_Physics_Noclip (ent);
		break;
	case MOVETYPE_STEP:
		SV_Physics_Step (ent);
		break;
	case MOVETYPE_TOSS:
	case MOVETYPE_BOUNCE:
	case MOVETYPE_FLY:
	case MOVETYPE_FLYMISSILE:
		SV_Physics_Toss (ent);
		break;
	default:
		gi.error ("SV_Physics: bad movetype %i", (int)ent->movetype);			
	}
}
