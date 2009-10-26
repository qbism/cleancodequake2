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
// cc_misc_entities.cpp
// misc_* entities
//

#include "cc_local.h"
#include "cc_tent.h"

/*QUAKED misc_explobox (0 .5 .8) (-16 -16 0) (16 16 40)
Large exploding box.  You can override its mass (100),
health (80), and dmg (150).
*/
class CMiscExploBox : public CMapEntity, public CStepPhysics, public CHurtableEntity, public CThinkableEntity, public CTouchableEntity
{
	bool		Dropped;
	CBaseEntity	*Shooter;

public:
	int			Explosivity;
	int			Damage;

	CMiscExploBox () :
	Dropped(false),
	CBaseEntity(),
	CMapEntity(),
	CThinkableEntity(),
	CHurtableEntity (),
	CStepPhysics(),
	Explosivity(100),
	Shooter(NULL),
	Damage(150)
	{
	};

	CMiscExploBox (int Index) : 
	Dropped(false),
	CBaseEntity(Index),
	CMapEntity(Index),
	CThinkableEntity(),
	CHurtableEntity(Index),
	CStepPhysics(Index),
	Explosivity(100),
	Shooter(NULL),
	Damage(150)
	{
	};

	ENTITYFIELD_DEFS

#define BARREL_STEPSIZE 8
	void Touch (CBaseEntity *other, plane_t *plane, cmBspSurface_t *surf)
	{
		if ((!other->GroundEntity) || (other->GroundEntity == this))
			return;
		if (!(other->EntityFlags & ENT_PHYSICS))
			return;

		float ratio = entity_cast<CPhysicsEntity>(other)->Mass / Mass;
		vec3f v = State.GetOrigin() - other->State.GetOrigin();
		float Yaw = (v.ToYaw ()*M_PI*2 / 360);
		vec3f move ( cosf(Yaw)*(2 * ratio),
							sinf(Yaw)*(2 * ratio),
							0);

		vec3f	oldOrigin = State.GetOrigin(),
				newOrigin = (oldOrigin + move);

		newOrigin.Z += BARREL_STEPSIZE;
		vec3f end (newOrigin);

		end.Z -= BARREL_STEPSIZE*2;

		CTrace trace (newOrigin, GetMins(), GetMaxs(), end, this, CONTENTS_MASK_MONSTERSOLID);

		if (trace.allSolid)
			return;

		if (trace.startSolid)
		{
			newOrigin[2] -= BARREL_STEPSIZE;
			trace (newOrigin, GetMins(), GetMaxs(), end, this, CONTENTS_MASK_MONSTERSOLID);
			if (trace.allSolid || trace.startSolid)
				return;
		}

	// check point traces down for dangling corners
		State.GetOrigin() = trace.EndPos;

		GroundEntity = trace.Ent;
		GroundEntityLinkCount = trace.Ent->GetLinkCount();

		if (trace.fraction == 1.0)
			GroundEntity = NULL;

	// the move is ok
		Link ();
		G_TouchTriggers (this);
	}

	void Think ()
	{
		if (!Dropped)
		{
			Dropped = true;

			CTrace		trace;
			vec3f		origin = State.GetOrigin();

			origin.Z += 1;
			vec3f end = vec3f(origin);
			end.Z -= 256;
			
			trace (origin, GetMins(), GetMaxs(), end, this, CONTENTS_MASK_MONSTERSOLID);

			if (trace.fraction == 1 || trace.allSolid)
				return;

			State.GetOrigin() = trace.EndPos;
			Link();
			return;
		}
		T_RadiusDamage (this, Shooter, Damage, NULL, Damage+40, MOD_BARREL);

		vec3f origin = State.GetOrigin ();
		CTempEnt_Explosions::GrenadeExplosion (origin, this);

		Free ();
	};

	void Die (CBaseEntity *inflictor, CBaseEntity *attacker, int damage, vec3f &point)
	{
		CanTakeDamage = false;
		NextThink = level.Frame + 2;
		Shooter = attacker;
	};

	void Pain (CBaseEntity *other, float kick, int damage) {};

	bool Run ()
	{
		return CStepPhysics::Run ();
	};

	void Spawn ()
	{
		if (deathmatch->Boolean())
		{
			Free ();
			return;
		}

		GetSolid() = SOLID_BBOX;

		State.GetModelIndex() = ModelIndex ("models/objects/barrels/tris.md2");

		GetMins().Set (-16, -16, 0);
		GetMaxs().Set (16, 16, 40);

		if (!Explosivity)
			Explosivity = 400;
		Mass = Explosivity;

		if (!Health)
			Health = 10;

		if (!Damage)
			Damage = 150;

		CanTakeDamage = true;
		NextThink = level.Frame + FRAMETIME;

		Link ();
	};
};

ENTITYFIELDS_BEGIN(CMiscExploBox)
{
	CEntityField ("mass", EntityMemberOffset(CMiscExploBox,Explosivity), FT_INT),
	CEntityField ("dmg", EntityMemberOffset(CMiscExploBox,Damage), FT_INT),
};
ENTITYFIELDS_END(CMiscExploBox)

bool			CMiscExploBox::ParseField (const char *Key, const char *Value)
{
	if (CheckFields<CMiscExploBox> (this, Key, Value))
		return true;

	return (CHurtableEntity::ParseField (Key, Value) || CMapEntity::ParseField (Key, Value));
};

LINK_CLASSNAME_TO_CLASS ("misc_explobox",CMiscExploBox);

#include "cc_brushmodels.h"
/*QUAKED misc_viper (1 .5 0) (-16 -16 0) (16 16 32)
This is the Viper for the flyby bombing.
It is trigger_spawned, so you must have something use it for it to show up.
There must be a path for it to follow once it is activated.

"speed"		How fast the Viper should fly
*/
class CMiscViper : public CTrainBase, public CTouchableEntity
{
	bool MyUse;
public:
	CMiscViper() :
		CBaseEntity (),
		CTrainBase(),
		MyUse(true)
	{
	};

	CMiscViper(int Index) :
		CBaseEntity (Index),
		CTrainBase(Index),
		MyUse(true)
	{
	};

	bool Run ()
	{
		return CTrainBase::Run ();
	};

	virtual void Use (CBaseEntity *other, CBaseEntity *activator)
	{
		if (MyUse)
		{
			GetSvFlags() &= ~SVF_NOCLIENT;
			MyUse = false;
		}
		CTrainBase::Use (other, activator);
	};

	virtual void Spawn ()
	{
		if (!Target)
		{
			//gi.dprintf ("misc_viper without a target at (%f %f %f)\n", ent->absMin[0], ent->absMin[1], ent->absMin[2]);
			MapPrint (MAPPRINT_ERROR, this, State.GetOrigin(), "No targetname\n");
			Free ();
			return;
		}

		if (!Speed)
			Speed = 300;

		PhysicsType = PHYSICS_PUSH;
		Touchable = true;
		GetSolid() = SOLID_NOT;
		State.GetModelIndex() = ModelIndex ("models/ships/viper/tris.md2");
		GetMins().Set (-16, -16, 0);
		GetMaxs().Set (16, 16, 32);

		NextThink = level.Frame + FRAMETIME;
		GetSvFlags() |= SVF_NOCLIENT;
		Accel = Decel = Speed;

		Link ();
	};
};

LINK_CLASSNAME_TO_CLASS ("misc_viper", CMiscViper);

class CMiscStroggShip : public CMiscViper
{
public:
	CMiscStroggShip () :
	  CBaseEntity (),
	  CMiscViper ()
	  {
	  };

	CMiscStroggShip (int Index) :
	  CBaseEntity (Index),
	  CMiscViper (Index)
	  {
	  };

	bool Run ()
	{
		return CTrainBase::Run ();
	};

	void Spawn ()
	{
		CMiscViper::Spawn ();
		State.GetModelIndex() = ModelIndex ("models/ships/strogg1/tris.md2");
	};
};

LINK_CLASSNAME_TO_CLASS ("misc_strogg_ship", CMiscStroggShip);

/*QUAKED misc_banner (1 .5 0) (-4 -4 -4) (4 4 4)
The origin is the bottom of the banner.
The banner is 128 tall.
*/
class CMiscBanner : public CMapEntity, public CThinkableEntity
{
public:
	CMiscBanner () :
	  CBaseEntity (),
	  CMapEntity (),
	  CThinkableEntity ()
	{
	};

	CMiscBanner (int Index) :
	  CBaseEntity (Index),
	  CMapEntity (Index),
	  CThinkableEntity (Index)
	{
	};

	bool Run ()
	{
		return CBaseEntity::Run();
	};

	void Think ()
	{
		State.GetFrame() = (State.GetFrame() + 1) % 16;
		NextThink = level.Frame + FRAMETIME;
	};

	void Spawn ()
	{
		GetSolid() = SOLID_NOT;
		State.GetModelIndex() = ModelIndex ("models/objects/banner/tris.md2");
		State.GetFrame() = irandom(16);
		Link ();

		NextThink = level.Frame + FRAMETIME;
	};
};

LINK_CLASSNAME_TO_CLASS ("misc_banner", CMiscBanner);

/*QUAKED misc_blackhole (1 .5 0) (-8 -8 -8) (8 8 8)
*/
class CMiscBlackhole : public CMapEntity, public CThinkableEntity, public CUsableEntity
{
public:
	CMiscBlackhole () :
	  CBaseEntity (),
	  CMapEntity (),
	  CThinkableEntity (),
	  CUsableEntity ()
	{
	};

	CMiscBlackhole (int Index) :
	  CBaseEntity (Index),
	  CMapEntity (Index),
	  CThinkableEntity (Index),
	  CUsableEntity (Index)
	{
	};

	virtual bool ParseField (const char *Key, const char *Value)
	{
		return (CUsableEntity::ParseField (Key, Value) || CMapEntity::ParseField (Key, Value));
	}

	bool Run ()
	{
		return CBaseEntity::Run();
	};

	void Think ()
	{
		if (++State.GetFrame() >= 19)
			State.GetFrame() = 0;
		NextThink = level.Frame + FRAMETIME;
	};

	void Use (CBaseEntity *other, CBaseEntity *activator)
	{
		Free ();
	};

	void Spawn ()
	{
		GetSolid() = SOLID_NOT;
		GetMins().Set (-64, -64, 0);
		GetMaxs().Set (64, 64, 8);
		State.GetModelIndex() = ModelIndex ("models/objects/black/tris.md2");
		State.GetRenderEffects() = RF_TRANSLUCENT;
		NextThink = level.Frame + 2;
		Link ();
	};
};

LINK_CLASSNAME_TO_CLASS ("misc_blackhole", CMiscBlackhole);

/*QUAKED misc_eastertank (1 .5 0) (-32 -32 -16) (32 32 32)
*/
class CMiscEasterTank : public CMapEntity, public CThinkableEntity
{
public:
	CMiscEasterTank () :
	  CBaseEntity (),
	  CMapEntity (),
	  CThinkableEntity ()
	{
	};

	CMiscEasterTank (int Index) :
	  CBaseEntity (Index),
	  CMapEntity (Index),
	  CThinkableEntity (Index)
	{
	};

	bool Run ()
	{
		return CBaseEntity::Run();
	};

	void Think ()
	{
		if (++State.GetFrame() >= 293)
			State.GetFrame() = 254;

		NextThink = level.Frame + FRAMETIME;
	};

	void Spawn ()
	{
		GetSolid() = SOLID_BBOX;
		GetMins().Set (-32, -32, -16);
		GetMaxs().Set (32, 32, 32);
		State.GetModelIndex() = ModelIndex ("models/monsters/tank/tris.md2");
		State.GetFrame() = 254;
		NextThink = level.Frame + 2;
		Link ();
	};
};

LINK_CLASSNAME_TO_CLASS ("misc_eastertank", CMiscEasterTank);

/*QUAKED misc_easterchick (1 .5 0) (-32 -32 0) (32 32 32)
*/
class CMiscEasterChick : public CMapEntity, public CThinkableEntity
{
public:
	CMiscEasterChick () :
	  CBaseEntity (),
	  CMapEntity (),
	  CThinkableEntity ()
	{
	};

	CMiscEasterChick (int Index) :
	  CBaseEntity (Index),
	  CMapEntity (Index),
	  CThinkableEntity (Index)
	{
	};

	bool Run ()
	{
		return CBaseEntity::Run();
	};

	void Think ()
	{
		if (++State.GetFrame() >= 247)
			State.GetFrame() = 208;

		NextThink = level.Frame + FRAMETIME;
	};

	void Spawn ()
	{
		GetSolid() = SOLID_BBOX;
		GetMins().Set (-32, -32, 0);
		GetMaxs().Set (32, 32, 32);
		State.GetModelIndex() = ModelIndex ("models/monsters/bitch/tris.md2");
		State.GetFrame() = 208;
		NextThink = level.Frame + 2;
		Link ();
	};
};

LINK_CLASSNAME_TO_CLASS ("misc_easterchick", CMiscEasterChick);

/*QUAKED misc_easterchick2 (1 .5 0) (-32 -32 0) (32 32 32)
*/
class CMiscEasterChick2 : public CMapEntity, public CThinkableEntity
{
public:
	CMiscEasterChick2 () :
	  CBaseEntity (),
	  CMapEntity (),
	  CThinkableEntity ()
	{
	};

	CMiscEasterChick2 (int Index) :
	  CBaseEntity (Index),
	  CMapEntity (Index),
	  CThinkableEntity (Index)
	{
	};

	bool Run ()
	{
		return CBaseEntity::Run();
	};

	void Think ()
	{
		if (++State.GetFrame() >= 287)
			State.GetFrame() = 248;

		NextThink = level.Frame + FRAMETIME;
	};

	void Spawn ()
	{
		GetSolid() = SOLID_BBOX;
		GetMins().Set (-32, -32, 0);
		GetMaxs().Set (32, 32, 32);
		State.GetModelIndex() = ModelIndex ("models/monsters/bitch/tris.md2");
		State.GetFrame() = 248;
		NextThink = level.Frame + 2;
		Link ();
	};
};

LINK_CLASSNAME_TO_CLASS ("misc_easterchick2", CMiscEasterChick2);

/*QUAKED monster_commander_body (1 .5 0) (-32 -32 0) (32 32 48)
Not really a monster, this is the Tank Commander's decapitated body.
There should be a item_commander_head that has this as it's target.
*/
class CCommanderBody : public CMapEntity, public CThinkableEntity, public CUsableEntity, public CTossProjectile, public CHurtableEntity
{
public:
	bool Drop;

	CCommanderBody () :
	  CBaseEntity (),
	  CMapEntity (),
	  CThinkableEntity (),
	  CUsableEntity (),
	  CTossProjectile (),
	  Drop(true)
	{
	};

	CCommanderBody (int Index) :
	  CBaseEntity (Index),
	  CMapEntity (Index),
	  CThinkableEntity (Index),
	  CUsableEntity (Index),
	  CTossProjectile (Index),
	  Drop(true)
	{
	};

	virtual bool			ParseField (const char *Key, const char *Value)
	{
		return (CUsableEntity::ParseField (Key, Value) || CHurtableEntity::ParseField (Key, Value) || CMapEntity::ParseField (Key, Value));
	};

	bool Run ()
	{
		return (PhysicsType == PHYSICS_TOSS) ? CTossProjectile::Run() : CBaseEntity::Run();
	};

	void Think ()
	{
		if (!Drop)
		{
			NextThink = (++State.GetFrame() < 24) ? level.Frame + FRAMETIME : 0;
			if (State.GetFrame() == 22)
				PlaySound (CHAN_BODY, SoundIndex ("tank/thud.wav"));
		}
		else
		{
			Drop = false;
			PhysicsType = PHYSICS_TOSS;
			State.GetOrigin().Z += 2;
		}
	};

	void Use (CBaseEntity *other, CBaseEntity *activator)
	{
		NextThink = level.Frame + FRAMETIME;
		PlaySound (CHAN_BODY, SoundIndex ("tank/pain.wav"));
	};

	void Spawn ()
	{
		PhysicsType = PHYSICS_NONE;
		GetSolid() = SOLID_BBOX;
		State.GetModelIndex() = ModelIndex ("models/monsters/commandr/tris.md2");
		GetMins().Set (-32, -32, 0);
		GetMaxs().Set (32, 32, 48);
		CanTakeDamage = true;
		Flags = FL_GODMODE;
		State.GetRenderEffects() |= RF_FRAMELERP;
		Link ();

		SoundIndex ("tank/thud.wav");
		SoundIndex ("tank/pain.wav");

		NextThink = level.Frame + 5;
	};
};

LINK_CLASSNAME_TO_CLASS ("monster_commander_body", CCommanderBody);

/*QUAKED misc_deadsoldier (1 .5 0) (-16 -16 0) (16 16 16) ON_BACK ON_STOMACH BACK_DECAP FETAL_POS SIT_DECAP IMPALED
This is the dead player model. Comes in 6 exciting different poses!
*/
vec3f VelocityForDamage (int damage);
void ClipGibVelocity (CPhysicsEntity *ent);
class CMiscDeadSoldier : public CMapEntity, public CHurtableEntity, public CThinkableEntity, public CTossProjectile
{
public:
	CMiscDeadSoldier () :
	  CBaseEntity (),
	  CMapEntity (),
	  CHurtableEntity (),
	  CThinkableEntity (),
	  CTossProjectile ()
	{
	};

	CMiscDeadSoldier (int Index) :
	  CBaseEntity (Index),
	  CMapEntity (Index),
	  CHurtableEntity (Index),
	  CThinkableEntity (Index),
	  CTossProjectile (Index)
	{
	};

	virtual bool			ParseField (const char *Key, const char *Value)
	{
		return (CHurtableEntity::ParseField (Key, Value) || CMapEntity::ParseField (Key, Value));
	};

	bool Run ()
	{
		switch (PhysicsType)
		{
		case PHYSICS_NONE:
		default:
			return CBaseEntity::Run();
		case PHYSICS_TOSS:
			return CTossProjectile::Run ();
		case PHYSICS_BOUNCE:
			return CBounceProjectile::Run ();
		};
	};

	void DamageEffect (vec3f &dir, vec3f &point, vec3f &normal, int &damage, int &dflags)
	{
		CTempEnt_Splashes::Blood (point, normal);
	}

	void Die (CBaseEntity *inflictor, CBaseEntity *attacker, int damage, vec3f &point)
	{
		if (Health > -80)
			return;

		PlaySound (CHAN_BODY, SoundIndex ("misc/udeath.wav"));
		for (int n = 0; n < 4; n++)
			CGibEntity::Spawn (this, GameMedia.Gib_SmallMeat, damage, GIB_ORGANIC);
		TossHead (GameMedia.Gib_Head[1], damage, GIB_ORGANIC);
	};

	void TossHead (MediaIndex gibIndex, int damage, int type)
	{
		float	vscale;

		State.GetSkinNum() = 0;
		State.GetFrame() = 0;

		GetMins().Clear ();
		GetMaxs().Clear ();

		State.GetModelIndex(2) = 0;
		State.GetModelIndex() = gibIndex;
		GetSolid() = SOLID_NOT;
		State.GetEffects() |= EF_GIB;
		State.GetEffects() &= ~EF_FLIES;
		State.GetSound() = 0;
		Flags |= FL_NO_KNOCKBACK;
		GetSvFlags() &= ~SVF_MONSTER;
		CanTakeDamage = true;

		if (type == GIB_ORGANIC)
		{
			PhysicsType = PHYSICS_TOSS;
			vscale = 0.5;
			backOff = 1.0f;
		}
		else
		{
			PhysicsType = PHYSICS_BOUNCE;
			backOff = 1.5f;
			vscale = 1.0;
		}

		vec3f vd = VelocityForDamage (damage);
		
		vec3f velocity (Velocity);
		velocity.MultiplyAngles (vscale, vd);
		Velocity = velocity;

		ClipGibVelocity (this);

		AngularVelocity.Y = crand()*600;

		NextThink = level.Frame + 100 + frand()*100;

		Link();
	};

	void Think ()
	{
		Free ();
	};

	void Spawn ()
	{
		if (game.mode & GAME_DEATHMATCH)
		{	// auto-remove for deathmatch
			Free ();
			return;
		}

		PhysicsType = PHYSICS_NONE;
		GetSolid() = SOLID_BBOX;
		State.GetModelIndex() = ModelIndex ("models/deadbods/dude/tris.md2");

		// Defaults to frame 0
		if (SpawnFlags & 2)
			State.GetFrame() = 1;
		else if (SpawnFlags & 4)
			State.GetFrame() = 2;
		else if (SpawnFlags & 8)
			State.GetFrame() = 3;
		else if (SpawnFlags & 16)
			State.GetFrame() = 4;
		else if (SpawnFlags & 32)
			State.GetFrame() = 5;
		else
			State.GetFrame() = 0;

		GetMins().Set (-16, -16, 0);
		GetMaxs().Set (16, 16, 16);
		DeadFlag = true;
		CanTakeDamage = true;
		GetSvFlags() |= (SVF_MONSTER|SVF_DEADMONSTER);

		Link ();
	};
};

LINK_CLASSNAME_TO_CLASS ("misc_deadsoldier", CMiscDeadSoldier);

/*QUAKED misc_bigviper (1 .5 0) (-176 -120 -24) (176 120 72) 
This is a large stationary viper as seen in Paul's intro
*/
class CMiscBigViper : public CMapEntity
{
public:
	CMiscBigViper () :
	  CBaseEntity (),
	  CMapEntity ()
	{
	};

	CMiscBigViper (int Index) :
	  CBaseEntity (Index),
	  CMapEntity (Index)
	{
	};

	bool Run ()
	{
		return CBaseEntity::Run();
	};

	void Spawn ()
	{
		GetSolid() = SOLID_BBOX;
		GetMins().Set (-176, -120, -24);
		GetMaxs().Set (176, 120, 72);
		State.GetModelIndex() = ModelIndex ("models/ships/bigviper/tris.md2");
		Link ();
	};
};

LINK_CLASSNAME_TO_CLASS ("misc_bigviper", CMiscBigViper);

/*QUAKED misc_viper_bomb (1 0 0) (-8 -8 -8) (8 8 8)
"dmg"	how much boom should the bomb make?
*/
class CMiscViperBomb : public CMapEntity, public CThinkableEntity, public CTouchableEntity, public CUsableEntity, public CTossProjectile
{
public:
	bool			PreThinkable;
	FrameNumber_t	TimeStamp;
	vec3f			MoveDir;
	int				Damage;

	CMiscViperBomb () :
	  CBaseEntity (),
	  CMapEntity (),
	  CThinkableEntity (),
	  CTouchableEntity (),
	  CUsableEntity (),
	  CTossProjectile (),
	  PreThinkable (false),
	  TimeStamp (0),
	  MoveDir(),
	  Damage(0)
	{
	};

	CMiscViperBomb (int Index) :
	  CBaseEntity (Index),
	  CMapEntity (Index),
	  CThinkableEntity (Index),
	  CTouchableEntity (Index),
	  CUsableEntity (Index),
	  CTossProjectile (Index),
	  PreThinkable (false),
	  TimeStamp (0),
	  MoveDir(),
	  Damage(0)
	{
	};

	ENTITYFIELD_DEFS

	bool Run ()
	{
		return (PhysicsType == PHYSICS_TOSS) ? CTossProjectile::Run() : CBaseEntity::Run();
	};

	void Think ()
	{
	};
	void PreThink ()
	{
		GroundEntity = NULL;

		float diff = TimeStamp - level.Frame;
		if (diff < -1.0)
			diff = -1.0;

		vec3f v = MoveDir * (1.0f + diff);
		v.Z = diff;

		diff = State.GetAngles().Z;

		vec3f angles = v.ToAngles();
		angles.Z = diff + 10;
		State.GetAngles() = angles;
	};

	void Touch (CBaseEntity *other, plane_t *plane, cmBspSurface_t *surf)
	{
		UseTargets (Activator, Message);

		State.GetOrigin().Z = GetAbsMin().Z + 1;
		T_RadiusDamage (this, this, Damage, NULL, Damage+40, MOD_BOMB);
		BecomeExplosion (true);
	};

	void Use (CBaseEntity *other, CBaseEntity *activator)
	{
		if (!Usable)
			return;

		GetSolid() = SOLID_BBOX;
		GetSvFlags() &= ~SVF_NOCLIENT;
		State.GetEffects() |= EF_ROCKET;
		Usable = false;
		PhysicsType = PHYSICS_TOSS;
		PreThinkable = true;
		Touchable = true;
		Activator = activator;

		CMiscViper *viper = entity_cast<CMiscViper>(CC_Find (NULL, FOFS(classname), "misc_viper"));

		Velocity = viper->Dir * viper->Speed;

		TimeStamp = level.Frame;
		MoveDir = viper->Dir;
	};


	void Spawn ()
	{
		PhysicsType = PHYSICS_NONE;
		Touchable = false;
		GetSolid() = SOLID_NOT;
		GetMins().Set (-8);
		GetMaxs().Set (8);

		State.GetModelIndex() = ModelIndex ("models/objects/bomb/tris.md2");

		if (!Damage)
			Damage = 1000;

		GetSvFlags() |= SVF_NOCLIENT;
		Link ();
	};
};

ENTITYFIELDS_BEGIN(CMiscViperBomb)
{
	CEntityField ("dmg", EntityMemberOffset(CMiscViperBomb,Damage), FT_INT),
};
ENTITYFIELDS_END(CMiscViperBomb)

bool			CMiscViperBomb::ParseField (const char *Key, const char *Value)
{
	if (CheckFields<CMiscViperBomb> (this, Key, Value))
		return true;

	// Couldn't find it here
	return (CUsableEntity::ParseField (Key, Value) || CMapEntity::ParseField (Key, Value));
};

LINK_CLASSNAME_TO_CLASS ("misc_viper_bomb", CMiscViperBomb);

/*QUAKED misc_satellite_dish (1 .5 0) (-64 -64 0) (64 64 128)
*/
class CMiscSattelite : public CMapEntity, public CThinkableEntity, public CUsableEntity
{
public:
	CMiscSattelite () :
	  CBaseEntity (),
	  CMapEntity (),
	  CThinkableEntity (),
	  CUsableEntity ()
	{
	};

	CMiscSattelite (int Index) :
	  CBaseEntity (Index),
	  CMapEntity (Index),
	  CThinkableEntity (Index),
	  CUsableEntity (Index)
	{
	};

	virtual bool ParseField (const char *Key, const char *Value)
	{
		return (CUsableEntity::ParseField (Key, Value) || CMapEntity::ParseField (Key, Value));
	}

	bool Run ()
	{
		return CBaseEntity::Run();
	};

	void Think ()
	{
		if (++State.GetFrame() < 38)
			NextThink = level.Frame + FRAMETIME;
	};

	void Use (CBaseEntity *other, CBaseEntity *activator)
	{
		NextThink = level.Frame + FRAMETIME;
	};

	void Spawn ()
	{
		GetSolid() = SOLID_BBOX;
		GetMins().Set (-64, -64, 0);
		GetMaxs().Set (64, 64, 128);
		State.GetModelIndex() = ModelIndex ("models/objects/satellite/tris.md2");
		Link ();
	};
};

LINK_CLASSNAME_TO_CLASS ("misc_satellite_dish", CMiscSattelite);

/*QUAKED light_mine1 (0 1 0) (-2 -2 -12) (2 2 12)
*/
class CLightMine1 : public CMapEntity
{
public:
	CLightMine1 () :
	  CBaseEntity (),
	  CMapEntity ()
	{
	};

	CLightMine1 (int Index) :
	  CBaseEntity (Index),
	  CMapEntity (Index)
	{
	};

	bool Run ()
	{
		return CBaseEntity::Run();
	};

	void Spawn ()
	{
		GetSolid() = SOLID_BBOX;
		State.GetModelIndex() = ModelIndex ("models/objects/minelite/light1/tris.md2");
		Link ();
	};
};

LINK_CLASSNAME_TO_CLASS ("light_mine1", CLightMine1);

/*QUAKED light_mine2 (0 1 0) (-2 -2 -12) (2 2 12)
*/
class CLightMine2 : public CMapEntity
{
public:
	CLightMine2 () :
	  CBaseEntity (),
	  CMapEntity ()
	{
	};

	CLightMine2 (int Index) :
	  CBaseEntity (Index),
	  CMapEntity (Index)
	{
	};

	bool Run ()
	{
		return CBaseEntity::Run();
	};

	void Spawn ()
	{
		GetSolid() = SOLID_BBOX;
		State.GetModelIndex() = ModelIndex ("models/objects/minelite/light2/tris.md2");
		Link ();
	};
};

LINK_CLASSNAME_TO_CLASS ("light_mine2", CLightMine2);

/*QUAKED misc_gib_arm (1 0 0) (-8 -8 -8) (8 8 8)
Intended for use with the target_spawner
*/
class CMiscGibArm : public CMapEntity, public CThinkableEntity, public CTossProjectile, public CHurtableEntity
{
public:
	CMiscGibArm () :
	  CBaseEntity (),
	  CMapEntity (),
	  CThinkableEntity (),
	  CTossProjectile ()
	{
	};

	CMiscGibArm (int Index) :
	  CBaseEntity (Index),
	  CMapEntity (Index),
	  CThinkableEntity (Index),
	  CTossProjectile (Index)
	{
	};

	virtual bool			ParseField (const char *Key, const char *Value)
	{
		return (CHurtableEntity::ParseField (Key, Value) || CMapEntity::ParseField (Key, Value));
	};

	bool Run ()
	{
		return CTossProjectile::Run();
	};

	void Think ()
	{
		Free ();
	};

	void Spawn ()
	{
		State.GetModelIndex() = GameMedia.Gib_Arm();
		GetSolid() = SOLID_NOT;
		State.GetEffects() |= EF_GIB;
		CanTakeDamage = true;
		PhysicsType = PHYSICS_TOSS;
		GetSvFlags() |= SVF_MONSTER;
		AngularVelocity.Set (frand()*200, frand()*200, frand()*200);
		NextThink = level.Frame + 300;
		Link ();
	};
};

LINK_CLASSNAME_TO_CLASS ("misc_gib_arm", CMiscGibArm);

/*QUAKED misc_gib_leg (1 0 0) (-8 -8 -8) (8 8 8)
Intended for use with the target_spawner
*/
class CMiscGibLeg : public CMapEntity, public CThinkableEntity, public CTossProjectile, public CHurtableEntity
{
public:
	CMiscGibLeg () :
	  CBaseEntity (),
	  CMapEntity (),
	  CThinkableEntity (),
	  CTossProjectile ()
	{
	};

	CMiscGibLeg (int Index) :
	  CBaseEntity (Index),
	  CMapEntity (Index),
	  CThinkableEntity (Index),
	  CTossProjectile (Index)
	{
	};

	virtual bool			ParseField (const char *Key, const char *Value)
	{
		return (CHurtableEntity::ParseField (Key, Value) || CMapEntity::ParseField (Key, Value));
	};

	bool Run ()
	{
		return CTossProjectile::Run();
	};

	void Think ()
	{
		Free ();
	};

	void Spawn ()
	{
		State.GetModelIndex() = GameMedia.Gib_Leg();
		GetSolid() = SOLID_NOT;
		State.GetEffects() |= EF_GIB;
		CanTakeDamage = true;
		PhysicsType = PHYSICS_TOSS;
		GetSvFlags() |= SVF_MONSTER;
		AngularVelocity.Set (frand()*200, frand()*200, frand()*200);
		NextThink = level.Frame + 300;
		Link ();
	};
};

LINK_CLASSNAME_TO_CLASS ("misc_gib_leg", CMiscGibLeg);

/*QUAKED misc_gib_head (1 0 0) (-8 -8 -8) (8 8 8)
Intended for use with the target_spawner
*/
class CMiscGibHead : public CMapEntity, public CThinkableEntity, public CTossProjectile, public CHurtableEntity
{
public:
	CMiscGibHead () :
	  CBaseEntity (),
	  CMapEntity (),
	  CThinkableEntity (),
	  CTossProjectile ()
	{
	};

	CMiscGibHead (int Index) :
	  CBaseEntity (Index),
	  CMapEntity (Index),
	  CThinkableEntity (Index),
	  CTossProjectile (Index)
	{
	};

	virtual bool			ParseField (const char *Key, const char *Value)
	{
		return (CHurtableEntity::ParseField (Key, Value) || CMapEntity::ParseField (Key, Value));
	};

	bool Run ()
	{
		return CTossProjectile::Run();
	};

	void Think ()
	{
		Free ();
	};

	void Spawn ()
	{
		State.GetModelIndex() = GameMedia.Gib_Head[0];
		GetSolid() = SOLID_NOT;
		State.GetEffects() |= EF_GIB;
		CanTakeDamage = true;
		PhysicsType = PHYSICS_TOSS;
		GetSvFlags() |= SVF_MONSTER;
		AngularVelocity.Set (frand()*200, frand()*200, frand()*200);
		NextThink = level.Frame + 300;
		Link ();
	};
};

LINK_CLASSNAME_TO_CLASS ("misc_gib_head", CMiscGibHead);

//=================================================================================

class CMiscModel : public CMapEntity
{
public:
	CMiscModel () :
	  CBaseEntity (),
	  CMapEntity ()
	  {
	  };

	CMiscModel (int Index) :
	  CBaseEntity (Index),
	  CMapEntity (Index)
	  {
	  };

	void Spawn ()
	{
		Free ();
	};
};

LINK_CLASSNAME_TO_CLASS ("misc_model", CMiscModel);

