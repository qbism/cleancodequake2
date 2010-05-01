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
// cc_gladiator.cpp
// WACHOOEYCHOY
//

#include "cc_local.h"
#include "m_gladiator.h"
#include "cc_gladiator.h"

CGladiator::CGladiator (uint32 ID) :
CMonster (ID)
{
	Scale = MODEL_SCALE;
	MonsterName = "Gladiator";
}

void CGladiator::Idle ()
{
	Entity->PlaySound (CHAN_VOICE, Sounds[SOUND_IDLE]);
}

void CGladiator::Sight ()
{
	Entity->PlaySound (CHAN_VOICE, Sounds[SOUND_SIGHT]);
}

void CGladiator::Search ()
{
	Entity->PlaySound (CHAN_VOICE, Sounds[SOUND_SEARCH]);
}

void CGladiator::SwingCleaver ()
{
	Entity->PlaySound (CHAN_WEAPON, Sounds[SOUND_CLEAVER_SWING]);
}

CFrame GladiatorFramesStand [] =
{
	CFrame (&CMonster::AI_Stand, 0),
	CFrame (&CMonster::AI_Stand, 0),
	CFrame (&CMonster::AI_Stand, 0),
	CFrame (&CMonster::AI_Stand, 0),
	CFrame (&CMonster::AI_Stand, 0),
	CFrame (&CMonster::AI_Stand, 0),
	CFrame (&CMonster::AI_Stand, 0)
};
CAnim GladiatorMoveStand (FRAME_stand1, FRAME_stand7, GladiatorFramesStand);

void CGladiator::Stand ()
{
	CurrentMove = &GladiatorMoveStand;
}

CFrame GladiatorFramesWalk [] =
{
	CFrame (&CMonster::AI_Walk, 15),
	CFrame (&CMonster::AI_Walk, 7),
	CFrame (&CMonster::AI_Walk, 6),
	CFrame (&CMonster::AI_Walk, 5),
	CFrame (&CMonster::AI_Walk, 2),
	CFrame (&CMonster::AI_Walk, 0),
	CFrame (&CMonster::AI_Walk, 2),
	CFrame (&CMonster::AI_Walk, 8),
	CFrame (&CMonster::AI_Walk, 12),
	CFrame (&CMonster::AI_Walk, 8),
	CFrame (&CMonster::AI_Walk, 5),
	CFrame (&CMonster::AI_Walk, 5),
	CFrame (&CMonster::AI_Walk, 2),
	CFrame (&CMonster::AI_Walk, 2),
	CFrame (&CMonster::AI_Walk, 1),
	CFrame (&CMonster::AI_Walk, 8)
};
CAnim GladiatorMoveWalk (FRAME_walk1, FRAME_walk16, GladiatorFramesWalk);

void CGladiator::Walk ()
{
	CurrentMove = &GladiatorMoveWalk;
}

CFrame GladiatorFramesRun [] =
{
	CFrame (&CMonster::AI_Run, 23),
	CFrame (&CMonster::AI_Run, 14),
	CFrame (&CMonster::AI_Run, 14),
	CFrame (&CMonster::AI_Run, 21),
	CFrame (&CMonster::AI_Run, 12),
	CFrame (&CMonster::AI_Run, 13)
};
CAnim GladiatorMoveRun (FRAME_run1, FRAME_run6, GladiatorFramesRun);

void CGladiator::Run ()
{
	CurrentMove = (AIFlags & AI_STAND_GROUND) ? &GladiatorMoveStand : &GladiatorMoveRun;
}

void CGladiator::MeleeAttack ()
{
	vec3f	aim (MELEE_DISTANCE, Entity->GetMins().X, -4);
	Entity->PlaySound (CHAN_AUTO, (CMeleeWeapon::Fire (Entity, aim, (20 + (irandom(5))), 300)) ? Sounds[SOUND_CLEAVER_HIT] : Sounds[SOUND_CLEAVER_MISS]);
}

CFrame GladiatorFramesAttackMelee [] =
{
	CFrame (&CMonster::AI_Charge, 0),
	CFrame (&CMonster::AI_Charge, 0),
	CFrame (&CMonster::AI_Charge, 0),
	CFrame (&CMonster::AI_Charge, 0),
	CFrame (&CMonster::AI_Charge, 0, ConvertDerivedFunction(&CGladiator::SwingCleaver)),
	CFrame (&CMonster::AI_Charge, 0),
	CFrame (&CMonster::AI_Charge, 0, ConvertDerivedFunction(&CGladiator::MeleeAttack)),
	CFrame (&CMonster::AI_Charge, 0),
	CFrame (&CMonster::AI_Charge, 0),
	CFrame (&CMonster::AI_Charge, 0),
	CFrame (&CMonster::AI_Charge, 0, ConvertDerivedFunction(&CGladiator::SwingCleaver)),
	CFrame (&CMonster::AI_Charge, 0),
	CFrame (&CMonster::AI_Charge, 0),
	CFrame (&CMonster::AI_Charge, 0, ConvertDerivedFunction(&CGladiator::MeleeAttack)),
	CFrame (&CMonster::AI_Charge, 0),
	CFrame (&CMonster::AI_Charge, 0),
	CFrame (&CMonster::AI_Charge, 0)
};
CAnim GladiatorMoveAttackMelee (FRAME_melee1, FRAME_melee17, GladiatorFramesAttackMelee, &CMonster::Run);

void CGladiator::Melee ()
{
	CurrentMove = &GladiatorMoveAttackMelee;
}

void CGladiator::FireRail ()
{
	if (!HasValidEnemy())
		return;

	vec3f	start, forward, right;

	Entity->State.GetAngles().ToVectors (&forward, &right, NULL);
	G_ProjectSource (Entity->State.GetOrigin(), MonsterFlashOffsets[MZ2_GLADIATOR_RAILGUN_1], forward, right, start);

	// calc direction to where we targted
	MonsterFireRailgun (start, (SavedFirePosition - start).GetNormalized(), 50, 100, MZ2_GLADIATOR_RAILGUN_1);
}

void CGladiator::StorePosition ()
{
	SavedFirePosition = Entity->Enemy->State.GetOrigin() + vec3f(0, 0, Entity->Enemy->ViewHeight);
}

CFrame GladiatorFramesAttackGun [] =
{
	CFrame (&CMonster::AI_Charge, 0),
	CFrame (&CMonster::AI_Charge, 0),
	CFrame (&CMonster::AI_Charge, 0),
	CFrame (&CMonster::AI_Charge, 0, ConvertDerivedFunction(&CGladiator::FireRail)),
	CFrame (&CMonster::AI_Charge, 0),
	CFrame (&CMonster::AI_Charge, 0),
	CFrame (&CMonster::AI_Charge, 0),
	CFrame (&CMonster::AI_Charge, 0),
	CFrame (&CMonster::AI_Charge, 0)
};
CAnim GladiatorMoveAttackGun (FRAME_attack1, FRAME_attack9, GladiatorFramesAttackGun, &CMonster::Run);

void CGladiator::Attack ()
{
	// a small safe zone
	if ((Entity->State.GetOrigin() - Entity->Enemy->State.GetOrigin()).Length() <= (MELEE_DISTANCE + 32))
		return;

	// charge up the railgun
	StorePosition ();
	Entity->PlaySound (CHAN_WEAPON, Sounds[SOUND_GUN]);
	SavedFirePosition = Entity->Enemy->State.GetOrigin() + vec3f(0, 0, Entity->Enemy->ViewHeight);
	CurrentMove = &GladiatorMoveAttackGun;
}

CFrame GladiatorFramesPain [] =
{
	CFrame (&CMonster::AI_Move, 0),
	CFrame (&CMonster::AI_Move, 0),
	CFrame (&CMonster::AI_Move, 0),
	CFrame (&CMonster::AI_Move, 0),
	CFrame (&CMonster::AI_Move, 0),
	CFrame (&CMonster::AI_Move, 0)
};
CAnim GladiatorMovePain (FRAME_pain1, FRAME_pain6, GladiatorFramesPain, &CMonster::Run);

CFrame GladiatorFramesPainAir [] =
{
	CFrame (&CMonster::AI_Move, 0),
	CFrame (&CMonster::AI_Move, 0),
	CFrame (&CMonster::AI_Move, 0),
	CFrame (&CMonster::AI_Move, 0),
	CFrame (&CMonster::AI_Move, 0),
	CFrame (&CMonster::AI_Move, 0),
	CFrame (&CMonster::AI_Move, 0)
};
CAnim GladiatorMovePainAir (FRAME_painup1, FRAME_painup7, GladiatorFramesPainAir, &CMonster::Run);

void CGladiator::Pain (IBaseEntity *Other, sint32 Damage)
{
	if (Entity->Health < (Entity->MaxHealth / 2))
		Entity->State.GetSkinNum() = 1;

	if (Level.Frame < PainDebounceTime)
	{
		if ((Entity->Velocity.Z > 100) && (CurrentMove == &GladiatorMovePain))
			CurrentMove = &GladiatorMovePainAir;
		return;
	}

	PainDebounceTime = Level.Frame + 30;

	Entity->PlaySound (CHAN_VOICE, (frand() < 0.5) ? Sounds[SOUND_PAIN1] : Sounds[SOUND_PAIN2]);
	if (CvarList[CV_SKILL].Integer() == 3)
		return;		// no pain anims in nightmare

	CurrentMove = (Entity->Velocity.Z > 100) ? &GladiatorMovePainAir : &GladiatorMovePain;
}

void CGladiator::Dead ()
{
	Entity->GetMins().Set (-16, -16, -24);
	Entity->GetMaxs().Set (16, 16, -8);
	Entity->PhysicsType = PHYSICS_TOSS;
	Entity->GetSvFlags() |= SVF_DEADMONSTER;
	Entity->NextThink = 0;
	Entity->Link ();
}

CFrame GladiatorFramesDeath [] =
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
	CFrame (&CMonster::AI_Move, 0)
};
CAnim GladiatorMoveDeath (FRAME_death1, FRAME_death22, GladiatorFramesDeath, ConvertDerivedFunction(&CGladiator::Dead));

void CGladiator::Die (IBaseEntity *Inflictor, IBaseEntity *Attacker, sint32 Damage, vec3f &point)
{
// check for gib
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

// regular death
	Entity->PlaySound (CHAN_VOICE, Sounds[SOUND_DIE]);
	Entity->DeadFlag = true;
	Entity->CanTakeDamage = true;

	CurrentMove = &GladiatorMoveDeath;
}


/*QUAKED monster_gladiator (1 .5 0) (-32 -32 -24) (32 32 64) Ambush Trigger_Spawn Sight
*/
void CGladiator::Spawn ()
{
	Sounds[SOUND_PAIN1] = SoundIndex ("gladiator/pain.wav");	
	Sounds[SOUND_PAIN2] = SoundIndex ("gladiator/gldpain2.wav");	
	Sounds[SOUND_DIE] = SoundIndex ("gladiator/glddeth2.wav");	
	Sounds[SOUND_GUN] = SoundIndex ("gladiator/railgun.wav");
	Sounds[SOUND_CLEAVER_SWING] = SoundIndex ("gladiator/melee1.wav");
	Sounds[SOUND_CLEAVER_HIT] = SoundIndex ("gladiator/melee2.wav");
	Sounds[SOUND_CLEAVER_MISS] = SoundIndex ("gladiator/melee3.wav");
	Sounds[SOUND_IDLE] = SoundIndex ("gladiator/gldidle1.wav");
	Sounds[SOUND_SEARCH] = SoundIndex ("gladiator/gldsrch1.wav");
	Sounds[SOUND_SIGHT] = SoundIndex ("gladiator/sight.wav");

	Entity->GetSolid() = SOLID_BBOX;
	Entity->State.GetModelIndex() = ModelIndex ("models/monsters/gladiatr/tris.md2");
	Entity->GetMins().Set (-32, -32, -24);
	Entity->GetMaxs().Set (32, 32, 64);

	Entity->Health = 400;
	Entity->GibHealth = -175;
	Entity->Mass = 400;

	MonsterFlags = (MF_HAS_SEARCH | MF_HAS_IDLE | MF_HAS_SIGHT | MF_HAS_MELEE | MF_HAS_ATTACK);

	Entity->Link ();

	CurrentMove = &GladiatorMoveStand;
	WalkMonsterStart ();
}

LINK_MONSTER_CLASSNAME_TO_CLASS ("monster_gladiator", CGladiator);
