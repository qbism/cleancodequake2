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
// cc_mutant.cpp
// Mutant
//

#include "cc_local.h"
#include "m_mutant.h"
#include "cc_mutant.h"

CMutant::CMutant ()
{
	Scale = MODEL_SCALE;
}

//
// SOUNDS
//

void CMutant::Step ()
{
	switch ((rand() + 1) % 3)
	{
	case 0:
	default:
		Entity->PlaySound (CHAN_VOICE, SoundStep1);
		break;
	case 1:
		Entity->PlaySound (CHAN_VOICE, SoundStep2);
		break;
	case 2:
		Entity->PlaySound (CHAN_VOICE, SoundStep3);
		break;
	}
}

void CMutant::Sight ()
{
	Entity->PlaySound (CHAN_VOICE, SoundSight);
}

void CMutant::Search ()
{
	Entity->PlaySound (CHAN_VOICE, SoundSearch);
}

void CMutant::Swing ()
{
	Entity->PlaySound (CHAN_VOICE, SoundSwing);
}


//
// STAND
//

CFrame MutantFramesStand [] =
{
	CFrame (&CMonster::AI_Stand, 0),
	CFrame (&CMonster::AI_Stand, 0),
	CFrame (&CMonster::AI_Stand, 0),
	CFrame (&CMonster::AI_Stand, 0),
	CFrame (&CMonster::AI_Stand, 0),
	CFrame (&CMonster::AI_Stand, 0),
	CFrame (&CMonster::AI_Stand, 0),
	CFrame (&CMonster::AI_Stand, 0),
	CFrame (&CMonster::AI_Stand, 0),
	CFrame (&CMonster::AI_Stand, 0),		// 10

	CFrame (&CMonster::AI_Stand, 0),
	CFrame (&CMonster::AI_Stand, 0),
	CFrame (&CMonster::AI_Stand, 0),
	CFrame (&CMonster::AI_Stand, 0),
	CFrame (&CMonster::AI_Stand, 0),
	CFrame (&CMonster::AI_Stand, 0),
	CFrame (&CMonster::AI_Stand, 0),
	CFrame (&CMonster::AI_Stand, 0),
	CFrame (&CMonster::AI_Stand, 0),
	CFrame (&CMonster::AI_Stand, 0),		// 20

	CFrame (&CMonster::AI_Stand, 0),
	CFrame (&CMonster::AI_Stand, 0),
	CFrame (&CMonster::AI_Stand, 0),
	CFrame (&CMonster::AI_Stand, 0),
	CFrame (&CMonster::AI_Stand, 0),
	CFrame (&CMonster::AI_Stand, 0),
	CFrame (&CMonster::AI_Stand, 0),
	CFrame (&CMonster::AI_Stand, 0),
	CFrame (&CMonster::AI_Stand, 0),
	CFrame (&CMonster::AI_Stand, 0),		// 30

	CFrame (&CMonster::AI_Stand, 0),
	CFrame (&CMonster::AI_Stand, 0),
	CFrame (&CMonster::AI_Stand, 0),
	CFrame (&CMonster::AI_Stand, 0),
	CFrame (&CMonster::AI_Stand, 0),
	CFrame (&CMonster::AI_Stand, 0),
	CFrame (&CMonster::AI_Stand, 0),
	CFrame (&CMonster::AI_Stand, 0),
	CFrame (&CMonster::AI_Stand, 0),
	CFrame (&CMonster::AI_Stand, 0),		// 40

	CFrame (&CMonster::AI_Stand, 0),
	CFrame (&CMonster::AI_Stand, 0),
	CFrame (&CMonster::AI_Stand, 0),
	CFrame (&CMonster::AI_Stand, 0),
	CFrame (&CMonster::AI_Stand, 0),
	CFrame (&CMonster::AI_Stand, 0),
	CFrame (&CMonster::AI_Stand, 0),
	CFrame (&CMonster::AI_Stand, 0),
	CFrame (&CMonster::AI_Stand, 0),
	CFrame (&CMonster::AI_Stand, 0),		// 50

	CFrame (&CMonster::AI_Stand, 0)
};
CAnim MutantMoveStand (FRAME_stand101, FRAME_stand151, MutantFramesStand);

void CMutant::Stand ()
{
	CurrentMove = &MutantMoveStand;
}


//
// IDLE
//

void CMutant::IdleLoop ()
{
	if (random() < 0.75)
		NextFrame = FRAME_stand155;
}

CFrame MutantFramesIdle [] =
{
	CFrame (&CMonster::AI_Stand, 0),
	CFrame (&CMonster::AI_Stand, 0),
	CFrame (&CMonster::AI_Stand, 0),
	CFrame (&CMonster::AI_Stand, 0),					// scratch loop start
	CFrame (&CMonster::AI_Stand, 0),
	CFrame (&CMonster::AI_Stand, 0),
	CFrame (&CMonster::AI_Stand, 0, ConvertDerivedFunction(&CMutant::IdleLoop)),		// scratch loop end
	CFrame (&CMonster::AI_Stand, 0),
	CFrame (&CMonster::AI_Stand, 0),
	CFrame (&CMonster::AI_Stand, 0),
	CFrame (&CMonster::AI_Stand, 0),
	CFrame (&CMonster::AI_Stand, 0),
	CFrame (&CMonster::AI_Stand, 0)
};
CAnim MutantMoveIdle (FRAME_stand152, FRAME_stand164, MutantFramesIdle, &CMonster::Stand);

void CMutant::Idle ()
{
	CurrentMove = &MutantMoveIdle;
	Entity->PlaySound (CHAN_VOICE, SoundIdle, 1, ATTN_IDLE);
}

//
// WALK
//

CFrame MutantFramesWalk [] =
{
	CFrame (&CMonster::AI_Walk,	3),
	CFrame (&CMonster::AI_Walk,	1),
	CFrame (&CMonster::AI_Walk,	5),
	CFrame (&CMonster::AI_Walk,	10),
	CFrame (&CMonster::AI_Walk,	13),
	CFrame (&CMonster::AI_Walk,	10),
	CFrame (&CMonster::AI_Walk,	0),
	CFrame (&CMonster::AI_Walk,	5),
	CFrame (&CMonster::AI_Walk,	6),
	CFrame (&CMonster::AI_Walk,	16),
	CFrame (&CMonster::AI_Walk,	15),
	CFrame (&CMonster::AI_Walk,	6)
};
CAnim MutantMoveWalk (FRAME_walk05, FRAME_walk16, MutantFramesWalk);

CFrame MutantFramesStartWalk [] =
{
	CFrame (&CMonster::AI_Walk,	5),
	CFrame (&CMonster::AI_Walk,	5),
	CFrame (&CMonster::AI_Walk,	-2),
	CFrame (&CMonster::AI_Walk,	1)
};
CAnim MutantMoveStartWalk (FRAME_walk01, FRAME_walk04, MutantFramesStartWalk, ConvertDerivedFunction(&CMutant::WalkLoop));

void CMutant::WalkLoop ()
{
	CurrentMove = &MutantMoveWalk;
}

void CMutant::Walk ()
{
	CurrentMove = &MutantMoveStartWalk;
}


//
// RUN
//

CFrame MutantFramesRun [] =
{
	CFrame (&CMonster::AI_Run,	40),
	CFrame (&CMonster::AI_Run,	40,		ConvertDerivedFunction(&CMutant::Step)),
	CFrame (&CMonster::AI_Run,	24),
	CFrame (&CMonster::AI_Run,	5,		ConvertDerivedFunction(&CMutant::Step)),
	CFrame (&CMonster::AI_Run,	17),
	CFrame (&CMonster::AI_Run,	10)
};
CAnim MutantMoveRun (FRAME_run03, FRAME_run08, MutantFramesRun);

void CMutant::Run ()
{
	CurrentMove = (AIFlags & AI_STAND_GROUND) ? &MutantMoveStand : &MutantMoveRun;
}


//
// MELEE
//

void CMutant::HitLeft ()
{
	static vec3_t	aim = {MELEE_DISTANCE, Entity->GetMins().X, 8};

	if (CMeleeWeapon::Fire (Entity, aim, (10 + (rand() %5)), 100))
		Entity->PlaySound (CHAN_WEAPON, SoundHit);
	else
		Entity->PlaySound (CHAN_WEAPON, SoundSwing);
}

void CMutant::HitRight ()
{
	static vec3_t	aim = {MELEE_DISTANCE, Entity->GetMins().X, 8};

	if (CMeleeWeapon::Fire (Entity, aim, (10 + (rand() %5)), 100))
		Entity->PlaySound (CHAN_WEAPON, SoundHit2);
	else
		Entity->PlaySound (CHAN_WEAPON, SoundSwing);
}

void CMutant::CheckRefire ()
{
	if (!Entity->gameEntity->enemy || !Entity->gameEntity->enemy->inUse || Entity->gameEntity->enemy->health <= 0)
		return;

	// Paril, this was kinda dumb because he would keep refiring on nightmare
	// making him really easy to kill
	if ((skill->Integer() == 3) && (random() < 0.5))
		return;

	if (range(Entity->gameEntity, Entity->gameEntity->enemy) == RANGE_MELEE)
		NextFrame = FRAME_attack09;
}

CFrame MutantFramesAttack [] =
{
	CFrame (&CMonster::AI_Charge,	0),
	CFrame (&CMonster::AI_Charge,	0),
	CFrame (&CMonster::AI_Charge,	0,	ConvertDerivedFunction(&CMutant::HitLeft)),
	CFrame (&CMonster::AI_Charge,	0),
	CFrame (&CMonster::AI_Charge,	0),
	CFrame (&CMonster::AI_Charge,	0,	ConvertDerivedFunction(&CMutant::HitRight)),
	CFrame (&CMonster::AI_Charge,	0,	ConvertDerivedFunction(&CMutant::CheckRefire))
};
CAnim MutantMoveAttack (FRAME_attack09, FRAME_attack15, MutantFramesAttack, &CMonster::Run);

void CMutant::Melee ()
{
	CurrentMove = &MutantMoveAttack;
}

//
// ATTACK
//

void CMutant::Touch (CBaseEntity *other, plane_t *plane, cmBspSurface_t *surf)
{
	if (!Jumping)
		return;

	if (Entity->gameEntity->health <= 0)
	{
		Jumping = false;
		return;
	}

	if (other->EntityFlags & ENT_HURTABLE)
	{
		if (Vec3Length(Entity->gameEntity->velocity) > 400)
		{
			vec3f	normal (Entity->gameEntity->velocity);
			normal.Normalize();

			vec3f origin = Entity->State.GetOrigin();
			vec3f point = origin.MultiplyAngles (Entity->GetMaxs().X, normal);

			int damage = 40 + 10 * random();
			dynamic_cast<CHurtableEntity*>(other)->TakeDamage (Entity, Entity, Entity->gameEntity->velocity, point, normal, damage, damage, 0, MOD_UNKNOWN);
		}
	}

	if (!CheckBottom ())
	{
		if (Entity->gameEntity->groundentity)
		{
			NextFrame = FRAME_attack02;
			Jumping = false;
		}
		return;
	}

	Jumping = false;
}

#define MUTANT_JUMPS_UNSTUPIDLY
void CMutant::JumpTakeOff ()
{
#ifndef MUTANT_JUMPS_UNSTUPIDLY
	vec3_t	forward;

	Entity->PlaySound (CHAN_VOICE, SoundSight);

	Angles_Vectors (Entity->state.angles, forward, NULL, NULL);
	Vec3Scale (forward, 600, Entity->gameEntity->velocity);
	Entity->gameEntity->velocity[2] = 250;
#else
	vec3_t	forward, up, angles, temp;

	vec3_t origin;
	Entity->State.GetOrigin(origin);

	if (AttemptJumpToLastSight)
	{
		Vec3Subtract (LastSighting, origin, angles);
		AttemptJumpToLastSight = false;
	}
	else
		Vec3Subtract (Entity->gameEntity->enemy->state.origin, origin, angles);
	//Angles_Vectors (angles, forward, NULL, NULL);
	//VectorNormalizef (forward, forward);
	VecToAngles (angles, temp);

	Angles_Vectors (temp, forward, NULL, up);

	Entity->PlaySound (CHAN_VOICE, SoundSight);
	Vec3MA (Entity->gameEntity->velocity, 550, forward, Entity->gameEntity->velocity);
	Vec3MA (Entity->gameEntity->velocity, 60 + Vec3Length(angles), up, Entity->gameEntity->velocity);
	//Entity->gameEntity->velocity[2] = 250;
#endif

	origin[2] += 1;
	Entity->State.SetOrigin(origin);
	Entity->gameEntity->groundentity = NULL;
	AIFlags |= AI_DUCKED;
	AttackFinished = level.framenum + 30;
	Jumping = true;
}

void CMutant::CheckLanding ()
{
	if (Entity->gameEntity->groundentity)
	{
		Entity->PlaySound (CHAN_WEAPON, SoundThud);
		AttackFinished = 0;
		AIFlags &= ~AI_DUCKED;
		return;
	}

	if (level.framenum > AttackFinished)
		NextFrame = FRAME_attack02;
	else
		NextFrame = FRAME_attack05;
}

CFrame MutantFramesJump [] =
{
	CFrame (&CMonster::AI_Charge,	 0),
	CFrame (&CMonster::AI_Charge,	17),
	CFrame (&CMonster::AI_Charge,	15,	ConvertDerivedFunction(&CMutant::JumpTakeOff)),
	CFrame (&CMonster::AI_Charge,	15),
	CFrame (&CMonster::AI_Charge,	15,	ConvertDerivedFunction(&CMutant::CheckLanding)),
	CFrame (&CMonster::AI_Charge,	 0),
	CFrame (&CMonster::AI_Charge,	 3),
	CFrame (&CMonster::AI_Charge,	 0)
};
CAnim MutantMoveJump (FRAME_attack01, FRAME_attack08, MutantFramesJump, &CMonster::Run);

void CMutant::Attack ()
{
	CurrentMove = &MutantMoveJump;
}


//
// CHECKATTACK
//

bool CMutant::CheckMelee ()
{
	if (range (Entity->gameEntity, Entity->gameEntity->enemy) == RANGE_MELEE)
		return true;
	return false;
}

bool CMutant::CheckJump ()
{
	vec3_t origin;
	Entity->State.GetOrigin(origin);
	// Did we lose sight of them?
	if (AIFlags & AI_LOST_SIGHT)
	{
		vec3_t temp;

		// Did we already try this?
		if (AttemptJumpToLastSight)
			return false;

		Vec3Subtract (origin, LastSighting, temp);
		if (Vec3Length(temp) > 400)
		{
			//DebugPrintf ("Not attempting.\n");
			return false; // Too far
		}

		// So we lost sight of the player.
		// Can we jump to the last spot we saw him?
		CTrace trace = CTrace(origin, LastSighting, Entity->gameEntity, CONTENTS_MASK_MONSTERSOLID);

		//CTempEnt_Trails::DebugTrail (origin, LastSighting);

		// Clear shot
		if (trace.fraction == 1.0)
		{
			// Now we need to check if the last sighting is on ground.
			vec3_t below = {LastSighting[0], LastSighting[1], LastSighting[2] - 64};

			trace = CTrace (LastSighting, below, Entity->gameEntity, CONTENTS_MASK_MONSTERSOLID);
			//CTempEnt_Trails::DebugTrail (LastSighting, below);
			if (trace.fraction < 1.0)
			{
				// Hit floor, we're solid and can do this jump
				AttemptJumpToLastSight = true;
				return true;
			}
		}
		else
		{
			// We weren't able to get to this spot right away.
			// We need to calculate a good spot for this.
			int escape = 0;
			vec3_t temp, angles, forward;

			// Keep going back about 15 units until we're clear
			Vec3Subtract (LastSighting, origin, angles);
			VecToAngles (angles, temp);
			angles[0] = angles[2] = 0; // Only move the yaw
			Angles_Vectors (temp, forward, NULL, NULL);

			Vec3Copy (LastSighting, temp);

			while (trace.fraction != 1.0 && escape < 100)
			{
				escape++;
			
				Vec3MA (temp, -15, forward, temp);
				trace = CTrace(origin, temp, Entity->gameEntity, CONTENTS_MASK_MONSTERSOLID);
				//CTempEnt_Trails::DebugTrail (origin, temp);
			}

			// Push it up a bit
			temp[2] += 8;

			if (escape != 100)
			{
				// Assume our last trace passed
				//CTempEnt::TeleportEffect (temp);
				AttemptJumpToLastSight = true;
				Vec3Copy (temp, LastSighting);
				return true;
			}
		}
		return false;
	}
	AttemptJumpToLastSight = false;
	//if (Entity->absMin[2] > (Entity->gameEntity->enemy->absMin[2] + 0.75 * Entity->gameEntity->enemy->size[2]))
	//	return false;

	//if (Entity->absMax[2] < (Entity->gameEntity->enemy->absMin[2] + 0.25 * Entity->gameEntity->enemy->size[2]))
	//	return false;

	vec3_t v = {origin[0] - Entity->gameEntity->enemy->state.origin[0],
				origin[1] - Entity->gameEntity->enemy->state.origin[1],
				0};
	float distance = Vec3Length(v);

	if (distance < 100)
		return false;
	if (distance > 100)
	{
		if (random() < 0.9)
			return false;
	}
	else if (distance > 350)
		return false;

	return true;
}

bool CMutant::CheckAttack ()
{
	if (!Entity->gameEntity->enemy || Entity->gameEntity->enemy->health <= 0)
		return false;

	if (CheckMelee())
	{
		AttackState = AS_MELEE;
		return true;
	}

	if (CheckJump())
	{
		AttackState = AS_MISSILE;
		return true;
	}

	return false;
}


//
// PAIN
//

CFrame MutantFramesPain1 [] =
{
	CFrame (&CMonster::AI_Move,	4),
	CFrame (&CMonster::AI_Move,	-3),
	CFrame (&CMonster::AI_Move,	-8),
	CFrame (&CMonster::AI_Move,	2),
	CFrame (&CMonster::AI_Move,	5)
};
CAnim MutantMovePain1 (FRAME_pain101, FRAME_pain105, MutantFramesPain1, &CMonster::Run);

CFrame MutantFramesPain2 [] =
{
	CFrame (&CMonster::AI_Move,	-24),
	CFrame (&CMonster::AI_Move,	11),
	CFrame (&CMonster::AI_Move,	5),
	CFrame (&CMonster::AI_Move,	-2),
	CFrame (&CMonster::AI_Move,	6),
	CFrame (&CMonster::AI_Move,	4)
};
CAnim MutantMovePain2 (FRAME_pain201, FRAME_pain206, MutantFramesPain2, &CMonster::Run);

CFrame MutantFramesPain3 [] =
{
	CFrame (&CMonster::AI_Move,	-22),
	CFrame (&CMonster::AI_Move,	3),
	CFrame (&CMonster::AI_Move,	3),
	CFrame (&CMonster::AI_Move,	2),
	CFrame (&CMonster::AI_Move,	1),
	CFrame (&CMonster::AI_Move,	1),
	CFrame (&CMonster::AI_Move,	6),
	CFrame (&CMonster::AI_Move,	3),
	CFrame (&CMonster::AI_Move,	2),
	CFrame (&CMonster::AI_Move,	0),
	CFrame (&CMonster::AI_Move,	1)
};
CAnim MutantMovePain3 (FRAME_pain301, FRAME_pain311, MutantFramesPain3, &CMonster::Run);

void CMutant::Pain (CBaseEntity *other, float kick, int damage)
{
	if (Entity->gameEntity->health < (Entity->gameEntity->max_health / 2))
		Entity->State.SetSkinNum(1);

	if (level.framenum < Entity->gameEntity->pain_debounce_time)
		return;

	Entity->gameEntity->pain_debounce_time = level.framenum + 30;

	if (skill->Integer() == 3)
		return;		// no pain anims in nightmare

	switch (rand()%3)
	{
	case 0:
		Entity->PlaySound (CHAN_VOICE, SoundPain1);
		CurrentMove = &MutantMovePain1;
		break;
	case 1:
		Entity->PlaySound (CHAN_VOICE, SoundPain2);
		CurrentMove = &MutantMovePain2;
		break;
	case 2:
		Entity->PlaySound (CHAN_VOICE, SoundPain1);
		CurrentMove = &MutantMovePain3;
		break;
	}
}


//
// DEATH
//

void CMutant::Dead ()
{
	Entity->SetMins (vec3f(-16, -16, -24));
	Entity->SetMaxs (vec3f(16, 16, -8));
	Entity->PhysicsType = PHYSICS_TOSS;
	Entity->SetSvFlags (Entity->GetSvFlags() | SVF_DEADMONSTER);
	Entity->Link ();

	CheckFlies ();
}

CFrame MutantFramesDeath1 [] =
{
	CFrame (&CMonster::AI_Move,	0),
	CFrame (&CMonster::AI_Move,	0),
	CFrame (&CMonster::AI_Move,	0),
	CFrame (&CMonster::AI_Move,	0),
	CFrame (&CMonster::AI_Move,	0),
	CFrame (&CMonster::AI_Move,	0),
	CFrame (&CMonster::AI_Move,	0),
	CFrame (&CMonster::AI_Move,	0),
	CFrame (&CMonster::AI_Move,	0)
};
CAnim MutantMoveDeath1 (FRAME_death101, FRAME_death109, MutantFramesDeath1, ConvertDerivedFunction(&CMutant::Dead));

CFrame MutantFramesDeath2 [] =
{
	CFrame (&CMonster::AI_Move,	0),
	CFrame (&CMonster::AI_Move,	0),
	CFrame (&CMonster::AI_Move,	0),
	CFrame (&CMonster::AI_Move,	0),
	CFrame (&CMonster::AI_Move,	0),
	CFrame (&CMonster::AI_Move,	0),
	CFrame (&CMonster::AI_Move,	0),
	CFrame (&CMonster::AI_Move,	0),
	CFrame (&CMonster::AI_Move,	0),
	CFrame (&CMonster::AI_Move,	0)
};
CAnim MutantMoveDeath2 (FRAME_death201, FRAME_death210, MutantFramesDeath2, ConvertDerivedFunction(&CMutant::Dead));

void CMutant::Die (CBaseEntity *inflictor, CBaseEntity *attacker, int damage, vec3_t point)
{
	if (Entity->gameEntity->health <= Entity->gameEntity->gib_health)
	{
		Entity->PlaySound (CHAN_VOICE, SoundIndex ("misc/udeath.wav"));
		for (int n= 0; n < 2; n++)
			CGibEntity::Spawn (Entity, gMedia.Gib_Bone[0], damage, GIB_ORGANIC);
		for (int n= 0; n < 4; n++)
			CGibEntity::Spawn (Entity, gMedia.Gib_SmallMeat, damage, GIB_ORGANIC);
		Entity->ThrowHead (gMedia.Gib_Head[1], damage, GIB_ORGANIC);
		Entity->gameEntity->deadflag = DEAD_DEAD;
		return;
	}

	if (Entity->gameEntity->deadflag == DEAD_DEAD)
		return;

	Entity->PlaySound (CHAN_VOICE, SoundDeath);
	Entity->gameEntity->deadflag = DEAD_DEAD;
	Entity->gameEntity->takedamage = true;
	Entity->State.SetSkinNum(1);

	CurrentMove = (random() < 0.5) ? &MutantMoveDeath1 : &MutantMoveDeath2;
}


//
// SPAWN
//

/*QUAKED monster_mutant (1 .5 0) (-32 -32 -24) (32 32 32) Ambush Trigger_Spawn Sight
*/
void CMutant::Spawn ()
{
	SoundSwing = SoundIndex ("mutant/mutatck1.wav");
	SoundHit = SoundIndex ("mutant/mutatck2.wav");
	SoundHit2 = SoundIndex ("mutant/mutatck3.wav");
	SoundDeath = SoundIndex ("mutant/mutdeth1.wav");
	SoundIdle = SoundIndex ("mutant/mutidle1.wav");
	SoundPain1 = SoundIndex ("mutant/mutpain1.wav");
	SoundPain2 = SoundIndex ("mutant/mutpain2.wav");
	SoundSight = SoundIndex ("mutant/mutsght1.wav");
	SoundSearch = SoundIndex ("mutant/mutsrch1.wav");
	SoundStep1 = SoundIndex ("mutant/step1.wav");
	SoundStep2 = SoundIndex ("mutant/step2.wav");
	SoundStep3 = SoundIndex ("mutant/step3.wav");
	SoundThud = SoundIndex ("mutant/thud1.wav");
	
	Entity->SetSolid (SOLID_BBOX);
	Entity->State.SetModelIndex (ModelIndex ("models/monsters/mutant/tris.md2"));
	Entity->SetMins (vec3f(-32, -32, -24));
	Entity->SetMaxs (vec3f(32, 32, 48));

	Entity->gameEntity->health = 300;
	Entity->gameEntity->gib_health = -120;
	Entity->gameEntity->mass = 300;

	MonsterFlags = (MF_HAS_ATTACK | MF_HAS_MELEE | MF_HAS_SIGHT | MF_HAS_SEARCH | MF_HAS_IDLE);

	Entity->Link ();
	CurrentMove = &MutantMoveStand;
	WalkMonsterStart ();
}

LINK_MONSTER_CLASSNAME_TO_CLASS ("monster_mutant", CMutant);