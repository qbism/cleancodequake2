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

sint32 ipcmp (IPAddress &filter, IPAddress &string)
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

void CBanList::Clear ()
{
	for (size_t i = 0; i < BanList.size(); i++)
		QDelete BanList.at(i);
	BanList.clear ();
}

void CBanList::LoadFromFile ()
{
	Bans.Clear ();
	CFile File ("bans.lst", FILEMODE_READ);

	if (!File.Valid())
		return;

	while (true)
	{
		if (File.EndOfFile())
			break;

		cc_string line = File.ReadLine ();
		
		// Parse the line.
		CParser Parser (line.c_str(), PSP_COMMENT_MASK);

		const char *token;
		if (!Parser.ParseToken (PSF_ALLOW_NEWLINES, &token))
			break;

		if (Q_stricmp (token, "name") == 0)
		{
			// Get the distance to the next token.
			// find_first_of operates on length so we need to calc the length
			// of the next token as well.
			if (!Parser.ParseToken (PSF_ALLOW_NEWLINES, &token))
				break;

			BanIndex *NewIndex = QNew (TAG_GENERIC) BanIndex;
			NewIndex->IP = false;
			NewIndex->Name = Mem_StrDup (token);

			if (!Parser.ParseDataType<EBanTypeFlags> (PSF_ALLOW_NEWLINES, &NewIndex->Flags, 1))
				break;

			BanList.push_back (NewIndex);
		}
		else if (Q_stricmp (token, "ip") == 0)
		{
			if (!Parser.ParseToken (PSF_ALLOW_NEWLINES, &token))
				break;

			BanIndex *NewIndex = QNew (TAG_GENERIC) BanIndex;
			NewIndex->IP = true;

			NewIndex->Address = QNew (TAG_GENERIC) IPAddress;
			Q_snprintfz (NewIndex->Address->str, sizeof(NewIndex->Address->str), "%s", token);

			if (!Parser.ParseDataType<EBanTypeFlags> (PSF_ALLOW_NEWLINES, &NewIndex->Flags, 1))
				break;

			BanList.push_back (NewIndex);
		}
	}
	
	ServerPrintf ("Loaded %u bans from file\n", Bans.BanList.size());
}

void CBanList::SaveList ()
{
	// No changes detected
	if (!Changed)
	{
		ServerPrintf ("No changes in ban file, skipping\n");
		return;
	}

	CFile File ("bans.lst", FILEMODE_WRITE | FILEMODE_CREATE | FILEMODE_ASCII);

	if (!File.Valid())
		return;

	for (size_t i = 0; i < BanList.size(); i++)
	{
		BanIndex *Index = BanList[i];

		File.Print ("%s %s %i\n",
			(Index->IP) ? "ip" : "name",
			(Index->IP) ? Index->Address->str :Index->Name,
			"%i\n", Index->Flags);
	}

	ServerPrintf ("Saved %u bans\n", Bans.BanList.size());
}

bool CBanList::AddToList (IPAddress Adr, EBanTypeFlags Flags)
{
	if (InList(Adr))
		return false;

	BanIndex *NewIndex = QNew (TAG_GENERIC) BanIndex;
	NewIndex->IP = true;

	NewIndex->Address = QNew (TAG_GENERIC) IPAddress(Adr);
	NewIndex->Flags = Flags;

	BanList.push_back (NewIndex);
	return true;
}

bool CBanList::AddToList (const char *Name, EBanTypeFlags Flags)
{
	if (InList(Name))
		return false;

	BanIndex *NewIndex = QNew (TAG_GENERIC) BanIndex;
	NewIndex->IP = false;
	NewIndex->Name = QNew (TAG_GENERIC) char[strlen(Name)];
	Q_strncpyz (NewIndex->Name, Name, strlen(Name)+1);
	NewIndex->Name[strlen(Name)] = 0;
	NewIndex->Flags = Flags;

	BanList.push_back (NewIndex);
	return true;
}

bool CBanList::InList (IPAddress Adr)
{
	for (TBanIndexContainer::iterator it = BanList.begin(); it < BanList.end(); ++it)
	{
		BanIndex *Index = *it;

		if (!Index->IP)
			continue;

		if (!ipcmp(*Index->Address, Adr))
			return true;
	}
	return false;
}

bool CBanList::InList (const char *Name)
{
	for (TBanIndexContainer::iterator it = BanList.begin(); it < BanList.end(); ++it)
	{
		BanIndex *Index = *it;

		if (Index->IP)
			continue;

		if (!strcmp(Name, Index->Name))
			return true;
	}
	return false;
}

bool CBanList::RemoveFromList (IPAddress Adr)
{
	for (TBanIndexContainer::iterator it = BanList.begin(); it < BanList.end(); ++it)
	{
		BanIndex *Index = *it;

		if (!Index->IP)
			continue;

		if (!ipcmp(*Index->Address, Adr))
		{
			BanList.erase(it);
			return true;
		}
	}
	return false;
}

bool CBanList::RemoveFromList (const char *Name)
{
	for (TBanIndexContainer::iterator it = BanList.begin(); it < BanList.end(); ++it)
	{
		BanIndex *Index = *it;

		if (Index->IP)
			continue;

		if (!strcmp(Name, Index->Name))
		{
			BanList.erase(it);
			return true;
		}
	}
	return false;
}

bool CBanList::ChangeBan (IPAddress Adr, EBanTypeFlags Flags)
{
	for (TBanIndexContainer::iterator it = BanList.begin(); it < BanList.end(); ++it)
	{
		BanIndex *Index = *it;

		if (!Index->IP)
			continue;

		if (!ipcmp(*Index->Address, Adr))
		{
			if (Index->Flags == Flags)
				return false;

			Index->Flags = Flags;
			return true;
		}
	}
	return false;
}

bool CBanList::ChangeBan (const char *Name, EBanTypeFlags Flags)
{
	for (TBanIndexContainer::iterator it = BanList.begin(); it < BanList.end(); ++it)
	{
		BanIndex *Index = *it;

		if (Index->IP)
			continue;

		if (!strcmp(Name, Index->Name))
		{
			if (Index->Flags == Flags)
				return false;

			Index->Flags = Flags;
			return true;
		}
	}
	return false;
}

bool CBanList::IsSquelched (IPAddress Adr)
{
	for (TBanIndexContainer::iterator it = BanList.begin(); it < BanList.end(); ++it)
	{
		BanIndex *Index = *it;

		if (!Index->IP)
			continue;

		if (!ipcmp(*Index->Address, Adr))
			return !!(Index->Flags & BAN_SQUELCH);
	}
	return false;
}

bool CBanList::IsBannedFromSpectator (IPAddress Adr)
{
	for (TBanIndexContainer::iterator it = BanList.begin(); it < BanList.end(); ++it)
	{
		BanIndex *Index = *it;

		if (!Index->IP)
			continue;

		if (!ipcmp(*Index->Address, Adr))
			return !!(Index->Flags & BAN_SPECTATOR);
	}
	return false;
}

bool CBanList::IsBanned (IPAddress Adr)
{
	for (TBanIndexContainer::iterator it = BanList.begin(); it < BanList.end(); ++it)
	{
		BanIndex *Index = *it;

		if (!Index->IP)
			continue;

		if (!ipcmp(*Index->Address, Adr))
			return !!(Index->Flags & BAN_ENTER);
	}
	return false;
}

bool CBanList::IsSquelched (const char *Name)
{
	for (TBanIndexContainer::iterator it = BanList.begin(); it < BanList.end(); ++it)
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
	for (TBanIndexContainer::iterator it = BanList.begin(); it < BanList.end(); ++it)
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
	for (TBanIndexContainer::iterator it = BanList.begin(); it < BanList.end(); ++it)
	{
		BanIndex *Index = *it;

		if (Index->IP)
			continue;

		if (!strcmp(Name, Index->Name))
			return !!(Index->Flags & BAN_ENTER);
	}
	return false;
}

