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
// cc_cmds.cpp
// Contains a class and functions to register/remove commands parsed by ClientCommand.
//

#include "cc_local.h"
#include "cc_cmds.h"
#include "cc_cmds_local.h"

void CPlayerCommand::Run (CPlayerEntity *ent)
{
	if ((Flags & CMD_CHEAT) && !game.cheats && (game.mode != GAME_SINGLEPLAYER))
	{
		ent->PrintToClient (PRINT_HIGH, "Cheats must be enabled to use this command.\n");
		return;
	}
	if (!(Flags & CMD_SPECTATOR) && (ent->Client.Respawn.Spectator || ent->Client.Chase.Target))	
		return;

	Func (ent);
};

typedef std::multimap<size_t, size_t, std::less<size_t>, std::game_allocator<size_t> > THashedPlayerCommandListType;
typedef std::vector<CPlayerCommand*, std::game_allocator<CPlayerCommand*> > TPlayerCommandListType;

TPlayerCommandListType CommandList;
THashedPlayerCommandListType CommandHashList;

CPlayerCommand *Cmd_FindCommand (std::cc_string commandName)
{
	return FindCommand <CPlayerCommand, TPlayerCommandListType, THashedPlayerCommandListType, CommandList, CommandHashList> (commandName);
}

void Cmd_AddCommand (std::cc_string commandName, void (*Func) (CPlayerEntity *ent), ECmdTypeFlags Flags)
{
	// Make sure the function doesn't already exist
	if (Cmd_FindCommand(commandName))
	{
		Com_Printf (0, "%s already exists as a command!\n", commandName);
		return;
	}

	// We can add it!
	CommandList.push_back (QNew (com_gamePool, 0) CPlayerCommand (commandName, Func, Flags));

	// Link it in the hash tree
	CommandHashList.insert (std::make_pair<size_t, size_t> (Com_HashGeneric (commandName, MAX_CMD_HASH), CommandList.size()-1));
}

void Cmd_RemoveCommands ()
{
	// Remove all commands
	for (uint32 i = 0; i < CommandList.size(); i++)
		QDelete CommandList.at(i);
	CommandList.clear ();
}

void Cmd_RunCommand (std::cc_string commandName, CPlayerEntity *ent)
{
	static CPlayerCommand *Command;

	if ((Command = Cmd_FindCommand(commandName)) != NULL)
		Command->Run(ent);
	else
		ent->PrintToClient (PRINT_HIGH, "Unknown command \"%s\"\n", commandName.c_str());
}