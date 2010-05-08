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
// cc_monsters.h
// Monsters
//

#if !defined(CC_GUARD_MONSTERS_H) || !INCLUDE_GUARDS
#define CC_GUARD_MONSTERS_H

class CMonster;

class CFrame
{
public:

	void (CMonster::*AIFunc) (float Dist);
	void (CMonster::*Function) ();
	float	Dist;

	CFrame ();

	CFrame(void (CMonster::*AIFunc) (float Dist), float Dist, void (CMonster::*Function) () = NULL) :
	AIFunc(AIFunc),
	Dist(Dist),
	Function(Function)
	{
	};
};

class CAnim
{
public:

	sint32			FirstFrame, LastFrame;
	void		(CMonster::*EndFunc) ();

	CFrame	*Frames;

	CAnim (sint32 FirstFrame, sint32 LastFrame, CFrame *Frames, void (CMonster::*EndFunc) () = NULL) :
	FirstFrame(FirstFrame),
	LastFrame(LastFrame),
	EndFunc(EndFunc),
	Frames(Frames)
	{
	};
};

CC_ENUM (uint32, EMonsterAIFlags)
{
	//monster ai flags
	AI_STAND_GROUND			= BIT(0),
	AI_TEMP_STAND_GROUND	= BIT(1),
	AI_SOUND_TARGET			= BIT(2),
	AI_LOST_SIGHT			= BIT(3),
	AI_PURSUIT_LAST_SEEN	= BIT(4),
	AI_PURSUE_NEXT			= BIT(5),
	AI_PURSUE_TEMP			= BIT(6),
	AI_HOLD_FRAME			= BIT(7),
	AI_GOOD_GUY				= BIT(8),
	AI_BRUTAL				= BIT(9),
	AI_NOSTEP				= BIT(10),
	AI_DUCKED				= BIT(11),
	AI_COMBAT_POINT			= BIT(12),
	AI_MEDIC				= BIT(13),
	AI_RESURRECTING			= BIT(14),
#if ROGUE_FEATURES
	//ROGUE
	AI_WALK_WALLS			= BIT(15),
	AI_MANUAL_STEERING		= BIT(16),
	AI_TARGET_ANGER			= BIT(17),
	AI_DODGING				= BIT(18),
	AI_CHARGING				= BIT(19),
	AI_HINT_PATH			= BIT(20),
	AI_IGNORE_SHOTS			= BIT(21),
	// PMM - FIXME - last second added for E3 .. there's probably a better way to do this, but
	// this works
	AI_DO_NOT_COUNT			= BIT(22),	// set for healed monsters
	AI_SPAWNED_CARRIER		= BIT(23),	// both do_not_count and spawned are set for spawned monsters
	AI_SPAWNED_MEDIC_C		= BIT(24),	// both do_not_count and spawned are set for spawned monsters
	AI_SPAWNED_WIDOW		= BIT(25),	// both do_not_count and spawned are set for spawned monsters
	// mask to catch all three flavors of spawned
	AI_SPAWNED_MASK			= (AI_SPAWNED_CARRIER | AI_SPAWNED_MEDIC_C | AI_SPAWNED_WIDOW),
	AI_BLOCKED				= BIT(26),	// used by blocked_checkattack: set to say I'm attacking while blocked 
										// (prevents run-attacks)
#endif
};

CC_ENUM (uint32, EMonsterFlags)
{
	MF_HAS_MELEE			= BIT(0),
	MF_HAS_IDLE				= BIT(1),
	MF_HAS_SEARCH			= BIT(2),
	MF_HAS_SIGHT			= BIT(3),
	MF_HAS_ATTACK			= BIT(4),
#if ROGUE_FEATURES
	MF_HAS_DODGE			= BIT(5),
	MF_HAS_DUCK				= BIT(6),
	MF_HAS_UNDUCK			= BIT(7),
	MF_HAS_SIDESTEP			= BIT(8),
#endif
};

//monster attack state
CC_ENUM (uint8, EAttackState)
{
	AS_STRAIGHT,
	AS_SLIDING,
	AS_MELEE,
	AS_MISSILE,
#if ROGUE_FEATURES
	AS_BLIND
#endif
};

#define MELEE_DISTANCE	80

//range
CC_ENUM (uint8, ERangeType)
{
	RANGE_MELEE,
	RANGE_NEAR,
	RANGE_MID,
	RANGE_FAR
};

enum
{
	MONSTERENTITY_THINK_NONE,
	MONSTERENTITY_THINK_USE,
	MONSTERENTITY_THINK_TRIGGEREDSPAWNUSE,
	MONSTERENTITY_THINK_CUSTOM
};

CC_ENUM (uint8, EMonsterSpawnflags)
{
	MONSTER_AMBUSH			= 	BIT(0),
	MONSTER_TRIGGER_SPAWN	=	BIT(1),
	MONSTER_SIGHT			=	BIT(2),
	MONSTER_DONT_COUNT		=	BIT(3),
};

#define STEPSIZE	18

class CMonsterEntity : public IMapEntity, public IStepPhysics, public ITossProjectile, public IPushPhysics, public IHurtableEntity, public IThinkableEntity, public ITouchableEntity, public IUsableEntity
{
public:
	bool			IsHead;
	uint8			UseState;
	FrameNumber_t	AirFinished;
	FrameNumber_t	DamageDebounceTime;
	FrameNumber_t	BonusDamageTime;
	FrameNumber_t	ShowHostile;
	IBaseEntity		*OldEnemy;
	IBaseEntity		*GoalEntity;
	IBaseEntity		*MoveTarget;
	class CMonster	*Monster;
	char			*DeathTarget;
	char			*CombatTarget;
	CBaseItem		*Item;

	CMonsterEntity	();
	CMonsterEntity	(sint32 Index);

	ENTITYFIELD_DEFS
	ENTITYFIELDS_SAVABLE(CMonsterEntity)

	virtual bool	CheckValidity ();

	void			Think ();

	void			Pain (IBaseEntity *Other, sint32 Damage);
	void			Die (IBaseEntity *Inflictor, IBaseEntity *Attacker, sint32 Damage, vec3f &point);

	virtual void	Touch (IBaseEntity *Other, plane_t *plane, cmBspSurface_t *surf); // Empty
	void			Use (IBaseEntity *Other, IBaseEntity *Activator);
	bool			Blocked (float Dist);

	void			DamageEffect (vec3f &dir, vec3f &point, vec3f &normal, sint32 &damage, sint32 &dflags, EMeansOfDeath &mod);

	bool			Run ();
	void			ThrowHead (MediaIndex gibIndex, sint32 Damage, sint32 type, uint32 effects = EF_GIB);

	void			Spawn ();
};

void AI_SetSightClient ();

class CMonster
{
private:
	CMonster			&operator = (const CMonster &r) { return *this; };

public:
	void				(CMonster::*Think) ();

	const uint32		MonsterID;
	CMonsterEntity		*Entity; // Entity linked to the monster

	float				IdealYaw;
	float				YawSpeed;
	uint32				AIFlags;

#if ROGUE_FEATURES
//ROGUE
	bool				BlindFire;		// will the monster blindfire?

	float				BaseHeight;
	FrameNumber_t		NextDuckTime;
	FrameNumber_t		DuckWaitTime;
	FrameNumber_t		BlindFireDelay;
	CPlayerEntity		*LastPlayerEnemy;
	vec3f				BlindFireTarget;
	CMonsterEntity		*BadMedic1, *BadMedic2;	// these medics have declared this monster "unhealable"
	CMonsterEntity		*Healer;	// this is who is healing this monster
#endif

#if ROGUE_FEATURES
	// used by the spawners to not spawn too much and keep track of #s of monsters spawned
	uint8			MonsterSlots;
	uint8			MonsterUsed;
	CMonsterEntity	*Commander;
	// powerup timers, used by widow, our friend
	FrameNumber_t	QuadFramenum;
	FrameNumber_t	InvincibleFramenum;
	FrameNumber_t	DoubleFramenum;
	class CBadArea	*BadArea;

	// this is for the count of monsters
	inline uint8 SlotsLeft () { return MonsterSlots - MonsterUsed; }
#endif

	sint32				NextFrame;
	float				Scale;
	FrameNumber_t		PauseTime;
	FrameNumber_t		AttackFinished;
	
	FrameNumber_t		SearchTime;
	FrameNumber_t		TrailTime;
	vec3f				LastSighting;
	vec3f				SavedGoal;
	sint32				AttackState;
	bool				Lefty;
	float				IdleTime;
	sint32				LinkCount;

	EPowerArmorType		PowerArmorType;
	sint32				PowerArmorPower;
	uint8				PowerArmorTime;
	uint8				ExplodeCount;
	bool				EnemyInfront, EnemyVis;

	ERangeType			EnemyRange;
	float				EnemyYaw;

	CAnim				*CurrentMove;

	uint32				MonsterFlags;
	std::string			MonsterName;

	FrameNumber_t		PainDebounceTime;

	CMonster(uint32 ID);

	void SaveFields (CFile &File);
	void LoadFields (CFile &File);

#define MONSTER_SOUND_ENUM(first,...) \
	CC_ENUM(uint8, EMyMonsterSoundEnum) \
	{ \
		first, \
		__VA_ARGS__ \
	}; \
	MediaIndex	Sounds[SOUND_MAX];

#define SAVE_MONSTER_SOUNDS \
	for (uint32 i = 0; i < SOUND_MAX; i++) \
		WriteIndex (File, Sounds[i], INDEX_SOUND);

#define LOAD_MONSTER_SOUNDS \
	for (uint32 i = 0; i < SOUND_MAX; i++) \
		ReadIndex (File, Sounds[i], INDEX_SOUND);

#define MONSTER_SAVE_LOAD_NO_FIELDS \
	void SaveMonsterFields (CFile &File) \
	{ \
		SAVE_MONSTER_SOUNDS \
	} \
	void LoadMonsterFields (CFile &File) \
	{ \
		LOAD_MONSTER_SOUNDS \
	}

	virtual void SaveMonsterFields (CFile &File) {};
	virtual void LoadMonsterFields (CFile &File) {};

	virtual void		Touch (IBaseEntity *Other, plane_t *plane, cmBspSurface_t *surf) {}; // Empty

	// Virtual functions
	virtual void		Stand			();
	virtual void		Idle			();
	virtual void		Search			();
	virtual void		Walk			();
	virtual void		Run				();
	virtual void		Dodge			(IBaseEntity *Other, float eta
#if ROGUE_FEATURES
		, CTrace *tr
#endif
		);
	virtual void		Attack			();
	virtual void		Melee			();
	virtual void		Sight			();
	virtual bool		CheckAttack		();

	virtual void		ReactToDamage	(IBaseEntity *Attacker, IBaseEntity *Inflictor);

	virtual void		MonsterThink	();
	virtual void		DamageEffect (vec3f &dir, vec3f &point, vec3f &normal, sint32 &damage, sint32 &dflags, EMeansOfDeath &mod);

	virtual bool		Blocked (float Dist) {return false;}

	void				AI_Charge (float Dist);
	void				AI_Move (float Dist);
	void				AI_Stand (float Dist);
	void				AI_Run (float Dist);
	void				AI_Run_Melee ();
	void				AI_Run_Missile ();
	void				AI_Run_Slide (float Dist);
	void				AI_Walk (float Dist);
	
	bool				FindTarget ();
	void				FoundTarget ();
	void				HuntTarget ();
	void				AlertNearbyStroggs ();

	void				FixInvalidEntities ();

	void				BossExplode ();
	void				MoveFrame ();

	bool				AI_CheckAttack ();
	bool				FacingIdeal ();

	void				CatagorizePosition ();
	void				CheckGround ();

	void				DropToFloor ();

	void				FliesOff ();
	void				FliesOn ();
	void				CheckFlies ();

	void				SetEffects ();
	void				WorldEffects ();

	void				MonsterDeathUse ();
	virtual void		Use (IBaseEntity *Other, IBaseEntity *Activator) {};

	void				MonsterFireBfg (vec3f start, vec3f aimdir, sint32 Damage, sint32 speed, sint32 kick, float damage_radius, sint32 flashtype);
	void				MonsterFireBlaster (vec3f start, vec3f dir, sint32 Damage, sint32 speed, sint32 flashtype, sint32 effect);
#if ROGUE_FEATURES
	void				MonsterFireBlaster2 (vec3f start, vec3f dir, sint32 Damage, sint32 speed, sint32 flashtype, sint32 effect);
	void				MonsterFireTracker (vec3f start, vec3f dir, int damage, int speed, IBaseEntity *enemy, int flashtype);
	void				MonsterFireHeat (vec3f start, vec3f dir, int damage, int kick, int flashtype);
#endif
	void				MonsterFireGrenade (vec3f start, vec3f aimdir, sint32 Damage, sint32 speed, sint32 flashtype);
	void				MonsterFireRailgun (vec3f start, vec3f aimdir, sint32 Damage, sint32 kick, sint32 flashtype);
	void				MonsterFireShotgun (vec3f start, vec3f aimdir, sint32 Damage, sint32 kick, sint32 hspread, sint32 vspread, sint32 count, sint32 flashtype);
	void				MonsterFireBullet (vec3f start, vec3f dir, sint32 Damage, sint32 kick, sint32 hspread, sint32 vspread, sint32 flashtype);
	void				MonsterFireRocket (vec3f start, vec3f dir, sint32 Damage, sint32 speed, sint32 flashtype);

#if XATRIX_FEATURES
	void				MonsterFireRipper (vec3f start, vec3f dir, sint32 Damage, sint32 speed, sint32 flashtype);
	void				MonsterFireBeam (class CMonsterBeamLaser *Laser);
	void				MonsterFireBlueBlaster (vec3f start, vec3f dir, sint32 Damage, sint32 speed, sint32 flashtype);
	void				MonsterFireHeatRocket (vec3f start, vec3f dir, sint32 Damage, sint32 speed, sint32 flashtype);
#endif

#if ROGUE_FEATURES
	void				CleanupHealTarget ();
	void				DoneDodge ();
	void				MonsterDodge (IBaseEntity *Attacker, float eta, CTrace *tr);
	virtual void		DuckUp ();
	virtual void		DuckHold ();
	virtual void		DuckDown ();

	virtual void		SideStep () {};
	virtual void		UnDuck () {};
	virtual void		Duck (float eta) {};

	class CBadArea		*CheckForBadArea ();
	bool				MarkTeslaArea (class CTesla *Tesla);
	void				TargetTesla (class CTesla *Tesla);
	bool				IsBadAhead (CBadArea *bad, vec3f move);
#endif

	void				MonsterTriggeredSpawn ();
	void				MonsterTriggeredStart ();

	void				MonsterStart ();
	void				MonsterStartGo ();

	void				FlyMonsterStart ();
	void				FlyMonsterStartGo ();
	void				SwimMonsterStart ();
	void				SwimMonsterStartGo ();
	void				WalkMonsterStart ();
	void				WalkMonsterStartGo ();

	void				StationaryMonsterStart ();
	void				StationaryMonsterStartGo ();

	void				ChangeYaw ();
	bool				CheckBottom ();
	void				MoveToGoal (float Dist);
	bool				WalkMove (float Yaw, float Dist);
	bool				CloseEnough (IBaseEntity *Goal, float Dist);
	void				NewChaseDir (IBaseEntity *Enemy, float Dist);
	bool				StepDirection (float Yaw, float Dist);
	bool				MoveStep (vec3f move, bool ReLink);

	virtual void		Spawn () = 0;
	virtual void		Die(IBaseEntity *Inflictor, IBaseEntity *Attacker, sint32 Damage, vec3f &point) = 0;
	virtual void		Pain(IBaseEntity *Other, sint32 Damage) = 0;

	inline bool HasValidEnemy ()
	{
		if (!Entity->Enemy)
			return false;
		if (!Entity->Enemy->GetInUse() || Entity->Enemy->Freed)
			return false;

		return true;
	}
};

#if XATRIX_FEATURES
class CMonsterBeamLaser : public IThinkableEntity
{
public:
	vec3f		MoveDir;
	bool		DoFree;
	bool		MakeEffect;
	sint32		Damage;

	CMonsterBeamLaser ();

	IMPLEMENT_SAVE_HEADER (CMonsterBeamLaser)

	void SaveFields (CFile &File)
	{
		File.Write<sint32> (Damage);
		File.Write<bool> (MakeEffect);
		File.Write<bool> (DoFree);
		File.Write<vec3f> (MoveDir);

		IThinkableEntity::SaveFields (File);
	}

	void LoadFields (CFile &File)
	{
		Damage = File.Read <sint32>();
		MakeEffect = File.Read <bool>();
		DoFree = File.Read <bool>();
		MoveDir = File.Read <vec3f>();

		IThinkableEntity::LoadFields (File);
	}

	void Think ();
};
#endif

#define DI_NODIR	-1

class CMonsterTableIndex
{
public:
	const char *Name;
	CMonster *(*FuncPtr) (uint32 index);

	CMonsterTableIndex (const char *Name, CMonster *(*FuncPtr) (uint32 index));

	CMonster *Create (uint32 number)
	{
		return FuncPtr (number);
	};
};

#define ConvertDerivedFunction(x) static_cast<void (CMonster::* )()>(x)
#define ConvertDerivedAIMove(x) static_cast<void (CMonster::* )(float)>(x)

extern uint32 LastID;
#define LINK_MONSTER_CLASSNAME_TO_CLASS(mapClassName,DLLClassName) \
	uint32 LINK_RESOLVE_CLASSNAME(DLLClassName, _ID) = LastID++; \
	const uint32 DLLClassName::ID = LINK_RESOLVE_CLASSNAME(DLLClassName, _ID); \
	IMapEntity *LINK_RESOLVE_CLASSNAME(DLLClassName, _Spawn) (sint32 Index) \
	{ \
		CMonsterEntity *newClass = QNewEntityOf CMonsterEntity(Index); \
		DLLClassName *Monster = QNewEntityOf DLLClassName (LINK_RESOLVE_CLASSNAME(DLLClassName, _ID)); \
		newClass->Monster = Monster; \
		Monster->Entity = newClass; \
		\
		newClass->ParseFields (); \
		\
		if (newClass->CheckValidity()) \
		{	\
			Monster->Spawn (); \
			newClass->NextThink = Level.Frame + 1; \
		}	\
		return newClass; \
	} \
	CClassnameToClassIndex LINK_RESOLVE_CLASSNAME(DLLClassName, _Linker) \
	(LINK_RESOLVE_CLASSNAME(DLLClassName, _Spawn), mapClassName); \
	CMonster *LINK_RESOLVE_CLASSNAME(DLLClassName, _Resolver) (uint32 ID) \
	{ \
		return QNewEntityOf DLLClassName(ID); \
	} \
	CMonsterTableIndex LINK_RESOLVE_CLASSNAME(DLLClassName, _ResolveIndex) (mapClassName, LINK_RESOLVE_CLASSNAME(DLLClassName, _Resolver));

#define MONSTER_ID_HEADER \
	static const uint32 ID;

#include "cc_player_trail.h"

#if ROGUE_FEATURES
void InitBadAreas ();
void SaveBadAreas (CFile &File);
void LoadBadAreas (CFile &File);
void RunBadAreas ();
void SaveBadArea (CFile &File, CBadArea *Area);
CBadArea *LoadBadArea (CFile &File);
#endif

#else
FILE_WARNING
#endif
