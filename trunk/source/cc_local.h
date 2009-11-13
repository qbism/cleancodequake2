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
// cc_local.h
// Local header. Definitions local to the game.
//

#if !defined(__CC_LOCAL_H__) || !defined(INCLUDE_GUARDS)
#define __CC_LOCAL_H__

#include "cc_options.h"
#include "../shared/shared.h"

#if 0
#include "Newton.h"
#endif

// Paril, CleanCode Quake2
#include "cc_shared.h"

// the "gameversion" client command will print this plus compile date
#define GAMENAME	"baseq2"

//==================================================================

// view pitching times
#define DAMAGE_TIME		5
#define FALL_TIME		3

#define FRAMETIME		1

// memory tags to allow dynamic memory to be cleaned up
#define TAG_GAME	765		// clear when unloading the dll
#define TAG_LEVEL	766		// clear when loading a new level

//gib types
CC_ENUM (uint8, EGibType)
{
	GIB_ORGANIC,
	GIB_METALLIC
};

// handedness values
CC_ENUM (uint8, EHandedness)
{
	RIGHT_HANDED,
	LEFT_HANDED,
	CENTER_HANDED
};

CC_ENUM (uint8, ECrossLevelTriggerFlags)
{
	SFL_CROSS_TRIGGER_1		= BIT(0),
	SFL_CROSS_TRIGGER_2		= BIT(1),
	SFL_CROSS_TRIGGER_3		= BIT(2),
	SFL_CROSS_TRIGGER_4		= BIT(3),
	SFL_CROSS_TRIGGER_5		= BIT(4),
	SFL_CROSS_TRIGGER_6		= BIT(5),
	SFL_CROSS_TRIGGER_7		= BIT(6),
	SFL_CROSS_TRIGGER_8		= BIT(7),
	SFL_CROSS_TRIGGER_MASK	= 255
};

// noise types for PlayerNoise
CC_ENUM (uint8, ENoiseType)
{
	PNOISE_SELF,
	PNOISE_WEAPON,
	PNOISE_IMPACT
};

//
// this structure is left intact through an entire game
// it should be initialized at dll load time, and read/written to
// the server.ssv file for savegames
//
CC_ENUM (uint16, EGameMode)
{
	GAME_SINGLEPLAYER		=	BIT(0),
	GAME_DEATHMATCH			=	BIT(1),
	GAME_COOPERATIVE		=	BIT(2),

#ifdef CLEANCTF_ENABLED
	GAME_CTF				=	BIT(3)
#endif
};

struct game_locals_t
{
	char		helpmessage1[128];
	char		helpmessage2[128];
	uint8		helpchanged;	// flash F1 icon if non 0, play sound
								// and increment only if 1, 2, or 3

	gclient_t	*clients;		// [maxclients]

	// can't store spawnpoint in level, because
	// it would get overwritten by the savegame restore
	char		spawnpoint[32];	// needed for coop respawns

	// store latched cvars here that we want to get at often
	uint8		maxclients;
	uint8		maxspectators;
	sint32			maxentities;
	bool		cheats;
	EGameMode	mode; // Game mode

	// cross level triggers
	ECrossLevelTriggerFlags		serverflags;
	bool		autosaved;
};

//
// this structure is cleared as each map is entered
// it is read/written to the level.sav file for savegames
//
struct GoalList_t
{
	uint8		Total;
	uint8		Found;
};

class CKeyValuePair
{
public:
	char	*Key;
	char	*Value;

	CKeyValuePair (const char *Key, const char *Value) :
	Key((Key) ? Q_strlwr(Mem_PoolStrDup(Key, com_gamePool, 0)) : NULL),
	Value((Key) ? Mem_PoolStrDup(Value, com_gamePool, 0) : NULL)
	{
	};

	~CKeyValuePair ()
	{
		if (Key)
			QDelete Key;
		if (Value)
			QDelete Value;
	};
};

typedef std::list<CKeyValuePair*, std::game_allocator<CKeyValuePair*> > TKeyValuePairContainer;
typedef std::list<edict_t*, std::game_allocator<edict_t*> > TEntitiesContainer;

class CLevelLocals
{
public:
	void Clear ()
	{
		CLevelLocals();
	};

	FrameNumber_t	Frame;

	std::cc_string	FullLevelName;		// the descriptive name (Outer Base, etc)
	std::cc_string	ServerLevelName;		// the server name (base1, etc)
	std::cc_string	NextMap;		// go here when fraglimit is hit
	std::cc_string	ForceMap;		// go here

	// intermission state
	FrameNumber_t		IntermissionTime;		// time the intermission was started
	char		*ChangeMap;
	bool		ExitIntermission;
	vec3f		IntermissionOrigin;
	vec3f		IntermissionAngles;

	CPlayerEntity		*SightClient;	// changed once each frame for coop games

#ifndef MONSTERS_USE_PATHFINDING
	CBaseEntity	*SightEntity;
	FrameNumber_t	SightEntityFrame;
	CBaseEntity	*SoundEntity;
	FrameNumber_t	SoundEntityFrame;
	CBaseEntity	*SoundEntity2;
	FrameNumber_t	SoundEntity2Frame;
#else
	class		CPathNode	*NoiseNode;
	float		SoundEntityFramenum;
	CPlayerEntity		*SoundEntity;
#endif

	GoalList_t	Secrets;
	GoalList_t	Goals;

	struct MonsterCount_t
	{
		uint16		Total;
		uint16		Killed;
	} Monsters;

	CBaseEntity	*CurrentEntity;	// entity running from G_RunFrame

	uint8		PowerCubeCount;		// ugly necessity for coop
	uint32		Inhibit;
	uint32		EntityNumber;

	std::cc_string			ClassName;
	TKeyValuePairContainer	ParseData;

	// Entity list
	struct entities_t
	{
		TEntitiesContainer		Open, Closed;
	} Entities;
};

CC_ENUM (uint8, EFuncState)
{
	STATE_TOP,
	STATE_BOTTOM,
	STATE_UP,
	STATE_DOWN
};

extern	game_locals_t	game;
extern	CLevelLocals	level;

// means of death
CC_ENUM (uint32, EMeansOfDeath)
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
#ifdef CLEANCTF_ENABLED
	MOD_GRAPPLE,
#endif

	MOD_FRIENDLY_FIRE		=	512
};

extern	EMeansOfDeath	meansOfDeath;

extern	edict_t			*g_edicts;

#define FOFS(x) (sint32)&(((edict_t *)0)->x)
#define STOFS(x) (sint32)&(((spawn_temp_t *)0)->x)
#define LLOFS(x) (sint32)&(((CLevelLocals *)0)->x)
#define CLOFS(x) (sint32)&(((CClient *)0)->x)

// Spawnflags
// 6 bits reserved for editor flags
// 8 bits used as power cube id bits for coop games

// item spawnflags
CC_ENUM (uint32, EItemSpawnflags)
{
	ITEM_TRIGGER_SPAWN			= BIT(0),
	ITEM_NO_TOUCH				= BIT(1),


	DROPPED_ITEM				= BIT(16),
	DROPPED_PLAYER_ITEM			= BIT(17),
	ITEM_TARGETS_USED			= BIT(18)
};

// edict->spawnflags
// these are set with checkboxes on each entity in the map editor
CC_ENUM (uint32, EEdictSpawnflags)
{
	SPAWNFLAG_NOT_EASY			= BIT(8),
	SPAWNFLAG_NOT_MEDIUM		= BIT(9),
	SPAWNFLAG_NOT_HARD			= BIT(10),
	SPAWNFLAG_NOT_DEATHMATCH	= BIT(11),
	SPAWNFLAG_NOT_COOP			= BIT(12)
};

//
// g_combat.c
//
void T_RadiusDamage (CBaseEntity *inflictor, CBaseEntity *attacker, float damage, CBaseEntity *ignore, float radius, EMeansOfDeath mod);

// damage flags
enum
{
	DAMAGE_RADIUS				= BIT(0), // Indirect damage
	DAMAGE_NO_ARMOR				= BIT(1), // Goes through armor
	DAMAGE_ENERGY				= BIT(2), // Energy-based (blaster)
	DAMAGE_NO_KNOCKBACK			= BIT(3), // Don't add knockback
	DAMAGE_BULLET				= BIT(4), // Bullet damage (used for ricochets)
	DAMAGE_NO_PROTECTION		= BIT(5)  // Always damages
};

#define DEFAULT_BULLET_HSPREAD	300
#define DEFAULT_BULLET_VSPREAD	500
#define DEFAULT_SHOTGUN_HSPREAD	1000
#define DEFAULT_SHOTGUN_VSPREAD	500
#define DEFAULT_DEATHMATCH_SHOTGUN_COUNT	12
#define DEFAULT_SHOTGUN_COUNT	12
#define DEFAULT_SSHOTGUN_COUNT	20

//
// g_ai.c
//
void AI_SetSightClient ();

//
// g_client.c
//
void BeginIntermission (class CTargetChangeLevel *targ);

//============================================================================

// this structure is cleared on each PutClientInServer(),
// except for 'client->Persistent'
struct gclient_t
{
	// known to server
	playerState_t		playerState;				// communicated by server to clients
	sint32				ping;
};

struct edict_t
{
	entityStateOld_t		state;
	gclient_t				*client;	// NULL if not a player
										// the server expects the first part
										// of gclient_s to be a player_state_t
										// but the rest of it is opaque
	BOOL					inUse;
	sint32					linkCount;

	// FIXME: move these fields to a server private sv_entity_t
	link_t					area;		// linked to a division node or leaf
	
	sint32					numClusters;	// if -1, use headnode instead
	sint32					clusterNums[MAX_ENT_CLUSTERS];
	sint32					headNode;		// unused if numClusters != -1
	sint32					areaNum, areaNum2;

	//================================

	EServerFlags			svFlags;			// SVF_NOCLIENT, SVF_DEADMONSTER, SVF_MONSTER, etc
	vec3f					mins, maxs;
	vec3f					absMin, absMax, size;
	ESolidType				solid;
	EBrushContents			clipMask;
	edict_t					*owner;


	// DO NOT MODIFY ANYTHING ABOVE THIS, THE SERVER
	// EXPECTS THE FIELDS IN THAT ORDER!

	//================================

	FrameNumber_t		freetime;			// sv.time when the object was freed
	bool				usedBefore;
	
	//
	// only used locally in game, not by server
	//
	// Paril
	CBaseEntity		*Entity;
#if 0
	const NewtonBody	*newtonBody;
#endif
};

//
// cc_utils.cpp
//
void	G_ProjectSource (const vec3f &point, const vec3f &distance, const vec3f &forward, const vec3f &right, vec3f &result);

// Changed to sint32, rarely used as a float..
CBaseEntity *FindRadius (CBaseEntity *From, vec3f &org, sint32 Radius, uint32 EntityFlags, bool CheckNonSolid);

template <class ReturnType, uint32 EntityFlags>
ReturnType *FindRadius (CBaseEntity *From, vec3f &org, sint32 Radius, bool CheckNonSolid = true)
{
	return entity_cast<ReturnType>(FindRadius (From, org, Radius, EntityFlags, CheckNonSolid));
}

template <uint32 EntityFlags>
inline CBaseEntity *FindRadius (CBaseEntity *From, vec3f &org, sint32 Radius, bool CheckNonSolid = true)
{
	return FindRadius (From, org, Radius, EntityFlags, CheckNonSolid);
}

_CC_INSECURE_DEPRECATE (CreateEntityFromClassname)
edict_t	*G_Spawn ();

_CC_INSECURE_DEPRECATE (Function not needed)
void	G_InitEdict (edict_t *e);

_CC_INSECURE_DEPRECATE (Entity->Free)
void	G_FreeEdict (edict_t *e);

void	ED_CallSpawn (edict_t *ent);

inline CBaseEntity *CreateEntityFromClassname (const char *classname)
{
_CC_DISABLE_DEPRECATION
	edict_t *ent = G_Spawn ();

	level.ClassName = classname;
	ED_CallSpawn (ent);

	if (ent->inUse && ent->Entity && !ent->Entity->Freed)
		return ent->Entity;
	return NULL;
_CC_ENABLE_DEPRECATION
}

extern	CCvar	*maxentities;
extern	CCvar	*deathmatch;
extern	CCvar	*coop;
extern	CCvar	*dmflags;
extern	CCvar	*skill;
extern	CCvar	*fraglimit;
extern	CCvar	*timelimit;
extern	CCvar	*password;
extern	CCvar	*spectator_password;
extern	CCvar	*needpass;
extern	CCvar	*g_select_empty;
extern	CCvar	*dedicated;

extern	CCvar	*filterban;

extern	CCvar	*sv_gravity;

extern	CCvar	*gun_x, *gun_y, *gun_z;
extern	CCvar	*sv_rollspeed;
extern	CCvar	*sv_rollangle;

extern	CCvar	*run_pitch;
extern	CCvar	*run_roll;
extern	CCvar	*bob_up;
extern	CCvar	*bob_pitch;
extern	CCvar	*bob_roll;

extern	CCvar	*developer;

extern	CCvar	*sv_cheats;
extern	CCvar	*maxclients;
extern	CCvar	*maxspectators;

extern	CCvar	*flood_msgs;
extern	CCvar	*flood_persecond;
extern	CCvar	*flood_waitdelay;

extern	CCvar	*sv_maplist;
extern	CCvar	*map_debug;
extern	CCvar	*cc_techflags;

#ifdef CLEANCTF_ENABLED
extern	CCvar	*capturelimit;
extern	CCvar	*instantweap;
#endif

extern CBaseEntity *World;
extern CItemList *ItemList;

#else
FILE_WARNING
#endif