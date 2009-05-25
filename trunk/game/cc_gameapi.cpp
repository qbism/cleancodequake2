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
// cc_gameapi.cpp
// Contains overloads and other neat things
//

#include "cc_local.h"

void SetModel (edict_t *ent, char *model)
{
	if (!(model[0] == '*'))
		DebugPrintf ("SetModel on a non-brush model!\n");

_CC_DISABLE_DEPRECATION
(
	gi.setmodel (ent, model);
)
}

static int mapWarnings, mapErrors;
int entityNumber;

void InitMapCounter ()
{
	mapWarnings = mapErrors = 0;
	DebugPrintf ("======================\nSpawning entities...\n");
}

void EndMapCounter ()
{
	DebugPrintf ("...entities spawned with %i error(s), %i warning(s).\n======================\n", mapErrors, mapWarnings);
}

void Map_Print (EMapPrintType printType, edict_t *ent, vec3_t origin)
{
	if (printType == MAPPRINT_WARNING)
	{
		mapWarnings++;
		DebugPrintf ("Warning %i>", mapWarnings);
	}
	else if (printType == MAPPRINT_ERROR)
	{	
		mapErrors++;
		DebugPrintf ("Error %i>", mapErrors);
	}

	DebugPrintf ("Entity #%i ", entityNumber);
	if (ent->classname)
		DebugPrintf ("(%s) ", ent->classname);
	if (origin)
		DebugPrintf ("(%.0f %.0f %.0f)", origin[0], origin[1], origin[2]);
	DebugPrintf ("\n");
}

void MapPrint (EMapPrintType printType, edict_t *ent, vec3_t origin, char *fmt, ...)
{
	Map_Print (printType, ent, origin);

	va_list		argptr;
	char		text[MAX_COMPRINT];

	va_start (argptr, fmt);
	vsnprintf_s (text, sizeof(text), MAX_COMPRINT, fmt, argptr);
	va_end (argptr);

	if (printType == MAPPRINT_WARNING)
		DebugPrintf ("Warning %i>", mapWarnings);
	else if (printType == MAPPRINT_ERROR)
		DebugPrintf ("Error %i>", mapErrors);
	DebugPrintf ("%s", text);
}

#include <string>
#include <vector>
using namespace std;

#define POUNDENTITIES_VERSION "1"
int fileVersion;

int curIf = 0;
vector<bool> ifLists;

inline void PushIf (bool expr)
{
	curIf++;
	ifLists.push_back(expr);
}

inline void EndIf ()
{
	curIf--;
	ifLists.pop_back();
}

inline bool IfExpr ()
{
	return ifLists[curIf-1];
}

// Returns an entire line.
char *CC_ParseLine (char **entString)
{
	size_t found = strcspn (*entString, "\n");
	char *token = *entString;

	token[found] = '\0';
	*entString = token+found+1;

	return token;
}


char *ParsePound (char *tok, char *realEntities)
{
	char *token = Com_Parse(&tok);
	if (strcmp(token, "#version") == 0)
	{
		token = Com_Parse (&tok);
		if (strcmp(token, POUNDENTITIES_VERSION))
			DebugPrintf ("Pound entity file is version %s against version "POUNDENTITIES_VERSION"!\n", token);
		fileVersion = atoi(token);
	}
	else if (strcmp(token, "#elseifcvar") == 0)
	{
		token = Com_Parse (&tok);
		CCvar temp = CCvar(token, "");

		token = Com_Parse (&tok);
		// An "else if" essentially means that we're re-evaluating the
		// value of the current statement, so we need to pop first
		EndIf();

		if (Q_stricmp(token, temp.String()) == 0)
			PushIf (true);
		else
		{
			PushIf (false);
			// Since we evaluated to false, we need to keep going
			// till we reach an #else*
			while (((token = CC_ParseLine(&realEntities)) != NULL) && (token[0] != '#' && token[1] != 'e'));

			ParsePound (token, realEntities);
		}
	}
	else if (strcmp(token, "#else") == 0)
	{
		// If the current "if" evaluated to false, we can continue.
		// Otherwise...
		if (IfExpr() == true)
		{
			// Since we evaluated to false, we need to keep going
			// till we reach an #else*
			while (((token = CC_ParseLine(&realEntities)) != NULL) && (token[0] != '#' && token[1] != 'e'));

			ParsePound (token, realEntities);
		}
	}
	else if (strcmp(token, "#endif") == 0)
	{
		EndIf();
	}
	else if (strcmp(token, "#ifcvar") == 0)
	{
		token = Com_Parse (&tok);
		CCvar temp = CCvar(token, "");

		token = Com_Parse (&tok);
		if (Q_stricmp(token, temp.String()) == 0)
			PushIf (true);
		else
		{
			PushIf (false);
			// Since we evaluated to false, we need to keep going
			// till we reach an #else*
			while (((token = CC_ParseLine(&realEntities)) != NULL) && (token[0] != '#' && token[1] != 'e'));

			ParsePound (token, realEntities);
		}
	}

	return realEntities;
}

char *CC_LoadEntFile (char *mapname, char *entities)
{
	string fileName;
	FILE *fp;

	CCvar Game = CCvar("gamename", "");
	fileName += Game.String();
	fileName += "/maps/";
	fileName += mapname;
	fileName += ".ent";

#ifndef CRT_USE_UNDEPRECATED_FUNCTIONS
	fp = fopen(fileName.c_str(), "rb");
#else
	int errorVal = fopen_s(&fp, fileName.c_str(), "rb");
#endif

	if (!fp || errorVal)
	{
		fileName = "";
		fileName += "baseq2/maps/";
		fileName += mapname;
		fileName += ".ent";


#ifndef CRT_USE_UNDEPRECATED_FUNCTIONS
		fp = fopen(fileName.c_str(), "rb");
#else
		int errorVal = fopen_s(&fp, fileName.c_str(), "rb");

		if (!fp || errorVal)
			return entities;
#endif
	}

	if (fp)
	{
		long len;

		fseek (fp, 0, SEEK_END);
		len = ftell (fp);
		fseek (fp, 0, SEEK_SET);

		char *newEntities = new char[len];
		fread (newEntities, sizeof(char), len, fp);
		newEntities[len] = '\0';

		fclose(fp);

		return newEntities;
	}
	return entities;
}

bool TokenEnd (char *token)
{
	int i = 0;
	while (token[i])
		i++;
	if (token[i] == '\0' && token[i+1] == '\0')
		return true;
	return false;
}

// To speed the process up a bit, and to reduce problematic newlines,
// this goes by entire lines.
char *CC_ParseSpawnEntities (char *mapname, char *entities)
{
	string finalString;
	char *realEntities;
	char *token;
	char *tempEntities;

	tempEntities = CC_LoadEntFile (mapname, entities);
	if (tempEntities == entities)
		return entities;
	else
		entities = tempEntities;
	realEntities = entities;

	while ((token = CC_ParseLine(&realEntities)) != NULL)
	{
		if (token == NULL)
			break; // ever happen?
		if (token[0] == '#' && (realEntities = ParsePound(token, realEntities)) != NULL)
			continue;

		// Basically if we reach here, this part makes it into the final compilation.
		finalString += token;
		finalString += "\n";

		if (TokenEnd(token))
			break;
	}

	char *finalEntString = new char[finalString.length()];
	Q_snprintfz (finalEntString, finalString.length(), "%s", finalString.c_str());

	return finalEntString;
}

void *::operator new (size_t size)
{
	return gi.TagMalloc (size, TAG_CLEAN_LEVEL);
}

void *::operator new[] (size_t size)
{
	return gi.TagMalloc (size, TAG_CLEAN_LEVEL);
}

void ::operator delete (void *ptr)
{
	gi.TagFree (ptr);
}

void ::operator delete[] (void *ptr)
{
	gi.TagFree (ptr);
}

/*
================
Sys_Milliseconds
================
*/
#include <windows.h>

uint32 curtime;
uint32 Sys_Milliseconds ()
{
	static uint32		base;
	static bool	initialized = false;

	if (!initialized)
	{	// let base retain 16 bits of effectively random data
		base = timeGetTime() & 0xffff0000;
		initialized = true;
	}
	curtime = timeGetTime() - base;

	return curtime;
}