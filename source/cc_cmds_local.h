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

#if !defined(__CC_CMDS_LOCAL_H__) || !defined(INCLUDE_GUARDS)
#define __CC_CMDS_LOCAL_H__

#define MAX_COMMANDS 128
#define MAX_CMD_HASH (MAX_COMMANDS/4)

class CCmd
{
public:
	uint32			hashValue;
	CCmd			*hashNext;
	std::cc_string	cmdName;

	ECmdTypeFlags	CmdFlags;
	void			(*RunFunction) (CPlayerEntity *ent);

	CCmd (std::cc_string name, void (*Func) (CPlayerEntity *ent), ECmdTypeFlags Flags);
	~CCmd();
	void Run (CPlayerEntity *ent);
};

// Also throwing server command class here.
class CServerCmd
{
public:
	uint32			hashValue;
	CServerCmd		*hashNext;
	std::cc_string	cmdName;

	ECmdTypeFlags	CmdFlags;
	void			(*RunFunction) ();

	CServerCmd (std::cc_string name, void (*Func) ());
	~CServerCmd();

	void Run ();
};

#else
FILE_WARNING
#endif