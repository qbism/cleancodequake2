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

#if !defined(__CC_MONSTERS_H__) || !defined(INCLUDE_GUARDS)
#define __CC_MONSTERS_H__

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

	int			FirstFrame, LastFrame;
	void		(CMonster::*EndFunc) ();

	CFrame	*Frames;

	CAnim (int FirstFrame, int LastFrame, CFrame *Frames, void (CMonster::*EndFunc) () = NULL) :
	FirstFrame(FirstFrame),
	LastFrame(LastFrame),
	EndFunc(EndFunc),
	Frames(Frames)
	{
	};
};

CC_ENUM (uint32, EMonsterAIFlags)
{
#ifndef MONSTER_USE_ROGUE_AI
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
	AI_SLIDE				= BIT(15),
#else
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
#ifdef MONSTER_USE_ROGUE_AI
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
#ifdef MONSTER_USE_ROGUE_AI
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
	MONSTERENTITY_THINK_TRIGGEREDSPAWNUSE
};

class CMonsterEntity : public CMapEntity, public CStepPhysics, public CTossProjectile, public CPushPhysics, public CHurtableEntity, public CThinkableEntity, public CTouchableEntity, public CUsableEntity
{
public:
	bool			IsHead;
	uint8			UseState;
	FrameNumber_t	AirFinished;
	FrameNumber_t	DamageDebounceTime;
	FrameNumber_t	BonusDamageTime;
	CBaseEntity		*OldEnemy;
	CBaseEntity		*GoalEntity;
	CBaseEntity		*MoveTarget;
	class CMonster	*Monster;
	char			*DeathTarget;
	char			*CombatTarget;

	CMonsterEntity	();
	CMonsterEntity	(int Index);

	ENTITYFIELD_DEFS

	virtual bool			CheckValidity ();

	void			Think ();

	void Pain (CBaseEntity *other, float kick, int damage);
	void Die (CBaseEntity *inflictor, CBaseEntity *attacker, int damage, vec3f &point);

	virtual void	Touch (CBaseEntity *other, plane_t *plane, cmBspSurface_t *surf); // Empty
	void	Use (CBaseEntity *other, CBaseEntity *activator);

	bool			Run ();
	void			ThrowHead (MediaIndex gibIndex, int damage, int type);

	void Spawn ();
};

class CMonster
{
private:
	CMonster			&operator = (const CMonster &r) { return *this; };
protected:
public:
	void				(CMonster::*Think) ();

	// Hash
	uint32				HashValue;
	CMonster			*HashNext;

	const uint32		MonsterID;
	CMonsterEntity		*Entity; // Entity linked to the monster

	float				IdealYaw;
	float				YawSpeed;
	uint32				AIFlags;

#ifdef MONSTER_USE_ROGUE_AI
//ROGUE
	bool		BlindFire;		// will the monster blindfire?
	int			MedicTries;

	//  while abort_duck would be nice, only monsters which duck but don't sidestep would use it .. only the brain
	//  not really worth it.  sidestep is an implied abort_duck
//	void		(*abort_duck)(edict_t *self);
	float		BaseHeight;
	FrameNumber_t		NextDuckTime;
	FrameNumber_t		DuckWaitTime;
	FrameNumber_t		BlindFireDelay;
	CPlayerEntity		*LastPlayerEnemy;
	vec3f				BlindFireTarget;
	// blindfire stuff .. the boolean says whether the monster will do it, and blind_fire_time is the timing
	// (set in the monster) of the next shot
	CMonsterEntity		*BadMedic1, *BadMedic2;	// these medics have declared this monster "unhealable"
#endif
	CMonsterEntity		*Healer;	// this is who is healing this monster

	int					NextFrame;
	float				Scale;
	FrameNumber_t				PauseTime;
	FrameNumber_t				AttackFinished;
	
	FrameNumber_t		SearchTime;
	FrameNumber_t		TrailTime;
	vec3f				LastSighting;
	vec3f				SavedGoal;
	int					AttackState;
	bool				Lefty;
	float				IdleTime;
	int					LinkCount;

	int					PowerArmorType;
	int					PowerArmorPower;

	bool				EnemyInfront, EnemyVis;
	ERangeType			EnemyRange;
	float				EnemyYaw;

	CAnim				*CurrentMove;

	uint32				MonsterFlags;
	char				*MonsterName;

	FrameNumber_t		PainDebounceTime;

#ifdef MONSTERS_USE_PATHFINDING
	// Pathfinding
	class CPath				*P_CurrentPath;
	class CPathNode			*P_CurrentGoalNode;
	class CPathNode			*P_CurrentNode; // Always the current path node
	int32				P_CurrentNodeIndex;
	FrameNumber_t				P_NodePathTimeout;
	FrameNumber_t				P_NodeFollowTimeout;
	bool				FollowingPath;

	// Pathfinding functions
	void	FoundPath		(); // Give it current and goal node and you can do this.
	void	MoveToPath		(float Dist);
#endif

	CMonster(uint32 ID);

	virtual void	Touch (CBaseEntity *other, plane_t *plane, cmBspSurface_t *surf) {}; // Empty

#ifdef MONSTER_USE_ROGUE_AI
	void DuckDown ();
	virtual void Duck (float eta);
	virtual void UnDuck ();
	virtual void DuckHold ();
	virtual void SideStep ();
#endif

	// Virtual functions
	virtual void		Stand			();
	virtual void		Idle			();
	virtual void		Search			();
	virtual void		Walk			();
	virtual void		Run				();
#ifndef MONSTER_USE_ROGUE_AI
	virtual void		Dodge			(CBaseEntity *other, float eta);
#else
	virtual void		Dodge			(CBaseEntity *attacker, float eta, CTrace *tr);
	void		DoneDodge	();
#endif
	virtual void		Attack			();
	virtual void		Melee			();
	virtual void		Sight			();
	virtual bool		CheckAttack		();

	virtual void		ReactToDamage	(CBaseEntity *attacker);

	virtual void		MonsterThink	();

	void AI_Charge (float Dist);
	void AI_Move (float Dist);
	void AI_Stand (float Dist);
	void AI_Run (float Dist);
	void AI_Run_Melee ();
	void AI_Run_Missile ();
	void AI_Run_Slide (float Dist);
	void AI_Walk (float Dist);
	bool FacingIdeal ();
	bool FindTarget ();
	void FoundTarget ();
	void HuntTarget ();

	void MoveFrame ();

	bool AI_CheckAttack ();

	void CatagorizePosition ();
	void CheckGround ();

	void DropToFloor ();

	void FliesOff ();
	void FliesOn ();
	void CheckFlies ();

	void SetEffects ();
	void WorldEffects ();

	void MonsterDeathUse ();

	void MonsterFireBfg (vec3f start, vec3f aimdir, int damage, int speed, int kick, float damage_radius, int flashtype);
	void MonsterFireBlaster (vec3f start, vec3f dir, int damage, int speed, int flashtype, int effect);
	void MonsterFireGrenade (vec3f start, vec3f aimdir, int damage, int speed, int flashtype);
	void MonsterFireRailgun (vec3f start, vec3f aimdir, int damage, int kick, int flashtype);
	void MonsterFireShotgun (vec3f start, vec3f aimdir, int damage, int kick, int hspread, int vspread, int count, int flashtype);
	void MonsterFireBullet (vec3f start, vec3f dir, int damage, int kick, int hspread, int vspread, int flashtype);
	void MonsterFireRocket (vec3f start, vec3f dir, int damage, int speed, int flashtype);

	void AlertNearbyStroggs ();
#ifdef MONSTERS_ARENT_STUPID
	bool FriendlyInLine (vec3f &Origin, vec3f &Direction);
#endif

	void MonsterTriggeredSpawn ();
	static void _cdecl MonsterTriggeredSpawnUse (edict_t *self, edict_t *other, edict_t *activator);
	static void _cdecl MonsterUse (edict_t *self, edict_t *other, edict_t *activator);
	void MonsterTriggeredStart ();

	void MonsterStart();
	void MonsterStartGo();

	void FlyMonsterStart ();
	void FlyMonsterStartGo ();
	void SwimMonsterStart ();
	void SwimMonsterStartGo ();
	void WalkMonsterStart ();
	void WalkMonsterStartGo ();

	void ChangeYaw ();
	bool CheckBottom ();
	void MoveToGoal (float Dist);
	bool WalkMove (float Yaw, float Dist);
	bool CloseEnough (CBaseEntity *Goal, float Dist);
	void NewChaseDir (CBaseEntity *Enemy, float Dist);
	bool StepDirection (float Yaw, float Dist);
	bool MoveStep (vec3f move, bool ReLink);

	virtual void	Spawn () = 0;
	virtual void	Die(CBaseEntity *inflictor, CBaseEntity *attacker, int damage, vec3f &point) = 0;
	virtual void	Pain(CBaseEntity *other, float kick, int damage) = 0;
};

#define DI_NODIR	-1

void Monster_Think (edict_t *ent);

#define ConvertDerivedFunction(x) static_cast<void (__thiscall CMonster::* )(void)>(x)
#define ConvertDerivedAIMove(x) static_cast<void (__thiscall CMonster::* )(float)>(x)

extern uint32 LastID;
#define LINK_MONSTER_CLASSNAME_TO_CLASS(mapClassName,DLLClassName) \
	uint32 LINK_RESOLVE_CLASSNAME(DLLClassName, _ID) = LastID++; \
	CMapEntity *LINK_RESOLVE_CLASSNAME(DLLClassName, _Spawn) (int Index) \
	{ \
		CMonsterEntity *newClass = QNew (com_levelPool, 0) CMonsterEntity(Index); \
		DLLClassName *Monster = QNew (com_levelPool, 0) DLLClassName (LINK_RESOLVE_CLASSNAME(DLLClassName, _ID)); \
		newClass->Monster = Monster; \
		Monster->Entity = newClass; \
		\
		newClass->ParseFields (); \
		\
		if (newClass->CheckValidity()) \
		{	\
			Monster->Spawn (); \
			newClass->NextThink = level.Frame + 1; \
		}	\
		return newClass; \
	} \
	CClassnameToClassIndex LINK_RESOLVE_CLASSNAME(DLLClassName, _Linker) \
	(LINK_RESOLVE_CLASSNAME(DLLClassName, _Spawn), mapClassName);

#else
FILE_WARNING
#endif