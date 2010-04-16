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

void CPlayerCommand::Run (CPlayerEntity *Player)
{
	if ((Flags & CMD_CHEAT) && !Game.CheatsEnabled && (Game.GameMode != GAME_SINGLEPLAYER))
	{
		Player->PrintToClient (PRINT_HIGH, "Cheats must be enabled to use this command.\n");
		return;
	}
	if (!(Flags & CMD_SPECTATOR) && (Player->Client.Respawn.Spectator || Player->Client.Chase.Target))	
		return;

	Func (Player);
};

typedef CCommand<TPlayerCommandFunctorType>::TCommandListType TPlayerCommandListType;
typedef CCommand<TPlayerCommandFunctorType>::THashedCommandListType THashedPlayerCommandListType;

TPlayerCommandListType &CommandList ()
{
	static TPlayerCommandListType CommandListV;
	return CommandListV;
};
THashedPlayerCommandListType &CommandHashList ()
{
	static THashedPlayerCommandListType CommandHashListV;
	return CommandHashListV;
};

CPlayerCommand *Cmd_FindCommand (const char *commandName)
{
	return FindCommand <CPlayerCommand, TPlayerCommandListType, THashedPlayerCommandListType, THashedPlayerCommandListType::iterator> (commandName, CommandList(), CommandHashList());
}

CPlayerCommand &Cmd_AddCommand (const char *commandName, void (*Func) (CPlayerEntity *Player), ECmdTypeFlags Flags)
{
	// Make sure the function doesn't already exist
	if (_CC_ASSERT_EXPR (!Cmd_FindCommand(commandName), "Tried to re-add a command, fatal error"))
		return *static_cast<CPlayerCommand*>(CommandList()[0]);

	// We can add it!
	CommandList().push_back (QNew (TAG_GAME) CPlayerCommand (commandName, Func, Flags));

	// Link it in the hash tree
	CommandHashList().insert (std::make_pair<size_t, size_t> (Com_HashGeneric (commandName, MAX_CMD_HASH), CommandList().size()-1));

	return *static_cast<CPlayerCommand*>(CommandList()[CommandList().size()-1]);
}

void Cmd_RemoveCommands ()
{
	// Remove all commands
	for (uint32 i = 0; i < CommandList().size(); i++)
		QDelete CommandList().at(i);
	CommandList().clear ();
}

void Cmd_RunCommand (const char *commandName, CPlayerEntity *Player)
{
	static CPlayerCommand *Command;

	if ((Command = Cmd_FindCommand(commandName)) != NULL)
		Command->Run(Player);
	else
		Player->PrintToClient (PRINT_HIGH, "Unknown command \"%s\"\n", commandName);
}

#include <sstream>
static std::stringstream printBuffer;

void PrintSpaces (uint32 &num)
{
	printBuffer << std::string (num, ' ');
}

void RecursiveCommandPrint (CPlayerCommand *Cmd, uint32 &depth)
{
	// Print this command
	PrintSpaces (depth);
	printBuffer << Cmd->Name << "\n";

	// Print each sub-command
	depth++;
	for (uint32 i = 0; i < Cmd->SubCommands.List.size(); i++)
		RecursiveCommandPrint (static_cast<CPlayerCommand*>(Cmd->SubCommands.List[i]), depth);
	depth--;
}

void SearchForRandomMonster (CMonsterEntity *Entity)
{
	static std::vector <CMonsterEntity *> ChosenMonsters;
	for (TEntitiesContainer::iterator it = Level.Entities.Closed.begin(); it != Level.Entities.Closed.end(); ++it)
	{
		edict_t *ent = (*it);

		if (!ent->inUse || !ent->Entity)
			continue;
		if (!(ent->Entity->EntityFlags & ENT_MONSTER))
			continue;
		if (ent->Entity == Entity)
			continue;
		if (!IsVisible(ent->Entity, Entity))
			continue;

		CMonsterEntity *WantedMonster = entity_cast<CMonsterEntity>(ent->Entity);
		if (WantedMonster->Health <= 0)
			continue;
		if (!(WantedMonster->GetSvFlags() & SVF_MONSTER))
			continue;

		ChosenMonsters.push_back (WantedMonster);
	}

	// Pick a random one
	if (!ChosenMonsters.size())
		return;

	CMonsterEntity *RandomPick = ChosenMonsters[irandom(ChosenMonsters.size())];
	Entity->Enemy = RandomPick;
	Entity->Monster->FoundTarget ();
	if (Entity->Monster->MonsterFlags & MF_HAS_SIGHT)
		Entity->Monster->Sight ();

	ChosenMonsters.clear ();
}

void Cmd_Test_f (CPlayerEntity *Player)
{
//	CRogueDefenderSphere::Create(Player, 0);
}

void Cmd_Two_t (CPlayerEntity *Player)
{
	for (TEntitiesContainer::iterator it = Level.Entities.Closed.begin(); it != Level.Entities.Closed.end(); ++it)
	{
		if (!(*it)->Entity || !((*it)->Entity->EntityFlags & ENT_MONSTER))
			continue;

		CMonsterEntity *Monster = entity_cast<CMonsterEntity>((*it)->Entity);
		if (Monster->Health <= 0)
			continue;

		SearchForRandomMonster (Monster);
	}
}

void Cmd_Three_t (CPlayerEntity *Player)
{
	ServerPrintf ("Three\n");
}

void Cmd_Four_t (CPlayerEntity *Player)
{
	ServerPrintf ("Four\n");
}

void AddTestDebugCommands ()
{
	Cmd_AddCommand ("test",					Cmd_Test_f)
		.AddSubCommand ("two",				Cmd_Two_t, 0)
			.AddSubCommand ("three",		Cmd_Three_t, 0).GoUp().GoUp()
		.AddSubCommand ("four",				Cmd_Four_t, 0);
}
