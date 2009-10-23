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
// cc_barracuda.cpp
// Sharky!
//

#include "cc_local.h"
#include "m_flipper.h"
#include "cc_barracuda.h"

CBarracudaShark::CBarracudaShark (uint32 ID) :
CMonster (ID)
{
	Scale = MODEL_SCALE;
	MonsterName = "Shark";
}

CFrame FlipperFramesStand [] =
{
	CFrame (&CMonster::AI_Stand, 0)
};
CAnim FlipperMoveStand (FRAME_flphor01, FRAME_flphor01, FlipperFramesStand);

void CBarracudaShark::Stand ()
{
	CurrentMove = &FlipperMoveStand;
}

#define FLIPPER_RUN_SPEED	24

CFrame FlipperFramesRun [] =
{
	CFrame (&CMonster::AI_Run, FLIPPER_RUN_SPEED),	// 6
	CFrame (&CMonster::AI_Run, FLIPPER_RUN_SPEED),
	CFrame (&CMonster::AI_Run, FLIPPER_RUN_SPEED),
	CFrame (&CMonster::AI_Run, FLIPPER_RUN_SPEED),
	CFrame (&CMonster::AI_Run, FLIPPER_RUN_SPEED),	// 10

	CFrame (&CMonster::AI_Run, FLIPPER_RUN_SPEED),
	CFrame (&CMonster::AI_Run, FLIPPER_RUN_SPEED),
	CFrame (&CMonster::AI_Run, FLIPPER_RUN_SPEED),
	CFrame (&CMonster::AI_Run, FLIPPER_RUN_SPEED),
	CFrame (&CMonster::AI_Run, FLIPPER_RUN_SPEED),
	CFrame (&CMonster::AI_Run, FLIPPER_RUN_SPEED),
	CFrame (&CMonster::AI_Run, FLIPPER_RUN_SPEED),
	CFrame (&CMonster::AI_Run, FLIPPER_RUN_SPEED),
	CFrame (&CMonster::AI_Run, FLIPPER_RUN_SPEED),
	CFrame (&CMonster::AI_Run, FLIPPER_RUN_SPEED),	// 20

	CFrame (&CMonster::AI_Run, FLIPPER_RUN_SPEED),
	CFrame (&CMonster::AI_Run, FLIPPER_RUN_SPEED),
	CFrame (&CMonster::AI_Run, FLIPPER_RUN_SPEED),
	CFrame (&CMonster::AI_Run, FLIPPER_RUN_SPEED),
	CFrame (&CMonster::AI_Run, FLIPPER_RUN_SPEED),
	CFrame (&CMonster::AI_Run, FLIPPER_RUN_SPEED),
	CFrame (&CMonster::AI_Run, FLIPPER_RUN_SPEED),
	CFrame (&CMonster::AI_Run, FLIPPER_RUN_SPEED),
	CFrame (&CMonster::AI_Run, FLIPPER_RUN_SPEED)		// 29
};
CAnim FlipperMoveRunLoop (FRAME_flpver06, FRAME_flpver29, FlipperFramesRun);

void CBarracudaShark::RunLoop ()
{
	CurrentMove = &FlipperMoveRunLoop;
}

CFrame FlipperFramesRunStart [] =
{
	CFrame (&CMonster::AI_Run, 8),
	CFrame (&CMonster::AI_Run, 8),
	CFrame (&CMonster::AI_Run, 8),
	CFrame (&CMonster::AI_Run, 8),
	CFrame (&CMonster::AI_Run, 8),
	CFrame (&CMonster::AI_Run, 8)
};
CAnim FlipperMoveRunStart (FRAME_flpver01, FRAME_flpver06, FlipperFramesRunStart, ConvertDerivedFunction(&CBarracudaShark::RunLoop));

void CBarracudaShark::DoRun ()
{
	CurrentMove = &FlipperMoveRunStart;
}

/* Standard Swimming */ 
CFrame FlipperFramesWalk [] =
{
	CFrame (&CMonster::AI_Walk, 4),
	CFrame (&CMonster::AI_Walk, 4),
	CFrame (&CMonster::AI_Walk, 4),
	CFrame (&CMonster::AI_Walk, 4),
	CFrame (&CMonster::AI_Walk, 4),
	CFrame (&CMonster::AI_Walk, 4),
	CFrame (&CMonster::AI_Walk, 4),
	CFrame (&CMonster::AI_Walk, 4),
	CFrame (&CMonster::AI_Walk, 4),
	CFrame (&CMonster::AI_Walk, 4),
	CFrame (&CMonster::AI_Walk, 4),
	CFrame (&CMonster::AI_Walk, 4),
	CFrame (&CMonster::AI_Walk, 4),
	CFrame (&CMonster::AI_Walk, 4),
	CFrame (&CMonster::AI_Walk, 4),
	CFrame (&CMonster::AI_Walk, 4),
	CFrame (&CMonster::AI_Walk, 4),
	CFrame (&CMonster::AI_Walk, 4),
	CFrame (&CMonster::AI_Walk, 4),
	CFrame (&CMonster::AI_Walk, 4),
	CFrame (&CMonster::AI_Walk, 4),
	CFrame (&CMonster::AI_Walk, 4),
	CFrame (&CMonster::AI_Walk, 4),
	CFrame (&CMonster::AI_Walk, 4)
};
CAnim FlipperMoveWalk (FRAME_flphor01, FRAME_flphor24, FlipperFramesWalk);

void CBarracudaShark::Walk ()
{
	CurrentMove = &FlipperMoveWalk;
}

CFrame FlipperFramesStartRun [] =
{
	CFrame (&CMonster::AI_Run, 8),
	CFrame (&CMonster::AI_Run, 8),
	CFrame (&CMonster::AI_Run, 8),
	CFrame (&CMonster::AI_Run, 8),
	CFrame (&CMonster::AI_Run, 8)
};
CAnim FlipperMoveStartRun (FRAME_flphor01, FRAME_flphor05, FlipperFramesStartRun, ConvertDerivedFunction(&CBarracudaShark::DoRun));

void CBarracudaShark::Run ()
{
	CurrentMove = &FlipperMoveStartRun;
}

CFrame FlipperFramesPain2 [] =
{
	CFrame (&CMonster::AI_Move, 0),
	CFrame (&CMonster::AI_Move, 0),
	CFrame (&CMonster::AI_Move, 0),
	CFrame (&CMonster::AI_Move, 0),
	CFrame (&CMonster::AI_Move, 0)
};
CAnim FlipperMovePain2 (FRAME_flppn101, FRAME_flppn105, FlipperFramesPain2, &CMonster::Run);

CFrame FlipperFramesPain1 [] =
{
	CFrame (&CMonster::AI_Move, 0),
	CFrame (&CMonster::AI_Move, 0),
	CFrame (&CMonster::AI_Move, 0),
	CFrame (&CMonster::AI_Move, 0),
	CFrame (&CMonster::AI_Move, 0)
};
CAnim FlipperMovePain1 (FRAME_flppn201, FRAME_flppn205, FlipperFramesPain1, &CMonster::Run);

void CBarracudaShark::Bite ()
{
	static vec3f	aim (MELEE_DISTANCE, 0, 0);
	CMeleeWeapon::Fire (Entity, aim, 5, 0);
}

void CBarracudaShark::PreAttack ()
{
	Entity->PlaySound (CHAN_WEAPON, SoundChomp);
}

CFrame FlipperFramesAttack [] =
{
	CFrame (&CMonster::AI_Charge, 0, ConvertDerivedFunction(&CBarracudaShark::PreAttack)),
	CFrame (&CMonster::AI_Charge, 0),
	CFrame (&CMonster::AI_Charge, 0),
	CFrame (&CMonster::AI_Charge, 0),
	CFrame (&CMonster::AI_Charge, 0),
	CFrame (&CMonster::AI_Charge, 0),
	CFrame (&CMonster::AI_Charge, 0),
	CFrame (&CMonster::AI_Charge, 0),
	CFrame (&CMonster::AI_Charge, 0),
	CFrame (&CMonster::AI_Charge, 0),
	CFrame (&CMonster::AI_Charge, 0),
	CFrame (&CMonster::AI_Charge, 0),
	CFrame (&CMonster::AI_Charge, 0),
	CFrame (&CMonster::AI_Charge, 0, ConvertDerivedFunction(&CBarracudaShark::Bite)),
	CFrame (&CMonster::AI_Charge, 0),
	CFrame (&CMonster::AI_Charge, 0),
	CFrame (&CMonster::AI_Charge, 0),
	CFrame (&CMonster::AI_Charge, 0),
	CFrame (&CMonster::AI_Charge, 0, ConvertDerivedFunction(&CBarracudaShark::Bite)),
	CFrame (&CMonster::AI_Charge, 0)
};
CAnim FlipperMoveAttack (FRAME_flpbit01, FRAME_flpbit20, FlipperFramesAttack, &CMonster::Run);

void CBarracudaShark::Melee()
{
	CurrentMove = &FlipperMoveAttack;
}

void CBarracudaShark::Pain (CBaseEntity *other, float kick, int damage)
{
	if (Entity->Health < (Entity->MaxHealth / 2))
		Entity->State.GetSkinNum() = 1;

	if (level.Frame < PainDebounceTime)
		return;

	PainDebounceTime = level.Frame + 30;
	
	if (skill->Integer() == 3)
		return;		// no pain anims in nightmare

	switch (irandom(2))
	{
	case 0:
		Entity->PlaySound (CHAN_VOICE, SoundPain1);
		CurrentMove = &FlipperMovePain1;
		break;
	case 1:
		Entity->PlaySound (CHAN_VOICE, SoundPain2);
		CurrentMove = &FlipperMovePain2;
		break;
	}
}

void CBarracudaShark::Dead ()
{
	Entity->GetMins().Set (-16, -16, -24);
	Entity->GetMaxs().Set (16, 16, -8);
	Entity->PhysicsType = PHYSICS_TOSS;
	Entity->GetSvFlags() |= SVF_DEADMONSTER;
	Entity->NextThink = 0;
	Entity->Link ();
}

CFrame FlipperFramesDeath [] =
{
	CFrame (&CMonster::AI_Move, 0),
	CFrame (&CMonster::AI_Move, 0),
	CFrame (&CMonster::AI_Move, 0),
	CFrame (&CMonster::AI_Move, 0),
	CFrame (&CMonster::AI_Move, 0),
	CFrame (&CMonster::AI_Move, 0),
	CFrame (&CMonster::AI_Move, 0),
	CFrame (&CMonster::AI_Move, 0),
	CFrame (&CMonster::AI_Move, 0),
	CFrame (&CMonster::AI_Move, 0),

	CFrame (&CMonster::AI_Move, 0),
	CFrame (&CMonster::AI_Move, 0),
	CFrame (&CMonster::AI_Move, 0),
	CFrame (&CMonster::AI_Move, 0),
	CFrame (&CMonster::AI_Move, 0),
	CFrame (&CMonster::AI_Move, 0),
	CFrame (&CMonster::AI_Move, 0),
	CFrame (&CMonster::AI_Move, 0),
	CFrame (&CMonster::AI_Move, 0),
	CFrame (&CMonster::AI_Move, 0),

	CFrame (&CMonster::AI_Move, 0),
	CFrame (&CMonster::AI_Move, 0),
	CFrame (&CMonster::AI_Move, 0),
	CFrame (&CMonster::AI_Move, 0),
	CFrame (&CMonster::AI_Move, 0),
	CFrame (&CMonster::AI_Move, 0),
	CFrame (&CMonster::AI_Move, 0),
	CFrame (&CMonster::AI_Move, 0),
	CFrame (&CMonster::AI_Move, 0),
	CFrame (&CMonster::AI_Move, 0),

	CFrame (&CMonster::AI_Move, 0),
	CFrame (&CMonster::AI_Move, 0),
	CFrame (&CMonster::AI_Move, 0),
	CFrame (&CMonster::AI_Move, 0),
	CFrame (&CMonster::AI_Move, 0),
	CFrame (&CMonster::AI_Move, 0),
	CFrame (&CMonster::AI_Move, 0),
	CFrame (&CMonster::AI_Move, 0),
	CFrame (&CMonster::AI_Move, 0),
	CFrame (&CMonster::AI_Move, 0),

	CFrame (&CMonster::AI_Move, 0),
	CFrame (&CMonster::AI_Move, 0),
	CFrame (&CMonster::AI_Move, 0),
	CFrame (&CMonster::AI_Move, 0),
	CFrame (&CMonster::AI_Move, 0),
	CFrame (&CMonster::AI_Move, 0),
	CFrame (&CMonster::AI_Move, 0),
	CFrame (&CMonster::AI_Move, 0),
	CFrame (&CMonster::AI_Move, 0),
	CFrame (&CMonster::AI_Move, 0),

	CFrame (&CMonster::AI_Move, 0),
	CFrame (&CMonster::AI_Move, 0),
	CFrame (&CMonster::AI_Move, 0),
	CFrame (&CMonster::AI_Move, 0),
	CFrame (&CMonster::AI_Move, 0),
	CFrame (&CMonster::AI_Move, 0)
};
CAnim FlipperMoveDeath (FRAME_flpdth01, FRAME_flpdth56, FlipperFramesDeath, ConvertDerivedFunction(&CBarracudaShark::Dead));

void CBarracudaShark::Sight ()
{
	Entity->PlaySound (CHAN_VOICE, SoundSight);
}

void CBarracudaShark::Die (CBaseEntity *inflictor, CBaseEntity *attacker, int damage, vec3f &point)
{
// check for gib
	if (Entity->Health <= Entity->GibHealth)
	{
		Entity->PlaySound (CHAN_VOICE, SoundIndex ("misc/udeath.wav"));
		for (int n= 0; n < 2; n++)
			CGibEntity::Spawn (Entity, GameMedia.Gib_Bone[0], damage, GIB_ORGANIC);
		for (int n= 0; n < 2; n++)
			CGibEntity::Spawn (Entity, GameMedia.Gib_SmallMeat, damage, GIB_ORGANIC);
		Entity->ThrowHead (GameMedia.Gib_SmallMeat, damage, GIB_ORGANIC);
		Entity->DeadFlag = true;
		return;
	}

	if (Entity->DeadFlag == true)
		return;

// regular death
	Entity->PlaySound (CHAN_VOICE, SoundDeath);
	Entity->DeadFlag = true;
	Entity->CanTakeDamage = true;
	CurrentMove = &FlipperMoveDeath;
}

/*QUAKED monster_flipper (1 .5 0) (-16 -16 -24) (16 16 32) Ambush Trigger_Spawn Sight
*/
void CBarracudaShark::Spawn ()
{
	SoundPain1		= SoundIndex ("flipper/flppain1.wav");	
	SoundPain2		= SoundIndex ("flipper/flppain2.wav");	
	SoundDeath		= SoundIndex ("flipper/flpdeth1.wav");	
	SoundChomp		= SoundIndex ("flipper/flpatck1.wav");
	SoundAttack		= SoundIndex ("flipper/flpatck2.wav");
	SoundIdle		= SoundIndex ("flipper/flpidle1.wav");
	SoundSearch		= SoundIndex ("flipper/flpsrch1.wav");
	SoundSight		= SoundIndex ("flipper/flpsght1.wav");

	Entity->GetSolid() = SOLID_BBOX;
	Entity->State.GetModelIndex() = ModelIndex ("models/monsters/flipper/tris.md2");
	Entity->GetMins().Set (-16, -16, 0);
	Entity->GetMaxs().Set (16, 16, 32);

	Entity->Health = 50;
	Entity->GibHealth = -30;
	Entity->Mass = 100;

	MonsterFlags = (MF_HAS_MELEE | MF_HAS_SIGHT);

	Entity->Link ();
	CurrentMove = &FlipperMoveStand;	

	SwimMonsterStart ();
}

LINK_MONSTER_CLASSNAME_TO_CLASS ("monster_flipper", CBarracudaShark);