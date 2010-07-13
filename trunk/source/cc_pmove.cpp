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
// cc_pmove.cpp
// Player move
//

// Paril
// Crouch-jumping!
#define ALLOW_CROUCH_JUMPING

#include "cc_local.h"

// all of the locals will be zeroed before each pmove, just to make damn sure
// we don't have any differences when running on client or server

struct pMoveLocal_t
{
	vec3f			origin;			// full float precision
	vec3f			velocity;		// full float precision

	vec3f			forward, right, up;
	float			frameTime;

	SBSPSurface	*groundSurface;
	sint32			groundContents;

	vec3Base<sint16>previousOrigin;
	bool			ladder;

	CPlayerEntity	*Player;
};

static SPMove	*pm;
static pMoveLocal_t	pml;

static float	pmAirAcceleration = 0;

const float MIN_STEP_NORMAL		= 0.7f;		// can't step up onto very steep slopes
const int MAX_CLIP_PLANES		= 5;

// movement parameters
const float SV_PM_STOPSPEED			= 100.0f;
const float SV_PM_MAXSPEED			= 300.0f;
const float SV_PM_DUCKSPEED			= 100.0f;
const float SV_PM_ACCELERATE		= 10.0f;
const float SV_PM_WATERACCELERATE	= 10.0f;
const float SV_PM_FRICTION			= 6.0f;
const float SV_PM_WATERFRICTION		= 1.0f;
const float SV_PM_WATERSPEED		= 400.0f;

/*
==================
SV_PM_ClipVelocity

Slide off of the impacting object
returns the blocked flags (1 = floor, 2 = step / wall)
==================
*/
static void SV_PM_ClipVelocity (vec3f &in, vec3f &Normal, vec3f &out, float overbounce)
{	
	float backoff = (in | Normal) * overbounce;

	for (sint32 i = 0; i < 3; i++)
	{
		out[i] = in[i] - Normal[i]*backoff;
		if ((out[i] > -LARGE_EPSILON) && (out[i] < LARGE_EPSILON))
			out[i] = 0;
	}
}
 

/*
==================
SV_PM_StepSlideMove

Each intersection will try to step over the obstruction instead of
sliding along it.

Returns a new origin, velocity, and contact entity
Does not modify any world state?
==================
*/
static void SV_PM_StepSlideMove_ ()
{
	sint32			numbumps = 4;
	vec3f		dir;
	float		d;
	sint32			numPlanes = 0;
	vec3f		planes[MAX_CLIP_PLANES];
	vec3f		primal_velocity = pml.velocity;
	sint32			i, j;
	CTrace		trace;
	vec3f		end;
	float		time_left = pml.frameTime;

	for (sint32 bumpcount = 0; bumpcount < numbumps; bumpcount++)
	{
		end = pml.origin + time_left * pml.velocity;
		trace (pml.origin, pm->Mins, pm->Maxs, end, pml.Player, (pml.Player->Health > 0) ? CONTENTS_MASK_PLAYERSOLID : CONTENTS_MASK_DEADSOLID);

		if (trace.AllSolid)
		{
			// entity is trapped in another solid
			pml.velocity.Z = 0;	// don't build up falling damage
			return;
		}

		if (trace.Fraction > 0)
		{
			// actually covered some distance
			pml.origin = trace.EndPosition;
			numPlanes = 0;
		}

		if (trace.Fraction == 1)
			 break;		// moved the entire distance

		// save entity for contact
		if ((pm->NumTouch < MAXTOUCH) && trace.Entity)
		{
			bool dup = false;
			for (sint32 z = 0; z < pm->NumTouch; z++)
			{
				if (pm->TouchEnts[z]->Entity == trace.Entity)
				{
					dup = true;
					break;
				}
			}
			if (!dup)
			{
				pm->TouchEnts[pm->NumTouch] = trace.Entity->GetGameEntity();
				pm->NumTouch++;
			}
		}
		
		time_left -= time_left * trace.Fraction;

		// slide along this plane
		if (numPlanes >= MAX_CLIP_PLANES)
		{
			// this shouldn't really happen
			pml.velocity.Clear ();
			break;
		}

		planes[numPlanes] = trace.Plane.Normal;
		numPlanes++;

		// modify original_velocity so it parallels all of the clip planes
		for (i = 0; i < numPlanes; i++)
		{
			SV_PM_ClipVelocity (pml.velocity, planes[i], pml.velocity, 1.01f);
			for (j = 0; j < numPlanes; j++)
			{
				if (j != i)
				{
					if ((pml.velocity | planes[j]) < 0)
						break;	// not ok
				}
			}

			if (j == numPlanes)
				break;
		}

		if (i == numPlanes)
		{
			// go along the crease
			if (numPlanes != 2)
			{
				pml.velocity.Clear ();
				break;
			}
			dir = planes[0] ^ planes[1];
			d = dir | pml.velocity;
			pml.velocity = dir * d;
		}

		/*
		** if velocity is against the original velocity, stop dead
		** to avoid tiny occilations in sloping corners
		*/
		if ((pml.velocity | primal_velocity) <= 0)
		{
			pml.velocity.Clear ();
			break;
		}
	}

	if (pm->State.PMoveTime)
		pml.velocity = primal_velocity;
}

static void SV_PM_StepSlideMove ()
{
	vec3f start_o = pml.origin;
	vec3f start_v = pml.velocity;

	SV_PM_StepSlideMove_ ();

	vec3f down_o = pml.origin;
	vec3f down_v = pml.velocity;

	vec3f up = start_o + vec3f(0, 0, STEPSIZE);
	CTrace trace (up, pm->Mins, pm->Maxs, up, pml.Player, (pml.Player->Health > 0) ? CONTENTS_MASK_PLAYERSOLID : CONTENTS_MASK_DEADSOLID);
	
	if (trace.AllSolid)
		return;		// can't step up

	// try sliding above
	pml.origin = up;
	pml.velocity = start_v;

	SV_PM_StepSlideMove_ ();

	// push down the final amount
	vec3f down = pml.origin - vec3f(0, 0, STEPSIZE);
	trace (pml.origin, pm->Mins, pm->Maxs, down, pml.Player, (pml.Player->Health > 0) ? CONTENTS_MASK_PLAYERSOLID : CONTENTS_MASK_DEADSOLID);
	
	if (!trace.AllSolid)
		pml.origin = trace.EndPosition;

	up = pml.origin;

	// decide which one went farther
	float down_dist = (down_o.X - start_o.X)*(down_o.X - start_o.X)
		+ (down_o.Y - start_o.Y)*(down_o.Y - start_o.Y);
	float up_dist = (up.X - start_o.X)*(up.X - start_o.X)
		+ (up.Y - start_o.Y)*(up.Y - start_o.Y);

	if (down_dist > up_dist || trace.Plane.Normal.Z < MIN_STEP_NORMAL)
	{
		pml.origin = down_o;
		pml.velocity = down_v;
		return;
	}

	// !! Special case !!
	// if we were walking along a plane, then we need to copy the Z over
	pml.velocity.Z = down_v.Z;
}

/*
==================
SV_PM_Friction

Handles both ground friction and water friction
==================
*/
static void SV_PM_Friction ()
{	
	vec3f &vel = pml.velocity;
	float speed = vel.Length();

	if (speed < 1)
	{
		vel.X = vel.Y = 0;
		return;
	}

	float drop = 0;

	// apply ground friction
	if ((pm->GroundEntity && pml.groundSurface && !(pml.groundSurface->Flags & SURF_TEXINFO_SLICK)) || pml.ladder)
		drop += ((speed < SV_PM_STOPSPEED) ? SV_PM_STOPSPEED : speed) * SV_PM_FRICTION * pml.frameTime;

	// apply water friction
	if (pm->WaterLevel && !pml.ladder)
		drop += speed*SV_PM_WATERFRICTION*pm->WaterLevel*pml.frameTime;

	// scale the velocity
	float newspeed = speed - drop;
	if (newspeed < 0)
		newspeed = 0;

	vel *= (newspeed / speed);
}

/*
==============
SV_PM_Accelerate

Handles user intended acceleration
==============
*/
static void SV_PM_Accelerate (vec3f &wishdir, float wishspeed, float accel)
{
	float addspeed = wishspeed - (pml.velocity | wishdir);
	if (addspeed <= 0)
		return;

	float accelspeed = accel*pml.frameTime*wishspeed;
	if (accelspeed > addspeed)
		accelspeed = addspeed;

	pml.velocity += accelspeed*wishdir;
}

/*
==============
SV_PM_AirAccelerate

Handles user intended air acceleration
==============
*/
static void SV_PM_AirAccelerate (vec3f &wishdir, float wishspeed, float accel)
{
	float wishspd = wishspeed;

	if (wishspd > 30)
		wishspd = 30;

	float addspeed = wishspd - (pml.velocity | wishdir);
	if (addspeed <= 0)
		return;

	float accelspeed = accel * wishspeed * pml.frameTime;
	if (accelspeed > addspeed)
		accelspeed = addspeed;

	pml.velocity += accelspeed*wishdir;
}

/*
=============
SV_PM_AddCurrents
=============
*/
static void SV_PM_AddCurrents (vec3f &wishvel)
{
	// account for ladders
	if (pml.ladder && Q_fabs (pml.velocity.Z) <= 200)
	{
		if ((pml.Player->Client.Respawn.AimDegrees < 0) && (((pm->ViewAngles.X <= -15) && (pm->Command.ForwardMove > 0)) || (pm->Command.UpMove > 0)))
			wishvel.Z = -200;
		else if (((pm->ViewAngles.X <= -15) && (pm->Command.ForwardMove > 0)) || (pm->Command.UpMove > 0))
			wishvel.Z = 200;
		else if (((pm->ViewAngles.X >= 15) && (pm->Command.ForwardMove > 0)) || (pm->Command.UpMove < 0))
			wishvel.Z = -200;
		else
			wishvel.Z = 0;

		// limit horizontal speed when on a ladder
		if (wishvel.X < -25)		wishvel.X = -25;
		else if (wishvel.X > 25)	wishvel.X = 25;

		if (wishvel[1] < -25)		wishvel.Y = -25;
		else if (wishvel[1] > 25)	wishvel.Y = 25;
	}

	// add water currents
	if (pm->WaterType & CONTENTS_MASK_CURRENT)
	{
		vec3f v = vec3fOrigin;

		if (pm->WaterType & CONTENTS_CURRENT_0)		v.X += 1;
		if (pm->WaterType & CONTENTS_CURRENT_90)	v.Y += 1;
		if (pm->WaterType & CONTENTS_CURRENT_180)	v.X -= 1;
		if (pm->WaterType & CONTENTS_CURRENT_270)	v.Y -= 1;
		if (pm->WaterType & CONTENTS_CURRENT_UP)	v.Z += 1;
		if (pm->WaterType & CONTENTS_CURRENT_DOWN)	v.Z -= 1;

		wishvel = wishvel.MultiplyAngles ((((pm->WaterLevel == WATER_FEET) && (pm->GroundEntity)) ? (SV_PM_WATERSPEED / 2) : SV_PM_WATERSPEED), v);
	}

	// add conveyor belt velocities
	if (pm->GroundEntity)
	{
		vec3f v = vec3fOrigin;

		if (pml.groundContents & CONTENTS_CURRENT_0)	v.X += 1;
		if (pml.groundContents & CONTENTS_CURRENT_90)	v.Y += 1;
		if (pml.groundContents & CONTENTS_CURRENT_180)	v.X -= 1;
		if (pml.groundContents & CONTENTS_CURRENT_270)	v.Y -= 1;
		if (pml.groundContents & CONTENTS_CURRENT_UP)	v.Z += 1;
		if (pml.groundContents & CONTENTS_CURRENT_DOWN)	v.Z -= 1;

		wishvel = wishvel.MultiplyAngles (100 /* pm->GroundEntity->speed */, v);
	}
}

/*
===================
SV_PM_WaterMove
===================
*/
static void SV_PM_WaterMove ()
{
	short		ForwardMove, SideMove, UpMove;

	//r1: keep a local version of these values, this prevents a physics exploit where a ridiculously high
	//ForwardMove/SideMove can allow player to negate effects of water currents. value of 400 is still higher
	//than maxvel (300), but 400 is the default q2 run speed, so we keep it for compatibility.
	if (pm->Command.ForwardMove > 400)
		ForwardMove = 400;
	else if (pm->Command.ForwardMove < -400)
		ForwardMove = -400;
	else
		ForwardMove = pm->Command.ForwardMove;

	if (pm->Command.SideMove > 400)
		SideMove = 400;
	else if (pm->Command.SideMove < -400)
		SideMove = -400;
	else
		SideMove = pm->Command.SideMove;

	if (pm->Command.UpMove > 400)
		UpMove = 400;
	else if (pm->Command.UpMove < -400)
		UpMove = -400;
	else
		UpMove = pm->Command.UpMove;

	// user intentions
	vec3f wishvel ( pml.forward.X*ForwardMove + pml.right.X*SideMove,
					pml.forward.Y*ForwardMove + pml.right.Y*SideMove,
					pml.forward.Z*ForwardMove + pml.right.Z*SideMove);

	// drift towards bottom
	wishvel.Z += ((!ForwardMove && !SideMove && !UpMove) ? -60 : UpMove);

	SV_PM_AddCurrents (wishvel);

	float wishspeed = wishvel.Normalize ();
	if (wishspeed > SV_PM_MAXSPEED)
		wishspeed = SV_PM_MAXSPEED;

	wishspeed *= 0.5;
	SV_PM_Accelerate (wishvel, wishspeed, SV_PM_WATERACCELERATE);

	SV_PM_StepSlideMove ();
}

static void SV_PM_CatagorizePosition ();

/*
===================
SV_PM_AirMove
===================
*/
static void SV_PM_AirMove ()
{
	float fmove = pm->Command.ForwardMove;
	float smove = pm->Command.SideMove;

	vec3f wishvel(	pml.forward.X*fmove + pml.right.X*smove,
					pml.forward.Y*fmove + pml.right.Y*smove,
					0);

	SV_PM_AddCurrents (wishvel);

	vec3f wishdir = wishvel;
	float wishspeed = wishdir.Normalize ();

	// clamp to server defined max speed
	const float maxspeed = (pm->State.PMoveFlags & PMF_DUCKED) ? SV_PM_DUCKSPEED : SV_PM_MAXSPEED;

	if (wishspeed > maxspeed)
	{
		wishvel *= maxspeed/wishspeed;
		wishspeed = maxspeed;
	}
	
	if (pml.ladder)
	{
		SV_PM_Accelerate (wishdir, wishspeed, SV_PM_ACCELERATE);

		if (!wishvel.Z)
		{
			if (pml.velocity.Z > 0)
			{
				pml.velocity.Z -= pm->State.Gravity * pml.frameTime;
				if (pml.velocity.Z < 0)
					pml.velocity.Z  = 0;
			}
			else
			{
				pml.velocity.Z += pm->State.Gravity * pml.frameTime;
				if (pml.velocity.Z > 0)
					pml.velocity.Z  = 0;
			}
		}

		SV_PM_StepSlideMove ();
	}
	else if (pm->GroundEntity)
	{
		// walking on ground
		pml.velocity.Z = 0; //!!! this is before the accel
		SV_PM_Accelerate (wishdir, wishspeed, SV_PM_ACCELERATE);

		// PGM	-- fix for negative trigger_Gravity fields
		//		pml.velocity[2] = 0;
		pml.velocity.Z -= (pm->State.Gravity > 0) ? 0 : (pm->State.Gravity * pml.frameTime);
		// PGM

		if (!pml.velocity.X && !pml.velocity.Y)
			return;
		SV_PM_StepSlideMove ();

		// Arcade Quake II
		// Paril, step down!
		// Pre-categorize for check of down-step
		SV_PM_CatagorizePosition ();
		if (!pm->GroundEntity)
		{
			static const vec3f down (0, 0, -1);
			
			vec3f start = pml.origin;
			vec3f end = start.MultiplyAngles (STEPSIZE, down);

			CTrace tr (start, pm->Mins, pm->Maxs, end, pml.Player, -1);

			if (tr.Fraction < 1.0)
			{
				// We can drop!
				//Com_Printf (0, "GROUND IS GONE!!!\n");
				end = start - tr.EndPosition;

				pml.origin[2] -= end.Length();
			}
		}
		// Paril
		return;
		// Arcade Quake II
	}
	else
	{
		// Arcade Quake II
		if (pml.Player->Client.WasOnLadder)
		{
			pml.Player->Client.WasOnLadder = false;
			if ((pm->AimChanged && (pm->Command.SideMove < -50 || pm->Command.SideMove > 50)) && pml.velocity.Z >= 100)
			{
				// We want to double jump, so force us to jump.
				pm->DoubleJumpDone = true;
				pml.ladder = false;

				pm->State.PMoveFlags |= PMF_JUMP_HELD;

				pm->GroundEntity = NULL;

				pml.velocity.Z += 135;
				if (pml.velocity.Z < 135)
					pml.velocity.Z = 135;

				if (pml.Player->Client.Respawn.AimingLeft)
					pml.velocity.X += 200;
				else
					pml.velocity.X -= 200;
			}
		}
		// Arcade Quake II

		// not on ground, so little effect on velocity
		if (pmAirAcceleration)
			SV_PM_AirAccelerate (wishdir, wishspeed, SV_PM_ACCELERATE);
		else
			SV_PM_Accelerate (wishdir, wishspeed, 1);

		// add Gravity
		pml.velocity.Z -= pm->State.Gravity * pml.frameTime;
		SV_PM_StepSlideMove ();
	}
}

/*
=============
SV_PM_CatagorizePosition
=============
*/
static void SV_PM_CatagorizePosition ()
{
	// if the player hull point one unit down is solid, the player is on ground
	// see if standing on something solid
	vec3f point = pml.origin;
	point.Z -= 0.25;

	if (pml.velocity.Z > 180)
	{
		pm->State.PMoveFlags &= ~PMF_ON_GROUND;
		pm->GroundEntity = NULL;
	}
	else
	{
		CTrace trace (pml.origin, pm->Mins, pm->Maxs, point, pml.Player, (pml.Player->Health > 0) ? CONTENTS_MASK_PLAYERSOLID : CONTENTS_MASK_DEADSOLID);
		pml.groundSurface = trace.Surface;
		pml.groundContents = trace.Contents;

		if (!trace.Entity || ((trace.Plane.Normal.Z < 0.7) && !trace.StartSolid))
		{
			pm->GroundEntity = NULL;
			pm->State.PMoveFlags &= ~PMF_ON_GROUND;
		}
		else
		{
			pm->GroundEntity = trace.Entity->GetGameEntity();

			// hitting solid ground will end a waterjump
			if (pm->State.PMoveFlags & PMF_TIME_WATERJUMP)
			{
				pm->State.PMoveFlags &= ~(PMF_TIME_WATERJUMP | PMF_TIME_LAND | PMF_TIME_TELEPORT);
				pm->State.PMoveTime = 0;
			}

			if (!(pm->State.PMoveFlags & PMF_ON_GROUND))
			{
				// just hit the ground
				pm->State.PMoveFlags |= PMF_ON_GROUND;
				// don't do landing time if we were just going down a slope
				if (pml.velocity.Z < -200)
				{
					pm->State.PMoveFlags |= PMF_TIME_LAND;
					// don't allow another jump for a little while
					pm->State.PMoveTime = (pml.velocity.Z < -400) ? 25 : 18;	
				}
			}
		}

		if ((pm->NumTouch < MAXTOUCH) && trace.Entity)
		{
			pm->TouchEnts[pm->NumTouch] = trace.Entity->GetGameEntity();
			pm->NumTouch++;
		}
	}

	// get WaterLevel, accounting for ducking
	pm->WaterLevel = WATER_NONE;
	pm->WaterType = 0;

	const float sample2 = pm->ViewHeight - pm->Mins.Z;
	const float sample1 = sample2 / 2;

	point.Z = pml.origin.Z + pm->Mins.Z + 1;	

	EBrushContents cont = PointContents (point);
	if (cont & CONTENTS_MASK_WATER)
	{
		pm->WaterType = cont;
		pm->WaterLevel = WATER_FEET;
		point.Z = pml.origin.Z + pm->Mins.Z + sample1;

		cont = PointContents (point);
		if (cont & CONTENTS_MASK_WATER)
		{
			pm->WaterLevel = WATER_WAIST;
			point.Z = pml.origin.Z + pm->Mins.Z + sample2;

			cont = PointContents (point);
			if (cont & CONTENTS_MASK_WATER)
				pm->WaterLevel = WATER_UNDER;
		}
	}
}

/*
=============
SV_PM_CheckJump
=============
*/
static void SV_PM_CheckJump ()
{
	if (pm->State.PMoveFlags & PMF_TIME_LAND)
		// hasn't been long enough since landing to jump again
		return;

#ifdef ALLOW_CROUCH_JUMPING
	if (pm->Command.UpMove < 10 || pm->State.PMoveFlags & PMF_DUCKED)
	{
#else
	if (pm->Command.UpMove < 10)
	{
#endif
		// not holding jump
		pm->State.PMoveFlags &= ~PMF_JUMP_HELD;
		return;
	}

	// must wait for jump to be released
	if (pm->State.PMoveFlags & PMF_JUMP_HELD)
		return;

	if (pm->State.PMoveType == PMT_DEAD)
		return;

	if (pm->WaterLevel >= WATER_WAIST)
	{
		// swimming, not jumping
		pm->GroundEntity = NULL;

		if (pml.velocity.Z <= -300)
			return;

		switch (pm->WaterType)
		{
		case CONTENTS_WATER:
			pml.velocity.Z = 100;
			break;
		case CONTENTS_SLIME:
			pml.velocity.Z = 80;
			break;
		default:
			pml.velocity.Z = 50;
			break;
		};
		return;
	}

	if (pm->GroundEntity == NULL)
		return;		// in air, so no effect

	pm->State.PMoveFlags |= PMF_JUMP_HELD;

	pm->GroundEntity = NULL;

	pml.velocity.Z += 270;
	if (pml.velocity.Z < 270)
		pml.velocity.Z = 270;
}

/*
=============
SV_PM_CheckSpecialMovement
=============
*/
static void SV_PM_CheckSpecialMovement ()
{
	if (pm->State.PMoveTime)
		return;

	pml.ladder = false;

	// check for ladder
	// Arcade Quake II
	vec3f forw = pm->ViewAngles;
	if (pml.Player->Client.Respawn.AimingLeft)
		forw.Y -= 90;
	else
		forw.Y += 90;
	forw.X = 0;
	vec3f flatforward;
	forw.ToVectors (&flatforward, NULL, NULL);
	// Arcade Quake II


	vec3f spot = pml.origin.MultiplyAngles (1, flatforward);
	CTrace trace (pml.origin, pm->Mins, pm->Maxs, spot, pml.Player, (pml.Player->Health > 0) ? CONTENTS_MASK_PLAYERSOLID : CONTENTS_MASK_DEADSOLID);
	
	if ((trace.Fraction < 1) && (trace.Contents & CONTENTS_LADDER))
	// Arcade Quake II
	{
		pml.ladder = true;
		pml.Player->Client.WasOnLadder = true;
	}
	// Arcade Quake II

	// check for water jump
	if (pm->WaterLevel != WATER_WAIST)
		return;

	spot = pml.origin.MultiplyAngles (30, flatforward) + vec3f(0, 0, 4);
	EBrushContents cont = PointContents (spot);

	if (!(cont & CONTENTS_SOLID))
		return;

	spot.Z += 16;
	cont = PointContents (spot);

	if (cont)
		return;

	// jump out of water
	pml.velocity = flatforward * 50;
	pml.velocity.Z = 350;

	pm->State.PMoveFlags |= PMF_TIME_WATERJUMP;
	pm->State.PMoveTime = 255;
}

/*
===============
SV_PM_FlyMove
===============
*/
static void SV_PM_FlyMove (bool doClip)
{
	pm->ViewHeight = 22;

	// friction
	float speed = pml.velocity.Length();
	if (speed < 1)
		pml.velocity.Clear ();
	else
	{
		// scale the velocity
		float newspeed = speed - ((speed < SV_PM_STOPSPEED) ? SV_PM_STOPSPEED : speed)*SV_PM_FRICTION*1.5*pml.frameTime;
		if (newspeed < 0)
			newspeed = 0;
		newspeed /= speed;

		pml.velocity *= newspeed;
	}

	// accelerate
	float fmove = pm->Command.ForwardMove;
	float smove = pm->Command.SideMove;

	pml.forward.Normalize ();
	pml.right.Normalize ();

	vec3f wishvel	(pml.forward.X*fmove + pml.right.X*smove,
					pml.forward.Y*fmove + pml.right.Y*smove,
					pml.forward.Z*fmove + pml.right.Z*smove + pm->Command.UpMove);

	float wishspeed = wishvel.Normalize ();

	// clamp to server defined max speed
	if (wishspeed > SV_PM_MAXSPEED)
		wishspeed = SV_PM_MAXSPEED;

	float addspeed = wishspeed - (pml.velocity | wishvel);
	if (addspeed <= 0)
		return;

	float accelspeed = SV_PM_ACCELERATE*pml.frameTime*wishspeed;

	if (accelspeed > addspeed)
		accelspeed = addspeed;

	pml.velocity += accelspeed*wishvel;

	if (doClip)
	{
		vec3f end = pml.origin.MultiplyAngles (pml.frameTime, pml.velocity);
		pml.origin = CTrace (pml.origin, pm->Mins, pm->Maxs, end, pml.Player, (pml.Player->Health > 0) ? CONTENTS_MASK_PLAYERSOLID : CONTENTS_MASK_DEADSOLID).EndPosition;
	}
	else
		// move
		pml.origin = pml.origin.MultiplyAngles (pml.frameTime, pml.velocity);
}

/*
==============
SV_PM_CheckDuck

Sets mins, maxs, and pm->ViewHeight
==============
*/
static void SV_PM_CheckDuck ()
{
	pm->Mins.X = -16;
	pm->Mins.Y = -16;

	pm->Maxs.X = 16;
	pm->Maxs.Y = 16;

	if (pm->State.PMoveType == PMT_GIB)
	{
		pm->Mins.Z = 0;
		pm->Maxs.Z = 16;
		pm->ViewHeight = 8;
		return;
	}

	pm->Mins.Z = -24;

	if (pm->State.PMoveType == PMT_DEAD)
		pm->State.PMoveFlags |= PMF_DUCKED;
#ifdef ALLOW_CROUCH_JUMPING
	else if (pm->Command.UpMove < 0)
#else
	else if (pm->Command.UpMove < 0 && (pm->State.PMoveFlags & PMF_ON_GROUND))
#endif
		// duck
		pm->State.PMoveFlags |= PMF_DUCKED;
	else
	{
		// stand up if possible
		if (pm->State.PMoveFlags & PMF_DUCKED)
		{
			// try to stand up
			pm->Maxs.Z = 32;
			CTrace trace (pml.origin, pm->Mins, pm->Maxs, pml.origin, pml.Player, (pml.Player->Health > 0) ? CONTENTS_MASK_PLAYERSOLID : CONTENTS_MASK_DEADSOLID);
			if (!trace.AllSolid)
				pm->State.PMoveFlags &= ~PMF_DUCKED;
		}
	}

	if (pm->State.PMoveFlags & PMF_DUCKED)
	{
		pm->Maxs.Z = 4;
		pm->ViewHeight = -2;
	}
	else
	{
		pm->Maxs.Z = 32;
		pm->ViewHeight = 22;
	}
}

/*
==============
SV_PM_DeadMove
==============
*/
static void SV_PM_DeadMove ()
{
	if (!pm->GroundEntity)
		return;

	// extra friction
	float forward = (pml.velocity.Length() - 20);
	if (forward <= 0)
		pml.velocity.Clear ();
	else
	{
		pml.velocity.Normalize ();
		pml.velocity *= forward;
	}
}

/*
================
SV_PM_GoodPosition
================
*/
static bool SV_PM_GoodPosition ()
{
	if (pm->State.PMoveType == PMT_SPECTATOR)
		return true;

	vec3f origin	(pm->State.Origin[0]*(1.0f/8.0f),
					pm->State.Origin[1]*(1.0f/8.0f),
					pm->State.Origin[2]*(1.0f/8.0f));
 
	return !CTrace (origin, pm->Mins, pm->Maxs, origin, pml.Player, (pml.Player->Health > 0) ? CONTENTS_MASK_PLAYERSOLID : CONTENTS_MASK_DEADSOLID).AllSolid;
}

/*
================
SV_PM_SnapPosition

On exit, the origin will have a value that is pre-quantized to the (1.0f/8.0f)
precision of the network channel and in a valid position.
================
*/
static void SV_PM_SnapPosition ()
{
	vec3Base<sint32>	sign;
	vec3Base<sint16>	base;
	// try all single bits first
	static const uint8 jitterBits[8] = {0,4,1,2,3,5,6,7};

	// snap velocity to eigths
	pm->State.Velocity[0] = (sint32)(pml.velocity.X*8);
	pm->State.Velocity[1] = (sint32)(pml.velocity.Y*8);
	pm->State.Velocity[2] = (sint32)(pml.velocity.Z*8);

	for (sint32 i = 0; i < 3; i++)
	{
		if (pml.origin[i] >= 0)
			sign[i] = 1;
		else 
			sign[i] = -1;
		pm->State.Origin[i] = (sint32)(pml.origin[i]*8);

		if (pm->State.Origin[i]*(1.0f/8.0f) == pml.origin[i])
			sign[i] = 0;
	}
	base = pm->State.Origin;

	// try all combinations
	for (sint32 i = 0; i < 8; i++)
	{
		sint32 bits = jitterBits[i];
		pm->State.Origin = base;

		if (bits & BIT(0))
			pm->State.Origin[0] += sign[0];
		if (bits & BIT(1))
			pm->State.Origin[1] += sign[1];
		if (bits & BIT(2))
			pm->State.Origin[2] += sign[2];

		if (SV_PM_GoodPosition ())
			return;
	}

	// go back to the last position
	pm->State.Origin = pml.previousOrigin;
}

/*
================
SV_PM_InitialSnapPosition
================
*/
static void SV_PM_InitialSnapPosition ()
{
	vec3Base<sint16>	base;
	static sint8		offset[3] = { 0, -1, 1 };

	base = pm->State.Origin;

	for (uint8 z = 0; z < 3; z++)
	{
		pm->State.Origin[2] = base[2] + offset[z];
		for (uint8 y = 0; y < 3; y++)
		{
			pm->State.Origin[1] = base[1] + offset[y];
			for (uint8 x = 0; x < 3; x++)
			{
				pm->State.Origin[0] = base[0] + offset[x];
				if (SV_PM_GoodPosition ())
				{
					pml.origin = pm->State.Origin.ConvertDerived<vec3f>()*(1.0f/8.0f);
					pml.previousOrigin = pm->State.Origin;
					return;
				}
			}
		}
	}

	DebugPrintf ("Bad InitialSnapPosition\n");
}

/*
================
SV_PM_ClampAngles
================
*/
static void SV_PM_ClampAngles ()
{
	if (pm->State.PMoveFlags & PMF_TIME_TELEPORT)
		pm->ViewAngles.Set (0, SHORT2ANGLE (pm->Command.Angles[YAW] + pm->State.DeltaAngles[YAW]), 0);
	else
	{
		// circularly clamp the angles with deltas
		pm->ViewAngles.Set (
			SHORT2ANGLE(pm->Command.Angles[0] + pm->State.DeltaAngles[0]),
			SHORT2ANGLE(pm->Command.Angles[1] + pm->State.DeltaAngles[1]),
			SHORT2ANGLE(pm->Command.Angles[2] + pm->State.DeltaAngles[2]));

		// don't let the player look up or down more than 90 degrees
		if ((pm->ViewAngles.X > 89) && (pm->ViewAngles.X < 180))
			pm->ViewAngles.X = 89;
		else if ((pm->ViewAngles.X < 271) && (pm->ViewAngles.X >= 180))
			pm->ViewAngles.X = 271;
	}

	pm->ViewAngles.ToVectors (&pml.forward, &pml.right, &pml.up);
}

/*
================
SV_Pmove

Can be called by either the server or the client
================
*/
void SV_Pmove (CPlayerEntity *Player, SPMove *pMove, float airAcceleration)
{
	pm = pMove;
	pmAirAcceleration = airAcceleration;

	// clear results
	pm->NumTouch = 0;
	pm->ViewAngles.Clear ();
	pm->ViewHeight = 0;
	pm->GroundEntity = 0;
	pm->WaterType = 0;
	pm->WaterLevel = WATER_NONE;

	// clear all pmove local vars
	Mem_Zero (&pml, sizeof(pml));

	// convert origin and velocity to float values
	pml.origin = pm->State.Origin.ConvertDerived<vec3f>()*(1.0f/8.0f);

	pml.velocity = pm->State.Velocity.ConvertDerived<vec3f>()*(1.0f/8.0f);

	pml.Player			= Player;

	// save old org in case we get stuck
	pml.previousOrigin = pm->State.Origin;

	pml.frameTime = pm->Command.MSec * 0.001;

	SV_PM_ClampAngles ();

	if (pm->State.PMoveType == PMT_SPECTATOR)
	{
		SV_PM_FlyMove (false);
		SV_PM_SnapPosition ();
		return;
	}

	if (pm->State.PMoveType >= PMT_DEAD)
	{
		if (Game.R1Protocol)
			pml.frameTime *= 2;

		pm->Command.ForwardMove = 0;
		pm->Command.SideMove = 0;
		pm->Command.UpMove = 0;
	}

	if (pm->State.PMoveType == PMT_FREEZE)
		return;		// no movement at all

	// set mins, maxs, and ViewHeight
	SV_PM_CheckDuck ();

	if (pm->SnapInitial)
		SV_PM_InitialSnapPosition ();

	// set GroundEntity, WaterType, and WaterLevel
	SV_PM_CatagorizePosition ();

	if (pm->State.PMoveType == PMT_DEAD)
		SV_PM_DeadMove ();

	SV_PM_CheckSpecialMovement ();

	// drop timing counter
	if (pm->State.PMoveTime)
	{
		sint32		MSec = pm->Command.MSec >> 3;

		if (!MSec)
			MSec = 1;
		if (MSec >= pm->State.PMoveTime)
		{
			pm->State.PMoveFlags &= ~(PMF_TIME_WATERJUMP | PMF_TIME_LAND | PMF_TIME_TELEPORT);
			pm->State.PMoveTime = 0;
		}
		else
			pm->State.PMoveTime -= MSec;
	}

	if (pm->State.PMoveFlags & PMF_TIME_TELEPORT)
	{
		// teleport pause stays exactly in place
	}
	else if (pm->State.PMoveFlags & PMF_TIME_WATERJUMP)
	{
		// waterjump has no control, but falls
		pml.velocity.Z -= pm->State.Gravity * pml.frameTime;
		
		if (pml.velocity[2] < 0)
		{
			// cancel as soon as we are falling down again
			pm->State.PMoveFlags &= ~(PMF_TIME_WATERJUMP | PMF_TIME_LAND | PMF_TIME_TELEPORT);
			pm->State.PMoveTime = 0;
		}

		SV_PM_StepSlideMove ();
	}
	else
	{
		SV_PM_CheckJump ();

		SV_PM_Friction ();

		if (pm->WaterLevel >= WATER_WAIST)
			SV_PM_WaterMove ();
		else
		{
			vec3f	angles = pm->ViewAngles;

			if (angles.X > 180)
				angles.X -= 360;
			angles.X /= 3;

			angles.ToVectors (&pml.forward, &pml.right, &pml.up);
			SV_PM_AirMove ();
		}
	}

	// set GroundEntity, WaterType, and WaterLevel for final spot
	SV_PM_CatagorizePosition ();

	SV_PM_SnapPosition ();
}
