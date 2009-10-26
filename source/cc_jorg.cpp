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
// cc_jorg.cpp
// 
//

#include "cc_local.h"
#include "cc_makron.h"
#include "cc_jorg.h"
#include "m_boss31.h"

CJorg::CJorg (uint32 ID) :
CMonster (ID)
{
	Scale = MODEL_SCALE;
	MonsterName = "JORG";
}

void CJorg::Search ()
{
	switch (irandom(3))
	{
	case 0:
		Entity->PlaySound (CHAN_VOICE, SoundSearch1);
		break;
	case 1:
		Entity->PlaySound (CHAN_VOICE, SoundSearch2);
		break;
	case 2:
		Entity->PlaySound (CHAN_VOICE, SoundSearch3);
		break;
	};
}

//
// stand
//

CFrame JorgFramesStand []=
{
	CFrame (&CMonster::AI_Stand, 0, &CMonster::Idle),
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
	CFrame (&CMonster::AI_Stand, 19),
	CFrame (&CMonster::AI_Stand, 11, ConvertDerivedFunction(&CJorg::StepLeft)),
	CFrame (&CMonster::AI_Stand, 0),
	CFrame (&CMonster::AI_Stand, 0),
	CFrame (&CMonster::AI_Stand, 6),
	CFrame (&CMonster::AI_Stand, 9, ConvertDerivedFunction(&CJorg::StepRight)),
	CFrame (&CMonster::AI_Stand, 0),		// 40
	CFrame (&CMonster::AI_Stand, 0),
	CFrame (&CMonster::AI_Stand, 0),
	CFrame (&CMonster::AI_Stand, 0),
	CFrame (&CMonster::AI_Stand, 0),
	CFrame (&CMonster::AI_Stand, 0),
	CFrame (&CMonster::AI_Stand, 0),
	CFrame (&CMonster::AI_Stand, -2),
	CFrame (&CMonster::AI_Stand, -17, ConvertDerivedFunction(&CJorg::StepLeft)),
	CFrame (&CMonster::AI_Stand, 0),
	CFrame (&CMonster::AI_Stand, -12),		// 50
	CFrame (&CMonster::AI_Stand, -14, ConvertDerivedFunction(&CJorg::StepRight))	// 51
};
CAnim	JorgMoveStand (FRAME_stand01, FRAME_stand51, JorgFramesStand);

void CJorg::Idle ()
{
	Entity->PlaySound (CHAN_VOICE, SoundIdle);
}

void CJorg::DeathHit ()
{
	Entity->PlaySound (CHAN_BODY, SoundDeathHit);
}

void CJorg::StepLeft ()
{
	Entity->PlaySound (CHAN_BODY, SoundStepLeft);
}

void CJorg::StepRight ()
{
	Entity->PlaySound (CHAN_BODY, SoundStepRight);
}

void CJorg::Stand ()
{
	CurrentMove = &JorgMoveStand;
}

CFrame JorgFramesRun [] =
{
	CFrame (&CMonster::AI_Run, 17,	ConvertDerivedFunction(&CJorg::StepLeft)),
	CFrame (&CMonster::AI_Run, 0),
	CFrame (&CMonster::AI_Run, 0),
	CFrame (&CMonster::AI_Run, 0),
	CFrame (&CMonster::AI_Run, 12),
	CFrame (&CMonster::AI_Run, 8),
	CFrame (&CMonster::AI_Run, 10),
	CFrame (&CMonster::AI_Run, 33,	ConvertDerivedFunction(&CJorg::StepRight)),
	CFrame (&CMonster::AI_Run, 0),
	CFrame (&CMonster::AI_Run, 0),
	CFrame (&CMonster::AI_Run, 0),
	CFrame (&CMonster::AI_Run, 9),
	CFrame (&CMonster::AI_Run, 9),
	CFrame (&CMonster::AI_Run, 9)
};
CAnim	JorgMoveRun (FRAME_walk06, FRAME_walk19, JorgFramesRun);

//
// walk
//

#if 0
CFrame JorgFramesStartWalk [] =
{
	CFrame (&CMonster::AI_Walk,	5),
	CFrame (&CMonster::AI_Walk,	6),
	CFrame (&CMonster::AI_Walk,	7),
	CFrame (&CMonster::AI_Walk,	9),
	CFrame (&CMonster::AI_Walk,	15)
};
CAnim JorgMoveStartWalk (FRAME_walk01, FRAME_walk05, JorgFramesStartWalk);
#endif

CFrame JorgFramesWalk [] =
{
	CFrame (&CMonster::AI_Walk, 17),
	CFrame (&CMonster::AI_Walk, 0),
	CFrame (&CMonster::AI_Walk, 0),
	CFrame (&CMonster::AI_Walk, 0),
	CFrame (&CMonster::AI_Walk, 12),
	CFrame (&CMonster::AI_Walk, 8),
	CFrame (&CMonster::AI_Walk, 10),
	CFrame (&CMonster::AI_Walk, 33),
	CFrame (&CMonster::AI_Walk, 0),
	CFrame (&CMonster::AI_Walk, 0),
	CFrame (&CMonster::AI_Walk, 0),
	CFrame (&CMonster::AI_Walk, 9),
	CFrame (&CMonster::AI_Walk, 9),
	CFrame (&CMonster::AI_Walk, 9)
};
CAnim	JorgMoveWalk (FRAME_walk06, FRAME_walk19, JorgFramesWalk);

#if 0
CFrame jorg_frames_end_walk [] =
{
	CFrame (&CMonster::AI_Walk,	11),
	CFrame (&CMonster::AI_Walk,	0),
	CFrame (&CMonster::AI_Walk,	0),
	CFrame (&CMonster::AI_Walk,	0),
	CFrame (&CMonster::AI_Walk,	8),
	CFrame (&CMonster::AI_Walk,	-8,	NULL
};
CAnim jorg_move_end_walk = {FRAME_walk20, FRAME_walk25, jorg_frames_end_walk, NULL};
#endif

void CJorg::Walk ()
{
	CurrentMove = &JorgMoveWalk;
}

void CJorg::Run ()
{
	CurrentMove = (AIFlags & AI_STAND_GROUND) ? &JorgMoveStand : &JorgMoveRun;
}

CFrame JorgFramesPain3 [] =
{
	CFrame (&CMonster::AI_Move,	-28),
	CFrame (&CMonster::AI_Move,	-6),
	CFrame (&CMonster::AI_Move,	-3,	ConvertDerivedFunction(&CJorg::StepLeft)),
	CFrame (&CMonster::AI_Move,	-9),
	CFrame (&CMonster::AI_Move,	0,	ConvertDerivedFunction(&CJorg::StepRight)),
	CFrame (&CMonster::AI_Move,	0),
	CFrame (&CMonster::AI_Move,	0),
	CFrame (&CMonster::AI_Move,	0),
	CFrame (&CMonster::AI_Move,	-7),
	CFrame (&CMonster::AI_Move,	1),
	CFrame (&CMonster::AI_Move,	-11),
	CFrame (&CMonster::AI_Move,	-4),
	CFrame (&CMonster::AI_Move,	0),
	CFrame (&CMonster::AI_Move,	0),
	CFrame (&CMonster::AI_Move,	10),
	CFrame (&CMonster::AI_Move,	11),
	CFrame (&CMonster::AI_Move,	0),
	CFrame (&CMonster::AI_Move,	10),
	CFrame (&CMonster::AI_Move,	3),
	CFrame (&CMonster::AI_Move,	10),
	CFrame (&CMonster::AI_Move,	7,	ConvertDerivedFunction(&CJorg::StepLeft)),
	CFrame (&CMonster::AI_Move,	17),
	CFrame (&CMonster::AI_Move,	0),
	CFrame (&CMonster::AI_Move,	0),
	CFrame (&CMonster::AI_Move,	0,	ConvertDerivedFunction(&CJorg::StepRight))
};
CAnim JorgMovePain3 (FRAME_pain301, FRAME_pain325, JorgFramesPain3, &CMonster::Run);

CFrame JorgFramesPain2 [] =
{
	CFrame (&CMonster::AI_Move,	0),
	CFrame (&CMonster::AI_Move,	0),
	CFrame (&CMonster::AI_Move,	0)
};
CAnim JorgMovePain2 (FRAME_pain201, FRAME_pain203, JorgFramesPain2, &CMonster::Run);

CFrame JorgFramesPain1 [] =
{
	CFrame (&CMonster::AI_Move,	0),
	CFrame (&CMonster::AI_Move,	0),
	CFrame (&CMonster::AI_Move,	0)
};
CAnim JorgMovePain1 (FRAME_pain101, FRAME_pain103, JorgFramesPain1, &CMonster::Run);

void CJorg::Pain (CBaseEntity *other, float kick, int damage)
{
	if (Entity->Health < (Entity->MaxHealth / 2))
			Entity->State.GetSkinNum() = 1;
	
	Entity->State.GetSound() = 0;

	if (level.Frame < PainDebounceTime)
			return;

	// Lessen the chance of him going into his pain frames if he takes little damage
	if ((damage <= 40) && (frand() <= 0.6))
		return;

	/* 
	If he's entering his attack1 or using attack1, lessen the chance of him
	going into pain
	*/
	
	int frame = Entity->State.GetFrame();
	if (((frame >= FRAME_attak101) && (frame <= FRAME_attak108)) && (frand() <= 0.005))
		return;

	if (((frame >= FRAME_attak109) && (frame <= FRAME_attak114)) && (frand() <= 0.00005))
		return;

	if (((frame >= FRAME_attak201) && (frame <= FRAME_attak208)) && (frand() <= 0.005))
		return;

	PainDebounceTime = level.Frame + 30;
	if (skill->Integer() == 3)
		return;		// no pain anims in nightmare

	if (damage <= 50)
	{
		Entity->PlaySound (CHAN_VOICE, SoundPain1);
		CurrentMove = &JorgMovePain1;
	}
	else if (damage <= 100)
	{
		Entity->PlaySound (CHAN_VOICE, SoundPain2);
		CurrentMove = &JorgMovePain2;
	}
	else if (frand() <= 0.3)
	{
		Entity->PlaySound (CHAN_VOICE, SoundPain3);
		CurrentMove = &JorgMovePain3;
	}
};

CFrame JorgFramesDeath1 [] =
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
	CFrame (&CMonster::AI_Move,	0),		// 10
	CFrame (&CMonster::AI_Move,	0),
	CFrame (&CMonster::AI_Move,	0),
	CFrame (&CMonster::AI_Move,	0),
	CFrame (&CMonster::AI_Move,	0),
	CFrame (&CMonster::AI_Move,	0),
	CFrame (&CMonster::AI_Move,	0),
	CFrame (&CMonster::AI_Move,	0),
	CFrame (&CMonster::AI_Move,	0),
	CFrame (&CMonster::AI_Move,	0),
	CFrame (&CMonster::AI_Move,	0),		// 20
	CFrame (&CMonster::AI_Move,	0),
	CFrame (&CMonster::AI_Move,	0),
	CFrame (&CMonster::AI_Move,	0),
	CFrame (&CMonster::AI_Move,	0),			
	CFrame (&CMonster::AI_Move,	0),
	CFrame (&CMonster::AI_Move,	0),
	CFrame (&CMonster::AI_Move,	0),			
	CFrame (&CMonster::AI_Move,	0),
	CFrame (&CMonster::AI_Move,	0),
	CFrame (&CMonster::AI_Move,	0),		// 30
	CFrame (&CMonster::AI_Move,	0),
	CFrame (&CMonster::AI_Move,	0),
	CFrame (&CMonster::AI_Move,	0),
	CFrame (&CMonster::AI_Move,	0),
	CFrame (&CMonster::AI_Move,	0),
	CFrame (&CMonster::AI_Move,	0),
	CFrame (&CMonster::AI_Move,	0),
	CFrame (&CMonster::AI_Move,	0),
	CFrame (&CMonster::AI_Move,	0),
	CFrame (&CMonster::AI_Move,	0),		// 40
	CFrame (&CMonster::AI_Move,	0),
	CFrame (&CMonster::AI_Move,	0),
	CFrame (&CMonster::AI_Move,	0),
	CFrame (&CMonster::AI_Move,	0),			
	CFrame (&CMonster::AI_Move,	0),
	CFrame (&CMonster::AI_Move,	0),
	CFrame (&CMonster::AI_Move,	0,  ConvertDerivedFunction(&CJorg::DeathHit)),			
	CFrame (&CMonster::AI_Move,	0),
	CFrame (&CMonster::AI_Move,	0,	ConvertDerivedFunction(&CJorg::TossMakron)),
	CFrame (&CMonster::AI_Move,	0,	&CMonster::BossExplode)		// 50
};
CAnim JorgMoveDeath (FRAME_death01, FRAME_death50, JorgFramesDeath1, NULL);

void CJorg::TossMakron ()
{
	CMakronJumpTimer::Spawn (this);
};

void CJorg::Die (CBaseEntity *inflictor, CBaseEntity *attacker, int damage, vec3f &point)
{
	Entity->PlaySound (CHAN_VOICE, SoundDeath);
	Entity->DeadFlag = true;
	Entity->CanTakeDamage = false;
	Entity->State.GetSound() = 0;
	ExplodeCount = 0;
	CurrentMove = &JorgMoveDeath;
}

CFrame JorgFramesAttack2 []=
{
	CFrame (&CMonster::AI_Charge,	0),
	CFrame (&CMonster::AI_Charge,	0),
	CFrame (&CMonster::AI_Charge,	0),
	CFrame (&CMonster::AI_Charge,	0),
	CFrame (&CMonster::AI_Charge,	0),
	CFrame (&CMonster::AI_Charge,	0),
	CFrame (&CMonster::AI_Charge,	0,	ConvertDerivedFunction(&CJorg::FireBFG)),		
	CFrame (&CMonster::AI_Move,	0),
	CFrame (&CMonster::AI_Move,	0),
	CFrame (&CMonster::AI_Move,	0),
	CFrame (&CMonster::AI_Move,	0),
	CFrame (&CMonster::AI_Move,	0),
	CFrame (&CMonster::AI_Move,	0)
};
CAnim JorgMoveAttack2 (FRAME_attak201, FRAME_attak213, JorgFramesAttack2, &CMonster::Run);

CFrame JorgFramesStartAttack1 [] =
{
	CFrame (&CMonster::AI_Charge,	0),
	CFrame (&CMonster::AI_Charge,	0),
	CFrame (&CMonster::AI_Charge,	0),
	CFrame (&CMonster::AI_Charge,	0),
	CFrame (&CMonster::AI_Charge,	0),
	CFrame (&CMonster::AI_Charge,	0),
	CFrame (&CMonster::AI_Charge,	0),
	CFrame (&CMonster::AI_Charge,	0)
};
CAnim JorgMoveStartAttack1 (FRAME_attak101, FRAME_attak108, JorgFramesStartAttack1, ConvertDerivedFunction(&CJorg::DoChainguns));

CFrame JorgFramesAttack1[]=
{
	CFrame (&CMonster::AI_Charge,	0,	ConvertDerivedFunction(&CJorg::FireBullet)),
	CFrame (&CMonster::AI_Charge,	0,	ConvertDerivedFunction(&CJorg::FireBullet)),
	CFrame (&CMonster::AI_Charge,	0,	ConvertDerivedFunction(&CJorg::FireBullet)),
	CFrame (&CMonster::AI_Charge,	0,	ConvertDerivedFunction(&CJorg::FireBullet)),
	CFrame (&CMonster::AI_Charge,	0,	ConvertDerivedFunction(&CJorg::FireBullet)),
	CFrame (&CMonster::AI_Charge,	0,	ConvertDerivedFunction(&CJorg::FireBullet))
};
CAnim JorgMoveAttack1 (FRAME_attak109, FRAME_attak114, JorgFramesAttack1, ConvertDerivedFunction(&CJorg::ReAttack1));

CFrame JorgFramesEndAttack1[]=
{
	CFrame (&CMonster::AI_Move,	0),
	CFrame (&CMonster::AI_Move,	0),
	CFrame (&CMonster::AI_Move,	0),
	CFrame (&CMonster::AI_Move,	0)
};
CAnim JorgMoveEndAttack1 (FRAME_attak115, FRAME_attak118, JorgFramesEndAttack1, &CMonster::Run);

void CJorg::ReAttack1()
{
	if (IsVisible(Entity, Entity->Enemy))
	{
		if (frand() < 0.9)
			CurrentMove = &JorgMoveAttack1;
		else
		{
			Entity->State.GetSound() = 0;
			CurrentMove = &JorgMoveEndAttack1;	
		}
	}
	else
	{
		Entity->State.GetSound() = 0;
		CurrentMove = &JorgMoveEndAttack1;	
	}
}

void CJorg::DoChainguns()
{
	CurrentMove = &JorgMoveAttack1;
}

void CJorg::FireBFG ()
{
	vec3f	forward, right;
	vec3f	start;
	vec3f	dir;
	vec3f	vec;

	Entity->State.GetAngles().ToVectors (&forward, &right, NULL);
	G_ProjectSource (Entity->State.GetOrigin(), dumb_and_hacky_monster_MuzzFlashOffset[MZ2_JORG_BFG_1], forward, right, start);

	dir = ((Entity->Enemy->State.GetOrigin() + vec3f(0, 0, Entity->Enemy->ViewHeight)) - start).GetNormalized();
	Entity->PlaySound (CHAN_VOICE, SoundAttack2);
	MonsterFireBfg (start, dir, 50, 300, 100, 200, MZ2_JORG_BFG_1);
}

void CJorg::FireBullet ()
{
	vec3f	forward, right, target;
	vec3f	start;

	Entity->State.GetAngles().ToVectors(&forward, &right, NULL);
	G_ProjectSource (Entity->State.GetOrigin(), dumb_and_hacky_monster_MuzzFlashOffset[MZ2_JORG_MACHINEGUN_R1], forward, right, start);

	target = Entity->Enemy->State.GetOrigin().MultiplyAngles(-0.2f, entity_cast<CPhysicsEntity>(Entity->Enemy)->Velocity);
	target[2] += Entity->Enemy->ViewHeight;
	forward = (target - start);
	forward.Normalize();

	MonsterFireBullet (start, forward, 6, 4, DEFAULT_BULLET_HSPREAD, DEFAULT_BULLET_VSPREAD, MZ2_JORG_MACHINEGUN_R1);

	Entity->State.GetAngles().ToVectors(&forward, &right, NULL);
	G_ProjectSource (Entity->State.GetOrigin(), dumb_and_hacky_monster_MuzzFlashOffset[MZ2_JORG_MACHINEGUN_L1], forward, right, start);

	target = Entity->Enemy->State.GetOrigin().MultiplyAngles(-0.2f, entity_cast<CPhysicsEntity>(Entity->Enemy)->Velocity);
	target[2] += Entity->Enemy->ViewHeight;
	forward = (target - start);
	forward.Normalize();

	MonsterFireBullet (start, forward, 6, 4, DEFAULT_BULLET_HSPREAD, DEFAULT_BULLET_VSPREAD, MZ2_JORG_MACHINEGUN_L1);
}

void CJorg::Attack()
{
	if (frand() <= 0.75)
	{
		Entity->PlaySound (CHAN_VOICE, SoundAttack1);
		Entity->State.GetSound() = SoundIndex ("boss3/w_loop.wav");
		CurrentMove = &JorgMoveStartAttack1;
	}
	else
	{
		Entity->PlaySound (CHAN_VOICE, SoundAttack2);
		CurrentMove = &JorgMoveAttack2;
	}
}

bool CJorg::CheckAttack ()
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

		tr (spot1, spot2, Entity->gameEntity, CONTENTS_SOLID|CONTENTS_MONSTER|CONTENTS_SLIME|CONTENTS_LAVA|CONTENTS_WINDOW);

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
		
	if (level.Frame < AttackFinished)
		return false;
		
	if (EnemyRange == RANGE_FAR)
		return false;

	if (AIFlags & AI_STAND_GROUND)
	{
		chance = 0.4f;
	}
	else if (EnemyRange == RANGE_MELEE)
	{
		chance = 0.8f;
	}
	else if (EnemyRange == RANGE_NEAR)
	{
		chance = 0.4f;
	}
	else if (EnemyRange == RANGE_MID)
	{
		chance = 0.2f;
	}
	else
	{
		return false;
	}

	if (skill->Integer() == 0)
		chance *= 0.5;
	else if (skill->Integer() >= 2)
		chance *= 2;

	if (frand () < chance)
	{
		AttackState = AS_MISSILE;
		AttackFinished = level.Frame + ((2*frand())*10);
		return true;
	}

	if (Entity->Flags & FL_FLY)
	{
		if (frand() < 0.3)
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
		vec3f spot1 = Entity->State.GetOrigin ();
		spot1.Z += Entity->ViewHeight;
		vec3f spot2 = Entity->Enemy->State.GetOrigin();
		spot2.Z += Entity->Enemy->ViewHeight;

		CTrace tr (spot1, spot2, Entity, CONTENTS_SOLID|CONTENTS_MONSTER|CONTENTS_SLIME|CONTENTS_LAVA|CONTENTS_WINDOW);

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
						if (level.Frame < AttackFinished)
							return false;
						if (level.Frame < (TrailTime + BlindFireDelay))
							// wait for our time
							return false;
						else
						{
							// make sure we're not going to shoot a monster
							tr (spot1, BlindFireTarget, Entity, CONTENTS_MONSTER);
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
	
	if (level.Frame < AttackFinished)
		return false;
		
	if (EnemyRange == RANGE_FAR)
		return false;

	if (AIFlags & AI_STAND_GROUND)
		chance = 0.4f;
	else if (EnemyRange == RANGE_MELEE)
		chance = 0.8f;
	else if (EnemyRange == RANGE_NEAR)
		chance = 0.4f;
	else if (EnemyRange == RANGE_MID)
		chance = 0.2f;
	else
		return false;

	if (skill->Integer() == 0)
		chance *= 0.5;
	else if (skill->Integer() >= 2)
		chance *= 2;

	// PGM - go ahead and shoot every time if it's a info_notnull
	if ((frand () < chance) || (Entity->Enemy->GetSolid() == SOLID_NOT))
	{
		AttackState = AS_MISSILE;
		AttackFinished = level.Frame + ((2*frand())*10);
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

		if (frand() < strafe_chance)
			AttackState = AS_SLIDING;
		else
			AttackState = AS_STRAIGHT;
	}
// do we want the monsters strafing?
	else
	{
		if (frand() < 0.4)
			AttackState = AS_SLIDING;
		else
			AttackState = AS_STRAIGHT;
	}

//-PMM

	return false;
#endif
}

/*QUAKED monster_jorg (1 .5 0) (-80 -80 0) (90 90 140) Ambush Trigger_Spawn Sight
*/
void CJorg::Spawn ()
{
	SoundPain1 = SoundIndex ("boss3/bs3pain1.wav");
	SoundPain2 = SoundIndex ("boss3/bs3pain2.wav");
	SoundPain3 = SoundIndex ("boss3/bs3pain3.wav");
	SoundDeath = SoundIndex ("boss3/bs3deth1.wav");
	SoundAttack1 = SoundIndex ("boss3/bs3atck1.wav");
	SoundAttack2 = SoundIndex ("boss3/bs3atck2.wav");
	SoundSearch1 = SoundIndex ("boss3/bs3srch1.wav");
	SoundSearch2 = SoundIndex ("boss3/bs3srch2.wav");
	SoundSearch3 = SoundIndex ("boss3/bs3srch3.wav");
	SoundIdle = SoundIndex ("boss3/bs3idle1.wav");
	SoundStepLeft = SoundIndex ("boss3/step1.wav");
	SoundStepRight = SoundIndex ("boss3/step2.wav");
	SoundFiregun = SoundIndex ("boss3/xfire.wav");
	SoundDeathHit = SoundIndex ("boss3/d_hit.wav");

	CMakron::Precache ();

	Entity->GetSolid() = SOLID_BBOX;
	Entity->State.GetModelIndex(2) = ModelIndex ("models/monsters/boss3/rider/tris.md2");
	Entity->State.GetModelIndex() = ModelIndex ("models/monsters/boss3/jorg/tris.md2");
	Entity->GetMins().Set (-80, -80, 0);
	Entity->GetMaxs().Set (80, 80, 140);

	Entity->Health = 3000;
	Entity->GibHealth = -2000;
	Entity->Mass = 1000;

	Entity->Link();
	MonsterFlags = (MF_HAS_ATTACK | MF_HAS_SEARCH);
	
	CurrentMove = &JorgMoveStand;

	WalkMonsterStart();
}

LINK_MONSTER_CLASSNAME_TO_CLASS ("monster_jorg", CJorg);