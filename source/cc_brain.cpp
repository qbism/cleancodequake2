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
// cc_brain.cpp
// 
//

#include "cc_local.h"
#include "cc_brain.h"
#include "m_brain.h"

CBrain::CBrain (uint32 ID) :
CMonster (ID)
{
	Scale = MODEL_SCALE;
	MonsterName = "Brain";
}

void CBrain::Sight ()
{
	Entity->PlaySound (CHAN_VOICE, SoundSight);
}

void CBrain::Search ()
{
	Entity->PlaySound (CHAN_VOICE, SoundSearch);
}

//
// STAND
//

CFrame BrainFramesStand [] =
{
	CFrame (&CMonster::AI_Stand,	0),
	CFrame (&CMonster::AI_Stand,	0),
	CFrame (&CMonster::AI_Stand,	0),
	CFrame (&CMonster::AI_Stand,	0),
	CFrame (&CMonster::AI_Stand,	0),
	CFrame (&CMonster::AI_Stand,	0),
	CFrame (&CMonster::AI_Stand,	0),
	CFrame (&CMonster::AI_Stand,	0),
	CFrame (&CMonster::AI_Stand,	0),
	CFrame (&CMonster::AI_Stand,	0),

	CFrame (&CMonster::AI_Stand,	0),
	CFrame (&CMonster::AI_Stand,	0),
	CFrame (&CMonster::AI_Stand,	0),
	CFrame (&CMonster::AI_Stand,	0),
	CFrame (&CMonster::AI_Stand,	0),
	CFrame (&CMonster::AI_Stand,	0),
	CFrame (&CMonster::AI_Stand,	0),
	CFrame (&CMonster::AI_Stand,	0),
	CFrame (&CMonster::AI_Stand,	0),
	CFrame (&CMonster::AI_Stand,	0),

	CFrame (&CMonster::AI_Stand,	0),
	CFrame (&CMonster::AI_Stand,	0),
	CFrame (&CMonster::AI_Stand,	0),
	CFrame (&CMonster::AI_Stand,	0),
	CFrame (&CMonster::AI_Stand,	0),
	CFrame (&CMonster::AI_Stand,	0),
	CFrame (&CMonster::AI_Stand,	0),
	CFrame (&CMonster::AI_Stand,	0),
	CFrame (&CMonster::AI_Stand,	0),
	CFrame (&CMonster::AI_Stand,	0)
};
CAnim BrainMoveStand (FRAME_stand01, FRAME_stand30, BrainFramesStand);

void CBrain::Stand ()
{
	CurrentMove = &BrainMoveStand;
}


//
// IDLE
//

CFrame BrainFramesIdle [] =
{
	CFrame (&CMonster::AI_Stand,	0),
	CFrame (&CMonster::AI_Stand,	0),
	CFrame (&CMonster::AI_Stand,	0),
	CFrame (&CMonster::AI_Stand,	0),
	CFrame (&CMonster::AI_Stand,	0),
	CFrame (&CMonster::AI_Stand,	0),
	CFrame (&CMonster::AI_Stand,	0),
	CFrame (&CMonster::AI_Stand,	0),
	CFrame (&CMonster::AI_Stand,	0),
	CFrame (&CMonster::AI_Stand,	0),

	CFrame (&CMonster::AI_Stand,	0),
	CFrame (&CMonster::AI_Stand,	0),
	CFrame (&CMonster::AI_Stand,	0),
	CFrame (&CMonster::AI_Stand,	0),
	CFrame (&CMonster::AI_Stand,	0),
	CFrame (&CMonster::AI_Stand,	0),
	CFrame (&CMonster::AI_Stand,	0),
	CFrame (&CMonster::AI_Stand,	0),
	CFrame (&CMonster::AI_Stand,	0),
	CFrame (&CMonster::AI_Stand,	0),

	CFrame (&CMonster::AI_Stand,	0),
	CFrame (&CMonster::AI_Stand,	0),
	CFrame (&CMonster::AI_Stand,	0),
	CFrame (&CMonster::AI_Stand,	0),
	CFrame (&CMonster::AI_Stand,	0),
	CFrame (&CMonster::AI_Stand,	0),
	CFrame (&CMonster::AI_Stand,	0),
	CFrame (&CMonster::AI_Stand,	0),
	CFrame (&CMonster::AI_Stand,	0),
	CFrame (&CMonster::AI_Stand,	0)
};
CAnim BrainMoveIdle (FRAME_stand31, FRAME_stand60, BrainFramesIdle, &CMonster::Stand);

void CBrain::Idle ()
{
	Entity->PlaySound (CHAN_AUTO, SoundIdle3, 255, ATTN_IDLE);
	CurrentMove = &BrainMoveIdle;
}

//
// WALK
//
CFrame BrainFramesWalk1 [] =
{
	CFrame (&CMonster::AI_Walk,	7),
	CFrame (&CMonster::AI_Walk,	2),
	CFrame (&CMonster::AI_Walk,	3),
	CFrame (&CMonster::AI_Walk,	3),
	CFrame (&CMonster::AI_Walk,	1),
	CFrame (&CMonster::AI_Walk,	0),
	CFrame (&CMonster::AI_Walk,	0),
	CFrame (&CMonster::AI_Walk,	9),
	CFrame (&CMonster::AI_Walk,	-4),
	CFrame (&CMonster::AI_Walk,	-1),
	CFrame (&CMonster::AI_Walk,	2)
};
CAnim BrainMoveWalk1 (FRAME_walk101, FRAME_walk111, BrainFramesWalk1);

// walk2 is FUBAR, do not use
#if 0
void brain_walk2_cycle (edict_t *self)
{
	if (frand() > 0.1)
		self->monsterinfo.nextframe = FRAME_walk220;
}

CFrame brain_frames_walk2 [] =
{
	CFrame (&CMonster::AI_Walk,	3),
	CFrame (&CMonster::AI_Walk,	-2),
	CFrame (&CMonster::AI_Walk,	-4),
	CFrame (&CMonster::AI_Walk,	-3),
	CFrame (&CMonster::AI_Walk,	0),
	CFrame (&CMonster::AI_Walk,	1),
	CFrame (&CMonster::AI_Walk,	12),
	CFrame (&CMonster::AI_Walk,	0),
	CFrame (&CMonster::AI_Walk,	-3),
	CFrame (&CMonster::AI_Walk,	0),

	CFrame (&CMonster::AI_Walk,	-2),
	CFrame (&CMonster::AI_Walk,	0),
	CFrame (&CMonster::AI_Walk,	0),
	CFrame (&CMonster::AI_Walk,	1),
	CFrame (&CMonster::AI_Walk,	0),
	CFrame (&CMonster::AI_Walk,	0),
	CFrame (&CMonster::AI_Walk,	0),
	CFrame (&CMonster::AI_Walk,	0),
	CFrame (&CMonster::AI_Walk,	0),
	CFrame (&CMonster::AI_Walk,	10),		// Cycle Start

	CFrame (&CMonster::AI_Walk,	-1),
	CFrame (&CMonster::AI_Walk,	7),
	CFrame (&CMonster::AI_Walk,	0),
	CFrame (&CMonster::AI_Walk,	3),
	CFrame (&CMonster::AI_Walk,	-3),
	CFrame (&CMonster::AI_Walk,	2),
	CFrame (&CMonster::AI_Walk,	4),
	CFrame (&CMonster::AI_Walk,	-3),
	CFrame (&CMonster::AI_Walk,	2),
	CFrame (&CMonster::AI_Walk,	0),

	CFrame (&CMonster::AI_Walk,	4,	brain_walk2_cycle,
	CFrame (&CMonster::AI_Walk,	-1),
	CFrame (&CMonster::AI_Walk,	-1),
	CFrame (&CMonster::AI_Walk,	-8),		
	CFrame (&CMonster::AI_Walk,	0),
	CFrame (&CMonster::AI_Walk,	1),
	CFrame (&CMonster::AI_Walk,	5),
	CFrame (&CMonster::AI_Walk,	2),
	CFrame (&CMonster::AI_Walk,	-1),
	CFrame (&CMonster::AI_Walk,	-5,	NULL
};
CAnim brain_move_walk2 = {FRAME_walk201, FRAME_walk240, brain_frames_walk2, NULL};
#endif

void CBrain::Walk ()
{
	//CurrentMove = (frand() <= 0.5) ? &BrainMoveWalk1 : &BrainMoveWalk2;
	CurrentMove = &BrainMoveWalk1;
}

CFrame BrainFramesDefense [] =
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
CAnim BrainMoveDefense (FRAME_defens01, FRAME_defens08, BrainFramesDefense);

CFrame BrainFramesPain3 [] =
{
	CFrame (&CMonster::AI_Move,	-2),
	CFrame (&CMonster::AI_Move,	2),
	CFrame (&CMonster::AI_Move,	1),
	CFrame (&CMonster::AI_Move,	3),
	CFrame (&CMonster::AI_Move,	0),
	CFrame (&CMonster::AI_Move,	-4)
};
CAnim BrainMovePain3 (FRAME_pain301, FRAME_pain306, BrainFramesPain3, &CMonster::Run);

CFrame BrainFramesPain2 [] =
{
	CFrame (&CMonster::AI_Move,	-2),
	CFrame (&CMonster::AI_Move,	0),
	CFrame (&CMonster::AI_Move,	0),
	CFrame (&CMonster::AI_Move,	0),
	CFrame (&CMonster::AI_Move,	0),
	CFrame (&CMonster::AI_Move,	3),
	CFrame (&CMonster::AI_Move,	1),
	CFrame (&CMonster::AI_Move,	-2)
};
CAnim BrainMovePain2 (FRAME_pain201, FRAME_pain208, BrainFramesPain2, &CMonster::Run);

CFrame BrainFramesPain1 [] =
{
	CFrame (&CMonster::AI_Move,	-6),
	CFrame (&CMonster::AI_Move,	-2),
	CFrame (&CMonster::AI_Move,	-6),
	CFrame (&CMonster::AI_Move,	0),
	CFrame (&CMonster::AI_Move,	0),
	CFrame (&CMonster::AI_Move,	0),
	CFrame (&CMonster::AI_Move,	0),
	CFrame (&CMonster::AI_Move,	0),
	CFrame (&CMonster::AI_Move,	0),
	CFrame (&CMonster::AI_Move,	0),
	CFrame (&CMonster::AI_Move,	0),
	CFrame (&CMonster::AI_Move,	0),
	CFrame (&CMonster::AI_Move,	0),
	CFrame (&CMonster::AI_Move,	2),
	CFrame (&CMonster::AI_Move,	0),
	CFrame (&CMonster::AI_Move,	2),
	CFrame (&CMonster::AI_Move,	1),
	CFrame (&CMonster::AI_Move,	7),
	CFrame (&CMonster::AI_Move,	0),
	CFrame (&CMonster::AI_Move,	3),
	CFrame (&CMonster::AI_Move,	-1)
};
CAnim BrainMovePain1 (FRAME_pain101, FRAME_pain121, BrainFramesPain1, &CMonster::Run);

void CBrain::Pain(CBaseEntity *other, float kick, sint32 damage)
{
	if (Entity->Health < (Entity->MaxHealth / 2))
		Entity->State.GetSkinNum() = 1;

	if (level.Frame < PainDebounceTime)
		return;

	PainDebounceTime = level.Frame + 30;
	if (skill->Integer() == 3)
		return;		// no pain anims in nightmare

	switch (irandom(3))
	{
	case 0:
		Entity->PlaySound (CHAN_VOICE, SoundPain1);
		CurrentMove = &BrainMovePain1;
		break;
	case 1:
		Entity->PlaySound (CHAN_VOICE, SoundPain2);
		CurrentMove = &BrainMovePain2;
		break;
	case 2:
		Entity->PlaySound (CHAN_VOICE, SoundPain1);
		CurrentMove = &BrainMovePain3;
		break;
	}
}

//
// DUCK
//

CFrame BrainFramesDuck [] =
{
	CFrame (&CMonster::AI_Move,	0),
#ifndef MONSTER_USE_ROGUE_AI
	CFrame (&CMonster::AI_Move,	-2,	ConvertDerivedFunction(&CBrain::Duck_Down)),
	CFrame (&CMonster::AI_Move,	17,	ConvertDerivedFunction(&CBrain::Duck_Hold)),
	CFrame (&CMonster::AI_Move,	-3),
	CFrame (&CMonster::AI_Move,	-1,	ConvertDerivedFunction(&CBrain::Duck_Up)),
#else
	CFrame (&CMonster::AI_Move,	-2,	&CMonster::DuckDown),
	CFrame (&CMonster::AI_Move,	17,	&CMonster::DuckHold),
	CFrame (&CMonster::AI_Move,	-3),
	CFrame (&CMonster::AI_Move,	-1,	&CMonster::UnDuck),
#endif
	CFrame (&CMonster::AI_Move,	-5),
	CFrame (&CMonster::AI_Move,	-6),
	CFrame (&CMonster::AI_Move,	-6)
};
CAnim BrainMoveDuck (FRAME_duck01, FRAME_duck08, BrainFramesDuck, &CMonster::Run);

#ifndef MONSTER_USE_ROGUE_AI
void CBrain::Duck_Down ()
{
	if (AIFlags & AI_DUCKED)
		return;
	AIFlags |= AI_DUCKED;
	Entity->GetMins() -= vec3f(0, 0, 32);
	Entity->Link ();
}

void CBrain::Duck_Hold ()
{
	if (level.Frame >= PauseTime)
		AIFlags &= ~AI_HOLD_FRAME;
	else
		AIFlags |= AI_HOLD_FRAME;
}

void CBrain::Duck_Up ()
{
	AIFlags &= ~AI_DUCKED;
	Entity->GetMins() += vec3f(0, 0, 32);
	Entity->Link ();
}

void CBrain::Dodge (CBaseEntity *attacker, float eta)
{
	if (frand() > 0.25f)
		return;

	if (!Entity->Enemy)
		Entity->Enemy = attacker->gameEntity;

	PauseTime = level.Frame + ((eta + 0.5) * 10);
	CurrentMove = &BrainMoveDuck;
}
#else
void CBrain::Duck (float eta)
{
	// has to be done immediately otherwise he can get stuck
	DuckDown ();

	if (!skill->Boolean())
		// PMM - stupid dodge
		DuckWaitTime = level.Frame + ((eta + 1) * 10);
	else
		DuckWaitTime = level.Frame + ((eta + (0.1 * (3 - skill->Integer()))) * 10);

	CurrentMove = &BrainMoveDuck;
	NextFrame = FRAME_duck01;
	return;
}
#endif

CFrame BrainFramesDeath2 [] =
{
	CFrame (&CMonster::AI_Move,	0),
	CFrame (&CMonster::AI_Move,	0),
	CFrame (&CMonster::AI_Move,	0),
	CFrame (&CMonster::AI_Move,	9),
	CFrame (&CMonster::AI_Move,	0)
};
CAnim BrainMoveDeath2 (FRAME_death201, FRAME_death205, BrainFramesDeath2, ConvertDerivedFunction(&CBrain::Dead));

CFrame BrainFramesDeath1 [] =
{
	CFrame (&CMonster::AI_Move,	0),
	CFrame (&CMonster::AI_Move,	0),
	CFrame (&CMonster::AI_Move,	-2),
	CFrame (&CMonster::AI_Move,	9),
	CFrame (&CMonster::AI_Move,	0),
	CFrame (&CMonster::AI_Move,	0),
	CFrame (&CMonster::AI_Move,	0),
	CFrame (&CMonster::AI_Move,	0),
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
CAnim BrainMoveDeath1 (FRAME_death101, FRAME_death118, BrainFramesDeath1, ConvertDerivedFunction(&CBrain::Dead));

void CBrain::Dead ()
{
	Entity->GetMins().Set (-16, -16, -24);
	Entity->GetMaxs().Set (16, 16, -8);
	Entity->PhysicsType = PHYSICS_TOSS;
	Entity->GetSvFlags() |= SVF_DEADMONSTER;
	Entity->NextThink = 0;
	Entity->Link ();
}

void CBrain::Die (CBaseEntity *inflictor, CBaseEntity *attacker, sint32 damage, vec3f &point)
{
	Entity->State.GetEffects() = 0;
	PowerArmorType = POWER_ARMOR_NONE;

// check for gib
	if (Entity->Health <= Entity->GibHealth)
	{
		Entity->PlaySound (CHAN_VOICE, SoundIndex ("misc/udeath.wav"));
		for (sint32 n = 0; n < 2; n++)
			CGibEntity::Spawn (Entity, GameMedia.Gib_Bone[0], damage, GIB_ORGANIC);
		for (sint32 n = 0; n < 4; n++)
			CGibEntity::Spawn (Entity, GameMedia.Gib_SmallMeat, damage, GIB_ORGANIC);
		Entity->ThrowHead (GameMedia.Gib_Head[1], damage, GIB_ORGANIC);
		return;
	}

	if (Entity->DeadFlag)
		return;

// regular death
	Entity->PlaySound (CHAN_VOICE, SoundDeath);
	Entity->DeadFlag = true;
	CurrentMove = (frand() <= 0.5) ? &BrainMoveDeath1 : &BrainMoveDeath2;
}

//
// MELEE
//

void CBrain::SwingRight ()
{
	Entity->PlaySound (CHAN_BODY, SoundMelee1);
}

void CBrain::HitRight ()
{
	const vec3f	aim (MELEE_DISTANCE, Entity->GetMaxs().X, 8);

	if (CMeleeWeapon::Fire (Entity, aim, (15 + (irandom(5))), 40))
		Entity->PlaySound (CHAN_WEAPON, SoundMelee3);
}

void CBrain::SwingLeft ()
{
	Entity->PlaySound (CHAN_BODY, SoundMelee2);
}

void CBrain::HitLeft ()
{
	const vec3f	aim (MELEE_DISTANCE, Entity->GetMins().X, 8);

	if (CMeleeWeapon::Fire (Entity, aim, (15 + (irandom(5))), 40))
		Entity->PlaySound (CHAN_WEAPON, SoundMelee3);
}

CFrame BrainFramesAttack1 [] =
{
	CFrame (&CMonster::AI_Charge,	8),
	CFrame (&CMonster::AI_Charge,	3),
	CFrame (&CMonster::AI_Charge,	5),
	CFrame (&CMonster::AI_Charge,	0),
	CFrame (&CMonster::AI_Charge,	-3,	ConvertDerivedFunction(&CBrain::SwingRight)),
	CFrame (&CMonster::AI_Charge,	0),
	CFrame (&CMonster::AI_Charge,	-5),
	CFrame (&CMonster::AI_Charge,	-7,	ConvertDerivedFunction(&CBrain::HitRight)),
	CFrame (&CMonster::AI_Charge,	0),
	CFrame (&CMonster::AI_Charge,	6,	ConvertDerivedFunction(&CBrain::SwingLeft)),
	CFrame (&CMonster::AI_Charge,	1),
	CFrame (&CMonster::AI_Charge,	2,	ConvertDerivedFunction(&CBrain::HitLeft)),
	CFrame (&CMonster::AI_Charge,	-3),
	CFrame (&CMonster::AI_Charge,	6),
	CFrame (&CMonster::AI_Charge,	-1),
	CFrame (&CMonster::AI_Charge,	-3),
	CFrame (&CMonster::AI_Charge,	2),
	CFrame (&CMonster::AI_Charge,	-11)
};
CAnim BrainMoveAttack1 (FRAME_attak101, FRAME_attak118, BrainFramesAttack1, &CMonster::Run);

void CBrain::ChestOpen ()
{
	Refire = false;
	PowerArmorType = POWER_ARMOR_NONE;
	Entity->PlaySound (CHAN_BODY, SoundChestOpen);
}

void CBrain::TentacleAttack ()
{
	static const vec3f aim (MELEE_DISTANCE, 0, 8);

	if (CMeleeWeapon::Fire (Entity, aim, (10 + (irandom(5))), -600) && (skill->Boolean()))
		Refire = true;
	Entity->PlaySound (CHAN_WEAPON, SoundTentaclesRetract);
}

void CBrain::ChestClosed ()
{
	PowerArmorType = POWER_ARMOR_SCREEN;
	if (Refire)
	{
		Refire = false;
		CurrentMove = &BrainMoveAttack1;
	}
}

CFrame BrainFramesAttack2 [] =
{
	CFrame (&CMonster::AI_Charge,	5),
	CFrame (&CMonster::AI_Charge,	-4),
	CFrame (&CMonster::AI_Charge,	-4),
	CFrame (&CMonster::AI_Charge,	-3),
	CFrame (&CMonster::AI_Charge,	0,	ConvertDerivedFunction(&CBrain::ChestOpen)),
	CFrame (&CMonster::AI_Charge,	0),
	CFrame (&CMonster::AI_Charge,	13,	ConvertDerivedFunction(&CBrain::TentacleAttack)),
	CFrame (&CMonster::AI_Charge,	0),
	CFrame (&CMonster::AI_Charge,	2),
	CFrame (&CMonster::AI_Charge,	0),
	CFrame (&CMonster::AI_Charge,	-9,	ConvertDerivedFunction(&CBrain::ChestClosed)),
	CFrame (&CMonster::AI_Charge,	0),
	CFrame (&CMonster::AI_Charge,	4),
	CFrame (&CMonster::AI_Charge,	3),
	CFrame (&CMonster::AI_Charge,	2),
	CFrame (&CMonster::AI_Charge,	-3),
	CFrame (&CMonster::AI_Charge,	-6)
};
CAnim BrainMoveAttack2 (FRAME_attak201, FRAME_attak217, BrainFramesAttack2, &CMonster::Run);

void CBrain::Melee ()
{
	CurrentMove = (frand() <= 0.5) ? &BrainMoveAttack1 : &BrainMoveAttack2;
}


//
// RUN
//

CFrame BrainFramesRun [] =
{
	CFrame (&CMonster::AI_Run,	9),
	CFrame (&CMonster::AI_Run,	2),
	CFrame (&CMonster::AI_Run,	3),
	CFrame (&CMonster::AI_Run,	3),
	CFrame (&CMonster::AI_Run,	1),
	CFrame (&CMonster::AI_Run,	0),
	CFrame (&CMonster::AI_Run,	0),
	CFrame (&CMonster::AI_Run,	10),
	CFrame (&CMonster::AI_Run,	-4),
	CFrame (&CMonster::AI_Run,	-1),
	CFrame (&CMonster::AI_Run,	2)
};
CAnim BrainMoveRun (FRAME_walk101, FRAME_walk111, BrainFramesRun);

void CBrain::Run ()
{
	PowerArmorType = POWER_ARMOR_SCREEN;
	CurrentMove = (AIFlags & AI_STAND_GROUND) ? &BrainMoveStand : &BrainMoveRun;
}

/*QUAKED monster_brain (1 .5 0) (-16 -16 -24) (16 16 32) Ambush Trigger_Spawn Sight
*/
void CBrain::Spawn ()
{
	SoundChestOpen = SoundIndex ("brain/brnatck1.wav");
	SoundTentaclesExtend = SoundIndex ("brain/brnatck2.wav");
	SoundTentaclesRetract = SoundIndex ("brain/brnatck3.wav");
	SoundDeath = SoundIndex ("brain/brndeth1.wav");
	SoundIdle1 = SoundIndex ("brain/brnidle1.wav");
	SoundIdle2 = SoundIndex ("brain/brnidle2.wav");
	SoundIdle3 = SoundIndex ("brain/brnlens1.wav");
	SoundPain1 = SoundIndex ("brain/brnpain1.wav");
	SoundPain2 = SoundIndex ("brain/brnpain2.wav");
	SoundSight = SoundIndex ("brain/brnsght1.wav");
	SoundSearch = SoundIndex ("brain/brnsrch1.wav");
	SoundMelee1 = SoundIndex ("brain/melee1.wav");
	SoundMelee2 = SoundIndex ("brain/melee2.wav");
	SoundMelee3 = SoundIndex ("brain/melee3.wav");

	Entity->PhysicsType = PHYSICS_STEP;
	Entity->GetSolid() = SOLID_BBOX;
	Entity->State.GetModelIndex() = ModelIndex ("models/monsters/brain/tris.md2");
	Entity->GetMins().Set (-16, -16, -24);
	Entity->GetMaxs().Set (16, 16, 32);

	Entity->Health = 300;
	Entity->GibHealth = -150;
	Entity->Mass = 400;

	MonsterFlags |= (MF_HAS_MELEE | MF_HAS_SIGHT | MF_HAS_SEARCH | MF_HAS_IDLE
#ifdef MONSTER_USE_ROGUE_AI
		| MF_HAS_DODGE | MF_HAS_DUCK | MF_HAS_UNDUCK
#endif
		);

	PowerArmorType = POWER_ARMOR_SCREEN;
	PowerArmorPower = 100;

	Entity->Link ();

	CurrentMove = &BrainMoveStand;	
	WalkMonsterStart ();
}

LINK_MONSTER_CLASSNAME_TO_CLASS ("monster_brain", CBrain);