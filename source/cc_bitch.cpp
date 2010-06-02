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
// cc_bitch.cpp
// Iron Maiden
//

#include "cc_local.h"
#include "m_chick.h"
#include "cc_bitch.h"

CMaiden::CMaiden (uint32 ID) :
CMonster(ID)
{
	Scale = MODEL_SCALE;
	MonsterName = "Iron Maiden";
}

void CMaiden::Moan ()
{
	Entity->PlaySound (CHAN_VOICE, (frand() < 0.5) ? Sounds[SOUND_IDLE1] : Sounds[SOUND_IDLE2], 255, ATTN_IDLE);
}

CFrame ChickFramesFidget [] =
{
	CFrame (&CMonster::AI_Stand, 0),
	CFrame (&CMonster::AI_Stand, 0),
	CFrame (&CMonster::AI_Stand, 0),
	CFrame (&CMonster::AI_Stand, 0),
	CFrame (&CMonster::AI_Stand, 0),
	CFrame (&CMonster::AI_Stand, 0),
	CFrame (&CMonster::AI_Stand, 0),
	CFrame (&CMonster::AI_Stand, 0),
	CFrame (&CMonster::AI_Stand, 0,  ConvertDerivedFunction(&CMaiden::Moan)),
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
CAnim ChickMoveFidget (FRAME_stand201, FRAME_stand230, ChickFramesFidget, ConvertDerivedFunction(&CMaiden::Stand));

void CMaiden::Idle ()
{
	if (AIFlags & AI_STAND_GROUND)
		return;
	if (frand() <= 0.3)
		CurrentMove = &ChickMoveFidget;
}

CFrame ChickFramesStand [] =
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
	CFrame (&CMonster::AI_Stand, 0),
	CFrame (&CMonster::AI_Stand, 0),
	CFrame (&CMonster::AI_Stand, 0),
	CFrame (&CMonster::AI_Stand, 0),
	CFrame (&CMonster::AI_Stand, 0),
	CFrame (&CMonster::AI_Stand, 0),
	CFrame (&CMonster::AI_Stand, 0),
	CFrame (&CMonster::AI_Stand, 0),
	CFrame (&CMonster::AI_Stand, 0),
};
CAnim ChickMoveStand (FRAME_stand101, FRAME_stand130, ChickFramesStand);

void CMaiden::Stand ()
{
	CurrentMove = &ChickMoveStand;
}

CFrame ChickFramesStartRun [] =
{
	CFrame (&CMonster::AI_Run, 1),
	CFrame (&CMonster::AI_Run, 0),
	CFrame (&CMonster::AI_Run, 0),
	CFrame (&CMonster::AI_Run, -1), 
	CFrame (&CMonster::AI_Run, -1), 
	CFrame (&CMonster::AI_Run, 0),
	CFrame (&CMonster::AI_Run, 1),
	CFrame (&CMonster::AI_Run, 3),
	CFrame (&CMonster::AI_Run, 6),
	CFrame (&CMonster::AI_Run, 3)
};
CAnim ChickMoveStartRun (FRAME_walk01, FRAME_walk10, ChickFramesStartRun, ConvertDerivedFunction(&CMaiden::Run));

CFrame ChickFramesRun [] =
{
	CFrame (&CMonster::AI_Run, 6),
	CFrame (&CMonster::AI_Run, 8),
	CFrame (&CMonster::AI_Run, 13),
#if ROGUE_FEATURES
	CFrame (&CMonster::AI_Run, 5, &CMonster::DoneDodge), // Make sure to clear the dodge bit
#else
	CFrame (&CMonster::AI_Run, 5),
#endif
	CFrame (&CMonster::AI_Run, 7),
	CFrame (&CMonster::AI_Run, 4),
	CFrame (&CMonster::AI_Run, 11),
	CFrame (&CMonster::AI_Run, 5),
	CFrame (&CMonster::AI_Run, 9),
	CFrame (&CMonster::AI_Run, 7)
};
CAnim ChickMoveRun (FRAME_walk11, FRAME_walk20, ChickFramesRun);

CFrame ChickFramesWalk [] =
{
	CFrame (&CMonster::AI_Walk, 6),
	CFrame (&CMonster::AI_Walk, 8),
	CFrame (&CMonster::AI_Walk, 13),
	CFrame (&CMonster::AI_Walk, 5),
	CFrame (&CMonster::AI_Walk, 7),
	CFrame (&CMonster::AI_Walk, 4),
	CFrame (&CMonster::AI_Walk, 11),
	CFrame (&CMonster::AI_Walk, 5),
	CFrame (&CMonster::AI_Walk, 9),
	CFrame (&CMonster::AI_Walk, 7)
};
CAnim ChickMoveWalk (FRAME_walk11, FRAME_walk20, ChickFramesWalk);

void CMaiden::Walk ()
{
	CurrentMove = &ChickMoveWalk;
}

void CMaiden::Run ()
{
#if ROGUE_FEATURES
	DoneDodge();
#endif

	if (AIFlags & AI_STAND_GROUND)
	{
		CurrentMove = &ChickMoveStand;
		return;
	}

	if (CurrentMove == &ChickMoveWalk ||
		CurrentMove == &ChickMoveStartRun)
		CurrentMove = &ChickMoveRun;
	else
		CurrentMove = &ChickMoveStartRun;
}

CFrame ChickFramesPain1 [] =
{
	CFrame (&CMonster::AI_Move, 0),
	CFrame (&CMonster::AI_Move, 0),
	CFrame (&CMonster::AI_Move, 0),
	CFrame (&CMonster::AI_Move, 0),
	CFrame (&CMonster::AI_Move, 0)
};
CAnim ChickMovePain1 (FRAME_pain101, FRAME_pain105, ChickFramesPain1, ConvertDerivedFunction(&CMaiden::Run));

CFrame ChickFramesPain2 [] =
{
	CFrame (&CMonster::AI_Move, 0),
	CFrame (&CMonster::AI_Move, 0),
	CFrame (&CMonster::AI_Move, 0),
	CFrame (&CMonster::AI_Move, 0),
	CFrame (&CMonster::AI_Move, 0)
};
CAnim ChickMovePain2 (FRAME_pain201, FRAME_pain205, ChickFramesPain2, ConvertDerivedFunction(&CMaiden::Run));

CFrame ChickFramesPain3 [] =
{
	CFrame (&CMonster::AI_Move, 0),
	CFrame (&CMonster::AI_Move, 0),
	CFrame (&CMonster::AI_Move, -6),
	CFrame (&CMonster::AI_Move, 3),
	CFrame (&CMonster::AI_Move, 11),
	CFrame (&CMonster::AI_Move, 3),
	CFrame (&CMonster::AI_Move, 0),
	CFrame (&CMonster::AI_Move, 0),
	CFrame (&CMonster::AI_Move, 4),
	CFrame (&CMonster::AI_Move, 1),
	CFrame (&CMonster::AI_Move, 0),
	CFrame (&CMonster::AI_Move, -3),
	CFrame (&CMonster::AI_Move, -4),
	CFrame (&CMonster::AI_Move, 5),
	CFrame (&CMonster::AI_Move, 7),
	CFrame (&CMonster::AI_Move, -2),
	CFrame (&CMonster::AI_Move, 3),
	CFrame (&CMonster::AI_Move, -5),
	CFrame (&CMonster::AI_Move, -2),
	CFrame (&CMonster::AI_Move, -8),
	CFrame (&CMonster::AI_Move, 2)
};
CAnim ChickMovePain3 (FRAME_pain301, FRAME_pain321, ChickFramesPain3, ConvertDerivedFunction(&CMaiden::Run));

void CMaiden::Pain (IBaseEntity *Other, sint32 Damage)
{
	if (Entity->Health < (Entity->MaxHealth / 2))
#if XATRIX_FEATURES
	{
		if (Entity->State.GetSkinNum() >= 2)
			Entity->State.GetSkinNum() = 2;
		else
#endif
			Entity->State.GetSkinNum() = 1;
#if XATRIX_FEATURES
	}
#endif

	if (Level.Frame < PainDebounceTime)
		return;

	PainDebounceTime = Level.Frame + 30;

	Entity->PlaySound (CHAN_VOICE, Sounds[SOUND_PAIN1+irandom(3)]);

	if (CvarList[CV_SKILL].Integer() == 3)
		return;		// no pain anims in nightmare

#if ROGUE_FEATURES
	// PMM - clear this from blindfire
	AIFlags &= ~AI_MANUAL_STEERING;

	// PMM - clear duck flag
	if (AIFlags & AI_DUCKED)
		UnDuck();
#endif

	CurrentMove = (Damage <= 10) ? &ChickMovePain1 : ((Damage <= 25) ? &ChickMovePain2 : &ChickMovePain3);
}

void CMaiden::Dead ()
{
	Entity->GetMins().Set (-16, -16, 0);
	Entity->GetMaxs().Set (16, 16, 16);
	Entity->PhysicsType = PHYSICS_TOSS;
	Entity->GetSvFlags() |= SVF_DEADMONSTER;
	Entity->NextThink = 0;
	Entity->Link ();
}

CFrame ChickFramesDeath2 [] =
{
	CFrame (&CMonster::AI_Move, -6),
	CFrame (&CMonster::AI_Move, 0),
	CFrame (&CMonster::AI_Move, -1),
	CFrame (&CMonster::AI_Move, -5),
	CFrame (&CMonster::AI_Move, 0),
	CFrame (&CMonster::AI_Move, -1),
	CFrame (&CMonster::AI_Move, -2),
	CFrame (&CMonster::AI_Move, 1),
	CFrame (&CMonster::AI_Move, 10),
	CFrame (&CMonster::AI_Move, 2),
	CFrame (&CMonster::AI_Move, 3),
	CFrame (&CMonster::AI_Move, 1),
	CFrame (&CMonster::AI_Move, 2),
	CFrame (&CMonster::AI_Move, 0),
	CFrame (&CMonster::AI_Move, 3),
	CFrame (&CMonster::AI_Move, 3),
	CFrame (&CMonster::AI_Move, 1),
	CFrame (&CMonster::AI_Move, -3),
	CFrame (&CMonster::AI_Move, -5),
	CFrame (&CMonster::AI_Move, 4),
	CFrame (&CMonster::AI_Move, 15),
	CFrame (&CMonster::AI_Move, 14),
	CFrame (&CMonster::AI_Move, 1)
};
CAnim ChickMoveDeath2 (FRAME_death201, FRAME_death223, ChickFramesDeath2, ConvertDerivedFunction(&CMaiden::Dead));

CFrame ChickFramesDeath1 [] =
{
	CFrame (&CMonster::AI_Move, 0),
	CFrame (&CMonster::AI_Move, 0),
	CFrame (&CMonster::AI_Move, -7),
	CFrame (&CMonster::AI_Move, 4),
	CFrame (&CMonster::AI_Move, 11),
	CFrame (&CMonster::AI_Move, 0),
	CFrame (&CMonster::AI_Move, 0),
	CFrame (&CMonster::AI_Move, 0),
	CFrame (&CMonster::AI_Move, 0),
	CFrame (&CMonster::AI_Move, 0),
	CFrame (&CMonster::AI_Move, 0),
	CFrame (&CMonster::AI_Move, 0)
};
CAnim ChickMoveDeath1 (FRAME_death101, FRAME_death112, ChickFramesDeath1, ConvertDerivedFunction(&CMaiden::Dead));

void CMaiden::Die (IBaseEntity *Inflictor, IBaseEntity *Attacker, sint32 Damage, vec3f &point)
{
	sint32		n;

// check for gib
	if (Entity->Health <= Entity->GibHealth)
	{
		Entity->PlaySound (CHAN_VOICE, SoundIndex ("misc/udeath.wav"));
		for (n= 0; n < 2; n++)
			CGibEntity::Spawn (Entity, GameMedia.Gib_Bone[0], Damage, GIB_ORGANIC);
		for (n= 0; n < 4; n++)
			CGibEntity::Spawn (Entity, GameMedia.Gib_SmallMeat, Damage, GIB_ORGANIC);
		Entity->ThrowHead (GameMedia.Gib_Head[1], Damage, GIB_ORGANIC);
		Entity->DeadFlag = true;
		return;
	}

	if (Entity->DeadFlag == true)
		return;

// regular death
	Entity->DeadFlag = true;
	Entity->CanTakeDamage = true;

	n = irandom(2);
	CurrentMove = (!n) ? &ChickMoveDeath1 : &ChickMoveDeath2;
	Entity->PlaySound (CHAN_VOICE, (!n) ? Sounds[SOUND_DEATH1] : Sounds[SOUND_DEATH2]);
}

#if !ROGUE_FEATURES
void CMaiden::DuckDown ()
{
	if (AIFlags & AI_DUCKED)
		return;
	AIFlags |= AI_DUCKED;
	Entity->GetMaxs().Z -= 32;
	Entity->CanTakeDamage = true;
	PauseTime = Level.Frame + 10;
	Entity->Link ();
}

void CMaiden::DuckHold ()
{
	if (Level.Frame >= PauseTime)
		AIFlags &= ~AI_HOLD_FRAME;
	else
		AIFlags |= AI_HOLD_FRAME;
}

void CMaiden::DuckUp ()
{
	AIFlags &= ~AI_DUCKED;
	Entity->GetMaxs().Z += 32;
	Entity->CanTakeDamage = true;
	Entity->Link ();
}

CFrame ChickFramesDuck [] =
{
	CFrame (&CMonster::AI_Move, 0, ConvertDerivedFunction(&CMaiden::DuckDown)),
	CFrame (&CMonster::AI_Move, 1),
	CFrame (&CMonster::AI_Move, 4, ConvertDerivedFunction(&CMaiden::DuckHold)),
	CFrame (&CMonster::AI_Move, -4),
	CFrame (&CMonster::AI_Move, -5, ConvertDerivedFunction(&CMaiden::DuckUp)),
	CFrame (&CMonster::AI_Move, 3),
	CFrame (&CMonster::AI_Move, 1)
};
#else
CFrame ChickFramesDuck [] =
{
	CFrame (&CMonster::AI_Move, 0, &CMonster::DuckDown),
	CFrame (&CMonster::AI_Move, 1),
	CFrame (&CMonster::AI_Move, 4, &CMonster::DuckHold),
	CFrame (&CMonster::AI_Move, -4),
	CFrame (&CMonster::AI_Move, -5, &CMonster::UnDuck),
	CFrame (&CMonster::AI_Move, 3),
	CFrame (&CMonster::AI_Move, 1)
};
#endif

CAnim ChickMoveDuck (FRAME_duck01, FRAME_duck07, ChickFramesDuck, ConvertDerivedFunction(&CMaiden::Run));

CFrame ChickFramesStartAttack1 [] =
{
	CFrame (&CMonster::AI_Charge, 0,	ConvertDerivedFunction(&CMaiden::PreAttack)),
	CFrame (&CMonster::AI_Charge, 0),
	CFrame (&CMonster::AI_Charge, 0),
	CFrame (&CMonster::AI_Charge, 4),
	CFrame (&CMonster::AI_Charge, 0),
	CFrame (&CMonster::AI_Charge, -3),
	CFrame (&CMonster::AI_Charge, 3),
	CFrame (&CMonster::AI_Charge, 5),
	CFrame (&CMonster::AI_Charge, 7),
	CFrame (&CMonster::AI_Charge, 0),
	CFrame (&CMonster::AI_Charge, 0),
	CFrame (&CMonster::AI_Charge, 0),
	CFrame (&CMonster::AI_Charge, 0, ConvertDerivedFunction(&CMaiden::Attack1))
};
CAnim ChickMoveStartAttack1 (FRAME_attak101, FRAME_attak113, ChickFramesStartAttack1);


CFrame ChickFramesAttack1 [] =
{
	CFrame (&CMonster::AI_Charge, 19, ConvertDerivedFunction(&CMaiden::Rocket)),
	CFrame (&CMonster::AI_Charge, -6),
	CFrame (&CMonster::AI_Charge, -5),
	CFrame (&CMonster::AI_Charge, -2),
	CFrame (&CMonster::AI_Charge, -7),
	CFrame (&CMonster::AI_Charge, 0),
	CFrame (&CMonster::AI_Charge, 1),
	CFrame (&CMonster::AI_Charge, 10, ConvertDerivedFunction(&CMaiden::Reload)),
	CFrame (&CMonster::AI_Charge, 4),
	CFrame (&CMonster::AI_Charge, 5),
	CFrame (&CMonster::AI_Charge, 6),
	CFrame (&CMonster::AI_Charge, 6),
	CFrame (&CMonster::AI_Charge, 4),
	CFrame (&CMonster::AI_Charge, 3, ConvertDerivedFunction(&CMaiden::ReRocket))
};
CAnim ChickMoveAttack1 (FRAME_attak114, FRAME_attak127, ChickFramesAttack1);

CFrame ChickFramesEndAttack1 [] =
{
	CFrame (&CMonster::AI_Charge, -3),
	CFrame (&CMonster::AI_Charge, 0),
	CFrame (&CMonster::AI_Charge, -6),
	CFrame (&CMonster::AI_Charge, -4),
	CFrame (&CMonster::AI_Charge, -2)
};
CAnim ChickMoveEndAttack1 (FRAME_attak128, FRAME_attak132, ChickFramesEndAttack1, ConvertDerivedFunction(&CMaiden::Run));

#if ROGUE_FEATURES
void CMaiden::Duck (float eta)
{
	if ((CurrentMove == &ChickMoveStartAttack1) ||
		(CurrentMove == &ChickMoveAttack1))
	{
		// if we're shooting, and not on easy, don't dodge
		if (CvarList[CV_SKILL].Integer())
		{
			AIFlags &= ~AI_DUCKED;
			return;
		}
	}

	if (!CvarList[CV_SKILL].Integer())
		// PMM - stupid dodge
		DuckWaitTime = Level.Frame + ((eta + 1 * 10));
	else
		DuckWaitTime = Level.Frame + ((eta + (0.1 * (3 - CvarList[CV_SKILL].Integer())) * 10));

	// has to be done immediately otherwise she can get stuck
	DuckDown();

	NextFrame = FRAME_duck01;
	CurrentMove = &ChickMoveDuck;
	return;
}

void CMaiden::SideStep ()
{
	if ((CurrentMove == &ChickMoveStartAttack1) ||
		(CurrentMove == &ChickMoveAttack1))
	{
		// if we're shooting, and not on easy, don't dodge
		if (CvarList[CV_SKILL].Integer())
		{
			AIFlags &= ~AI_DODGING;
			return;
		}
	}

	if (CurrentMove != &ChickMoveRun)
		CurrentMove = &ChickMoveRun;
}
#else
void CMaiden::Dodge (IBaseEntity *Attacker, float eta)
{
	if (frand() > 0.25)
		return;

	if (!Entity->Enemy.IsValid())
		Entity->Enemy = Attacker;

	CurrentMove = &ChickMoveDuck;
}
#endif

void CMaiden::Slash ()
{
	vec3f	aim (MELEE_DISTANCE, Entity->GetMins().X, 10);
	Entity->PlaySound (CHAN_WEAPON, Sounds[SOUND_MELEE_SWING]);
	CMeleeWeapon::Fire (Entity, aim, (10 + (irandom(6))), 100);
}

void CMaiden::Rocket ()
{
	if (!HasValidEnemy())
		return;

#if ROGUE_FEATURES
	vec3f	forward, right, start, dir, vec, target;
	bool blindfire = (AIFlags & AI_MANUAL_STEERING) ? true : false;

	Entity->State.GetAngles().ToVectors (&forward, &right, NULL);
	G_ProjectSource (Entity->State.GetOrigin(), MonsterFlashOffsets[MZ2_CHICK_ROCKET_1], forward, right, start);

	sint32 rocketSpeed = 500 + (100 * CvarList[CV_SKILL].Integer());	// PGM rock & roll.... :)

	target = (blindfire) ? BlindFireTarget : Entity->Enemy->State.GetOrigin();
	if (blindfire)
	{
		vec = target;
		dir = vec - start;
	}
	// pmm
	// don't shoot at feet if they're above where i'm shooting from.
	else if(frand() < 0.33 || (start[2] < Entity->Enemy->GetAbsMin().Z))
	{
		vec = target;
		vec.Z += Entity->Enemy->ViewHeight;
		dir = vec - start;
	}
	else
	{
		vec = target;
		vec.Z = Entity->Enemy->GetAbsMin().Z;
		dir = vec - start;
	}

	// Lead target  (not when blindfiring)
	// 20, 35, 50, 65 chance of leading
	if((!blindfire) && ((frand() < (0.2 + ((3 - CvarList[CV_SKILL].Integer()) * 0.15)))))
	{
		vec = vec.MultiplyAngles (dir.Length() / rocketSpeed, entity_cast<IPhysicsEntity>(*Entity->Enemy)->Velocity);
		dir = vec - start;
	}

	dir.Normalize ();

	// pmm blindfire doesn't check target (done in checkattack)
	// paranoia, make sure we're not shooting a target right next to us
	CTrace trace (start, vec, Entity, CONTENTS_MASK_SHOT);
	if (blindfire)
	{
		// blindfire has different fail criteria for the trace
		if (!(trace.startSolid || trace.allSolid || (trace.fraction < 0.5)))
			MonsterFireRocket (start, dir, 50, rocketSpeed, MZ2_CHICK_ROCKET_1);
		else 
		{
			// geez, this is bad.  she's avoiding about 80% of her blindfires due to hitting things.
			// hunt around for a good shot
			// try shifting the target to the left a little (to help counter her large offset)
			vec = target;
			vec = vec.MultiplyAngles (-10, right);
			dir = vec - start;
			dir.NormalizeFast();
			trace (start, vec, Entity, CONTENTS_MASK_SHOT);
			if (!(trace.startSolid || trace.allSolid || (trace.fraction < 0.5)))
				MonsterFireRocket (start, dir, 50, rocketSpeed, MZ2_CHICK_ROCKET_1);
			else 
			{
				// ok, that failed.  try to the right
				vec = target;
				vec = vec.MultiplyAngles (10, right);
				dir = vec - start;
				dir.NormalizeFast();
				trace (start, vec, Entity, CONTENTS_MASK_SHOT);
				if (!(trace.startSolid || trace.allSolid || (trace.fraction < 0.5)))
					MonsterFireRocket (start, dir, 50, rocketSpeed, MZ2_CHICK_ROCKET_1);
			}
		}
	}
	else
		MonsterFireRocket (start, dir, 50, rocketSpeed, MZ2_CHICK_ROCKET_1);
#else
	vec3f	forward, right, start, dir, vec;

	Entity->State.GetAngles().ToVectors (&forward, &right, NULL);
	G_ProjectSource (Entity->State.GetOrigin(), MonsterFlashOffsets[MZ2_CHICK_ROCKET_1], forward, right, start);

	vec = Entity->Enemy->State.GetOrigin();
	vec.Z += Entity->Enemy->ViewHeight;
	dir = vec - start;
	dir.NormalizeFast ();

	MonsterFireRocket (start, dir, 50, 500, MZ2_CHICK_ROCKET_1);
#endif
}	

void CMaiden::PreAttack ()
{
	Entity->PlaySound (CHAN_VOICE, Sounds[SOUND_MISSILE_PRELAUNCH]);
}

void CMaiden::Reload ()
{
	Entity->PlaySound (CHAN_VOICE, Sounds[SOUND_MISSILE_RELOAD]);
}

void CMaiden::ReRocket()
{
#if ROGUE_FEATURES
	if (AIFlags & AI_MANUAL_STEERING)
		AIFlags &= ~AI_MANUAL_STEERING;
	else
#endif
	if (entity_cast<IHurtableEntity>(*Entity->Enemy)->Health > 0)
	{
		if (Range (Entity, *Entity->Enemy) > RANGE_MELEE &&
			IsVisible (Entity, *Entity->Enemy) &&
			(frand() <= (0.6 + (0.05*CvarList[CV_SKILL].Float()))))
		{
			CurrentMove = &ChickMoveAttack1;
			return;
		}
	}	
	CurrentMove = &ChickMoveEndAttack1;
}

void CMaiden::Attack1()
{
	CurrentMove = &ChickMoveAttack1;
}

CFrame ChickFramesSlash [] =
{
	CFrame (&CMonster::AI_Charge, 1),
	CFrame (&CMonster::AI_Charge, 7, ConvertDerivedFunction(&CMaiden::Slash)),
	CFrame (&CMonster::AI_Charge, -7),
	CFrame (&CMonster::AI_Charge, 1),
	CFrame (&CMonster::AI_Charge, -1),
	CFrame (&CMonster::AI_Charge, 1),
	CFrame (&CMonster::AI_Charge, 0),
	CFrame (&CMonster::AI_Charge, 1),
	CFrame (&CMonster::AI_Charge, -2, ConvertDerivedFunction(&CMaiden::ReSlash))
};
CAnim ChickMoveSlash (FRAME_attak204, FRAME_attak212, ChickFramesSlash);

CFrame ChickFramesEndSlash [] =
{
	CFrame (&CMonster::AI_Charge, -6),
	CFrame (&CMonster::AI_Charge, -1),
	CFrame (&CMonster::AI_Charge, -6),
	CFrame (&CMonster::AI_Charge, 0)
};
CAnim ChickMoveEndSlash (FRAME_attak213, FRAME_attak216, ChickFramesEndSlash, ConvertDerivedFunction(&CMaiden::Run));

void CMaiden::ReSlash()
{
	if (entity_cast<IHurtableEntity>(*Entity->Enemy)->Health > 0 && (Range (Entity, *Entity->Enemy) == RANGE_MELEE) && (frand() <= 0.9))
		CurrentMove = &ChickMoveSlash;
	else
		CurrentMove = &ChickMoveEndSlash;
}

void CMaiden::DoSlash()
{
	CurrentMove = &ChickMoveSlash;
}

CFrame ChickFramesStartSlash [] =
{	
	CFrame (&CMonster::AI_Charge, 1),
	CFrame (&CMonster::AI_Charge, 8),
	CFrame (&CMonster::AI_Charge, 3)
};
CAnim ChickMoveStartSlash (FRAME_attak201, FRAME_attak203, ChickFramesStartSlash, ConvertDerivedFunction(&CMaiden::DoSlash));

void CMaiden::Melee()
{
	CurrentMove = &ChickMoveStartSlash;
}

void CMaiden::Attack()
{
#if ROGUE_FEATURES
	DoneDodge ();

	// PMM 
	if (AttackState == AS_BLIND)
	{
		// minimum of 2 seconds, plus 0-3, after the shots are done
		BlindFireDelay += 2.0 + (4.5 * frand());

		// don't shoot at the origin
		if (BlindFireTarget == vec3fOrigin)
			return;

		// don't shoot if the dice say not to
		float r = frand();
		if (BlindFireDelay < 7.5f && (r > 0.4))
			return;
		else if (r > 0.1)
			return;

		// turn on manual steering to signal both manual steering and blindfire
		AIFlags |= AI_MANUAL_STEERING;
		CurrentMove = &ChickMoveStartAttack1;
		AttackFinished = Level.Frame + ((2*frand())*10);
		return;
	}
	// pmm
#endif
	CurrentMove = &ChickMoveStartAttack1;
}

void CMaiden::Sight()
{
	Entity->PlaySound (CHAN_VOICE, Sounds[SOUND_SIGHT]);
}

void CMaiden::Spawn ()
{
	Entity->GetSolid() = SOLID_BBOX;
	Entity->State.GetModelIndex() = ModelIndex("models/monsters/bitch/tris.md2");
	Entity->GetMins().Set (-16, -16, 0);
	Entity->GetMaxs().Set (16, 16, 56);

	Sounds[SOUND_MISSILE_PRELAUNCH]	= SoundIndex ("chick/chkatck1.wav");	
	Sounds[SOUND_MISSILE_LAUNCH]	= SoundIndex ("chick/chkatck2.wav");	
	Sounds[SOUND_MELEE_SWING]		= SoundIndex ("chick/chkatck3.wav");	
	Sounds[SOUND_MELEE_HIT]			= SoundIndex ("chick/chkatck4.wav");	
	Sounds[SOUND_MISSILE_RELOAD]	= SoundIndex ("chick/chkatck5.wav");	
	Sounds[SOUND_DEATH1]			= SoundIndex ("chick/chkdeth1.wav");	
	Sounds[SOUND_DEATH2]			= SoundIndex ("chick/chkdeth2.wav");	
	Sounds[SOUND_FALL_DOWN]			= SoundIndex ("chick/chkfall1.wav");	
	Sounds[SOUND_IDLE1]				= SoundIndex ("chick/chkidle1.wav");	
	Sounds[SOUND_IDLE2]				= SoundIndex ("chick/chkidle2.wav");	
	Sounds[SOUND_PAIN1]				= SoundIndex ("chick/chkpain1.wav");	
	Sounds[SOUND_PAIN2]				= SoundIndex ("chick/chkpain2.wav");	
	Sounds[SOUND_PAIN3]				= SoundIndex ("chick/chkpain3.wav");	
	Sounds[SOUND_SIGHT]				= SoundIndex ("chick/chksght1.wav");	
	Sounds[SOUND_SEARCH]			= SoundIndex ("chick/chksrch1.wav");	

	Entity->Health = 175;
	Entity->GibHealth = -70;
	Entity->Mass = 200;

	MonsterFlags = (MF_HAS_MELEE | MF_HAS_ATTACK | MF_HAS_IDLE | MF_HAS_SIGHT
#if ROGUE_FEATURES
		| MF_HAS_DODGE | MF_HAS_DUCK | MF_HAS_UNDUCK | MF_HAS_SIDESTEP
#endif
		);

#if ROGUE_FEATURES
	BlindFire = true;
#endif

	Entity->Link ();

	CurrentMove = &ChickMoveStand;
	WalkMonsterStart ();
}

LINK_MONSTER_CLASSNAME_TO_CLASS ("monster_chick", CMaiden);
