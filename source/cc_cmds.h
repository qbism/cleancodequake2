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
// cc_cmds.h
// Contains a class and functions to register/remove commands parsed by ClientCommand.
//
#if !defined(CC_GUARD_CMDS_H) || !INCLUDE_GUARDS
#define CC_GUARD_CMDS_H

#define MAX_COMMANDS 128
#define MAX_CMD_HASH (MAX_COMMANDS/4)

CC_ENUM (uint32, ECmdTypeFlags)
{
	CMD_NORMAL		= 0, // Doesn't do anything at all, just here for show.
	CMD_SPECTATOR	= 1,
	CMD_CHEAT		= 2
};

template <typename TFunctor>
class CCommand
{
public:
	typedef std::vector<CCommand<TFunctor>*> TCommandListType;
	typedef std::multimap<size_t, size_t> THashedCommandListType;

	struct SubCommands_t
	{
		CCommand<TFunctor>			*Owner; // Pointer to the command that owns us

		TCommandListType			List;
		THashedCommandListType		HashedList;
	};

	char					*Name;
	ECmdTypeFlags			Flags;
	TFunctor				Func;
	SubCommands_t			SubCommands;

	CCommand (const char *Name, TFunctor Func, ECmdTypeFlags Flags) :
	  Name(Mem_StrDup(Name)),
	  Func(Func),
	  Flags(Flags)
	  {
	  };
	
	virtual ~CCommand ()
	{
		QDelete[] Name;
	};

	CCommand &GoUp ()
	{
		if (!SubCommands.Owner)
			return *this;
		return *SubCommands.Owner;
	};

	virtual void *NewOfMe (const char *Name, TFunctor Func, ECmdTypeFlags Flags) = 0;

	virtual CCommand &AddSubCommand (const char *Name, TFunctor Func, ECmdTypeFlags Flags)
	{
		CCommand *NewCommand = (CCommand*)NewOfMe (Name, Func, Flags);

		// We can add it!
		SubCommands.List.push_back (NewCommand);

		// Link it in the hash tree
		SubCommands.HashedList.insert (std::make_pair<size_t, size_t> (Com_HashGeneric (Name, MAX_CMD_HASH), SubCommands.List.size()-1));
		SubCommands.List[SubCommands.List.size()-1]->SubCommands.Owner = this;

		return *SubCommands.List[SubCommands.List.size()-1];
	};
};

template <class TReturnValue>
inline TReturnValue *RecurseSubCommands (uint32 &depth, TReturnValue *Cmd)
{
	if (Q_stricmp (Cmd->Name, ArgGets(depth).c_str()) == 0)
	{
		depth++;
		if (ArgCount() > depth)
		{
			for (uint32 i = 0; i < Cmd->SubCommands.List.size(); i++)
			{
				TReturnValue *Found = static_cast<TReturnValue*>(RecurseSubCommands (depth, Cmd->SubCommands.List[i]));
				if (Found)
					return Found;
			}
		}
		depth--;
		return Cmd;
	}
	return NULL;
}

template <class TReturnValue, class TListType, class THashListType, class TItType>
inline TReturnValue *FindCommand (const char *commandName, TListType &List, THashListType &HashList)
{
	uint32 hash = Com_HashGeneric(commandName, MAX_CMD_HASH);

	for (TItType it = HashList.equal_range(hash).first; it != HashList.equal_range(hash).second; ++it)
	{
		TReturnValue *Command = static_cast<TReturnValue*>(List.at((*it).second));
		if (Q_stricmp (Command->Name, commandName) == 0)
		{
			if (ArgCount() > 1)
			{
				for (uint32 i = 0; i < List.at((*it).second)->SubCommands.List.size(); i++)
				{
					uint32 depth = 1;
					TReturnValue *Found = static_cast<TReturnValue*>(RecurseSubCommands (depth, List.at((*it).second)->SubCommands.List[i]));
					if (Found)
						return Found;
				}
			}
			return Command;
		}
	}
	return NULL;
}

typedef void (*TPlayerCommandFunctorType) (CPlayerEntity*);
class CPlayerCommand : public CCommand<TPlayerCommandFunctorType>
{
public:
	CPlayerCommand (const char *Name, TPlayerCommandFunctorType Func, ECmdTypeFlags Flags) :
	  CCommand<TPlayerCommandFunctorType> (Name, Func, Flags)
	  {
	  };

	~CPlayerCommand ()
	{
	};

	void Run (CPlayerEntity *Player);

	void *NewOfMe (const char *Name, TPlayerCommandFunctorType Func, ECmdTypeFlags Flags)
	{
		return QNew (TAG_GAME) CPlayerCommand (Name, Func, Flags);
	}

	CPlayerCommand &AddSubCommand (const char *Name, TPlayerCommandFunctorType Func, ECmdTypeFlags Flags)
	{
		return static_cast<CPlayerCommand&>(CCommand<TPlayerCommandFunctorType>::AddSubCommand(Name, Func, Flags));
	};
};

void Cmd_RunCommand (const char *commandName, CPlayerEntity *Player);
void Cmd_RemoveCommands ();

CPlayerCommand &Cmd_AddCommand (const char *commandName, void (*Func) (CPlayerEntity *Player), ECmdTypeFlags Flags = 0);

void AddTestDebugCommands ();

#else
FILE_WARNING
#endif
