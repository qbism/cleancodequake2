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
// cc_monsters.cpp
// Monsters
//

#include "cc_local.h"
#include "cc_brushmodels.h"
#define STEPSIZE	18

#ifdef MONSTERS_USE_PATHFINDING
#include "cc_pathfinding.h"

bool VecInFront (vec3f &angles, vec3f &origin1, vec3f &origin2);
void CMonster::FoundPath ()
{
	if (!P_CurrentGoalNode || !P_CurrentNode)
	{
		P_NodePathTimeout = level.framenum + 100; // in 10 seconds we will try again.
		return;
	}
	// Just in case...
	if (P_CurrentGoalNode == P_CurrentNode)
		return;

	P_CurrentPath = GetPath(P_CurrentNode, P_CurrentGoalNode);

	if (!P_CurrentPath)
	{
		P_CurrentNode = P_CurrentGoalNode = NULL;
		P_CurrentNodeIndex = -1;
		P_NodePathTimeout = level.framenum + 100; // in 10 seconds we will try again.
		return;
	}

	P_CurrentNodeIndex = (int32)P_CurrentPath->Path.size()-1;

	// If our first node is behind us and it's not too far away, we can
	// just skip this node and go to the next one.	
	// Revision: Only do this if we have > 2 nodes (it messes up if we have exactly 2)
	vec3f a = Entity->State.GetAngles(), origin = Entity->State.GetOrigin();
	if (VecInFront(a, origin, P_CurrentPath->Path[P_CurrentNodeIndex]->Origin) && P_CurrentPath->Path.size() > 2)
		P_CurrentNodeIndex--;

	P_CurrentNode = P_CurrentPath->Path[P_CurrentNodeIndex];

	float timeOut = level.framenum + 20; // Always at least 2 seconds
	// Calculate approximate distance and check how long we want this to time out for
	switch (Range(origin, P_CurrentNode->Origin))
	{
	case RANGE_MELEE:
		timeOut += 60; // 10 seconds max
		break;
	case RANGE_NEAR:
		timeOut += 230; // 25 seconds
		break;
	case RANGE_MID:
		timeOut += 160; // 18 seconds
		break;
	case RANGE_FAR:
		timeOut += 300; // 32 seconds
		break;
	}
	P_NodeFollowTimeout = timeOut;

	FollowingPath = true;
	Run ();
}

void ForcePlatToGoUp (CBaseEntity *Entity);
void CMonster::MoveToPath (float Dist)
{
	if (!Entity->GroundEntity && !(Entity->Flags & (FL_FLY|FL_SWIM)))
		return;
	if (FindTarget() && (Entity->Enemy && IsVisible(Entity, Entity->Enemy))) // Did we find an enemy while going to our path?
	{
		FollowingPath = false;
		PauseTime = 100000000;

		P_CurrentPath = NULL;
		P_CurrentNode = P_CurrentGoalNode = NULL;
		return;
	}

	bool doit = false;

	if (!P_CurrentNode || !P_CurrentPath->Path.size())
	{
		FollowingPath = false;
		return;
	}

	// Check if we hit our new path.
	vec3f sub = Entity->State.GetOrigin() - P_CurrentNode->Origin;
	//CTempEnt_Trails::FleshCable (origin, P_CurrentNode->Origin, Entity->State.GetNumber());
	if (sub.Length() < 30)
	{
		bool shouldJump = (P_CurrentNode->Type == NODE_JUMP);
		// Hit the path.
		// If our node isn't the goal...
		if (P_CurrentNodeIndex > 0)
		{
			P_CurrentNodeIndex--; // Head to the next node.
			// Set our new path to the next node
			P_CurrentNode = P_CurrentPath->Path[P_CurrentNodeIndex];
			DebugPrintf ("Hit node %u\n", P_CurrentNodeIndex);
			doit = true;
			switch (P_CurrentNode->Type)
			{
			case NODE_DOOR:
					{
						CDoor *Door = entity_cast<CDoor>(P_CurrentNode->LinkedEntity); // get the plat
						Door->Use (Entity, Entity);
					}
					Stand (); // We stand, and wait.
				break;
			case NODE_PLATFORM:
				{
					CPlatForm *Plat = entity_cast<CPlatForm>(P_CurrentNode->LinkedEntity); // get the plat
					// If we reached the node, but the platform isn't down, go back two nodes
					if (Plat->MoveState != STATE_BOTTOM)
					{
						if (P_CurrentPath->Path.size() > (uint32)(P_CurrentNodeIndex + 2)) // Can we even go that far?
						{
							P_CurrentNodeIndex += 2;
							P_CurrentNode = P_CurrentPath->Path[P_CurrentNodeIndex];
							DebugPrintf ("Plat in bad spot: going back to %u\n", P_CurrentNodeIndex);
						}
					}
					else
					{
						Stand (); // We stand, and wait.
						if (Plat->MoveState == STATE_BOTTOM)
							Plat->GoUp ();
						else if (Plat->MoveState == STATE_TOP)
							Plat->NextThink = level.framenum + 10;	// the player is still on the plat, so delay going down
						//ForcePlatToGoUp (P_CurrentNode->LinkedEntity->Entity);
					}
				}

				break;
			default:
				break;
			};

			if (P_CurrentNodeIndex > 1) // If we have two more goals to destination
			{
				// In two goals, do we reach the platform node?
				if (P_CurrentPath->Path[P_CurrentNodeIndex-1]->Type == NODE_PLATFORM)
				{
					CPlatForm *Plat = entity_cast<CPlatForm>(P_CurrentPath->Path[P_CurrentNodeIndex-1]->LinkedEntity); // get the plat
					// Is it at bottom?
					if (Plat->MoveState != STATE_BOTTOM)
						Stand (); // We wait till it comes down
				}
			}

			if (shouldJump)
			{
				vec3f sub2 = P_CurrentNode->Origin - Entity->State.GetOrigin(), forward;
				sub2.ToVectors (&forward, NULL, NULL);
				Entity->Velocity = Entity->Velocity.MultiplyAngles (1.5, sub2);
				Entity->Velocity.Z = 300;
				Entity->GroundEntity = NULL;
				CheckGround();
			}
		}
		else
		{
			if (Entity->Enemy)
			{
				vec3f sub = (Entity->State.GetOrigin() - Entity->Enemy->State.GetOrigin());
				if (sub.Length() < 250) // If we're still close enough that it's possible
					// to hear him breathing (lol), start back on the trail
				{
					P_CurrentPath = NULL;
					P_CurrentGoalNode = GetClosestNodeTo(Entity->Enemy->State.GetOrigin());
					FoundPath ();
					return;
				}
				return;
			}
			FollowingPath = false;
			PauseTime = 100000000;
			Stand ();
			P_CurrentPath = NULL;
			P_CurrentNode = P_CurrentGoalNode = NULL;
			return;
		}
	}

	if (P_NodeFollowTimeout < level.framenum && P_CurrentPath && P_CurrentNode)
	{
		// Re-evaluate start and end nodes
		CPathNode *End = P_CurrentPath->Path[0];
		P_CurrentNode = GetClosestNodeTo(Entity->State.GetOrigin());
		P_CurrentGoalNode = End;
		P_CurrentPath = NULL;
		FoundPath ();

		FrameNumber_t timeOut = level.framenum + 20; // Always at least 2 seconds
		// Calculate approximate distance and check how long we want this to time out for
		switch (Range(Entity->State.GetOrigin(), P_CurrentNode->Origin))
		{
		case RANGE_MELEE:
			timeOut += 60; // 10 seconds max
			break;
		case RANGE_NEAR:
			timeOut += 230; // 25 seconds
			break;
		case RANGE_MID:
			timeOut += 160; // 18 seconds
			break;
		case RANGE_FAR:
			timeOut += 300; // 32 seconds
			break;
		}
		P_NodeFollowTimeout = timeOut;
		return;
	}

// bump around...
	if ( doit || (randomMT()&3) == 1 || !StepDirection (IdealYaw, Dist))
	{
		if (Entity->IsInUse())
		{
			float	deltax,deltay;
			float	tdir, olddir, turnaround;

			olddir = AngleModf ((int)(IdealYaw/45)*45);
			turnaround = AngleModf (olddir - 180);

			deltax = P_CurrentNode->Origin.X - Entity->State.GetOrigin().X;
			deltay = P_CurrentNode->Origin.Y - Entity->State.GetOrigin().Y;
			
			vec2f d (
				(deltax > 10) ? 0 : ((deltax < -10) ? 180 : DI_NODIR),
				(deltay < -10) ? 270 : ((deltay > 10) ? 90 : DI_NODIR)
				);

		// try direct route
			if (d.X != DI_NODIR && d.Y != DI_NODIR)
			{
				if (d.X == 0)
					tdir = d.Y == 90 ? 45 : 315;
				else
					tdir = d.Y == 90 ? 135 : 215;
					
				if (tdir != turnaround && StepDirection(tdir, Dist))
					return;
			}

		// try other directions
			if ( ((randomMT()&3) & 1) ||  Q_fabs(deltay)>Q_fabs(deltax))
			{
				tdir=d.X;
				d.X=d.Y;
				d.Y=tdir;
			}

			if (d.X!=DI_NODIR && d.X!=turnaround 
			&& StepDirection(d.X, Dist))
					return;

			if (d.Y!=DI_NODIR && d.Y!=turnaround
			&& StepDirection(d.Y, Dist))
					return;

		/* there is no direct path to the player, so pick another direction */

			if (olddir!=DI_NODIR && StepDirection(olddir, Dist))
					return;

			if (randomMT()&1) 	/*randomly determine direction of search*/
			{
				for (tdir=0 ; tdir<=315 ; tdir += 45)
					if (tdir!=turnaround && StepDirection(tdir, Dist) )
							return;
			}
			else
			{
				for (tdir=315 ; tdir >=0 ; tdir -= 45)
					if (tdir!=turnaround && StepDirection(tdir, Dist) )
							return;
			}

			if (turnaround != DI_NODIR && StepDirection(turnaround, Dist) )
					return;

			IdealYaw = olddir;		// can't move

		// if a bridge was pulled out from underneath a monster, it may not have
		// a valid standing position at all

			if (!CheckBottom ())
				Entity->Flags |= FL_PARTIALGROUND;
		}
	}
}
#endif

/*
=================
AI_SetSightClient

Called once each frame to set level.sight_client to the
player to be checked for in findtarget.

If all clients are either dead or in notarget, sight_client
will be null.

In coop games, sight_client will cycle between the clients.
=================
*/
void AI_SetSightClient ()
{
	int		start, check;

	if (level.sight_client == NULL)
		start = 1;
	else
		start = level.sight_client->State.GetNumber();

	check = start;
	while (1)
	{
		check++;
		if (check > game.maxclients)
			check = 1;
		CPlayerEntity *ent = entity_cast<CPlayerEntity>(g_edicts[check].Entity);
		if (ent->IsInUse()
			&& ent->Health > 0
			&& !(ent->Flags & FL_NOTARGET) )
		{
			level.sight_client = ent;
			return;		// got one
		}
		if (check == start)
		{
			level.sight_client = NULL;
			return;		// nobody to see
		}
	}
}

CMonsterEntity::CMonsterEntity () :
CBaseEntity(),
CMapEntity(),
CTossProjectile(),
CPushPhysics(),
CHurtableEntity(),
CThinkableEntity(),
CStepPhysics(),
CTouchableEntity(),
CUsableEntity(),
UseState(MONSTERENTITY_THINK_NONE)
{
	EntityFlags |= ENT_MONSTER;
	PhysicsType = PHYSICS_STEP;
};

CMonsterEntity::CMonsterEntity (int Index) :
CBaseEntity(Index),
CMapEntity(Index),
CTossProjectile(Index),
CPushPhysics(Index),
CHurtableEntity(Index),
CThinkableEntity(Index),
CStepPhysics(Index),
CTouchableEntity(Index),
CUsableEntity(Index),
UseState(MONSTERENTITY_THINK_NONE)
{
	EntityFlags |= ENT_MONSTER;
	PhysicsType = PHYSICS_STEP;
};

bool CMonsterEntity::CheckValidity ()
{
	if (game.mode & GAME_DEATHMATCH)
	{
		Free ();
		return false;
	}
	return CMapEntity::CheckValidity ();
}

void CMonsterEntity::Spawn ()
{
	PhysicsType = PHYSICS_STEP;
};

bool			CMonsterEntity::ParseField (char *Key, char *Value)
{
	return (CUsableEntity::ParseField (Key, Value) || CHurtableEntity::ParseField (Key, Value) || CMapEntity::ParseField (Key, Value));
};

void CMonsterEntity::Think ()
{
	if (IsHead)
		Free ();
	else if (Monster->Think)
	{
		void	(CMonster::*TheThink) () = Monster->Think;
		(Monster->*TheThink) ();
	}
}

void CMonsterEntity::Die(CBaseEntity *inflictor, CBaseEntity *attacker, int damage, vec3f &point)
{
	Monster->Die (inflictor, attacker, damage, point);
}

void CMonsterEntity::Pain (CBaseEntity *other, float kick, int damage)
{
	Monster->Pain (other, kick, damage);
}

void CMonsterEntity::Touch (CBaseEntity *other, plane_t *plane, cmBspSurface_t *surf)
{
	Monster->Touch (other, plane, surf);
}

vec3f VelocityForDamage (int damage);
void ClipGibVelocity (CPhysicsEntity *);
void CMonsterEntity::ThrowHead (MediaIndex gibIndex, int damage, int type)
{
	float	vscale;

	IsHead = true;
	State.SetSkinNum (0);
	State.SetFrame (0);

	SetMins (vec3fOrigin);
	SetMaxs (vec3fOrigin);

	State.SetModelIndex (0, 2);
	State.SetModelIndex (gibIndex);
	SetSolid (SOLID_NOT);
	State.AddEffects (EF_GIB);
	State.RemoveEffects (EF_FLIES);
	State.SetSound (0);
	Flags |= FL_NO_KNOCKBACK;
	SetSvFlags (GetSvFlags() & ~SVF_MONSTER);
	CanTakeDamage = true;

	if (type == GIB_ORGANIC)
	{
		PhysicsType = PHYSICS_TOSS;
		vscale = 0.5;
	}
	else
	{
		PhysicsType = PHYSICS_BOUNCE;
		vscale = 1.0;
	}
	
	Velocity = Velocity.MultiplyAngles (vscale, VelocityForDamage (damage));
	ClipGibVelocity (this);
	AngularVelocity.Y = crandom()*600;

	NextThink = level.framenum + 100 + random()*100;

	Link();
}

bool CMonsterEntity::Run ()
{
	switch (PhysicsType)
	{
	case PHYSICS_TOSS:
		return CTossProjectile::Run();
	case PHYSICS_BOUNCE:
		backOff = 1.5f;
		return CBounceProjectile::Run ();
	case PHYSICS_PUSH:
		return CPushPhysics::Run ();
	default:
		return CStepPhysics::Run ();
	};
}

void CMonster::ChangeYaw ()
{
	vec3f angles = Entity->State.GetAngles();
	float current = AngleModf (angles.Y);

	if (current == IdealYaw)
		return;

	float move = IdealYaw - current;
	if (IdealYaw > current)
	{
		if (move >= 180)
			move = move - 360;
	}
	else
	{
		if (move <= -180)
			move = move + 360;
	}
	if (move > 0)
	{
		if (move > YawSpeed)
			move = YawSpeed;
	}
	else
	{
		if (move < -YawSpeed)
			move = -YawSpeed;
	}
	
	angles.Y = AngleModf(current+move);
	Entity->State.SetAngles(angles);
}

bool CMonster::CheckBottom ()
{
	vec3f	mins, maxs, start, stop;
	CTrace	trace;
	int		x, y;
	float	mid, bottom;
	
	mins = Entity->State.GetOrigin() + Entity->GetMins();
	maxs = Entity->State.GetOrigin() + Entity->GetMaxs();

// if all of the points under the corners are solid world, don't bother
// with the tougher checks
// the corners must be within 16 of the midpoint
	bool gotOutEasy = false;
	start[2] = mins[2] - 1;
	for	(x=0 ; x<=1 && !gotOutEasy; x++)
	{
		for	(y=0 ; y<=1 && !gotOutEasy; y++)
		{
			start.X = x ? maxs.X : mins.X;
			start.Y = y ? maxs.Y : mins.Y;

			if (PointContents (start) != CONTENTS_SOLID)
				gotOutEasy = true;
		}

	}

	if (gotOutEasy)
		return true;		// we got out easy

//
// check it for real...
//
	start.Z = mins.Z;
	
// the midpoint must be within 16 of the bottom
	start.X = stop.X = (mins.X + maxs.X)*0.5;
	start.Y = stop.Y = (mins.Y + maxs.Y)*0.5;
	stop.Z = start.Z - 2*STEPSIZE;
	trace = CTrace(start, stop, Entity->gameEntity, CONTENTS_MASK_MONSTERSOLID);

	if (trace.fraction == 1.0)
		return false;
	mid = bottom = trace.endPos[2];
	
// the corners must be within 16 of the midpoint	
	for	(x=0 ; x<=1 ; x++)
		for	(y=0 ; y<=1 ; y++)
		{
			start.X = stop.X = x ? maxs.X : mins.X;
			start.Y = stop.Y = y ? maxs.Y : mins.Y;
			
			trace = CTrace(start, stop, Entity->gameEntity, CONTENTS_MASK_MONSTERSOLID);
			
			if (trace.fraction != 1.0 && trace.endPos[2] > bottom)
				bottom = trace.endPos[2];
			if (trace.fraction == 1.0 || mid - trace.endPos[2] > STEPSIZE)
				return false;
		}

	return true;
}

void CMonster::MoveToGoal (float Dist)
{
	CBaseEntity *goal = (Entity->gameEntity->goalentity) ? Entity->gameEntity->goalentity->Entity : Entity->Enemy;

	if (!Entity->GroundEntity && !(Entity->Flags & (FL_FLY|FL_SWIM)))
		return;

// if the next step hits the enemy, return immediately
	if (Entity->Enemy && CloseEnough (Entity->Enemy, Dist) )
		return;

// bump around...
	if ( (randomMT()&3)==1 || !StepDirection (IdealYaw, Dist))
	{
		if (Entity->IsInUse())
			NewChaseDir (goal, Dist);
	}
}

bool CMonster::WalkMove (float Yaw, float Dist)
{	
	if (!Entity->GroundEntity && !(Entity->Flags & (FL_FLY|FL_SWIM)))
		return false;

	Yaw = Yaw*M_PI*2 / 360;
	return MoveStep(vec3f (cosf(Yaw)*Dist, sinf(Yaw)*Dist, 0), true);
}

bool CMonster::MoveStep (vec3f move, bool ReLink)
{
	vec3f		end;
	CTrace		trace;
	float		stepsize;

// try the move	
	vec3f oldorg = Entity->State.GetOrigin();

// flying monsters don't step up
	if (Entity->Flags & (FL_SWIM | FL_FLY))
	{
	// try one move with vertical motion, then one without
		for (int i = 0; i < 2; i++)
		{
			vec3f neworg = oldorg + move;
			if (i == 0 && Entity->Enemy)
			{
				if (!Entity->gameEntity->goalentity)
					Entity->gameEntity->goalentity = Entity->Enemy->gameEntity;

				float dz = oldorg.Z - Entity->gameEntity->goalentity->state.origin[2];
				if (Entity->gameEntity->goalentity->client)
				{
					if (dz > 40)
						neworg.Z -= 8;

					if (!((Entity->Flags & FL_SWIM) && (Entity->gameEntity->waterlevel < 2)) && (dz < 30))
						neworg.Z += 8;
				}
				else
				{
					if (dz > 8)
						neworg.Z -= 8;
					else if (dz > 0)
						neworg.Z -= dz;
					else if (dz < -8)
						neworg.Z += 8;
					else
						neworg.Z += dz;
				}
			}
			trace = CTrace(oldorg, Entity->GetMins(), Entity->GetMaxs(), neworg, Entity->gameEntity, CONTENTS_MASK_MONSTERSOLID);
	
			// fly monsters don't enter water voluntarily
			if (Entity->Flags & FL_FLY)
			{
				if (!Entity->gameEntity->waterlevel)
				{
					vec3f test (trace.EndPos);
					test.Z += Entity->GetMins().Z + 1;
					if (PointContents(test) & CONTENTS_MASK_WATER)
						return false;
				}
			}

			// swim monsters don't exit water voluntarily
			if (Entity->Flags & FL_SWIM)
			{
				if (Entity->gameEntity->waterlevel < 2)
				{
					vec3f test (trace.EndPos);
					test.Z += Entity->GetMins().Z + 1;
					if (!(PointContents(test) & CONTENTS_MASK_WATER))
						return false;
				}
			}

			if (trace.fraction == 1)
			{
				Entity->State.SetOrigin (trace.endPos);
				if (ReLink)
				{
					Entity->Link ();
					G_TouchTriggers (Entity);
				}
				return true;
			}
			
			if (!Entity->Enemy)
				break;
		}
		
		return false;
	}

	vec3f neworg = oldorg + move;

	// push down from a step height above the wished position
	stepsize = (AIFlags & AI_NOSTEP) ? 1 :  STEPSIZE;

	neworg.Z += stepsize;
	end = neworg;
	end.Z -= stepsize * 2;

	trace = CTrace(neworg, Entity->GetMins(), Entity->GetMaxs(), end, Entity->gameEntity, CONTENTS_MASK_MONSTERSOLID);

	if (trace.allSolid)
		return false;

	if (trace.startSolid)
	{
		neworg.Z -= stepsize;
		trace = CTrace(neworg, Entity->GetMins(), Entity->GetMaxs(), end, Entity->gameEntity, CONTENTS_MASK_MONSTERSOLID);
		if (trace.allSolid || trace.startSolid)
			return false;
	}


	// don't go in to water
	/*if (Entity->gameEntity->waterlevel == 0)
	{
		test[0] = trace.endPos[0];
		test[1] = trace.endPos[1];
		test[2] = trace.endPos[2] + Entity->mins[2] + 1;	
		contents = PointContents(test);

		if (contents & CONTENTS_MASK_WATER)
			return false;
	}*/

	if (trace.fraction == 1)
	{
	// if monster had the ground pulled out, go ahead and fall
		if (Entity->Flags & FL_PARTIALGROUND)
		{
			Entity->State.SetOrigin (Entity->State.GetOrigin() + move);
			if (ReLink)
			{
				Entity->Link ();
				G_TouchTriggers (Entity);
			}
			Entity->GroundEntity = NULL;
			return true;
		}
	
		return false;		// walked off an edge
	}

// check point traces down for dangling corners
	Entity->State.SetOrigin(trace.endPos);
	
	if (!CheckBottom ())
	{
		if (Entity->Flags & FL_PARTIALGROUND)
		{	// entity had floor mostly pulled out from underneath it
			// and is trying to correct
			if (ReLink)
			{
				Entity->Link ();
				G_TouchTriggers (Entity);
			}
			return true;
		}

		// Check to see if there's floor just after this
		vec3f org, up, end2;
		Entity->State.GetAngles().ToVectors (NULL, NULL, &up);
		org = Entity->State.GetOrigin() + move;

		// Go down
		end2 = org.MultiplyAngles (-STEPSIZE, up);

		// Trace
		trace = CTrace (org, end2, Entity->gameEntity, CONTENTS_MASK_SOLID);

		// Couldn't make the move
		if (trace.fraction == 1.0)
		{
			Entity->State.SetOrigin (oldorg);
			return false;
		}
	}

	//if ( Entity->Flags & FL_PARTIALGROUND )
	Entity->Flags &= ~FL_PARTIALGROUND;

	Entity->GroundEntity = trace.Ent;
	Entity->GroundEntityLinkCount = trace.Ent->GetLinkCount();

// the move is ok
	if (ReLink)
	{
		Entity->Link ();
		G_TouchTriggers (Entity);
	}
	return true;
}

bool CMonster::CloseEnough (CBaseEntity *Goal, float Dist)
{
	for (int i=0 ; i<3 ; i++)
	{
		if (Goal->GetAbsMin()[i] > Entity->GetAbsMax()[i] + Dist)
			return false;
		if (Goal->GetAbsMax()[i] < Entity->GetAbsMin()[i] - Dist)
			return false;
	}
	return true;
}

void CMonster::NewChaseDir (CBaseEntity *Enemy, float Dist)
{
	//FIXME: how did we get here with no enemy
	if (!Enemy)
	{
//		assert (0); // Let's find out!
		return;
	}

	float olddir = AngleModf ((int)(IdealYaw/45)*45);
	float turnaround = AngleModf (olddir - 180);

	float deltax = Enemy->State.GetOrigin().X - Entity->State.GetOrigin().X;
	float deltay = Enemy->State.GetOrigin().Y - Entity->State.GetOrigin().Y;
	
	vec2f d (
		((deltax > 10) ? 0 : (deltax < -10) ? 180 : DI_NODIR),
		((deltay < -10) ? 270 : (deltay > 10) ? 90 : DI_NODIR));

// try direct route
	float tdir;
	if (d.X != DI_NODIR && d.Y != DI_NODIR)
	{
		if (d.X == 0)
			tdir = (d.Y == 90) ? 45 : 315;
		else
			tdir = (d.Y == 90) ? 135 : 215;
			
		if (tdir != turnaround && StepDirection(tdir, Dist))
			return;
	}

// try other directions
	if ( ((randomMT()&3) & 1) ||  Q_fabs(deltay) > Q_fabs(deltax))
	{
		tdir = d.X;
		d.X = d.Y;
		d.Y = tdir;
	}

	if (d.X != DI_NODIR && d.X != turnaround && StepDirection(d.X, Dist))
			return;

	if (d.Y != DI_NODIR && d.Y != turnaround && StepDirection(d.Y, Dist))
			return;

/* there is no direct path to the player, so pick another direction */

	if (olddir != DI_NODIR && StepDirection(olddir, Dist))
			return;

	if (randomMT()&1) 	/*randomly determine direction of search*/
	{
		for (tdir = 0; tdir <= 315; tdir += 45)
			if (tdir != turnaround && StepDirection(tdir, Dist) )
					return;
	}
	else
	{
		for (tdir = 315; tdir >= 0; tdir -= 45)
			if (tdir != turnaround && StepDirection(tdir, Dist) )
					return;
	}

	if (turnaround != DI_NODIR && StepDirection(turnaround, Dist) )
			return;

	IdealYaw = olddir;		// can't move

// if a bridge was pulled out from underneath a monster, it may not have
// a valid standing position at all

	if (!CheckBottom ())
		Entity->Flags |= FL_PARTIALGROUND;
}

bool CMonster::StepDirection (float Yaw, float Dist)
{	
	IdealYaw = Yaw;
	ChangeYaw ();
	Yaw = Yaw*M_PI*2 / 360;

	vec3f oldOrigin = Entity->State.GetOrigin ();
	if (MoveStep (vec3f(cosf(Yaw) * Dist, sinf(Yaw) * Dist, 0), false))
	{
		float delta = Entity->State.GetAngles().Y - IdealYaw;
		// not turned far enough, so don't take the step
		if (delta > 45 && delta < 315)
			Entity->State.SetOrigin (oldOrigin);

		Entity->Link ();
		G_TouchTriggers (Entity);
		return true;
	}

	Entity->Link ();
	G_TouchTriggers (Entity);
	return false;
}

void CMonster::WalkMonsterStartGo ()
{
	if (!(Entity->SpawnFlags & 2) && level.framenum < 10)
	{
		DropToFloor ();

		if (Entity->GroundEntity)
		{
			if (!WalkMove (0, 0))
				MapPrint (MAPPRINT_WARNING, Entity, Entity->State.GetOrigin(), "In solid\n");
		}
	}
	
	if (!YawSpeed)
		YawSpeed = 20;
	Entity->ViewHeight = 25;

	MonsterStartGo ();

	if (Entity->SpawnFlags & 2)
		MonsterTriggeredStart ();
}

void CMonster::WalkMonsterStart ()
{
	Think = &CMonster::WalkMonsterStartGo;
	Entity->NextThink = level.framenum + FRAMETIME;
	MonsterStart ();
}

void CMonster::SwimMonsterStartGo ()
{
	if (!YawSpeed)
		YawSpeed = 10;
	Entity->ViewHeight = 10;

	MonsterStartGo ();

	if (Entity->SpawnFlags & 2)
		MonsterTriggeredStart ();
}

void CMonster::SwimMonsterStart ()
{
	Entity->Flags |= FL_SWIM;
	Think = &CMonster::SwimMonsterStartGo;
	MonsterStart ();
}

void CMonster::FlyMonsterStartGo ()
{
	if (!WalkMove (0, 0))
		MapPrint (MAPPRINT_WARNING, Entity, Entity->State.GetOrigin(), "Entity in solid\n");

	if (!YawSpeed)
		YawSpeed = 10;
	Entity->ViewHeight = 25;

	MonsterStartGo ();

	if (Entity->SpawnFlags & 2)
		MonsterTriggeredStart ();
}

void CMonster::FlyMonsterStart ()
{
	Entity->Flags |= FL_FLY;
	Think = &CMonster::FlyMonsterStartGo;
	MonsterStart ();
}

void CMonster::MonsterStartGo ()
{
	if (Entity->Health <= 0)
		return;

	// check for target to combat_point and change to combattarget
	if (Entity->gameEntity->target)
	{
		bool		notcombat = false, fixup = false;
		CBaseEntity		*target = NULL;

		while ((target = CC_Find (target, FOFS(targetname), Entity->gameEntity->target)) != NULL)
		{
			if (strcmp(target->gameEntity->classname, "point_combat") == 0)
			{
				Entity->gameEntity->combattarget = Entity->gameEntity->target;
				fixup = true;
			}
			else
				notcombat = true;
		}
		if (notcombat && Entity->gameEntity->combattarget)
			MapPrint (MAPPRINT_WARNING, Entity, Entity->State.GetOrigin(), "Target with mixed types\n");
			//gi.dprintf("%s at (%f %f %f) has target with mixed types\n", self->classname, self->state.origin[0], self->state.origin[1], self->state.origin[2]);
		if (fixup)
			Entity->gameEntity->target = NULL;
	}

	// validate combattarget
	if (Entity->gameEntity->combattarget)
	{
		CBaseEntity		*target = NULL;
		while ((target = CC_Find (target, FOFS(targetname), Entity->gameEntity->combattarget)) != NULL)
		{
			if (strcmp(target->gameEntity->classname, "point_combat") != 0)
				MapPrint (MAPPRINT_WARNING, Entity, Entity->State.GetOrigin(), "Has a bad combattarget (\"%s\")\n", Entity->gameEntity->combattarget);
		}
	}

	if (Entity->gameEntity->target)
	{
		CBaseEntity *Target = CC_PickTarget(Entity->gameEntity->target);

		if (Target)
			Entity->gameEntity->goalentity = Entity->gameEntity->movetarget = Target->gameEntity;
		if (!Entity->gameEntity->movetarget)
		{
			//gi.dprintf ("%s can't find target %s at (%f %f %f)\n", self->classname, self->target, self->state.origin[0], self->state.origin[1], self->state.origin[2]);
			MapPrint (MAPPRINT_WARNING, Entity, Entity->State.GetOrigin(), "Can't find target\n");
			Entity->gameEntity->target = NULL;
			PauseTime = 100000000;
			Stand ();
		}
		else if (strcmp (Entity->gameEntity->movetarget->classname, "path_corner") == 0)
		{
			vec3f angles = Entity->State.GetAngles();
			IdealYaw = angles.Y = (Entity->gameEntity->goalentity->Entity->State.GetOrigin() - Entity->State.GetOrigin()).ToYaw();
			Entity->State.SetAngles(angles);
			Walk ();
			Entity->gameEntity->target = NULL;
		}
		else
		{
			Entity->gameEntity->goalentity = Entity->gameEntity->movetarget = NULL;
			PauseTime = 100000000;
			Stand ();
		}
	}
	else
	{
		PauseTime = 100000000;
		Stand ();
	}

	// are we in debug mode?
	if (map_debug->Integer())
	{
		Think = NULL; // Don't think
		
		// Make us non-solid
		Entity->SetSolid (SOLID_NOT);
	}
	else
	{
		Think = &CMonster::MonsterThink;
		Entity->NextThink = level.framenum + FRAMETIME;
	}
}

void CMonster::MonsterStart ()
{
//	DebugPrintf ("%s spawned with ID %u\n", MonsterName, MonsterID);
	if (game.mode & GAME_DEATHMATCH)
	{
		Entity->Free ();
		return;
	}

	if ((Entity->SpawnFlags & 4) && !(AIFlags & AI_GOOD_GUY))
	{
		Entity->SpawnFlags &= ~4;
		Entity->SpawnFlags |= 1;
	}

	if (!(AIFlags & AI_GOOD_GUY))
		level.total_monsters++;

	Entity->NextThink = level.framenum + FRAMETIME;
	Entity->SetSvFlags (Entity->GetSvFlags() | SVF_MONSTER);
	Entity->State.AddRenderEffects (RF_FRAMELERP);
	Entity->CanTakeDamage = true;
	Entity->AirFinished = level.framenum + 120;
	Entity->UseState = MONSTERENTITY_THINK_USE;
	Entity->MaxHealth = Entity->Health;
	Entity->SetClipmask (CONTENTS_MASK_MONSTERSOLID);

	Entity->DeadFlag = false;
	Entity->SetSvFlags (Entity->GetSvFlags() & ~SVF_DEADMONSTER);

	Entity->State.SetOldOrigin(Entity->State.GetOrigin());

	// randomize what frame they start on
	if (CurrentMove)
		Entity->State.SetFrame(CurrentMove->FirstFrame + (irandom(CurrentMove->LastFrame - CurrentMove->FirstFrame + 1)));

#ifdef MONSTER_USE_ROGUE_AI
	BaseHeight = Entity->GetMaxs().Z;
#endif

	Entity->NextThink = level.framenum + FRAMETIME;
}

void CMonster::MonsterTriggeredStart ()
{
	Entity->SetSolid (SOLID_NOT);
	Entity->PhysicsDisabled = true;

	if (!map_debug->Integer())
		Entity->SetSvFlags (Entity->GetSvFlags() | SVF_NOCLIENT);
	else
		Entity->State.SetEffects (EF_SPHERETRANS);
	Entity->NextThink = 0;
	Think = NULL;
	Entity->UseState = MONSTERENTITY_THINK_TRIGGEREDSPAWNUSE;
}

void CMonsterEntity::Use (CBaseEntity *other, CBaseEntity *activator)
{
	switch (UseState)
	{
	case MONSTERENTITY_THINK_NONE:
		break;
	case MONSTERENTITY_THINK_USE:
		if (Enemy)
			return;
		if (Health <= 0)
			return;
		if (activator->Flags & FL_NOTARGET)
			return;
		if (!(activator->EntityFlags & ENT_PLAYER) && !(Monster->AIFlags & AI_GOOD_GUY))
			return;
		
	// delay reaction so if the monster is teleported, its sound is still heard
		Enemy = activator;
		Monster->FoundTarget ();
		break;
	case MONSTERENTITY_THINK_TRIGGEREDSPAWNUSE:
		// we have a one frame delay here so we don't telefrag the guy who activated us
		Monster->Think = &CMonster::MonsterTriggeredSpawn;
		NextThink = level.framenum + FRAMETIME;
		if (activator->EntityFlags & ENT_PLAYER)
			Enemy = activator;
		UseState = MONSTERENTITY_THINK_USE;
		break;
	};
}

void CMonster::MonsterTriggeredSpawn ()
{
	vec3f newOrigin = Entity->State.GetOrigin();
	newOrigin.Z += 1;
	Entity->State.SetOrigin (newOrigin);
	Entity->KillBox ();

	Entity->SetSolid (SOLID_BBOX);
	Entity->PhysicsDisabled = false;
	Entity->SetSvFlags (Entity->GetSvFlags() & ~SVF_NOCLIENT);
	Entity->AirFinished = level.framenum + 120;
	Entity->Link ();

	MonsterStartGo ();

	if (Entity->Enemy && !(Entity->SpawnFlags & 1) && !(Entity->Enemy->Flags & FL_NOTARGET))
		FoundTarget ();
	else
		Entity->Enemy = NULL;
}

#ifdef MONSTERS_ARENT_STUPID
bool CMonster::FriendlyInLine (vec3f &Origin, vec3f &Direction)
{
	vec3f forward;
	Direction.ToAngles ().ToVectors (&forward, NULL, NULL);

	vec3f end = Origin.MultiplyAngles (8192, forward);
	CTrace trace = CTrace(Origin, end, Entity->gameEntity, CONTENTS_MONSTER);

	if (trace.fraction <= 0.5 && trace.ent && (trace.ent->Entity && (trace.ent->Entity->EntityFlags & ENT_MONSTER)) &&
		(entity_cast<CMonsterEntity>(trace.Ent)->Enemy != Entity))
		return true;
	return false;
}
#endif

/*
=================
AlertNearbyStroggs

Alerts nearby Stroggs of possible enemy targets
=================
*/

void CMonster::AlertNearbyStroggs ()
{
	float dist;
	CMonsterEntity		*strogg = NULL;

	if (Entity->Enemy->Flags & FL_NOTARGET)
		return;

	switch (skill->Integer())
	{
	case 0:
		return;
	case 1:
		dist = 300;
		break;
	case 2:
		dist = 500;
		break;
	default:
		dist = 750 + (skill->Integer()) * 75;
		break;
	}

	if (dist > 2400)
		dist = 2400;

	vec3f origin = Entity->State.GetOrigin ();
	while ( (strogg = FindRadius<CMonsterEntity, ENT_MONSTER>(strogg, origin, dist)) != NULL)
	{
		if (strogg->Health < 1 || !(strogg->CanTakeDamage))
			continue;
		if (strogg == Entity)
			continue;
		if (!(strogg->EntityFlags & ENT_MONSTER))
			continue;
		if (strogg->Enemy)
			continue;
		if (strogg->SpawnFlags & 1)
			continue;
		
#ifdef MONSTERS_USE_PATHFINDING
		// Set us up for pathing
		// Revision: if we aren't visible, that is.
		if (!IsVisible(strogg, Entity->Enemy))
		{
			strogg->Monster->P_CurrentNode = GetClosestNodeTo(strogg->State.GetOrigin());
			strogg->Monster->P_CurrentGoalNode = GetClosestNodeTo(Entity->Enemy->State.GetOrigin());
			//FoundStrogg->Enemy = Entity->Enemy;
			strogg->Monster->FoundPath ();
		}
		else
		{
			strogg->Enemy = Entity->Enemy;
			strogg->Monster->FoundTarget ();
			strogg->Monster->Sight ();
		}
#else
		strogg->Enemy = Entity->Enemy;
		strogg->Monster->FoundTarget ();
		strogg->Monster->Sight ();
#endif
	}
}

void CMonster::MonsterFireBullet (vec3f start, vec3f dir, int damage, int kick, int hspread, int vspread, int flashtype)
{
#ifdef MONSTERS_ARENT_STUPID
	if (FriendlyInLine (start, dir))
		return;
#endif

	CBullet::Fire (Entity, start, dir, damage, kick, hspread, vspread, MOD_MACHINEGUN);

	if (flashtype != -1)
		CTempEnt::MonsterFlash (start, Entity->State.GetNumber(), flashtype);
}

void CMonster::MonsterFireShotgun (vec3f start, vec3f aimdir, int damage, int kick, int hspread, int vspread, int count, int flashtype)
{
#ifdef MONSTERS_ARENT_STUPID
	if (FriendlyInLine (start, aimdir))
		return;
#endif

	CShotgunPellets::Fire (Entity, start, aimdir, damage, kick, hspread, vspread, count, MOD_SHOTGUN);

	if (flashtype != -1)
		CTempEnt::MonsterFlash (start, Entity->State.GetNumber(), flashtype);
}

void CMonster::MonsterFireBlaster (vec3f start, vec3f dir, int damage, int speed, int flashtype, int effect)
{
#ifdef MONSTERS_ARENT_STUPID
	if (FriendlyInLine (start, dir))
		return;
#endif

	CBlasterProjectile::Spawn (Entity, start, dir, damage, speed, effect, false);

	if (flashtype != -1)
		CTempEnt::MonsterFlash (start, Entity->State.GetNumber(), flashtype);
}	

void CMonster::MonsterFireGrenade (vec3f start, vec3f aimdir, int damage, int speed, int flashtype)
{
#ifdef MONSTERS_ARENT_STUPID
	if (FriendlyInLine (start, aimdir))
		return;
#endif

	CGrenade::Spawn (Entity, start, aimdir, damage, speed, 2.5, damage+40);

	if (flashtype != -1)
		CTempEnt::MonsterFlash (start, Entity->State.GetNumber(), flashtype);
}

void CMonster::MonsterFireRocket (vec3f start, vec3f dir, int damage, int speed, int flashtype)
{
#ifdef MONSTERS_ARENT_STUPID
	if (FriendlyInLine (start, dir))
		return;
#endif

	CRocket::Spawn (Entity, start, dir, damage, speed, damage+20, damage);

	if (flashtype != -1)
		CTempEnt::MonsterFlash (start, Entity->State.GetNumber(), flashtype);
}	

void CMonster::MonsterFireRailgun (vec3f start, vec3f aimdir, int damage, int kick, int flashtype)
{
#ifdef MONSTERS_ARENT_STUPID
	if (FriendlyInLine (start, aimdir))
		return;
#endif

	if (!(PointContents (start) & CONTENTS_MASK_SOLID))
		CRailGunShot::Fire (Entity, start, aimdir, damage, kick);

	if (flashtype != -1)
		CTempEnt::MonsterFlash (start, Entity->State.GetNumber(), flashtype);
}

void CMonster::MonsterFireBfg (vec3f start, vec3f aimdir, int damage, int speed, int kick, float damage_radius, int flashtype)
{
#ifdef MONSTERS_ARENT_STUPID
	if (FriendlyInLine (start, aimdir))
		return;
#endif

	CBFGBolt::Spawn (Entity, start, aimdir, damage, speed, damage_radius);

	if (flashtype != -1)
		CTempEnt::MonsterFlash (start, Entity->State.GetNumber(), flashtype);
}

bool CMonster::CheckAttack ()
{
#ifndef MONSTER_USE_ROGUE_AI
	vec3f	spot1, spot2;
	float	chance;
	CTrace	tr;

	if (entity_cast<CHurtableEntity>(Entity->Enemy)->Health > 0)
	{
	// see if any entities are in the way of the shot
		spot1 = Entity->State.GetOrigin();
		spot1.Z += Entity->ViewHeight;
		spot2 = Entity->Enemy->State.GetOrigin();
		spot2.Z += Entity->Enemy->ViewHeight;

		tr = CTrace(spot1, spot2, Entity->gameEntity, CONTENTS_SOLID|CONTENTS_MONSTER|CONTENTS_SLIME|CONTENTS_LAVA|CONTENTS_WINDOW);

		// do we have a clear shot?
		if (tr.ent != Entity->Enemy)
			return false;
	}
	
	// melee attack
	if (EnemyRange == RANGE_MELEE)
	{
		// don't always melee in easy mode
		if (skill->Integer() == 0 && (randomMT()&3) )
			return false;
		if (MonsterFlags & MF_HAS_MELEE)
			AttackState = AS_MELEE;
		else
			AttackState = AS_MISSILE;
		return true;
	}
	
// missile attack
	if (!(MonsterFlags & MF_HAS_ATTACK))
		return false;
		
	if (level.framenum < AttackFinished)
		return false;
		
	if (EnemyRange == RANGE_FAR)
		return false;

	if (AIFlags & AI_STAND_GROUND)
	{
		chance = 0.4f;
	}
	else if (EnemyRange == RANGE_MELEE)
	{
		chance = 0.2f;
	}
	else if (EnemyRange == RANGE_NEAR)
	{
		chance = 0.1f;
	}
	else if (EnemyRange == RANGE_MID)
	{
		chance = 0.02f;
	}
	else
	{
		return false;
	}

	if (skill->Integer() == 0)
		chance *= 0.5;
	else if (skill->Integer() >= 2)
		chance *= 2;

	if (random () < chance)
	{
		AttackState = AS_MISSILE;
		AttackFinished = level.framenum + ((2*random())*10);
		return true;
	}

	if (Entity->Flags & FL_FLY)
	{
		if (random() < 0.3)
			AttackState = AS_SLIDING;
		else
			AttackState = AS_STRAIGHT;
	}

	return false;
#else
	float	chance;

	if (entity_cast<CHurtableEntity>(Entity->Enemy)->Health > 0)
	{
	// see if any entities are in the way of the shot
		vec3f	spot1, spot2;
		spot1 = Entity->State.GetOrigin ();
		spot1.Z += Entity->ViewHeight;
		spot2 = Entity->Enemy->State.GetOrigin();
		spot2.Z += Entity->Enemy->ViewHeight;

		CTrace tr (spot1, spot2, Entity->gameEntity, CONTENTS_SOLID|CONTENTS_MONSTER|CONTENTS_SLIME|CONTENTS_LAVA|CONTENTS_WINDOW);

		// do we have a clear shot?
		if (tr.Ent != Entity->Enemy)
		{	
			// PGM - we want them to go ahead and shoot at info_notnulls if they can.
			if(Entity->Enemy->GetSolid() != SOLID_NOT || tr.fraction < 1.0)		//PGM
			{
				// PMM - if we can't see our target, and we're not blocked by a monster, go into blind fire if available
				if ((!(tr.ent->svFlags & SVF_MONSTER)) && (!IsVisible(Entity, Entity->Enemy)))
				{
					if ((BlindFire) && (BlindFireDelay <= 20.0))
					{
						if (level.framenum < AttackFinished)
							return false;
						if (level.framenum < (TrailTime + BlindFireDelay))
							// wait for our time
							return false;
						else
						{
							// make sure we're not going to shoot a monster
							tr = CTrace (spot1, BlindFireTarget, Entity->gameEntity, CONTENTS_MONSTER);
							if (tr.allSolid || tr.startSolid || ((tr.fraction < 1.0) && (tr.Ent != Entity->Enemy)))
								return false;

							AttackState = AS_BLIND;
							return true;
						}
					}
				}
				// pmm
				return false;
			}
		}
	}
	
	// melee attack
	if (EnemyRange == RANGE_MELEE)
	{
		// don't always melee in easy mode
		if (skill->Integer() == 0 && (randomMT()&3) )
		{
			// PMM - fix for melee only monsters & strafing
			AttackState = AS_STRAIGHT;
			return false;
		}
		if (MonsterFlags & MF_HAS_MELEE)
			AttackState = AS_MELEE;
		else
			AttackState = AS_MISSILE;
		return true;
	}
	
// missile attack
	if (!(MonsterFlags & MF_HAS_ATTACK))
	{
		// PMM - fix for melee only monsters & strafing
		AttackState = AS_STRAIGHT;
		return false;
	}
	
	if (level.framenum < AttackFinished)
		return false;
		
	if (EnemyRange == RANGE_FAR)
		return false;

	if (AIFlags & AI_STAND_GROUND)
		chance = 0.4f;
	else if (EnemyRange == RANGE_MELEE)
		chance = 0.2f;
	else if (EnemyRange == RANGE_NEAR)
		chance = 0.1f;
	else if (EnemyRange == RANGE_MID)
		chance = 0.02f;
	else
		return false;

	if (skill->Integer() == 0)
		chance *= 0.5;
	else if (skill->Integer() >= 2)
		chance *= 2;

	// PGM - go ahead and shoot every time if it's a info_notnull
	if ((random () < chance) || (Entity->Enemy->GetSolid() == SOLID_NOT))
	{
		AttackState = AS_MISSILE;
		AttackFinished = level.framenum + ((2*random())*10);
		return true;
	}

	// PMM -daedalus should strafe more .. this can be done here or in a customized
	// check_attack code for the hover.
	if (Entity->Flags & FL_FLY)
	{
		// originally, just 0.3
		float strafe_chance;
		if (!(strcmp(Entity->gameEntity->classname, "monster_daedalus")))
			strafe_chance = 0.8f;
		else
			strafe_chance = 0.6f;

		// if enemy is tesla, never strafe
		if ((Entity->Enemy) && (Entity->Enemy->gameEntity->classname) && (!strcmp(Entity->Enemy->gameEntity->classname, "tesla")))
			strafe_chance = 0;

		if (random() < strafe_chance)
			AttackState = AS_SLIDING;
		else
			AttackState = AS_STRAIGHT;
	}
// do we want the monsters strafing?
	else
	{
		if (random() < 0.4)
			AttackState = AS_SLIDING;
		else
			AttackState = AS_STRAIGHT;
	}

//-PMM

	return false;
#endif
}

void CMonster::DropToFloor ()
{
	vec3f		origin = Entity->State.GetOrigin() + vec3f(0, 0, 1);
	Entity->State.SetOrigin(origin);

	vec3f end = origin - vec3f(0, 0, 256);	
	CTrace trace (Entity->State.GetOrigin(), Entity->GetMins(), Entity->GetMaxs(), end, Entity->gameEntity, CONTENTS_MASK_MONSTERSOLID);

	if (trace.fraction == 1 || trace.allSolid)
		return;

	Entity->State.SetOrigin (trace.endPos);

	Entity->Link ();
	CheckGround ();
	CatagorizePosition ();
}

void CMonster::AI_Charge(float Dist)
{
#ifndef MONSTER_USE_ROGUE_AI
	IdealYaw = (Entity->Enemy->State.GetOrigin() - Entity->State.GetOrigin()).ToYaw ();
	ChangeYaw ();

	if (Dist)
		WalkMove (Entity->State.GetAngles().Y, Dist);
#else
	// PMM - made AI_MANUAL_STEERING affect things differently here .. they turn, but
	// don't set the ideal_yaw

	// This is put in there so monsters won't move towards the origin after killing
	// a tesla. This could be problematic, so keep an eye on it.
	if(!Entity->Enemy || !Entity->Enemy->IsInUse())		//PGM
		return;									//PGM

	// PMM - save blindfire target
	if (IsVisible(Entity, Entity->Enemy))
		BlindFireTarget = Entity->Enemy->State.GetOrigin();
	// pmm 

	if (!(AIFlags & AI_MANUAL_STEERING))
		IdealYaw = (Entity->Enemy->State.GetOrigin() - Entity->State.GetOrigin()).ToYaw ();
	ChangeYaw ();

	if (Dist)
	{
		if (AIFlags & AI_CHARGING)
		{
			MoveToGoal (Dist);
			return;
		}
		// circle strafe support
		if (AttackState == AS_SLIDING)
		{
			float ofs;
			// if we're fighting a tesla, NEVER circle strafe
			if ((Entity->Enemy) && (Entity->Enemy->gameEntity->classname) && (!strcmp(Entity->Enemy->gameEntity->classname, "tesla")))
				ofs = 0;
			else if (Lefty)
				ofs = 90;
			else
				ofs = -90;
			
			if (WalkMove (IdealYaw + ofs, Dist))
				return;
				
			Lefty = !Lefty;
			WalkMove (IdealYaw - ofs, Dist);
		}
		else
			WalkMove (Entity->State.GetAngles().Y, Dist);
	}
// PMM
#endif
}

bool CMonster::AI_CheckAttack()
{
#ifndef MONSTER_USE_ROGUE_AI
	bool		hesDeadJim = false;

// this causes monsters to run blindly to the combat point w/o firing
	if (Entity->gameEntity->goalentity)
	{
		if (AIFlags & AI_COMBAT_POINT)
			return false;

		if (AIFlags & AI_SOUND_TARGET)
		{
			if (Entity->Enemy)
			{
				if ((level.framenum - Entity->Enemy->teleport_time) > 50)
				{
					if (Entity->gameEntity->goalentity == Entity->Enemy)
					{
						if (Entity->gameEntity->movetarget)
							Entity->gameEntity->goalentity = Entity->gameEntity->movetarget;
						else
							Entity->gameEntity->goalentity = NULL;
					}

					AIFlags &= ~AI_SOUND_TARGET;
					if (AIFlags & AI_TEMP_STAND_GROUND)
						AIFlags &= ~(AI_STAND_GROUND | AI_TEMP_STAND_GROUND);
				}
				else
				{
					Entity->gameEntity->show_hostile = level.framenum + 10;
					return false;
				}
			}
		}
	}

// see if the enemy is dead
	if ((!Entity->Enemy) || (!Entity->Enemy->IsInUse()))
		hesDeadJim = true;
	else if (AIFlags & AI_MEDIC)
	{
		if (entity_cast<CHurtableEntity>(Entity->Enemy)->Health > 0)
		{
			hesDeadJim = true;
			AIFlags &= ~AI_MEDIC;
		}
	}
	else
	{
		if (AIFlags & AI_BRUTAL)
		{
			if (entity_cast<CHurtableEntity>(Entity->Enemy)->Health <= -80)
				hesDeadJim = true;
		}
		else
		{
			if (entity_cast<CHurtableEntity>(Entity->Enemy)->Health <= 0)
				hesDeadJim = true;
		}
	}

	if (hesDeadJim)
	{
		Entity->Enemy = NULL;
	// FIXME: look all around for other targets
		if (Entity->OldEnemy && entity_cast<CHurtableEntity>(Entity->OldEnemy)->Health > 0)
		{
			Entity->Enemy = Entity->OldEnemy;
			Entity->OldEnemy = NULL;
			HuntTarget ();
		}
		else
		{
			if (Entity->gameEntity->movetarget)
			{
				Entity->gameEntity->goalentity = Entity->gameEntity->movetarget;
				Walk ();
			}
			else
			{
				// we need the pausetime otherwise the stand code
				// will just revert to walking with no target and
				// the monsters will wonder around aimlessly trying
				// to hunt the world entity
				PauseTime = level.framenum + 100000000;
				Stand ();
			}
			return true;
		}
	}

	Entity->gameEntity->show_hostile = level.framenum + 10;		// wake up other monsters

// check knowledge of enemy
	EnemyVis = IsVisible(Entity, Entity->Enemy);
	if (EnemyVis)
	{
		SearchTime = level.framenum + 50;
		LastSighting = Entity->Enemy->State.GetOrigin();
	}

	EnemyInfront = IsInFront(Entity, Entity->Enemy);
	EnemyRange = Range(Entity, Entity->Enemy);
	EnemyYaw = (Entity->Enemy->State.GetOrigin() - Entity->State.GetOrigin()).ToYaw();

	// JDC self->ideal_yaw = enemy_yaw;

	if (AttackState == AS_MISSILE)
	{
		AI_Run_Missile ();
		return true;
	}
	else if (AttackState == AS_MELEE)
	{
		AI_Run_Melee ();
		return true;
	}

	// if enemy is not currently visible, we will never attack
	if (!EnemyVis)
		return false;

	return CheckAttack ();
#else
	vec3f		temp;
	bool	hesDeadJim;
	// PMM
	bool	retval;

// this causes monsters to run blindly to the combat point w/o firing
	if (Entity->gameEntity->goalentity)
	{
		if (AIFlags & AI_COMBAT_POINT)
			return false;

		if (AIFlags & AI_SOUND_TARGET)
		{
			if ((level.framenum - Entity->Enemy->gameEntity->teleport_time) > 50)
			{
				if (Entity->gameEntity->goalentity == Entity->Enemy->gameEntity)
				{
					if (Entity->gameEntity->movetarget)
						Entity->gameEntity->goalentity = Entity->gameEntity->movetarget;
					else
						Entity->gameEntity->goalentity = NULL;
				}

				AIFlags &= ~AI_SOUND_TARGET;
				if (AIFlags & AI_TEMP_STAND_GROUND)
					AIFlags &= ~(AI_STAND_GROUND | AI_TEMP_STAND_GROUND);
			}
			else
			{
				Entity->gameEntity->show_hostile = level.framenum + 10;
				return false;
			}
		}
	}

	EnemyVis = false;

// see if the enemy is dead
	hesDeadJim = false;
	if ((!Entity->Enemy) || (!Entity->Enemy->IsInUse()))
	{
		hesDeadJim = true;
	}
	else if (AIFlags & AI_MEDIC)
	{
		if (!(Entity->Enemy->IsInUse()) || (entity_cast<CHurtableEntity>(Entity->Enemy)->Health > 0))
		{
			hesDeadJim = true;
//			self->monsterinfo.aiflags &= ~AI_MEDIC;
		}
	}
	else
	{
		if (AIFlags & AI_BRUTAL)
		{
			if (entity_cast<CHurtableEntity>(Entity->Enemy)->Health <= -80)
				hesDeadJim = true;
		}
		else
		{
			if (entity_cast<CHurtableEntity>(Entity->Enemy)->Health <= 0)
				hesDeadJim = true;
		}
	}

	if (hesDeadJim)
	{
		AIFlags &= ~AI_MEDIC;
		Entity->Enemy = NULL;
	// FIXME: look all around for other targets
		if (Entity->OldEnemy && entity_cast<CHurtableEntity>(Entity->OldEnemy)->Health > 0)
		{
			Entity->Enemy = Entity->OldEnemy;
			Entity->OldEnemy = NULL;
			HuntTarget ();
		}
//ROGUE - multiple teslas make monsters lose track of the player.
		else if(LastPlayerEnemy && LastPlayerEnemy->Health > 0)
		{
//			if ((g_showlogic) && (g_showlogic->value))
//				gi.dprintf("resorting to last_player_enemy...\n");
			Entity->Enemy = LastPlayerEnemy;
			Entity->OldEnemy = NULL;
			LastPlayerEnemy = NULL;
			HuntTarget ();
		}
//ROGUE
		else
		{
			if (Entity->gameEntity->movetarget)
			{
				Entity->gameEntity->goalentity = Entity->gameEntity->movetarget;
				Walk ();
			}
			else
			{
				// we need the pausetime otherwise the stand code
				// will just revert to walking with no target and
				// the monsters will wonder around aimlessly trying
				// to hunt the world entity
				PauseTime = level.framenum + 100000000;
				Stand ();
			}
			return true;
		}
	}

	Entity->gameEntity->show_hostile = level.framenum + 10;		// wake up other monsters

// check knowledge of enemy
	EnemyVis = IsVisible(Entity, Entity->Enemy);
	if (EnemyVis)
	{
		SearchTime = level.framenum + 50;
		LastSighting = Entity->Enemy->State.GetOrigin();
		// PMM
		AIFlags &= ~AI_LOST_SIGHT;
		TrailTime = level.framenum;
		BlindFireTarget = Entity->Enemy->State.GetOrigin();
		BlindFireDelay = 0;
		// pmm
	}

	EnemyInfront = IsInFront(Entity, Entity->Enemy);
	EnemyRange = Range(Entity, Entity->Enemy);

	temp = Entity->Enemy->State.GetOrigin() - Entity->State.GetOrigin();
	EnemyYaw = temp.ToYaw ();

	// JDC self->ideal_yaw = enemy_yaw;

	// PMM -- reordered so the monster specific checkattack is called before the run_missle/melee/checkvis
	// stuff .. this allows for, among other things, circle strafing and attacking while in ai_run
	retval = CheckAttack ();
	if (retval)
	{
		// PMM
		if (AttackState == AS_MISSILE)
		{
			AI_Run_Missile ();
			return true;
		}
		if (AttackState == AS_MELEE)
		{
			AI_Run_Melee ();
			return true;
		}
		// PMM -- added so monsters can shoot blind
		if (AttackState == AS_BLIND)
		{
			AI_Run_Missile ();
			return true;
		}
		// pmm

		// if enemy is not currently visible, we will never attack
		if (!EnemyVis)
			return false;
		// PMM
	}
	return retval;
#endif
}

void CMonster::AI_Move (float Dist)
{
	WalkMove (Entity->State.GetAngles().Y, Dist);
}

class CTempGoal : public virtual CBaseEntity
{
public:
	CTempGoal () :
	  CBaseEntity ()
	{
	};

	CTempGoal (int Index) :
	  CBaseEntity (Index)
	{
	};
};

void CMonster::AI_Run(float Dist)
{
#ifndef MONSTER_USE_ROGUE_AI
	edict_t		*tempgoal, *save, *marker;
	bool		isNew;
	float		d1, d2, left, center, right;
	CTrace		tr;
	vec3f		v_forward, v_right, left_target, right_target, v;

#ifdef MONSTERS_USE_PATHFINDING
	if (FollowingPath)
	{
		MoveToPath(Dist);
		return;
	}
#endif

	// if we're going to a combat point, just proceed
	if (AIFlags & AI_COMBAT_POINT)
	{
		MoveToGoal (Dist);
		return;
	}

	vec3f origin = Entity->State.GetOrigin();
	if (AIFlags & AI_SOUND_TARGET)
	{
		if (Entity->Enemy)
		{
			v = origin - Entity->Enemy->State.GetOrigin();
			if (v.Length() < 64)
			{
				AIFlags |= (AI_STAND_GROUND | AI_TEMP_STAND_GROUND);
				Stand ();
				return;
			}

			MoveToGoal (Dist);
		}

		if (!FindTarget ())
			return;
	}

	if (AI_CheckAttack ())
		return;

	if (AttackState == AS_SLIDING)
	{
		AI_Run_Slide (Dist);
		return;
	}

	if (EnemyVis)
	{
		//if (AIFlags & AI_LOST_SIGHT)
			//gi.dprintf("regained sight\n");
		MoveToGoal (Dist);
		AIFlags &= ~AI_LOST_SIGHT;
		LastSighting = Entity->Enemy->State.GetOrigin();
		TrailTime = level.framenum;
		return;
	}

	// coop will change to another enemy if visible
	if (game.mode == GAME_COOPERATIVE)
	{	// FIXME: insane guys get mad with this, which causes crashes!
		if (FindTarget ())
			return;
	}

	//if (SearchTime && (level.framenum > (SearchTime + 200)))
	//{
	//	MoveToGoal (Dist);
	//	SearchTime = 0;
//		dprint("search timeout\n");
	//	return;
	//}

	save = Entity->gameEntity->goalentity;
	tempgoal = G_Spawn();
	Entity->gameEntity->goalentity = tempgoal;

	isNew = false;

	if (!(AIFlags & AI_LOST_SIGHT))
	{
#ifdef MONSTERS_USE_PATHFINDING
		P_NodePathTimeout = level.framenum + 100; // Do "blind fire" first.
#endif

		// just lost sight of the player, decide where to go first
		AIFlags |= (AI_LOST_SIGHT | AI_PURSUIT_LAST_SEEN);
		AIFlags &= ~(AI_PURSUE_NEXT | AI_PURSUE_TEMP);
		isNew = true;
	}
#ifdef MONSTERS_USE_PATHFINDING
	else if ((AIFlags & AI_LOST_SIGHT) && P_NodePathTimeout < level.framenum)
	{
		// Set us up for pathing
		P_CurrentNode = GetClosestNodeTo(origin);
		P_CurrentGoalNode = GetClosestNodeTo(Entity->Enemy->state.origin);
		FoundPath ();
	}
#endif

	if (AIFlags & AI_PURSUE_NEXT)
	{
		AIFlags &= ~AI_PURSUE_NEXT;
//		dprint("reached current goal: "); dprint(vtos(self.origin)); dprint(" "); dprint(vtos(self.last_sighting)); dprint(" "); dprint(ftos(vlen(self.origin - self.last_sighting))); dprint("\n");

		// give ourself more time since we got this far
		SearchTime = level.framenum + 50;

		if (AIFlags & AI_PURSUE_TEMP)
		{
//			dprint("was temp goal; retrying original\n");
			AIFlags &= ~AI_PURSUE_TEMP;
			marker = NULL;
			LastSighting = SavedGoal;
			isNew = true;
		}
	}

	v = origin - LastSighting;
	d1 = v.Length();
	if (d1 <= Dist)
	{
		AIFlags |= AI_PURSUE_NEXT;
		Dist = d1;
	}

	Entity->gameEntity->goalentity->Entity->State.SetOrigin (LastSighting);

	if (isNew)
	{
//		gi.dprintf("checking for course correction\n");

		tr = CTrace (origin, Entity->GetMins(), Entity->GetMaxs(), LastSighting, Entity->gameEntity, CONTENTS_MASK_PLAYERSOLID);
		if (tr.fraction < 1)
		{
			v = vec3f(Entity->gameEntity->goalentity->state.origin) - origin;
			d1 = v.Length();
			center = tr.fraction;
			d2 = d1 * ((center+1)/2);
			vec3f ang = Entity->State.GetAngles();
			ang.Y = IdealYaw = v.ToYaw();
			Entity->State.SetAngles(ang);

			ang.ToVectors (&v_forward, &v_right, NULL);

			v = vec3f(d2, -16, 0);
			G_ProjectSource (origin, v, v_forward, v_right, left_target);
			tr = CTrace(origin, Entity->GetMins(), Entity->GetMaxs(), left_target, Entity->gameEntity, CONTENTS_MASK_PLAYERSOLID);
			left = tr.fraction;

			v = vec3f(d2, 16, 0);
			G_ProjectSource (origin, v, v_forward, v_right, right_target);
			tr = CTrace(origin, Entity->GetMins(), Entity->GetMaxs(), right_target, Entity->gameEntity, CONTENTS_MASK_PLAYERSOLID);
			right = tr.fraction;

			center = (d1*center)/d2;
			if (left >= center && left > right)
			{
				if (left < 1)
				{
					v = vec3f(d2 * left * 0.5, -16, 0);
					G_ProjectSource (origin, v, v_forward, v_right, left_target);
//					gi.dprintf("incomplete path, go part way and adjust again\n");
				}
				SavedGoal = LastSighting;
				AIFlags |= AI_PURSUE_TEMP;
				Vec3Copy (left_target, Entity->gameEntity->goalentity->state.origin);
				LastSighting = left_target;
				v = vec3f(Entity->gameEntity->goalentity->state.origin) - origin;

				vec3f ang = Entity->State.GetAngles();
				ang.Y = IdealYaw = v.ToYaw();
				Entity->State.SetAngles(ang);
//				gi.dprintf("adjusted left\n");
//				debug_drawline(self.origin, self.last_sighting, 152);
			}
			else if (right >= center && right > left)
			{
				if (right < 1)
				{
					v = vec3f(d2 * right * 0.5, -16, 0);
					G_ProjectSource (origin, v, v_forward, v_right, right_target);
//					gi.dprintf("incomplete path, go part way and adjust again\n");
				}
				SavedGoal = LastSighting;
				AIFlags |= AI_PURSUE_TEMP;
				Vec3Copy (right_target, Entity->gameEntity->goalentity->state.origin);
				LastSighting = right_target;
				v = vec3f(Entity->gameEntity->goalentity->state.origin) - origin;
				vec3f ang = Entity->State.GetAngles();
				ang.Y = IdealYaw = v.ToYaw();
				Entity->State.SetAngles(ang);
//				gi.dprintf("adjusted right\n");
//				debug_drawline(self.origin, self.last_sighting, 152);
			}
		}
//		else gi.dprintf("course was fine\n");
	}

	MoveToGoal (Dist);

	G_FreeEdict(tempgoal);

	if (Entity)
		Entity->gameEntity->goalentity = save;
#else
	CTempGoal		*tempgoal;
	edict_t		*save;
	bool	isNew;
	//PMM
	bool	retval;
	bool	alreadyMoved = false;

 #ifdef MONSTERS_USE_PATHFINDING
	if (FollowingPath)
	{
		MoveToPath(Dist);
		return;
	}
#endif

	// if we're going to a combat point, just proceed
	if (AIFlags & AI_COMBAT_POINT)
	{
		MoveToGoal (Dist);
		return;
	}

	// PMM
	if (AIFlags & AI_DUCKED)
	{
//		if ((g_showlogic) && (g_showlogic->value))
//			gi.dprintf ("%s - duck flag cleaned up!\n", self->classname);
		AIFlags &= ~AI_DUCKED;
	}
	if (Entity->GetMaxs().Z != BaseHeight)
	{
//		if ((g_showlogic) && (g_showlogic->value))
//			gi.dprintf ("%s - ducked height corrected!\n", self->classname);
		UnDuck ();
	}
//	if ((self->monsterinfo.aiflags & AI_MANUAL_STEERING) && (strcmp(self->classname, "monster_turret")))
//	{
//		if ((g_showlogic) && (g_showlogic->value))
//			gi.dprintf ("%s - manual steering in ai_run!\n", self->classname);
//	}
	// pmm

	if (AIFlags & AI_SOUND_TARGET)
	{
		// PMM - paranoia checking
		vec3f v (0, 0, 0);

		if (Entity->Enemy)
			v = Entity->State.GetOrigin() - Entity->Enemy->State.GetOrigin();

		if ((!Entity->Enemy) || (v.Length() < 64))
		// pmm
		{
			AIFlags |= (AI_STAND_GROUND | AI_TEMP_STAND_GROUND);
			Stand ();
			return;
		}

		MoveToGoal (Dist);
		// PMM - prevent double moves for sound_targets
		alreadyMoved = true;
		// pmm
		if(!Entity->IsInUse())
			return;			// PGM - g_touchtrigger free problem

		if (!FindTarget ())
			return;
	}

	// PMM -- moved ai_checkattack up here so the monsters can attack while strafing or charging
	// PMM -- if we're dodging, make sure to keep the attack_state AS_SLIDING
	retval = AI_CheckAttack ();

	// PMM - don't strafe if we can't see our enemy
	if ((!EnemyVis) && (AttackState == AS_SLIDING))
		AttackState = AS_STRAIGHT;

	// unless we're dodging (dodging out of view looks smart)
	if (AIFlags & AI_DODGING)
		AttackState = AS_SLIDING;
	// pmm

	if (AttackState == AS_SLIDING)
	{
		// PMM - protect against double moves
		if (!alreadyMoved)
			AI_Run_Slide (Dist);
		// PMM
		// we're using attack_state as the return value out of ai_run_slide to indicate whether or not the
		// move succeeded.  If the move succeeded, and we're still sliding, we're done in here (since we've
		// had our chance to shoot in ai_checkattack, and have moved).
		// if the move failed, our state is as_straight, and it will be taken care of below
		if ((!retval) && (AttackState == AS_SLIDING))
			return;
	}
	else if (AIFlags & AI_CHARGING)
	{
		IdealYaw = EnemyYaw;
		if (!(AIFlags & AI_MANUAL_STEERING))
			ChangeYaw ();
	}
	if (retval)
	{
		// PMM - is this useful?  Monsters attacking usually call the ai_charge routine..
		// the only monster this affects should be the soldier
		if ((Dist != 0) && (!alreadyMoved) && (AttackState == AS_STRAIGHT) && (!(AIFlags & AI_STAND_GROUND)))
			MoveToGoal (Dist);

		if ((Entity->Enemy) && (Entity->Enemy->IsInUse()) && (EnemyVis))
		{
			AIFlags &= ~AI_LOST_SIGHT;
			LastSighting = Entity->Enemy->State.GetOrigin();
			TrailTime = level.framenum;
			//PMM
			BlindFireTarget = Entity->Enemy->State.GetOrigin();
			BlindFireDelay = 0;
			//pmm
		}
		return;
	}
	//PMM

	// PGM - added a little paranoia checking here... 9/22/98
	if ((Entity->Enemy) && (Entity->Enemy->IsInUse()) && (EnemyVis))
	{
		// PMM - check for alreadyMoved
		if (!alreadyMoved)
			MoveToGoal (Dist);
		if(!Entity->IsInUse())
			return;			// PGM - g_touchtrigger free problem

		AIFlags &= ~AI_LOST_SIGHT;
		LastSighting = Entity->Enemy->State.GetOrigin();
		TrailTime = level.framenum;
		// PMM
		BlindFireTarget = Entity->Enemy->State.GetOrigin();
		BlindFireDelay = 0;
		// pmm
		return;
	}

// PMM - moved down here to allow monsters to get on hint paths
	// coop will change to another enemy if visible
	if (game.mode == GAME_COOPERATIVE)
	{	// FIXME: insane guys get mad with this, which causes crashes!
		if (FindTarget ())
			return;
	}
// pmm

	if ((SearchTime) && (level.framenum > (SearchTime + 200)))
	{
		// PMM - double move protection
		if (!alreadyMoved)
			MoveToGoal (Dist);
		SearchTime = 0;
		return;
	}

	save = Entity->gameEntity->goalentity;
	tempgoal = QNew (com_levelPool, 0) CTempGoal;
	Entity->gameEntity->goalentity = tempgoal->gameEntity;

	isNew = false;

	if (!(AIFlags & AI_LOST_SIGHT))
	{
#ifdef MONSTERS_USE_PATHFINDING
		P_NodePathTimeout = level.framenum + 100; // Do "blind fire" first.
#endif

		// just lost sight of the player, decide where to go first
		AIFlags |= (AI_LOST_SIGHT | AI_PURSUIT_LAST_SEEN);
		AIFlags &= ~(AI_PURSUE_NEXT | AI_PURSUE_TEMP);
		isNew = true;
	}
#ifdef MONSTERS_USE_PATHFINDING
	else if ((AIFlags & AI_LOST_SIGHT) && P_NodePathTimeout < level.framenum)
	{
		// Set us up for pathing
		P_CurrentNode = GetClosestNodeTo(Entity->State.GetOrigin());
		P_CurrentGoalNode = GetClosestNodeTo(Entity->Enemy->State.GetOrigin());
		FoundPath ();
	}
#endif

	if (AIFlags & AI_PURSUE_NEXT)
	{
		AIFlags &= ~AI_PURSUE_NEXT;

		// give ourself more time since we got this far
		SearchTime = level.framenum + 50;

		if (AIFlags & AI_PURSUE_TEMP)
		{
			AIFlags &= ~AI_PURSUE_TEMP;
			LastSighting = SavedGoal;
			isNew = true;
		}
	}

	vec3f v = Entity->State.GetOrigin() - LastSighting;
	float d1 = v.Length();
	if (d1 <= Dist)
	{
		AIFlags |= AI_PURSUE_NEXT;
		Dist = d1;
	}

	Entity->gameEntity->goalentity->Entity->State.SetOrigin (LastSighting);

	if (isNew)
	{
		CTrace tr (Entity->State.GetOrigin(), Entity->GetMins(), Entity->GetMaxs(), LastSighting, Entity->gameEntity, CONTENTS_MASK_PLAYERSOLID);
		if (tr.fraction < 1)
		{
			float center = tr.fraction;
			v = Entity->gameEntity->goalentity->Entity->State.GetOrigin() - Entity->State.GetOrigin();
			d1 = v.Length();

			float d2 = d1 * ((center+1)/2);
			vec3f angles = Entity->State.GetAngles();
			vec3f origin = Entity->State.GetOrigin();

			angles.Y = IdealYaw = v.ToYaw();
			Entity->State.SetAngles(angles);

			vec3f v_forward, v_right;
			angles.ToVectors (&v_forward, &v_right, NULL);

			vec3f left_target;
			vec3f offset (d2, -16, 0);
			G_ProjectSource (origin, offset, v_forward, v_right, left_target);
			tr = CTrace(origin, Entity->GetMins(), Entity->GetMaxs(), left_target, Entity->gameEntity, CONTENTS_MASK_PLAYERSOLID);
			float left = tr.fraction;

			vec3f right_target;
			offset = vec3f(d2, 16, 0);
			G_ProjectSource (origin, offset, v_forward, v_right, right_target);
			tr = CTrace(origin, Entity->GetMins(), Entity->GetMaxs(), right_target, Entity->gameEntity, CONTENTS_MASK_PLAYERSOLID);
			float right = tr.fraction;

			center = (d1*center)/d2;
			if (left >= center && left > right)
			{
				if (left < 1)
				{
					offset = vec3f(d2 * left * 0.5, -16, 0);
					G_ProjectSource (origin, offset, v_forward, v_right, left_target);
				}
				SavedGoal = LastSighting;
				AIFlags |= AI_PURSUE_TEMP;
				Vec3Copy (left_target, Entity->gameEntity->goalentity->state.origin);
				LastSighting = left_target;
				v = Entity->gameEntity->goalentity->Entity->State.GetOrigin() - origin;

				angles[YAW] = IdealYaw = v.ToYaw();
				Entity->State.SetAngles(angles);
			}
			else if (right >= center && right > left)
			{
				if (right < 1)
				{
					offset = vec3f(d2 * right * 0.5, 16, 0);
					G_ProjectSource (origin, offset, v_forward, v_right, right_target);
				}
				SavedGoal = LastSighting;
				AIFlags |= AI_PURSUE_TEMP;
				Vec3Copy (right_target, Entity->gameEntity->goalentity->state.origin);
				LastSighting = right_target;
				v = Entity->gameEntity->goalentity->Entity->State.GetOrigin() - origin;
				angles[YAW] = IdealYaw = v.ToYaw();
				Entity->State.SetAngles(angles);
			}
		}
	}

	MoveToGoal (Dist);

	tempgoal->Free ();

	if(!Entity->IsInUse())
		return;			// PGM - g_touchtrigger free problem

	if (Entity)
		Entity->gameEntity->goalentity = save;
#endif
}

void CMonster::AI_Run_Melee ()
{
#ifndef MONSTER_USE_ROGUE_AI
	IdealYaw = EnemyYaw;
	ChangeYaw ();

	if (FacingIdeal())
	{
		Melee ();
		AttackState = AS_STRAIGHT;
	}
#else
	IdealYaw = EnemyYaw;
	if (!(AIFlags & AI_MANUAL_STEERING))
		ChangeYaw ();

	if (FacingIdeal())
	{
		Melee ();
		AttackState = AS_STRAIGHT;
	}
#endif
}

void CMonster::AI_Run_Missile()
{
#ifndef MONSTER_USE_ROGUE_AI
	IdealYaw = EnemyYaw;
	ChangeYaw ();

	if (FacingIdeal())
	{
		Attack ();
		AttackState = AS_STRAIGHT;
	}
#else
	IdealYaw = EnemyYaw;
	if (!(AIFlags & AI_MANUAL_STEERING))
		ChangeYaw ();

	if (FacingIdeal())
	{
		Attack ();
		if ((AttackState == AS_MISSILE) || (AttackState == AS_BLIND))
			AttackState = AS_STRAIGHT;
	}
#endif
}

void CMonster::AI_Run_Slide(float Dist)
{
#ifndef MONSTER_USE_ROGUE_AI
	IdealYaw = EnemyYaw;
	ChangeYaw ();

	if (WalkMove (IdealYaw + ((Lefty) ? 90 : -90), Dist))
		return;
		
	Lefty = !Lefty;
	WalkMove (IdealYaw - ((Lefty) ? 90 : -90), Dist);
#else
	float	ofs;
	float	angle;

	IdealYaw = EnemyYaw;
	
	angle = 90;
	
	if (Lefty)
		ofs = angle;
	else
		ofs = -angle;

	if (!(AIFlags & AI_MANUAL_STEERING))
		ChangeYaw ();

	// PMM - clamp maximum sideways move for non flyers to make them look less jerky
	if (!(Entity->Flags & FL_FLY))
		Dist = Min<> (Dist, 8.0f);
	if (WalkMove (IdealYaw + ofs, Dist))
		return;
	// PMM - if we're dodging, give up on it and go straight
	if (AIFlags & AI_DODGING)
	{
		DoneDodge ();
		// by setting as_straight, caller will know to try straight move
		AttackState = AS_STRAIGHT;
		return;
	}

	Lefty = !Lefty;
	if (WalkMove (IdealYaw - ofs, Dist))
		return;
	// PMM - if we're dodging, give up on it and go straight
	if (AIFlags & AI_DODGING)
		DoneDodge ();

	// PMM - the move failed, so signal the caller (ai_run) to try going straight
	AttackState = AS_STRAIGHT;
#endif
}

void CMonster::AI_Stand (float Dist)
{
#ifndef MONSTER_USE_ROGUE_AI
	if (Dist)
		WalkMove (Entity->State.GetAngles().Y, Dist);

#ifdef MONSTERS_USE_PATHFINDING
	if (FollowingPath)
	{
		// Assuming we got here because we're waiting for something.
		if (P_CurrentNode->Type == NODE_DOOR || P_CurrentNode->Type == NODE_PLATFORM)
		{
			CBrushModel *Door = entity_cast<CBrushModel>(P_CurrentNode->LinkedEntity);
			if (Door->MoveState == STATE_TOP)
				Run(); // We can go again!
		}
		// In two goals, do we reach the platform node?
		else if (P_CurrentPath->Path[P_CurrentNodeIndex-1]->Type == NODE_PLATFORM)
		{
			CPlatForm *Plat = entity_cast<CPlatForm>(P_CurrentPath->Path[P_CurrentNodeIndex-1]->LinkedEntity); // get the plat
			// Is it at bottom?
			if (Plat->MoveState == STATE_BOTTOM)
				Run (); // Go!
		}
		else
		{
			// ...this shouldn't happen. FIND OUT WHY PLZ
			FollowingPath = false;
		}
		return;
	}
#endif

	if (AIFlags & AI_STAND_GROUND)
	{
		if (Entity->Enemy)
		{
			IdealYaw = (Entity->Enemy->State.GetOrigin() - Entity->State.GetOrigin()).ToYaw();
			if (Entity->State.GetAngles().Y != IdealYaw && AIFlags & AI_TEMP_STAND_GROUND)
			{
				AIFlags &= ~(AI_STAND_GROUND | AI_TEMP_STAND_GROUND);
				Run ();
			}
			ChangeYaw ();
			AI_CheckAttack();
		}
		else
			FindTarget ();
		return;
	}

	if (FindTarget ())
		return;
	
	if (level.framenum > PauseTime)
	{
		Walk ();
		return;
	}

	if (!(Entity->SpawnFlags & 1) && (MonsterFlags & MF_HAS_IDLE) && (level.framenum > IdleTime))
	{
		if (IdleTime)
		{
			Idle ();
			IdleTime = level.framenum + 150 + (random() * 150);
		}
		else
			IdleTime = level.framenum + (random() * 150);
	}
#else
	if (Dist)
		WalkMove (Entity->State.GetAngles().Y, Dist);

#ifdef MONSTERS_USE_PATHFINDING
	if (FollowingPath)
	{
		// Assuming we got here because we're waiting for something.
		if (P_CurrentNode->Type == NODE_DOOR || P_CurrentNode->Type == NODE_PLATFORM)
		{
			CBrushModel *Door = entity_cast<CBrushModel>(P_CurrentNode->LinkedEntity);
			if (Door->MoveState == STATE_TOP)
				Run(); // We can go again!
		}
		// In two goals, do we reach the platform node?
		else if (P_CurrentPath->Path[P_CurrentNodeIndex-1]->Type == NODE_PLATFORM)
		{
			CPlatForm *Plat = entity_cast<CPlatForm>(P_CurrentPath->Path[P_CurrentNodeIndex-1]->LinkedEntity); // get the plat
			// Is it at bottom?
			if (Plat->MoveState == STATE_BOTTOM)
				Run (); // Go!
		}
		else
		{
			// ...this shouldn't happen. FIND OUT WHY PLZ
			FollowingPath = false;
		}
		return;
	}
#endif

	if (AIFlags & AI_STAND_GROUND)
	{
		if (Entity->Enemy)
		{
			vec3f v = (Entity->Enemy->State.GetOrigin() - Entity->State.GetOrigin());
			IdealYaw = v.ToYaw();

			if (Entity->State.GetAngles().Y != IdealYaw && (AIFlags & AI_TEMP_STAND_GROUND))
			{
				AIFlags &= ~(AI_STAND_GROUND | AI_TEMP_STAND_GROUND);
				Run ();
			}
			if (!(AIFlags & AI_MANUAL_STEERING))
				ChangeYaw ();
			// PMM
			// find out if we're going to be shooting
			bool retval = AI_CheckAttack ();
			// record sightings of player
			if ((Entity->Enemy) && (Entity->Enemy->IsInUse()) && (IsVisible(Entity, Entity->Enemy)))
			{
				AIFlags &= ~AI_LOST_SIGHT;
				LastSighting = Entity->Enemy->State.GetOrigin();
				BlindFireTarget = Entity->Enemy->State.GetOrigin();
				TrailTime = level.framenum;
				BlindFireDelay = 0;
			}
			// check retval to make sure we're not blindfiring
			else if (!retval)
			{
				FindTarget ();
				return;
			}
//			CheckAttack ();
			// pmm
		}
		else
			FindTarget ();
		return;
	}

	if (FindTarget ())
		return;
	
	if (level.framenum > PauseTime)
	{
		Walk ();
		return;
	}

	if (!(Entity->SpawnFlags & 1) && (MonsterFlags & MF_HAS_IDLE) && (level.framenum > IdleTime))
	{
		if (IdleTime)
		{
			Idle ();
			IdleTime = level.framenum + 150 + random() * 150;
		}
		else
		{
			IdleTime = level.framenum + random() * 150;
		}
	}
#endif
}

void CMonster::ReactToDamage (CBaseEntity *attacker)
{
	if (!(attacker->EntityFlags & ENT_PLAYER) && !(attacker->EntityFlags & ENT_MONSTER))
		return;

	if (attacker == Entity || (Entity->Enemy && (attacker == Entity->Enemy)))
		return;

	// if we are a good guy monster and our attacker is a player
	// or another good guy, do not get mad at them
	if (AIFlags & AI_GOOD_GUY)
	{
		if ((attacker->EntityFlags & ENT_PLAYER) || ((attacker->EntityFlags & ENT_MONSTER) && (entity_cast<CMonsterEntity>(attacker))->Monster->AIFlags & AI_GOOD_GUY))
			return;
	}

	// we now know that we are not both good guys

	// if attacker is a client, get mad at them because he's good and we're not
	if (attacker->EntityFlags & ENT_PLAYER)
	{
		AIFlags &= ~AI_SOUND_TARGET;

		// this can only happen in coop (both new and old enemies are clients)
		// only switch if can't see the current enemy
		if (Entity->Enemy && (Entity->Enemy->EntityFlags & ENT_PLAYER))
		{
			if (IsVisible(Entity, Entity->Enemy))
			{
				Entity->OldEnemy = attacker;
				return;
			}
			Entity->OldEnemy = Entity->Enemy;
		}
		Entity->Enemy = attacker;
		if (!(AIFlags & AI_DUCKED))
			FoundTarget ();
		return;
	}

#ifdef MONSTERS_ARENT_STUPID
	// Help our buddy!
	if ((attacker->EntityFlags & ENT_MONSTER) && attacker->Enemy && attacker->Enemy != Entity)
	{
		if (Entity->Enemy && (Entity->Enemy->EntityFlags & ENT_PLAYER))
			Entity->OldEnemy = Entity->Enemy;
		Entity->Enemy = attacker->Enemy;
		if (!(AIFlags & AI_DUCKED))
			FoundTarget ();
	}
	return;
#else
	// it's the same base (walk/swim/fly) type and a different classname and it's not a tank
	// (they spray too much), get mad at them
	if (((Entity->Flags & (FL_FLY|FL_SWIM)) == (attacker->Flags & (FL_FLY|FL_SWIM))) &&
		 (strcmp (Entity->gameEntity->classname, attacker->gameEntity->classname) != 0) &&
		 (strcmp(attacker->gameEntity->classname, "monster_tank") != 0) &&
		 (strcmp(attacker->gameEntity->classname, "monster_supertank") != 0) &&
		 (strcmp(attacker->gameEntity->classname, "monster_makron") != 0) &&
		 (strcmp(attacker->gameEntity->classname, "monster_jorg") != 0))
	{
		if (Entity->Enemy && (Entity->Enemy->EntityFlags & ENT_PLAYER))
			Entity->OldEnemy = Entity->Enemy;
		Entity->Enemy = attacker->gameEntity;
		if (!(AIFlags & AI_DUCKED))
			FoundTarget ();
	}
	// if they *meant* to shoot us, then shoot back
	else if (attacker->Enemy == Entity->gameEntity)
	{
		if (Entity->Enemy && (Entity->Enemy->EntityFlags & ENT_PLAYER))
			Entity->OldEnemy = Entity->Enemy;
		Entity->Enemy = attacker->gameEntity;
		if (!(AIFlags & AI_DUCKED))
			FoundTarget ();
	}
	// Help our buddy!
	else if ((attacker->EntityFlags & ENT_MONSTER) && attacker->Enemy && attacker->Enemy != Entity->gameEntity)
	{
		if (Entity->Enemy && (Entity->Enemy->EntityFlags & ENT_PLAYER))
			Entity->OldEnemy = Entity->Enemy;
		Entity->Enemy = attacker->Enemy;
		if (!(AIFlags & AI_DUCKED))
			FoundTarget ();
	}
#endif
}

void CMonster::AI_Walk(float Dist)
{
	MoveToGoal (Dist);

	// check for noticing a player
	if (FindTarget ())
		return;

	if ((MonsterFlags & MF_HAS_SEARCH) && (level.framenum > IdleTime))
	{
		if (IdleTime)
		{
			Search ();
			IdleTime = level.framenum + 150 + (random() * 150);
		}
		else
			IdleTime = level.framenum + (random() * 150);
	}
}

// These are intended to be virtually replaced.
void CMonster::Stand ()
{
}

void CMonster::Idle ()
{
	if (MonsterFlags & MF_HAS_IDLE)
		DebugPrintf ("Warning: Monster with no idle has MF_HAS_IDLE!\n");
}

void CMonster::Search ()
{
	if (MonsterFlags & MF_HAS_SEARCH)
		DebugPrintf ("Warning: Monster with no search has MF_HAS_SEARCH!\n");
}

void CMonster::Walk ()
{
}

void CMonster::Run ()
{
}

#ifndef MONSTER_USE_ROGUE_AI
void CMonster::Dodge (CBaseEntity *other, float eta)
{
}
#endif

void CMonster::Attack()
{
	if (MonsterFlags & MF_HAS_ATTACK)
		DebugPrintf ("Warning: Monster with no attack has MF_HAS_ATTACK!\n");
}

void CMonster::Melee ()
{
	if (MonsterFlags & MF_HAS_MELEE)
		DebugPrintf ("Warning: Monster with no melee has MF_HAS_MELEE!\n");
}

void CMonster::Sight ()
{
	if (MonsterFlags & MF_HAS_SIGHT)
		DebugPrintf ("Warning: Monster with no sight has MF_HAS_SIGHT!\n");
}

void CMonster::MonsterDeathUse ()
{
	Entity->Flags &= ~(FL_FLY|FL_SWIM);
	AIFlags &= AI_GOOD_GUY;

	if (Entity->gameEntity->item)
	{
		Entity->gameEntity->item->DropItem (Entity);
		Entity->gameEntity->item = NULL;
	}

	if (Entity->gameEntity->deathtarget)
		Entity->gameEntity->target = Entity->gameEntity->deathtarget;

	if (!Entity->gameEntity->target)
		return;

	Entity->UseTargets (Entity->Enemy, Entity->Message);
}

void CMonster::MonsterThink ()
{
	Entity->NextThink = level.framenum + FRAMETIME;
	MoveFrame ();
	if (Entity->GetLinkCount() != LinkCount)
	{
		LinkCount = Entity->GetLinkCount();
		CheckGround ();
	}
	CatagorizePosition ();
	WorldEffects ();
	SetEffects ();
}

void CMonster::MoveFrame ()
{
	int		index;
	CAnim	*Move = CurrentMove;

	// Backwards animation support
	if (Move->FirstFrame > Move->LastFrame)
	{
		if ((NextFrame) && (NextFrame >= Move->LastFrame) && (NextFrame <= Move->FirstFrame))
		{
			Entity->State.SetFrame(NextFrame);
			NextFrame = 0;
		}
		else
		{
			if (Entity->State.GetFrame() == Move->LastFrame)
			{
				if (Move->EndFunc)
				{
					void (CMonster::*EndFunc) () = Move->EndFunc;
					(this->*EndFunc) ();

					// regrab move, endfunc is very likely to change it
					Move = CurrentMove;

					// check for death
					if (Entity->GetSvFlags() & SVF_DEADMONSTER)
						return;
				}
			}

			if (Entity->State.GetFrame() < Move->LastFrame || Entity->State.GetFrame() > Move->FirstFrame)
			{
				AIFlags &= ~AI_HOLD_FRAME;
				Entity->State.SetFrame (Move->FirstFrame);
			}
			else
			{
				if (!(AIFlags & AI_HOLD_FRAME))
				{
					Entity->State.SetFrame(Entity->State.GetFrame() - 1);
					if (Entity->State.GetFrame() < Move->LastFrame)
						Entity->State.SetFrame(Move->FirstFrame);
				}
			}
		}

		index = Move->FirstFrame - Entity->State.GetFrame();

		void (CMonster::*AIFunc) (float Dist) = Move->Frames[index].AIFunc;
		if (AIFunc)
			(this->*AIFunc) ((AIFlags & AI_HOLD_FRAME) ? 0 : (Move->Frames[index].Dist * Scale));

		void (CMonster::*Function) () = Move->Frames[index].Function;
		if (Function)
			(this->*Function) ();
	}
	else
	{
		if ((NextFrame) && (NextFrame >= Move->FirstFrame) && (NextFrame <= Move->LastFrame))
		{
			Entity->State.SetFrame (NextFrame);
			NextFrame = 0;
		}
		else
		{
			if (Entity->State.GetFrame() == Move->LastFrame)
			{
				if (Move->EndFunc)
				{
					void (CMonster::*EndFunc) () = Move->EndFunc;
					(this->*EndFunc) ();

					// regrab move, endfunc is very likely to change it
					Move = CurrentMove;

					// check for death
					if (Entity->GetSvFlags() & SVF_DEADMONSTER)
						return;
				}
			}

			if (Entity->State.GetFrame() < Move->FirstFrame || Entity->State.GetFrame() > Move->LastFrame)
			{
				AIFlags &= ~AI_HOLD_FRAME;
				Entity->State.SetFrame (Move->FirstFrame);
			}
			else
			{
				if (!(AIFlags & AI_HOLD_FRAME))
				{
					Entity->State.SetFrame(Entity->State.GetFrame()+1);
					if (Entity->State.GetFrame() > Move->LastFrame)
						Entity->State.SetFrame(Move->FirstFrame);
				}
			}
		}

		index = Entity->State.GetFrame() - Move->FirstFrame;

		void (CMonster::*AIFunc) (float Dist) = Move->Frames[index].AIFunc;
		if (AIFunc)
			(this->*AIFunc) ((AIFlags & AI_HOLD_FRAME) ? 0 : (Move->Frames[index].Dist * Scale));

		void (CMonster::*Function) () = Move->Frames[index].Function;
		if (Function)
			(this->*Function) ();
	}
}

void CMonster::FoundTarget ()
{
	Entity->gameEntity->show_hostile = level.framenum + 10;		// wake up other monsters

	LastSighting = Entity->Enemy->State.GetOrigin();
	TrailTime = level.framenum;

	if (!Entity->gameEntity->combattarget)
	{
		HuntTarget ();
		return;
	}

	CBaseEntity *Target = CC_PickTarget(Entity->gameEntity->combattarget);
	Entity->gameEntity->goalentity = Entity->gameEntity->movetarget = Target->gameEntity;
	if (!Entity->gameEntity->movetarget)
	{
		if (Entity->Enemy)
			Entity->gameEntity->goalentity = Entity->gameEntity->movetarget = Entity->Enemy->gameEntity;
		HuntTarget ();
		MapPrint (MAPPRINT_ERROR, Entity, Entity->State.GetOrigin(), "combattarget %s not found\n", Entity->gameEntity->combattarget);
		return;
	}

	// clear out our combattarget, these are a one shot deal
	Entity->gameEntity->combattarget = NULL;
	AIFlags |= AI_COMBAT_POINT;

	// clear the targetname, that point is ours!
	Entity->gameEntity->movetarget->targetname = NULL;
	PauseTime = 0;
#ifdef MONSTER_USE_ROGUE_AI
	// PMM
	BlindFireTarget = Entity->Enemy->State.GetOrigin();
	BlindFireDelay = 0;
	// PMM
#endif

	// run for it
	Run ();
}

void CMonster::SetEffects()
{
	Entity->State.SetEffects (0);
	Entity->State.SetRenderEffects(RF_FRAMELERP);

	if (AIFlags & AI_RESURRECTING)
	{
		Entity->State.AddEffects(EF_COLOR_SHELL);
		Entity->State.AddRenderEffects(RF_SHELL_RED);
	}

	if (Entity->Health <= 0)
		return;

	if (Entity->gameEntity->powerarmor_time > level.framenum)
	{
		if (PowerArmorType == POWER_ARMOR_SCREEN)
			Entity->State.AddEffects (EF_POWERSCREEN);
		else if (PowerArmorType == POWER_ARMOR_SHIELD)
		{
			Entity->State.AddEffects (EF_COLOR_SHELL);
			Entity->State.AddRenderEffects (RF_SHELL_GREEN);
		}
	}
}

void CMonster::WorldEffects()
{
	vec3f origin = Entity->State.GetOrigin();

	if (Entity->Health > 0)
	{
		if (!(Entity->Flags & FL_SWIM))
		{
			if (Entity->gameEntity->waterlevel < 3)
				Entity->AirFinished = level.framenum + 120;
			else if (Entity->AirFinished < level.framenum)
			{
				if (PainDebounceTime < level.framenum)
				{
					int dmg = 2 + 2 * (level.framenum - Entity->AirFinished);
					if (dmg > 15)
						dmg = 15;
					Entity->TakeDamage (World, World, vec3fOrigin, origin, vec3fOrigin, dmg, 0, DAMAGE_NO_ARMOR, MOD_WATER);
					PainDebounceTime = level.framenum + 10;
				}
			}
		}
		else
		{
			if (Entity->gameEntity->waterlevel > 0)
				Entity->AirFinished = level.framenum + 90;
			else if (Entity->AirFinished < level.framenum)
			{	// suffocate!
				if (PainDebounceTime < level.framenum)
				{
					int dmg = 2 + 2 * (level.framenum - Entity->AirFinished);
					if (dmg > 15)
						dmg = 15;
					Entity->TakeDamage (World, World, vec3fOrigin, origin, vec3fOrigin, dmg, 0, DAMAGE_NO_ARMOR, MOD_WATER);
					PainDebounceTime = level.framenum + 10;
				}
			}
		}
	}
	
	if (Entity->gameEntity->waterlevel == 0)
	{
		if (Entity->Flags & FL_INWATER)
		{	
			Entity->PlaySound (CHAN_BODY, SoundIndex("player/watr_out.wav"));
			Entity->Flags &= ~FL_INWATER;
		}
		return;
	}

	if ((Entity->gameEntity->watertype & CONTENTS_LAVA) && !(Entity->Flags & FL_IMMUNE_LAVA))
	{
		if (Entity->DamageDebounceTime < level.framenum)
		{
			Entity->DamageDebounceTime = level.framenum + 2;
			Entity->TakeDamage (World, World, vec3fOrigin, origin, vec3fOrigin, 10*Entity->gameEntity->waterlevel, 0, 0, MOD_LAVA);
		}
	}
	if ((Entity->gameEntity->watertype & CONTENTS_SLIME) && !(Entity->Flags & FL_IMMUNE_SLIME))
	{
		if (Entity->DamageDebounceTime < level.framenum)
		{
			Entity->DamageDebounceTime = level.framenum + 10;
			Entity->TakeDamage (World, World, vec3fOrigin, origin, vec3fOrigin, 4*Entity->gameEntity->waterlevel, 0, 0, MOD_SLIME);
		}
	}
	
	if ( !(Entity->Flags & FL_INWATER) )
	{	
		if (!(Entity->GetSvFlags() & SVF_DEADMONSTER))
		{
			if (Entity->gameEntity->watertype & CONTENTS_LAVA)
			{
				if (random() <= 0.5)
					Entity->PlaySound (CHAN_BODY, SoundIndex("player/lava1.wav"));
				else
					Entity->PlaySound (CHAN_BODY, SoundIndex("player/lava2.wav"));
			}
			else
				Entity->PlaySound (CHAN_BODY, SoundIndex("player/watr_in.wav"));
		}

		Entity->Flags |= FL_INWATER;
		Entity->DamageDebounceTime = 0;
	}
}

void CMonster::CatagorizePosition()
{
//
// get waterlevel
//
	vec3f point = Entity->State.GetOrigin() + vec3f (0, 0, Entity->GetMins().Z + 1);	
	int cont = PointContents (point);

	if (!(cont & CONTENTS_MASK_WATER))
	{
		Entity->gameEntity->waterlevel = 0;
		Entity->gameEntity->watertype = 0;
		return;
	}

	Entity->gameEntity->watertype = cont;
	Entity->gameEntity->waterlevel = 1;
	point.Z += 26;
	cont = PointContents (point);
	if (!(cont & CONTENTS_MASK_WATER))
		return;

	Entity->gameEntity->waterlevel = 2;
	point.Z += 22;
	cont = PointContents (point);
	if (cont & CONTENTS_MASK_WATER)
		Entity->gameEntity->waterlevel = 3;
}

void CMonster::CheckGround()
{
	if (Entity->Flags & (FL_SWIM|FL_FLY))
		return;

	if (Entity->Velocity.Z > 100)
	{
		Entity->GroundEntity = NULL;
		return;
	}

// if the hull point one-quarter unit down is solid the entity is on ground
	vec3f point = Entity->State.GetOrigin() - vec3f(0, 0, 0.25f);
	CTrace trace (Entity->State.GetOrigin(), Entity->GetMins(), Entity->GetMaxs(), point, Entity->gameEntity, CONTENTS_MASK_MONSTERSOLID);

	// check steepness
	if (trace.plane.normal[2] < 0.7 && !trace.startSolid)
	{
		Entity->GroundEntity = NULL;
		return;
	}

	if (!trace.startSolid && !trace.allSolid)
	{
		Entity->State.SetOrigin (trace.endPos);
		Entity->GroundEntity = trace.Ent;
		Entity->GroundEntityLinkCount = trace.Ent->GetLinkCount();
		Entity->Velocity.Z = 0;
	}
}

void CMonster::HuntTarget()
{
	Entity->gameEntity->goalentity = Entity->Enemy->gameEntity;
	if (AIFlags & AI_STAND_GROUND)
		Stand ();
	else
		Run ();
 
	IdealYaw = (Entity->Enemy->State.GetOrigin() - Entity->State.GetOrigin()).ToYaw();
	// wait a while before first attack
	if (!(AIFlags & AI_STAND_GROUND))
		AttackFinished = level.framenum + 1;
}

bool CMonster::FindTarget()
{
	bool		heardit;
	CBaseEntity *client;

	if (AIFlags & AI_GOOD_GUY)
	{
		if (Entity->gameEntity->goalentity && Entity->gameEntity->goalentity->inUse && Entity->gameEntity->goalentity->classname)
		{
			if (strcmp(Entity->gameEntity->goalentity->classname, "target_actor") == 0)
				return false;
		}

		//FIXME look for monsters?
		return false;
	}

	// if we're going to a combat point, just proceed
	if (AIFlags & AI_COMBAT_POINT)
		return false;

#ifdef MONSTERS_USE_PATHFINDING
	if ((level.SoundEntityFramenum >= (level.framenum - 1)) && level.NoiseNode)
	{
		if (Entity->SpawnFlags & 1)
		{
			CTrace trace = CTrace(Entity->State.GetOrigin(), level.NoiseNode->Origin, Entity->gameEntity, CONTENTS_MASK_SOLID);

			if (trace.fraction < 1.0)
				return false;
		}
		else
		{
			if (!InHearableArea(Entity->State.GetOrigin(), level.NoiseNode->Origin))
				return false;
		}

		vec3f temp = level.NoiseNode->Origin - Entity->State.GetOrigin();
		if (temp.Length() > 1000)	// too far to hear
			return false;

		IdealYaw = temp.ToYaw ();
		ChangeYaw ();

		// hunt the sound for a bit; hopefully find the real player
		//AIFlags |= AI_SOUND_TARGET;

		P_CurrentNode = GetClosestNodeTo(Entity->State.GetOrigin());
		P_CurrentGoalNode = level.NoiseNode;
		FoundPath ();

		// Check if we can see the entity too
		if (IsVisible(Entity, level.SoundEntity) && !Entity->Enemy && (level.SoundEntityFramenum >= (level.framenum - 1)) && !(Entity->SpawnFlags & 1) )
		{
			client = level.SoundEntity;

			if (client)
			{
				if (client->Flags & FL_NOTARGET)
					return false;

				if (Entity->SpawnFlags & 1)
				{
					if (!IsVisible (Entity, client))
						return false;
				}
				else
				{
					if (!InHearableArea(Entity->State.GetOrigin(), client->State.GetOrigin()))
						return false;
				}

				vec3f temp = client->State.GetOrigin() - Entity->State.GetOrigin();
				if (temp.Length() > 1000)	// too far to hear
					return false;

				// check area portals - if they are different and not connected then we can't hear it
				if (client->GetAreaNum() != Entity->GetAreaNum())
				{
					if (!gi.AreasConnected(Entity->GetAreaNum(), client->GetAreaNum()))
						return false;
				}

				// hunt the sound for a bit; hopefully find the real player
				Entity->Enemy = client;

				FoundTarget ();
				AlertNearbyStroggs ();

				if (MonsterFlags & MF_HAS_SIGHT)
					Sight ();
			}
		}

		return true;
	}
#endif

// if the first spawnflag bit is set, the monster will only wake up on
// really seeing the player, not another monster getting angry or hearing
// something

// revised behavior so they will wake up if they "see" a player make a noise
// but not weapon impact/explosion noises

	heardit = false;
#ifndef MONSTERS_USE_PATHFINDING
	if ((level.sight_entity_framenum >= (level.framenum - 1)) && !(Entity->SpawnFlags & 1) )
	{
		client = level.sight_entity;
		if (client->Enemy == Entity->Enemy)
			return false;
	}
#endif

#ifndef MONSTERS_USE_PATHFINDING
	else if (level.sound_entity_framenum >= (level.framenum - 1))
	{
		client = level.sound_entity;
		heardit = true;
	}
	else if (!(Entity->Enemy) && (level.sound2_entity_framenum >= (level.framenum - 1)) && !(Entity->SpawnFlags & 1) )
	{
		client = level.sound2_entity;
		heardit = true;
	}
#else
	if (level.SoundEntityFramenum >= (level.framenum - 1))
	{
		client = level.SoundEntity;
		heardit = true;
	}
#endif
	else
	{
		client = level.sight_client;
		if (!client)
			return false;	// no clients to get mad at
	}

	if (!client)
		client = level.sight_client;
	if (!client)
		return false;

	// if the entity went away, forget it
	if (!client->IsInUse())
		return false;

	if (IsVisible(Entity, client) && (client == Entity->Enemy))
		return true;	// JDC false;

	if (client)
	{
		if (client->Flags & FL_NOTARGET)
			return false;
	}
	else
		return false;

	CBaseEntity *old = Entity->Enemy;
	if (!heardit)
	{
		ERangeType r = Range (Entity, client);

		if (r == RANGE_FAR)
			return false;

// this is where we would check invisibility

		// is client in an spot too dark to be seen?
		if (client->gameEntity->light_level <= 5)
			return false;

		if (!IsVisible (Entity, client))
			return false;

		if (r == RANGE_NEAR)
		{
			if (client->gameEntity->show_hostile < level.framenum && !IsInFront (Entity, client))
				return false;
		}
		else if (r == RANGE_MID)
		{
			if (!IsInFront (Entity, client))
				return false;
		}

		Entity->Enemy = client;

		AIFlags &= ~AI_SOUND_TARGET;

		if (!(Entity->Enemy->EntityFlags & ENT_PLAYER))
		{
			Entity->Enemy = Entity->Enemy->Enemy;
			if (!(Entity->Enemy->EntityFlags & ENT_PLAYER))
			{
				Entity->Enemy = NULL;
				return false;
			}
		}
	}
	else	// heardit
	{
		if (Entity->SpawnFlags & 1)
		{
			if (!IsVisible (Entity, client))
				return false;
		}
		else
		{
			if (!InHearableArea(Entity->State.GetOrigin (), client->State.GetOrigin ()))
				return false;
		}

		vec3f temp = client->State.GetOrigin() - Entity->State.GetOrigin();
		if (temp.Length() > 1000)	// too far to hear
			return false;

		// check area portals - if they are different and not connected then we can't hear it
		if (client->GetAreaNum() != Entity->GetAreaNum())
		{
			if (!gi.AreasConnected(Entity->GetAreaNum(), client->GetAreaNum()))
				return false;
		}

		IdealYaw = temp.ToYaw ();
		ChangeYaw ();

		// hunt the sound for a bit; hopefully find the real player
		//AIFlags |= AI_SOUND_TARGET;
		Entity->Enemy = client;
	}

//
// got one
//
	FoundTarget ();
	AlertNearbyStroggs ();

	if (!(AIFlags & AI_SOUND_TARGET) && (Entity->Enemy != old) && (MonsterFlags & MF_HAS_SIGHT))
		Sight ();

	return true;
}

bool CMonster::FacingIdeal()
{
	float delta = AngleModf (Entity->State.GetAngles().Y - IdealYaw);
	if (delta > 45 && delta < 315)
		return false;
	return true;
}

void CMonster::FliesOff()
{
	Entity->State.RemoveEffects(EF_FLIES);
	Entity->State.SetSound (0);
}

void CMonster::FliesOn ()
{
	if (Entity->gameEntity->waterlevel)
		return;
	Entity->State.AddEffects (EF_FLIES);
	Entity->State.SetSound (SoundIndex ("infantry/inflies1.wav"));
	Think = &CMonster::FliesOff;
	Entity->NextThink = level.framenum + 600;
}

void CMonster::CheckFlies ()
{
	if (Entity->gameEntity->waterlevel)
		return;

	if (random() > 0.5)
		return;

	Think = &CMonster::FliesOn;
	Entity->NextThink = level.framenum + ((5 + 10 * random()) * 10);
}

uint32 LastID = 0;
CMonster::CMonster (uint32 ID) :
MonsterName(NULL),
MonsterID(ID)
{
}

#ifdef MONSTER_USE_ROGUE_AI
void CMonster::DoneDodge ()
{
	AIFlags &= ~AI_DODGING;
}

void CMonster::SideStep ()
{
}

void CMonster::Dodge (CBaseEntity *attacker, float eta, CTrace *tr)
{
	float	r = random();
	float	height;
	bool	ducker = false, dodger = false;

	// this needs to be here since this can be called after the monster has "died"
	if (Entity->Health < 1)
		return;

	if ((MonsterFlags & MF_HAS_DUCK) && (MonsterFlags & MF_HAS_UNDUCK))
		ducker = true;
	if ((MonsterFlags & MF_HAS_SIDESTEP) && !(AIFlags & AI_STAND_GROUND))
		dodger = true;

	if ((!ducker) && (!dodger))
		return;

	if (!Entity->Enemy)
	{
		Entity->Enemy = attacker;
		FoundTarget ();
	}

	// PMM - don't bother if it's going to hit anyway; fix for weird in-your-face etas (I was
	// seeing numbers like 13 and 14)
	if ((eta < 0.1) || (eta > 5))
	{
//		if ((g_showlogic) && (g_showlogic->value))
//			gi.dprintf ("timeout\n");
		return;
	}

	// skill level determination..
	if (r > (0.25*((skill->Integer())+1)))
	{
		return;
	}

	// stop charging, since we're going to dodge (somehow) instead
//	soldier_stop_charge (self);

	if (ducker)
	{
		height = Entity->GetAbsMax().Z-32-1;  // the -1 is because the absmax is s.origin + maxs + 1

		// FIXME, make smarter
		// if we only duck, and ducking won't help or we're already ducking, do nothing
		//
		// need to add monsterinfo.abort_duck() and monsterinfo.next_duck_time

		if ((!dodger) && ((tr->endPos[2] <= height) || (AIFlags & AI_DUCKED)))
			return;
	}
	else
		height = Entity->GetAbsMax().Z;

	if (dodger)
	{
		// if we're already dodging, just finish the sequence, i.e. don't do anything else
		if (AIFlags & AI_DODGING)
		{
//			if ((g_showlogic) && (g_showlogic->value))
//				gi.dprintf ("already dodging\n");
			return;
		}

		// if we're ducking already, or the shot is at our knees
		if ((tr->endPos[2] <= height) || (AIFlags & AI_DUCKED))
		{
			vec3f right;

			Entity->State.GetAngles().ToVectors (NULL, &right, NULL);
			Lefty = !(right.Dot (tr->EndPos - Entity->State.GetOrigin()) < 0);	
			// if we are currently ducked, unduck

			if ((ducker) && (AIFlags & AI_DUCKED))
			{
//				if ((g_showlogic) && (g_showlogic->value))
//					gi.dprintf ("unducking - ");
				UnDuck();
			}

			AIFlags |= AI_DODGING;
			AttackState = AS_SLIDING;

			// call the monster specific code here
			SideStep ();
			return;
		}
	}

	if (ducker)
	{
		if (NextDuckTime > level.framenum)
		{
//			if ((g_showlogic) && (g_showlogic->value))
//				gi.dprintf ("ducked too often, not ducking\n");
			return;
		}

//		if ((g_showlogic) && (g_showlogic->value))
//			gi.dprintf ("ducking!\n");

		DoneDodge ();
		// set this prematurely; it doesn't hurt, and prevents extra iterations
		AIFlags |= AI_DUCKED;

		Duck (eta);
	}
}

void CMonster::DuckDown ()
{
	AIFlags |= AI_DUCKED;

	vec3f tempMaxs = Entity->GetMaxs();
	tempMaxs.Z = BaseHeight - 32;
	Entity->SetMaxs(tempMaxs);
	Entity->CanTakeDamage = true;
	if (DuckWaitTime < level.framenum)
		DuckWaitTime = level.framenum + 10;
	Entity->Link ();
}

void CMonster::DuckHold ()
{
	if (level.framenum >= DuckWaitTime)
		AIFlags &= ~AI_HOLD_FRAME;
	else
		AIFlags |= AI_HOLD_FRAME;
}

void CMonster::Duck (float ETA)
{
}

void CMonster::UnDuck ()
{
	AIFlags &= ~AI_DUCKED;

	vec3f tempMaxs = Entity->GetMaxs();
	tempMaxs.Z = BaseHeight;
	Entity->SetMaxs(tempMaxs);
	Entity->CanTakeDamage = true;
	NextDuckTime = level.framenum + 5;
	Entity->Link ();
}
#endif