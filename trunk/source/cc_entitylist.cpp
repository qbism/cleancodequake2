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
// cc_entitylist.cpp
// Resolves an entity from a classname
//

#include "cc_local.h"

#define MAX_CLASSNAME_CLASSES 1024

typedef std::multimap<size_t, size_t, std::less<size_t>, std::generic_allocator<size_t> > THashedEntityListType;
typedef std::vector<CClassnameToClassIndex*, std::generic_allocator<CClassnameToClassIndex*> > TEntityListType;

class CEntityList
{
	TEntityListType			EntityList;
	THashedEntityListType	HashedEntityList;

public:
	void AddToList (CClassnameToClassIndex *Entity);
	CBaseEntity *Resolve (edict_t *ent);
};

// Construct on first use idiom
CEntityList &EntityList ()
{
	static CEntityList List;
	return List;
}

CClassnameToClassIndex::CClassnameToClassIndex (CMapEntity				*(*Spawn) (sint32 Index), char *Classname) :
Spawn(Spawn),
Classname(Classname)
{
	EntityList().AddToList (this);
};

void CEntityList::AddToList (CClassnameToClassIndex *Entity)
{
	EntityList.push_back (Entity);

	// Link it in the hash tree
	HashedEntityList.insert (std::make_pair<size_t, size_t> (Com_HashGeneric (Entity->Classname, MAX_CLASSNAME_CLASSES_HASH), EntityList.size()-1));
};

void SpawnWorld ();
CBaseEntity *CEntityList::Resolve (edict_t *ent)
{
	if (Q_stricmp(level.ClassName.c_str(), "worldspawn") == 0)
	{
		SpawnWorld ();
		return g_edicts[0].Entity;
	}

	uint32 hash = Com_HashGeneric(level.ClassName.c_str(), MAX_CLASSNAME_CLASSES_HASH);

	for (THashedEntityListType::iterator it = HashedEntityList.equal_range(hash).first; it != HashedEntityList.equal_range(hash).second; ++it)
	{
		CClassnameToClassIndex *Table = EntityList.at((*it).second);
		if (Q_stricmp (Table->Classname, level.ClassName.c_str()) == 0)
			return Table->Spawn(ent->state.number);
	}

	return NULL;
}

CBaseEntity *ResolveMapEntity (edict_t *ent)
{
	return EntityList().Resolve (ent);
};

/*
===============
ED_CallSpawn

Finds the spawn function for the entity and calls it
===============
*/
void ED_CallSpawn (edict_t *ent)
{
	if (level.ClassName.empty())
	{
		MapPrint (MAPPRINT_ERROR, NULL, vec3fOrigin, "NULL classname!\n");
		return;
	}

	// Check CleanCode stuff
	CBaseEntity *MapEntity = ResolveMapEntity(ent);

	if (!MapEntity)
	{
		MapPrint (MAPPRINT_ERROR, NULL, vec3fOrigin, "Invalid entity: %s (no spawn function)\n", level.ClassName.c_str());

_CC_DISABLE_DEPRECATION
		G_FreeEdict (ent);
_CC_ENABLE_DEPRECATION
		return;
	}

	// Did we get freed?
	if (MapEntity->Freed)
	{
		// We're done then
		MapEntity->gameEntity->Entity = NULL;
		QDelete MapEntity;
		ent->inUse = false;
		return;
	}

	// Link in the classname
	MapEntity->ClassName = Mem_PoolStrDup (level.ClassName.c_str(), com_levelPool, 0);

	if (map_debug->Boolean())
	{
		if (MapEntity->SpawnFlags & SPAWNFLAG_NOT_EASY)
		{
			MapEntity->State.GetEffects() |= EF_COLOR_SHELL;
			MapEntity->State.GetRenderEffects() |= RF_SHELL_RED;
		}
		if (MapEntity->SpawnFlags & SPAWNFLAG_NOT_MEDIUM)
		{
			MapEntity->State.GetEffects() |= EF_COLOR_SHELL;
			MapEntity->State.GetRenderEffects() |= RF_SHELL_BLUE;
		}
		if (MapEntity->SpawnFlags & SPAWNFLAG_NOT_HARD)
		{
			MapEntity->State.GetEffects() |= EF_COLOR_SHELL;
			MapEntity->State.GetRenderEffects() |= RF_SHELL_GREEN;
		}
		if (MapEntity->SpawnFlags & SPAWNFLAG_NOT_DEATHMATCH)
		{
			MapEntity->State.GetEffects() |= EF_COLOR_SHELL;
			MapEntity->State.GetRenderEffects() |= RF_SHELL_DOUBLE;
		}
		if (MapEntity->SpawnFlags & SPAWNFLAG_NOT_COOP)
		{
			MapEntity->State.GetEffects() |= EF_COLOR_SHELL;
			MapEntity->State.GetRenderEffects() |= RF_SHELL_HALF_DAM;
		}
	}
}

/*
====================
ED_ParseEdict

Parses an edict out of the given string, returning the new position
ed should be a properly initialized empty edict.
====================
*/
static void ED_ParseEdict (CParser &data, edict_t *ent)
{
	bool	init = false;

	// Go through all the dictionary pairs
	while (true)
	{
		const char *token;

		// Parse key
		if (!data.ParseToken (PSF_ALLOW_NEWLINES, &token))
			GameError ("ED_ParseEntity: EOF without closing brace");

		if (token[0] == '}')
			break;

		char keyName[256];
		Q_strncpyz (keyName, token, sizeof(keyName));
		
		// Parse value	
		if (!data.ParseToken (PSF_ALLOW_NEWLINES, &token) && data.IsEOF())
			GameError ("ED_ParseEntity: EOF without closing brace");

		if (token[0] == '}')
			GameError ("ED_ParseEntity: closing brace without data");

		init = true;

		// Keynames with a leading underscore are used for utility comments,
		// and are immediately discarded by quake
		if (keyName[0] == '_')
			continue;
		else if (Q_stricmp (keyName, "classname") == 0)
			level.ClassName = token;
		else
			// push it in the list for the entity
			level.ParseData.push_back (QNew (com_levelPool, 0) CKeyValuePair (keyName, token));
	}

	if (!init)
		memset (ent, 0, sizeof(*ent));
}


/*
================
G_FindTeams

Chain together all entities with a matching team field.

All but the first will have the FL_TEAMSLAVE flag set.
All but the last will have the teamchain field set to the next one
================
*/
void G_FindTeams ()
{
	sint32		c = 0, c2 = 0;

	for (int i = 1; i < Game.GetNumEdicts(); i++)
	{
		CBaseEntity *e = g_edicts[i].Entity;
		if (!e)
			continue;
		if (!e->GetInUse())
			continue;
		if (!e->Team.String)
			continue;
		if (e->Flags & FL_TEAMSLAVE)
			continue;

		CBaseEntity *chain = e;
		e->Team.Master = e;
		e->Team.HasTeam = true;

		c++;
		c2++;
		for (int j = i + 1; j < Game.GetNumEdicts(); j++)
		{
			CBaseEntity *e2 = g_edicts[j].Entity;
			if (!e2)
				continue;
			if (!e2->GetInUse())
				continue;
			if (!e2->Team.String)
				continue;
			if (e2->Flags & FL_TEAMSLAVE)
				continue;

			if (!strcmp(e->Team.String, e2->Team.String))
			{
				c2++;
				chain->Team.Chain = e2;
				e2->Team.Master = e;
				e2->Team.HasTeam = true;
		
				QDelete e2->Team.String; // Free team string

				chain = e2;
				e2->Flags |= FL_TEAMSLAVE;
			}
		}

		QDelete e->Team.String; // Free team string
	}

	DebugPrintf ("%i teams with %i entities\n", c, c2);
}



#include "cc_exceptionhandler.h"
#include "cc_brushmodels.h"

void InitPlayers ()
{
	// Set up the client entities
	for (sint32 i = 1; i <= game.MaxClients; i++)
	{
		edict_t *ent = &g_edicts[i];

		if (!ent->Entity)
			ent->Entity = QNewEntityOf CPlayerEntity(i);
	}
}

void InitEntities ()
{
	// Set up the world
	edict_t *theWorld = &g_edicts[0];
	if (!theWorld->Entity)
		theWorld->Entity = QNewEntityOf CWorldEntity(0);

	InitPlayers();
}

char *gEntString;

/*
==============
SpawnEntities

Creates a server's entity / program execution context by
parsing textual entity definitions out of an ent file.
==============
*/

void ShutdownBodyQueue ();
void InitVersion ();
void InitEntityLists ();
void DeallocateEntities ();
void SetClientFields ();

void CGameAPI::SpawnEntities (char *ServerLevelName, char *entities, char *spawnpoint)
{
	CTimer Timer;

	level.EntityNumber = 0;
	InitMapCounter();

	sint32 skill_level = Clamp (skill->Integer(), 0, 3);
	if (skill->Integer() != skill_level)
		skill->Set(skill_level, true);

	CPlayerEntity::BackupClientData ();

	if (Q_stricmp (ServerLevelName + strlen(ServerLevelName) - 4, ".cin") == 0 || Q_stricmp (ServerLevelName + strlen(ServerLevelName) - 4, ".dm2") == 0)
	{
		level.Demo = true;
		ShutdownBodyQueue ();
		Shutdown_Junk ();
	}

	// Deallocate entities
	DeallocateEntities ();

	level.Clear ();

	Mem_FreePool (com_levelPool);
	gEntString = Mem_PoolStrDup(entities, com_levelPool, 0);

	char *oldEntities = entities;
	bool FreeIt = false;
	entities = CC_ParseSpawnEntities (ServerLevelName, entities);
	if (oldEntities != entities)
		FreeIt = true;

#if MONSTERS_USE_PATHFINDING
	if (!level.Demo)
		InitNodes ();
#endif

	memset (g_edicts, 0, game.MaxEntities * sizeof(g_edicts[0]));
	InitEntityLists ();
	ClearTimers ();

	level.ServerLevelName = ServerLevelName;

	game.SpawnPoint = spawnpoint;

	InitEntities ();

	// set client fields on player ents
	CPlayerEntity::RestoreClientData ();

	if (!level.Demo)
	{
		level.Inhibit = 0;

		// Parse ents
		CParser EntityParser (entities, PSP_COMMENT_MASK);

		bool SpawnedWorld = false;
		while (true)
		{
			// Clear classname
			level.ClassName.clear ();

			// Parse the opening brace
			const char *token;

			if (!EntityParser.ParseToken (PSF_ALLOW_NEWLINES, &token))
				break;

			if (token[0] != '{')
				GameError ("ED_LoadFromFile: found %s when expecting {", token);

	_CC_DISABLE_DEPRECATION
			edict_t *ent = (!SpawnedWorld) ? g_edicts : G_Spawn();
	_CC_ENABLE_DEPRECATION
			SpawnedWorld = true;

			ED_ParseEdict (EntityParser, ent);

			ED_CallSpawn (ent);
			level.EntityNumber++;

			if (!ent->inUse)
			{
				level.Inhibit++;
				if (ent->Entity && !ent->Entity->Freed)
					_CC_ASSERT_EXPR (0, "Entity not inuse but freed!");
			}
		}

		DebugPrintf ("%i entities removed (out of %i total)\n", level.Inhibit, level.EntityNumber);

		G_FindTeams ();

	#if MONSTERS_USE_PATHFINDING
		LoadNodes ();
		FinalizeNodes ();
		//LoadPathTable ();
	#endif

		SetupTechSpawn();

	#if CLEANCTF_ENABLED
	//ZOID
		CTFSpawn();
	//ZOID
	#endif
	}
	else
		DebugPrintf ("Demo detected, skipping map init.\n");

#if !NO_VERSION_CHECKING
	InitVersion ();
#endif
	DebugPrintf ("Finished server initialization in "TIMER_STRING"\n", Timer.Get());

	if (FreeIt)
		QDelete entities;
}
