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
// cc_ban.h
// Simple ban system
//

#ifdef BAN_BASIC_INFO
#ifndef MAX_IP_BLOCK_LEN
#define MAX_IP_BLOCK_LEN 3
#define MAX_IP_LEN (MAX_IP_BLOCK_LEN * 4 + 3)

struct IPAddress
{
	char str[MAX_IP_LEN];
};

#endif
#else
#if !defined(CC_GUARD_BAN_H) || !INCLUDE_GUARDS
#define CC_GUARD_BAN_H

CC_ENUM (uint8, EBanTypeFlags)
{
	BAN_SQUELCH		=	BIT(0), // Banned from talking
	BAN_SPECTATOR	=	BIT(1), // Banned from moving to Spectator mode
	BAN_ENTER		=	BIT(2), // Banned from entering the game
};

class BanIndex
{
public:
	bool			IP;
	union
	{
		IPAddress	*IPAddress;
		char		*Name;
	};

	EBanTypeFlags	Flags;

	~BanIndex ()
	{
		if (!IP)
			QDelete Name;
	};
};

typedef std::vector<BanIndex*, std::generic_allocator<BanIndex*> > TBanIndexContainer;

class CBanList
{
public:
	TBanIndexContainer	BanList;
	bool Changed;

	void Clear ();

	void LoadFromFile ();
	void SaveList ();

	bool InList (IPAddress Adr);
	bool InList (const char *Name);

	bool AddToList (IPAddress Adr, EBanTypeFlags Flags);
	bool AddToList (const char *Name, EBanTypeFlags Flags);
	bool RemoveFromList (IPAddress Adr);
	bool RemoveFromList (const char *Name);

	bool ChangeBan (IPAddress Adr, EBanTypeFlags Flags);
	bool ChangeBan (const char *Name, EBanTypeFlags Flags);

	// Call these with ent->client->Persistent.IP if after spawning
	bool IsSquelched (IPAddress IP);
	bool IsBannedFromSpectator (IPAddress IP);
	bool IsBanned (IPAddress IP);

	bool IsSquelched (const char *Name);
	bool IsBannedFromSpectator (const char *Name);
	bool IsBanned (const char *Name);
};

extern CBanList	Bans;
#else
FILE_WARNING
#endif
#endif

