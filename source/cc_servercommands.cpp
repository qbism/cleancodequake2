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
// cc_servercommands.cpp
// ServerCommand & related functions
//

#include "cc_local.h"
#include "cc_exceptionhandler.h"
#include "cc_cmds.h"
#include "cc_cmds_local.h"
#include "cc_gamecommands.h"
#include "cc_servercommands.h"
#include "cc_version.h"

typedef std::multimap<size_t, size_t, std::less<size_t>, std::generic_allocator<size_t> > THashedServerCommandListType;
typedef std::vector<CServerCommand*, std::generic_allocator<CServerCommand*> > TServerCommandListType;

TServerCommandListType &ServerCommandList ()
{
	static TServerCommandListType ServerCommandListV;
	return ServerCommandListV;
};
THashedServerCommandListType &ServerCommandHashList ()
{
	static THashedServerCommandListType ServerCommandHashListV;
	return ServerCommandHashListV;
};

CServerCommand *SvCmd_FindCommand (std::cc_string commandName)
{
	return FindCommand <CServerCommand, TServerCommandListType, THashedServerCommandListType> (commandName, ServerCommandList(), ServerCommandHashList());
}

void SvCmd_AddCommand (std::cc_string commandName, void (*Func) ())
{
	// Make sure the function doesn't already exist
	if (SvCmd_FindCommand(commandName))
	{
		DebugPrintf ("%s already exists as a command!\n", commandName);
		return;
	}

	// We can add it!
	ServerCommandList().push_back (QNew (com_genericPool, 0) CServerCommand (commandName, Func));

	// Link it in the hash tree
	ServerCommandHashList().insert (std::make_pair<size_t, size_t> (Com_HashGeneric (commandName, MAX_CMD_HASH), ServerCommandList().size()-1));
}

void SvCmd_RemoveCommands ()
{
	// Remove all commands
	for (uint32 i = 0; i < ServerCommandList().size(); i++)
		QDelete ServerCommandList().at(i);
	ServerCommandList().clear ();
}

void SvCmd_RunCommand (std::cc_string commandName)
{
	static CServerCommand *Command;

	if ((Command = SvCmd_FindCommand(commandName)) != NULL)
		Command->Run();
	else
		DebugPrintf ( "Unknown server command \"%s\"\n", commandName.c_str());
}

struct SServerEntityListEntity
{
	vec3f		Origin;
};

struct SServerEntityListIndex
{
	const char	*className;
	uint32		Num;
	bool		Old;
	std::vector<SServerEntityListEntity, std::generic_allocator<SServerEntityListEntity> > List;

	SServerEntityListIndex			*hashNext;
	uint32							hashValue;

	SServerEntityListIndex (const char *name) :
		className(name),
		hashValue (Com_HashGeneric(className, MAX_CS_EDICTS)),
		Num (0),
		Old (true),
		List()
	{
	};
};

class CServerEntityList
{
public:
	SServerEntityListIndex		**List;
	SServerEntityListIndex		**HashedList;
	uint32						NumInList;

	CServerEntityList () :
		NumInList (0)
	{
		List = QNew (com_genericPool, 0) SServerEntityListIndex*[MAX_CS_EDICTS];
		HashedList = QNew (com_genericPool, 0) SServerEntityListIndex*[MAX_CS_EDICTS];
		memset (List, 0, sizeof(List));
		memset (HashedList, 0, sizeof(HashedList));
	};

	~CServerEntityList ()
	{
		for (uint32 i = 0; i < NumInList; i++)
			QDelete List[i];
		QDelete[] List;
		QDelete[] HashedList;
	};

	SServerEntityListIndex *Exists (const char *className)
	{
		uint32 Hash = Com_HashGeneric (className, MAX_CS_EDICTS);

		for (SServerEntityListIndex *Index = HashedList[Hash]; Index; Index = Index->hashNext)
		{
			if (Q_stricmp(Index->className, className) == 0)
				return Index;
		}
		return NULL;
	};

	SServerEntityListIndex *AddToList (const char *className)
	{
		SServerEntityListIndex *Ind = QNew (com_genericPool, 0) SServerEntityListIndex(className);
		List[NumInList] = Ind;
		List[NumInList]->hashNext = HashedList[List[NumInList]->hashValue];
		HashedList[List[NumInList]->hashValue] = List[NumInList];
		NumInList++;

		return Ind;
	};

	void Search (const char *WildCard)
	{
		for (TEntitiesContainer::iterator it = level.Entities.Closed.begin(); it != level.Entities.Closed.end(); ++it)
		{
			edict_t *e = (*it);
			if (!e->inUse)
				continue;

			if (!Q_WildcardMatch (WildCard, e->Entity->ClassName, true))
				continue;

			SServerEntityListIndex *Index = Exists(e->Entity->ClassName);

			if (!Index)
				Index = AddToList (e->Entity->ClassName);

			if (e->Entity)
			{
				Index->Old = false;

				vec3f Origin = (e->Entity->State.GetOrigin() == vec3fOrigin && (e->Entity->GetSolid() == SOLID_BSP)) ? e->Entity->GetAbsMin() : e->Entity->State.GetOrigin();

				SServerEntityListEntity index = {Origin};
				Index->List.push_back (index);
			}
			else
				Index->Old = true;

			Index->Num++;
		}
	};
};

void SvCmd_EntList_f ()
{
	std::cc_string WildCard = (!ArgGets(2).empty()) ? ArgGets(2) : "*";
	CServerEntityList tmp;
	tmp.Search (WildCard.c_str());

	DebugPrintf ("Entity Stats\n"
			"      old        amount       origin             classname\n"
			"----  ---        ------       ----------------   --------------------\n");

	uint32 oldCount = 0, newCount = 0;
	for (uint32 i = 0; i < tmp.NumInList; i++)
	{
		DebugPrintf (
			"#%3u  %s         %5u                          %s\n",
			i, (tmp.List[i]->Old) ? "Yes" : "No ", tmp.List[i]->Num, tmp.List[i]->className);

		for (size_t z = 0; z < tmp.List[i]->List.size(); z++)
		{
			SServerEntityListEntity &ent = tmp.List[i]->List[z];

			DebugPrintf (
			"[%2u]                         %4.0f %4.0f %4.0f\n",
			z+1, ent.Origin.X, ent.Origin.Y, ent.Origin.Z);
		}

		if (tmp.List[i]->Old)
			oldCount += tmp.List[i]->Num;
		else
			newCount += tmp.List[i]->Num;
	}

	DebugPrintf ("----  ---        ------       --------------------------\n");
	DebugPrintf ("Tally Old:       %5u          %5.0f%%\n", oldCount, (!oldCount) ? 0 : (float)oldCount / ((float)(oldCount + newCount)) * 100);
	DebugPrintf ("Tally New:       %5u          %5.0f%%\n", newCount, (!newCount) ? 0 : (float)newCount / ((float)(oldCount + newCount)) * 100);
}

extern char *gEntString;
void SvCmd_Dump_f ()
{
	CFile File ((std::cc_string("/maps/ents/") + level.ServerLevelName + ".ccent").c_str(), FILEMODE_CREATE | FILEMODE_WRITE);

	if (!File.Valid())
		return;

	File.Write (gEntString);
}

bool requestedBreak = false;
void SvCmd_Break_f ()
{
	requestedBreak = true;
}

void SvCmd_IndexList_f ();

#include "cc_ban.h"
IPAddress CopyIP (const char *val);

void SvCmd_Ban_t ()
{
	const std::cc_string str = Q_strlwr(ArgGets(2));

	if (str == "save")
		Bans.SaveList ();
	else if (str == "load")
		Bans.LoadFromFile ();
	else if (str == "list")
	{
		DebugPrintf (
			"Name/IP                   IP       Flags\n"
			"---------------------    ----      -----\n");

		for (TBanIndexContainer::iterator it = Bans.BanList.begin(); it < Bans.BanList.end(); it++)
		{
			BanIndex *Index = *it;

			if (!Index->IP)
				continue;

			DebugPrintf ("%-24s Yes         %u\n", Index->IPAddress->str, Index->Flags);
		}

		for (TBanIndexContainer::iterator it = Bans.BanList.begin(); it < Bans.BanList.end(); it++)
		{
			BanIndex *Index = *it;

			if (Index->IP)
				continue;

			DebugPrintf ("%-24s No          %u\n", Index->Name, Index->Flags);
		}
	}
	else if (str == "name")
	{
		if (ArgGets(3) == "remove")
		{
			std::cc_string name = ArgGets(4);
			if (Bans.RemoveFromList (name.c_str()))
				DebugPrintf ("Removed %s from ban list\n", name.c_str());
			else
				DebugPrintf ("%s not found in ban list\n", name.c_str());
		}
		else
		{
			std::cc_string name = ArgGets(3);
			const uint8 flags = ArgGeti(4);

			if (Bans.AddToList (name.c_str(), flags))
				DebugPrintf ("Added %s with flags %u to ban list\n", name.c_str(), flags);
			else if (Bans.InList (name.c_str()))
			{
				if (Bans.ChangeBan (name.c_str(), flags))
					DebugPrintf ("%s flags changed to %u\n", name.c_str(), flags);
				else
					DebugPrintf ("%s already has flags %u\n", name.c_str(), flags);
			}
			else
				DebugPrintf ("%s not found in ban list\n", name.c_str());
		}
	}
	else if (str == "ip")
	{
		if (ArgGets(3) == "remove")
		{
			IPAddress Ip = CopyIP(ArgGets(4).c_str());
			if (Bans.RemoveFromList (Ip))
				DebugPrintf ("Removed %s from ban list\n", Ip.str);
			else
				DebugPrintf ("%s not found in ban list\n", Ip.str);
		}
		else
		{
			IPAddress Ip = CopyIP(ArgGets(3).c_str());
			const uint8 flags = ArgGeti(4);

			if (Bans.AddToList (Ip, flags))
				DebugPrintf ("Added %s with flags %u to ban list\n", Ip.str, flags);
			else if (Bans.InList (Ip))
			{
				if (Bans.ChangeBan (Ip, flags))
					DebugPrintf ("%s flags changed to %u\n", Ip.str, flags);
				else
					DebugPrintf ("%s already has flags %u\n", Ip.str, flags);
			}
			else
				DebugPrintf ("%s not found in ban list\n", Ip.str);
		}
	}
	else
		DebugPrintf (
		"Unknown ban command \"%s\"\n"
		"List of available ban commands:\n"
		"list                       List all current bans\n"
		"save                       Saves current bans to bans.lst\n"
		"load                       Loads bans from bans.lst\n"
		"name x [remove] flags      Ban name from server\n"
		"ip x [remove] flags        Ban IP from server\n\n"
		
		"Available ban flags:\n"
		"%u       Squelch\n"
		"%u       Spectator Mode\n"
		"%u       Entering Game\n"
		, str.c_str(), BAN_SQUELCH, BAN_SPECTATOR, BAN_ENTER);
}

void SvCmd_Test_t ()
{
}

void SvCmd_Register ()
{
	SvCmd_AddCommand ("entlist",				SvCmd_EntList_f);
	SvCmd_AddCommand ("indexlist",				SvCmd_IndexList_f);
	SvCmd_AddCommand ("dump",					SvCmd_Dump_f);
	SvCmd_AddCommand ("break",					SvCmd_Break_f);
	SvCmd_AddCommand ("cc_version",				SvCmd_CCVersion_t);
	SvCmd_AddCommand ("ban",					SvCmd_Ban_t);
	SvCmd_AddCommand ("test",					SvCmd_Test_t);
}

/*
=================
ServerCommand

ServerCommand will be called when an "sv" command is issued.
The game can issue gi.argc() / gi.argv() commands to get the rest
of the parameters
=================
*/
void CGameAPI::ServerCommand ()
{
	InitArg ();
	SvCmd_RunCommand (ArgGets(1));
	EndArg ();
}

