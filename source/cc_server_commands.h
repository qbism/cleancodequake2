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
// cc_servercommands.h
// ServerCommand and related classes
//

#if !defined(CC_GUARD_SERVERCOMMANDS_H) || !INCLUDE_GUARDS
#define CC_GUARD_SERVERCOMMANDS_H

void SvCmd_Register ();
void SvCmd_RemoveCommands ();

class CServerCommand : public CCommand
{
public:
	CServerCommand (const char *Name, CCommandFunctor *Func) :
	  CCommand (Name, Func, 0)
	  {
	  };

	~CServerCommand ()
	{
	};

	void *NewOfMe (const char *Name, CCommandFunctor *Func, ECmdTypeFlags Flags)
	{
		return QNew (TAG_GENERIC) CServerCommand (Name, Func);
	}

	template <class TFunctor>
	CServerCommand &AddSubCommand (const char *Name, ECmdTypeFlags Flags = 0)
	{
		return static_cast<CServerCommand&>(CCommand::AddSubCommand<TFunctor> (Name, Flags));
	};
};

CServerCommand &SvCmd_AddCommand_Internal (const char *commandName, CCommandFunctor *Functor);
template <class TFunctor>
CServerCommand &SvCmd_AddCommand (const char *commandName)
{
	TFunctor *Func = QNew (TAG_GENERIC) TFunctor;
	return SvCmd_AddCommand_Internal (commandName, Func);
};

#else
FILE_WARNING
#endif
