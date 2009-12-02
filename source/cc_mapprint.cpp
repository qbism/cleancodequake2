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
// cc_mapprint.cpp
// Contains a system to more easily-er warn about map errors
//

#include "cc_local.h"

static sint32 mapWarnings, mapErrors;

void InitMapCounter ()
{
	mapWarnings = mapErrors = 0;
	DebugPrintf ("======================\nSpawning entities...\n");
}

void EndMapCounter ()
{
	DebugPrintf ("...entities spawned with %i error(s), %i warning(s).\n======================\n", mapErrors, mapWarnings);
}

#include <sstream>
static std::cc_stringstream PrintBuffer;

void Map_Print (EMapPrintType printType, CBaseEntity *ent, vec3f &origin)
{
	if (printType == MAPPRINT_WARNING)
	{
		mapWarnings++;
		PrintBuffer << "Warning " << mapWarnings << " (" << level.ClassName << " #" << level.EntityNumber << ")";
	}
	else if (printType == MAPPRINT_ERROR)
	{	
		mapErrors++;
		PrintBuffer << "Error " << mapErrors << " (" << level.ClassName << " #" << level.EntityNumber << ")";
	}

	if (origin)
		PrintBuffer << " @ (" << origin.X << " " << origin.Y << " " << origin.Z << ")";
	PrintBuffer << ":\n";
}

void MapPrint (EMapPrintType printType, CBaseEntity *ent, vec3f &origin, char *fmt, ...)
{
	Map_Print (printType, ent, origin);

	va_list		argptr;
	static char	text[MAX_COMPRINT];

	va_start (argptr, fmt);
	vsnprintf_s (text, sizeof(text), MAX_COMPRINT, fmt, argptr);
	va_end (argptr);

	PrintBuffer << "   " << text;

	DebugPrintf ("%s", PrintBuffer.str().c_str());

	PrintBuffer.str().clear();
}

#define POUNDENTITIES_VERSION "1"
sint32 fileVersion;

sint32 curIf = 0;
std::vector<bool, std::generic_allocator<bool> > ifLists;

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

CC_ENUM (uint8, EPoundVariableType)
{
	POUNDVARIABLE_INTEGER,
	POUNDVARIABLE_FLOATING,
	POUNDVARIABLE_STRING
};

struct PoundVariable_t
{
	char				*variableName;
	EPoundVariableType	variableType;
	union
	{
		sint32			*integer;
		float		*floating;
		char		*string;
	} vars;
};

std::vector<PoundVariable_t *, std::generic_allocator<PoundVariable_t*> > VariableList;

PoundVariable_t *Pound_FindVar (char *name)
{
	for (uint32 i = 0; i < VariableList.size(); i++)
	{
		if (strcmp((VariableList[i])->variableName, name) == 0)
			return VariableList[i];
	}
	return NULL;
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
	else if (strcmp(token, "#dim") == 0)
	{
		token = Com_Parse (&tok);

		// Name of variable
		PoundVariable_t *newVar = QNew (com_levelPool, 0) PoundVariable_t;
		newVar->variableName = Mem_PoolStrDup (token, com_levelPool, 0);

		token = Com_Parse (&tok);
		sint32 completed = 0;

		for (uint32 i = 0; i < strlen(token); i++)
		{
			if (token[i] == 0 || isdigit(token[i]))
			{
				completed++;
				continue;
			}

			break;
		}

		if (completed == strlen(token))
		{
			sint32 value = atoi(token);
			newVar->vars.integer = QNew (com_levelPool, 0) sint32(value);
			newVar->variableType = POUNDVARIABLE_INTEGER;
		}
		else
		{
			completed = 0;
			// Not an integer, check if it could be a floating point
			for (uint32 i = 0; i < strlen(token); i++)
			{
				if (token[i] == 0 || isdigit(token[i]) || token[i] == '.')
				{
					completed++;
					continue;
				}

				break;
			}

			if (completed == strlen(token))
			{
				float value = atof(token);
				newVar->vars.floating = QNew (com_levelPool, 0) float(value);
				newVar->variableType = POUNDVARIABLE_FLOATING;
			}
			else
			{
				// Must be a string then
				newVar->vars.string = Mem_PoolStrDup (token, com_levelPool, 0);
				newVar->variableType = POUNDVARIABLE_STRING;
			}
		}
		VariableList.push_back (newVar);
	}
	else if (strcmp(token, "#elseifcvar") == 0)
	{
		token = Com_Parse (&tok);
		CCvar temp (token, "");

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
		CCvar temp (token, "");

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
	else if (strcmp(token, "#if") == 0)
	{
		token = Com_Parse (&tok);
		PoundVariable_t *Var = Pound_FindVar (token);

		token = Com_Parse (&tok);

		if (Var)
		{
			switch (Var->variableType)
			{
				case POUNDVARIABLE_INTEGER:
					if (*Var->vars.integer == atoi(token))
						PushIf (true);
					else
					{
						PushIf (false);
						// Since we evaluated to false, we need to keep going
						// till we reach an #else*
						while (((token = CC_ParseLine(&realEntities)) != NULL) && (token[0] != '#' && token[1] != 'e'));

						ParsePound (token, realEntities);
					}
					break;
				case POUNDVARIABLE_FLOATING:
					if (*Var->vars.floating == atof(token))
						PushIf (true);
					else
					{
						PushIf (false);
						// Since we evaluated to false, we need to keep going
						// till we reach an #else*
						while (((token = CC_ParseLine(&realEntities)) != NULL) && (token[0] != '#' && token[1] != 'e'));

						ParsePound (token, realEntities);
					}
					break;
				case POUNDVARIABLE_STRING:
					if (Q_stricmp(Var->vars.string, token) == 0)
						PushIf (true);
					else
					{
						PushIf (false);
						// Since we evaluated to false, we need to keep going
						// till we reach an #else*
						while (((token = CC_ParseLine(&realEntities)) != NULL) && (token[0] != '#' && token[1] != 'e'));

						ParsePound (token, realEntities);
					}
					break;
			}
		}
	}

	return realEntities;
}

bool TokenEnd (char *token)
{
	sint32 i = 0;
	while (token[i])
		i++;
	if (token[i] == '\0' && token[i+1] == '\0')
		return true;
	return false;
}

// To speed the process up a bit, and to reduce problematic newlines,
// this goes by entire lines.
char *CC_ParseSpawnEntities (char *ServerLevelName, char *entities)
{
	CFileBuffer FileBuffer((std::cc_string("maps/ents/") + ServerLevelName + ".ccent").c_str(), true);
	std::cc_string finalString;
	char *realEntities;
	char *token;

	if (!FileBuffer.Valid())
		return entities;
	else
		entities = FileBuffer.GetBuffer<char> ();
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

	char *finalEntString = Mem_PoolStrDup (finalString.c_str(), com_levelPool, 0);

	return finalEntString;
}