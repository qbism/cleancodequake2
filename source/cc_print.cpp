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
// cc_print.cpp
// Print replacements
//

#include "cc_local.h"

#if !USE_EXTENDED_GAME_IMPORTS

/*
=================
SV_ClientPrintf

Sends text across to be displayed if the level passes
=================
*/
static void SV_ClientPrintf (edict_t *ent, EGamePrintLevel printLevel, char *fmt, ...)
{
	va_list		argptr;
	static char	string[MAX_COMPRINT];
	CPlayerEntity *Player = entity_cast<CPlayerEntity>(ent->Entity);

	if (printLevel < Player->Client.Respawn.MessageLevel)
		return;

	va_start (argptr, fmt);
	vsnprintf_s (string, sizeof(string), sizeof(string), fmt, argptr);
	va_end (argptr);

	WriteByte ((printLevel != PRINT_CENTER) ? SVC_PRINT : SVC_CENTERPRINT);
	if (printLevel != PRINT_CENTER)
		WriteByte (printLevel);
	WriteString (string);
	Cast ((printLevel != PRINT_CENTER) ? CASTFLAG_UNRELIABLE : CASTFLAG_RELIABLE, Player);
}

void ClientPrintf (edict_t *ent, EGamePrintLevel printLevel, char *fmt, ...)
{
	static char	msg[MAX_COMPRINT];
	va_list		argptr;

	if (ent)
	{
		sint32 n = ent - g_edicts;
		if (n < 1 || n > game.MaxClients)
		{
			DebugPrintf ( "CleanCode Warning: ClientPrintf to a non-client\n");
			return;
		}
	}

	// Evaluate args
	va_start (argptr, fmt);
	vsnprintf_s (msg, sizeof(msg), sizeof(msg), fmt, argptr);
	va_end (argptr);

	// Print
	if (ent)
		SV_ClientPrintf (ent, printLevel, "%s", msg);
	else
		DebugPrintf ( "%s", msg);
}

void DeveloperPrintf (char *fmt, ...)
{
	if (!developer->Integer())
		return;

	va_list		argptr;
	static char	text[MAX_COMPRINT];

	va_start (argptr, fmt);
	vsnprintf_s (text, sizeof(text), MAX_COMPRINT, fmt, argptr);
	va_end (argptr);

_CC_DISABLE_DEPRECATION
	gi.dprintf ("%s", text);
_CC_ENABLE_DEPRECATION

	CC_OutputDebugString (text);
}

// Dprintf is the only command that has to be the same, because of Com_ConPrintf (we don't have it)
void DebugPrintf (char *fmt, ...)
{
	va_list		argptr;
	static char	text[MAX_COMPRINT];

	va_start (argptr, fmt);
	vsnprintf_s (text, sizeof(text), MAX_COMPRINT, fmt, argptr);
	va_end (argptr);

_CC_DISABLE_DEPRECATION
	gi.dprintf ("%s", text);
_CC_ENABLE_DEPRECATION

	CC_OutputDebugString (text);
}

void BroadcastPrintf (EGamePrintLevel printLevel, char *fmt, ...)
{
	va_list		argptr;
	static char	string[MAX_COMPRINT];

	va_start (argptr, fmt);
	vsnprintf_s (string, sizeof(string), sizeof(string), fmt, argptr);
	va_end (argptr);
	
	// Echo to console
	if (dedicated->Integer())
	{
		static char	copy[1024];
		sint32		i;
		
		// Mask off high bits
		for (i = 0; i < ((MAX_COMPRINT/2) - 1) && string[i]; i++)
			copy[i] = string[i]&127;
		copy[i] = 0;
		DebugPrintf ( "%s", copy);
	}

	for (sint32 i = 1; i <= game.MaxClients; i++)
	{
		CPlayerEntity *Player = entity_cast<CPlayerEntity>(g_edicts[i].Entity);
		if (printLevel < Player->Client.Respawn.MessageLevel)
			continue;
		if (Player->Client.Persistent.State != SVCS_SPAWNED)
			continue;


		WriteByte ((printLevel != PRINT_CENTER) ? SVC_PRINT : SVC_CENTERPRINT);
		if (printLevel != PRINT_CENTER)
			WriteByte (printLevel);
		WriteString (string);
		Cast (CASTFLAG_UNRELIABLE, Player);
	}
}
#else
void ClientPrintf (edict_t *ent, EGamePrintLevel printLevel, char *fmt, ...)
{
	va_list		argptr;
	static char	string[MAX_COMPRINT];

	va_start (argptr, fmt);
	vsnprintf_s (string, sizeof(string), sizeof(string), fmt, argptr);
	va_end (argptr);
	
	if (printLevel == PRINT_CENTER)
		gi.centerprintf (ent, "%s", string);
	else
		gi.cprintf (ent, printLevel, "%s", string);
}

void DeveloperPrintf (char *fmt, ...)
{
	va_list		argptr;
	static char	string[MAX_COMPRINT];

	va_start (argptr, fmt);
	vsnprintf_s (string, sizeof(string), sizeof(string), fmt, argptr);
	va_end (argptr);
	
	gi.dprintf ("%s", string);
}

void DebugPrintf (char *fmt, ...)
{
	va_list		argptr;
	static char	string[MAX_COMPRINT];

	va_start (argptr, fmt);
	vsnprintf_s (string, sizeof(string), sizeof(string), fmt, argptr);
	va_end (argptr);
	
	gi.dprintf ("%s", string);
}

void BroadcastPrintf (EGamePrintLevel printLevel, char *fmt, ...)
{
	va_list		argptr;
	static char	string[MAX_COMPRINT];

	va_start (argptr, fmt);
	vsnprintf_s (string, sizeof(string), sizeof(string), fmt, argptr);
	va_end (argptr);
	
	gi.bprintf (printLevel, "%s", string);
}

#endif