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

#if !defined(CC_GUARD_PLAYERENTITY_H) || !INCLUDE_GUARDS
#define CC_GUARD_PLAYERENTITY_H

class CPlayerState
{
	friend class CClient;
protected:
	SPlayerState	*playerState; // Private so no one mucks with it if they shouldn't

public:
	CPlayerState (SPlayerState *playerState);

	void			Initialize (SPlayerState *playerState);
	
	SPMoveState	*GetPMove (); // Direct pointer
	void			SetPMove (SPMoveState *newState);

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

/**
\typedef	uint8 EGender

\brief	Defines an alias representing a gender.
**/
typedef uint8 EGender;

/**
\enum	

\brief	Values that represent a gender. 
**/
enum
{
	GENDER_MALE,
	GENDER_FEMALE,
	GENDER_NEUTRAL
};

/**
\typedef	uint8 EClientState

\brief	Defines an alias representing state of a client's connection.
**/
typedef uint8 EClientState;

/**
\enum	

\brief	Values that represent the state of a client's connection. 
**/
enum
{
	SVCS_FREE,		// can be reused for a new connection
	SVCS_CONNECTED,	// has been assigned to a svClient_t, but not in game yet
	SVCS_SPAWNED	// client is fully in game
};

void LoadWeapon (CFile &File, CWeapon **Weapon);
void SaveWeapon (CFile &File, CWeapon *Weapon);

#include "cc_userinfo.h"

/**
\typedef	uint8 EHandedness

\brief	Defines an alias representing the handedness of a player.
**/
typedef uint8 EHandedness;

/**
\enum	

\brief	Values that represent the handedness of a player. 
**/
enum
{
	RIGHT_HANDED,
	LEFT_HANDED,
	CENTER_HANDED
};

// client data that stays across multiple level loads
class CPersistentData
{
public:
	CPersistentData ();
	~CPersistentData();

	void Save (CFile &File)
	{
		UserInfo.Save (File);
		File.Write (Name);

		File.Write<IPAddress> (IP);
		File.Write<EHandedness> (Hand);

		File.Write<sint32> (Health);
		File.Write<sint32> (MaxHealth);
		File.Write<sint32> (SavedFlags);

		Inventory.Save (File);
		File.WriteArray<sint32> (MaxAmmoValues, CAmmo::AMMOTAG_MAX);

		SaveWeapon (File, Weapon);
		SaveWeapon (File, LastWeapon);

		File.Write<sint32> ((Armor) ? Armor->GetIndex() : -1);

		File.Write<sint32> (PowerCubeCount);
		File.Write<sint32> (Score);
		File.Write<uint8> (GameHelpChanged);
		File.Write<uint8> (HelpChanged);
		File.Write<bool> (Spectator);
		File.Write<colorf> (ViewBlend);
	}

	void Load (CFile &File)
	{
		UserInfo.Load (File);
		Name = File.ReadCCString ();

		IP = File.Read<IPAddress> ();
		Hand = File.Read<EHandedness> ();

		Health = File.Read<sint32> ();
		MaxHealth = File.Read<sint32> ();
		SavedFlags = File.Read<sint32> ();

		Inventory.Load (File);
		File.ReadArray (MaxAmmoValues, CAmmo::AMMOTAG_MAX);

		LoadWeapon (File, &Weapon);
		LoadWeapon (File, &LastWeapon);

		sint32 Index = File.Read<sint32> ();

		if (Index != -1)
			Armor = dynamic_cast<CArmor*>(GetItemByIndex(Index));

		PowerCubeCount = File.Read<sint32> ();
		Score = File.Read<sint32> ();
		GameHelpChanged = File.Read<uint8> ();
		HelpChanged = File.Read<uint8> ();
		Spectator = File.Read<bool> ();
		ViewBlend = File.Read<colorf> ();
	}

	CUserInfo		UserInfo;
	std::string		Name;
	IPAddress		IP;
	EHandedness		Hand;

	EClientState 	State;			// a loadgame will leave valid entities that
									// just don't have a connection yet

	// values saved and restored from edicts when changing levels
	sint32			Health;
	sint32			MaxHealth;
	sint32			SavedFlags;

	CInventory		Inventory;

	// ammo capacities
	sint32			MaxAmmoValues[CAmmo::AMMOTAG_MAX];

	CWeapon			*Weapon, *LastWeapon;
	CArmor			*Armor; // Current armor.
#if CLEANCTF_ENABLED
	CFlag			*Flag; // Set if holding a flag
#endif
	CTech			*Tech; // Set if holding a tech

	sint32			PowerCubeCount;	// used for tracking the cubes in coop games
	sint32			Score;			// for calculating total unit Score in coop games

	uint8			GameHelpChanged;
	uint8			HelpChanged;

	bool			Spectator;			// client is a Spectator

	colorf			ViewBlend; // View blending

	void Clear ()
	{
		UserInfo.Clear();
		Name.clear();
		Mem_Zero (&IP, sizeof(IP));
		Hand = 0;
		State = 0;
		Health = 0;
		MaxHealth = 0;
		SavedFlags = 0;
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
		GameHelpChanged = 0;
		HelpChanged = 0;
		Spectator = false;
		ViewBlend.Set (0, 0, 0, 0);

		Mem_Zero (&MaxAmmoValues, sizeof(MaxAmmoValues));
	}
};

// All players have a copy of this class.
class CMenuState
{
public:
	/**
	\typedef	uint8 EMenuKeys
	
	\brief	Defines an alias representing a menu key.
	**/
	typedef uint8 EMenuKeys;

	/**
	\enum	
	
	\brief	Values that represent menu keys. 
	**/
	enum
	{
		KEY_NONE,
		KEY_LEFT,
		KEY_RIGHT
	};

	CPlayerEntity		*Player; // A pointer to the entity that is running the show (needed?)
	sint8				Cursor; // Cursor position (relative to order)
	EMenuKeys			Key; // Key hit this frame
	// CurrentMenu is set to a copy of whatever menu class
	// you need
	class CMenu			*CurrentMenu;
	bool				InMenu;

	CMenuState			();
	CMenuState			(CPlayerEntity *Player);

	void Initialize		(CPlayerEntity *Player);

	void OpenMenu		(); // Do this AFTER setting CurrentMenu
	void CloseMenu		();

	void SelectNext		(); // invnext
	void SelectPrev		(); // invprev
	void Select			(); // invuse

	void Clear ()
	{
		Player = NULL;
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
		File.Write<FrameNumber> (EnterFrame);
		File.Write<sint32> (Score);
		File.Write<vec3f> (CmdAngles);
		File.Write<bool> (Spectator);
		File.Write<EGender> (Gender);
		File.Write<EGamePrintLevel> (MessageLevel);
	}

	void Load (CFile &File)
	{
		CoopRespawn.Load (File);
		EnterFrame = File.Read<FrameNumber> ();
		Score = File.Read<sint32> ();
		CmdAngles = File.Read<vec3f> ();
		Spectator = File.Read<bool> ();
		Gender = File.Read<EGender> ();
		MessageLevel = File.Read<EGamePrintLevel> ();
	}

	CPersistentData		CoopRespawn;	// what to set client->Persistent to on a respawn
	FrameNumber		EnterFrame;		// Level.Frame the client entered the game
	sint32				Score;			// frags, etc
	vec3f				CmdAngles;		// angles sent over in the last command

	bool				Spectator;		// client is a Spectator
	EGender				Gender;
	EGamePrintLevel		MessageLevel;
#if CLEANCODE_IRC
	CIRCClient			IRC;
#endif

	CMenuState			MenuState;

#if CLEANCTF_ENABLED
//ZOID
	struct SCTFData
	{
		ETeamIndex		Team;					// CTF team
		uint8			State;
		FrameNumber		LastHurtCarrier,
						LastReturnedFlag,
						FlagSince,
						LastFraggedCarrier;

		bool			IDState;
		bool			Voted;					// for elections
		bool			Admin;
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
		MenuState.Clear ();

#if CLEANCTF_ENABLED
		Mem_Zero (&CTF, sizeof(CTF));
#endif
	}
};

/**
\typedef	uint8 ELayoutFlags

\brief	Defines an alias representing the layout flags.
**/
typedef uint8 ELayoutFlags;

/**
\enum	

\brief	Values that represent flags for a client's layout system. 
**/
enum
{
	LF_SHOWSCORES		= BIT(0),
	LF_SHOWINVENTORY	= BIT(1),
	LF_SHOWHELP			= BIT(2),
	LF_UPDATECHASE		= BIT(3),

	LF_SCREEN_MASK		= (LF_SHOWSCORES | LF_SHOWINVENTORY | LF_SHOWHELP),
};

/**
\typedef	sint16 EDamageType

\brief	Defines an alias representing the type of damage dealt to a player.
**/
typedef uint8 EDamageType;

/**
\enum	

\brief	Values that represent the type of damage dealt to a player. 
**/
enum
{
	DT_ARMOR,			// damage absorbed by armor
	DT_POWERARMOR,		// damage absorbed by power armor
	DT_BLOOD,			// damage taken out of health
	DT_KNOCKBACK,		// impact damage

	DT_MAX
};

/**
\typedef	uint8 EWeaponState

\brief	Defines an alias representing state of a weapon.
**/
typedef uint8 EWeaponState;

/**
\enum	

\brief	Values that represent the state of a weapon.
		This could be used for animations and such.
**/
enum
{
	WS_ACTIVATING,
	WS_IDLE,
	WS_FIRING,
	WS_DEACTIVATING
};

/**
\typedef	uint8 EPowerArmorType

\brief	Defines an alias representing the type of power armor worn.
**/
typedef uint8 EPowerArmorType;

/**
\enum	

\brief	Values that represent the type of power armor worn.
**/
enum
{
	POWER_ARMOR_NONE,
	POWER_ARMOR_SCREEN,
	POWER_ARMOR_SHIELD
};

class CPlayerNoise : public virtual IBaseEntity
{
public:
	FrameNumber	Time;

	CPlayerNoise () :
	  IBaseEntity ()
	{
		EntityFlags |= ENT_NOISE;
	};

	CPlayerNoise (sint32 Index) :
	  IBaseEntity (Index)
	{
		EntityFlags |= ENT_NOISE;
	};

	void SaveFields (CFile &File)
	{
		File.Write<FrameNumber> (Time);
	}

	void LoadFields (CFile &File)
	{
		Time = File.Read<FrameNumber> ();
	}

	IMPLEMENT_SAVE_HEADER(CPlayerNoise);
};

/**
\typedef	uint8 EAnimPriority

\brief	Defines an alias representing the animation types.
**/
typedef uint8 EAnimPriority;

/**
\enum	

\brief	Values that represent animation types. 
**/
enum
{
	ANIM_BASIC,
	ANIM_WAVE,
	ANIM_JUMP,
	ANIM_PAIN,
	ANIM_ATTACK,
	ANIM_DEATH,
	ANIM_REVERSE
};

// view pitching times
const int	DAMAGE_TIME		= 5;
const int	FALL_TIME		= 3;

class CClient
{
public:
	void Write (CFile &File);	
	void Load (CFile &File);

	void WriteClientStructure (CFile &File);
	static void ReadClientStructure (CFile &File, sint32 index);
	void RepositionClient (SServerClient *client);

protected:
	SServerClient		*client; // Private so no one messes it up!

public:
	CPlayerState	PlayerState;
	CPersistentData	Persistent;
	CRespawnData	Respawn;

	vec3f			KickAngles;	// weapon kicks
	vec3f			KickOrigin;
	vec3f			ViewAngle;			// aiming direction
	vec3f			DamageFrom;		// origin for vector calculation
	colorf			DamageBlend;
	IBaseEntity		*mynoise;		// can go in client only
	IBaseEntity		*mynoise2;
	vec3f			OldViewAngles;
	vec3f			OldVelocity;
	float			ViewDamage[2];
	FrameNumber		ViewDamageTime;
	float			KillerYaw;			// when dead, look at killer
	SPMoveState		OldPMove;	// for detecting out-of-pmove changes
	ELayoutFlags	LayoutFlags;
	// sum up damage over an entire frame, so
	// shotgun blasts give a single big kick
	sint32			DamageValues[DT_MAX];
	EButtons		Buttons;
	EButtons		LatchedButtons;
	CWeapon			*NewWeapon;
	EWeaponState	WeaponState;
	FrameNumber		FallTime;
	float			FallValue;		// for view drop on fall
	float			BonusAlpha;
	float			BobTime;			// so off-ground doesn't change it
	uint8			PowerArmorTime;
	bool			PowerArmorEnabled;
	MediaIndex		WeaponSound;

#if ROGUE_FEATURES
	class CRogueBaseSphere	*OwnedSphere;
#endif

	// animation vars
	struct SClientAnimation
	{
		uint16			EndFrame;
		EAnimPriority	Priority;
		bool			Duck, Run;
	} Anim;

	// powerup timers
	struct SClientTimers
	{
		FrameNumber			QuadDamage,
							Invincibility,
							Rebreather,
							EnvironmentSuit
#if XATRIX_FEATURES
						,	QuadFire
#endif

#if ROGUE_FEATURES
						, Double,
							IR,
							Nuke,
							Tracker
#endif
							;

		sint16				SilencerShots;

		uint8				MachinegunShots;	// for weapon raising
		bool				BreatherSound;

		FrameNumber			PickupMessageTime;
		FrameNumber			RespawnTime;		// can respawn when time > this
	} Timers;

	struct SClientGrenadeData
	{
		bool			BlewUp;
		bool			Thrown;
		FrameNumber	Time;
	} Grenade;

	struct SClientFlood
	{
		FrameNumber		LockTill; // locked from talking
		FrameNumber		When[10]; // when messages were said
		uint8			WhenHead; // head pointer for when said
	} Flood;

	struct SClientChase
	{
		CPlayerEntity	*Target;
		uint8			Mode;
	} Chase;

#if CLEANCTF_ENABLED
//ZOID
	struct SCTFGrapple
	{
		class CGrappleEntity	*Entity;
		EGrappleState			State;
		FrameNumber				ReleaseTime;
	} Grapple;
//ZOID
#endif

	struct SClientTech
	{
		// Tech-specific fields
		FrameNumber	RegenTime;

		// Global fields
		FrameNumber	SoundTime;
		FrameNumber	LastTechMessage;
	} Tech;

	CClient (SServerClient *client);

	sint32			&GetPing ();
	void			Clear ();
};

/**
\typedef	uint8 ENoiseType

\brief	Defines an alias representing type of a player noise.
**/
typedef uint8 ENoiseType;

/**
\enum	

\brief	Values that represent a type of player noise. 
**/
enum
{
	PNOISE_SELF,
	PNOISE_WEAPON,
	PNOISE_IMPACT
};

// Players don't think or have (game) controlled physics.
// PhysicsEntity inherited for velocity.
class CPlayerEntity : public IHurtableEntity, public IPhysicsEntity
{
public:
	CClient					Client;
	bool					NoClip;
	bool					TossPhysics;
	FrameNumber				FlySoundDebounceTime;
	FrameNumber				DamageDebounceTime;
	FrameNumber				AirFinished;
	FrameNumber				NextDrownTime;
	sint32					NextDrownDamage;
	FrameNumber				PainDebounceTime;

	CPlayerEntity (sint32 Index);
	~CPlayerEntity ();

	void SaveFields (CFile &File)
	{
		IHurtableEntity::SaveFields (File);
		IPhysicsEntity::SaveFields (File);
	
		// Write the player data first
		File.Write<bool> (NoClip);
		File.Write<bool> (TossPhysics);
		File.Write<FrameNumber> (FlySoundDebounceTime);
		File.Write<FrameNumber> (DamageDebounceTime);
		File.Write<FrameNumber> (AirFinished);
		File.Write<FrameNumber> (NextDrownTime);
		File.Write<sint32> (NextDrownDamage);
		File.Write<FrameNumber> (PainDebounceTime);

		// Write client data
		//Client.Write (File);
	}

	void LoadFields (CFile &File)
	{
		IHurtableEntity::LoadFields (File);
		IPhysicsEntity::LoadFields (File);
	
		// Read the player data first
		NoClip = File.Read<bool> ();
		TossPhysics = File.Read<bool> ();
		FlySoundDebounceTime = File.Read<FrameNumber> ();
		DamageDebounceTime = File.Read<FrameNumber> ();
		AirFinished = File.Read<FrameNumber> ();
		NextDrownTime = File.Read<FrameNumber> ();
		NextDrownDamage = File.Read<sint32> ();
		PainDebounceTime = File.Read<FrameNumber> ();

		// Read client data
		//Client.Load (File);
	}

	bool			Run ();
	void			BeginServerFrame ();

	void			BeginDeathmatch ();
	void			Begin ();
	bool			Connect (const char *userinfo, CUserInfo &UserInfo);
	void			Disconnect ();
	void			Obituary (IBaseEntity *Attacker);

	void			SpectatorRespawn ();
	void			CopyToBodyQueue ();
	void			Respawn ();
	void			PutInServer ();
	void			InitPersistent ();
	void			InitItemMaxValues ();
	void			UserinfoChanged (std::string userinfo);
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
#endif

	bool			ApplyStrengthSound();
	bool			ApplyHaste();
	void			ApplyHasteSound();
	bool			HasRegeneration();

	void			DeadDropTech ();
	void			TossClientWeapon ();
	void			MoveToIntermission ();
	void			ClientThink (SUserCmd *ucmd);

	void			DeathmatchScoreboardMessage (bool reliable);
	void			EndServerFrame ();
	void			LookAtKiller (IBaseEntity *Inflictor, IBaseEntity *Attacker);
	void			ShowScores ();
	void			ShowHelp ();
	void			HelpComputer ();
	bool			CheckFlood ();

	void			InitResp ();
	static void		SaveClientData ();
	static void		BackupClientData ();
	static void		RestoreClientData ();
	void			SelectSpawnPoint (vec3f &origin, vec3f &angles);
	class CSpotBase	*SelectCoopSpawnPoint ();
#if ROGUE_FEATURES
	class CSpotBase	*SelectLavaCoopSpawnPoint ();
#endif

#if CLEANCTF_ENABLED
	void			CTFDeadDropFlag ();
	class CSpotBase	*SelectCTFSpawnPoint ();
	void			CTFAssignTeam ();
	void			CTFAssignSkin (CUserInfo &s);
	bool			CTFStart ();
#endif

	void			Die (IBaseEntity *Inflictor, IBaseEntity *Attacker, sint32 Damage, vec3f &Point);

	// Printing routines
	void			PrintToClient (EGamePrintLevel printLevel, const char *fmt, ...);

	void			UpdateChaseCam();
	void			ChaseNext();
	void			ChasePrev();
	void			GetChaseTarget ();

	void			TossHead (sint32 Damage);

	void			P_ProjectSource (vec3f distance, vec3f &forward, vec3f &right, vec3f &result);
	void			PlayerNoiseAt (vec3f Where, ENoiseType type);

	void			PushInDirection (vec3f vel, ESpawnflags flags);

#if ROGUE_FEATURES
	void			RemoveAttackingPainDaemons ();
#endif

	/**
	\fn	void CastTo (ECastFlags castFlags)
	
	\brief	Perform a cast to this entity.
	
	\author	Paril
	\date	29/05/2010
	
	\param	CastFlags	The cast flags. 
	**/
	void			CastTo (ECastFlags castFlags);

	/**
	\fn	void StuffText (const char *text)

	\attention	Be careful with this function - do not abuse it.
	
	\brief	Stuff text to player's command buffer. 
	
	\author	Paril
	\date	29/05/2010
	
	\param	text	The text. 
	**/
	void			StuffText (const char *text);

	IMPLEMENT_SAVE_HEADER(CPlayerEntity)
};

void ClientEndServerFrames ();

#else
FILE_WARNING
#endif
