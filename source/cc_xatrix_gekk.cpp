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
// cc_xatrix_gekk.cpp
// 
//

#include "cc_local.h"

#if XATRIX_FEATURES
#include "cc_xatrix_gekk.h"
#include "cc_temporary_entities.h"

CLoogie::CLoogie () :
  IFlyMissileProjectile(),
  ITouchableEntity(),
  IThinkableEntity()
{
};

CLoogie::CLoogie (sint32 Index) :
  IBaseEntity (Index),
  IFlyMissileProjectile(Index),
  ITouchableEntity(Index),
  IThinkableEntity(Index)
{
};

IMPLEMENT_SAVE_SOURCE(CLoogie)

void CLoogie::Think ()
{
	Free();
}

void CLoogie::Touch (IBaseEntity *Other, SBSPPlane *plane, SBSPSurface *surf)
{
	if (Other == GetOwner())
		return;

	if (surf && (surf->Flags & SURF_TEXINFO_SKY))
	{
		Free (); // "delete" the entity
		return;
	}

	if (GetOwner() && (GetOwner()->EntityFlags & ENT_PLAYER))
		entity_cast<CPlayerEntity>(GetOwner())->PlayerNoiseAt (State.GetOrigin (), PNOISE_IMPACT);

	if ((Other->EntityFlags & ENT_HURTABLE) && entity_cast<IHurtableEntity>(Other)->CanTakeDamage)
		entity_cast<IHurtableEntity>(Other)->TakeDamage (this, GetOwner(), Velocity, State.GetOrigin (), plane ? plane->Normal : vec3fOrigin, Damage, 1, DAMAGE_ENERGY, MOD_UNKNOWN);

	Free (); // "delete" the entity
}

void CLoogie::Spawn (IBaseEntity *Spawner, vec3f start, vec3f dir,
						sint32 Damage, sint32 speed)
{
	CLoogie		*Bolt = QNewEntityOf CLoogie;

	Bolt->GetSvFlags() |= (SVF_DEADMONSTER | SVF_PROJECTILE);
	Bolt->State.GetOrigin() = start;
	Bolt->State.GetOldOrigin() = start;
	Bolt->State.GetAngles() = dir.ToAngles();
	Bolt->Velocity = dir.GetNormalizedFast() * speed;

	Bolt->State.GetRenderEffects() = RF_FULLBRIGHT;
	Bolt->State.GetModelIndex() = ModelIndex ("models/objects/loogy/tris.md2");

	Bolt->SetOwner (Spawner);
	Bolt->NextThink = Level.Frame + 20;
	Bolt->Damage = Damage;
	Bolt->ClassName = "bolt";
	Bolt->GetClipmask() = CONTENTS_MASK_SHOT;
	Bolt->GetSolid() = SOLID_BBOX;
	Bolt->GetMins().Clear ();
	Bolt->GetMaxs().Clear ();
	Bolt->Touchable = true;
	Bolt->Link ();

	CTrace tr ((Spawner) ? Spawner->State.GetOrigin() : start, start, Bolt, CONTENTS_MASK_SHOT);
	if (tr.Fraction < 1.0)
	{
		start = start.MultiplyAngles (-10, dir.GetNormalizedFast());
		Bolt->State.GetOrigin() = start;
		Bolt->State.GetOldOrigin() = start;

		if (tr.Entity)
			Bolt->Touch (tr.Entity, &tr.Plane, tr.Surface);
	}
	else if (Spawner && (Spawner->EntityFlags & ENT_PLAYER))
		CheckDodge (Spawner, start, dir, speed);
}

bool CLoogie::Run ()
{
	return IFlyMissileProjectile::Run();
}

CGekk::CGekk (uint32 ID) :
CMonster (ID)
{
	Scale = MODEL_SCALE;
	MonsterName = "Gekk";
};

//
// CHECKATTACK
//

void CGekk::DamageEffect (vec3f &dir, vec3f &point, vec3f &normal, sint32 &damage, EDamageFlags &dflags, EMeansOfDeath &mod)
{
	CBlood(point, normal, BT_GREEN_BLOOD).Send();
}

bool CGekk::CheckMelee ()
{
	if (!Entity->Enemy || entity_cast<IHurtableEntity>(*Entity->Enemy)->Health <= 0)
		return false;

	if (Range (Entity, *Entity->Enemy) == RANGE_MELEE)
		return true;
	return false;
}

bool CGekk::CheckJump ()
{
	if (Entity->GetAbsMin().Z > (Entity->Enemy->GetAbsMin().Z + 0.75 * Entity->Enemy->GetSize().Z))
		return false;

	if (Entity->GetAbsMax().Z < (Entity->Enemy->GetAbsMin().Z + 0.25 * Entity->Enemy->GetSize().Z))
		return false;

	vec3f v = Entity->State.GetOrigin() - Entity->Enemy->State.GetOrigin();
	v.Z = 0;

	if (v.Length() < 100)
		return false;
	else if (frand() < 0.9)
		return false;

	return true;
}

bool CGekk::CheckJumpClose ()
{
	vec3f v = Entity->State.GetOrigin() - Entity->Enemy->State.GetOrigin();
	v.Z = 0;

	if (v.Length() < 100)
		return (Entity->State.GetOrigin().Z < Entity->Enemy->State.GetOrigin().Z);

	return true;
}


bool CGekk::CheckAttack ()
{
	if (!Entity->Enemy || !(Entity->Enemy->EntityFlags & ENT_HURTABLE) || entity_cast<IHurtableEntity>(*Entity->Enemy)->Health <= 0)
		return false;

	if (CheckMelee())
	{
		AttackState = AS_MELEE;
		return true;
	}

	if (CheckJump() || (CheckJumpClose() && !Entity->WaterInfo.Level))
	{
		AttackState = AS_MISSILE;
		return true;
	}

	return false;
}

//
// SOUNDS
//

void CGekk::Step ()
{
	Entity->PlaySound (CHAN_VOICE, Sounds[SOUND_STEP1+irandom(3)]);
}

void CGekk::Sight ()
{
	Entity->PlaySound (CHAN_VOICE, Sounds[SOUND_SIGHT]);
}

void CGekk::Search ()
{
	if (Entity->SpawnFlags & 8)
		Entity->PlaySound (CHAN_VOICE, Sounds[SOUND_CHANT_LOW+irandom(3)]);
	else
		Entity->PlaySound (CHAN_VOICE, Sounds[SOUND_SEARCH]);

	Entity->Health += 10 + (10 * frand());
	if (Entity->Health > Entity->MaxHealth)
		Entity->Health = Entity->MaxHealth;

	if (Entity->Health < (Entity->MaxHealth /4))
		Entity->State.GetSkinNum() = 2;
	else if (Entity->Health < (Entity->MaxHealth / 2))
		Entity->State.GetSkinNum() = 1;
	else 
		Entity->State.GetSkinNum() = 0;
}

void CGekk::Swing ()
{
	Entity->PlaySound (CHAN_VOICE, Sounds[SOUND_SEARCH]);
}

//
// STAND
//

void CGekk::Gekk_AI_Stand (float Dist)
{
	if (Entity->SpawnFlags & 8)
	{
		AI_Move (Dist);

		if (!(Entity->SpawnFlags & 1) && (MonsterFlags & MF_HAS_IDLE) && (Level.Frame > IdleTime))
		{
			if (IdleTime)
			{
				Idle ();
				IdleTime = Level.Frame + ((15 + frand() * 15) * 10);
			}
			else
				IdleTime = Level.Frame + ((frand() * 15) * 10);
		}
	}
	else
		AI_Stand (Dist);
}

CFrame GekkFramesStand [] =
{
	CFrame (ConvertDerivedAIMove(&CGekk::Gekk_AI_Stand), 0),
	CFrame (ConvertDerivedAIMove(&CGekk::Gekk_AI_Stand), 0),
	CFrame (ConvertDerivedAIMove(&CGekk::Gekk_AI_Stand), 0),
	CFrame (ConvertDerivedAIMove(&CGekk::Gekk_AI_Stand), 0),
	CFrame (ConvertDerivedAIMove(&CGekk::Gekk_AI_Stand), 0),
	CFrame (ConvertDerivedAIMove(&CGekk::Gekk_AI_Stand), 0),
	CFrame (ConvertDerivedAIMove(&CGekk::Gekk_AI_Stand), 0),
	CFrame (ConvertDerivedAIMove(&CGekk::Gekk_AI_Stand), 0),
	CFrame (ConvertDerivedAIMove(&CGekk::Gekk_AI_Stand), 0),
	CFrame (ConvertDerivedAIMove(&CGekk::Gekk_AI_Stand), 0),		// 10

	CFrame (ConvertDerivedAIMove(&CGekk::Gekk_AI_Stand), 0),
	CFrame (ConvertDerivedAIMove(&CGekk::Gekk_AI_Stand), 0),
	CFrame (ConvertDerivedAIMove(&CGekk::Gekk_AI_Stand), 0),
	CFrame (ConvertDerivedAIMove(&CGekk::Gekk_AI_Stand), 0),
	CFrame (ConvertDerivedAIMove(&CGekk::Gekk_AI_Stand), 0),
	CFrame (ConvertDerivedAIMove(&CGekk::Gekk_AI_Stand), 0),
	CFrame (ConvertDerivedAIMove(&CGekk::Gekk_AI_Stand), 0),
	CFrame (ConvertDerivedAIMove(&CGekk::Gekk_AI_Stand), 0),
	CFrame (ConvertDerivedAIMove(&CGekk::Gekk_AI_Stand), 0),
	CFrame (ConvertDerivedAIMove(&CGekk::Gekk_AI_Stand), 0),		// 20

	CFrame (ConvertDerivedAIMove(&CGekk::Gekk_AI_Stand), 0),
	CFrame (ConvertDerivedAIMove(&CGekk::Gekk_AI_Stand), 0),
	CFrame (ConvertDerivedAIMove(&CGekk::Gekk_AI_Stand), 0),
	CFrame (ConvertDerivedAIMove(&CGekk::Gekk_AI_Stand), 0),
	CFrame (ConvertDerivedAIMove(&CGekk::Gekk_AI_Stand), 0),
	CFrame (ConvertDerivedAIMove(&CGekk::Gekk_AI_Stand), 0),
	CFrame (ConvertDerivedAIMove(&CGekk::Gekk_AI_Stand), 0),
	CFrame (ConvertDerivedAIMove(&CGekk::Gekk_AI_Stand), 0),
	CFrame (ConvertDerivedAIMove(&CGekk::Gekk_AI_Stand), 0),
	CFrame (ConvertDerivedAIMove(&CGekk::Gekk_AI_Stand), 0),		// 30

	CFrame (ConvertDerivedAIMove(&CGekk::Gekk_AI_Stand), 0),
	CFrame (ConvertDerivedAIMove(&CGekk::Gekk_AI_Stand), 0),
	CFrame (ConvertDerivedAIMove(&CGekk::Gekk_AI_Stand), 0),
	CFrame (ConvertDerivedAIMove(&CGekk::Gekk_AI_Stand), 0),
	CFrame (ConvertDerivedAIMove(&CGekk::Gekk_AI_Stand), 0),
	CFrame (ConvertDerivedAIMove(&CGekk::Gekk_AI_Stand), 0),
	CFrame (ConvertDerivedAIMove(&CGekk::Gekk_AI_Stand), 0),
	CFrame (ConvertDerivedAIMove(&CGekk::Gekk_AI_Stand), 0),

	CFrame (ConvertDerivedAIMove(&CGekk::Gekk_AI_Stand), 0, ConvertDerivedFunction(&CGekk::CheckUnderwater)),
};
CAnim GekkMoveStand (FRAME_stand_01, FRAME_stand_39, GekkFramesStand);

CFrame GekkFramesStandUnderwater[] =
{
	CFrame (ConvertDerivedAIMove(&CGekk::Gekk_AI_Stand), 0),	
	CFrame (ConvertDerivedAIMove(&CGekk::Gekk_AI_Stand), 0),
	CFrame (ConvertDerivedAIMove(&CGekk::Gekk_AI_Stand), 0),

	CFrame (ConvertDerivedAIMove(&CGekk::Gekk_AI_Stand), 0, ConvertDerivedFunction(&CGekk::CheckUnderwater))
};
CAnim GekkMoveStandUnderwater (FRAME_amb_01, FRAME_amb_04, GekkFramesStandUnderwater);

CFrame GekkFramesSwim [] =
{
	CFrame (&CMonster::AI_Run, 16),	
	CFrame (&CMonster::AI_Run, 16),
	CFrame (&CMonster::AI_Run, 16),

	CFrame (&CMonster::AI_Run, 16, ConvertDerivedFunction(&CGekk::Swim))
};
CAnim GekkMoveSwimLoop (FRAME_amb_01, FRAME_amb_04, GekkFramesSwim, ConvertDerivedFunction(&CGekk::SwimLoop));

CFrame GekkFramesSwimStart [] =
{
	CFrame (&CMonster::AI_Run, 14),
	CFrame (&CMonster::AI_Run, 14),
	CFrame (&CMonster::AI_Run, 14),
	CFrame (&CMonster::AI_Run, 14),
	CFrame (&CMonster::AI_Run, 16),
	CFrame (&CMonster::AI_Run, 16),
	CFrame (&CMonster::AI_Run, 16),
	CFrame (&CMonster::AI_Run, 18),
	CFrame (&CMonster::AI_Run, 18, ConvertDerivedFunction(&CGekk::HitLeft)),
	CFrame (&CMonster::AI_Run, 18),

	CFrame (&CMonster::AI_Run, 20),
	CFrame (&CMonster::AI_Run, 20),
	CFrame (&CMonster::AI_Run, 22),
	CFrame (&CMonster::AI_Run, 22),
	CFrame (&CMonster::AI_Run, 24, ConvertDerivedFunction(&CGekk::HitRight)),
	CFrame (&CMonster::AI_Run, 24),
	CFrame (&CMonster::AI_Run, 26),
	CFrame (&CMonster::AI_Run, 26),
	CFrame (&CMonster::AI_Run, 24),
	CFrame (&CMonster::AI_Run, 24),

	CFrame (&CMonster::AI_Run, 22, ConvertDerivedFunction(&CGekk::Bite)),
	CFrame (&CMonster::AI_Run, 22),
	CFrame (&CMonster::AI_Run, 22),
	CFrame (&CMonster::AI_Run, 22),
	CFrame (&CMonster::AI_Run, 22),
	CFrame (&CMonster::AI_Run, 22),
	CFrame (&CMonster::AI_Run, 22),
	CFrame (&CMonster::AI_Run, 22),
	CFrame (&CMonster::AI_Run, 18),
	CFrame (&CMonster::AI_Run, 18),

	CFrame (&CMonster::AI_Run, 18),
	CFrame (&CMonster::AI_Run, 18)
};
CAnim GekkMoveSwimStart (FRAME_swim_01, FRAME_swim_32, GekkFramesSwimStart, ConvertDerivedFunction(&CGekk::SwimLoop));

void CGekk::SwimLoop ()
{
	Entity->Flags |= FL_SWIM;	
	CurrentMove = &GekkMoveSwimLoop;
}

void CGekk::Swim ()
{
	if (CheckAttack() && (Entity->Enemy->EntityFlags & ENT_PHYSICS) && !entity_cast<IPhysicsEntity>(*Entity->Enemy)->WaterInfo.Level && frand() > 0.7)
		WaterToLand ();
	else
		CurrentMove = &GekkMoveSwimStart;
}

void CGekk::Stand ()
{
	if (Entity->WaterInfo.Level)
		CurrentMove = &GekkMoveStandUnderwater;
	else
		CurrentMove = &GekkMoveStand;
}

//
// IDLE
//

void CGekk::IdleLoop ()
{
	if (frand() > 0.75 && (Entity->Health < Entity->MaxHealth))
		NextFrame = FRAME_idle_01;
}

CFrame GekkFramesIdle [] =
{
	CFrame (ConvertDerivedAIMove(&CGekk::Gekk_AI_Stand), 0, &CMonster::Search),
	CFrame (ConvertDerivedAIMove(&CGekk::Gekk_AI_Stand), 0),
	CFrame (ConvertDerivedAIMove(&CGekk::Gekk_AI_Stand), 0),
	CFrame (ConvertDerivedAIMove(&CGekk::Gekk_AI_Stand), 0),		
	CFrame (ConvertDerivedAIMove(&CGekk::Gekk_AI_Stand), 0),
	CFrame (ConvertDerivedAIMove(&CGekk::Gekk_AI_Stand), 0),
	CFrame (ConvertDerivedAIMove(&CGekk::Gekk_AI_Stand), 0),		
	CFrame (ConvertDerivedAIMove(&CGekk::Gekk_AI_Stand), 0),
	CFrame (ConvertDerivedAIMove(&CGekk::Gekk_AI_Stand), 0),
	CFrame (ConvertDerivedAIMove(&CGekk::Gekk_AI_Stand), 0),

	CFrame (ConvertDerivedAIMove(&CGekk::Gekk_AI_Stand), 0),
	CFrame (ConvertDerivedAIMove(&CGekk::Gekk_AI_Stand), 0),
	CFrame (ConvertDerivedAIMove(&CGekk::Gekk_AI_Stand), 0),
	CFrame (ConvertDerivedAIMove(&CGekk::Gekk_AI_Stand), 0),		
	CFrame (ConvertDerivedAIMove(&CGekk::Gekk_AI_Stand), 0),
	CFrame (ConvertDerivedAIMove(&CGekk::Gekk_AI_Stand), 0),
	CFrame (ConvertDerivedAIMove(&CGekk::Gekk_AI_Stand), 0),		
	CFrame (ConvertDerivedAIMove(&CGekk::Gekk_AI_Stand), 0),
	CFrame (ConvertDerivedAIMove(&CGekk::Gekk_AI_Stand), 0),
	CFrame (ConvertDerivedAIMove(&CGekk::Gekk_AI_Stand), 0),

	CFrame (ConvertDerivedAIMove(&CGekk::Gekk_AI_Stand), 0),
	CFrame (ConvertDerivedAIMove(&CGekk::Gekk_AI_Stand), 0),
	CFrame (ConvertDerivedAIMove(&CGekk::Gekk_AI_Stand), 0),
	CFrame (ConvertDerivedAIMove(&CGekk::Gekk_AI_Stand), 0),		
	CFrame (ConvertDerivedAIMove(&CGekk::Gekk_AI_Stand), 0),
	CFrame (ConvertDerivedAIMove(&CGekk::Gekk_AI_Stand), 0),
	CFrame (ConvertDerivedAIMove(&CGekk::Gekk_AI_Stand), 0),		
	CFrame (ConvertDerivedAIMove(&CGekk::Gekk_AI_Stand), 0),
	CFrame (ConvertDerivedAIMove(&CGekk::Gekk_AI_Stand), 0),
	CFrame (ConvertDerivedAIMove(&CGekk::Gekk_AI_Stand), 0),

	CFrame (ConvertDerivedAIMove(&CGekk::Gekk_AI_Stand), 0),
	CFrame (ConvertDerivedAIMove(&CGekk::Gekk_AI_Stand), 0, ConvertDerivedFunction(&CGekk::IdleLoop))
};
CAnim GekkMoveIdle (FRAME_idle_01, FRAME_idle_32, GekkFramesIdle, &CMonster::Stand);
CAnim GekkMoveIdle2 (FRAME_idle_01, FRAME_idle_32, GekkFramesIdle, ConvertDerivedFunction(&CGekk::Face));

CFrame GekkFramesChant [] =
{
	CFrame (&CMonster::AI_Move, 0, &CMonster::Search),
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
	CFrame (&CMonster::AI_Move, 0, ConvertDerivedFunction(&CGekk::IdleLoop))
};
CAnim GekkMoveChant (FRAME_idle_01, FRAME_idle_32, GekkFramesChant, ConvertDerivedFunction(&CGekk::Chant));

void CGekk::Chant ()
{
	CurrentMove = &GekkMoveChant;
}

void CGekk::Idle ()
{
	if (!Entity->WaterInfo.Level)
		CurrentMove = &GekkMoveIdle;
	else
		CurrentMove = &GekkMoveSwimStart;
}

//
// WALK
//

CFrame GekkFramesWalk[] = 
{
	CFrame (&CMonster::AI_Walk,   3.849f, ConvertDerivedFunction(&CGekk::CheckUnderwater)), // frame 0
	CFrame (&CMonster::AI_Walk,  19.606f), // frame 1
	CFrame (&CMonster::AI_Walk,  25.583f), // frame 2
	CFrame (&CMonster::AI_Walk,  34.625f, ConvertDerivedFunction(&CGekk::Step)), // frame 3
	CFrame (&CMonster::AI_Walk,  27.365f), // frame 4
	CFrame (&CMonster::AI_Walk,  28.480f), // frame 5
};
CAnim GekkMoveWalk (FRAME_run_01, FRAME_run_06, GekkFramesWalk);

void CGekk::Walk ()
{
	CurrentMove = &GekkMoveWalk;
}

//
// RUN
//

CFrame GekkFramesRun[] = 
{
	CFrame (&CMonster::AI_Run,   3.849f, ConvertDerivedFunction(&CGekk::CheckUnderwater)), // frame 0
	CFrame (&CMonster::AI_Run,  19.606f), // frame 1
	CFrame (&CMonster::AI_Run,  25.583f), // frame 2
	CFrame (&CMonster::AI_Run,  34.625f, ConvertDerivedFunction(&CGekk::Step)), // frame 3
	CFrame (&CMonster::AI_Run,  27.365f), // frame 4
	CFrame (&CMonster::AI_Run,  28.480f), // frame 5
};
CAnim GekkMoveRun (FRAME_run_01, FRAME_run_06, GekkFramesRun);

CFrame GekkFramesRunStart[] = 
{
	CFrame (&CMonster::AI_Run,   0.212f), // frame 0
	CFrame (&CMonster::AI_Run,  19.753f), // frame 1
};
CAnim GekkMoveRunStart (FRAME_stand_01, FRAME_stand_02, GekkFramesRunStart, ConvertDerivedFunction(&CGekk::DoRun));

void CGekk::Face ()
{
	CurrentMove = &GekkMoveRun;	
}

void CGekk::Run ()
{
	if (Entity->WaterInfo.Level)
		CurrentMove = &GekkMoveSwimStart;
	else
		CurrentMove = &GekkMoveRunStart;
}

void CGekk::DoRun ()
{
	if (Entity->WaterInfo.Level)
		CurrentMove = &GekkMoveSwimStart;
	else
	{
		if (AIFlags & AI_STAND_GROUND)
			CurrentMove = &GekkMoveStand;
		else
			CurrentMove = &GekkMoveRun;
	}
}

//
// MELEE
//

void CGekk::HitLeft ()
{
	const vec3f aim (MELEE_DISTANCE, Entity->GetMins().X, 8);

	if (CMeleeWeapon::Fire (Entity, aim, (15 + (irandom(5))), 100))
		Entity->PlaySound (CHAN_WEAPON, Sounds[SOUND_HIT]);
	else
		Entity->PlaySound (CHAN_WEAPON, Sounds[SOUND_SWING]);
}

void CGekk::HitRight ()
{
	const vec3f aim (MELEE_DISTANCE, Entity->GetMaxs().X, 8);

	if (CMeleeWeapon::Fire (Entity, aim, (15 + (irandom(5))), 100))
		Entity->PlaySound (CHAN_WEAPON, Sounds[SOUND_HIT]);
	else
		Entity->PlaySound (CHAN_WEAPON, Sounds[SOUND_SWING]);
}

void CGekk::Loogie ()
{
	vec3f start;
	vec3f forward, right, up;
	vec3f end;
	vec3f dir;
	static const vec3f fireOffset (-18, -0.8f, 24);

	if (!Entity->Enemy || entity_cast<IHurtableEntity>(*Entity->Enemy)->Health <= 0)
		return;

	Entity->State.GetAngles().ToVectors (&forward, &right, &up);
	G_ProjectSource (Entity->State.GetOrigin(), fireOffset, forward, right, start);

	start = start.MultiplyAngles (2, up);

	end = Entity->Enemy->State.GetOrigin() + vec3f(0, 0, Entity->Enemy->ViewHeight);;

	CLoogie::Spawn (Entity, start, (end - start), 5, 550);
}


CFrame GekkFramesSpit [] =
{
	CFrame (&CMonster::AI_Charge,   0), 
	CFrame (&CMonster::AI_Charge,   0), 
	CFrame (&CMonster::AI_Charge,   0), 
	CFrame (&CMonster::AI_Charge,   0), 
	CFrame (&CMonster::AI_Charge,   0), 

	CFrame (&CMonster::AI_Charge,   0, ConvertDerivedFunction(&CGekk::Loogie)), 
	CFrame (&CMonster::AI_Charge,   0, ConvertDerivedFunction(&CGekk::ReFireLoogie)) 
};
CAnim GekkMoveSpit (FRAME_spit_01, FRAME_spit_07, GekkFramesSpit, &CMonster::Run);

void CGekk::ReFireLoogie ()
{
	if (frand() > 0.8 && Entity->Health < Entity->MaxHealth)
	{
		CurrentMove = &GekkMoveIdle2;
		return;
	}

	if (entity_cast<IHurtableEntity>(*Entity->Enemy)->Health >= 0 && frand() > 0.7 && (Range(Entity, *Entity->Enemy) == RANGE_NEAR))
		CurrentMove = &GekkMoveSpit;
}

CFrame GekkFramesAttack1 [] =
{
	CFrame (&CMonster::AI_Charge,   0), 
	CFrame (&CMonster::AI_Charge,   0), 
	CFrame (&CMonster::AI_Charge,   0), 

	CFrame (&CMonster::AI_Charge,   0, ConvertDerivedFunction(&CGekk::HitLeft)), 
	CFrame (&CMonster::AI_Charge,   0), 
	CFrame (&CMonster::AI_Charge,   0), 

	CFrame (&CMonster::AI_Charge,   0), 
	CFrame (&CMonster::AI_Charge,   0), 
	CFrame (&CMonster::AI_Charge,   0, ConvertDerivedFunction(&CGekk::CheckMeleeRefire)) 

};
CAnim GekkMoveAttack1 (FRAME_clawatk3_01, FRAME_clawatk3_09, GekkFramesAttack1, &CMonster::Run);

CFrame GekkFramesAttack2[] = 
{
	CFrame (&CMonster::AI_Charge,   0), 
	CFrame (&CMonster::AI_Charge,   0), 
	CFrame (&CMonster::AI_Charge,   0, ConvertDerivedFunction(&CGekk::HitLeft)), 

	CFrame (&CMonster::AI_Charge,   0), 
	CFrame (&CMonster::AI_Charge,   0), 
	CFrame (&CMonster::AI_Charge,   0, ConvertDerivedFunction(&CGekk::HitRight)), 

	CFrame (&CMonster::AI_Charge,   0), 
	CFrame (&CMonster::AI_Charge,   0), 
	CFrame (&CMonster::AI_Charge,   0, ConvertDerivedFunction(&CGekk::CheckMeleeRefire)) 
};
CAnim GekkMoveAttack2 (FRAME_clawatk5_01, FRAME_clawatk5_09, GekkFramesAttack2, &CMonster::Run);

void CGekk::CheckMeleeRefire ()
{
	if (!Entity->Enemy || !Entity->Enemy->GetInUse() || entity_cast<IHurtableEntity>(*Entity->Enemy)->Health <= 0)
		return;

	if (frand() < (CvarList[CV_SKILL].Integer() * 0.1))
	{
		if (Range (Entity, *Entity->Enemy) == RANGE_MELEE)
		{
			switch (Entity->State.GetFrame())
			{
			case FRAME_clawatk3_09:
				CurrentMove = &GekkMoveAttack2;
				break;
			case FRAME_clawatk5_09:
				CurrentMove = &GekkMoveAttack1;
				break;
			}
		}
	}
}

void CGekk::CheckUnderwater ()
{
	if (Entity->WaterInfo.Level)
		LandToWater ();
}

CFrame GekkFramesLeapAttack[] = 
{
	CFrame (&CMonster::AI_Charge,   0), // frame 0
	CFrame (&CMonster::AI_Charge,  -0.387f), // frame 1
	CFrame (&CMonster::AI_Charge,  -1.113f), // frame 2
	CFrame (&CMonster::AI_Charge,  -0.237f), // frame 3
	CFrame (&CMonster::AI_Charge,   6.720f, ConvertDerivedFunction(&CGekk::JumpTakeoff)), // frame 4  last frame on ground
	CFrame (&CMonster::AI_Charge,   6.414f), // frame 5  leaves ground
	CFrame (&CMonster::AI_Charge,   0.163f), // frame 6
	CFrame (&CMonster::AI_Charge,  28.316f), // frame 7
	CFrame (&CMonster::AI_Charge,  24.198f), // frame 8
	CFrame (&CMonster::AI_Charge,  31.742f), // frame 9
	CFrame (&CMonster::AI_Charge,  35.977f, ConvertDerivedFunction(&CGekk::CheckLanding)), // frame 10  last frame in air
	CFrame (&CMonster::AI_Charge,  12.303f, ConvertDerivedFunction(&CGekk::StopSkid)), // frame 11  feet back on ground
	CFrame (&CMonster::AI_Charge,  20.122f, ConvertDerivedFunction(&CGekk::StopSkid)), // frame 12
	CFrame (&CMonster::AI_Charge,  -1.042f, ConvertDerivedFunction(&CGekk::StopSkid)), // frame 13
	CFrame (&CMonster::AI_Charge,   2.556f, ConvertDerivedFunction(&CGekk::StopSkid)), // frame 14
	CFrame (&CMonster::AI_Charge,   0.544f, ConvertDerivedFunction(&CGekk::StopSkid)), // frame 15
	CFrame (&CMonster::AI_Charge,   1.862f, ConvertDerivedFunction(&CGekk::StopSkid)), // frame 16
	CFrame (&CMonster::AI_Charge,   1.224f, ConvertDerivedFunction(&CGekk::StopSkid)), // frame 17

	CFrame (&CMonster::AI_Charge,  -0.457f, ConvertDerivedFunction(&CGekk::CheckUnderwater)), // frame 18
};
CAnim GekkMoveLeapAtk (FRAME_leapatk_01, FRAME_leapatk_19, GekkFramesLeapAttack, &CMonster::Run);


CFrame GekkFramesLeapAttack2[] = 
{
	CFrame (&CMonster::AI_Charge,   0), // frame 0
	CFrame (&CMonster::AI_Charge,  -0.387f), // frame 1
	CFrame (&CMonster::AI_Charge,  -1.113f), // frame 2
	CFrame (&CMonster::AI_Charge,  -0.237f), // frame 3
	CFrame (&CMonster::AI_Charge,   6.720f, ConvertDerivedFunction(&CGekk::JumpTakeoff2)), // frame 4  last frame on ground
	CFrame (&CMonster::AI_Charge,   6.414f), // frame 5  leaves ground
	CFrame (&CMonster::AI_Charge,   0.163f), // frame 6
	CFrame (&CMonster::AI_Charge,  28.316f), // frame 7
	CFrame (&CMonster::AI_Charge,  24.198f), // frame 8
	CFrame (&CMonster::AI_Charge,  31.742f), // frame 9
	CFrame (&CMonster::AI_Charge,  35.977f, ConvertDerivedFunction(&CGekk::CheckLanding)), // frame 10  last frame in air
	CFrame (&CMonster::AI_Charge,  12.303f, ConvertDerivedFunction(&CGekk::StopSkid)), // frame 11  feet back on ground
	CFrame (&CMonster::AI_Charge,  20.122f, ConvertDerivedFunction(&CGekk::StopSkid)), // frame 12
	CFrame (&CMonster::AI_Charge,  -1.042f, ConvertDerivedFunction(&CGekk::StopSkid)), // frame 13
	CFrame (&CMonster::AI_Charge,   2.556f, ConvertDerivedFunction(&CGekk::StopSkid)), // frame 14
	CFrame (&CMonster::AI_Charge,   0.544f, ConvertDerivedFunction(&CGekk::StopSkid)), // frame 15
	CFrame (&CMonster::AI_Charge,   1.862f, ConvertDerivedFunction(&CGekk::StopSkid)), // frame 16
	CFrame (&CMonster::AI_Charge,   1.224f, ConvertDerivedFunction(&CGekk::StopSkid)), // frame 17

	CFrame (&CMonster::AI_Charge,  -0.457f, ConvertDerivedFunction(&CGekk::CheckUnderwater)), // frame 18
};
CAnim GekkMoveLeapAtk2 (FRAME_leapatk_01, FRAME_leapatk_19, GekkFramesLeapAttack2, &CMonster::Run);


void CGekk::Bite ()
{
	static const vec3f aim (MELEE_DISTANCE, 0, 0);
	CMeleeWeapon::Fire (Entity, aim, 5, 0);
}

CFrame GekkFramesAttack [] =
{
	CFrame (&CMonster::AI_Charge, 16),
	CFrame (&CMonster::AI_Charge, 16),
	CFrame (&CMonster::AI_Charge, 16),
	CFrame (&CMonster::AI_Charge, 16),
	CFrame (&CMonster::AI_Charge, 16,	ConvertDerivedFunction(&CGekk::Bite)),
	CFrame (&CMonster::AI_Charge, 16),
	CFrame (&CMonster::AI_Charge, 16),
	CFrame (&CMonster::AI_Charge, 16),
	CFrame (&CMonster::AI_Charge, 16),
	CFrame (&CMonster::AI_Charge, 16,	ConvertDerivedFunction(&CGekk::Bite)),

	CFrame (&CMonster::AI_Charge, 16),
	CFrame (&CMonster::AI_Charge, 16),
	CFrame (&CMonster::AI_Charge, 16),
	CFrame (&CMonster::AI_Charge, 16,	ConvertDerivedFunction(&CGekk::HitLeft)),
	CFrame (&CMonster::AI_Charge, 16),
	CFrame (&CMonster::AI_Charge, 16),
	CFrame (&CMonster::AI_Charge, 16),
	CFrame (&CMonster::AI_Charge, 16),
	CFrame (&CMonster::AI_Charge, 16,	ConvertDerivedFunction(&CGekk::HitRight)),
	CFrame (&CMonster::AI_Charge, 16),

	CFrame (&CMonster::AI_Charge, 16)
};
CAnim GekkMoveAttack (FRAME_attack_01, FRAME_attack_21, GekkFramesAttack, &CMonster::Run);

void CGekk::Melee ()
{	
	if (Entity->WaterInfo.Level)
		CurrentMove = &GekkMoveAttack;	
	else if (frand() > 0.66)
		CurrentMove = &GekkMoveAttack1;
	else 
		CurrentMove = &GekkMoveAttack2;
}

//
// ATTACK
//

void CGekk::Touch (IBaseEntity *Other, SBSPPlane *plane, SBSPSurface *surf)
{
	if (!Jumping)
		return;

	if (Entity->Health <= 0)
	{
		Jumping = false;
		return;
	}

	if (!(Other->EntityFlags & ENT_HURTABLE))
		return;

	IHurtableEntity *Hurtable = entity_cast<IHurtableEntity>(Other);

	if (Hurtable->CanTakeDamage)
	{
		if (Entity->Velocity.Length() > 200)
		{
			vec3f point, normal;
			int damage;

			normal = Entity->Velocity.GetNormalized();
			point = Entity->State.GetOrigin().MultiplyAngles (Entity->GetMaxs().X, normal);
			damage = 10 + 10 * frand();
			Hurtable->TakeDamage (Entity, Entity, Entity->Velocity, point, normal, damage, damage, 0, MOD_UNKNOWN);
		}
	}

	if (!CheckBottom () && (Entity->GroundEntity))
		NextFrame = FRAME_leapatk_11;

	Jumping = false;
}

void CGekk::JumpTakeoff ()
{
	vec3f	forward;

	Entity->PlaySound (CHAN_VOICE, Sounds[SOUND_SIGHT]);
	Entity->State.GetAngles().ToVectors (&forward, NULL, NULL);
	Entity->State.GetOrigin().Z += 1;

	// high jump
	if (CheckJump ())
	{
		Entity->Velocity = forward * 700;
		Entity->Velocity.Z = 250;
	}
	else
	{
		Entity->Velocity = forward * 250;
		Entity->Velocity.Z = 400;
	}

	Entity->GroundEntity = nullentity;
	AIFlags |= AI_DUCKED;
	AttackFinished = Level.Frame + 30;
	Jumping = true;
}

void CGekk::JumpTakeoff2 ()
{
	vec3f	forward;

	Entity->PlaySound (CHAN_VOICE, Sounds[SOUND_SIGHT]);
	Entity->State.GetAngles().ToVectors (&forward, NULL, NULL);
	Entity->State.GetOrigin().Z += 1;

	// high jump
	if (CheckJump ())
	{
		Entity->Velocity = forward * 300;
		Entity->Velocity.Z = 250;
	}
	else
	{
		Entity->Velocity = forward * 150;
		Entity->Velocity.Z = 300;
	}

	Entity->GroundEntity = nullentity;
	AIFlags |= AI_DUCKED;
	AttackFinished = Level.Frame + 30;
	Jumping = true;
}

void CGekk::StopSkid ()
{
	if (Entity->GroundEntity)
		Entity->Velocity.Clear();
}

void CGekk::CheckLanding ()
{
	if (Entity->GroundEntity)
	{
		Entity->PlaySound (CHAN_WEAPON, Sounds[SOUND_THUD]);
		AttackFinished = 0;
		AIFlags &= ~AI_DUCKED;

		Entity->Velocity.Clear();
		return;
	}

	// note to self
	// causing skid
	if (Level.Frame > AttackFinished)
		NextFrame = FRAME_leapatk_11;
	else
		NextFrame = FRAME_leapatk_12;
}

void CGekk::Jump ()
{
	if (Entity->Flags & FL_SWIM || Entity->WaterInfo.Level)
		return;
	else
	{
		if ((frand() > 0.5 && (Range (Entity, *Entity->Enemy) >= RANGE_NEAR)) || (frand() > 0.8))
			CurrentMove = &GekkMoveSpit;
		else
			CurrentMove = &GekkMoveLeapAtk;
	}
}

//
// PAIN
//

CFrame GekkFramesPain[] = 
{
	CFrame (&CMonster::AI_Move,   0), // frame 0
	CFrame (&CMonster::AI_Move,   0), // frame 1
	CFrame (&CMonster::AI_Move,   0), // frame 2
	CFrame (&CMonster::AI_Move,   0), // frame 3
	CFrame (&CMonster::AI_Move,   0), // frame 4
	CFrame (&CMonster::AI_Move,   0), // frame 5
};
CAnim GekkMovePain (FRAME_pain_01, FRAME_pain_06, GekkFramesPain, &CMonster::Run);

CFrame GekkFramesPain1[] = 
{
	CFrame (&CMonster::AI_Move,   0), // frame 0
	CFrame (&CMonster::AI_Move,   0), // frame 1
	CFrame (&CMonster::AI_Move,   0), // frame 2
	CFrame (&CMonster::AI_Move,   0), // frame 3
	CFrame (&CMonster::AI_Move,   0), // frame 4
	CFrame (&CMonster::AI_Move,   0), // frame 5
	CFrame (&CMonster::AI_Move,   0), // frame 6
	CFrame (&CMonster::AI_Move,   0), // frame 7
	CFrame (&CMonster::AI_Move,   0), // frame 8
	CFrame (&CMonster::AI_Move,   0), // frame 9

	CFrame (&CMonster::AI_Move,   0, ConvertDerivedFunction(&CGekk::CheckUnderwater)) 
};
CAnim GekkMovePain1 (FRAME_pain3_01, FRAME_pain3_11, GekkFramesPain1, &CMonster::Run);

CFrame GekkFramesPain2[] = 
{
	CFrame (&CMonster::AI_Move,   0), // frame 0
	CFrame (&CMonster::AI_Move,   0), // frame 1
	CFrame (&CMonster::AI_Move,   0), // frame 2
	CFrame (&CMonster::AI_Move,   0), // frame 3
	CFrame (&CMonster::AI_Move,   0), // frame 4
	CFrame (&CMonster::AI_Move,   0), // frame 5
	CFrame (&CMonster::AI_Move,   0), // frame 6
	CFrame (&CMonster::AI_Move,   0), // frame 7
	CFrame (&CMonster::AI_Move,   0), // frame 8
	CFrame (&CMonster::AI_Move,   0), // frame 9

	CFrame (&CMonster::AI_Move,   0), // frame 10
	CFrame (&CMonster::AI_Move,   0), // frame 11
	CFrame (&CMonster::AI_Move,   0, ConvertDerivedFunction(&CGekk::CheckUnderwater)), 
};
CAnim GekkMovePain2 (FRAME_pain4_01, FRAME_pain4_13, GekkFramesPain2, &CMonster::Run);

void CGekk::Pain (IBaseEntity *Other, sint32 Damage)
{
	if (Entity->SpawnFlags & 8)
	{
		Entity->SpawnFlags &= ~8;
		return;
	}

	if (Entity->Health < (Entity->MaxHealth /4))
		Entity->State.GetSkinNum() = 2;
	else if (Entity->Health < (Entity->MaxHealth / 2))
		Entity->State.GetSkinNum() = 1;

	if (Level.Frame < PainDebounceTime)
		return;

	PainDebounceTime = Level.Frame + 30;

	Entity->PlaySound (CHAN_VOICE, Sounds[SOUND_PAIN1]);

	if (Entity->WaterInfo.Level)
	{
		if (!(Entity->Flags & FL_SWIM))
			Entity->Flags |= FL_SWIM;

		CurrentMove = &GekkMovePain;
	}
	else
	{
		if (frand() > 0.5)
			CurrentMove = &GekkMovePain1;
		else
			CurrentMove = &GekkMovePain2;
	}
}


//
// DEATH
//

void CGekk::Dead ()
{
	// fix this because of no blocking problem
	if (Entity->WaterInfo.Level)
		return;
	else
	{
		Entity->GetMins().Set (-16, -16, -24);
		Entity->GetMaxs().Set (16, 16, -8);
		Entity->PhysicsType = PHYSICS_TOSS;
		Entity->GetSvFlags() |= SVF_DEADMONSTER;
		Entity->Link ();
		Entity->NextThink = 0;
	}
}

void CGekk::GibFest ()
{
	Entity->PlaySound (CHAN_VOICE, SoundIndex ("misc/udeath.wav"));

	CGibEntity::Spawn (Entity, ModelIndex ("models/objects/gekkgib/arm/tris.md2"), 20, GIB_ORGANIC, EF_GREENGIB);
	CGibEntity::Spawn (Entity, ModelIndex ("models/objects/gekkgib/arm/tris.md2"), 20, GIB_ORGANIC, EF_GREENGIB);
	CGibEntity::Spawn (Entity, ModelIndex ("models/objects/gekkgib/torso/tris.md2"), 20, GIB_ORGANIC, EF_GREENGIB);
	CGibEntity::Spawn (Entity, ModelIndex ("models/objects/gekkgib/claw/tris.md2"), 20, GIB_ORGANIC, EF_GREENGIB);
	CGibEntity::Spawn (Entity, ModelIndex ("models/objects/gekkgib/leg/tris.md2"), 20, GIB_ORGANIC, EF_GREENGIB);
	CGibEntity::Spawn (Entity, ModelIndex ("models/objects/gekkgib/leg/tris.md2"), 20, GIB_ORGANIC, EF_GREENGIB);
	CGibEntity::Spawn (Entity, ModelIndex ("models/objects/gekkgib/pelvis/tris.md2"), 20, GIB_ORGANIC, EF_GREENGIB);

	Entity->ThrowHead (ModelIndex ("models/objects/gekkgib/head/tris.md2"), 20, GIB_ORGANIC, EF_GREENGIB);

	Entity->DeadFlag = true;
}

void CGekk::IsGibFest ()
{
	if (frand() > 0.9)
		GibFest ();
}

CFrame GekkFramesDeath1[] =
{
	CFrame (&CMonster::AI_Move,  -5.151f), // frame 0
	CFrame (&CMonster::AI_Move, -12.223f), // frame 1
	CFrame (&CMonster::AI_Move, -11.484f), // frame 2
	CFrame (&CMonster::AI_Move, -17.952f), // frame 3
	CFrame (&CMonster::AI_Move,  -6.953f), // frame 4
	CFrame (&CMonster::AI_Move,  -7.393f), // frame 5
	CFrame (&CMonster::AI_Move, -10.713f), // frame 6
	CFrame (&CMonster::AI_Move, -17.464f), // frame 7
	CFrame (&CMonster::AI_Move, -11.678f), // frame 8
	CFrame (&CMonster::AI_Move, -11.678f)  // frame 9
};
CAnim GekkMoveDeath1 (FRAME_death1_01, FRAME_death1_10, GekkFramesDeath1, ConvertDerivedFunction(&CGekk::Dead));

CFrame GekkFramesDeath3[] =
{
	CFrame (&CMonster::AI_Move,   0), // frame 0
	CFrame (&CMonster::AI_Move,   0.022f), // frame 1
	CFrame (&CMonster::AI_Move,   0.169f), // frame 2
	CFrame (&CMonster::AI_Move,  -0.710f), // frame 3
	CFrame (&CMonster::AI_Move, -13.446f), // frame 4
	CFrame (&CMonster::AI_Move,  -7.654f, ConvertDerivedFunction(&CGekk::IsGibFest)), // frame 5
	CFrame (&CMonster::AI_Move, -31.951f), // frame 6
};
CAnim GekkMoveDeath3 (FRAME_death3_01, FRAME_death3_07, GekkFramesDeath3, ConvertDerivedFunction(&CGekk::Dead));

CFrame GekkFramesDeath4[] = 
{
	CFrame (&CMonster::AI_Move,   5.103f), // frame 0
	CFrame (&CMonster::AI_Move,  -4.808f), // frame 1
	CFrame (&CMonster::AI_Move, -10.509f), // frame 2
	CFrame (&CMonster::AI_Move,  -9.899f), // frame 3
	CFrame (&CMonster::AI_Move,   4.033f, ConvertDerivedFunction(&CGekk::IsGibFest)), // frame 4
	CFrame (&CMonster::AI_Move,  -5.197f), // frame 5
	CFrame (&CMonster::AI_Move,  -0.919f), // frame 6
	CFrame (&CMonster::AI_Move,  -8.821f), // frame 7
	CFrame (&CMonster::AI_Move,  -5.626f), // frame 8
	CFrame (&CMonster::AI_Move,  -8.865f, ConvertDerivedFunction(&CGekk::IsGibFest)), // frame 9
	CFrame (&CMonster::AI_Move,  -0.845f), // frame 10
	CFrame (&CMonster::AI_Move,   1.986f), // frame 11
	CFrame (&CMonster::AI_Move,   0.170f), // frame 12
	CFrame (&CMonster::AI_Move,   1.339f, ConvertDerivedFunction(&CGekk::IsGibFest)), // frame 13
	CFrame (&CMonster::AI_Move,  -0.922f), // frame 14
	CFrame (&CMonster::AI_Move,   0.818f), // frame 15
	CFrame (&CMonster::AI_Move,  -1.288f), // frame 16
	CFrame (&CMonster::AI_Move,  -1.408f, ConvertDerivedFunction(&CGekk::IsGibFest)), // frame 17
	CFrame (&CMonster::AI_Move,  -7.787f), // frame 18
	CFrame (&CMonster::AI_Move,  -3.995f), // frame 19
	CFrame (&CMonster::AI_Move,  -4.604f), // frame 20
	CFrame (&CMonster::AI_Move,  -1.715f, ConvertDerivedFunction(&CGekk::IsGibFest)), // frame 21
	CFrame (&CMonster::AI_Move,  -0.564f), // frame 22
	CFrame (&CMonster::AI_Move,  -0.597f), // frame 23
	CFrame (&CMonster::AI_Move,   0.074f), // frame 24
	CFrame (&CMonster::AI_Move,  -0.309f, ConvertDerivedFunction(&CGekk::IsGibFest)), // frame 25
	CFrame (&CMonster::AI_Move,  -0.395f), // frame 26
	CFrame (&CMonster::AI_Move,  -0.501f), // frame 27
	CFrame (&CMonster::AI_Move,  -0.325f), // frame 28
	CFrame (&CMonster::AI_Move,  -0.931f, ConvertDerivedFunction(&CGekk::IsGibFest)), // frame 29
	CFrame (&CMonster::AI_Move,  -1.433f), // frame 30
	CFrame (&CMonster::AI_Move,  -1.626f), // frame 31
	CFrame (&CMonster::AI_Move,   4.680f), // frame 32
	CFrame (&CMonster::AI_Move,   0.560f), // frame 33
	CFrame (&CMonster::AI_Move,  -0.549f, ConvertDerivedFunction(&CGekk::GibFest)) // frame 34
};
CAnim GekkMoveDeath4 (FRAME_death4_01, FRAME_death4_35, GekkFramesDeath4, ConvertDerivedFunction(&CGekk::Dead));

CFrame GekkFramesWDeath[] = 
{
	CFrame (&CMonster::AI_Move,   0), // frame 0
	CFrame (&CMonster::AI_Move,   0), // frame 1
	CFrame (&CMonster::AI_Move,   0), // frame 2
	CFrame (&CMonster::AI_Move,   0), // frame 3
	CFrame (&CMonster::AI_Move,   0), // frame 4
	CFrame (&CMonster::AI_Move,   0), // frame 5
	CFrame (&CMonster::AI_Move,   0), // frame 6
	CFrame (&CMonster::AI_Move,   0), // frame 7
	CFrame (&CMonster::AI_Move,   0), // frame 8
	CFrame (&CMonster::AI_Move,   0), // frame 9
	CFrame (&CMonster::AI_Move,   0), // frame 10
	CFrame (&CMonster::AI_Move,   0), // frame 11
	CFrame (&CMonster::AI_Move,   0), // frame 12
	CFrame (&CMonster::AI_Move,   0), // frame 13
	CFrame (&CMonster::AI_Move,   0), // frame 14
	CFrame (&CMonster::AI_Move,   0), // frame 15
	CFrame (&CMonster::AI_Move,   0), // frame 16
	CFrame (&CMonster::AI_Move,   0), // frame 17
	CFrame (&CMonster::AI_Move,   0), // frame 18
	CFrame (&CMonster::AI_Move,   0), // frame 19
	CFrame (&CMonster::AI_Move,   0), // frame 20
	CFrame (&CMonster::AI_Move,   0), // frame 21
	CFrame (&CMonster::AI_Move,   0), // frame 22
	CFrame (&CMonster::AI_Move,   0), // frame 23
	CFrame (&CMonster::AI_Move,   0), // frame 24
	CFrame (&CMonster::AI_Move,   0), // frame 25
	CFrame (&CMonster::AI_Move,   0), // frame 26
	CFrame (&CMonster::AI_Move,   0), // frame 27
	CFrame (&CMonster::AI_Move,   0), // frame 28
	CFrame (&CMonster::AI_Move,   0), // frame 29
	CFrame (&CMonster::AI_Move,   0), // frame 30
	CFrame (&CMonster::AI_Move,   0), // frame 31
	CFrame (&CMonster::AI_Move,   0), // frame 32
	CFrame (&CMonster::AI_Move,   0), // frame 33
	CFrame (&CMonster::AI_Move,   0), // frame 34
	CFrame (&CMonster::AI_Move,   0), // frame 35
	CFrame (&CMonster::AI_Move,   0), // frame 36
	CFrame (&CMonster::AI_Move,   0), // frame 37
	CFrame (&CMonster::AI_Move,   0), // frame 38
	CFrame (&CMonster::AI_Move,   0), // frame 39
	CFrame (&CMonster::AI_Move,   0), // frame 40
	CFrame (&CMonster::AI_Move,   0), // frame 41
	CFrame (&CMonster::AI_Move,   0), // frame 42
	CFrame (&CMonster::AI_Move,   0), // frame 43
	CFrame (&CMonster::AI_Move,   0)  // frame 44
};
CAnim GekkMoveWDeath (FRAME_wdeath_01, FRAME_wdeath_45, GekkFramesWDeath, ConvertDerivedFunction(&CGekk::Dead));

void CGekk::Die (IBaseEntity *Inflictor, IBaseEntity *Attacker, sint32 Damage, vec3f &point)
{	
	if (Entity->Health < Entity->GibHealth)
	{
		Entity->PlaySound (CHAN_VOICE, SoundIndex ("misc/udeath.wav"));

		CGibEntity::Spawn (Entity, ModelIndex ("models/objects/gekkgib/arm/tris.md2"), 20, GIB_ORGANIC, EF_GREENGIB);
		CGibEntity::Spawn (Entity, ModelIndex ("models/objects/gekkgib/arm/tris.md2"), 20, GIB_ORGANIC, EF_GREENGIB);
		CGibEntity::Spawn (Entity, ModelIndex ("models/objects/gekkgib/torso/tris.md2"), 20, GIB_ORGANIC, EF_GREENGIB);
		CGibEntity::Spawn (Entity, ModelIndex ("models/objects/gekkgib/claw/tris.md2"), 20, GIB_ORGANIC, EF_GREENGIB);
		CGibEntity::Spawn (Entity, ModelIndex ("models/objects/gekkgib/leg/tris.md2"), 20, GIB_ORGANIC, EF_GREENGIB);
		CGibEntity::Spawn (Entity, ModelIndex ("models/objects/gekkgib/leg/tris.md2"), 20, GIB_ORGANIC, EF_GREENGIB);
		CGibEntity::Spawn (Entity, ModelIndex ("models/objects/gekkgib/pelvis/tris.md2"), 20, GIB_ORGANIC, EF_GREENGIB);

		Entity->ThrowHead (ModelIndex ("models/objects/gekkgib/head/tris.md2"), 20, GIB_ORGANIC, EF_GREENGIB);
		return;
	}

	if (Entity->DeadFlag)
		return;

	Entity->PlaySound (CHAN_VOICE, Sounds[SOUND_DEATH]);
	Entity->DeadFlag = true;
	Entity->CanTakeDamage = true;
	Entity->State.GetSkinNum() = 2;

	if (Entity->WaterInfo.Level)
		CurrentMove = &GekkMoveWDeath;
	else
	{
		switch (irandom(3))
		{
		case 0:
			CurrentMove = &GekkMoveDeath1;
			break;
		case 1:
			CurrentMove = &GekkMoveDeath3;
			break;
		case 2:
			CurrentMove = &GekkMoveDeath4;
			break;
		}
	}

}

/*
duck
*/
#if !ROGUE_FEATURES
void CGekk::Duck_Down ()
{
	if (AIFlags & AI_DUCKED)
		return;
	AIFlags |= AI_DUCKED;
	Entity->GetMaxs().Z -= 32;
	PauseTime = Level.Frame + 10;
	Entity->Link ();
}

void CGekk::Duck_Up ()
{
	AIFlags &= ~AI_DUCKED;
	Entity->GetMaxs().Z += 32;
	Entity->Link ();
}

void CGekk::Duck_Hold ()
{
	if (Level.Frame >= PauseTime)
		AIFlags &= ~AI_HOLD_FRAME;
	else
		AIFlags |= AI_HOLD_FRAME;
}
#else
void CGekk::SideStep ()
{
	if (AIFlags & AI_STAND_GROUND)
		CurrentMove = &GekkMoveStand;
	else
		CurrentMove = &GekkMoveRun;
}
#endif

CFrame GekkFramesLDuck[] = 
{
	CFrame (&CMonster::AI_Move,   0
#if MONSTERS_USE_ROGUE_AI
	, &CMonster::DuckDown
#endif
	), // frame 0
	CFrame (&CMonster::AI_Move,   0), // frame 1
	CFrame (&CMonster::AI_Move,   0), // frame 2
	CFrame (&CMonster::AI_Move,   0), // frame 3
	CFrame (&CMonster::AI_Move,   0), // frame 4
	CFrame (&CMonster::AI_Move,   0), // frame 5
	CFrame (&CMonster::AI_Move,   0), // frame 6
	CFrame (&CMonster::AI_Move,   0
#if MONSTERS_USE_ROGUE_AI
	, &CMonster::DuckUp
#endif
	), // frame 7
	CFrame (&CMonster::AI_Move,   0), // frame 8
	CFrame (&CMonster::AI_Move,   0), // frame 9

	CFrame (&CMonster::AI_Move,   0), // frame 10
	CFrame (&CMonster::AI_Move,   0), // frame 11
	CFrame (&CMonster::AI_Move,   0)  // frame 12

};
CAnim GekkMoveLDuck (FRAME_lduck_01, FRAME_lduck_13, GekkFramesLDuck, &CMonster::Run);

CFrame GekkFramesRDuck[] = 
{
	CFrame (&CMonster::AI_Move,   0
#if MONSTERS_USE_ROGUE_AI
	, &CMonster::DuckDown
#endif
	), // frame 0
	CFrame (&CMonster::AI_Move,   0), // frame 1
	CFrame (&CMonster::AI_Move,   0), // frame 2
	CFrame (&CMonster::AI_Move,   0), // frame 3
	CFrame (&CMonster::AI_Move,   0), // frame 4
	CFrame (&CMonster::AI_Move,   0), // frame 5
	CFrame (&CMonster::AI_Move,   0), // frame 6
	CFrame (&CMonster::AI_Move,   0
#if MONSTERS_USE_ROGUE_AI
	, &CMonster::DuckUp
#endif
	), // frame 7
	CFrame (&CMonster::AI_Move,   0), // frame 8
	CFrame (&CMonster::AI_Move,   0), // frame 9
	CFrame (&CMonster::AI_Move,   0), // frame 10
	CFrame (&CMonster::AI_Move,   0), // frame 11
	CFrame (&CMonster::AI_Move,   0) // frame 12

};
CAnim GekkMoveRDuck (FRAME_rduck_01, FRAME_rduck_13, GekkFramesRDuck, &CMonster::Run);

void CGekk::Attack ()
{
	Jump ();
}

void CGekk::
#if !ROGUE_FEATURES
	Dodge 
#else
	Duck
#endif
	(
#if !ROGUE_FEATURES
	IBaseEntity *Attacker, 
#endif
	float eta)
{
	if (frand() > 0.25)
		return;

#if !ROGUE_FEATURES
	if (!Entity->Enemy)
		Entity->Enemy = Attacker;
#endif

	if (Entity->WaterInfo.Level)
	{
		CurrentMove = &GekkMoveAttack;
		return;
	}

	switch (CvarList[CV_SKILL].Integer())
	{
	case 0:
		if (frand() > 0.5)
			CurrentMove = &GekkMoveLDuck;
		else 
			CurrentMove = &GekkMoveRDuck;

#if ROGUE_FEATURES
			// has to be done immediately otherwise he can get stuck
			DuckDown();
#endif
		return;
	case 1:
		PauseTime = Level.Frame + ((eta + 0.3) * 10);
		if (frand() > 0.33)
		{
			if (frand() > 0.5)
				CurrentMove = &GekkMoveLDuck;
			else 
				CurrentMove = &GekkMoveRDuck;

#if ROGUE_FEATURES
			// has to be done immediately otherwise he can get stuck
			DuckDown();
#endif
		}
		else
		{
			if (frand() > 0.66)
				CurrentMove = &GekkMoveAttack1;
			else 
				CurrentMove = &GekkMoveAttack2;
		}
		return;
	case 2:
		PauseTime = Level.Frame + ((eta + 0.3) * 10);
		if (frand() > 0.66)
		{
			if (frand() > 0.5)
				CurrentMove = &GekkMoveLDuck;
			else 
				CurrentMove = &GekkMoveRDuck;

#if ROGUE_FEATURES
			// has to be done immediately otherwise he can get stuck
			DuckDown();
#endif
		}
		else
		{
			if (frand() > 0.66)
				CurrentMove = &GekkMoveAttack1;
			else 
				CurrentMove = &GekkMoveAttack2;
		}
	default:
		PauseTime = Level.Frame + ((eta + 0.3) * 10);
		if (frand() > 0.66)
			CurrentMove = &GekkMoveAttack1;
		else 
			CurrentMove = &GekkMoveAttack2;
	};	
}

//
// SPAWN
//

/*QUAKED monster_gekk (1 .5 0) (-24 -24 -24) (24 24 24) Ambush Trigger_Spawn Sight Chant
*/
void CGekk::Spawn ()
{
	Sounds[SOUND_SWING] = SoundIndex ("gek/gk_atck1.wav");
	Sounds[SOUND_HIT] = SoundIndex ("gek/gk_atck2.wav");
	Sounds[SOUND_HIT2] = SoundIndex ("gek/gk_atck3.wav");
	Sounds[SOUND_DEATH] = SoundIndex ("gek/gk_deth1.wav");
	Sounds[SOUND_PAIN1] = SoundIndex ("gek/gk_pain1.wav");
	Sounds[SOUND_SIGHT] = SoundIndex ("gek/gk_sght1.wav");
	Sounds[SOUND_SEARCH] = SoundIndex ("gek/gk_idle1.wav");
	Sounds[SOUND_STEP1] = SoundIndex ("gek/gk_step1.wav");
	Sounds[SOUND_STEP2] = SoundIndex ("gek/gk_step2.wav");
	Sounds[SOUND_STEP3] = SoundIndex ("gek/gk_step3.wav");
	Sounds[SOUND_THUD] = SoundIndex ("mutant/thud1.wav");

	Sounds[SOUND_CHANT_LOW] = SoundIndex ("gek/gek_low.wav");
	Sounds[SOUND_CHANT_MID] = SoundIndex ("gek/gek_mid.wav");
	Sounds[SOUND_CHANT_HIGH] = SoundIndex ("gek/gek_high.wav");

	Entity->GetSolid() = SOLID_BBOX;
	Entity->State.GetModelIndex() = ModelIndex ("models/monsters/gekk/tris.md2");
	Entity->GetMins().Set (-24, -24, -24);
	Entity->GetMaxs().Set (24, 24, 24);

	ModelIndex ("models/objects/gekkgib/pelvis/tris.md2");
	ModelIndex ("models/objects/gekkgib/arm/tris.md2");
	ModelIndex ("models/objects/gekkgib/torso/tris.md2");
	ModelIndex ("models/objects/gekkgib/claw/tris.md2");
	ModelIndex ("models/objects/gekkgib/leg/tris.md2");
	ModelIndex ("models/objects/gekkgib/head/tris.md2");

	Entity->Health = 125;
	Entity->GibHealth = -30;
	Entity->Mass = 300;

	MonsterFlags = (MF_HAS_ATTACK | MF_HAS_SIGHT | MF_HAS_SEARCH | MF_HAS_IDLE | MF_HAS_MELEE
#if ROGUE_FEATURES
		| MF_HAS_DODGE | MF_HAS_DUCK | MF_HAS_UNDUCK
#endif
		);

	Entity->Link ();
	Entity->Touchable = true;

	CurrentMove = &GekkMoveStand;

	WalkMonsterStart ();

	if (Entity->SpawnFlags & 8)
		CurrentMove = &GekkMoveChant;
}

void CGekk::WaterToLand ()
{
	Entity->Flags &= ~FL_SWIM;
	YawSpeed = 20;
	Entity->ViewHeight = 25;

	CurrentMove = &GekkMoveLeapAtk2;

	Entity->GetMins().Set (-24, -24, -24);
	Entity->GetMaxs().Set (24, 24, 24);
}

void CGekk::LandToWater ()
{
	Entity->Flags |= FL_SWIM;
	YawSpeed = 10;
	Entity->ViewHeight = 10;

	CurrentMove = &GekkMoveSwimStart;

	Entity->GetMins().Set (-24, -24, -24);
	Entity->GetMaxs().Set (24, 24, 16);
}

LINK_MONSTER_CLASSNAME_TO_CLASS ("monster_gekk", CGekk);
#endif
