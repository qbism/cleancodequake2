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
// cc_turret_entities.cpp
// Turret entities
//

#include "cc_local.h"
#include "cc_brushmodels.h"
#include "cc_infantry.h"
#include "cc_turret_entities.h"

void AnglesNormalize(vec3f &vec)
{
	while(vec.X > 360)
		vec.X -= 360;
	while(vec.X < 0)
		vec.X += 360;
	while(vec.Y > 360)
		vec.Y -= 360;
	while(vec.Y < 0)
		vec.Y += 360;
}

float SnapToEights(float x)
{
	x *= 8.0;
	if (x > 0.0)
		x += 0.5;
	else
		x -= 0.5;
	return 0.125 * (sint32)x;
}

CTurretEntityBase::CTurretEntityBase () :
CBaseEntity (),
CMapEntity (),
CBrushModel (),
CBlockableEntity ()
{
};

CTurretEntityBase::CTurretEntityBase (sint32 Index) :
CBaseEntity (Index),
CMapEntity (Index),
CBrushModel (Index),
CBlockableEntity (Index)
{
};

bool CTurretEntityBase::Run ()
{
	return CBrushModel::Run ();
};

void CTurretEntityBase::Blocked (CBaseEntity *other)
{
	if ((other->EntityFlags & ENT_HURTABLE) && entity_cast<CHurtableEntity>(other)->CanTakeDamage)
		entity_cast<CHurtableEntity>(other)->TakeDamage (this, (Team.Master->GetOwner()) ? Team.Master->GetOwner() : Team.Master,
					vec3fOrigin, other->State.GetOrigin(), vec3fOrigin, entity_cast<CBrushModel>(Team.Master)->Damage, 10, 0, MOD_CRUSH);
}

/*QUAKED turret_breach (0 0 0) ?
This portion of the turret can change both pitch and yaw.
The model  should be made with a flat pitch.
It (and the associated base) need to be oriented towards 0.
Use "angle" to set the starting angle.

"speed"		default 50
"dmg"		default 10
"angle"		point this forward
"target"	point this at an info_notnull at the muzzle tip
"minpitch"	min acceptable pitch angle : default -30
"maxpitch"	max acceptable pitch angle : default 30
"minyaw"	min acceptable yaw angle   : default 0
"maxyaw"	max acceptable yaw angle   : default 360
*/

CTurretBreach::CTurretBreach () :
CBaseEntity (),
CTurretEntityBase (),
FinishInit(true),
ShouldFire(false)
{
	PitchOptions[0] = PitchOptions[1] = PitchOptions[2] = PitchOptions[3] = 0;
};

CTurretBreach::CTurretBreach (sint32 Index) :
CBaseEntity (Index),
CTurretEntityBase (Index),
FinishInit(true),
ShouldFire(false)
{
	PitchOptions[0] = PitchOptions[1] = PitchOptions[2] = PitchOptions[3] = 0;
};

void CTurretBreach::Fire ()
{
	vec3f f, r, u;
	State.GetAngles().ToVectors (&f, &r, &u);
	vec3f start = State.GetOrigin().MultiplyAngles (MoveOrigin.X, f);
	start = start.MultiplyAngles (MoveOrigin.Y, r);
	start = start.MultiplyAngles (MoveOrigin.Z, u);

	sint32 damage = 100 + frand() * 50;
	CRocket::Spawn (Team.Master->GetOwner(), start, f, damage, 550 + 50 * skill->Integer(), 150, damage);
	PlayPositionedSound (start, CHAN_WEAPON, SoundIndex("weapons/rocklf1a.wav"));
}

class CAvelocityForEachTeamChainCallback : public CForEachTeamChainCallback
{
public:
	float avelYaw;

	CAvelocityForEachTeamChainCallback (float avelYaw) :
	avelYaw(avelYaw)
	{
	};

	void Callback (CBaseEntity *Entity)
	{
		if (Entity->EntityFlags & ENT_PHYSICS)
			entity_cast<CPhysicsEntity>(Entity)->AngularVelocity.Y = avelYaw;
	};
};

void CTurretBreach::Think ()
{
	if (FinishInit)
	{
		FinishInit = false;

		// get and save info for muzzle location
		if (!Target)
			MapPrint (MAPPRINT_ERROR, this, State.GetOrigin(), "Needs a target\n");
		else
		{
			CBaseEntity *targ = CC_PickTarget (Target);
			MoveOrigin = (targ->State.GetOrigin() - State.GetOrigin());
			targ->Free();

			QDelete Target;
			Target = NULL;
		}

		entity_cast<CBrushModel>(Team.Master)->Damage = Damage;
		Think ();
	}
	else
	{
		vec3f current_angles = State.GetAngles ();

		AnglesNormalize(current_angles);
		AnglesNormalize(MoveAngles);

		if (MoveAngles.X > 180)
			MoveAngles.X -= 360;

		// clamp angles to mins & maxs
		if (MoveAngles.X > Positions[0].X)
			MoveAngles.X = Positions[0].X;
		else if (MoveAngles.X < Positions[1].X)
			MoveAngles.X = Positions[1].X;

		if ((MoveAngles.Y < Positions[0].Y) || (MoveAngles.Y > Positions[1].Y))
		{
			float dmin = Q_fabs(Positions[0].Y - MoveAngles.Y);
			if (dmin < -180)
				dmin += 360;
			else if (dmin > 180)
				dmin -= 360;

			float dmax = Q_fabs(Positions[1].Y - MoveAngles.Y);
			if (dmax < -180)
				dmax += 360;
			else if (dmax > 180)
				dmax -= 360;

			MoveAngles.Y = Positions[(Q_fabs(dmin) < Q_fabs(dmax)) ? 0 : 1].Y;
		}

		vec3f delta = (MoveAngles - current_angles);
		if (delta.X < -180)
			delta.X += 360;
		else if (delta.X > 180)
			delta.X -= 360;
		if (delta.Y < -180)
			delta.Y += 360;
		else if (delta.Y > 180)
			delta.Y -= 360;
		delta.Z = 0;

		if (delta.X > Speed * 0.1f)
			delta.X = Speed * 0.1f;
		if (delta.X < -1 * Speed * 0.1f)
			delta.X = -1 * Speed * 0.1f;
		if (delta.Y > Speed * 0.1f)
			delta.Y = Speed * 0.1f;
		if (delta.Y < -1 * Speed * 0.1f)
			delta.Y = -1 * Speed * 0.1f;

		AngularVelocity = delta;
		NextThink = level.Frame + FRAMETIME;

		CAvelocityForEachTeamChainCallback (AngularVelocity.Y).Query (this);

		// if we have adriver, adjust his velocities
		if (GetOwner())
		{
			float	angle;
			float	target_z;
			float	diff;
			vec3f	target;
			vec3f	dir;
			CMonsterEntity *TheDriver = entity_cast<CMonsterEntity>(GetOwner());
			CTurretDriver *Driver = dynamic_cast<CTurretDriver*>(TheDriver->Monster);

			// angular is easy, just copy ours
			TheDriver->AngularVelocity.X = AngularVelocity.Z;
			TheDriver->AngularVelocity.Y = AngularVelocity.Y;

			// x & y
			angle = State.GetAngles().Y + Driver->MoveOrigin.Y;
			angle *= (((float)M_PI)*2 / 360);
			target.X = SnapToEights(State.GetOrigin().X + cosf(angle) * Driver->MoveOrigin.X);
			target.Y = SnapToEights(State.GetOrigin().Y + sinf(angle) * Driver->MoveOrigin.X);
			target.Z = TheDriver->State.GetOrigin().Z;

			dir = target - TheDriver->State.GetOrigin();

			TheDriver->Velocity.X = dir.X * 1.0 / 1;
			TheDriver->Velocity.Y = dir.Y * 1.0 / 1;

			// z
			angle = State.GetAngles().X * (M_PI*2 / 360);
			target_z = SnapToEights(State.GetOrigin().Z + Driver->MoveOrigin.X * tan(angle) + Driver->MoveOrigin.Z);

			diff = target_z - TheDriver->State.GetOrigin().Z;
			TheDriver->Velocity.Z = diff * 1.0 / 1;

			if (ShouldFire)
			{
				Fire ();
				ShouldFire = false;
			}
		}
	}
}

ENTITYFIELDS_BEGIN(CTurretBreach)
{
	CEntityField ("minpitch", EntityMemberOffset(CTurretBreach,PitchOptions[0]), FT_FLOAT | FT_SAVABLE),
	CEntityField ("maxpitch", EntityMemberOffset(CTurretBreach,PitchOptions[1]), FT_FLOAT | FT_SAVABLE),
	CEntityField ("minyaw", EntityMemberOffset(CTurretBreach,PitchOptions[2]), FT_FLOAT | FT_SAVABLE),
	CEntityField ("maxyaw", EntityMemberOffset(CTurretBreach,PitchOptions[3]), FT_FLOAT | FT_SAVABLE),
	CEntityField ("target", EntityMemberOffset(CTurretBreach,Target), FT_LEVEL_STRING | FT_SAVABLE),
};
ENTITYFIELDS_END(CTurretBreach)

bool			CTurretBreach::ParseField (const char *Key, const char *Value)
{
	if (CheckFields<CTurretBreach> (this, Key, Value))
		return true;

	// Couldn't find it here
	return (CTurretEntityBase::ParseField (Key, Value));
};

void			CTurretBreach::SaveFields (CFile &File)
{
	SaveEntityFields <CTurretBreach> (this, File);
	CTurretEntityBase::SaveFields (File);
}

void			CTurretBreach::LoadFields (CFile &File)
{
	LoadEntityFields <CTurretBreach> (this, File);
	CTurretEntityBase::LoadFields (File);
}

void CTurretBreach::Spawn ()
{
	GetSolid() = SOLID_BSP;
	PhysicsType = PHYSICS_PUSH;
	SetBrushModel ();

	if (!Speed)
		Speed = 50;
	if (!Damage)
		Damage = 10;

	if (!PitchOptions[0])
		PitchOptions[0] = -30;
	if (!PitchOptions[1])
		PitchOptions[1] = 30;
	if (!PitchOptions[3])
		PitchOptions[3] = 360;

	Positions[0].X = -1 * PitchOptions[0];
	Positions[0].Y = PitchOptions[2];
	Positions[1].X = -1 * PitchOptions[1];
	Positions[1].Y = PitchOptions[3];

	MoveAngles.Y = State.GetAngles().Y;
	State.GetAngles().Clear ();

	NextThink = level.Frame + FRAMETIME;
	Link ();
}

LINK_CLASSNAME_TO_CLASS ("turret_breach", CTurretBreach);

/*QUAKED turret_base (0 0 0) ?
This portion of the turret changes yaw only.
MUST be teamed with a turret_breach.
*/

CTurretBase::CTurretBase () :
CBaseEntity (),
CTurretEntityBase ()
{
};

CTurretBase::CTurretBase (sint32 Index) :
CBaseEntity (Index),
CTurretEntityBase (Index)
{
};

void CTurretBase::Spawn ()
{
	GetSolid() = SOLID_BSP;
	PhysicsType = PHYSICS_PUSH;
	SetBrushModel ();
	Link ();
}

LINK_CLASSNAME_TO_CLASS ("turret_base", CTurretBase);

// Paril Note
// This may seem like a bit of a jump to call this guy a "monster",
// but he does have a lot of monster-like qualities.
// The only difference you'll see in-game is that in CleanCode
// the turret driver will not gib when you kill him; he'll do the
// infantry's death animation, like I hope they intended!

/*QUAKED turret_driver (1 .5 0) (-16 -16 -24) (16 16 32)
Must NOT be on the team with the rest of the turret parts.
Instead it must target the turret_breach.
*/

CTurretDriver::CTurretDriver (uint32 ID) :
CInfantry (ID),
TargetedBreach(NULL)
{
};

void CTurretDriver::Die (CBaseEntity *inflictor, CBaseEntity *attacker, sint32 damage, vec3f &point)
{
// check for gib
	if (Entity->Health <= Entity->GibHealth)
	{
		Entity->PlaySound (CHAN_VOICE, SoundIndex ("misc/udeath.wav"));
		for (sint32 n= 0; n < 2; n++)
			CGibEntity::Spawn (Entity, GameMedia.Gib_Bone[0], damage, GIB_ORGANIC);
		for (sint32 n= 0; n < 4; n++)
			CGibEntity::Spawn (Entity, GameMedia.Gib_SmallMeat, damage, GIB_ORGANIC);
		Entity->ThrowHead (GameMedia.Gib_Head[1], damage, GIB_ORGANIC);
		Entity->DeadFlag = true;
		return;
	}

	if (Entity->DeadFlag == true)
		return;

	CBaseEntity	*ent;

	// level the gun
	TargetedBreach->MoveAngles.X = 0;

	// remove the driver from the end of them team chain
	for (ent = TargetedBreach->Team.Master; ent->Team.Chain != Entity; ent = ent->Team.Chain)
		;
	ent->Team.Chain = NULL;
	Entity->Team.Master = NULL;
	Entity->Flags &= ~FL_TEAMSLAVE;
	Entity->Velocity.Clear ();

	TargetedBreach->SetOwner (NULL);
	TargetedBreach->Team.Master->SetOwner (NULL);

	CInfantry::Die (inflictor, attacker, damage, point);
	Think = &CMonster::MonsterThink;
	Entity->NextThink = level.Frame + FRAMETIME;
}

void CTurretDriver::Pain (CBaseEntity *other, float kick, sint32 damage)
{
	if (Entity->Health < (Entity->MaxHealth / 2))
		Entity->State.GetSkinNum() = 1;

	if (level.Frame < PainDebounceTime)
		return;

	PainDebounceTime = level.Frame + 30;
	Entity->PlaySound (CHAN_VOICE, (!irandom(2) == 0) ? SoundPain1 : SoundPain2);
}

void CTurretDriver::TurretThink ()
{
	Entity->NextThink = level.Frame + FRAMETIME;

	if (Entity->Enemy && (!Entity->Enemy->GetInUse() || entity_cast<CHurtableEntity>(Entity->Enemy)->Health <= 0))
		Entity->Enemy = NULL;

	if (!Entity->Enemy)
	{
		if (!FindTarget ())
			return;
		TrailTime = level.Frame;
		AIFlags &= ~AI_LOST_SIGHT;
	}
	else
	{
		if (IsVisible (Entity, Entity->Enemy))
		{
			if (AIFlags & AI_LOST_SIGHT)
			{
				TrailTime = level.Frame;
				AIFlags &= ~AI_LOST_SIGHT;
			}
		}
		else
		{
			AIFlags |= AI_LOST_SIGHT;
			return;
		}
	}

	if (Entity->Enemy)
	{
		// let the turret know where we want it to aim
		vec3f dir = (Entity->Enemy->State.GetOrigin() +
			vec3f(0, 0, Entity->Enemy->ViewHeight)) -
			TargetedBreach->State.GetOrigin();

		vec3f ang = dir.ToAngles ();
		TargetedBreach->MoveAngles = ang;

		// decide if we should shoot
		if (level.Frame < AttackFinished)
			return;

		float reaction_time = (3 - skill->Integer()) * 1.0;
		if ((level.Frame - TrailTime) < (reaction_time * 10))
			return;

		AttackFinished = level.Frame + ((reaction_time + 1.0) * 10);

		TargetedBreach->ShouldFire = true;
	}
}

void CTurretDriver::TurretLink ()
{
	Think = static_cast<void (__thiscall CMonster::* )()>(&CTurretDriver::TurretThink);
	Entity->NextThink = level.Frame + FRAMETIME;

	TargetedBreach = entity_cast<CTurretBreach>(CC_PickTarget (Entity->Target));
	TargetedBreach->SetOwner (Entity);
	TargetedBreach->Team.Master->SetOwner (Entity);
	Entity->State.GetAngles() = TargetedBreach->State.GetAngles();

	vec3f vec = (TargetedBreach->State.GetOrigin() - Entity->State.GetOrigin());
	vec.Z = 0;
	MoveOrigin.X = vec.Length();

	vec = (Entity->State.GetOrigin() - TargetedBreach->State.GetOrigin());
	vec = vec.ToAngles();
	AnglesNormalize(vec);
	MoveOrigin.Y = vec.Y;

	MoveOrigin.Z = Entity->State.GetOrigin().Z - TargetedBreach->State.GetOrigin().Z;

	// add the driver to the end of them team chain
	CBaseEntity	*ent;
	for (ent = TargetedBreach->Team.Master; ent->Team.Chain; ent = ent->Team.Chain)
		;
	ent->Team.Chain = Entity;
	Entity->Team.Master = TargetedBreach->Team.Master;
	Entity->Flags |= FL_TEAMSLAVE;
}

void CTurretDriver::Spawn ()
{
	if (deathmatch->Integer())
	{
		Entity->Free ();
		return;
	}

	Entity->PhysicsType = PHYSICS_PUSH;
	Entity->GetSolid() = SOLID_BBOX;
	Entity->State.GetModelIndex() = ModelIndex("models/monsters/infantry/tris.md2");
	Entity->GetMins().Set (-16, -16, -24);
	Entity->GetMaxs().Set (16, 16, 32);

	Entity->Health = Entity->MaxHealth = 100;
	Entity->GibHealth = -40;
	Entity->Mass = 200;
	Entity->ViewHeight = 24;

	SoundPain1 = SoundIndex ("infantry/infpain1.wav");
	SoundPain2 = SoundIndex ("infantry/infpain2.wav");
	SoundDie1 = SoundIndex ("infantry/infdeth1.wav");
	SoundDie2 = SoundIndex ("infantry/infdeth2.wav");

	Entity->Flags |= FL_NO_KNOCKBACK;

	level.Monsters.Total++;

	Entity->GetSvFlags() |= SVF_MONSTER;
	Entity->State.GetRenderEffects() |= RF_FRAMELERP;
	Entity->CanTakeDamage = true;
	Entity->GetClipmask() = CONTENTS_MASK_MONSTERSOLID;
	Entity->State.GetOldOrigin() = Entity->State.GetOrigin ();
	AIFlags |= (AI_STAND_GROUND);

	Think = static_cast<void (__thiscall CMonster::* )()>(&CTurretDriver::TurretLink);
	Entity->NextThink = level.Frame + FRAMETIME;

	Entity->Link ();
}

LINK_MONSTER_CLASSNAME_TO_CLASS ("turret_driver", CTurretDriver);