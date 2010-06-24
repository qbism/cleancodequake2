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
// cc_entitytypes.h
// Different base entity classes
//

#if !defined(CC_GUARD_ENTITYTYPES_H) || !INCLUDE_GUARDS
#define CC_GUARD_ENTITYTYPES_H

/**
\typedef	uint32 EMeansOfDeath

\brief	Defines an alias representing a means of death.
**/
typedef uint32 EMeansOfDeath;

/**
\enum	

\brief	Values that represent a means of death. 
**/
enum
{
	MOD_UNKNOWN,
	MOD_BLASTER,
	MOD_SHOTGUN,
	MOD_SSHOTGUN,
	MOD_MACHINEGUN,
	MOD_CHAINGUN,
	MOD_GRENADE,
	MOD_G_SPLASH,
	MOD_ROCKET,
	MOD_R_SPLASH,
	MOD_HYPERBLASTER,
	MOD_RAILGUN,
	MOD_BFG_LASER,
	MOD_BFG_BLAST,
	MOD_BFG_EFFECT,
	MOD_HANDGRENADE,
	MOD_HG_SPLASH,
	MOD_WATER,
	MOD_SLIME,
	MOD_LAVA,
	MOD_CRUSH,
	MOD_TELEFRAG,
	MOD_FALLING,
	MOD_SUICIDE,
	MOD_HELD_GRENADE,
	MOD_EXPLOSIVE,
	MOD_BARREL,
	MOD_BOMB,
	MOD_EXIT,
	MOD_SPLASH,
	MOD_TARGET_LASER,
	MOD_TRIGGER_HURT,
	MOD_HIT,
	MOD_TARGET_BLASTER,

#if CLEANCTF_ENABLED
	MOD_GRAPPLE,
#endif

#if XATRIX_FEATURES
	MOD_RIPPER,
	MOD_TRAP,
#endif

#if ROGUE_FEATURES
	MOD_CHAINFIST,
	MOD_DISINTEGRATOR,
	MOD_ETF_RIFLE,
	MOD_BLASTER2,
	MOD_HEATBEAM,
	MOD_TESLA,
	MOD_PROX,
	MOD_NUKE,
	MOD_VENGEANCE_SPHERE,
	MOD_HUNTER_SPHERE,
	MOD_DEFENDER_SPHERE,
	MOD_TRACKER,
	MOD_DOPPLE_EXPLODE,
	MOD_DOPPLE_VENGEANCE,
	MOD_DOPPLE_HUNTER,
#endif

	MOD_FRIENDLY_FIRE		=	512
};

// "Hurtable" entity
class IHurtableEntity : public virtual IBaseEntity
{
public:
	sint32			Health;
	sint32			MaxHealth;
	sint32			GibHealth;
	bool			DeadFlag;
	bool			CanTakeDamage;
	bool			AffectedByKnockback;

	ENTITYFIELD_VIRTUAL_DEFS
	ENTITYFIELDS_SAVABLE_VIRTUAL(IHurtableEntity)

	IHurtableEntity ();
	IHurtableEntity (sint32 index);

	virtual void Pain (IBaseEntity *Other, sint32 Damage) {};
	virtual void Die (IBaseEntity *Inflictor, IBaseEntity *Attacker, sint32 Damage, vec3f &Point) {};

	bool DamageCanReach (IBaseEntity *Inflictor);
	bool CheckTeamDamage (IBaseEntity *Attacker);
	sint32 CheckPowerArmor (vec3f &Point, vec3f &Normal, sint32 Damage, EDamageFlags dflags);

	virtual void Killed (IBaseEntity *Inflictor, IBaseEntity *Attacker, sint32 Damage, vec3f &Point);

	// An "extension" of sorts to TakeDamage
	// which handles the effects when we are hurt
	virtual void DamageEffect (vec3f &Dir, vec3f &Point, vec3f &Normal, sint32 &Damage, EDamageFlags &DamageFlags, EMeansOfDeath &MeansOfDeath);

	// Takes damage.
	// For this, "this" is target. Use this if the
	// entity can be casted to IHurtableEntity
	// without question.
	virtual void TakeDamage (	IBaseEntity *Inflictor, IBaseEntity *Attacker,
							vec3f dir, vec3f point, vec3f normal, sint32 Damage,
							sint32 knockback, EDamageFlags dflags, EMeansOfDeath mod);
	
	// This is a convenient static version.
	// This will cast targ to IHurtableEntity
	// and make the necessary damage adjustments if possible.
	// If "targ" can't take damage, nothing will happen.
	static void TakeDamage (	IBaseEntity *targ, IBaseEntity *Inflictor,
							IBaseEntity *Attacker, vec3f dir, vec3f point,
							vec3f normal, sint32 Damage, sint32 knockback,
							EDamageFlags dflags, EMeansOfDeath mod);
};

class IBlockableEntity : public virtual IBaseEntity
{
public:
	IBlockableEntity ();
	IBlockableEntity (sint32 Index);

	virtual void Blocked (IBaseEntity *Other) = 0;
};

class IUsableEntity : public virtual IBaseEntity
{
public:
	std::string					Message;
	char						*Target;
	char						*KillTarget;
	char						*PathTarget;
	entity_ptr<IBaseEntity>		User;
	FrameNumber					Delay;
	MediaIndex					NoiseIndex;
	bool						Usable;

	ENTITYFIELD_VIRTUAL_DEFS
	ENTITYFIELDS_SAVABLE_VIRTUAL(IUsableEntity)

	IUsableEntity ();
	IUsableEntity (sint32 Index);

	virtual void Use (IBaseEntity *Other, IBaseEntity *Activator) {};
	virtual void UseTargets (IBaseEntity *Activator, std::string &Message);
};

// Thinkable entity
// Simple think function
class IThinkableEntity : public virtual IBaseEntity
{
public:
	FrameNumber			NextThink;

	IThinkableEntity ();
	IThinkableEntity (sint32 index);

	ENTITYFIELDS_SAVABLE(IThinkableEntity)

	void			RunThink ();
	virtual void	Think () = 0;
	virtual void	PreThink () {};
};

// Touchable entity
class ITouchableEntity : public virtual IBaseEntity
{
public:
	bool				Touchable; // Setting to false is equivilent to putting touch = NULL in original Q2

	ENTITYFIELDS_SAVABLE(ITouchableEntity)

	ITouchableEntity ();
	ITouchableEntity (sint32 index);

	virtual void	Touch (IBaseEntity *Other, SBSPPlane *plane, SBSPSurface *surf);
};

/**
\typedef	uint8 EPhysicsType

\brief	Defines an alias representing type of physics this entity is currently using.
**/
typedef uint8 EPhysicsType;

/**
\enum	

\brief	Values that represent physics types. 
**/
enum
{
	PHYSICS_NONE,

	PHYSICS_NOCLIP,
	PHYSICS_PUSH,
	PHYSICS_STOP,
	PHYSICS_WALK,
	PHYSICS_STEP,
	PHYSICS_FLY,
	PHYSICS_TOSS,
	PHYSICS_FLYMISSILE,
	PHYSICS_BOUNCE
};

/**
\typedef	uint32 EPhysicsFlags

\brief	Defines an alias representing physics flags.
**/
typedef uint32 EPhysicsFlags;

/**
\enum	

\brief	Values that represent physics flags; extra features that physics objects may have.
		Note that these cannot be set directly. You must inherit the proper classes.
**/
enum
{
	// Class flags
	PHYSICFLAG_BUOYANCY			=	BIT(0), // Has IBuoyancyPhysics
	PHYSICFLAG_RESISTANCE		=	BIT(1), // Has IResistancePhysics
	PHYSICFLAG_AERODYNAMICS		=	BIT(2), // Has IAerodynamicPhysics

	// Other flags
	PHYSICFLAG_FLOATING			=	BIT(16), // Is "floating"
};

// Contains common code that projectiles will use
class IPhysicsEntity : public virtual IBaseEntity
{
public:
	float				GravityMultiplier;	// per entity gravity multiplier (1.0 is normal)
											// use for lowgrav artifact, flares
	EPhysicsType		PhysicsType;
	EPhysicsFlags		PhysicsFlags;
	vec3f				AngularVelocity;
	vec3f				Velocity;
	vec3f				GravityVector;
	float				Mass;
	struct SWaterInfo
	{
		EBrushContents		Type;
		EWaterLevel			Level,
							OldLevel;
	} WaterInfo;

	bool				PhysicsDisabled;
	vec3f				VelocityEffect, DampeningEffect, AVelocityEffect, ADampeningEffect;

	IPhysicsEntity ();
	IPhysicsEntity (sint32 index);

	virtual void SaveFields (CFile &File)
	{
		File.Write<float> (GravityMultiplier);
		File.Write<EPhysicsType> (PhysicsType);
		File.Write<vec3f> (&AngularVelocity);
		File.Write<vec3f> (&Velocity);
		File.Write<vec3f> (&GravityVector);
		File.Write<float> (&Mass);
		File.Write<SWaterInfo> (WaterInfo);
		File.Write<bool> (PhysicsDisabled);
	};

	virtual void LoadFields (CFile &File)
	{
		GravityMultiplier = File.Read<float> ();
		PhysicsType = File.Read<EPhysicsType> ();
		AngularVelocity = File.Read<vec3f> ();
		Velocity = File.Read<vec3f> ();
		GravityVector = File.Read<vec3f> ();
		Mass = File.Read<float> ();
		WaterInfo = File.Read<SWaterInfo> ();
		PhysicsDisabled = File.Read<bool> ();
	};

	class CTrace	PushEntity (vec3f &push);
	void			AddGravity ();
	void			Impact (CTrace *trace);
	virtual void	PushInDirection (vec3f vel, uint32 flags);
};

// "Bouncy" projectile
class IBounceProjectile : public virtual IPhysicsEntity
{
public:
	float			backOff;
	bool			AffectedByGravity;
	bool			StopOnEqualPlane;
	bool			AimInVelocityDirection;

	IBounceProjectile ();
	IBounceProjectile (sint32 index);

	virtual void SaveFields (CFile &File)
	{
		File.Write<float> (backOff);
		IPhysicsEntity::SaveFields (File);
	}

	virtual void LoadFields (CFile &File)
	{
		backOff = File.Read<float> ();
		IPhysicsEntity::LoadFields (File);
	}

	bool			Run ();
};

// Same as bouncy, except doesn't bounce
class ITossProjectile : public virtual IBounceProjectile
{
public:
	ITossProjectile();
	ITossProjectile (sint32 index);

	virtual void SaveFields (CFile &File)
	{
		IBounceProjectile::SaveFields (File);
	}

	virtual void LoadFields (CFile &File)
	{
		IBounceProjectile::LoadFields (File);
	}
};

// Doesn't add gravity
class IFlyMissileProjectile : public virtual IBounceProjectile
{
public:
	IFlyMissileProjectile ();
	IFlyMissileProjectile (sint32 index);

	virtual void SaveFields (CFile &File)
	{
		IPhysicsEntity::SaveFields (File);
	}

	virtual void LoadFields (CFile &File)
	{
		IPhysicsEntity::LoadFields (File);
	}
};

// Gravity, special edge handling
class IStepPhysics : public virtual IPhysicsEntity
{
public:
	IStepPhysics ();
	IStepPhysics (sint32 index);

	virtual void	CheckGround ();

	virtual void SaveFields (CFile &File)
	{
		IPhysicsEntity::SaveFields (File);
	}

	virtual void LoadFields (CFile &File)
	{
		IPhysicsEntity::LoadFields (File);
	}

	sint32			FlyMove (float time, sint32 mask);
	void			AddRotationalFriction ();
	bool			Run ();
};

class IPushPhysics : public virtual IPhysicsEntity
{
public:
	IPushPhysics ();
	IPushPhysics (sint32 Index);

	virtual void SaveFields (CFile &File)
	{
		IPhysicsEntity::SaveFields (File);
	}

	virtual void LoadFields (CFile &File)
	{
		IPhysicsEntity::LoadFields (File);
	}

	bool			Run ();
};

class IStopPhysics : public virtual IPushPhysics
{
public:
	IStopPhysics ();
	IStopPhysics (sint32 Index);

	virtual void SaveFields (CFile &File)
	{
		IPhysicsEntity::SaveFields (File);
	}

	virtual void LoadFields (CFile &File)
	{
		IPhysicsEntity::LoadFields (File);
	}

	bool			Run ();
};

// Buoyancy physics.
// Only works for: Bounce, Toss (via Bounce)
class IBuoyancyPhysics : public virtual IPhysicsEntity
{
public:
	float		BuoyancyPowerWater, BuoyancyPowerAir;

	IBuoyancyPhysics ();
	IBuoyancyPhysics (sint32 Index);

	void RunBouyancy ();
	void SetBuoyancy (float Power, float ModAir = 0.0f, float ModWater = 1.0f);
};

// Resistance physics.
// Only works for: Bounce, Toss (via Bounce)
class IResistancePhysics : public virtual IPhysicsEntity
{
public:
	float		ResistPowerWater, ResistPowerAir;

	IResistancePhysics ();
	IResistancePhysics (sint32 Index);

	void RunResistance ();
	void SetResistance (float Power, float ModAir = 0.0f, float ModWater = 1.0f);
};

// Aerodynamics
class IAerodynamicPhysics : public virtual IPhysicsEntity
{
public:
	float		AeroPower;

	IAerodynamicPhysics ();
	IAerodynamicPhysics (sint32 Index);

	void RunAerodynamics ();
	void SetAerodynamics (float Power);
};

#else
FILE_WARNING
#endif
