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

#if !defined(__CC_PLAYERENTITY_H__) || !defined(INCLUDE_GUARDS)
#define __CC_PLAYERENTITY_H__

class CPlayerState
{
protected:
	playerState_t	*playerState; // Private so no one mucks with it if they shouldn't

public:
	CPlayerState (playerState_t *playerState);
	
	pMoveState_t	*GetPMove (); // Direct pointer
	void			SetPMove (pMoveState_t *newState);

	// Unless, of course, you use the vec3f class :D
	vec3f			&GetViewAngles ();
	vec3f			&GetViewOffset ();

	vec3f			&GetGunAngles ();
	vec3f			&GetGunOffset ();
	vec3f			&GetKickAngles ();

	MediaIndex		&GetGunIndex ();

	int				&GetGunFrame ();

	colorf			&GetViewBlend ();
	colorb			GetViewBlendB (); // Name had to be different

	float			&GetFov ();

	ERenderDefFlags	&GetRdFlags ();

	int16			&GetStat (uint8 index);
	void			CopyStats (int16 *Stats);
	int16			*GetStats ();

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

// client data that stays across multiple level loads
class CPersistentData
{
public:
	std::cc_string	UserInfo;
	IPAddress	IP;
	char		netname[16];
	int			hand;

	EClientState state;			// a loadgame will leave valid entities that
									// just don't have a connection yet

	// values saved and restored from edicts when changing levels
	int			health;
	int			max_health;
	int			savedFlags;

	CInventory	Inventory;

	// ammo capacities
	int			maxAmmoValues[CAmmo::AMMOTAG_MAX];

	CWeapon		*Weapon, *LastWeapon;
	CArmor		*Armor; // Current armor.
#ifdef CLEANCTF_ENABLED
	CFlag		*Flag; // Set if holding a flag
#endif
	CTech		*Tech; // Set if holding a tech
	// Stored here for convenience. (dynamic_cast ew)

	int			PowerCubeCount;	// used for tracking the cubes in coop games
	int			score;			// for calculating total unit score in coop games

	int			game_helpchanged;
	int			helpchanged;

	bool		spectator;			// client is a spectator

	colorf		viewBlend; // View blending
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
	int					Cursor; // Cursor position (relative to order)
	EMenuKeys			Key; // Key hit this frame
	// CurrentMenu is set to a copy of whatever menu class
	// you need
	class CMenu			*CurrentMenu;
	bool				InMenu;

	CMenuState			();
	CMenuState			(CPlayerEntity *ent);

	void OpenMenu		(); // Do this AFTER setting CurrentMenu
	void CloseMenu		();

	void SelectNext		(); // invnext
	void SelectPrev		(); // invprev
	void Select			(); // invuse
};

// client data that stays across deathmatch respawns
class CRespawnData
{
public:
	CPersistentData	coop_respawn;	// what to set client->Persistent to on a respawn
	int			enterframe;			// level.Frame the client entered the game
	int			score;				// frags, etc
	vec3_t		cmd_angles;			// angles sent over in the last command

	bool		spectator;			// client is a spectator
	EGender		Gender;
	int			messageLevel;

#ifdef MONSTERS_USE_PATHFINDING
	class CPathNode	*LastNode;
#endif

	CMenuState	MenuState;

#ifdef CLEANCTF_ENABLED
//ZOID
	ETeamIndex			ctf_team;			// CTF team
	int			ctf_state;
	FrameNumber_t		ctf_lasthurtcarrier;
	FrameNumber_t		ctf_lastreturnedflag;
	FrameNumber_t		ctf_flagsince;
	FrameNumber_t		ctf_lastfraggedcarrier;
	bool		id_state;
	bool		voted; // for elections
	bool		ready;
	bool		admin;
	ghost_t		*ghost; // for ghost codes
//ZOID
#endif
};

CC_ENUM (uint8, ELayoutFlags)
{
	LF_SHOWSCORES		= BIT(0),
	LF_SHOWINVENTORY	= BIT(1),
	LF_SHOWHELP			= BIT(2),
	LF_UPDATECHASE		= BIT(3),

	LF_SCREEN_MASK		= (LF_SHOWSCORES | LF_SHOWINVENTORY | LF_SHOWHELP),
};

// short should suffice here.
// Not unsigned because healing shots can affect screen too.
CC_ENUM (int16, EDamageType)
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

#ifndef MONSTERS_USE_PATHFINDING
class CPlayerNoise : public virtual CBaseEntity
{
public:
	FrameNumber_t	Time;

	CPlayerNoise () :
	  CBaseEntity ()
	{
	};

	CPlayerNoise (int Index) :
	  CBaseEntity (Index)
	{
	};
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
protected:
	gclient_t	*client; // Private so no one messes it up!

public:
	CPlayerState	PlayerState;

	vec3f			KickAngles;	// weapon kicks
	vec3f			KickOrigin;
	vec3f			ViewAngle;			// aiming direction
	vec3f			DamageFrom;		// origin for vector calculation
	colorf			DamageBlend;
#ifndef MONSTERS_USE_PATHFINDING
	CBaseEntity		*mynoise;		// can go in client only
	CBaseEntity		*mynoise2;
#endif
	vec3f			OldViewAngles;
	vec3f			OldVelocity;
	vec2f			ViewDamage;
	FrameNumber_t	ViewDamageTime;
	float			KillerYaw;			// when dead, look at killer
	CPersistentData	Persistent;
	CRespawnData	Respawn;
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

	CClient (gclient_t *client);

	int				&GetPing ();
	void			Clear ();

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

		int16				SilencerShots;

		uint8				MachinegunShots;	// for weapon raising
		bool				BreatherSound;
	} Timers;

	struct client_Grenade_Data_t
	{
		bool			BlewUp;
		bool			Thrown;
		FrameNumber_t	Time;
	} Grenade;

	FrameNumber_t		PickupMessageTime;

	struct client_Flood_t
	{
		FrameNumber_t	LockTill; // locked from talking
		FrameNumber_t	When[10]; // when messages were said
		uint8			WhenHead; // head pointer for when said
	} Flood;

	FrameNumber_t		RespawnTime;		// can respawn when time > this

	struct client_Chase_t
	{
		CPlayerEntity	*Target;
		uint8			Mode;
	} Chase;

#ifdef CLEANCTF_ENABLED
//ZOID
	class CGrappleEntity*ctf_grapple;		// entity of grapple
	EGrappleState		ctf_grapplestate;		// true if pulling
	FrameNumber_t		ctf_grapplereleasetime;	// time of grapple release
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
	int					NextDrownDamage;
	FrameNumber_t		PainDebounceTime;

	CPlayerEntity (int Index);

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
	int				PowerArmorType ();
	void			SetClientEffects ();
	inline void		SetClientEvent (float xyspeed);
	inline void		SetClientSound ();
	inline void		SetClientFrame (float xyspeed);
	void			SetStats ();
	void			SetSpectatorStats ();
#ifdef CLEANCTF_ENABLED
	void			SetCTFStats ();
	void			CTFSetIDView ();
	void			CTFScoreboardMessage (bool reliable);
	void			CTFAssignGhost ();

	bool			ApplyStrengthSound();
	bool			ApplyHaste();
	void			ApplyHasteSound();
	bool			HasRegeneration();
#endif

	void			DeadDropTech ();
	void			TossClientWeapon ();
	void			MoveToIntermission ();
	void			ClientThink (userCmd_t *ucmd);

	void			DeathmatchScoreboardMessage (bool reliable);
	void			EndServerFrame ();
	void			LookAtKiller (CBaseEntity *inflictor, CBaseEntity *attacker);

	void			InitResp ();
	static void		SaveClientData ();
	void			SelectSpawnPoint (vec3f &origin, vec3f &angles);
	CBaseEntity		*SelectCoopSpawnPoint ();

#ifdef CLEANCTF_ENABLED
	void			CTFDeadDropFlag ();
	CBaseEntity		*SelectCTFSpawnPoint ();
	void			CTFAssignTeam ();
	void			CTFAssignSkin (std::cc_string s);
	bool			CTFStart ();
#endif

	void			Die (CBaseEntity *inflictor, CBaseEntity *attacker, int damage, vec3f &point);

	// Printing routines
	inline void		PrintToClient (EGamePrintLevel printLevel, char *fmt, ...);

	void			UpdateChaseCam();
	void			ChaseNext();
	void			ChasePrev();
	void			GetChaseTarget ();

	void			TossHead (int damage);

	void			P_ProjectSource (vec3f distance, vec3f &forward, vec3f &right, vec3f &result);
	void			PlayerNoiseAt (vec3f Where, int type);
};

void ClientEndServerFrames ();

#else
FILE_WARNING
#endif