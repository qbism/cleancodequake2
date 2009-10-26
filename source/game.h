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
Foundation, Inc., 59 Temple Place - Suite 330, Boston, MA 02111-1307, USA.
*/

// game.h
// - game dll information visible to server

#define GAME_APIVERSION		3

// edict->svFlags

CC_ENUM (int, EServerFlags)
{
	SVF_NOCLIENT			= BIT(0), // don't send entity to clients, even if it has effects
	SVF_DEADMONSTER			= BIT(1), // treat as CONTENTS_DEADMONSTER for collision
	SVF_MONSTER				= BIT(2), // treat as CONTENTS_MONSTER for collision

// ZOID
// entity is simple projectile, used for network optimization
// if an entity is projectile, the model index/x/y/z/pitch/yaw are sent, encoded into
// seven (or eight) bytes.  This is to speed up projectiles.  Currently, only the
// hyperblaster makes use of this.  use for items that are moving with a constant
// velocity that don't change direction or model
	SVF_PROJECTILE			= BIT(3)
// ZOID
};

// edict->solid values
CC_ENUM (int, ESolidType)
{
	SOLID_NOT,			// no interaction with other objects
	SOLID_TRIGGER,		// only touch when inside, after moving
	SOLID_BBOX,			// touch on edge
	SOLID_BSP			// bsp clip, touch on edge
};

// ==========================================================================

// link_t is only used for entity area links now
struct link_t
{
	link_t	*prev, *next;
};

#define MAX_ENT_CLUSTERS	16

#ifndef GAME_INCLUDE

struct gclient_t
{
	playerState_t		playerState;		// communicated by server to clients
	int					ping;
	// the game dll can add anything it wants after
	// this point in the structure
};

struct edict_t
{
	entityStateOld_t	s;
	gclient_t			*client;	// NULL if not a player
									// the server expects the first part
									// of gclient_s to be a player_state_t
									// but the rest of it is opaque
	BOOL				inUse;
	int					linkCount;

	// FIXME: move these fields to a server private sv_entity_t
	link_t				area;				// linked to a division node or leaf
	
	int					numClusters;		// if -1, use headnode instead
	int					clusterNums[MAX_ENT_CLUSTERS];
	int					headNode;			// unused if numClusters != -1
	int					areaNum, areaNum2;

	//================================

	EServerFlags		svFlags;			// SVF_NOCLIENT, SVF_DEADMONSTER, SVF_MONSTER, etc
	vec3_t				mins, maxs;
	vec3_t				absMin, absMax, size;
	ESolidType			solid;
	EBrushContents		clipMask;
	edict_t				*owner;

	// the game dll can add anything it wants after
	// this point in the structure
};

#else		// GAME_INCLUDE

struct edict_t;
struct gclient_t;

#endif		// GAME_INCLUDE

// ==========================================================================

//
// functions provided by the main engine
//
struct gameImport_t
{
	// special messages
#ifndef USE_EXTENDED_GAME_IMPORTS
	_CC_INSECURE_DEPRECATE (BroadcastPrintf)
#endif
	void	(*bprintf) (EGamePrintLevel printlevel, char *fmt, ...);
#ifndef USE_EXTENDED_GAME_IMPORTS
	_CC_INSECURE_DEPRECATE (DebugPrintf)
#endif
	void	(*dprintf) (char *fmt, ...);
#ifndef USE_EXTENDED_GAME_IMPORTS
	_CC_INSECURE_DEPRECATE (ClientPrintf)
#endif
	void	(*cprintf) (edict_t *ent, EGamePrintLevel printLevel, char *fmt, ...);
#ifndef USE_EXTENDED_GAME_IMPORTS
	_CC_INSECURE_DEPRECATE (CenterPrintf)
#endif
	void	(*centerprintf) (edict_t *ent, char *fmt, ...);

#ifndef USE_EXTENDED_GAME_IMPORTS
	_CC_INSECURE_DEPRECATE (PlaySoundFrom or CBaseEntity->PlaySound)
#endif
	void	(*sound) (edict_t *ent, int channel, int soundIndex, float volume, float attenuation, float timeOffset);
#ifndef USE_EXTENDED_GAME_IMPORTS
	_CC_INSECURE_DEPRECATE (PlaySoundAt or CBaseEntity->PlayPositionedSound)
#endif
	void	(*positioned_sound) (vec3_t origin, edict_t *ent, int channel, int soundIndex, float volume, float attenuation, float timeOffset);

	// config strings hold all the index strings, the lightstyles,
	// and misc data like the sky definition and cdtrack.
	// All of the current configstrings are sent to clients when
	// they connect, and changes are sent to all connected clients.
#ifndef USE_EXTENDED_GAME_IMPORTS
	_CC_INSECURE_DEPRECATE (ConfigString)
#endif
	void	(*configstring) (int num, char *string);

#ifndef USE_EXTENDED_GAME_IMPORTS
	_CC_INSECURE_DEPRECATE (GameError)
#endif
	void	(*error) (char *fmt, ...);

	// the *index functions create configstrings and some internal server state
#ifndef USE_EXTENDED_GAME_IMPORTS
	_CC_INSECURE_DEPRECATE (ModelIndex)
#endif
	int		(*modelindex) (char *name);
#ifndef USE_EXTENDED_GAME_IMPORTS
	_CC_INSECURE_DEPRECATE (SoundIndex)
#endif
	int		(*soundindex) (char *name);
#ifndef USE_EXTENDED_GAME_IMPORTS
	_CC_INSECURE_DEPRECATE (ImageIndex)
#endif
	int		(*imageindex) (char *name);

#ifndef USE_EXTENDED_GAME_IMPORTS
	_CC_INSECURE_DEPRECATE (SetModel)
#endif
	void	(*setmodel) (edict_t *ent, char *name);

	// collision detection
#ifndef USE_EXTENDED_GAME_IMPORTS
	_CC_INSECURE_DEPRECATE (class CTrace)
#endif
	cmTrace_t	(*trace) (vec3_t start, vec3_t mins, vec3_t maxs, vec3_t end, edict_t *passEnt, int contentMask);
#ifndef USE_EXTENDED_GAME_IMPORTS
	_CC_INSECURE_DEPRECATE (PointContents)
#endif
	int			(*pointcontents) (vec3_t point);
#ifndef USE_EXTENDED_GAME_IMPORTS
	_CC_INSECURE_DEPRECATE (InArea)
#endif
	BOOL		(*inPVS) (vec3_t p1, vec3_t p2);
#ifndef USE_EXTENDED_GAME_IMPORTS
	_CC_INSECURE_DEPRECATE (InArea)
#endif
	BOOL		(*inPHS) (vec3_t p1, vec3_t p2);
	void		(*SetAreaPortalState) (int portalNum, BOOL open);
	BOOL		(*AreasConnected) (int area1, int area2);

	// an entity will never be sent to a client or used for collision
	// if it is not passed to linkentity.  If the size, position, or
	// solidity changes, it must be relinked.
	/*
#ifndef USE_EXTENDED_GAME_IMPORTS
	_CC_INSECURE_DEPRECATE ((CBaseEntity)->Link)
#endif
	*/
		void	(*linkentity) (edict_t *ent);
	/*
#ifndef USE_EXTENDED_GAME_IMPORTS
	_CC_INSECURE_DEPRECATE ((CBaseEntity)->Unlink)
#endif
	*/
	void	(*unlinkentity) (edict_t *ent);		// call before removing an interactive edict
#ifndef USE_EXTENDED_GAME_IMPORTS
	_CC_INSECURE_DEPRECATE (BoxEdicts)
#endif
	int		(*BoxEdicts) (vec3_t mins, vec3_t maxs, edict_t **list,	int maxCount, int areaType);
#ifndef USE_EXTENDED_GAME_IMPORTS
	_CC_INSECURE_DEPRECATE (SV_Pmove)
#endif
	void	(*Pmove) (
#ifdef USE_EXTENDED_GAME_IMPORTS
	pMove_t *pMove
#else
	pMoveNew_t *pMove // Just to supress error
#endif
	);		// player movement code common with client prediction

	// network messaging
#ifndef USE_EXTENDED_GAME_IMPORTS
	_CC_INSECURE_DEPRECATE (Cast)
#endif
	void	(*multicast) (vec3_t origin, EMultiCast to);

#ifndef USE_EXTENDED_GAME_IMPORTS
	_CC_INSECURE_DEPRECATE (Cast)
#endif
	void	(*unicast) (edict_t *ent, BOOL reliable);

#ifndef USE_EXTENDED_GAME_IMPORTS
	_CC_INSECURE_DEPRECATE (WriteChar)
#endif
	void	(*WriteChar) (int c);

#ifndef USE_EXTENDED_GAME_IMPORTS
	_CC_INSECURE_DEPRECATE (WriteByte)
#endif
	void	(*WriteByte) (int c);

#ifndef USE_EXTENDED_GAME_IMPORTS
	_CC_INSECURE_DEPRECATE (WriteShort)
#endif
	void	(*WriteShort) (int c);

#ifndef USE_EXTENDED_GAME_IMPORTS
	_CC_INSECURE_DEPRECATE (WriteLong)
#endif
	void	(*WriteLong) (int c);

#ifndef USE_EXTENDED_GAME_IMPORTS
	_CC_INSECURE_DEPRECATE (WriteFloat)
#endif
	void	(*WriteFloat) (float f);

#ifndef USE_EXTENDED_GAME_IMPORTS
	_CC_INSECURE_DEPRECATE (WriteString)
#endif
	void	(*WriteString) (char *s);

#ifndef USE_EXTENDED_GAME_IMPORTS
	_CC_INSECURE_DEPRECATE (WritePosition)
#endif
	void	(*WritePosition) (vec3_t pos);	// some fractional bits

#ifndef USE_EXTENDED_GAME_IMPORTS
	_CC_INSECURE_DEPRECATE (WriteDir)
#endif
	void	(*WriteDir) (vec3_t pos);		// single byte encoded, very coarse

#ifndef USE_EXTENDED_GAME_IMPORTS
	_CC_INSECURE_DEPRECATE (WriteAngle)
#endif
	void	(*WriteAngle) (float f);

	// managed memory allocation
#ifndef USE_EXTENDED_GAME_IMPORTS
	_CC_INSECURE_DEPRECATE (QNew)
#endif
	void	*(*TagMalloc) (int size, int tag);
#ifndef USE_EXTENDED_GAME_IMPORTS
	_CC_INSECURE_DEPRECATE (QDelete)
#endif
	void	(*TagFree) (void *block);
#ifndef USE_EXTENDED_GAME_IMPORTS
	_CC_INSECURE_DEPRECATE (Mem_FreePool)
#endif
	void	(*FreeTags) (int tag);

	// console variable interaction
#ifndef USE_EXTENDED_GAME_IMPORTS
	_CC_INSECURE_DEPRECATE (class CCvar)
#endif
	cVar_t	*(*cvar) (char *varName, char *value, int flags);

#ifndef USE_EXTENDED_GAME_IMPORTS
	_CC_INSECURE_DEPRECATE (class CCvar)
#endif
	cVar_t	*(*cvar_set) (char *varName, char *value);

#ifndef USE_EXTENDED_GAME_IMPORTS
	_CC_INSECURE_DEPRECATE (class CCvar)
#endif
	cVar_t	*(*cvar_forceset) (char *varName, char *value);

	// ClientCommand and ServerCommand parameter access
#ifndef USE_EXTENDED_GAME_IMPORTS
	_CC_INSECURE_DEPRECATE (ArgCount)
#endif
	int		(*argc) ();

#ifndef USE_EXTENDED_GAME_IMPORTS
	_CC_INSECURE_DEPRECATE (ArgGets/ArgGeti/ArgGetf)
#endif
	char	*(*argv) (int n);

#ifndef USE_EXTENDED_GAME_IMPORTS
	_CC_INSECURE_DEPRECATE (ArgGetConcatenatedString)
#endif
	char	*(*args) ();	// concatenation of all argv >= 1

	// add commands to the server console as if they were typed in
	// for map changing, etc
	void	(*AddCommandString) (char *text);

	void	(*DebugGraph) (float value, int color);
};

#ifdef GAME_INCLUDE
extern	gameImport_t	gi;
#endif

//
// functions exported by the game subsystem
//
typedef struct gameExport_s {
	int			apiVersion;

	// the init function will only be called when a game starts,
	// not each time a level is loaded.  Persistant data for clients
	// and the server can be allocated in init
	void		(*Init) ();
	void		(*Shutdown) ();

	// each new level entered will cause a call to SpawnEntities
	void		(*SpawnEntities) (char *mapName, char *entString, char *spawnPoint);

	// Read/Write Game is for storing persistant cross level information
	// about the world state and the clients.
	// WriteGame is called every time a level is exited.
	// ReadGame is called on a loadgame.
	void		(*WriteGame) (char *fileName, BOOL autoSave);
	void		(*ReadGame) (char *fileName);

	// ReadLevel is called after the default map information has been
	// loaded with SpawnEntities
	void		(*WriteLevel) (char *filename);
	void		(*ReadLevel) (char *filename);

	BOOL		(*ClientConnect) (edict_t *ent, char *userInfo);
	void		(*ClientBegin) (edict_t *ent);
	void		(*ClientUserinfoChanged) (edict_t *ent, char *userInfo);
	void		(*ClientDisconnect) (edict_t *ent);
	void		(*ClientCommand) (edict_t *ent);
	void		(*ClientThink) (edict_t *ent, userCmd_t *cmd);

	void		(*RunFrame) ();

	// ServerCommand will be called when an "sv <command>" command is issued on the
	// server console.
	// The game can issue gi.argc() / gi.argv() commands to get the rest
	// of the parameters
	void		(*ServerCommand) ();

	//
	// global variables shared between game and server
	//

	// The edict array is allocated in the game dll so it
	// can vary in size from one game to another.
	// 
	// The size will be fixed when ge->Init() is called
	edict_t			*edicts;
	int				edictSize;
	int				numEdicts;		// current number, <= MAX_CS_EDICTS
	int				maxEdicts;
} gameExport_t;

gameExport_t *GetGameApi (gameImport_t *import);

