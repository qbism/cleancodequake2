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
// cc_save.cpp
// 
//

#include "cc_local.h"
#include "cc_exceptionhandler.h"

bool ReadingGame = false;
CClient **SaveClientData;

#define SAVE_USE_GZ

#ifdef SAVE_USE_GZ
#define SAVE_GZ_FLAGS FILEMODE_GZ | FILEMODE_COMPRESS_HIGH
#else
#define SAVE_GZ_FLAGS 0
#endif

#define MAGIC_NUMBER 0xf2843dfa

// Writes the magic number
#define WRITE_MAGIC { File.Write<uint32> (MAGIC_NUMBER); }

// Reads the magic number.
#define READ_MAGIC { if (File.Read<uint32> () != MAGIC_NUMBER) _CC_ASSERT_EXPR (0, "Magic number mismatch"); }

typedef std::multimap<size_t, size_t, std::less<size_t>, std::generic_allocator<size_t> > THashedEntityTableList;
typedef std::vector<CEntityTableIndex*, std::generic_allocator <CEntityTableIndex*> > TEntityTableList;
#define MAX_ENTITY_TABLE_HASH 256

TEntityTableList &EntityTable ()
{
	static TEntityTableList EntityTableV;
	return EntityTableV;
};
THashedEntityTableList &EntityTableHash ()
{
	static THashedEntityTableList EntityTableHashV;
	return EntityTableHashV;
};

CEntityTableIndex::CEntityTableIndex (const char *Name, CBaseEntity *(*FuncPtr) (sint32 index)) :
  Name(Name),
  FuncPtr(FuncPtr)
{
	EntityTable().push_back (this);

	// Link it in the hash tree
	EntityTableHash().insert (std::make_pair<size_t, size_t> (Com_HashGeneric (Name, MAX_ENTITY_TABLE_HASH), EntityTable().size()-1));
};

CBaseEntity *CreateEntityFromTable (sint32 index, const char *Name)
{
	uint32 hash = Com_HashGeneric(Name, MAX_ENTITY_TABLE_HASH);

	for (THashedEntityTableList::iterator it = EntityTableHash().equal_range(hash).first; it != EntityTableHash().equal_range(hash).second; ++it)
	{
		CEntityTableIndex *TableIndex = EntityTable().at((*it).second);
		if (Q_stricmp (TableIndex->Name, Name) == 0)
			return TableIndex->FuncPtr(index);
	}

	_CC_ASSERT_EXPR (0, "Couldn't find entity");
	return NULL;
};

extern bool ShuttingDownEntities;
bool RemoveAll (const edict_t *it)
{
	if (it && it->Entity && it->Entity->gameEntity && (it->state.number <= game.maxclients || it->inUse))
		QDelete it->Entity;
	return true;
}

void ClearSpawnPoints();

void DeallocateEntities ()
{
	ShuttingDownEntities = true;
	level.Entities.Closed.remove_if (RemoveAll);
	ShuttingDownEntities = false;
	Mem_FreePool (com_entityPool);
	ClearSpawnPoints ();
};

void WriteIndex (CFile &File, MediaIndex Index, EIndexType IndexType)
{
	sint32 len = 0;
	const char *str = NULL;

	if (!Index)
		len = 0;
	else
	{
		switch (IndexType)
		{
		case INDEX_MODEL:
			str = StringFromModelIndex(Index);
			break;
		case INDEX_SOUND:
			str = StringFromSoundIndex(Index);
			break;
		case INDEX_IMAGE:
			str = StringFromImageIndex(Index);
			break;
		};

		if (!str)
		{
			// SPECIAL CASE
			// World or inline model
			len = -2;
			File.Write<sint32> (len);
			File.Write<MediaIndex> (Index);

			return;
		}
		else
			len = strlen(str) + 1;
	}

	File.Write<sint32> (len);
	if (len > 1)
	{
		if (!str)
			_CC_ASSERT_EXPR (0, "How'd this happen?");
		else
			File.WriteArray (str, len);
	}
};

void ReadIndex (CFile &File, MediaIndex &Index, EIndexType IndexType)
{
	MediaIndex In = 0;
	sint32 len = File.Read<sint32> ();

	if (len == -2)
		In = File.Read<MediaIndex> ();
	else if (len > 1)
	{
		char *tempBuffer = QNew (com_genericPool, 0) char[len];
		File.ReadArray (tempBuffer, len);

		switch (IndexType)
		{
		case INDEX_MODEL:
			In = ModelIndex(tempBuffer);
			break;
		case INDEX_SOUND:
			In = SoundIndex(tempBuffer);
			break;
		case INDEX_IMAGE:
			In = ImageIndex(tempBuffer);
			break;
		};

		QDelete[] tempBuffer;
	}

	Index = In;
};

#include <typeinfo>

void WriteEntity (CFile &File, CBaseEntity *Entity)
{
	// Write the edict_t info first
	File.Write<edict_t> (*Entity->gameEntity);

	// Write special data
	if (Entity->State.GetNumber() > game.maxclients)
	{
		sint32 OwnerNumber = -1;

		if (Entity->GetOwner())
			OwnerNumber = Entity->GetOwner()->State.GetNumber();

		File.Write<sint32> (OwnerNumber);
	}

	WriteIndex (File, Entity->State.GetModelIndex(), INDEX_MODEL);
	WriteIndex (File, Entity->State.GetModelIndex(2), INDEX_MODEL);
	WriteIndex (File, Entity->State.GetModelIndex(3), INDEX_MODEL);
	WriteIndex (File, Entity->State.GetModelIndex(4), INDEX_MODEL);
	WriteIndex (File, Entity->State.GetSound(), INDEX_SOUND);

	// Write entity stuff
	if (Entity->State.GetNumber() > game.maxclients)
	{
		//DebugPrintf ("Writing %s\n", Entity->__GetName ());
		File.WriteString (Entity->__GetName ());

#if WIN32 && _DEBUG
		if (!(Entity->EntityFlags & ENT_ITEM))
		{
			if (!strstr(Q_strlwr(std::cc_string(typeid(*Entity).name())).c_str(), Q_strlwr(std::cc_string(Entity->__GetName())).c_str()))
				DebugPrintf ("%s did not write correctly (wrote as %s)\n", typeid(*Entity).name(), Entity->__GetName());
		}
#endif
	}

	WRITE_MAGIC
}

void WriteFinalizedEntity (CFile &File, CBaseEntity *Entity)
{
	Entity->WriteBaseEntity (File);

	// Call SaveFields
	Entity->SaveFields (File);

	WRITE_MAGIC
}

void WriteEntities (CFile &File)
{
	for (TEntitiesContainer::iterator it = level.Entities.Closed.begin(); it != level.Entities.Closed.end(); it++)
	{
		edict_t *ent = (*it);
		if (!ent->Entity || !ent->Entity->Savable())
			continue;

		File.Write<sint32> (ent->state.number);

		CBaseEntity *Entity = ent->Entity;
		WriteEntity (File, Entity);
	}
	File.Write<sint32> (-1);

	for (TEntitiesContainer::iterator it = level.Entities.Closed.begin(); it != level.Entities.Closed.end(); it++)
	{
		CBaseEntity *Entity = (*it)->Entity;
		if (!Entity || !Entity->Savable())
			continue;
		WriteFinalizedEntity (File, Entity);
	}

	WRITE_MAGIC
}

// Writes out gclient_t
void WriteClient (CFile &File, CPlayerEntity *Player)
{
	Player->Client.WriteClientStructure (File);
	Player->Client.Write (File);

	WRITE_MAGIC
}

void WriteClients (CFile &File)
{
	for (sint8 i = 1; i <= game.maxclients; i++)
		WriteClient (File, entity_cast<CPlayerEntity>(g_edicts[i].Entity));
}

void ReadEntity (CFile &File, sint32 number)
{
	// Get the edict_t for this number
	edict_t *ent = &g_edicts[number];

	// Restore entity
	CBaseEntity *RestoreEntity = ent->Entity;

	// Read
	*ent = File.Read<edict_t> ();

	// Make a copy
	edict_t temp (*ent);

	// Null out pointers
	ent->owner = NULL;

	// Restore entity
	ent->Entity = RestoreEntity;

	// Read special data
	if (number > game.maxclients)
	{
		sint32 OwnerNumber = File.Read<sint32> ();
		ent->owner = (OwnerNumber == -1) ? NULL : &g_edicts[OwnerNumber];
	}

	ReadIndex (File, (MediaIndex &)ent->state.modelIndex, INDEX_MODEL);
	ReadIndex (File, (MediaIndex &)ent->state.modelIndex2, INDEX_MODEL);
	ReadIndex (File, (MediaIndex &)ent->state.modelIndex3, INDEX_MODEL);
	ReadIndex (File, (MediaIndex &)ent->state.modelIndex4, INDEX_MODEL);
	ReadIndex (File, (MediaIndex &)ent->state.sound, INDEX_SOUND);

	// Read entity stuff
	if (number > game.maxclients)
	{
		CBaseEntity *Entity;
		char *tempBuffer = File.ReadString ();
		//DebugPrintf ("Loading %s (%i)\n", tempBuffer, number);

		Entity = CreateEntityFromTable (number, tempBuffer);
		QDelete[] tempBuffer;

		// Revision:
		// This will actually change some base members..
		// Restore them all here!
		edict_t *oldOwner = ent->owner;
		memcpy (ent, &temp, sizeof(edict_t));
		ent->owner = oldOwner;

		ent->Entity = Entity;
	}

	READ_MAGIC
}

void ReadFinalizeEntity (CFile &File, CBaseEntity *Entity)
{
	Entity->ReadBaseEntity (File);

	// Call LoadFields
	Entity->LoadFields (File);

	// let the server rebuild world links for this ent
	Entity->ClearArea ();
	Entity->Link (); // If this passes, Entity loaded fine!

	READ_MAGIC
}

void ReadEntities (CFile &File)
{
	std::vector <sint32, std::generic_allocator <sint32> > LoadedNumbers;
	while (true)
	{
		sint32 number = File.Read<sint32> ();

		//DebugPrintf ("Reading entity number %i\n", number);

		if (number == -1)
			break;

		LoadedNumbers.push_back (number);
		ReadEntity (File, number);
	
		if (Game.GetNumEdicts() < number + 1)
			Game.GetNumEdicts() = number + 1;	
	}

	// Here, load any systems that need entity lists
#if MONSTERS_USE_PATHFINDING
	LoadNodes ();
	//LoadPathTable ();
#endif

	for (size_t i = 0; i < LoadedNumbers.size(); i++)
		ReadFinalizeEntity (File, g_edicts[LoadedNumbers[i]].Entity);

#if MONSTERS_USE_PATHFINDING
	FinalizeNodes ();
#endif

	World = g_edicts[0].Entity;

	READ_MAGIC
}

void ReadClient (CFile &File, sint32 i)
{
	CClient::ReadClientStructure (File, i);
	SaveClientData[i]->Load (File);

	READ_MAGIC
}

void ReadClients (CFile &File)
{
	SaveClientData = QNew (com_genericPool, 0) CClient*[game.maxclients];
	for (uint8 i = 0; i < game.maxclients; i++)
	{
		SaveClientData[i] = QNew (com_genericPool, 0) CClient(g_edicts[1+i].client);
		ReadClient (File, i);
	}
}

void WriteLevelLocals (CFile &File)
{
	level.Save (File);

	WRITE_MAGIC
}

void ReadLevelLocals (CFile &File)
{
	level.Load (File);

	READ_MAGIC
}

void WriteGameLocals (CFile &File, bool autosaved)
{
	game.autosaved = autosaved;
	game.Save (File);
	game.autosaved = false;

	WRITE_MAGIC
}

void ReadGameLocals (CFile &File)
{
	game.Load (File);
	_CC_ASSERT_EXPR ((game.mode == GAME_COOPERATIVE), "Loaded game in non-coop?");

	READ_MAGIC
}

void CGameAPI::WriteGame (char *filename, bool autosave)
{
	DebugPrintf ("Writing %sgame to %s...\n", (autosave) ? "autosaved " : "", filename);

	if (!autosave)
		CPlayerEntity::SaveClientData ();

	CFile File (filename, FILEMODE_WRITE | FILEMODE_CREATE | SAVE_GZ_FLAGS);

	if (!File.Valid())
	{
		GameError ("Couldn't open %s", filename);
		return; // Fix to engines who don't shutdown on gi.error
	}

	File.WriteString (BuildDate());

	WriteGameLocals (File, autosave);
	WriteClients (File);

	WRITE_MAGIC
}

#include "cc_ban.h"
void InitVersion ();

void CGameAPI::ReadGame (char *filename)
{
	ReadingGame = true;
	DebugPrintf ("Reading game from %s...\n", filename);

	// Free any game-allocated memory before us
	Mem_FreePool (com_gamePool);
	CFile File (filename, FILEMODE_READ | SAVE_GZ_FLAGS);

	if (!File.Valid())
	{
		GameError ("Couldn't open %s", filename);
		return;
	}

	char *date = File.ReadString ();
	if (strcmp (date, BuildDate()))
	{
		GameError ("Savegame from an older version.\n");
		return;
	}
	QDelete[] date;

	seedMT (time(NULL));

	g_edicts = QNew (com_gamePool, 0) edict_t[game.maxentities];
	Game.SetEntities (g_edicts);
	ReadGameLocals (File);

	game.clients = QNew (com_gamePool, 0) gclient_t[game.maxclients];
	for (uint8 i = 0; i < game.maxclients; i++)
	{
		edict_t *ent = &g_edicts[i+1];
		ent->client = game.clients + i;
	}

	ReadClients (File);

	Bans.LoadFromFile ();
	InitVersion ();

	READ_MAGIC
}

void LoadBodyQueue (CFile &File);
void SaveBodyQueue (CFile &File);
void LoadJunk (CFile &File);
void SaveJunk (CFile &File);

void CGameAPI::WriteLevel (char *filename)
{
	DebugPrintf ("Writing level to %s...\n", filename);
	CFile File (filename, FILEMODE_WRITE | FILEMODE_CREATE | SAVE_GZ_FLAGS);

	if (!File.Valid())
	{
		GameError ("Couldn't open %s", filename);
		return;
	}

	// write out edict size for checking
	File.Write<size_t> (sizeof(edict_t));

	// write out a function pointer for checking
	byte *base = (byte *)ReadClient;
	File.Write (&base, sizeof(base));

	// write out level_locals_t
	WriteLevelLocals (File);

	// write out all the entities
	WriteEntities (File);

	// Write out systems
	SaveBodyQueue (File);
	SaveJunk (File);

	WRITE_MAGIC
}

void InitEntityLists ();
void InitEntities ();
#if MONSTERS_USE_PATHFINDING
void InitNodes ();
void ShutdownNodes ();
#endif
void FireCrosslevelTrigger (CBaseEntity *Entity);

#include "cc_bodyqueue.h"

char	ReadConfigSt[MAX_CFGSTRINGS][MAX_CFGSTRLEN];
void ReadConfigStrings (char *filename)
{
	size_t len = strlen(filename);

	std::cc_string temp = filename;
	temp[len-3] = 's';
	temp[len-2] = 'v';
	temp[len-1] = '2';

	CFile File (temp.c_str(), FILEMODE_READ);

	File.Read (ReadConfigSt, sizeof(ReadConfigSt));

	// Pump to the indexing system
	ClearList ();
	ListConfigstrings ();
}

void SetClientFields ()
{
	if (ReadingGame)
	{
		ReadingGame = false;

		for (uint8 i = 0; i < game.maxclients; i++)
		{
			CPlayerEntity *Player = entity_cast<CPlayerEntity>(g_edicts[i+1].Entity);
			Player->Client = *SaveClientData[i];
			Player->Client.RepositionClient (g_edicts[i+1].client);
			QDelete SaveClientData[i];
		}
		QDelete[] SaveClientData;
		SaveClientData = NULL;
	}
}

void CGameAPI::ReadLevel (char *filename)
{
	// Load configstrings
	ReadConfigStrings (filename);

	DebugPrintf ("Reading level from %s...\n", filename);
	CFile File (filename, FILEMODE_READ | SAVE_GZ_FLAGS);

	if (!File.Valid())
	{
		GameError ("Couldn't open %s", filename);
		return;
	}

	// Shut down any systems that may need shutting down first
	ShutdownBodyQueue ();
	Shutdown_Junk ();
#if MONSTERS_USE_PATHFINDING
	ShutdownNodes ();
#endif

	memset (&GameMedia, 0, sizeof(GameMedia));
	InitGameMedia ();

	if (!ReadingGame)
		CPlayerEntity::BackupClientData ();

	// Deallocate entities
	DeallocateEntities ();
	// free any dynamic memory allocated by loading the level
	// base state
	Mem_FreePool (com_levelPool);

	// Re-initialize the systems
	BodyQueue_Init (0);
	Init_Junk ();
#if MONSTERS_USE_PATHFINDING
	InitNodes ();
#endif

	// wipe all the entities
	memset (g_edicts, 0, game.maxentities*sizeof(g_edicts[0]));

	InitEntityLists ();

	InitEntities (); // Get the world and players setup

	// check edict size
	size_t size = File.Read<size_t> ();
	if (size != sizeof(edict_t))
	{
		GameError ("ReadLevel: mismatched edict size");
		return;
	}

	// check function pointer base address
	byte *base;
	File.Read (&base, sizeof(base));

#ifdef WIN32
	if (base != (byte *)ReadClient)
	{
		GameError ("ReadLevel: function pointers have moved");
		return;
	}
#else
	gi.dprintf("Function offsets %d\n", ((uint8 *)base) - ((uint8 *)InitGame));
#endif

	// load the level locals
	ReadLevelLocals (File);

	// load all the entities
	ReadEntities (File);

	// Put together systems
	LoadBodyQueue (File);
	LoadJunk (File);

	// mark all clients as unconnected
	for (uint8 i = 0; i < game.maxclients; i++)
	{
		edict_t *ent = &g_edicts[i+1];
		ent->client = game.clients + i;

		CPlayerEntity *Player = entity_cast<CPlayerEntity>(ent->Entity);
		Player->Client.RepositionClient (ent->client);
		Player->Client.Persistent.state = SVCS_FREE;
	}

	// set client fields on player ents
	if (!ReadingGame)
		CPlayerEntity::RestoreClientData ();

	// do any load time things at this point
	// FIXME: make this faster...
	for (TEntitiesContainer::iterator it = level.Entities.Closed.begin(); it != level.Entities.Closed.end(); ++it)
	{
		edict_t *ent = (*it);

		if (!ent->inUse)
			continue;

		// fire any cross-level triggers
		if ((ent->Entity->ClassName) && strcmp(ent->Entity->ClassName, "target_crosslevel_target") == 0)
			FireCrosslevelTrigger (ent->Entity);
	}

	//SetClientFields ();

	READ_MAGIC
}
