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
// cc_infantry.cpp
// Infantry Monster
//

#include "cc_local.h"
#include "m_infantry.h"

CInfantry Monster_Infantry;

CInfantry::CInfantry ()
{
	Classname = "monster_infantry";
	Scale = MODEL_SCALE;
}

void CInfantry::Allocate (edict_t *ent)
{
	ent->Monster = new CInfantry(Monster_Infantry);
}

CFrame InfantryFramesStand [] =
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
	CFrame (&CMonster::AI_Stand, 0),
	CFrame (&CMonster::AI_Stand, 0),
	CFrame (&CMonster::AI_Stand, 0),
	CFrame (&CMonster::AI_Stand, 0),
	CFrame (&CMonster::AI_Stand, 0),
	CFrame (&CMonster::AI_Stand, 0),
	CFrame (&CMonster::AI_Stand, 0),
	CFrame (&CMonster::AI_Stand, 0),
	CFrame (&CMonster::AI_Stand, 0),
	CFrame (&CMonster::AI_Stand, 0),
	CFrame (&CMonster::AI_Stand, 0),
	CFrame (&CMonster::AI_Stand, 0),
	CFrame (&CMonster::AI_Stand, 0)
};
CAnim InfantryMoveStand (FRAME_stand50, FRAME_stand71, InfantryFramesStand);

void CInfantry::Stand ()
{
	CurrentMove = &InfantryMoveStand;
}

CFrame InfantryFramesFidget [] =
{
	CFrame (&CMonster::AI_Stand, 1),
	CFrame (&CMonster::AI_Stand, 0),
	CFrame (&CMonster::AI_Stand, 1),
	CFrame (&CMonster::AI_Stand, 3),
	CFrame (&CMonster::AI_Stand, 6),
	CFrame (&CMonster::AI_Stand, 3),
	CFrame (&CMonster::AI_Stand, 0),
	CFrame (&CMonster::AI_Stand, 0),
	CFrame (&CMonster::AI_Stand, 0),
	CFrame (&CMonster::AI_Stand, 0),
	CFrame (&CMonster::AI_Stand, 1),
	CFrame (&CMonster::AI_Stand, 0),
	CFrame (&CMonster::AI_Stand, 0),
	CFrame (&CMonster::AI_Stand, 0),
	CFrame (&CMonster::AI_Stand, 0),
	CFrame (&CMonster::AI_Stand, 1),
	CFrame (&CMonster::AI_Stand, 0),
	CFrame (&CMonster::AI_Stand, -1),
	CFrame (&CMonster::AI_Stand, 0),
	CFrame (&CMonster::AI_Stand, 0),
	CFrame (&CMonster::AI_Stand, 1),
	CFrame (&CMonster::AI_Stand, 0),
	CFrame (&CMonster::AI_Stand, -2),
	CFrame (&CMonster::AI_Stand, 1),
	CFrame (&CMonster::AI_Stand, 1),
	CFrame (&CMonster::AI_Stand, 1),
	CFrame (&CMonster::AI_Stand, -1),
	CFrame (&CMonster::AI_Stand, 0),
	CFrame (&CMonster::AI_Stand, 0),
	CFrame (&CMonster::AI_Stand, -1),
	CFrame (&CMonster::AI_Stand, 0),
	CFrame (&CMonster::AI_Stand, 0),
	CFrame (&CMonster::AI_Stand, 0),
	CFrame (&CMonster::AI_Stand, 0),
	CFrame (&CMonster::AI_Stand, 0),
	CFrame (&CMonster::AI_Stand, -1),
	CFrame (&CMonster::AI_Stand, 0),
	CFrame (&CMonster::AI_Stand, 0),
	CFrame (&CMonster::AI_Stand, 1),
	CFrame (&CMonster::AI_Stand, 0),
	CFrame (&CMonster::AI_Stand, 0),
	CFrame (&CMonster::AI_Stand, -1),
	CFrame (&CMonster::AI_Stand, -1),
	CFrame (&CMonster::AI_Stand, 0),
	CFrame (&CMonster::AI_Stand, -3),
	CFrame (&CMonster::AI_Stand, -2),
	CFrame (&CMonster::AI_Stand, -3),
	CFrame (&CMonster::AI_Stand, -3),
	CFrame (&CMonster::AI_Stand, -2)
};
CAnim InfantryMoveFidget (FRAME_stand01, FRAME_stand49, InfantryFramesFidget, &CMonster::Stand);

void CInfantry::Idle ()
{
	CurrentMove = &InfantryMoveFidget;
	Sound (Entity, CHAN_VOICE, SoundIdle, 1, ATTN_IDLE, 0);
}

CFrame InfantryFramesWalk [] =
{
	CFrame (&CMonster::AI_Walk, 5),
	CFrame (&CMonster::AI_Walk, 4),
	CFrame (&CMonster::AI_Walk, 4),
	CFrame (&CMonster::AI_Walk, 5),
	CFrame (&CMonster::AI_Walk, 4),
	CFrame (&CMonster::AI_Walk, 5),
	CFrame (&CMonster::AI_Walk, 6),
	CFrame (&CMonster::AI_Walk, 4),
	CFrame (&CMonster::AI_Walk, 4),
	CFrame (&CMonster::AI_Walk, 4),
	CFrame (&CMonster::AI_Walk, 4),
	CFrame (&CMonster::AI_Walk, 5)
};
CAnim InfantryMoveWalk (FRAME_walk03, FRAME_walk14, InfantryFramesWalk);

void CInfantry::Walk ()
{
	CurrentMove = &InfantryMoveWalk;
}

CFrame InfantryFramesRun [] =
{
	CFrame (&CMonster::AI_Run, 10),
	CFrame (&CMonster::AI_Run, 20),
	CFrame (&CMonster::AI_Run, 5),
	CFrame (&CMonster::AI_Run, 7),
	CFrame (&CMonster::AI_Run, 30),
	CFrame (&CMonster::AI_Run, 35),
	CFrame (&CMonster::AI_Run, 2),
	CFrame (&CMonster::AI_Run, 6)
};
CAnim InfantryMoveRun (FRAME_run01, FRAME_run08, InfantryFramesRun);

void CInfantry::Run ()
{
	// FIXME: handle this elsewhere
	if (AIFlags & AI_STAND_GROUND)
		CurrentMove = &InfantryMoveStand;
	else
		CurrentMove = &InfantryMoveRun;
}

CFrame InfantryFramesPain1 [] =
{
	CFrame (&CMonster::AI_Move, -3),
	CFrame (&CMonster::AI_Move, -2),
	CFrame (&CMonster::AI_Move, -1),
	CFrame (&CMonster::AI_Move, -2),
	CFrame (&CMonster::AI_Move, -1),
	CFrame (&CMonster::AI_Move, 1),
	CFrame (&CMonster::AI_Move, -1),
	CFrame (&CMonster::AI_Move, 1),
	CFrame (&CMonster::AI_Move, 6),
	CFrame (&CMonster::AI_Move, 2)
};
CAnim InfantryMovePain1 (FRAME_pain101, FRAME_pain110, InfantryFramesPain1, ConvertDerivedFunction(&CInfantry::Run));

CFrame InfantryFramesPain2 [] =
{
	CFrame (&CMonster::AI_Move, -3),
	CFrame (&CMonster::AI_Move, -3),
	CFrame (&CMonster::AI_Move, 0),
	CFrame (&CMonster::AI_Move, -1),
	CFrame (&CMonster::AI_Move, -2),
	CFrame (&CMonster::AI_Move, 0),
	CFrame (&CMonster::AI_Move, 0),
	CFrame (&CMonster::AI_Move, 2),
	CFrame (&CMonster::AI_Move, 5),
	CFrame (&CMonster::AI_Move, 2)
};
CAnim InfantryMovePain2 (FRAME_pain201, FRAME_pain210, InfantryFramesPain2, ConvertDerivedFunction(&CInfantry::Run));

void CInfantry::Pain (edict_t *other, float kick, int damage)
{
	if (Entity->health < (Entity->max_health / 2))
		Entity->s.skinNum = 1;

	if (level.time < Entity->pain_debounce_time)
		return;

	Entity->pain_debounce_time = level.time + 3;
	
	if (skill->Integer() == 3)
		return;		// no pain anims in nightmare

	CurrentMove = (rand() % 2 == 0) ? &InfantryMovePain1 : &InfantryMovePain2;
	Sound (Entity, CHAN_VOICE, (rand() % 2 == 0) ? SoundPain1 : SoundPain2);
}

vec3_t	DeathAimAngles[] =
{
	0.0, 5.0, 0.0,
	10.0, 15.0, 0.0,
	20.0, 25.0, 0.0,
	25.0, 35.0, 0.0,
	30.0, 40.0, 0.0,
	30.0, 45.0, 0.0,
	25.0, 50.0, 0.0,
	20.0, 40.0, 0.0,
	15.0, 35.0, 0.0,
	40.0, 35.0, 0.0,
	70.0, 35.0, 0.0,
	90.0, 35.0, 0.0
};

void CInfantry::MachineGun ()
{
	vec3_t	start, target;
	vec3_t	forward, right;
	vec3_t	vec;
	int		flash_number;

	if (Entity->s.frame == FRAME_attak111)
	{
		flash_number = MZ2_INFANTRY_MACHINEGUN_1;
		Angles_Vectors (Entity->s.angles, forward, right, NULL);
		G_ProjectSource (Entity->s.origin, dumb_and_hacky_monster_MuzzFlashOffset[flash_number], forward, right, start);

		if (Entity->enemy)
		{
			Vec3MA (Entity->enemy->s.origin, -0.2, Entity->enemy->velocity, target);
			target[2] += Entity->enemy->viewheight;
			Vec3Subtract (target, start, forward);
			VectorNormalizef (forward, forward);
		}
		else
			Angles_Vectors (Entity->s.angles, forward, right, NULL);
	}
	else
	{
		flash_number = MZ2_INFANTRY_MACHINEGUN_2 + (Entity->s.frame - FRAME_death211);

		Angles_Vectors (Entity->s.angles, forward, right, NULL);
		G_ProjectSource (Entity->s.origin, dumb_and_hacky_monster_MuzzFlashOffset[flash_number], forward, right, start);

		Vec3Subtract (Entity->s.angles, DeathAimAngles[flash_number-MZ2_INFANTRY_MACHINEGUN_2], vec);
		Angles_Vectors (vec, forward, NULL, NULL);
	}

	MonsterFireBullet (start, forward, 3, 4, DEFAULT_BULLET_HSPREAD, DEFAULT_BULLET_VSPREAD, flash_number);
}

void CInfantry::Sight ()
{
	Sound (Entity, CHAN_BODY, SoundSight);
}

void CInfantry::Dead ()
{
	Vec3Set (Entity->mins, -16, -16, -24);
	Vec3Set (Entity->maxs, 16, 16, -8);
	Entity->movetype = MOVETYPE_TOSS;
	Entity->svFlags |= SVF_DEADMONSTER;
	gi.linkentity (Entity);

	// FIXME: BAD
	CheckFlies ();
}

CFrame InfantryFramesDeath1 [] =
{
	CFrame (&CMonster::AI_Move, -4),
	CFrame (&CMonster::AI_Move, 0),
	CFrame (&CMonster::AI_Move, 0),
	CFrame (&CMonster::AI_Move, -1),
	CFrame (&CMonster::AI_Move, -4),
	CFrame (&CMonster::AI_Move, 0),
	CFrame (&CMonster::AI_Move, 0),
	CFrame (&CMonster::AI_Move, 0),
	CFrame (&CMonster::AI_Move, -1),
	CFrame (&CMonster::AI_Move, 3),
	CFrame (&CMonster::AI_Move, 1),
	CFrame (&CMonster::AI_Move, 1),
	CFrame (&CMonster::AI_Move, -20),
	CFrame (&CMonster::AI_Move, 2),
	CFrame (&CMonster::AI_Move, 2),
	CFrame (&CMonster::AI_Move, 9),
	CFrame (&CMonster::AI_Move, 9),
	CFrame (&CMonster::AI_Move, 5),
	CFrame (&CMonster::AI_Move, -3),
	CFrame (&CMonster::AI_Move, -3)
};
CAnim InfantryMoveDeath1 (FRAME_death101, FRAME_death120, InfantryFramesDeath1, ConvertDerivedFunction(&CInfantry::Dead));

// Off with his head
CFrame InfantryFramesDeath2 [] =
{
	CFrame (&CMonster::AI_Move, 0),
	CFrame (&CMonster::AI_Move, 1),
	CFrame (&CMonster::AI_Move, 5),
	CFrame (&CMonster::AI_Move, -1),
	CFrame (&CMonster::AI_Move, 0),
	CFrame (&CMonster::AI_Move, 1),
	CFrame (&CMonster::AI_Move, 1),
	CFrame (&CMonster::AI_Move, 4),
	CFrame (&CMonster::AI_Move, 3),
	CFrame (&CMonster::AI_Move, 0),
	CFrame (&CMonster::AI_Move, -2,  ConvertDerivedFunction(&CInfantry::MachineGun)),
	CFrame (&CMonster::AI_Move, -2,  ConvertDerivedFunction(&CInfantry::MachineGun)),
	CFrame (&CMonster::AI_Move, -3,  ConvertDerivedFunction(&CInfantry::MachineGun)),
	CFrame (&CMonster::AI_Move, -1,  ConvertDerivedFunction(&CInfantry::MachineGun)),
	CFrame (&CMonster::AI_Move, -2,  ConvertDerivedFunction(&CInfantry::MachineGun)),
	CFrame (&CMonster::AI_Move, 0,   ConvertDerivedFunction(&CInfantry::MachineGun)),
	CFrame (&CMonster::AI_Move, 2,   ConvertDerivedFunction(&CInfantry::MachineGun)),
	CFrame (&CMonster::AI_Move, 2,   ConvertDerivedFunction(&CInfantry::MachineGun)),
	CFrame (&CMonster::AI_Move, 3,   ConvertDerivedFunction(&CInfantry::MachineGun)),
	CFrame (&CMonster::AI_Move, -10, ConvertDerivedFunction(&CInfantry::MachineGun)),
	CFrame (&CMonster::AI_Move, -7,  ConvertDerivedFunction(&CInfantry::MachineGun)),
	CFrame (&CMonster::AI_Move, -8,  ConvertDerivedFunction(&CInfantry::MachineGun)),
	CFrame (&CMonster::AI_Move, -6),
	CFrame (&CMonster::AI_Move, 4),
	CFrame (&CMonster::AI_Move, 0)
};
CAnim InfantryMoveDeath2 (FRAME_death201, FRAME_death225, InfantryFramesDeath2, ConvertDerivedFunction(&CInfantry::Dead));

CFrame InfantryFramesDeath3 [] =
{
	CFrame (&CMonster::AI_Move, 0),
	CFrame (&CMonster::AI_Move, 0),
	CFrame (&CMonster::AI_Move, 0),
	CFrame (&CMonster::AI_Move, -6),
	CFrame (&CMonster::AI_Move, -11),
	CFrame (&CMonster::AI_Move, -3),
	CFrame (&CMonster::AI_Move, -11),
	CFrame (&CMonster::AI_Move, 0),
	CFrame (&CMonster::AI_Move, 0)
};
CAnim InfantryMoveDeath3 (FRAME_death301, FRAME_death309, InfantryFramesDeath3, ConvertDerivedFunction(&CInfantry::Dead));


void CInfantry::Die (edict_t *inflictor, edict_t *attacker, int damage, vec3_t point)
{
// check for gib
	if (Entity->health <= Entity->gib_health)
	{
		Sound (Entity, CHAN_VOICE, SoundIndex ("misc/udeath.wav"));
		for (int n= 0; n < 2; n++)
			ThrowGib (Entity, "models/objects/gibs/bone/tris.md2", damage, GIB_ORGANIC);
		for (int n= 0; n < 4; n++)
			ThrowGib (Entity, "models/objects/gibs/sm_meat/tris.md2", damage, GIB_ORGANIC);
		ThrowHead (Entity, "models/objects/gibs/head2/tris.md2", damage, GIB_ORGANIC);
		Entity->deadflag = DEAD_DEAD;
		return;
	}

	if (Entity->deadflag == DEAD_DEAD)
		return;

// regular death
	Entity->deadflag = DEAD_DEAD;
	Entity->takedamage = DAMAGE_YES;

	CAnim *Animation;
	int pSound;
	switch (rand() % 3)
	{
	case 0:
	default:
		Animation = &InfantryMoveDeath1;
		pSound = SoundDie2;
		break;
	case 1:
		Animation = &InfantryMoveDeath2;
		pSound = SoundDie1;
		break;
	case 2:
		Animation = &InfantryMoveDeath3;
		pSound = SoundDie2;
		break;
	}
	CurrentMove = Animation;
	Sound (Entity, CHAN_VOICE, pSound);
}

void CInfantry::Duck_Down ()
{
	if (AIFlags & AI_DUCKED)
		return;
	AIFlags |= AI_DUCKED;
	Entity->maxs[2] -= 32;
	Entity->takedamage = DAMAGE_YES;
	PauseTime = level.time + 1;
	gi.linkentity (Entity);
}

void CInfantry::Duck_Hold ()
{
	if (level.time >= PauseTime)
		AIFlags &= ~AI_HOLD_FRAME;
	else
		AIFlags |= AI_HOLD_FRAME;
}

void CInfantry::Duck_Up ()
{
	AIFlags &= ~AI_DUCKED;
	Entity->maxs[2] += 32;
	Entity->takedamage = DAMAGE_AIM;
	gi.linkentity (Entity);
}

CFrame InfantryFramesDuck [] =
{
	CFrame (&CMonster::AI_Move, -2, ConvertDerivedFunction(&CInfantry::Duck_Down)),
	CFrame (&CMonster::AI_Move, -5, ConvertDerivedFunction(&CInfantry::Duck_Hold)),
	CFrame (&CMonster::AI_Move, 3),
	CFrame (&CMonster::AI_Move, 4, ConvertDerivedFunction(&CInfantry::Duck_Up)),
	CFrame (&CMonster::AI_Move, 0)
};
CAnim InfantryMoveDuck (FRAME_duck01, FRAME_duck05, InfantryFramesDuck, ConvertDerivedFunction(&CInfantry::Run));

void CInfantry::Dodge (edict_t *attacker, float eta)
{
	if (random() > 0.25)
		return;

	if (!Entity->enemy)
		Entity->enemy = attacker;

	CurrentMove = &InfantryMoveDuck;
}


void CInfantry::CockGun ()
{
	Sound (Entity, CHAN_WEAPON, SoundWeaponCock);
	PauseTime = level.time + ((rand() & 15) + 3 + 7) * FRAMETIME;
}

void CInfantry::Fire ()
{
	MachineGun ();

	if (level.time >= PauseTime)
		AIFlags &= ~AI_HOLD_FRAME;
	else
		AIFlags |= AI_HOLD_FRAME;
}

CFrame InfantryFramesAttack1 [] =
{
	CFrame (&CMonster::AI_Charge, 4),
	CFrame (&CMonster::AI_Charge, -1),
	CFrame (&CMonster::AI_Charge, -1),
	CFrame (&CMonster::AI_Charge, 0, ConvertDerivedFunction(&CInfantry::CockGun)),
	CFrame (&CMonster::AI_Charge, -1),
	CFrame (&CMonster::AI_Charge, 1),
	CFrame (&CMonster::AI_Charge, 1),
	CFrame (&CMonster::AI_Charge, 2),
	CFrame (&CMonster::AI_Charge, -2),
	CFrame (&CMonster::AI_Charge, -3),
	CFrame (&CMonster::AI_Charge, 1, ConvertDerivedFunction(&CInfantry::Fire)),
	CFrame (&CMonster::AI_Charge, 5),
	CFrame (&CMonster::AI_Charge, -1),
	CFrame (&CMonster::AI_Charge, -2),
	CFrame (&CMonster::AI_Charge, -3)
};
CAnim InfantryMoveAttack1 (FRAME_attak101, FRAME_attak115, InfantryFramesAttack1, ConvertDerivedFunction(&CInfantry::Run));


void CInfantry::Swing ()
{
	Sound (Entity, CHAN_WEAPON, SoundPunchSwing);
}

void CInfantry::Smack ()
{
	vec3_t	aim = {80, 0, 0};
	if (fire_hit (Entity, aim, (5 + (rand() % 5)), 50))
		Sound (Entity, CHAN_WEAPON, SoundPunchHit);
}

CFrame InfantryFramesAttack2 [] =
{
	CFrame (&CMonster::AI_Charge, 3),
	CFrame (&CMonster::AI_Charge, 6),
	CFrame (&CMonster::AI_Charge, 0, ConvertDerivedFunction(&CInfantry::Swing)),
	CFrame (&CMonster::AI_Charge, 8),
	CFrame (&CMonster::AI_Charge, 5),
	CFrame (&CMonster::AI_Charge, 8, ConvertDerivedFunction(&CInfantry::Smack)),
	CFrame (&CMonster::AI_Charge, 6),
	CFrame (&CMonster::AI_Charge, 3),
};
CAnim InfantryMoveAttack2 (FRAME_attak201, FRAME_attak208, InfantryFramesAttack2, ConvertDerivedFunction(&CInfantry::Run));

void CInfantry::Attack ()
{
	CurrentMove = &InfantryMoveAttack1;
}

void CInfantry::Melee ()
{
	CurrentMove = &InfantryMoveAttack2;
}

void CInfantry::Spawn ()
{
	Entity->movetype = MOVETYPE_STEP;
	Entity->solid = SOLID_BBOX;
	Entity->s.modelIndex = ModelIndex("models/monsters/infantry/tris.md2");
	Vec3Set (Entity->mins, -16, -16, -24);
	Vec3Set (Entity->maxs, 16, 16, 32);

	SoundPain1 = SoundIndex ("infantry/infpain1.wav");
	SoundPain2 = SoundIndex ("infantry/infpain2.wav");
	SoundDie1 = SoundIndex ("infantry/infdeth1.wav");
	SoundDie2 = SoundIndex ("infantry/infdeth2.wav");

	SoundGunshot = SoundIndex ("infantry/infatck1.wav");
	SoundWeaponCock = SoundIndex ("infantry/infatck3.wav");
	SoundPunchSwing = SoundIndex ("infantry/infatck2.wav");
	SoundPunchHit = SoundIndex ("infantry/melee2.wav");
	
	SoundSight = SoundIndex ("infantry/infsght1.wav");
	SoundSearch = SoundIndex ("infantry/infsrch1.wav");
	SoundIdle = SoundIndex ("infantry/infidle1.wav");

	Entity->health = 100;
	Entity->gib_health = -40;
	Entity->mass = 200;

	MonsterFlags = (MF_HAS_MELEE | MF_HAS_ATTACK | MF_HAS_IDLE | MF_HAS_SIGHT);

	gi.linkentity (Entity);

	CurrentMove = &InfantryMoveStand;
	WalkMonsterStart ();
}