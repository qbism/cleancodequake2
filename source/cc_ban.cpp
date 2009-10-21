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
// cc_ban.cpp
// Simple ban system
//

#include "cc_local.h"
#include "cc_ban.h"

CBanList	Bans;

int ipcmp (IPAddress &filter, IPAddress &string)
{
	return !Q_WildcardMatch (filter.str, string.str, true);
}

// Ban list looks as follows:
/*
// Comments are ignored.
// First token needs to be either ip or name, followed by the IP address
// or name of the offender, respectively.
// Then flags! See cc_ban.h for flag numbers.
// Unknown flags are stored, but ignored by older versions.
name Paril 1
ip 252.6.10.6 5
*/

void CBanList::LoadFromFile ()
{
	char *data;
	FS_LoadFile ("bans.lst", (void**)&data, true);

	if (!data)
		return;
	std::cc_string mainString = data;
	std::cc_string line, token;

	// Get the entire line
	size_t z = 0, c = 0, oc = 0;
	z = mainString.find_first_of("\n\0");
	line = mainString.substr(0, (z == -1) ? strlen(data) : z);

	while (line[0])
	{
		// Parse the line.
		// Get the distance to the first character.
		oc = line.find_first_of(" \n\0", c);
		token = line.substr (c, oc-c);
		c = oc;
		while (token[0])
		{
			if (token[0] == '/' && token[1] == '/')
				break;

			if (Q_stricmp (token.c_str(), "name") == 0)
			{
				// Get the distance to the next token.
				// find_first_of operates on length so we need to calc the length
				// of the next token as well.
				oc = line.find_first_of(" \n\0", ++c);
				token = line.substr (c, oc-c);
				c = oc;

				BanIndex *NewIndex = QNew (com_gamePool, 0) BanIndex;
				NewIndex->IP = false;

				NewIndex->Name = QNew (com_gamePool, 0) char[token.length()];
				Q_strncpyz (NewIndex->Name, token.c_str(), token.length());
				NewIndex->Name[token.length()] = 0;

				oc = line.find_first_of(" \n\0", ++c);
				token = line.substr (c, oc-c);
				c = oc;

				EBanTypeFlags Flags = atoi(token.c_str());

				NewIndex->Flags = Flags;

				BanList.push_back (NewIndex);
			}
			else if (Q_stricmp (token.c_str(), "ip") == 0)
			{
				oc = line.find_first_of(" \n\0", ++c);
				token = line.substr (c, oc-c);
				c = oc;

				BanIndex *NewIndex = QNew (com_gamePool, 0) BanIndex;
				NewIndex->IP = true;

				NewIndex->IPAddress = QNew (com_gamePool, 0) IPAddress;
				Q_snprintfz (NewIndex->IPAddress->str, sizeof(NewIndex->IPAddress->str), "%s", token.c_str());

				oc = line.find_first_of(" \n\0", ++c);
				token = line.substr (c, oc-c);
				c = oc;

				EBanTypeFlags Flags = atoi(token.c_str());
				NewIndex->Flags = Flags;
				BanList.push_back (NewIndex);
			}

			if (c == -1 || c == std::cc_string::npos)
				break;

			oc = line.find_first_of(" \n\0", ++c);
			token = line.substr (c, oc-c);
			c = oc;
		}

		c = 0;

		if (z == -1)
			break;

		oc = mainString.find_first_of("\n\0", ++z);
		line = mainString.substr (z, oc-z);
		z = oc;
		oc = 0;
	}

	// Free everything
	FS_FreeFile (data);
}

void CBanList::SaveList ()
{
	fileHandle_t f;
	FS_OpenFile ("bans.lst", &f, FS_MODE_WRITE_TEXT);

	if (!f)
		return;

	for (size_t i = 0; i < BanList.size(); i++)
	{
		BanIndex *Index = BanList[i];
		char tempData[128];

		Q_snprintfz (tempData, sizeof(tempData), "%s ", (Index->IP) ? "ip" : "name");
		FS_Write (&tempData, strlen(tempData), f);
		if (Index->IP)
			Q_snprintfz (tempData, sizeof(tempData), "%s ", Index->IPAddress->str);
		else
			Q_snprintfz (tempData, sizeof(tempData), "%s ", Index->Name);
		FS_Write (&tempData, strlen(tempData), f);

		Q_snprintfz (tempData, sizeof(tempData), "%i\n", Index->Flags);
		FS_Write (&tempData, strlen(tempData), f);
	}

	FS_CloseFile (f);
}

void CBanList::AddToList (IPAddress Adr, EBanTypeFlags Flags)
{
	BanIndex *NewIndex = QNew (com_gamePool, 0) BanIndex;
	NewIndex->IP = true;

	NewIndex->IPAddress = QNew (com_gamePool, 0) IPAddress(Adr);
	NewIndex->Flags = Flags;

	BanList.push_back (NewIndex);
}

void CBanList::AddToList (char *Name, EBanTypeFlags Flags)
{
	BanIndex *NewIndex = QNew (com_gamePool, 0) BanIndex;
	NewIndex->IP = false;
	NewIndex->Name = QNew (com_gamePool, 0) char[strlen(Name)];
	Q_strncpyz (NewIndex->Name, Name, sizeof(NewIndex->Name));
	NewIndex->Name[strlen(Name)] = 0;
	NewIndex->Flags = Flags;

	BanList.push_back (NewIndex);
}

void CBanList::RemoveFromList (IPAddress Adr)
{
	std::vector<BanIndex*, std::level_allocator<BanIndex*> >::iterator it;

	for ( it=BanList.begin() ; it < BanList.end(); it++ )
	{
		BanIndex *Index = *it;

		if (!Index->IP)
			continue;

		if (!ipcmp(*Index->IPAddress, Adr))
		{
			BanList.erase(it);
			break;
		}
	}
}

void CBanList::RemoveFromList (char *Name)
{
	std::vector<BanIndex*, std::level_allocator<BanIndex*> >::iterator it;

	for ( it=BanList.begin() ; it < BanList.end(); it++ )
	{
		BanIndex *Index = *it;

		if (Index->IP)
			continue;

		if (!strcmp(Name, Index->Name))
		{
			BanList.erase(it);
			break;
		}
	}
}

void CBanList::ChangeBan (IPAddress Adr, EBanTypeFlags Flags)
{
	std::vector<BanIndex*, std::level_allocator<BanIndex*> >::iterator it;

	for ( it=BanList.begin() ; it < BanList.end(); it++ )
	{
		BanIndex *Index = *it;

		if (!Index->IP)
			continue;

		if (!ipcmp(*Index->IPAddress, Adr))
		{
			Index->Flags = Flags;
			break;
		}
	}
}

void CBanList::ChangeBan (char *Name, EBanTypeFlags Flags)
{
	std::vector<BanIndex*, std::level_allocator<BanIndex*> >::iterator it;

	for ( it=BanList.begin() ; it < BanList.end(); it++ )
	{
		BanIndex *Index = *it;

		if (Index->IP)
			continue;

		if (!strcmp(Name, Index->Name))
		{
			Index->Flags = Flags;
			break;
		}
	}
}

bool CBanList::IsSquelched (IPAddress Adr)
{
	std::vector<BanIndex*, std::level_allocator<BanIndex*> >::iterator it;

	for ( it=BanList.begin() ; it < BanList.end(); it++ )
	{
		BanIndex *Index = *it;

		if (!Index->IP)
			continue;

		if (!ipcmp(*Index->IPAddress, Adr))
			return !!(Index->Flags & BAN_SQUELCH);
	}
	return false;
}

bool CBanList::IsBannedFromSpectator (IPAddress Adr)
{
	std::vector<BanIndex*, std::level_allocator<BanIndex*> >::iterator it;

	for ( it=BanList.begin() ; it < BanList.end(); it++ )
	{
		BanIndex *Index = *it;

		if (!Index->IP)
			continue;

		if (!ipcmp(*Index->IPAddress, Adr))
			return !!(Index->Flags & BAN_SPECTATOR);
	}
	return false;
}

bool CBanList::IsBanned (IPAddress Adr)
{
	std::vector<BanIndex*, std::level_allocator<BanIndex*> >::iterator it;

	for ( it=BanList.begin() ; it < BanList.end(); it++ )
	{
		BanIndex *Index = *it;

		if (!Index->IP)
			continue;

		if (!ipcmp(*Index->IPAddress, Adr))
			return !!(Index->Flags & BAN_ENTER);
	}
	return false;
}

bool CBanList::IsSquelched (const char *Name)
{
	std::vector<BanIndex*, std::level_allocator<BanIndex*> >::iterator it;

	for (it = BanList.begin(); it < BanList.end(); it++)
	{
		BanIndex *Index = *it;

		if (Index->IP)
			continue;

		if (!strcmp(Name, Index->Name))
			return !!(Index->Flags & BAN_SQUELCH);
	}
	return false;
}

bool CBanList::IsBannedFromSpectator (const char *Name)
{
	std::vector<BanIndex*, std::level_allocator<BanIndex*> >::iterator it;

	for (it = BanList.begin(); it < BanList.end(); it++)
	{
		BanIndex *Index = *it;

		if (Index->IP)
			continue;

		if (!strcmp(Name, Index->Name))
			return !!(Index->Flags & BAN_SPECTATOR);
	}
	return false;
}

bool CBanList::IsBanned (const char *Name)
{
	std::vector<BanIndex*, std::level_allocator<BanIndex*> >::iterator it;

	for (it = BanList.begin(); it < BanList.end(); it++)
	{
		BanIndex *Index = *it;

		if (Index->IP)
			continue;

		if (!strcmp(Name, Index->Name))
			return !!(Index->Flags & BAN_ENTER);
	}
	return false;
}
