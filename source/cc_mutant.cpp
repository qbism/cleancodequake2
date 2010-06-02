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

CMutant::CMutant (uint32 ID) :
CMonster(ID)
{
	Scale = MODEL_SCALE;
	MonsterName = "Mutant";
}

//
// SOUNDS
//

void CMutant::Step ()
{
	switch (irandom(3))
	{
	case 0:
	default:
		Entity->PlaySound (CHAN_VOICE, Sounds[SOUND_STEP1]);
		break;
	case 1:
		Entity->PlaySound (CHAN_VOICE, Sounds[SOUND_STEP2]);
		break;
	case 2:
		Entity->PlaySound (CHAN_VOICE, Sounds[SOUND_STEP3]);
		break;
	}
}

void CMutant::Sight ()
{
	Entity->PlaySound (CHAN_VOICE, Sounds[SOUND_SIGHT]);
}

void CMutant::Search ()
{
	Entity->PlaySound (CHAN_VOICE, Sounds[SOUND_SEARCH]);
}

void CMutant::Swing ()
{
	Entity->PlaySound (CHAN_VOICE, Sounds[SOUND_SWING]);
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
	if (frand() < 0.75)
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
	Entity->PlaySound (CHAN_VOICE, Sounds[SOUND_IDLE], 255, ATTN_IDLE);
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
	vec3f	aim (MELEE_DISTANCE, Entity->GetMins().X, 8);

	if (CMeleeWeapon::Fire (Entity, aim, (10 + (irandom(5))), 100))
		Entity->PlaySound (CHAN_WEAPON, Sounds[SOUND_HIT1]);
	else
		Entity->PlaySound (CHAN_WEAPON, Sounds[SOUND_SWING]);
}

void CMutant::HitRight ()
{
	vec3f	aim (MELEE_DISTANCE, Entity->GetMins().X, 8);

	if (CMeleeWeapon::Fire (Entity, aim, (10 + (irandom(5))), 100))
		Entity->PlaySound (CHAN_WEAPON, Sounds[SOUND_HIT2]);
	else
		Entity->PlaySound (CHAN_WEAPON, Sounds[SOUND_SWING]);
}

void CMutant::CheckRefire ()
{
	if (!Entity->Enemy.IsValid() || entity_cast<IHurtableEntity>(*Entity->Enemy)->Health <= 0)
		return;

	// Paril, this was kinda dumb because he would keep refiring on nightmare
	// making him really easy to kill
	if ((CvarList[CV_SKILL].Integer() == 3) && (frand() < 0.5))
		return;

	if (Range(Entity, *Entity->Enemy) == RANGE_MELEE)
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

void CMutant::Touch (IBaseEntity *Other, SBSPPlane *plane, SBSPSurface *surf)
{
	if (!Jumping)
		return;

	if (Entity->Health <= 0)
	{
		Jumping = false;
		return;
	}

	if (Other->EntityFlags & ENT_HURTABLE)
	{
		if (Entity->Velocity.Length() > 400)
		{
			vec3f	normal (Entity->Velocity.GetNormalized());

			sint32 Damage = 40 + 10 * frand();
			entity_cast<IHurtableEntity>(Other)->TakeDamage (Entity, Entity, Entity->Velocity, Entity->State.GetOrigin().MultiplyAngles (Entity->GetMaxs().X, normal), normal, Damage, Damage, 0, MOD_UNKNOWN);
		}
	}

	if (!CheckBottom ())
	{
		if (Entity->GroundEntity.IsValid())
		{
			NextFrame = FRAME_attack02;
			Jumping = false;
		}
		return;
	}

	Jumping = false;
}

void CMutant::JumpTakeOff ()
{
#if !(MONSTER_SPECIFIC_FLAGS & MUTANT_JUMPS_UNSTUPIDLY)
	vec3f	forward;

	Entity->PlaySound (CHAN_VOICE, Sounds[SOUND_SIGHT]);

	Entity->State.GetAngles().ToVectors (&forward, NULL, NULL);
	Entity->Velocity = forward * 600;
	Entity->Velocity.Z = 250;
#else
	vec3f	forward, up, angles;

	if (AttemptJumpToLastSight)
	{
		angles = LastSighting - Entity->State.GetOrigin();
		AttemptJumpToLastSight = false;
	}
	else
		angles = Entity->Enemy->State.GetOrigin() - Entity->State.GetOrigin();

	angles.ToAngles ().ToVectors (&forward, NULL, &up);

	Entity->PlaySound (CHAN_VOICE, Sounds[SOUND_SIGHT]);
	Entity->Velocity = Entity->Velocity.MultiplyAngles (550, forward);
	Entity->Velocity = Entity->Velocity.MultiplyAngles (60 + angles.Length(), up);
#endif

	Entity->State.GetOrigin().Z += 1;
	Entity->GroundEntity = NULL;
	AIFlags |= AI_DUCKED;
	AttackFinished = Level.Frame + 30;
	Jumping = true;
	Entity->Touchable = true;
}

void CMutant::CheckLanding ()
{
	if (Entity->GroundEntity.IsValid())
	{
		Entity->PlaySound (CHAN_WEAPON, Sounds[SOUND_THUD]);
		AttackFinished = 0;
		AIFlags &= ~AI_DUCKED;
		Entity->Touchable = false;
		return;
	}

	if (Level.Frame > AttackFinished)
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
	if (Range (Entity, *Entity->Enemy) == RANGE_MELEE)
		return true;
	return false;
}

#include "cc_temporary_entities.h"

bool CMutant::CheckJump ()
{
	vec3f origin = Entity->State.GetOrigin();

	// Did we lose sight of them?
	if (AIFlags & AI_LOST_SIGHT)
	{
		// Did we already try this?
		if (AttemptJumpToLastSight)
			return false;

		vec3f temp = origin - LastSighting;
		if (temp.Length() > 400)
			return false; // Too far

		// So we lost sight of the player.
		// Can we jump to the last spot we saw him?
		CTrace trace (origin, LastSighting, Entity, CONTENTS_MASK_MONSTERSOLID);

		// Clear shot
		if (trace.fraction == 1.0)
		{
			// Now we need to check if the last sighting is on ground.
			vec3f below = LastSighting - vec3f(0, 0, 64);

			trace (LastSighting, below, Entity, CONTENTS_MASK_MONSTERSOLID);

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
			sint32 escape = 0;
			vec3f angles, forward;

			// Keep going back about 15 units until we're clear
			angles = (LastSighting - origin).ToAngles();
			angles.X = angles.Z = 0; // Only move the yaw
			temp.ToVectors (&forward, NULL, NULL);

			temp = LastSighting;
			while (trace.fraction != 1.0 && escape < 100)
			{
				escape++;
				
				temp = temp.MultiplyAngles (-15, forward);
				trace (origin, temp, Entity, CONTENTS_MASK_MONSTERSOLID);
			}

			// Push it up a bit
			temp.Z += 8;

			if (escape != 100)
			{
				// Assume our last trace passed
				AttemptJumpToLastSight = true;
				LastSighting = temp;
				return true;
			}
		}
		return false;
	}
	AttemptJumpToLastSight = false;
	//if (Entity->absMin[2] > (Entity->Enemy->absMin[2] + 0.75 * Entity->Enemy->size[2]))
	//	return false;

	//if (Entity->absMax[2] < (Entity->Enemy->absMin[2] + 0.25 * Entity->Enemy->size[2]))
	//	return false;

	vec3f v = origin - Entity->Enemy->State.GetOrigin();
	v.Z = 0;
	float distance = v.Length();

	if (distance < 100)
		return false;
	if (distance > 100)
	{
		if (frand() < 0.9)
			return false;
	}
	else if (distance > 350)
		return false;

	return true;
}

bool CMutant::CheckAttack ()
{
	if (!Entity->Enemy.IsValid() || entity_cast<IHurtableEntity>(*Entity->Enemy)->Health <= 0)
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

void CMutant::Pain (IBaseEntity *Other, sint32 Damage)
{
	if (Entity->Health < (Entity->MaxHealth / 2))
		Entity->State.GetSkinNum() = 1;

	if (Level.Frame < PainDebounceTime)
		return;

	PainDebounceTime = Level.Frame + 30;

	if (CvarList[CV_SKILL].Integer() == 3)
		return;		// no pain anims in nightmare

	switch (irandom(3))
	{
	case 0:
		Entity->PlaySound (CHAN_VOICE, Sounds[SOUND_PAIN1]);
		CurrentMove = &MutantMovePain1;
		break;
	case 1:
		Entity->PlaySound (CHAN_VOICE, Sounds[SOUND_PAIN2]);
		CurrentMove = &MutantMovePain2;
		break;
	case 2:
		Entity->PlaySound (CHAN_VOICE, Sounds[SOUND_PAIN1]);
		CurrentMove = &MutantMovePain3;
		break;
	}
}


//
// DEATH
//

void CMutant::Dead ()
{
	Entity->GetMins().Set (-16, -16, -24);
	Entity->GetMaxs().Set (16, 16, -8);
	Entity->PhysicsType = PHYSICS_TOSS;
	Entity->GetSvFlags() |= SVF_DEADMONSTER;
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

void CMutant::Die (IBaseEntity *Inflictor, IBaseEntity *Attacker, sint32 Damage, vec3f &point)
{
	if (Entity->Health <= Entity->GibHealth)
	{
		Entity->PlaySound (CHAN_VOICE, SoundIndex ("misc/udeath.wav"));
		for (sint32 n= 0; n < 2; n++)
			CGibEntity::Spawn (Entity, GameMedia.Gib_Bone[0], Damage, GIB_ORGANIC);
		for (sint32 n= 0; n < 4; n++)
			CGibEntity::Spawn (Entity, GameMedia.Gib_SmallMeat, Damage, GIB_ORGANIC);
		Entity->ThrowHead (GameMedia.Gib_Head[1], Damage, GIB_ORGANIC);
		Entity->DeadFlag = true;
		return;
	}

	if (Entity->DeadFlag == true)
		return;

	Entity->PlaySound (CHAN_VOICE, Sounds[SOUND_DEATH]);
	Entity->DeadFlag = true;
	Entity->CanTakeDamage = true;
	Entity->State.GetSkinNum() = 1;

	CurrentMove = (frand() < 0.5) ? &MutantMoveDeath1 : &MutantMoveDeath2;
}


//
// SPAWN
//

/*QUAKED monster_mutant (1 .5 0) (-32 -32 -24) (32 32 32) Ambush Trigger_Spawn Sight
*/
void CMutant::Spawn ()
{
	Sounds[SOUND_SWING] = SoundIndex ("mutant/mutatck1.wav");
	Sounds[SOUND_HIT1] = SoundIndex ("mutant/mutatck2.wav");
	Sounds[SOUND_HIT2] = SoundIndex ("mutant/mutatck3.wav");
	Sounds[SOUND_DEATH] = SoundIndex ("mutant/mutdeth1.wav");
	Sounds[SOUND_IDLE] = SoundIndex ("mutant/mutidle1.wav");
	Sounds[SOUND_PAIN1] = SoundIndex ("mutant/mutpain1.wav");
	Sounds[SOUND_PAIN2] = SoundIndex ("mutant/mutpain2.wav");
	Sounds[SOUND_SIGHT] = SoundIndex ("mutant/mutsght1.wav");
	Sounds[SOUND_SEARCH] = SoundIndex ("mutant/mutsrch1.wav");
	Sounds[SOUND_STEP1] = SoundIndex ("mutant/step1.wav");
	Sounds[SOUND_STEP2] = SoundIndex ("mutant/step2.wav");
	Sounds[SOUND_STEP3] = SoundIndex ("mutant/step3.wav");
	Sounds[SOUND_THUD] = SoundIndex ("mutant/thud1.wav");
	
	Entity->GetSolid() = SOLID_BBOX;
	Entity->State.GetModelIndex() = ModelIndex ("models/monsters/mutant/tris.md2");
	Entity->GetMins().Set (-32, -32, -24);
	Entity->GetMaxs().Set (32, 32, 48);

	Entity->Health = 300;
	Entity->GibHealth = -120;
	Entity->Mass = 300;

	MonsterFlags = (MF_HAS_ATTACK | MF_HAS_MELEE | MF_HAS_SIGHT | MF_HAS_SEARCH | MF_HAS_IDLE);

	Entity->Link ();
	CurrentMove = &MutantMoveStand;
	WalkMonsterStart ();
}

LINK_MONSTER_CLASSNAME_TO_CLASS ("monster_mutant", CMutant);
