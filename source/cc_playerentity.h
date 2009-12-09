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
// cc_playerentity.h
// Player entity
//

#if !defined(__CC_PLAYERENTITY_H__) || !INCLUDE_GUARDS
#define __CC_PLAYERENTITY_H__

class CPlayerState
{
	friend class CClient;
protected:
	playerState_t	*playerState; // Private so no one mucks with it if they shouldn't

public:
	CPlayerState (playerState_t *playerState);

	void			Initialize (playerState_t *playerState);
	
	pMoveState_t	*GetPMove (); // Direct pointer
	void			SetPMove (pMoveState_t *newState);

	// Unless, of course, you use the vec3f class :D
	vec3f			&GetViewAngles ();
	vec3f			&GetViewOffset ();

	vec3f			&GetGunAngles ();
	vec3f			&GetGunOffset ();
	vec3f			&GetKickAngles ();

	MediaIndex		&GetGunIndex ();

	sint32				&GetGunFrame ();

	colorf			&GetViewBlend ();
	colorb			GetViewBlendB (); // Name had to be different

	float			&GetFov ();

	ERenderDefFlags	&GetRdFlags ();

	sint16			&GetStat (uint8 index);
	void			CopyStats (sint16 *Stats);
	sint16			*GetStats ();

	void			Clear ();
};

CC_ENUM (uint8, EGender)
{
	GENDER_MALE,
	GENDER_FEMALE,
	GENDER_NEUTRAL
};

// svClient->state options
CC_ENUM (uint8, EClientState)
{
	SVCS_FREE,		// can be reused for a new connection
	SVCS_CONNECTED,	// has been assigned to a svClient_t, but not in game yet
	SVCS_SPAWNED	// client is fully in game
};

void LoadWeapon (CFile &File, CWeapon **Weapon);
void SaveWeapon (CFile &File, CWeapon *Weapon);

// client data that stays across multiple level loads
class CPersistentData
{
public:
	CPersistentData ();
	~CPersistentData();

	void Save (CFile &File)
	{
		File.WriteCCString (UserInfo);
		File.WriteCCString (Name);

		File.Write<IPAddress> (IP);
		File.Write<sint32> (hand);

		File.Write<sint32> (health);
		File.Write<sint32> (max_health);
		File.Write<sint32> (savedFlags);

		Inventory.Save (File);
		File.WriteArray<sint32> (maxAmmoValues, CAmmo::AMMOTAG_MAX);

		SaveWeapon (File, Weapon);
		SaveWeapon (File, LastWeapon);

		File.Write<sint32> ((Armor) ? Armor->GetIndex() : -1);

		File.Write<sint32> (PowerCubeCount);
		File.Write<sint32> (Score);
		File.Write<uint8> (game_helpchanged);
		File.Write<uint8> (helpchanged);
		File.Write<bool> (Spectator);
		File.Write<colorf> (viewBlend);
	}

	void Load (CFile &File)
	{
		UserInfo = File.ReadCCString ();
		Name = File.ReadCCString ();

		IP = File.Read<IPAddress> ();
		hand = File.Read<sint32> ();

		health = File.Read<sint32> ();
		max_health = File.Read<sint32> ();
		savedFlags = File.Read<sint32> ();

		Inventory.Load (File);
		File.ReadArray (maxAmmoValues, CAmmo::AMMOTAG_MAX);

		LoadWeapon (File, &Weapon);
		LoadWeapon (File, &LastWeapon);

		sint32 Index = File.Read<sint32> ();

		if (Index != -1)
			Armor = dynamic_cast<CArmor*>(GetItemByIndex(Index));

		PowerCubeCount = File.Read<sint32> ();
		Score = File.Read<sint32> ();
		game_helpchanged = File.Read<uint8> ();
		helpchanged = File.Read<uint8> ();
		Spectator = File.Read<bool> ();
		viewBlend = File.Read<colorf> ();
	}

	std::cc_string	UserInfo;
	std::cc_string	Name;
	IPAddress		IP;
	sint32			hand;

	EClientState state;			// a loadgame will leave valid entities that
									// just don't have a connection yet

	// values saved and restored from edicts when changing levels
	sint32			health;
	sint32			max_health;
	sint32			savedFlags;

	CInventory		Inventory;

	// ammo capacities
	sint32			maxAmmoValues[CAmmo::AMMOTAG_MAX];

	CWeapon		*Weapon, *LastWeapon;
	CArmor		*Armor; // Current armor.
#if CLEANCTF_ENABLED
	CFlag		*Flag; // Set if holding a flag
#endif
	CTech		*Tech; // Set if holding a tech
	// Stored here for convenience. (dynamic_cast ew)

	sint32			PowerCubeCount;	// used for tracking the cubes in coop games
	sint32			Score;			// for calculating total unit Score in coop games

	uint8			game_helpchanged;
	uint8			helpchanged;

	bool		Spectator;			// client is a Spectator

	colorf		viewBlend; // View blending

	void Clear ()
	{
		UserInfo.clear();
		Name.clear();
		memset (&IP, 0, sizeof(IP));
		hand = 0;
		state = 0;
		health = 0;
		max_health = 0;
		savedFlags = 0;
		Inventory.Clear();
		Weapon = NULL;
		LastWeapon  = NULL;
		Armor = NULL;
#if CLEANCTF_ENABLED
		Flag = NULL;
#endif
		Tech = NULL;
		PowerCubeCount = 0;
		Score = 0;
		game_helpchanged = 0;
		helpchanged = 0;
		Spectator = false;
		viewBlend.Set (0,0,0,0);

		memset (&maxAmmoValues, 0, sizeof(maxAmmoValues));
	}
};

// All players have a copy of this class.
class CMenuState
{
public:
	CC_ENUM (uint8, EMenuKeys)
	{
		KEY_NONE,
		KEY_LEFT,
		KEY_RIGHT
	};

	CPlayerEntity		*ent; // A pointer to the entity that is running the show (needed?)
	sint8				Cursor; // Cursor position (relative to order)
	EMenuKeys			Key; // Key hit this frame
	// CurrentMenu is set to a copy of whatever menu class
	// you need
	class CMenu			*CurrentMenu;
	bool				InMenu;

	CMenuState			();
	CMenuState			(CPlayerEntity *ent);

	void Initialize		(CPlayerEntity *ent);

	void OpenMenu		(); // Do this AFTER setting CurrentMenu
	void CloseMenu		();

	void SelectNext		(); // invnext
	void SelectPrev		(); // invprev
	void Select			(); // invuse

	void Clear ()
	{
		ent = NULL;
		Cursor = -1;
		Key = KEY_NONE;
		CurrentMenu = NULL;
		InMenu = false;
	}
};

// client data that stays across deathmatch respawns
class CRespawnData
{
public:
	CRespawnData ()
	{
		Clear ();
	};

	void Save (CFile &File)
	{
		CoopRespawn.Save (File);
		File.Write<FrameNumber_t> (EnterFrame);
		File.Write<sint32> (Score);
		File.Write<vec3f> (CmdAngles);
		File.Write<bool> (Spectator);
		File.Write<EGender> (Gender);
		File.Write<EGamePrintLevel> (MessageLevel);
	}

	void Load (CFile &File)
	{
		CoopRespawn.Load (File);
		EnterFrame = File.Read<FrameNumber_t> ();
		Score = File.Read<sint32> ();
		CmdAngles = File.Read<vec3f> ();
		Spectator = File.Read<bool> ();
		Gender = File.Read<EGender> ();
		MessageLevel = File.Read<EGamePrintLevel> ();
	}

	CPersistentData		CoopRespawn;	// what to set client->Persistent to on a respawn
	FrameNumber_t		EnterFrame;		// level.Frame the client entered the game
	sint32				Score;			// frags, etc
	vec3f				CmdAngles;		// angles sent over in the last command

	bool				Spectator;		// client is a Spectator
	EGender				Gender;
	EGamePrintLevel		MessageLevel;

#if MONSTERS_USE_PATHFINDING
	class CPathNode		*LastNode;
#endif

	CMenuState			MenuState;

#if CLEANCTF_ENABLED
//ZOID
	struct respawn_CTF_t
	{
		ETeamIndex		Team;					// CTF team
		sint16			State;
		FrameNumber_t	LastHurtCarrier,
						LastReturnedFlag,
						FlagSince,
						LastFraggedCarrier;

		bool			IDState;
		bool			Voted;					// for elections
		bool			Ready;
		bool			Admin;
		CCTFGhost		*Ghost;					// for ghost codes
	} CTF;

//ZOID
#endif

	void Clear ()
	{
		CoopRespawn.Clear ();
		EnterFrame = 0;
		Score = 0;
		CmdAngles.Clear();
		Spectator = false;
		Gender = GENDER_NEUTRAL;
		MessageLevel = 0;
#if MONSTERS_USE_PATHFINDING
		LastNode = NULL;
#endif
		MenuState.Clear ();

#if CLEANCTF_ENABLED
		memset (&CTF, 0, sizeof(CTF));
#endif
	}
};

CC_ENUM (uint8, ELayoutFlags)
{
	LF_SHOWSCORES		= BIT(0),
	LF_SHOWINVENTORY	= BIT(1),
	LF_SHOWHELP			= BIT(2),
	LF_UPDATECHASE		= BIT(3),

	LF_SCREEN_MASK		= (LF_SHOWSCORES | LF_SHOWINVENTORY | LF_SHOWHELP),
};

// sint16 should suffice here.
// Not unsigned because healing shots can affect screen too.
CC_ENUM (sint16, EDamageType)
{
	DT_ARMOR,			// damage absorbed by armor
	DT_POWERARMOR,		// damage absorbed by power armor
	DT_BLOOD,			// damage taken out of health
	DT_KNOCKBACK,		// impact damage

	DT_MAX
};

CC_ENUM (uint8, EWeaponState)
{
	WS_ACTIVATING,
	WS_IDLE,
	WS_FIRING,
	WS_DEACTIVATING
};

// power armor types
CC_ENUM (uint8, EPowerArmorType)
{
	POWER_ARMOR_NONE,
	POWER_ARMOR_SCREEN,
	POWER_ARMOR_SHIELD
};

#if !MONSTERS_USE_PATHFINDING
class CPlayerNoise : public virtual CBaseEntity
{
public:
	FrameNumber_t	Time;

	CPlayerNoise () :
	  CBaseEntity ()
	{
	};

	CPlayerNoise (sint32 Index) :
	  CBaseEntity (Index)
	{
	};

	ENTITYFIELDS_NONSAVABLE
	const char *__GetName () { return "NO!"; }
};
#endif

// client_t->Anim.Priority
CC_ENUM (uint8, EAnimPriority)
{
	ANIM_BASIC,
	ANIM_WAVE,
	ANIM_JUMP,
	ANIM_PAIN,
	ANIM_ATTACK,
	ANIM_DEATH,
	ANIM_REVERSE
};

class CClient
{
public:
	void Write (CFile &File);	
	void Load (CFile &File);

	void WriteClientStructure (CFile &File);
	static void ReadClientStructure (CFile &File, sint32 index);
	void RepositionClient (gclient_t *client);

protected:
	gclient_t		*client; // Private so no one messes it up!

public:
	CPlayerState	PlayerState;
	CPersistentData	Persistent;
	CRespawnData	Respawn;

	vec3f			KickAngles;	// weapon kicks
	vec3f			KickOrigin;
	vec3f			ViewAngle;			// aiming direction
	vec3f			DamageFrom;		// origin for vector calculation
	colorf			DamageBlend;
#if !MONSTERS_USE_PATHFINDING
	CBaseEntity		*mynoise;		// can go in client only
	CBaseEntity		*mynoise2;
#endif
	vec3f			OldViewAngles;
	vec3f			OldVelocity;
	vec2f			ViewDamage;
	FrameNumber_t	ViewDamageTime;
	float			KillerYaw;			// when dead, look at killer
	pMoveState_t	OldPMove;	// for detecting out-of-pmove changes
	ELayoutFlags	LayoutFlags;
	// sum up damage over an entire frame, so
	// shotgun blasts give a single big kick
	EDamageType		DamageValues[DT_MAX];
	EButtons		Buttons;
	EButtons		LatchedButtons;
	CWeapon			*NewWeapon;
	EWeaponState	WeaponState;
	FrameNumber_t	FallTime;
	float			FallValue;		// for view drop on fall
	float			BonusAlpha;
	float			BobTime;			// so off-ground doesn't change it
	uint8			PowerArmorTime;
	EWaterLevel		OldWaterLevel;
	MediaIndex		WeaponSound;

	// animation vars
	struct client_Animation_t
	{
		uint16			EndFrame;
		EAnimPriority	Priority;
		bool			Duck, Run;
	} Anim;

	// powerup timers
	struct client_Timers_t
	{
		FrameNumber_t		QuadDamage,
							Invincibility,
							Rebreather,
							EnvironmentSuit;

		sint16				SilencerShots;

		uint8				MachinegunShots;	// for weapon raising
		bool				BreatherSound;

		FrameNumber_t		PickupMessageTime;
		FrameNumber_t		RespawnTime;		// can respawn when time > this
	} Timers;

	struct client_Grenade_Data_t
	{
		bool			BlewUp;
		bool			Thrown;
		FrameNumber_t	Time;
	} Grenade;

	struct client_Flood_t
	{
		FrameNumber_t	LockTill; // locked from talking
		FrameNumber_t	When[10]; // when messages were said
		uint8			WhenHead; // head pointer for when said
	} Flood;

	struct client_Chase_t
	{
		CPlayerEntity	*Target;
		uint8			Mode;
	} Chase;

#if CLEANCTF_ENABLED
//ZOID
	struct client_CTF_Grapple_t
	{
		class CGrappleEntity		*Entity;
		EGrappleState				State;
		FrameNumber_t				ReleaseTime;
	} Grapple;
//ZOID
#endif

	struct client_Tech_t
	{
		// Tech-specific fields
		FrameNumber_t	RegenTime;

		// Global fields
		FrameNumber_t	SoundTime;
		FrameNumber_t	LastTechMessage;
	} Tech;

	CClient (gclient_t *client);

	sint32			&GetPing ();
	void			Clear ();
};

// Players don't think or have (game) controlled physics.
// PhysicsEntity inherited for velocity.
class CPlayerEntity : public CHurtableEntity, public CPhysicsEntity
{
public:
	CClient				Client;
	bool				NoClip;
	bool				TossPhysics;
	FrameNumber_t		FlySoundDebounceTime;
	FrameNumber_t		DamageDebounceTime;
	FrameNumber_t		AirFinished;
	FrameNumber_t		NextDrownTime;
	sint32				NextDrownDamage;
	FrameNumber_t		PainDebounceTime;

	CPlayerEntity (sint32 Index);
	~CPlayerEntity ();

	void SaveFields (CFile &File)
	{
		CHurtableEntity::SaveFields (File);
		CPhysicsEntity::SaveFields (File);
	
		// Write the player data first
		File.Write<bool> (NoClip);
		File.Write<bool> (TossPhysics);
		File.Write<FrameNumber_t> (FlySoundDebounceTime);
		File.Write<FrameNumber_t> (DamageDebounceTime);
		File.Write<FrameNumber_t> (AirFinished);
		File.Write<FrameNumber_t> (NextDrownTime);
		File.Write<sint32> (NextDrownDamage);
		File.Write<FrameNumber_t> (PainDebounceTime);

		// Write client data
		//Client.Write (File);
	}

	void LoadFields (CFile &File)
	{
		CHurtableEntity::LoadFields (File);
		CPhysicsEntity::LoadFields (File);
	
		// Read the player data first
		NoClip = File.Read<bool> ();
		TossPhysics = File.Read<bool> ();
		FlySoundDebounceTime = File.Read<FrameNumber_t> ();
		DamageDebounceTime = File.Read<FrameNumber_t> ();
		AirFinished = File.Read<FrameNumber_t> ();
		NextDrownTime = File.Read<FrameNumber_t> ();
		NextDrownDamage = File.Read<sint32> ();
		PainDebounceTime = File.Read<FrameNumber_t> ();

		// Read client data
		//Client.Load (File);
	}

	bool			Run ();
	void			BeginServerFrame ();

	void			BeginDeathmatch ();
	void			Begin ();
	bool			Connect (char *userinfo);
	void			Disconnect ();
	void			Obituary (CBaseEntity *attacker);

	void			SpectatorRespawn ();
	void			Respawn ();
	void			PutInServer ();
	void			InitPersistent ();
	void			InitItemMaxValues ();
	void			UserinfoChanged (char *userinfo);
	void			FetchEntData ();

	// EndServerFrame-related functions
	inline float	CalcRoll (vec3f &forward, vec3f &right);
	inline void		DamageFeedback (vec3f &forward, vec3f &right);
	inline void		CalcViewOffset (vec3f &forward, vec3f &right, vec3f &up, float xyspeed);
	inline void		CalcGunOffset (vec3f &forward, vec3f &right, vec3f &up, float xyspeed);
	inline void		CalcBlend ();
	inline void		FallingDamage ();
	inline void		WorldEffects ();
	EPowerArmorType	PowerArmorType ();
	void			SetClientEffects ();
	inline void		SetClientEvent (float xyspeed);
	inline void		SetClientSound ();
	inline void		SetClientFrame (float xyspeed);
	void			SetStats ();
	void			SetSpectatorStats ();
#if CLEANCTF_ENABLED
	void			SetCTFStats ();
	void			CTFSetIDView ();
	void			CTFScoreboardMessage (bool reliable);
	void			CTFAssignGhost ();
#endif

	bool			ApplyStrengthSound();
	bool			ApplyHaste();
	void			ApplyHasteSound();
	bool			HasRegeneration();

	void			DeadDropTech ();
	void			TossClientWeapon ();
	void			MoveToIntermission ();
	void			ClientThink (userCmd_t *ucmd);

	void			DeathmatchScoreboardMessage (bool reliable);
	void			EndServerFrame ();
	void			LookAtKiller (CBaseEntity *inflictor, CBaseEntity *attacker);

	void			InitResp ();
	static void		SaveClientData ();
	static void		BackupClientData ();
	static void		RestoreClientData ();
	void			SelectSpawnPoint (vec3f &origin, vec3f &angles);
	class CSpotBase	*SelectCoopSpawnPoint ();

#if CLEANCTF_ENABLED
	void			CTFDeadDropFlag ();
	class CSpotBase	*SelectCTFSpawnPoint ();
	void			CTFAssignTeam ();
	void			CTFAssignSkin (std::cc_string s);
	bool			CTFStart ();
#endif

	void			Die (CBaseEntity *inflictor, CBaseEntity *attacker, sint32 damage, vec3f &point);

	// Printing routines
	inline void		PrintToClient (EGamePrintLevel printLevel, char *fmt, ...);

	void			UpdateChaseCam();
	void			ChaseNext();
	void			ChasePrev();
	void			GetChaseTarget ();

	void			TossHead (sint32 damage);

	void			P_ProjectSource (vec3f distance, vec3f &forward, vec3f &right, vec3f &result);
	void			PlayerNoiseAt (vec3f Where, sint32 type);

	IMPLEMENT_SAVE_HEADER(CPlayerEntity)
};

void ClientEndServerFrames ();

#else
FILE_WARNING
#endif