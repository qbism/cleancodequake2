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
#include "m_gunner.h"
#include "cc_gunner.h"

CGunner::CGunner (uint32 ID) :
CMonster (ID)
{
	Scale = MODEL_SCALE;
	MonsterName = "Gunner";
}

void CGunner::Idle ()
{
	Entity->PlaySound (CHAN_VOICE, SoundIdle, 255, ATTN_IDLE);
}

void CGunner::Sight ()
{
	Entity->PlaySound (CHAN_VOICE, SoundSight);
}

void CGunner::Search ()
{
	Entity->PlaySound (CHAN_VOICE, SoundSearch);
}

CFrame GunnerFramesFidget [] =
{
	CFrame (&CMonster::AI_Stand, 0),
	CFrame (&CMonster::AI_Stand, 0),
	CFrame (&CMonster::AI_Stand, 0),
	CFrame (&CMonster::AI_Stand, 0),
	CFrame (&CMonster::AI_Stand, 0),
	CFrame (&CMonster::AI_Stand, 0),
	CFrame (&CMonster::AI_Stand, 0),
	CFrame (&CMonster::AI_Stand, 0, ConvertDerivedFunction(&CGunner::Idle)),
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
CAnim GunnerMoveFidget (FRAME_stand31, FRAME_stand70, GunnerFramesFidget, ConvertDerivedFunction(&CGunner::Stand));

void CGunner::Fidget ()
{
	if (AIFlags & AI_STAND_GROUND)
		return;
	if (frand() <= 0.05)
		CurrentMove = &GunnerMoveFidget;
}

CFrame GunnerFramesStand [] =
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
	CFrame (&CMonster::AI_Stand, 0, ConvertDerivedFunction(&CGunner::Fidget)),

	CFrame (&CMonster::AI_Stand, 0),
	CFrame (&CMonster::AI_Stand, 0),
	CFrame (&CMonster::AI_Stand, 0),
	CFrame (&CMonster::AI_Stand, 0),
	CFrame (&CMonster::AI_Stand, 0),
	CFrame (&CMonster::AI_Stand, 0),
	CFrame (&CMonster::AI_Stand, 0),
	CFrame (&CMonster::AI_Stand, 0),
	CFrame (&CMonster::AI_Stand, 0),
	CFrame (&CMonster::AI_Stand, 0, ConvertDerivedFunction(&CGunner::Fidget)),

	CFrame (&CMonster::AI_Stand, 0),
	CFrame (&CMonster::AI_Stand, 0),
	CFrame (&CMonster::AI_Stand, 0),
	CFrame (&CMonster::AI_Stand, 0),
	CFrame (&CMonster::AI_Stand, 0),
	CFrame (&CMonster::AI_Stand, 0),
	CFrame (&CMonster::AI_Stand, 0),
	CFrame (&CMonster::AI_Stand, 0),
	CFrame (&CMonster::AI_Stand, 0),
	CFrame (&CMonster::AI_Stand, 0, ConvertDerivedFunction(&CGunner::Fidget))
};
CAnim GunnerMoveStand (FRAME_stand01, FRAME_stand30, GunnerFramesStand);

void CGunner::Stand ()
{
	CurrentMove = &GunnerMoveStand;
}

CFrame GunnerFramesWalk [] =
{
	CFrame (&CMonster::AI_Walk, 0),
	CFrame (&CMonster::AI_Walk, 3),
	CFrame (&CMonster::AI_Walk, 4),
	CFrame (&CMonster::AI_Walk, 5),
	CFrame (&CMonster::AI_Walk, 7),
	CFrame (&CMonster::AI_Walk, 2),
	CFrame (&CMonster::AI_Walk, 6),
	CFrame (&CMonster::AI_Walk, 4),
	CFrame (&CMonster::AI_Walk, 2),
	CFrame (&CMonster::AI_Walk, 7),
	CFrame (&CMonster::AI_Walk, 5),
	CFrame (&CMonster::AI_Walk, 7),
	CFrame (&CMonster::AI_Walk, 4)
};
CAnim GunnerMoveWalk (FRAME_walk07, FRAME_walk19, GunnerFramesWalk);

void CGunner::Walk ()
{
	CurrentMove = &GunnerMoveWalk;
}

CFrame GunnerFramesRun [] =
{
	CFrame (&CMonster::AI_Run, 26),
	CFrame (&CMonster::AI_Run, 9),
	CFrame (&CMonster::AI_Run, 9),
	CFrame (&CMonster::AI_Run, 9),
	CFrame (&CMonster::AI_Run, 15),
	CFrame (&CMonster::AI_Run, 10),
	CFrame (&CMonster::AI_Run, 13),
	CFrame (&CMonster::AI_Run, 6)
};
CAnim GunnerMoveRun(FRAME_run01, FRAME_run08, GunnerFramesRun);

void CGunner::Run ()
{
#ifdef MONSTER_USE_ROGUE_AI
	DoneDodge();
#endif
	CurrentMove = (AIFlags & AI_STAND_GROUND) ? &GunnerMoveStand : &GunnerMoveRun;
}

CFrame GunnerFramesRunAndShoot [] =
{
	CFrame (&CMonster::AI_Run, 32),
	CFrame (&CMonster::AI_Run, 15),
	CFrame (&CMonster::AI_Run, 10),
	CFrame (&CMonster::AI_Run, 18),
	CFrame (&CMonster::AI_Run, 8),
	CFrame (&CMonster::AI_Run, 20)
};
CAnim GunnerMoveRunAndShoot (FRAME_runs01, FRAME_runs06, GunnerFramesRunAndShoot);

void CGunner::RunAndShoot ()
{
	CurrentMove = &GunnerMoveRunAndShoot;
}

CFrame GunnerFramesPain3 [] =
{
	CFrame (&CMonster::AI_Move, -3),
	CFrame (&CMonster::AI_Move, 1),
	CFrame (&CMonster::AI_Move, 1),
	CFrame (&CMonster::AI_Move, 0),
	CFrame (&CMonster::AI_Move, 1)
};
CAnim GunnerMovePain3 (FRAME_pain301, FRAME_pain305, GunnerFramesPain3, ConvertDerivedFunction(&CGunner::Run));

CFrame GunnerFramesPain2 [] =
{
	CFrame (&CMonster::AI_Move, -2),
	CFrame (&CMonster::AI_Move, 11),
	CFrame (&CMonster::AI_Move, 6),
	CFrame (&CMonster::AI_Move, 2),
	CFrame (&CMonster::AI_Move, -1),
	CFrame (&CMonster::AI_Move, -7),
	CFrame (&CMonster::AI_Move, -2),
	CFrame (&CMonster::AI_Move, -7)
};
CAnim GunnerMovePain2 (FRAME_pain201, FRAME_pain208, GunnerFramesPain2, ConvertDerivedFunction(&CGunner::Run));

CFrame GunnerFramesPain1 [] =
{
	CFrame (&CMonster::AI_Move, 2),
	CFrame (&CMonster::AI_Move, 0),
	CFrame (&CMonster::AI_Move, -5),
	CFrame (&CMonster::AI_Move, 3),
	CFrame (&CMonster::AI_Move, -1),
	CFrame (&CMonster::AI_Move, 0),
	CFrame (&CMonster::AI_Move, 0),
	CFrame (&CMonster::AI_Move, 0),
	CFrame (&CMonster::AI_Move, 0),
	CFrame (&CMonster::AI_Move, 1),
	CFrame (&CMonster::AI_Move, 1),
	CFrame (&CMonster::AI_Move, 2),
	CFrame (&CMonster::AI_Move, 1),
	CFrame (&CMonster::AI_Move, 0),
	CFrame (&CMonster::AI_Move, -2),
	CFrame (&CMonster::AI_Move, -2),
	CFrame (&CMonster::AI_Move, 0),
	CFrame (&CMonster::AI_Move, 0)
};
CAnim GunnerMovePain1 (FRAME_pain101, FRAME_pain118, GunnerFramesPain1, ConvertDerivedFunction(&CGunner::Run));

void CGunner::Pain (CBaseEntity *other, float kick, int damage)
{
	if (Entity->Health < (Entity->MaxHealth / 2))
		Entity->State.GetSkinNum() = 1;

#ifdef MONSTER_USE_ROGUE_AI
	DoneDodge();
#endif

	if (level.Frame < PainDebounceTime)
		return;

	PainDebounceTime = level.Frame + 30;
	Entity->PlaySound (CHAN_VOICE, (irandom(2)) ? SoundPain : SoundPain2);

	if (skill->Integer() == 3)
		return;		// no pain anims in nightmare

	CurrentMove = ((damage <= 10) ? &GunnerMovePain3 : ((damage <= 25) ? &GunnerMovePain2 : &GunnerMovePain1));

#ifdef MONSTER_USE_ROGUE_AI
	AIFlags &= ~AI_MANUAL_STEERING;

	// PMM - clear duck flag
	if (AIFlags & AI_DUCKED)
		UnDuck();
#endif
}

void CGunner::Dead ()
{
	Entity->GetMins().Set (-16, -16, -24);
	Entity->GetMaxs().Set (16, 16, -8);
	Entity->PhysicsType = PHYSICS_TOSS;
	Entity->GetSvFlags() |= SVF_DEADMONSTER;
	Entity->NextThink = 0;
	Entity->Link ();
}

CFrame GunnerFramesDeath [] =
{
	CFrame (&CMonster::AI_Move, 0),
	CFrame (&CMonster::AI_Move, 0),
	CFrame (&CMonster::AI_Move, 0),
	CFrame (&CMonster::AI_Move, -7),
	CFrame (&CMonster::AI_Move, -3),
	CFrame (&CMonster::AI_Move, -5),
	CFrame (&CMonster::AI_Move, 8),
	CFrame (&CMonster::AI_Move, 6),
	CFrame (&CMonster::AI_Move, 0),
	CFrame (&CMonster::AI_Move, 0),
	CFrame (&CMonster::AI_Move, 0)
};
CAnim GunnerMoveDeath (FRAME_death01, FRAME_death11, GunnerFramesDeath, ConvertDerivedFunction(&CGunner::Dead));

void CGunner::Die (CBaseEntity *inflictor, CBaseEntity *attacker, int damage, vec3f &point)
{
// check for gib
	if (Entity->Health <= Entity->GibHealth)
	{
		Entity->PlaySound (CHAN_VOICE, SoundIndex ("misc/udeath.wav"));
		for (int n= 0; n < 2; n++)
			CGibEntity::Spawn (Entity, GameMedia.Gib_Bone[0], damage, GIB_ORGANIC);
		for (int n= 0; n < 4; n++)
			CGibEntity::Spawn (Entity, GameMedia.Gib_SmallMeat, damage, GIB_ORGANIC);
		Entity->ThrowHead (GameMedia.Gib_Head[1], damage, GIB_ORGANIC);
		Entity->DeadFlag = true;
		return;
	}

	if (Entity->DeadFlag == true)
		return;

// regular death
	Entity->PlaySound (CHAN_VOICE, SoundDeath);
	Entity->DeadFlag = true;
	Entity->CanTakeDamage = true;
	CurrentMove = &GunnerMoveDeath;
}

void CGunner::DuckDown ()
{
#ifndef MONSTER_USE_ROGUE_AI
	if (AIFlags & AI_DUCKED)
		return;
	AIFlags |= AI_DUCKED;
	if (skill->Integer() >= 2)
	{
		if (frand() > 0.5)
			Grenade ();
	}

	Entity->GetMaxs().Z -= 32;
	Entity->CanTakeDamage = true;
	PauseTime = level.Frame + 10;
	Entity->Link ();
#else
	AIFlags |= AI_DUCKED;
	if (skill->Integer() >= 2)
	{
		if (frand() > 0.5)
			Grenade ();
	}

	Entity->GetMaxs().Z = BaseHeight - 32;
	Entity->CanTakeDamage = true;
	if (DuckWaitTime < level.Frame)
		DuckWaitTime = level.Frame + 10;
	Entity->Link ();
#endif
}

#ifndef MONSTER_USE_ROGUE_AI
void CGunner::DuckHold ()
{
	if (level.Frame >= PauseTime)
		AIFlags &= ~AI_HOLD_FRAME;
	else
		AIFlags |= AI_HOLD_FRAME;
}

void CGunner::DuckUp ()
{
	AIFlags &= ~AI_DUCKED;
	Entity->GetMaxs().Z += 32;
	Entity->CanTakeDamage = true;
	Entity->Link ();
}
#endif

CFrame GunnerFramesDuck [] =
{
	CFrame (&CMonster::AI_Move, 1, ConvertDerivedFunction(&CGunner::DuckDown)),
	CFrame (&CMonster::AI_Move, 1),
#ifndef MONSTER_USE_ROGUE_AI
	CFrame (&CMonster::AI_Move, 1, ConvertDerivedFunction(&CGunner::DuckHold)),
#else
	CFrame (&CMonster::AI_Move, 1, &CMonster::DuckHold),
#endif
	CFrame (&CMonster::AI_Move, 0),
	CFrame (&CMonster::AI_Move, -1),
	CFrame (&CMonster::AI_Move, -1),
#ifndef MONSTER_USE_ROGUE_AI
	CFrame (&CMonster::AI_Move, 0, ConvertDerivedFunction(&CGunner::DuckUp)),
#else
	CFrame (&CMonster::AI_Move, 0, &CGunner::UnDuck),
#endif
	CFrame (&CMonster::AI_Move, -1)
};
CAnim GunnerMoveDuck (FRAME_duck01, FRAME_duck08, GunnerFramesDuck, ConvertDerivedFunction(&CGunner::Run));

void CGunner::Dodge (CBaseEntity *attacker, float eta
#ifdef MONSTER_USE_ROGUE_AI
					 , CTrace *tr
#endif
					 )
{
	if (frand() > 0.25)
		return;

	if (!Entity->Enemy)
		Entity->Enemy = attacker;

	CurrentMove = &GunnerMoveDuck;
}

void CGunner::OpenGun ()
{
	Entity->PlaySound (CHAN_VOICE, SoundOpen, 255, ATTN_IDLE);
}

#ifdef MONSTER_USE_ROGUE_AI
bool CGunner::GrenadeCheck()
{
	if(!Entity->Enemy)
		return false;

	vec3f		start, forward, right, target, dir;

	// if the player is above my head, use machinegun.

	// check for flag telling us that we're blindfiring
	if (AIFlags & AI_MANUAL_STEERING)
	{
		if ((Entity->State.GetOrigin().Z + Entity->ViewHeight) < BlindFireTarget.Z)
			return false;
	}
	else if(Entity->GetAbsMax().Z <= Entity->Enemy->GetAbsMin().Z)
		return false;

	// check to see that we can trace to the player before we start
	// tossing grenades around.
	Entity->State.GetAngles().ToVectors (&forward, &right, NULL);
	G_ProjectSource (Entity->State.GetOrigin(), dumb_and_hacky_monster_MuzzFlashOffset[MZ2_GUNNER_GRENADE_1], forward, right, start);

	// pmm - check for blindfire flag
	target = (AIFlags & AI_MANUAL_STEERING) ? BlindFireTarget : Entity->Enemy->State.GetOrigin();

	dir = Entity->State.GetOrigin() - target;

	if (dir.Length() < 100)
		return false;

	CTrace tr (start, target, Entity, CONTENTS_MASK_SHOT);
	if(tr.Ent == Entity->Enemy || tr.fraction == 1)
		return true;

	return false;
}
#endif

void CGunner::Fire ()
{
	vec3f	start, forward, right, target, aim;
	int		flash_number = MZ2_GUNNER_MACHINEGUN_1 + (Entity->State.GetFrame() - FRAME_attak216);

	Entity->State.GetAngles().ToVectors (&forward, &right, NULL);
	G_ProjectSource (Entity->State.GetOrigin(), dumb_and_hacky_monster_MuzzFlashOffset[flash_number], forward, right, start);

	// project enemy back a bit and target there
	target = Entity->Enemy->State.GetOrigin();
	target = target.MultiplyAngles (-0.2f, entity_cast<CPhysicsEntity>(Entity->Enemy)->Velocity);
	target.Z += Entity->Enemy->ViewHeight;

	aim = target - start;
	aim.NormalizeFast ();

	MonsterFireBullet (start, aim, 3, 4, DEFAULT_BULLET_HSPREAD, DEFAULT_BULLET_VSPREAD, flash_number);
}

void CGunner::Grenade ()
{
#ifndef MONSTER_USE_ROGUE_AI
	vec3f	start, forward, right;
	int		flash_number;

	switch (Entity->State.GetFrame())
	{
	case FRAME_attak105:
		flash_number = MZ2_GUNNER_GRENADE_1;
		break;
	case FRAME_attak108:
		flash_number = MZ2_GUNNER_GRENADE_2;
		break;
	case FRAME_attak111:
		flash_number = MZ2_GUNNER_GRENADE_3;
		break;
	default:
		flash_number = MZ2_GUNNER_GRENADE_4;
		break;
	}

	Entity->State.GetAngles().ToVectors (&forward, &right, NULL);
	G_ProjectSource (Entity->State.GetOrigin(), dumb_and_hacky_monster_MuzzFlashOffset[flash_number], forward, right, start);

	MonsterFireGrenade (start, forward, 50, 600, flash_number);
#else
	vec3f	start, forward, right, up, aim;
	int		flash_number;
	float	spread;
	float	pitch = 0;
	// PMM
	vec3f	target;	

	if(!Entity->Enemy || !Entity->Enemy->GetInUse())		//PGM
		return;									//PGM

	switch (Entity->State.GetFrame())
	{
	case FRAME_attak105:
		flash_number = MZ2_GUNNER_GRENADE_1;
		spread = .02f;
		break;
	case FRAME_attak108:
		flash_number = MZ2_GUNNER_GRENADE_2;
		spread = .05f;
		break;
	case FRAME_attak111:
		flash_number = MZ2_GUNNER_GRENADE_3;
		spread = .08f;
		break;
	default:
		flash_number = MZ2_GUNNER_GRENADE_4;
		AIFlags &= ~AI_MANUAL_STEERING;
		spread = .11f;
		break;
	}

	//	pmm
	// if we're shooting blind and we still can't see our enemy
	if ((AIFlags & AI_MANUAL_STEERING) && (!IsVisible(Entity, Entity->Enemy)))
	{
		// and we have a valid blind_fire_target
		if (BlindFireTarget == vec3fOrigin)
			return;

		target = BlindFireTarget;
	}
	else
		target = Entity->State.GetOrigin();
	// pmm

	Entity->State.GetAngles().ToVectors (&forward, &right, &up);	//PGM
	G_ProjectSource (Entity->State.GetOrigin(), dumb_and_hacky_monster_MuzzFlashOffset[flash_number], forward, right, start);

//PGM
	if(Entity->Enemy)
	{
		aim = target - Entity->State.GetOrigin();

		// aim up if they're on the same level as me and far away.
		if((aim.Length() > 512) && (aim.Z < 64) && (aim.Z > -64))
			aim.Z += (aim.Length() - 512);

		aim.NormalizeFast ();
		pitch = aim.Z;
		if(pitch > 0.4f)
			pitch = 0.4f;
		else if(pitch < -0.5f)
			pitch = -0.5f;
	}
//PGM

	aim = (forward.MultiplyAngles (spread, right)).MultiplyAngles (pitch, up);
	MonsterFireGrenade (start, aim, 50, 600, flash_number);
#endif
}

CFrame GunnerFramesAttackChain [] =
{
	/*
	CFrame (&CMonster::AI_Charge, 0),
	CFrame (&CMonster::AI_Charge, 0),
	CFrame (&CMonster::AI_Charge, 0),
	CFrame (&CMonster::AI_Charge, 0),
	CFrame (&CMonster::AI_Charge, 0),
	CFrame (&CMonster::AI_Charge, 0),
	CFrame (&CMonster::AI_Charge, 0),
	CFrame (&CMonster::AI_Charge, 0),
	*/
	CFrame (&CMonster::AI_Charge, 0, ConvertDerivedFunction(&CGunner::OpenGun)),
	CFrame (&CMonster::AI_Charge, 0),
	CFrame (&CMonster::AI_Charge, 0),
	CFrame (&CMonster::AI_Charge, 0),
	CFrame (&CMonster::AI_Charge, 0),
	CFrame (&CMonster::AI_Charge, 0),
	CFrame (&CMonster::AI_Charge, 0)
};
CAnim GunnerMoveAttackChain (FRAME_attak209, FRAME_attak215, GunnerFramesAttackChain, ConvertDerivedFunction(&CGunner::FireChain));

CFrame GunnerFramesFireChain [] =
{
	CFrame (&CMonster::AI_Charge,   0, ConvertDerivedFunction(&CGunner::Fire)),
	CFrame (&CMonster::AI_Charge,   0, ConvertDerivedFunction(&CGunner::Fire)),
	CFrame (&CMonster::AI_Charge,   0, ConvertDerivedFunction(&CGunner::Fire)),
	CFrame (&CMonster::AI_Charge,   0, ConvertDerivedFunction(&CGunner::Fire)),
	CFrame (&CMonster::AI_Charge,   0, ConvertDerivedFunction(&CGunner::Fire)),
	CFrame (&CMonster::AI_Charge,   0, ConvertDerivedFunction(&CGunner::Fire)),
	CFrame (&CMonster::AI_Charge,   0, ConvertDerivedFunction(&CGunner::Fire)),
	CFrame (&CMonster::AI_Charge,   0, ConvertDerivedFunction(&CGunner::Fire))
};
CAnim GunnerMoveFireChain (FRAME_attak216, FRAME_attak223, GunnerFramesFireChain, ConvertDerivedFunction(&CGunner::ReFireChain));

CFrame GunnerFramesEndFireChain [] =
{
	CFrame (&CMonster::AI_Charge, 0),
	CFrame (&CMonster::AI_Charge, 0),
	CFrame (&CMonster::AI_Charge, 0),
	CFrame (&CMonster::AI_Charge, 0),
	CFrame (&CMonster::AI_Charge, 0),
	CFrame (&CMonster::AI_Charge, 0),
	CFrame (&CMonster::AI_Charge, 0)
};
CAnim GunnerMoveEndFireChain (FRAME_attak224, FRAME_attak230, GunnerFramesEndFireChain, ConvertDerivedFunction(&CGunner::Run));

#ifdef MONSTER_USE_ROGUE_AI
void CGunner::BlindCheck ()
{
	if (AIFlags & AI_MANUAL_STEERING)
		IdealYaw = (BlindFireTarget - Entity->State.GetOrigin()).ToYaw();
}
#endif

CFrame GunnerFramesAttackGrenade [] =
{
#ifdef MONSTER_USE_ROGUE_AI
	CFrame (&CMonster::AI_Charge, 0, ConvertDerivedFunction(&CGunner::BlindCheck)),
#else
	CFrame (&CMonster::AI_Charge, 0),
#endif
	CFrame (&CMonster::AI_Charge, 0),
	CFrame (&CMonster::AI_Charge, 0),
	CFrame (&CMonster::AI_Charge, 0),
	CFrame (&CMonster::AI_Charge, 0, ConvertDerivedFunction(&CGunner::Grenade)),
	CFrame (&CMonster::AI_Charge, 0),
	CFrame (&CMonster::AI_Charge, 0),
	CFrame (&CMonster::AI_Charge, 0, ConvertDerivedFunction(&CGunner::Grenade)),
	CFrame (&CMonster::AI_Charge, 0),
	CFrame (&CMonster::AI_Charge, 0),
	CFrame (&CMonster::AI_Charge, 0, ConvertDerivedFunction(&CGunner::Grenade)),
	CFrame (&CMonster::AI_Charge, 0),
	CFrame (&CMonster::AI_Charge, 0),
	CFrame (&CMonster::AI_Charge, 0, ConvertDerivedFunction(&CGunner::Grenade)),
	CFrame (&CMonster::AI_Charge, 0),
	CFrame (&CMonster::AI_Charge, 0),
	CFrame (&CMonster::AI_Charge, 0),
	CFrame (&CMonster::AI_Charge, 0),
	CFrame (&CMonster::AI_Charge, 0),
	CFrame (&CMonster::AI_Charge, 0),
	CFrame (&CMonster::AI_Charge, 0)
};
CAnim GunnerMoveAttackGrenade (FRAME_attak101, FRAME_attak121, GunnerFramesAttackGrenade, ConvertDerivedFunction(&CGunner::Run));

void CGunner::Attack()
{
#ifndef MONSTER_USE_ROGUE_AI
	if (Range (Entity, Entity->Enemy) == RANGE_MELEE)
		CurrentMove = &GunnerMoveAttackChain;
	else
		CurrentMove = (frand() <= 0.5) ? &GunnerMoveAttackGrenade : &GunnerMoveAttackChain;
#else
	DoneDodge();

	// PMM 
	if (AttackState == AS_BLIND)
	{
		// minimum of 2 seconds, plus 0-3, after the shots are done
		BlindFireDelay += 2.1 + 2.0 + frand()*3.0;

		// don't shoot at the origin
		if (BlindFireTarget == vec3fOrigin)
			return;

		// don't shoot if the dice say not to
		float r = frand();
		if (BlindFireDelay < 7.5 && (r > 0.4f))
			return;
		else if (r > 0.1f)
			return;


		// turn on manual steering to signal both manual steering and blindfire
		AIFlags |= AI_MANUAL_STEERING;
		if (GrenadeCheck())
		{
			// if the check passes, go for the attack
			CurrentMove = &GunnerMoveAttackGrenade;
			AttackFinished = level.Frame + ((2*frand())*10);
		}
		// turn off blindfire flag
		AIFlags &= ~AI_MANUAL_STEERING;
		return;
	}
	// pmm

	// PGM - gunner needs to use his chaingun if he's being attacked by a tesla.
	if (Range (Entity, Entity->Enemy) == RANGE_MELEE)
		CurrentMove = &GunnerMoveAttackChain;
	else
		CurrentMove = (frand() <= 0.5 && GrenadeCheck()) ? &GunnerMoveAttackGrenade : &GunnerMoveAttackChain;
#endif
}

void CGunner::FireChain ()
{
	CurrentMove = &GunnerMoveFireChain;
}

void CGunner::ReFireChain ()
{
	if (entity_cast<CHurtableEntity>(Entity->Enemy)->Health > 0 && IsVisible (Entity, Entity->Enemy) && frand() <= 0.5)
	{
		CurrentMove = &GunnerMoveFireChain;
		return;
	}
	CurrentMove = &GunnerMoveEndFireChain;
}

#ifdef MONSTER_USE_ROGUE_AI
void CGunner::Duck (float eta)
{
	if ((CurrentMove == &GunnerMoveAttackChain) ||
		(CurrentMove == &GunnerMoveFireChain) ||
		(CurrentMove == &GunnerMoveAttackGrenade))
	{
		// if we're shooting, and not on easy, don't dodge
		if (skill->Integer())
		{
			AIFlags &= ~AI_DUCKED;
			return;
		}
	}

	if (skill->Integer() == 0)
		// PMM - stupid dodge
		DuckWaitTime = level.Frame + ((eta + 1) * 10);
	else
		DuckWaitTime = level.Frame + ((eta + (0.1 * (3 - skill->Integer()))) * 10);

	// has to be done immediately otherwise he can get stuck
	DuckDown();

	NextFrame = FRAME_duck01;
	CurrentMove = &GunnerMoveDuck;
	return;
}

void CGunner::SideStep ()
{
	if ((CurrentMove == &GunnerMoveAttackChain) ||
		(CurrentMove == &GunnerMoveFireChain) ||
		(CurrentMove == &GunnerMoveAttackGrenade))
	{
		// if we're shooting, and not on easy, don't dodge
		if (skill->Integer())
		{
			AIFlags &= ~AI_DODGING;
			return;
		}
	}

	if (CurrentMove != &GunnerMoveRun)
		CurrentMove = &GunnerMoveRun;
}
#endif

void CGunner::Spawn ()
{
	SoundDeath = SoundIndex ("gunner/death1.wav");	
	SoundPain = SoundIndex ("gunner/gunpain2.wav");	
	SoundPain2 = SoundIndex ("gunner/gunpain1.wav");	
	SoundIdle = SoundIndex ("gunner/gunidle1.wav");	
	SoundOpen = SoundIndex ("gunner/gunatck1.wav");	
	SoundSearch = SoundIndex ("gunner/gunsrch1.wav");	
	SoundSight = SoundIndex ("gunner/sight1.wav");	

	SoundIndex ("gunner/gunatck2.wav");
	SoundIndex ("gunner/gunatck3.wav");

	Entity->GetSolid() = SOLID_BBOX;
	Entity->State.GetModelIndex() = ModelIndex ("models/monsters/gunner/tris.md2");
	Entity->GetMins().Set (-16, -16, -24);
	Entity->GetMaxs().Set (16, 16, 32);

	Entity->Health = 175;
	Entity->GibHealth = -70;
	Entity->Mass = 200;

	MonsterFlags |= (MF_HAS_ATTACK | MF_HAS_SIGHT
#ifdef MONSTER_USE_ROGUE_AI
		| MF_HAS_DODGE | MF_HAS_DUCK | MF_HAS_UNDUCK | MF_HAS_SIDESTEP
#endif
		);
	Entity->Link ();

#ifdef MONSTER_USE_ROGUE_AI
	BlindFire = true;
#endif

	CurrentMove = &GunnerMoveStand;	
	WalkMonsterStart ();
}

LINK_MONSTER_CLASSNAME_TO_CLASS ("monster_gunner", CGunner);