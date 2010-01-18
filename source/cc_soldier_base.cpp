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
// cc_soldier_base.cpp
// The base for soldier-like monsters.
//

#include "cc_local.h"
#include "cc_soldier_base.h"

CSoldierBase::CSoldierBase (uint32 ID) :
CMonster (ID)
{
};

void CSoldierBase::Idle ()
{
	if (frand() > 0.8)
		Entity->PlaySound (CHAN_VOICE, Sounds[SOUND_IDLE], 255, ATTN_IDLE);
}

void CSoldierBase::CockGun ()
{
	Entity->PlaySound (CHAN_WEAPON, Sounds[SOUND_COCK]);
}

// STAND

CFrame SoldierFramesStand1 [] =
{
	CFrame (&CMonster::AI_Stand, 0, ConvertDerivedFunction(&CSoldierBase::Idle)),
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
	CFrame (&CMonster::AI_Stand, 0),
	CFrame (&CMonster::AI_Stand, 0),
	CFrame (&CMonster::AI_Stand, 0),
	CFrame (&CMonster::AI_Stand, 0),
	CFrame (&CMonster::AI_Stand, 0),
	CFrame (&CMonster::AI_Stand, 0),
	CFrame (&CMonster::AI_Stand, 0),
	CFrame (&CMonster::AI_Stand, 0)
};
CAnim SoldierMoveStand1 (FRAME_stand101, FRAME_stand130, SoldierFramesStand1, ConvertDerivedFunction(&CSoldierBase::Stand));

CFrame SoldierFramesStand3 [] =
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
	CFrame (&CMonster::AI_Stand, 0, ConvertDerivedFunction(&CSoldierBase::CockGun)),
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
CAnim SoldierMoveStand3 (FRAME_stand301, FRAME_stand339, SoldierFramesStand3, ConvertDerivedFunction(&CSoldierBase::Stand));

void CSoldierBase::Stand ()
{
	if ((CurrentMove == &SoldierMoveStand3) || (frand() < 0.8))
		CurrentMove = &SoldierMoveStand1;
	else
		CurrentMove = &SoldierMoveStand3;
}


//
// WALK
//

void CSoldierBase::Walk1_Random ()
{
	if (frand() > 0.1)
		NextFrame = FRAME_walk101;
}

CFrame SoldierFramesWalk1 [] =
{
	CFrame (&CMonster::AI_Walk, 3),
	CFrame (&CMonster::AI_Walk, 6),
	CFrame (&CMonster::AI_Walk, 2),
	CFrame (&CMonster::AI_Walk, 2),
	CFrame (&CMonster::AI_Walk, 2),
	CFrame (&CMonster::AI_Walk, 1),
	CFrame (&CMonster::AI_Walk, 6),
	CFrame (&CMonster::AI_Walk, 5),
	CFrame (&CMonster::AI_Walk, 3),
	CFrame (&CMonster::AI_Walk, -1, ConvertDerivedFunction(&CSoldierBase::Walk1_Random)),
	CFrame (&CMonster::AI_Walk, 0),
	CFrame (&CMonster::AI_Walk, 0),
	CFrame (&CMonster::AI_Walk, 0),
	CFrame (&CMonster::AI_Walk, 0),
	CFrame (&CMonster::AI_Walk, 0),
	CFrame (&CMonster::AI_Walk, 0),
	CFrame (&CMonster::AI_Walk, 0),
	CFrame (&CMonster::AI_Walk, 0),
	CFrame (&CMonster::AI_Walk, 0),
	CFrame (&CMonster::AI_Walk, 0),
	CFrame (&CMonster::AI_Walk, 0),
	CFrame (&CMonster::AI_Walk, 0),
	CFrame (&CMonster::AI_Walk, 0),
	CFrame (&CMonster::AI_Walk, 0),
	CFrame (&CMonster::AI_Walk, 0),
	CFrame (&CMonster::AI_Walk, 0),
	CFrame (&CMonster::AI_Walk, 0),
	CFrame (&CMonster::AI_Walk, 0),
	CFrame (&CMonster::AI_Walk, 0),
	CFrame (&CMonster::AI_Walk, 0),
	CFrame (&CMonster::AI_Walk, 0),
	CFrame (&CMonster::AI_Walk, 0),
	CFrame (&CMonster::AI_Walk, 0)
};
CAnim SoldierMoveWalk1 (FRAME_walk101, FRAME_walk133, SoldierFramesWalk1);

CFrame SoldierFramesWalk2 [] =
{
	CFrame (&CMonster::AI_Walk, 4),
	CFrame (&CMonster::AI_Walk, 4),
	CFrame (&CMonster::AI_Walk, 9),
	CFrame (&CMonster::AI_Walk, 8),
	CFrame (&CMonster::AI_Walk, 5),
	CFrame (&CMonster::AI_Walk, 1),
	CFrame (&CMonster::AI_Walk, 3),
	CFrame (&CMonster::AI_Walk, 7),
	CFrame (&CMonster::AI_Walk, 6),
	CFrame (&CMonster::AI_Walk, 7)
};
CAnim SoldierMoveWalk2 (FRAME_walk209, FRAME_walk218, SoldierFramesWalk2);

void CSoldierBase::Walk ()
{
	if (frand() < 0.5)
		CurrentMove = &SoldierMoveWalk1;
	else
		CurrentMove = &SoldierMoveWalk2;
}


//
// RUN
//

CFrame SoldierFramesStartRun [] =
{
	CFrame (&CMonster::AI_Run, 7),
	CFrame (&CMonster::AI_Run, 5)
};
CAnim SoldierMoveStartRun (FRAME_run01, FRAME_run02, SoldierFramesStartRun, ConvertDerivedFunction(&CSoldierBase::Run));

CFrame SoldierFramesRun [] =
{
	CFrame (&CMonster::AI_Run, 10),
	CFrame (&CMonster::AI_Run, 11),
	CFrame (&CMonster::AI_Run, 11),
	CFrame (&CMonster::AI_Run, 16),
	CFrame (&CMonster::AI_Run, 10),
	CFrame (&CMonster::AI_Run, 15)
};
CAnim SoldierMoveRun (FRAME_run03, FRAME_run08, SoldierFramesRun);

void CSoldierBase::Run ()
{
	if (AIFlags & AI_STAND_GROUND)
	{
		CurrentMove = &SoldierMoveStand1;
		return;
	}

	if (CurrentMove == &SoldierMoveWalk1 ||
		CurrentMove == &SoldierMoveWalk2 ||
		CurrentMove == &SoldierMoveStartRun)
		CurrentMove = &SoldierMoveRun;
	else
		CurrentMove = &SoldierMoveStartRun;
}


//
// PAIN
//

CFrame SoldierFramesPain1 [] =
{
	CFrame (&CMonster::AI_Move, -3),
	CFrame (&CMonster::AI_Move, 4),
	CFrame (&CMonster::AI_Move, 1),
	CFrame (&CMonster::AI_Move, 1),
	CFrame (&CMonster::AI_Move, 0)
};
CAnim SoldierMovePain1 (FRAME_pain101, FRAME_pain105, SoldierFramesPain1, ConvertDerivedFunction(&CSoldierBase::Run));

CFrame SoldierFramesPain2 [] =
{
	CFrame (&CMonster::AI_Move, -13),
	CFrame (&CMonster::AI_Move, -1),
	CFrame (&CMonster::AI_Move, 2),
	CFrame (&CMonster::AI_Move, 4),
	CFrame (&CMonster::AI_Move, 2),
	CFrame (&CMonster::AI_Move, 3),
	CFrame (&CMonster::AI_Move, 2)
};
CAnim SoldierMovePain2 (FRAME_pain201, FRAME_pain207, SoldierFramesPain2, ConvertDerivedFunction(&CSoldierBase::Run));

CFrame SoldierFramesPain3 [] =
{
	CFrame (&CMonster::AI_Move, -8),
	CFrame (&CMonster::AI_Move, 10),
	CFrame (&CMonster::AI_Move, -4),
	CFrame (&CMonster::AI_Move, -1),
	CFrame (&CMonster::AI_Move, -3),
	CFrame (&CMonster::AI_Move, 0),
	CFrame (&CMonster::AI_Move, 3),
	CFrame (&CMonster::AI_Move, 0),
	CFrame (&CMonster::AI_Move, 0),
	CFrame (&CMonster::AI_Move, 0),
	CFrame (&CMonster::AI_Move, 0),
	CFrame (&CMonster::AI_Move, 1),
	CFrame (&CMonster::AI_Move, 0),
	CFrame (&CMonster::AI_Move, 1),
	CFrame (&CMonster::AI_Move, 2),
	CFrame (&CMonster::AI_Move, 4),
	CFrame (&CMonster::AI_Move, 3),
	CFrame (&CMonster::AI_Move, 2)
};
CAnim SoldierMovePain3 (FRAME_pain301, FRAME_pain318, SoldierFramesPain3, ConvertDerivedFunction(&CSoldierBase::Run));

CFrame SoldierFramesPain4 [] =
{
	CFrame (&CMonster::AI_Move, 0),
	CFrame (&CMonster::AI_Move, 0),
	CFrame (&CMonster::AI_Move, 0),
	CFrame (&CMonster::AI_Move, -10),
	CFrame (&CMonster::AI_Move, -6),
	CFrame (&CMonster::AI_Move, 8),
	CFrame (&CMonster::AI_Move, 4),
	CFrame (&CMonster::AI_Move, 1),
	CFrame (&CMonster::AI_Move, 0),
	CFrame (&CMonster::AI_Move, 2),
	CFrame (&CMonster::AI_Move, 5),
	CFrame (&CMonster::AI_Move, 2),
	CFrame (&CMonster::AI_Move, -1),
	CFrame (&CMonster::AI_Move, -1),
	CFrame (&CMonster::AI_Move, 3),
	CFrame (&CMonster::AI_Move, 2),
	CFrame (&CMonster::AI_Move, 0)
};
CAnim SoldierMovePain4 (FRAME_pain401, FRAME_pain417, SoldierFramesPain4, ConvertDerivedFunction(&CSoldierBase::Run));


void CSoldierBase::Pain (CBaseEntity *Other, sint32 Damage)
{
	if (Entity->Health < (Entity->MaxHealth / 2))
			Entity->State.GetSkinNum() |= 1;

#if MONSTER_USE_ROGUE_AI
	DoneDodge ();
	StopCharge();

	// if we're blind firing, this needs to be turned off here
	AIFlags &= ~AI_MANUAL_STEERING;
	// PMM - clear duck flag
	if (AIFlags & AI_DUCKED)
		UnDuck();
#endif

	if (Level.Frame < PainDebounceTime)
	{
		if ((Entity->Velocity.Z > 100) && ( (CurrentMove == &SoldierMovePain1) || (CurrentMove == &SoldierMovePain2) || (CurrentMove == &SoldierMovePain3)))
			CurrentMove = &SoldierMovePain4;
		return;
	}

	PainDebounceTime = Level.Frame + 30;
	Entity->PlaySound (CHAN_VOICE, Sounds[SOUND_PAIN]);

	if (Entity->Velocity.Z > 100)
	{
		CurrentMove = &SoldierMovePain4;
		return;
	}

	if (skill.Integer() == 3)
		return;		// no pain anims in nightmare

	float r = frand();
	if (r < 0.33)
		CurrentMove = &SoldierMovePain1;
	else if (r < 0.66)
		CurrentMove = &SoldierMovePain2;
	else
		CurrentMove = &SoldierMovePain3;
}


//
// ATTACK
//

// ATTACK1 (blaster/shotgun)

void CSoldierBase::Fire1 ()
{
	FireGun (0);
}

void CSoldierBase::Attack1_Refire1 ()
{
#if MONSTER_USE_ROGUE_AI
	// PMM - blindfire
	if (AIFlags & AI_MANUAL_STEERING)
	{
		AIFlags &= ~AI_MANUAL_STEERING;
		NextFrame = FRAME_attak110; // Skips the chicklee
		return;
	}
	// pmm
#endif

	if (SoldierAI != AI_BLASTER)
		return;

	if (entity_cast<CHurtableEntity>(Entity->Enemy)->Health <= 0)
		return;

	if (!EnemyVis)
		return;

	if ( ((skill.Integer() == 3) && (frand() < 0.5)) || (Range(Entity, Entity->Enemy) == RANGE_MELEE) )
		NextFrame = FRAME_attak102;
	else
		NextFrame = FRAME_attak110;
}

void CSoldierBase::Attack1_Refire2 ()
{
	if (SoldierAI == AI_BLASTER)
		return;

	if (entity_cast<CHurtableEntity>(Entity->Enemy)->Health <= 0)
		return;

	if (!EnemyVis)
		return;

	if ( ((skill.Integer() == 3) && (frand() < 0.5)) || (Range(Entity, Entity->Enemy) == RANGE_MELEE) )
		NextFrame = FRAME_attak102;
}

CFrame SoldierFramesAttack1 [] =
{
	CFrame (&CMonster::AI_Charge, 0),
	CFrame (&CMonster::AI_Charge, 0),
	CFrame (&CMonster::AI_Charge, 0, ConvertDerivedFunction(&CSoldierBase::Fire1)),
	CFrame (&CMonster::AI_Charge, 0),
	CFrame (&CMonster::AI_Charge, 0),
	CFrame (&CMonster::AI_Charge, 0, ConvertDerivedFunction(&CSoldierBase::Attack1_Refire1)),
	CFrame (&CMonster::AI_Charge, 0),
	CFrame (&CMonster::AI_Charge, 0, ConvertDerivedFunction(&CSoldierBase::CockGun)),
	CFrame (&CMonster::AI_Charge, 0, ConvertDerivedFunction(&CSoldierBase::Attack1_Refire2)),
	CFrame (&CMonster::AI_Charge, 0),
	CFrame (&CMonster::AI_Charge, 0),
	CFrame (&CMonster::AI_Charge, 0)
};
CAnim SoldierMoveAttack1 (FRAME_attak101, FRAME_attak112, SoldierFramesAttack1, ConvertDerivedFunction(&CSoldierBase::Run));

// ATTACK2 (blaster/shotgun)

void CSoldierBase::Fire2 ()
{
	FireGun (1);
}

void CSoldierBase::Attack2_Refire1 ()
{
	if (SoldierAI != AI_BLASTER)
		return;

	if (entity_cast<CHurtableEntity>(Entity->Enemy)->Health <= 0)
		return;

	if (!EnemyVis)
		return;

	if ( ((skill.Integer() == 3) && (frand() < 0.5)) || (Range(Entity, Entity->Enemy) == RANGE_MELEE) )
		NextFrame = FRAME_attak204;
	else
		NextFrame = FRAME_attak216;
}

void CSoldierBase::Attack2_Refire2 ()
{
	if (SoldierAI == AI_BLASTER)
		return;

	if (entity_cast<CHurtableEntity>(Entity->Enemy)->Health <= 0)
		return;

	if (!EnemyVis)
		return;

	if ( ((skill.Integer() == 3) && (frand() < 0.5)) || (Range(Entity, Entity->Enemy) == RANGE_MELEE) )
		NextFrame = FRAME_attak204;
}

CFrame SoldierFramesAttack2 [] =
{
	CFrame (&CMonster::AI_Charge, 0),
	CFrame (&CMonster::AI_Charge, 0),
	CFrame (&CMonster::AI_Charge, 0),
	CFrame (&CMonster::AI_Charge, 0),
	CFrame (&CMonster::AI_Charge, 0, ConvertDerivedFunction(&CSoldierBase::Fire2)),
	CFrame (&CMonster::AI_Charge, 0),
	CFrame (&CMonster::AI_Charge, 0),
	CFrame (&CMonster::AI_Charge, 0, ConvertDerivedFunction(&CSoldierBase::Attack2_Refire1)),
	CFrame (&CMonster::AI_Charge, 0),
	CFrame (&CMonster::AI_Charge, 0),
	CFrame (&CMonster::AI_Charge, 0),
	CFrame (&CMonster::AI_Charge, 0),
	CFrame (&CMonster::AI_Charge, 0, ConvertDerivedFunction(&CSoldierBase::CockGun)),
	CFrame (&CMonster::AI_Charge, 0),
	CFrame (&CMonster::AI_Charge, 0, ConvertDerivedFunction(&CSoldierBase::Attack2_Refire2)),
	CFrame (&CMonster::AI_Charge, 0),
	CFrame (&CMonster::AI_Charge, 0),
	CFrame (&CMonster::AI_Charge, 0)
};
CAnim SoldierMoveAttack2 (FRAME_attak201, FRAME_attak218, SoldierFramesAttack2, ConvertDerivedFunction(&CSoldierBase::Run));

// ATTACK3 (duck and shoot)

void CSoldierBase::Fire3 ()
{
#if MONSTER_USE_ROGUE_AI
	DuckDown ();
#else
	Duck_Down ();
#endif
	FireGun (2);
}

void CSoldierBase::Attack3_Refire ()
{
	if (!EnemyVis)
		return;

#if MONSTER_USE_ROGUE_AI
	if ((Level.Frame + 4) < DuckWaitTime)
#else
	if ((Level.Frame + 4) < PauseTime)
#endif
		NextFrame = FRAME_attak303;
}

CFrame SoldierFramesAttack3 [] =
{
	CFrame (&CMonster::AI_Charge, 0),
	CFrame (&CMonster::AI_Charge, 0),
	CFrame (&CMonster::AI_Charge, 0, ConvertDerivedFunction(&CSoldierBase::Fire3)),
	CFrame (&CMonster::AI_Charge, 0),
	CFrame (&CMonster::AI_Charge, 0),
	CFrame (&CMonster::AI_Charge, 0, ConvertDerivedFunction(&CSoldierBase::Attack3_Refire)),
#if MONSTER_USE_ROGUE_AI
	CFrame (&CMonster::AI_Charge, 0, &CMonster::UnDuck),
#else
	CFrame (&CMonster::AI_Charge, 0, ConvertDerivedFunction(&CSoldierBase::Duck_Up)),
#endif
	CFrame (&CMonster::AI_Charge, 0),
	CFrame (&CMonster::AI_Charge, 0)
};
CAnim SoldierMoveAttack3 (FRAME_attak301, FRAME_attak309, SoldierFramesAttack3, ConvertDerivedFunction(&CSoldierBase::Run));

// ATTACK4 (machinegun)

void CSoldierBase::Fire4 ()
{
	FireGun (3);
//
//	if (self->enemy->health <= 0)
//		return;
//
//	if ( ((skill.Integer() == 3) && (frand() < 0.5)) || (range(self, self->enemy) == RANGE_MELEE) )
//		self->monsterinfo.nextframe = FRAME_attak402;
}

CFrame SoldierFramesAttack4 [] =
{
	CFrame (&CMonster::AI_Charge, 0),
	CFrame (&CMonster::AI_Charge, 0),
	CFrame (&CMonster::AI_Charge, 0, ConvertDerivedFunction(&CSoldierBase::Fire4)),
	CFrame (&CMonster::AI_Charge, 0),
	CFrame (&CMonster::AI_Charge, 0),
	CFrame (&CMonster::AI_Charge, 0)
};
CAnim SoldierMoveAttack4 (FRAME_attak401, FRAME_attak406, SoldierFramesAttack4, ConvertDerivedFunction(&CSoldierBase::Run));

// ATTACK6 (run & shoot)

void CSoldierBase::Fire8 ()
{
	FireGun (7);
}

void CSoldierBase::Attack6_Refire ()
{
#if MONSTER_USE_ROGUE_AI
	// PMM - make sure dodge & charge bits are cleared
	DoneDodge ();
	StopCharge ();
#endif

	if (entity_cast<CHurtableEntity>(Entity->Enemy)->Health <= 0)
		return;

	if (Range(Entity, Entity->Enemy) < RANGE_NEAR)
		return;

	if (!EnemyVis)
		return;

	if (((skill.Integer() == 3 && (SoldierAI == AI_BLASTER)) || ((frand() < (0.09*((float)skill.Integer()))))))
		NextFrame = FRAME_runs03;
}

#if MONSTER_USE_ROGUE_AI
void CSoldierBase::Attack6_RefireBlaster ()
{
	if (SoldierAI != AI_BLASTER)
		return;

	// PMM - make sure dodge & charge bits are cleared
	DoneDodge ();
	StopCharge ();

	if (entity_cast<CHurtableEntity>(Entity->Enemy)->Health <= 0)
		return;

	if (Range(Entity, Entity->Enemy) < RANGE_NEAR)
		return;

	if (!EnemyVis)
		return;

	if ((skill.Integer() == 3) || ((frand() < (0.25*((float)skill.Integer())))))
		NextFrame = FRAME_runs03;
	else
		NextFrame = FRAME_runs14;
}
#endif

#if MONSTER_USE_ROGUE_AI
void CSoldierBase::StartCharge ()
{
	AIFlags |= AI_CHARGING;
}

void CSoldierBase::StopCharge ()
{
	AIFlags &= ~AI_CHARGING;
}
#endif

CFrame SoldierFramesAttack6 [] =
{
#if !MONSTER_USE_ROGUE_AI
	CFrame (&CMonster::AI_Charge, 10),
	CFrame (&CMonster::AI_Charge,  4),
	CFrame (&CMonster::AI_Charge, 12),
	CFrame (&CMonster::AI_Charge, 11, ConvertDerivedFunction(&CSoldierBase::Fire8)),
	CFrame (&CMonster::AI_Charge, 13),
	CFrame (&CMonster::AI_Charge, 18),
	CFrame (&CMonster::AI_Charge, 15),
	CFrame (&CMonster::AI_Charge, 14),
	CFrame (&CMonster::AI_Charge, 11),
	CFrame (&CMonster::AI_Charge,  8),
	CFrame (&CMonster::AI_Charge, 11),
	CFrame (&CMonster::AI_Charge, 12),
	CFrame (&CMonster::AI_Charge, 12),
	CFrame (&CMonster::AI_Charge, 17, ConvertDerivedFunction(&CSoldierBase::Attack6_Refire))
#else
	CFrame (&CMonster::AI_Charge, 10, ConvertDerivedFunction(&CSoldierBase::StartCharge)),
	CFrame (&CMonster::AI_Charge,  4),
	CFrame (&CMonster::AI_Charge, 12),
	CFrame (&CMonster::AI_Charge, 11, ConvertDerivedFunction(&CSoldierBase::Fire8)),
	CFrame (&CMonster::AI_Charge, 13),
	CFrame (&CMonster::AI_Charge, 18, ConvertDerivedFunction(&CSoldierBase::DoneDodge)),
	CFrame (&CMonster::AI_Charge, 15),
	CFrame (&CMonster::AI_Charge, 14, ConvertDerivedFunction(&CSoldierBase::Attack6_RefireBlaster)),
	CFrame (&CMonster::AI_Charge, 11),
	CFrame (&CMonster::AI_Charge,  8, ConvertDerivedFunction(&CSoldierBase::CockGun)),
	CFrame (&CMonster::AI_Charge, 11),
	CFrame (&CMonster::AI_Charge, 12),
	CFrame (&CMonster::AI_Charge, 12),
	CFrame (&CMonster::AI_Charge, 17, ConvertDerivedFunction(&CSoldierBase::Attack6_Refire))
#endif
};
CAnim SoldierMoveAttack6 (FRAME_runs01, FRAME_runs14, SoldierFramesAttack6, ConvertDerivedFunction(&CSoldierBase::Run));

//
// SIGHT
//

void CSoldierBase::Sight ()
{
	Entity->PlaySound (CHAN_VOICE, (frand() < 0.5) ? Sounds[SOUND_SIGHT1] : Sounds[SOUND_SIGHT2]);

	if ((skill.Integer() > 0) && (Entity->Enemy->EntityFlags & ENT_HURTABLE) && (Range(Entity, Entity->Enemy) >= RANGE_NEAR))
	{
		// Only do run-shoot off the bat if we're not a shotgun soldier (too cheap)
		if ((frand() > 0.75) && (SoldierAI == AI_BLASTER))
			CurrentMove = &SoldierMoveAttack6;
	}
}

//
// DUCK
//

CFrame SoldierFramesDuck [] =
{
#if MONSTER_USE_ROGUE_AI
	CFrame (&CMonster::AI_Move, 5, &CMonster::DuckDown),
	CFrame (&CMonster::AI_Move, -1, &CMonster::DuckHold),
	CFrame (&CMonster::AI_Move, 1),
	CFrame (&CMonster::AI_Move, 0,  &CMonster::UnDuck),
	CFrame (&CMonster::AI_Move, 5)
#else
	CFrame (&CMonster::AI_Move, 5, ConvertDerivedFunction(&CSoldierBase::Duck_Down)),
	CFrame (&CMonster::AI_Move, -1, ConvertDerivedFunction(&CSoldierBase::Duck_Hold)),
	CFrame (&CMonster::AI_Move, 1),
	CFrame (&CMonster::AI_Move, 0,  ConvertDerivedFunction(&CSoldierBase::Duck_Up)),
	CFrame (&CMonster::AI_Move, 5)
#endif
};
CAnim SoldierMoveDuck (FRAME_duck01, FRAME_duck05, SoldierFramesDuck, ConvertDerivedFunction(&CSoldierBase::Run));

#if !MONSTER_USE_ROGUE_AI
void CSoldierBase::Dodge (CBaseEntity *Attacker, float eta)
{
	if (frand() > 0.25)
		return;

	if (!Entity->Enemy)
		Entity->Enemy = Attacker;

	PauseTime = Level.Frame + ((eta + 0.3) * 10);

	switch (skill.Integer())
	{
	case 0:
	case 1:
		if (frand() > 0.33)
			CurrentMove = &SoldierMoveDuck;
		else
			CurrentMove = &SoldierMoveAttack3;
		break;
	case 2:
	case 3:
	default:
		if (frand() > 0.66)
			CurrentMove = &SoldierMoveDuck;
		else
			CurrentMove = &SoldierMoveAttack3;
		break;
	}
}

void CSoldierBase::Duck_Hold ()
{
	if (Level.Frame >= PauseTime)
		AIFlags &= ~AI_HOLD_FRAME;
	else
		AIFlags |= AI_HOLD_FRAME;
}
#endif


//
// DEATH
//

void CSoldierBase::Fire6 ()
{
	FireGun (5);
}

void CSoldierBase::Fire7 ()
{
	FireGun (6);
}

void CSoldierBase::Dead ()
{
	Entity->GetMins().Set (-16, -16, -24);
	Entity->GetMaxs().Set (16, 16, -8);
	Entity->PhysicsType = PHYSICS_TOSS;
	Entity->GetSvFlags() |= SVF_DEADMONSTER;
	Entity->NextThink = 0;
	Entity->Link ();
}

CFrame SoldierFramesDeath1 [] =
{
	CFrame (&CMonster::AI_Move, 0),
	CFrame (&CMonster::AI_Move, -10),
	CFrame (&CMonster::AI_Move, -10),
	CFrame (&CMonster::AI_Move, -10),
	CFrame (&CMonster::AI_Move, -5),
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
	CFrame (&CMonster::AI_Move, 0, ConvertDerivedFunction(&CSoldierBase::Fire6)),
	CFrame (&CMonster::AI_Move, 0),
	CFrame (&CMonster::AI_Move, 0),
	CFrame (&CMonster::AI_Move, 0, ConvertDerivedFunction(&CSoldierBase::Fire7)),
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
CAnim SoldierMoveDeath1 (FRAME_death101, FRAME_death136, SoldierFramesDeath1, ConvertDerivedFunction(&CSoldierBase::Dead));

CFrame SoldierFramesDeath2 [] =
{
	CFrame (&CMonster::AI_Move, -5),
	CFrame (&CMonster::AI_Move, -5),
	CFrame (&CMonster::AI_Move, -5),
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
CAnim SoldierMoveDeath2 (FRAME_death201, FRAME_death235, SoldierFramesDeath2, ConvertDerivedFunction(&CSoldierBase::Dead));

CFrame SoldierFramesDeath3 [] =
{
	CFrame (&CMonster::AI_Move, -5),
	CFrame (&CMonster::AI_Move, -5),
	CFrame (&CMonster::AI_Move, -5),
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
};
CAnim SoldierMoveDeath3 (FRAME_death301, FRAME_death345, SoldierFramesDeath3, ConvertDerivedFunction(&CSoldierBase::Dead));

CFrame SoldierFramesDeath4 [] =
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
	CFrame (&CMonster::AI_Move, 0)
};
CAnim SoldierMoveDeath4 (FRAME_death401, FRAME_death453, SoldierFramesDeath4, ConvertDerivedFunction(&CSoldierBase::Dead));

CFrame SoldierFramesDeath5 [] =
{
	CFrame (&CMonster::AI_Move, -5),
	CFrame (&CMonster::AI_Move, -5),
	CFrame (&CMonster::AI_Move, -5),
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
CAnim SoldierMoveDeath5 (FRAME_death501, FRAME_death524, SoldierFramesDeath5, ConvertDerivedFunction(&CSoldierBase::Dead));

CFrame SoldierFramesDeath6 [] =
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
	CFrame (&CMonster::AI_Move, 0)
};
CAnim SoldierMoveDeath6 (FRAME_death601, FRAME_death610, SoldierFramesDeath6, ConvertDerivedFunction(&CSoldierBase::Dead));

void CSoldierBase::Die (CBaseEntity *Inflictor, CBaseEntity *Attacker, sint32 Damage, vec3f &point)
{
// check for gib
	if (Entity->Health <= Entity->GibHealth)
	{
		Entity->PlaySound (CHAN_VOICE, SoundIndex ("misc/udeath.wav"));
		for (sint32 n= 0; n < 3; n++)
			CGibEntity::Spawn (Entity, GameMedia.Gib_SmallMeat, Damage, GIB_ORGANIC);
		CGibEntity::Spawn (Entity, GameMedia.Gib_Chest, Damage, GIB_ORGANIC);
		Entity->ThrowHead (GameMedia.Gib_Head[1], Damage, GIB_ORGANIC);
		Entity->DeadFlag = true;
		return;
	}

	if (Entity->DeadFlag == true)
		return;

// regular death
	Entity->DeadFlag = true;
	Entity->CanTakeDamage = true;
	Entity->State.GetSkinNum() |= 1;

	Entity->PlaySound (CHAN_VOICE, Sounds[SOUND_DEATH]);

	if (Q_fabs((Entity->State.GetOrigin().Z + Entity->ViewHeight) - point[2]) <= 4)
	{
		// head shot
		CurrentMove = &SoldierMoveDeath3;
		return;
	}

	switch (irandom(5))
	{
	case 0:
		CurrentMove = &SoldierMoveDeath1;
		break;
	case 1:
		CurrentMove = &SoldierMoveDeath2;
		break;
	case 2:
		CurrentMove = &SoldierMoveDeath4;
		break;
	case 3:
		CurrentMove = &SoldierMoveDeath5;
		break;
	case 4:
		CurrentMove = &SoldierMoveDeath6;
		break;
	}
}

//
// NEW DODGE CODE
//

#if MONSTER_USE_ROGUE_AI
void CSoldierBase::Duck (float eta)
{
	float r;

	// has to be done immediately otherwise he can get stuck
	DuckDown();

	if (skill.Integer() == 0)
	{
		// PMM - stupid dodge
		NextFrame = FRAME_duck01;
		CurrentMove = &SoldierMoveDuck;
		DuckWaitTime = Level.Frame + ((eta + 1) * 10);
		return;
	}

	r = frand();

	if (r > (skill.Integer() * 0.3))
	{
		NextFrame = FRAME_duck01;
		CurrentMove = &SoldierMoveDuck;
		DuckWaitTime = Level.Frame + ((eta + (0.1 * (3 - skill.Integer()))) * 10);
	}
	else
	{
		NextFrame = FRAME_attak301;
		CurrentMove = &SoldierMoveAttack3;
		DuckWaitTime = Level.Frame + ((eta + 1) * 10);
	}
	return;
}

void CSoldierBase::SideStep ()
{
	if (SoldierAI != AI_MACHINEGUN)
	{
		if (CurrentMove != &SoldierMoveAttack6)
			CurrentMove = &SoldierMoveAttack6;
	}
	else
	{
		if (CurrentMove != &SoldierMoveStartRun)
			CurrentMove = &SoldierMoveStartRun;
	}
}
#else
void CSoldierBase::Duck_Down ()
{
	if (AIFlags & AI_DUCKED)
		return;
	AIFlags |= AI_DUCKED;
	Entity->GetMaxs().Z -= 32;
	Entity->CanTakeDamage = true;
	PauseTime = Level.Frame + 10;
	Entity->Link ();
}

void CSoldierBase::Duck_Up ()
{
	AIFlags &= ~AI_DUCKED;
	Entity->GetMaxs().Z += 32;
	Entity->CanTakeDamage = true;
	Entity->Link ();
}
#endif

void CSoldierBase::Spawn ()
{
	Scale = MODEL_SCALE;

	Entity->GetMins().Set (-16, -16, -24);
	Entity->GetMaxs().Set (16, 16, 32);
	Entity->GetSolid() = SOLID_BBOX;

	Entity->State.GetModelIndex() = ModelIndex ("models/monsters/soldier/tris.md2");

	Sounds[SOUND_IDLE] =	SoundIndex ("soldier/solidle1.wav");
	Sounds[SOUND_SIGHT1] =	SoundIndex ("soldier/solsght1.wav");
	Sounds[SOUND_SIGHT2] =	SoundIndex ("soldier/solsrch1.wav");
	Sounds[SOUND_COCK] =	SoundIndex ("infantry/infatck3.wav");

	Entity->Mass = 100;
	MonsterFlags |= (MF_HAS_ATTACK | MF_HAS_SIGHT | MF_HAS_IDLE
#if MONSTER_USE_ROGUE_AI
		| MF_HAS_DODGE | MF_HAS_DUCK | MF_HAS_UNDUCK | MF_HAS_SIDESTEP
#endif
		);

	SpawnSoldier ();

	Entity->Link ();

	Stand ();
	WalkMonsterStart ();
}
