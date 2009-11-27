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
// cc_cmds_local.h
// The class, local to CleanCode.
//

#if !defined(__CC_CMDS_LOCAL_H__) || !INCLUDE_GUARDS
#define __CC_CMDS_LOCAL_H__

#define MAX_COMMANDS 128
#define MAX_CMD_HASH (MAX_COMMANDS/4)

template <typename TFunctor>
class CCommand
{
public:
	std::cc_string		Name;
	ECmdTypeFlags		Flags;
	TFunctor			Func;

	CCommand (std::cc_string Name, TFunctor Func, ECmdTypeFlags Flags) :
	  Name(Name),
	  Func(Func),
	  Flags(Flags)
	  {
	  };
	
	virtual ~CCommand ()
	{
	};
};

typedef void (*TCommandFunctorType) (CPlayerEntity*);
class CPlayerCommand : public CCommand <TCommandFunctorType>
{
public:
	CPlayerCommand (std::cc_string Name, TCommandFunctorType Func, ECmdTypeFlags Flags) :
	  CCommand (Name, Func, Flags)
	  {
	  };

	~CPlayerCommand ()
	{
	};

	void Run (CPlayerEntity *ent);
};

// Also throwing server command class here.
typedef void (*TServerCommandFunctorType) ();
class CServerCommand : public CCommand <TServerCommandFunctorType>
{
public:
	CServerCommand (std::cc_string Name, TServerCommandFunctorType Func) :
	  CCommand (Name, Func, 0)
	  {
	  };

	~CServerCommand ()
	{
	};

	void Run ()
	{
		Func ();
	};
};

template <class TReturnValue, typename TListType, typename THashListType>
TReturnValue *FindCommand (std::cc_string commandName, TListType &List, THashListType &HashList)
{
	uint32 hash = Com_HashGeneric(commandName, MAX_CMD_HASH);

	for (THashListType::iterator it = HashList.equal_range(hash).first; it != HashList.equal_range(hash).second; ++it)
	{
		TReturnValue *Command = List.at((*it).second);
		if (Q_stricmp (Command->Name.c_str(), commandName.c_str()) == 0)
			return Command;
	}
	return NULL;
}

#else
FILE_WARNING
#endif