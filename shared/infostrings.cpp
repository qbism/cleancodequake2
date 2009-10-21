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
Foundation, Inc., 59 Temple Place - Suite 330, Boston, MA 02111-1307, USA.
*/

//
// infostrings.c
//

#include "../source/cc_options.h"
#include "shared.h"

/*
=============================================================================

	INFO STRINGS

=============================================================================
*/

/*
================
Info_Print
================
*/
void Info_Print (std::cc_string &s)
{
	size_t curIndex = 0;

	if (s[curIndex] == '\\')
		s[curIndex++];

	while (s[curIndex])
	{
		std::cc_string	key, value;

		while (s[curIndex] && s[curIndex] != '\\')
			key += s[curIndex++];

		// Number of spaces to add
		size_t l = key.length();
		if (l < 20)
		{
			for (size_t i = 0; i < (20 - l); i++)
				key += ' ';
		}

		Com_Printf (0, "%s", key.c_str());

		if (!s[curIndex])
		{
			Com_Printf (0, "MISSING VALUE\n");
			return;
		}

		s[curIndex++];
		while (s[curIndex] && s[curIndex] != '\\')
			value += s[curIndex++];

		if (s[curIndex])
			s[curIndex++];

		Com_Printf (0, "%s\n", value.c_str());
	}
}


/*
===============
Info_ValueForKey

Searches the string for the given key and returns the associated value, or an empty string.
===============
*/
std::cc_string Info_ValueForKey (std::cc_string &s, std::cc_string key)
{
	size_t	curIndex = 0;

	if (s[curIndex] == '\\')
		curIndex++;

	while (true)
	{
		std::cc_string pkey;
		std::cc_string value;

		while (s[curIndex] != '\\')
		{
			if (!s[curIndex])
				return "";
			pkey += s[curIndex++];
		}
		curIndex++;

		while (s[curIndex] != '\\' && s[curIndex])
		{
			if (!s[curIndex])
				return "";
			value += s[curIndex++];
		}

		if (key == pkey)
			return value;

		if (!s[curIndex])
			return "";
		curIndex++;
	}
}


/*
==================
Info_RemoveKey
==================
*/
void Info_RemoveKey (std::cc_string &s, std::cc_string key)
{
	if (key.find ('\\'))
		return;

	size_t	curIndex = 0;
	std::cc_string	pkey, value;
	while (true)
	{
		size_t start = curIndex;
		if (s[curIndex] == '\\')
			curIndex++;

		pkey.clear ();
		value.clear ();

		while (s[curIndex] != '\\')
		{
			if (!s[curIndex])
				return;
			
			pkey += s[curIndex++];
		}
		curIndex++;

		while (s[curIndex] != '\\' && s[curIndex])
		{
			if (!s[curIndex])
				return;
			value += s[curIndex++];
		}

		if (key == pkey)
		{
			//Q_strncpyz (start, s, MAX_INFO_KEY);	// Remove this part
			s.erase (start, (start - curIndex));
			return;
		}

		if (!s[curIndex])
			return;
	}
}


/*
==================
Info_Validate

Some characters are illegal in info strings because they
can mess up the server's parsing
==================
*/
bool Info_Validate (std::cc_string &s)
{
	return (s.find ('\"') || s.find (';'));
}


/*
==================
Info_SetValueForKey
==================
*/
void Info_SetValueForKey (std::cc_string &s, std::cc_string key, std::cc_string value)
{
	// Sanity check
	if (key.find ('\\'))
	{
		Com_Printf (PRNT_WARNING, "Can't use keys with a \\\n");
		return;
	}
	if (value.find ('\\'))
	{
		Com_Printf (PRNT_WARNING, "Can't use values with a \\\n");
		return;
	}
	if (key.find (';'))
	{
		Com_Printf (PRNT_WARNING, "Can't use keys or values with a semicolon\n");
		return;
	}
	if (key.find ('\"'))
	{
		Com_Printf (PRNT_WARNING, "Can't use keys with a \"\n");
		return;
	}
	if (value.find ('\"'))
	{
		Com_Printf (PRNT_WARNING, "Can't use values with a \"\n");
		return;
	}
	if (key.length() > MAX_INFO_KEY-1 || value.length() > MAX_INFO_KEY-1)
	{
		Com_Printf (PRNT_WARNING, "Keys and values must be < %i characters.\n", MAX_INFO_KEY);
		return;
	}

	// Remove the key so it can be re-added cleanly
	Info_RemoveKey (s, key);
	if (!value.length())
		return;

	// Generate the key and make sure it will fit
	std::cc_string newPair = "\\" + key + '\\' + value;
	if (newPair.length() + s.length() > MAX_INFO_STRING-1)
	{
		Com_Printf (PRNT_WARNING, "Info string length exceeded\n");
		return;
	}

	// Only copy ascii values
	for (size_t cur = 0; newPair[cur]; )
	{
		char c = newPair[cur++] & 127;	// Strip high bits
		if (c >= 32 && c < 127)
			s += c;
	}
}
