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
// cc_gameapi.h
// Contains overloads and other neat things
//

#if !defined(CC_GUARD_GAMEAPI_H) || !INCLUDE_GUARDS
#define CC_GUARD_GAMEAPI_H

// define GAME_INCLUDE so that cc_game.h does not define the
// short, server-visible gclient_t and edict_t structures,
// because we define the full size ones in this file
#define GAME_INCLUDE
#include "cc_game.h"

#define GAME_APIVERSION		3

//
// functions provided by the main engine
//
struct gameImport_t
{
	// special messages
#if !USE_EXTENDED_GAME_IMPORTS
	_CC_INSECURE_DEPRECATE (BroadcastPrintf)
#endif
	void	(*bprintf) (EGamePrintLevel printlevel, char *fmt, ...);
#if !USE_EXTENDED_GAME_IMPORTS
	_CC_INSECURE_DEPRECATE (ServerPrintf or DebugPrintf)
#endif
	void	(*dprintf) (char *fmt, ...);
#if !USE_EXTENDED_GAME_IMPORTS
	_CC_INSECURE_DEPRECATE (ClientPrintf)
#endif
	void	(*cprintf) (edict_t *ent, EGamePrintLevel printLevel, char *fmt, ...);
#if !USE_EXTENDED_GAME_IMPORTS
	_CC_INSECURE_DEPRECATE (CenterPrintf)
#endif
	void	(*centerprintf) (edict_t *ent, char *fmt, ...);

#if !USE_EXTENDED_GAME_IMPORTS
	_CC_INSECURE_DEPRECATE (PlaySoundFrom or CBaseEntity->PlaySound)
#endif
	void	(*sound) (edict_t *ent, sint32 channel, sint32 soundIndex, float volume, float attenuation, float timeOffset);
#if !USE_EXTENDED_GAME_IMPORTS
	_CC_INSECURE_DEPRECATE (PlaySoundAt or CBaseEntity->PlayPositionedSound)
#endif
	void	(*positioned_sound) (vec3_t origin, edict_t *ent, sint32 channel, sint32 soundIndex, float volume, float attenuation, float timeOffset);

	// config strings hold all the index strings, the lightstyles,
	// and misc data like the sky definition and cdtrack.
	// All of the current configstrings are sent to clients when
	// they connect, and changes are sent to all connected clients.
#if !USE_EXTENDED_GAME_IMPORTS
	_CC_INSECURE_DEPRECATE (ConfigString)
#endif
	void	(*configstring) (sint32 num, char *string);

#if !USE_EXTENDED_GAME_IMPORTS
	_CC_INSECURE_DEPRECATE (GameError)
#endif
	void	(*error) (char *fmt, ...);

	// the *index functions create configstrings and some internal server state
#if !USE_EXTENDED_GAME_IMPORTS
	_CC_INSECURE_DEPRECATE (ModelIndex)
#endif
	sint32		(*modelindex) (char *name);
#if !USE_EXTENDED_GAME_IMPORTS
	_CC_INSECURE_DEPRECATE (SoundIndex)
#endif
	sint32		(*soundindex) (char *name);
#if !USE_EXTENDED_GAME_IMPORTS
	_CC_INSECURE_DEPRECATE (ImageIndex)
#endif
	sint32		(*imageindex) (char *name);

#if !USE_EXTENDED_GAME_IMPORTS
	_CC_INSECURE_DEPRECATE (SetModel)
#endif
	void	(*setmodel) (edict_t *ent, char *name);

	// collision detection
#if !USE_EXTENDED_GAME_IMPORTS
	_CC_INSECURE_DEPRECATE (class CTrace)
#endif
	cmTrace_t	(*trace) (vec3_t start, vec3_t mins, vec3_t maxs, vec3_t end, edict_t *passEnt, sint32 contentMask);
#if !USE_EXTENDED_GAME_IMPORTS
	_CC_INSECURE_DEPRECATE (PointContents)
#endif
	sint32			(*pointcontents) (vec3_t point);
#if !USE_EXTENDED_GAME_IMPORTS
	_CC_INSECURE_DEPRECATE (InArea)
#endif
	BOOL		(*inPVS) (vec3_t p1, vec3_t p2);
#if !USE_EXTENDED_GAME_IMPORTS
	_CC_INSECURE_DEPRECATE (InArea)
#endif
	BOOL		(*inPHS) (vec3_t p1, vec3_t p2);
	void		(*SetAreaPortalState) (sint32 portalNum, BOOL open);
	BOOL		(*AreasConnected) (sint32 area1, sint32 area2);

	// an entity will never be sent to a client or used for collision
	// if it is not passed to linkentity.  If the size, position, or
	// solidity changes, it must be relinked.
	/*
#if !USE_EXTENDED_GAME_IMPORTS
	_CC_INSECURE_DEPRECATE ((CBaseEntity)->Link)
#endif
	*/
		void	(*linkentity) (edict_t *ent);
	/*
#if !USE_EXTENDED_GAME_IMPORTS
	_CC_INSECURE_DEPRECATE ((CBaseEntity)->Unlink)
#endif
	*/
	void	(*unlinkentity) (edict_t *ent);		// call before removing an interactive edict
#if !USE_EXTENDED_GAME_IMPORTS
	_CC_INSECURE_DEPRECATE (BoxEdicts)
#endif
	sint32		(*BoxEdicts) (vec3_t mins, vec3_t maxs, edict_t **list,	sint32 maxCount, sint32 areaType);
#if !USE_EXTENDED_GAME_IMPORTS
	_CC_INSECURE_DEPRECATE (SV_Pmove)
#endif
	void	(*Pmove) (
#if USE_EXTENDED_GAME_IMPORTS
	pMove_t *pMove
#else
	pMoveNew_t *pMove // Just to supress error
#endif
	);		// player movement code common with client prediction

	// network messaging
#if !USE_EXTENDED_GAME_IMPORTS
	_CC_INSECURE_DEPRECATE (Cast)
#endif
	void	(*multicast) (vec3_t origin, EMultiCast to);

#if !USE_EXTENDED_GAME_IMPORTS
	_CC_INSECURE_DEPRECATE (Cast)
#endif
	void	(*unicast) (edict_t *ent, BOOL reliable);

#if !USE_EXTENDED_GAME_IMPORTS
	_CC_INSECURE_DEPRECATE (WriteChar)
#endif
	void	(*WriteChar) (sint32 c);

#if !USE_EXTENDED_GAME_IMPORTS
	_CC_INSECURE_DEPRECATE (WriteByte)
#endif
	void	(*WriteByte) (sint32 c);

#if !USE_EXTENDED_GAME_IMPORTS
	_CC_INSECURE_DEPRECATE (WriteShort)
#endif
	void	(*WriteShort) (sint32 c);

#if !USE_EXTENDED_GAME_IMPORTS
	_CC_INSECURE_DEPRECATE (WriteLong)
#endif
	void	(*WriteLong) (sint32 c);

#if !USE_EXTENDED_GAME_IMPORTS
	_CC_INSECURE_DEPRECATE (WriteFloat)
#endif
	void	(*WriteFloat) (float f);

#if !USE_EXTENDED_GAME_IMPORTS
	_CC_INSECURE_DEPRECATE (WriteString)
#endif
	void	(*WriteString) (char *s);

#if !USE_EXTENDED_GAME_IMPORTS
	_CC_INSECURE_DEPRECATE (WritePosition)
#endif
	void	(*WritePosition) (vec3_t pos);	// some fractional bits

#if !USE_EXTENDED_GAME_IMPORTS
	_CC_INSECURE_DEPRECATE (WriteDir)
#endif
	void	(*WriteDir) (vec3_t pos);		// single uint8 encoded, very coarse

#if !USE_EXTENDED_GAME_IMPORTS
	_CC_INSECURE_DEPRECATE (WriteAngle)
#endif
	void	(*WriteAngle) (float f);

	// managed memory allocation
#if !USE_EXTENDED_GAME_IMPORTS
	_CC_INSECURE_DEPRECATE (QNew)
#endif
	void	*(*TagMalloc) (sint32 size, sint32 tag);
#if !USE_EXTENDED_GAME_IMPORTS
	_CC_INSECURE_DEPRECATE (QDelete)
#endif
	void	(*TagFree) (void *block);
#if !USE_EXTENDED_GAME_IMPORTS
	_CC_INSECURE_DEPRECATE (Mem_FreePool)
#endif
	void	(*FreeTags) (sint32 tag);

	// console variable interaction
#if !USE_EXTENDED_GAME_IMPORTS
	_CC_INSECURE_DEPRECATE (class CCvar)
#endif
	cVar_t	*(*cvar) (char *varName, char *value, sint32 flags);

#if !USE_EXTENDED_GAME_IMPORTS
	_CC_INSECURE_DEPRECATE (class CCvar)
#endif
	cVar_t	*(*cvar_set) (char *varName, char *value);

#if !USE_EXTENDED_GAME_IMPORTS
	_CC_INSECURE_DEPRECATE (class CCvar)
#endif
	cVar_t	*(*cvar_forceset) (char *varName, char *value);

	// ClientCommand and ServerCommand parameter access
#if !USE_EXTENDED_GAME_IMPORTS
	_CC_INSECURE_DEPRECATE (ArgCount)
#endif
	sint32		(*argc) ();

#if !USE_EXTENDED_GAME_IMPORTS
	_CC_INSECURE_DEPRECATE (ArgGets/ArgGeti/ArgGetf)
#endif
	char	*(*argv) (sint32 n);

#if !USE_EXTENDED_GAME_IMPORTS
	_CC_INSECURE_DEPRECATE (ArgGetConcatenatedString)
#endif
	char	*(*args) ();	// concatenation of all argv >= 1

	// add commands to the server console as if they were typed in
	// for map changing, etc
	void	(*AddCommandString) (char *text);

	void	(*DebugGraph) (float value, sint32 color);
};

#ifdef GAME_INCLUDE
extern	gameImport_t	gi;
#endif

// C++ wrapper
class CGameAPI
{
public:
	CGameAPI () {}
	virtual ~CGameAPI () {}

	virtual void Init ();
	virtual void Shutdown ();

	virtual void SpawnEntities (char *mapName, char *entString, char *spawnPoint);
	
	virtual void WriteGame (char *fileName, bool autosave);
	virtual void ReadGame (char *fileName);
	virtual void WriteLevel (char *fileName);
	virtual void ReadLevel (char *fileName);

	virtual bool ClientConnect (CPlayerEntity *Player, char *userinfo);
	virtual void ClientBegin (CPlayerEntity *Player);
	virtual void ClientUserinfoChanged (CPlayerEntity *Player, char *userinfo);
	virtual void ClientDisconnect (CPlayerEntity *Player);
	virtual void ClientCommand (CPlayerEntity *Player);
	virtual void ClientThink (CPlayerEntity *Player, userCmd_t *cmd);

	virtual void RunFrame ();

	virtual void ServerCommand ();

	edict_t *GetEntities ();
	void SetEntities (edict_t *);
	sint32 &GetEdictSize ();
	sint32 &GetNumEdicts();
	sint32 &GetMaxEdicts();
};

// If you make a new class for a game API, replace this.
#define GAME_CLASS	CGameAPI

extern GAME_CLASS	GameAPI;

CC_ENUM (uint8, ECastType)
{
	// Cast type
	CAST_MULTI,
	CAST_UNI
};

CC_ENUM (uint8, ECastFlags)
{
	CASTFLAG_UNRELIABLE = 0,
	CASTFLAG_PVS = 1,
	CASTFLAG_PHS = 2,
	CASTFLAG_RELIABLE = 4
};

void Cast (ECastFlags castFlags, CBaseEntity *Ent);
void Cast (ECastFlags castFlags, vec3f &Origin);

EBrushContents PointContents (vec3f &start);

sint32 BoxEdicts (vec3f &mins, vec3f &maxs, edict_t **list, sint32 maxCount, bool triggers);

void ConfigString (sint32 configStringIndex, const char *configStringValue, CPlayerEntity *Audience = NULL);
void GameError (char *fmt, ...);

_CC_DISABLE_DEPRECATION

// Hearable false = PVS, otherwise PHS
inline bool InArea (bool Hearable, vec3f &p1, vec3f &p2)
{
	if (Hearable)
		return (gi.inPHS (p1, p2) == 1);
	return (gi.inPVS (p1, p2) == 1);
}

inline bool InHearableArea (vec3f &p1, vec3f &p2) { return InArea (true, p1, p2); }
inline bool InVisibleArea (vec3f &p1, vec3f &p2) { return InArea (false, p1, p2); }

_CC_ENABLE_DEPRECATION

#else
FILE_WARNING
#endif
