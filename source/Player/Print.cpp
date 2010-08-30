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

#include "Local.h"

#if !USE_EXTENDED_GAME_IMPORTS

/*
=================
SV_ClientPrintf

Sends text across to be displayed if the level passes
=================
*/
static void SV_ClientPrintf (SEntity *ent, EGamePrintLevel printLevel, const char *fmt, ...)
{
	va_list			argptr;
	CTempMemoryBlock	string = CTempHunkSystem::Allocator.GetBlock(MAX_COMPRINT);
	CPlayerEntity	*Player = entity_cast<CPlayerEntity>(ent->Entity);

	if (printLevel < Player->Client.Respawn.MessageLevel)
		return;

	va_start (argptr, fmt);
	vsnprintf (string.GetBuffer<char>(), string.GetSize() - 1, fmt, argptr);
	va_end (argptr);

	WriteByte ((printLevel != PRINT_CENTER) ? SVC_PRINT : SVC_CENTERPRINT);
	if (printLevel != PRINT_CENTER)
		WriteByte (printLevel);
	WriteString (string.GetBuffer<char>());
	Cast ((printLevel != PRINT_CENTER) ? CASTFLAG_UNRELIABLE : CASTFLAG_RELIABLE, Player);
}

void ClientPrintf (SEntity *ent, EGamePrintLevel printLevel, const char *fmt, ...)
{
	CTempMemoryBlock		msg = CTempHunkSystem::Allocator.GetBlock(MAX_COMPRINT);
	va_list		argptr;

	if (ent)
	{
		sint32 n = ent - Game.Entities;
		if (n < 1 || n > Game.MaxClients)
		{
			DebugPrintf ("CleanCode Warning: ClientPrintf to a non-client\n");
			return;
		}
	}

	// Evaluate args
	va_start (argptr, fmt);
	vsnprintf (msg.GetBuffer<char>(), msg.GetSize() - 1, fmt, argptr);
	va_end (argptr);

	// Print
	if (ent)
		SV_ClientPrintf (ent, printLevel, "%s", msg.GetBuffer<char>());
	else
		ServerPrintf ("%s", msg.GetBuffer<char>());
}

void DeveloperPrintf (const char *fmt, ...)
{
	if (!CvarList[CV_DEVELOPER].Integer())
		return;

	va_list		argptr;
	CTempMemoryBlock		text = CTempHunkSystem::Allocator.GetBlock(MAX_COMPRINT);

	va_start (argptr, fmt);
	vsnprintf (text.GetBuffer<char>(), text.GetSize() - 1, fmt, argptr);
	va_end (argptr);

CC_DISABLE_DEPRECATION
	gi.dprintf ("%s", text.GetBuffer<char>());
CC_ENABLE_DEPRECATION

	CC_OutputDebugString (text.GetBuffer<char>());
}

// Dprintf is the only command that has to be the same, because of Com_ConPrintf (we don't have it)
void DebugPrintf (const char *fmt, ...)
{
#ifdef _DEBUG
	va_list		argptr;
	CTempMemoryBlock	text = CTempHunkSystem::Allocator.GetBlock(MAX_COMPRINT);

	va_start (argptr, fmt);
	vsnprintf (text.GetBuffer<char>(), text.GetSize() - 1, fmt, argptr);
	va_end (argptr);

CC_DISABLE_DEPRECATION
	gi.dprintf ("%s", text.GetBuffer<char>());
CC_ENABLE_DEPRECATION

	CC_OutputDebugString (text.GetBuffer<char>());
#endif
}

void ServerPrintf (const char *fmt, ...)
{
	va_list		argptr;
	CTempMemoryBlock	text = CTempHunkSystem::Allocator.GetBlock(MAX_COMPRINT);

	va_start (argptr, fmt);
	vsnprintf (text.GetBuffer<char>(), text.GetSize() - 1, fmt, argptr);
	va_end (argptr);

CC_DISABLE_DEPRECATION
	gi.dprintf ("%s", text.GetBuffer<char>());
CC_ENABLE_DEPRECATION

	CC_OutputDebugString (text.GetBuffer<char>());
}

void BroadcastPrintf (EGamePrintLevel printLevel, const char *fmt, ...)
{
	va_list		argptr;
	CTempMemoryBlock	string = CTempHunkSystem::Allocator.GetBlock(MAX_COMPRINT);

	va_start (argptr, fmt);
	vsnprintf (string.GetBuffer<char>(), string.GetSize() - 1, fmt, argptr);
	va_end (argptr);
	
	for (sint32 i = 1; i <= Game.MaxClients; i++)
	{
		CPlayerEntity *Player = entity_cast<CPlayerEntity>(Game.Entities[i].Entity);
		if (printLevel < Player->Client.Respawn.MessageLevel)
			continue;
		if (Player->Client.Persistent.State != SVCS_SPAWNED)
			continue;

		WriteByte ((printLevel != PRINT_CENTER) ? SVC_PRINT : SVC_CENTERPRINT);

		if (printLevel != PRINT_CENTER)
			WriteByte (printLevel);

		WriteString (string.GetBuffer<char>());

		Cast (CASTFLAG_UNRELIABLE, Player);
	}

	// Echo to console
	if (CvarList[CV_DEDICATED].Integer())
	{
		// Mask off high bits
		for (size_t i = 0; i < string.GetSize() && string.GetBuffer<char>()[i]; i++)
			string.GetBuffer<char>()[i] &= 127;

		ServerPrintf ("%s", string.GetBuffer<char>());
	}
}
#else
void ClientPrintf (SEntity *ent, EGamePrintLevel printLevel, const char *fmt, ...)
{
	va_list		argptr;
	CTempMemoryBlock string = CTempHunkSystem::Allocator.GetBlock(MAX_COMPRINT);

	va_start (argptr, fmt);
	vsnprintf (string.GetBuffer<char>(), string.GetSize() - 1, fmt, argptr);
	va_end (argptr);
	
	if (printLevel == PRINT_CENTER)
		gi.centerprintf (ent, "%s", string.GetBuffer<char>());
	else
		gi.cprintf (ent, printLevel, "%s", string.GetBuffer<char>());
}

void DeveloperPrintf (const char *fmt, ...)
{
	va_list		argptr;
	CTempMemoryBlock string = CTempHunkSystem::Allocator.GetBlock(MAX_COMPRINT);

	va_start (argptr, fmt);
	vsnprintf (string.GetBuffer<char>(), string.GetSize() - 1, fmt, argptr);
	va_end (argptr);
	
	gi.dprintf ("%s", string.GetBuffer<char>());
}

void DebugPrintf (const char *fmt, ...)
{
	va_list		argptr;
	CTempMemoryBlock string = CTempHunkSystem::Allocator.GetBlock(MAX_COMPRINT);

	va_start (argptr, fmt);
	vsnprintf (string.GetBuffer<char>(), string.GetSize() - 1, fmt, argptr);
	va_end (argptr);
	
	gi.dprintf ("%s", string.GetBuffer<char>());
}

void BroadcastPrintf (EGamePrintLevel printLevel, const char *fmt, ...)
{
	va_list		argptr;
	CTempMemoryBlock string = CTempHunkSystem::Allocator.GetBlock(MAX_COMPRINT);

	va_start (argptr, fmt);
	vsnprintf (string.GetBuffer<char>(), string.GetSize() - 1, fmt, argptr);
	va_end (argptr);
	
	gi.bprintf (printLevel, "%s", string.GetBuffer<char>());
}

void ServerPrintf (const char *fmt, ...)
{
	va_list		argptr;
	CTempMemoryBlock string = CTempHunkSystem::Allocator.GetBlock(MAX_COMPRINT);

	va_start (argptr, fmt);
	vsnprintf (string.GetBuffer<char>(), string.GetSize() - 1, fmt, argptr);
	va_end (argptr);
	
	gi.dprintf ("%s", string.GetBuffer<char>());
}

#endif
