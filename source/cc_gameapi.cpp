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
// cc_gameapi.cpp
// Contains overloads and other neat things
//

#include "cc_local.h"
#include "cc_game.h"
#include "cc_exceptionhandler.h"
#include "cc_cmds.h"
#include "cc_gamecommands.h"
#include "cc_servercommands.h"
#include "cc_save.h"

gameImport_t	gi;

EBrushContents PointContents (vec3f &start)
{
_CC_DISABLE_DEPRECATION
	return gi.pointcontents(start);
_CC_ENABLE_DEPRECATION};

int BoxEdicts (vec3f &mins, vec3f &maxs, edict_t **list, int maxCount, bool triggers)
{
_CC_DISABLE_DEPRECATION
	return gi.BoxEdicts (mins, maxs, list, maxCount, (triggers) ? AREA_TRIGGERS : AREA_SOLID);
_CC_ENABLE_DEPRECATION
}

void ConfigString (int configStringIndex, const char *configStringValue, CPlayerEntity *Audience)
{
	if (Audience)
	{
		WriteChar (SVC_CONFIGSTRING);
		WriteShort (configStringIndex);
		WriteString (const_cast<char*>(configStringValue));
		Audience->CastTo (CASTFLAG_UNRELIABLE);
	}
	else
	{
_CC_DISABLE_DEPRECATION
		gi.configstring (configStringIndex, const_cast<char*>(configStringValue));
_CC_ENABLE_DEPRECATION
	}
}

void GameError (char *fmt, ...)
{
	va_list		argptr;
	static char	text[MAX_COMPRINT];

	va_start (argptr, fmt);
	vsnprintf_s (text, sizeof(text), MAX_COMPRINT, fmt, argptr);
	va_end (argptr);

	CC_ReportGameError (text);

_CC_DISABLE_DEPRECATION
	gi.error (text);
_CC_ENABLE_DEPRECATION
}

//
//
// Main Game API
//
//

/*
===========
ShutdownGame

called when the game is being unloaded.
============
*/
void ShutdownGame ()
{
#ifdef CC_USE_EXCEPTION_HANDLER
CC_EXCEPTION_HANDLER_BEGIN
#endif

	CC_ShutdownGame ();

#ifdef CC_USE_EXCEPTION_HANDLER
CC_EXCEPTION_HANDLER_END
#endif
}


#ifndef _FRONTEND

/*
===========
ClientBegin

called when a client has finished connecting, and is ready
to be placed into the game.  This will happen every level load.
============
*/
void ClientBegin (edict_t *ent)
{
#ifdef CC_USE_EXCEPTION_HANDLER
CC_EXCEPTION_HANDLER_BEGIN
#endif

	entity_cast<CPlayerEntity>(ent->Entity)->Begin ();

#ifdef CC_USE_EXCEPTION_HANDLER
CC_EXCEPTION_HANDLER_END
#endif
}

/*
===========
ClientUserInfoChanged

called whenever the player updates a userinfo variable.

The game can override any of the settings in place
(forcing skins or names, etc) before copying it off.
============
*/
void ClientUserinfoChanged (edict_t *ent, char *userinfo)
{
#ifdef CC_USE_EXCEPTION_HANDLER
CC_EXCEPTION_HANDLER_BEGIN
#endif

	entity_cast<CPlayerEntity>(ent->Entity)->UserinfoChanged (userinfo);

#ifdef CC_USE_EXCEPTION_HANDLER
CC_EXCEPTION_HANDLER_END
#endif
}


/*
===========
ClientConnect

Called when a player begins connecting to the server.
The game can refuse entrance to a client by returning false.
If the client is allowed, the connection process will continue
and eventually get to ClientBegin()
Changing levels will NOT cause this to be called again, but
loadgames will.
============
*/
BOOL ClientConnect (edict_t *ent, char *userinfo)
{
#ifdef CC_USE_EXCEPTION_HANDLER
CC_EXCEPTION_HANDLER_BEGIN
#endif

	return (entity_cast<CPlayerEntity>(ent->Entity)->Connect(userinfo)) ? 1 : 0;

#ifdef CC_USE_EXCEPTION_HANDLER
CC_EXCEPTION_HANDLER_END_CUSTOM
(
	return 0;
)
#endif
}

/*
===========
ClientDisconnect

Called when a player drops from the server.
Will not be called between levels.
============
*/
void ClientDisconnect (edict_t *ent)
{
#ifdef CC_USE_EXCEPTION_HANDLER
CC_EXCEPTION_HANDLER_BEGIN
#endif

	entity_cast<CPlayerEntity>(ent->Entity)->Disconnect();

#ifdef CC_USE_EXCEPTION_HANDLER
CC_EXCEPTION_HANDLER_END
#endif
}


//==============================================================

/*
==============
ClientThink

This will be called once for each client frame, which will
usually be a couple times for each server frame.
==============
*/
void ClientThink (edict_t *ent, userCmd_t *ucmd)
{
#ifdef CC_USE_EXCEPTION_HANDLER
CC_EXCEPTION_HANDLER_BEGIN
#endif

(entity_cast<CPlayerEntity>(ent->Entity))->ClientThink (ucmd);

#ifdef CC_USE_EXCEPTION_HANDLER
CC_EXCEPTION_HANDLER_END
#endif
}

/*
================
RunFrame

Advances the world by 0.1 seconds
================
*/
void RunFrame ()
{
#ifdef CC_USE_EXCEPTION_HANDLER
CC_EXCEPTION_HANDLER_BEGIN
#endif

	CC_RunFrame ();

#ifdef CC_USE_EXCEPTION_HANDLER
CC_EXCEPTION_HANDLER_END
#endif
}

/*
============
InitGame

This will be called when the dll is first loaded, which
only happens when a new game is started or a save game
is loaded.
============
*/
void InitGame ()
{
#ifdef CC_USE_EXCEPTION_HANDLER
CC_EXCEPTION_HANDLER_BEGIN
#endif

	CC_InitGame ();

#ifdef CC_USE_EXCEPTION_HANDLER
CC_EXCEPTION_HANDLER_END
#endif
}

/*
==============
SpawnEntities

Creates a server's entity / program execution context by
parsing textual entity definitions out of an ent file.
==============
*/
void SpawnEntities (char *ServerLevelName, char *entities, char *spawnpoint)
{
#ifdef CC_USE_EXCEPTION_HANDLER
CC_EXCEPTION_HANDLER_BEGIN
#endif

	CC_SpawnEntities (ServerLevelName, entities, spawnpoint);

#ifdef CC_USE_EXCEPTION_HANDLER
CC_EXCEPTION_HANDLER_END
#endif
}

/*
=================
ServerCommand

ServerCommand will be called when an "sv" command is issued.
The game can issue gi.argc() / gi.argv() commands to get the rest
of the parameters
=================
*/
void ServerCommand ()
{
#ifdef CC_USE_EXCEPTION_HANDLER
CC_EXCEPTION_HANDLER_BEGIN
#endif

	CC_ServerCommand ();

#ifdef CC_USE_EXCEPTION_HANDLER
CC_EXCEPTION_HANDLER_END
#endif
}

/*
=================
ClientCommand

game commands issued by clients
=================
*/
void ClientCommand (edict_t *ent)
{
#ifdef CC_USE_EXCEPTION_HANDLER
CC_EXCEPTION_HANDLER_BEGIN
#endif
	if (!ent->client || !ent->Entity)
		return;		// not fully in game yet

	CC_ClientCommand (entity_cast<CPlayerEntity>(ent->Entity));

#ifdef CC_USE_EXCEPTION_HANDLER
CC_EXCEPTION_HANDLER_END
#endif
}

/*
=================
GetGameAPI

Returns a pointer to the structure with all entry points
and global variables
=================
*/

gameExport_t	globals =
{
	GAME_APIVERSION,
	InitGame,
	ShutdownGame,
	SpawnEntities,
	WriteGame,
	ReadGame,
	WriteLevel,
	ReadLevel,
	ClientConnect,
	ClientBegin,
	ClientUserinfoChanged,
	ClientDisconnect,
	ClientCommand,
	ClientThink,
	RunFrame,
	ServerCommand,
	NULL,
	sizeof(edict_t),
	0,
	0
};

gameExport_t *GetGameAPI (gameImport_t *import)
{
	gi = *import;

	Swap_Init ();
	return &globals;
}
#endif

#ifndef GAME_HARD_LINKED
// this is only here so the functions in q_shared.c and q_shwin.c can link

void Com_Printf (EComPrint flags, char *fmt, ...)
{
	va_list		argptr;
	static char	text[MAX_COMPRINT];

	va_start (argptr, fmt);
	vsnprintf_s (text, sizeof(text), MAX_COMPRINT, fmt, argptr);
	va_end (argptr);

	DebugPrintf ("%s", text);
}

void Com_DevPrintf (EComPrint flags, char *fmt, ...)
{
	va_list		argptr;
	static char	text[MAX_COMPRINT];

	va_start (argptr, fmt);
	vsnprintf_s (text, sizeof(text), MAX_COMPRINT, fmt, argptr);
	va_end (argptr);

	DebugPrintf ("%s", text);
}

// FIXME code is ignored
void Com_Error (EComErrorType code, char *fmt, ...)
{
	va_list		argptr;
	static char	text[MAX_COMPRINT];

	va_start (argptr, fmt);
	vsnprintf_s (text, sizeof(text), MAX_COMPRINT, fmt, argptr);
	va_end (argptr);

_CC_DISABLE_DEPRECATION
	gi.error ("%s", text);
_CC_ENABLE_DEPRECATION
}

#endif
