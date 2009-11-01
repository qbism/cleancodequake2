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

void CCmd::Run (CPlayerEntity *ent)
{
	if ((CmdFlags & CMD_CHEAT) && !game.cheats && (game.mode != GAME_SINGLEPLAYER))
	{
		ent->PrintToClient (PRINT_HIGH, "Cheats must be enabled to use this command.\n");
		return;
	}
	if (!(CmdFlags & CMD_SPECTATOR) && (ent->Client.Respawn.Spectator || ent->Client.Chase.Target))	
		return;

	RunFunction (ent);
};

CCmd::CCmd (std::cc_string name, void (*Func)(CPlayerEntity *ent), ECmdTypeFlags Flags)
{
	cmdName = name;
	hashValue = Com_HashGeneric(name, MAX_CMD_HASH);
	CmdFlags = Flags;
	RunFunction = Func;
};

CCmd::~CCmd ()
{
};

CCmd *CommandList[MAX_COMMANDS];
CCmd *CommandHashList[MAX_CMD_HASH];
int numCommands = 0;

CCmd *Cmd_FindCommand (std::cc_string commandName)
{
	CCmd *Command;
	uint32 hash = Com_HashGeneric(commandName, MAX_CMD_HASH);

	for (Command = CommandHashList[hash]; Command; Command=Command->hashNext)
	{
		if (Q_stricmp (Command->cmdName.c_str(), commandName.c_str()) == 0)
			return Command;
	}
	return NULL;
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
	CommandList[numCommands] = QNew (com_gamePool, 0) CCmd (commandName, Func, Flags);

	// Link it in the hash tree
	CommandList[numCommands]->hashNext = CommandHashList[CommandList[numCommands]->hashValue];
	CommandHashList[CommandList[numCommands]->hashValue] = CommandList[numCommands];
	numCommands++;
}

void Cmd_RemoveCommands ()
{
	// Remove all commands
	for (int i = 0; i < numCommands; i++)
	{
		QDelete CommandList[numCommands];
		numCommands--;
	}
}

void Cmd_RunCommand (std::cc_string commandName, CPlayerEntity *ent)
{
	CCmd *Command = Cmd_FindCommand(commandName);

	if (Command)
		Command->Run(ent);
	else
		ent->PrintToClient (PRINT_HIGH, "Unknown command \"%s\"\n", commandName.c_str());
}