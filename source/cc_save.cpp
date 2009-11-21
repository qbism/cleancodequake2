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

// FIXME: Hash
std::vector<CEntityTableIndex*, std::generic_allocator <CEntityTableIndex*> > EntityTable;

CEntityTableIndex::CEntityTableIndex (const char *Name, CBaseEntity *(*FuncPtr) (sint32 index)) :
  Name(Name),
  FuncPtr(FuncPtr)
{
	EntityTable.push_back (this);
};

CBaseEntity *CreateEntityFromTable (sint32 index, const char *Name)
{
	for (size_t i = 0; i < EntityTable.size(); i++)
	{
		if (strcmp(Name, EntityTable[i]->Name) == 0)
			return EntityTable[i]->FuncPtr(index);
	}
	return NULL;
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

void WriteEntity (CFile &File, CBaseEntity *Entity)
{
	// Write the edict_t info first
	File.Write<edict_t> (*Entity->gameEntity);

	// Write special data
	if (Entity->gameEntity->state.number > game.maxclients)
	{
		sint32 OwnerNumber = -1;

		if (Entity->gameEntity->owner)
			OwnerNumber = Entity->gameEntity->owner->state.number;

		File.Write<sint32> (OwnerNumber);
	}

	WriteIndex (File, Entity->gameEntity->state.modelIndex, INDEX_MODEL);
	WriteIndex (File, Entity->gameEntity->state.modelIndex2, INDEX_MODEL);
	WriteIndex (File, Entity->gameEntity->state.modelIndex3, INDEX_MODEL);
	WriteIndex (File, Entity->gameEntity->state.modelIndex4, INDEX_MODEL);
	WriteIndex (File, Entity->gameEntity->state.sound, INDEX_SOUND);

	// Write entity stuff
	if (Entity->gameEntity->state.number > game.maxclients)
	{
		DebugPrintf ("Writing %s\n", Entity->__GetName ());
		File.WriteString (Entity->__GetName ());
	}
}

void WriteFinalizedEntity (CFile &File, CBaseEntity *Entity)
{
	Entity->WriteBaseEntity (File);

	// Call SaveFields
	Entity->SaveFields (File);
}

void WriteEntities (CFile &File)
{
	for (TEntitiesContainer::iterator it = level.Entities.Closed.begin(); it != level.Entities.Closed.end(); it++)
	{
		edict_t *ent = (*it);
		File.Write<sint32> (ent->state.number);

		CBaseEntity *Entity = ent->Entity;
		WriteEntity (File, Entity);
	}
	File.Write<sint32> (-1);

	for (TEntitiesContainer::iterator it = level.Entities.Closed.begin(); it != level.Entities.Closed.end(); it++)
		WriteFinalizedEntity (File, (*it)->Entity);
}

// Writes out gclient_t
void WriteClient (CFile &File, CPlayerEntity *Player)
{
	Player->Client.WriteClientStructure (File);
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
		DebugPrintf ("Loading %s (%i)\n", tempBuffer, number);

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
}

void ReadFinalizeEntity (CFile &File, CBaseEntity *Entity)
{
	Entity->ReadBaseEntity (File);

	// Call LoadFields
	Entity->LoadFields (File);

	// let the server rebuild world links for this ent
	Entity->ClearArea ();
	Entity->Link (); // If this passes, Entity loaded fine!
}

void ReadEntities (CFile &File)
{
	std::vector <sint32, std::generic_allocator <sint32> > LoadedNumbers;
	while (true)
	{
		sint32 number = File.Read<sint32> ();

		DebugPrintf ("Reading entity number %i\n", number);

		if (number == -1)
			break;

		LoadedNumbers.push_back (number);
		ReadEntity (File, number);
	
		if (globals.numEdicts < number + 1)
			globals.numEdicts = number + 1;	
	}

	for (size_t i = 0; i < LoadedNumbers.size(); i++)
		ReadFinalizeEntity (File, g_edicts[LoadedNumbers[i]].Entity);
}

void ReadClient (CFile &File, sint32 i)
{
	CClient::ReadClientStructure (File, i);
}

void ReadClients (CFile &File)
{
	for (uint8 i = 0; i < game.maxclients; i++)
		ReadClient (File, i);
}

void WriteLevelLocals (CFile &File)
{
	level.Save (File);
}

void ReadLevelLocals (CFile &File)
{
	level.Load (File);
}

void WriteGameLocals (CFile &File, bool autosaved)
{
	game.autosaved = autosaved;
	game.Save (File);
	game.autosaved = false;
}

void ReadGameLocals (CFile &File)
{
	game.Load (File);
}

void CC_WriteGame (char *filename, bool autosave)
{
	DebugPrintf ("Writing %sgame to %s...\n", (autosave) ? "autosaved " : "", filename);

	if (!autosave)
		CPlayerEntity::SaveClientData ();

	CFile File (filename, FILEMODE_WRITE | FILEMODE_CREATE);

	if (!File.Valid())
	{
		GameError ("Couldn't open %s", filename);
		return; // Fix to engines who don't shutdown on gi.error
	}

	File.WriteString (__DATE__);

	WriteGameLocals (File, autosave);
	WriteClients (File);
}

#include "cc_ban.h"
void InitVersion ();

void CC_ReadGame (char *filename)
{
	DebugPrintf ("Reading game from %s...\n", filename);

	// Free any game-allocated memory before us
	Mem_FreePool (com_gamePool);
	CFile File (filename, FILEMODE_READ);

	if (!File.Valid())
	{
		GameError ("Couldn't open %s", filename);
		return;
	}

	char *date = File.ReadString ();
	if (strcmp (date, __DATE__))
	{
		GameError ("Savegame from an older version.\n");
		return;
	}
	QDelete[] date;

	seedMT (time(NULL));

	g_edicts = QNew (com_gamePool, 0) edict_t[game.maxentities];
	globals.edicts = g_edicts;
	ReadGameLocals (File);

	game.clients = QNew (com_gamePool, 0) gclient_t[game.maxclients];
	ReadClients (File);

	Bans.LoadFromFile ();
	InitVersion ();
}

void LoadBodyQueue (CFile &File);
void SaveBodyQueue (CFile &File);
void LoadJunk (CFile &File);
void SaveJunk (CFile &File);

void CC_WriteLevel (char *filename)
{
	DebugPrintf ("Writing level to %s...\n", filename);
	CFile File (filename, FILEMODE_WRITE | FILEMODE_CREATE);

	if (!File.Valid())
	{
		GameError ("Couldn't open %s", filename);
		return;
	}

	// write out edict size for checking
	File.Write<size_t> (sizeof(edict_t));

	// write out a function pointer for checking
	byte *base = (byte *)CC_InitGame;
	File.Write (&base, sizeof(base));

	// write out level_locals_t
	WriteLevelLocals (File);

	// write out all the entities
	WriteEntities (File);

	// Write out systems
	SaveBodyQueue (File);
	SaveJunk (File);
}

void InitEntityLists ();
void InitEntities ();
void FireCrosslevelTrigger (CBaseEntity *Entity);

#include "cc_bodyqueue.h"

void CC_ReadLevel (char *filename)
{
	DebugPrintf ("Reading level from %s...\n", filename);
	CFile File (filename, FILEMODE_READ);

	if (!File.Valid())
	{
		GameError ("Couldn't open %s", filename);
		return;
	}

	// Shut down any systems that may need shutting down first
	ShutdownBodyQueue ();
	Shutdown_Junk ();

	// free any dynamic memory allocated by loading the level
	// base state
	Mem_FreePool (com_levelPool);
	Mem_FreePool (com_entityPool);

	// Re-initialize the systems
	BodyQueue_Init (0);
	Init_Junk ();

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

#ifdef _WIN32
	if (base != (byte *)CC_InitGame)
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

	// do any load time things at this point
	for (sint32 i = 0; i < globals.numEdicts; i++)
	{
		edict_t *ent = &g_edicts[i];

		if (!ent->inUse)
			continue;

		// fire any cross-level triggers
		if ((ent->Entity->ClassName) && strcmp(ent->Entity->ClassName, "target_crosslevel_target") == 0)
			FireCrosslevelTrigger (ent->Entity);
	}
}

//=========================================================

/*
============
WriteGame

This will be called whenever the game goes to a new level,
and when the user explicitly saves the game.

Game information include cross level data, like multi level
triggers, help computer info, and all client states.

A single player death will automatically restore from the
last save position.
============
*/
void WriteGame (char *filename, BOOL autosave)
{
#ifdef CC_USE_EXCEPTION_HANDLER
CC_EXCEPTION_HANDLER_BEGIN
#endif
	CC_WriteGame (filename, !!autosave);
#ifdef CC_USE_EXCEPTION_HANDLER
CC_EXCEPTION_HANDLER_END
#endif
#if 0
#ifdef CC_USE_EXCEPTION_HANDLER
CC_EXCEPTION_HANDLER_BEGIN
#endif

	fileHandle_t f;
	sint32		i;
	char	str[16];

	if (!autosave)
		CPlayerEntity::SaveClientData ();

	FS_OpenFile (filename, &f, FILEMODE_WRITE, false);

	if (!f)
	{
		GameError ("Couldn't open %s", filename);
		return; // Fix to engines who don't shutdown on gi.error
	}

	memset (str, 0, sizeof(str));
	Q_strncpyz (str, __DATE__, sizeof(str));
	FS_Write (str, sizeof(str), f);

	game.autosaved = autosave ? true : false;
	FS_Write (&game, sizeof(game), f);
	game.autosaved = false;

	for (i=0 ; i<game.maxclients ; i++)
		WriteClient (f, entity_cast<CPlayerEntity>(g_edicts[i+1].Entity));

	FS_CloseFile (f);

#ifdef CC_USE_EXCEPTION_HANDLER
CC_EXCEPTION_HANDLER_END
#endif
#endif
}

void InitPlayers ();
void ReadGame (char *filename)
{
#ifdef CC_USE_EXCEPTION_HANDLER
CC_EXCEPTION_HANDLER_BEGIN
#endif
	CC_ReadGame (filename);
#ifdef CC_USE_EXCEPTION_HANDLER
CC_EXCEPTION_HANDLER_END
#endif

#if 0
#ifdef CC_USE_EXCEPTION_HANDLER
CC_EXCEPTION_HANDLER_BEGIN
#endif

	fileHandle_t	f;
	char	str[16];

	//Mem_FreePool (com_gamePool);
	FS_OpenFile (filename, &f, FILEMODE_READ, false);

	if (!f)
		GameError ("Couldn't open %s", filename);

	FS_Read (str, sizeof(str), f);
	if (strcmp (str, __DATE__))
	{
		FS_CloseFile (f);
		GameError ("Savegame from an older version.\n");
		return;
	}

	g_edicts = QNew (com_gamePool, 0) edict_t[game.maxentities];
	globals.edicts = g_edicts;

	FS_Read (&game, sizeof(game), f);
	game.clients = QNew (com_gamePool, 0) gclient_t[game.maxclients];
	InitPlayers();
	for (sint32 i=0 ; i<game.maxclients ; i++)
		ReadClient (f, entity_cast<CPlayerEntity>(g_edicts[i+1].Entity));

	FS_CloseFile (f);

#ifdef CC_USE_EXCEPTION_HANDLER
CC_EXCEPTION_HANDLER_END
#endif
#endif
}

//==========================================================


#if 0
/*
==============
WriteLevelLocals

All pointer variables (except function pointers) must be handled specially.
==============
*/
void WriteLevelLocals (fileHandle_t f)
{
	field_t		*field;
	CLevelLocals		temp;

	// all of the ints, floats, and vectors stay as they are
	temp = level;

	// change the pointers to lengths or indexes
	for (field=levelfields ; field->name ; field++)
		WriteField1 (f, field, (uint8 *)&temp);

	// write the block
	FS_Write (&temp, sizeof(temp), f);

	// now write any allocated data following the edict
	for (field=levelfields ; field->name ; field++)
		WriteField2 (f, field, (uint8 *)&level);
}

/*
==============
ReadLevelLocals

All pointer variables (except function pointers) must be handled specially.
==============
*/
void ReadLevelLocals (fileHandle_t f)
{
	field_t		*field;

	FS_Read (&level, sizeof(level), f);

	for (field=levelfields ; field->name ; field++)
		ReadField (f, field, (uint8 *)&level);
}
#endif

/*
=================
WriteLevel

=================
*/
void WriteLevel (char *filename)
{
#ifdef CC_USE_EXCEPTION_HANDLER
CC_EXCEPTION_HANDLER_BEGIN
#endif
	CC_WriteLevel (filename);
#ifdef CC_USE_EXCEPTION_HANDLER
CC_EXCEPTION_HANDLER_END
#endif

#if 0
#ifdef CC_USE_EXCEPTION_HANDLER
CC_EXCEPTION_HANDLER_BEGIN
#endif

	sint32		i;
	edict_t	*ent;
	fileHandle_t	f;
	void	*base;

	FS_OpenFile (filename, &f, FILEMODE_WRITE, false);

	if (!f)
		GameError ("Couldn't open %s", filename);

	// write out edict size for checking
	i = sizeof(edict_t);
	FS_Write (&i, sizeof(i), f);

	// write out a function pointer for checking
	base = (void *)InitGame;
	FS_Write (&base, sizeof(base), f);

	// write out level_locals_t
	WriteLevelLocals (f);

	// write out all the entities
	for (i=0 ; i<globals.numEdicts ; i++)
	{
		ent = &g_edicts[i];
		if (!ent->inUse)
			continue;
		FS_Write (&i, sizeof(i), f);
		WriteEdict (f, ent);
	}
	i = -1;
	FS_Write (&i, sizeof(i), f);

	FS_CloseFile (f);

#ifdef CC_USE_EXCEPTION_HANDLER
CC_EXCEPTION_HANDLER_END
#endif
#endif
}


/*
=================
ReadLevel

SpawnEntities will allready have been called on the
level the same way it was when the level was saved.

That is necessary to get the baselines
set up identically.

The server will have cleared all of the world links before
calling ReadLevel.

No clients are connected yet.
=================
*/
void ReadLevel (char *filename)
{
#ifdef CC_USE_EXCEPTION_HANDLER
CC_EXCEPTION_HANDLER_BEGIN
#endif
	CC_ReadLevel (filename);
#ifdef CC_USE_EXCEPTION_HANDLER
CC_EXCEPTION_HANDLER_END
#endif

#if 0
#ifdef CC_USE_EXCEPTION_HANDLER
CC_EXCEPTION_HANDLER_BEGIN
#endif

	sint32		entNum;
	fileHandle_t	f;
	sint32		i;
	void	*base;
	edict_t	*ent;

	FS_OpenFile (filename, &f, FILEMODE_READ, false);

	if (!f)
		GameError ("Couldn't open %s", filename);

	// free any dynamic memory allocated by loading the level
	// base state
	Mem_FreePool (com_levelPool);
	Mem_FreePool (com_genericPool);

	// wipe all the entities
	memset (g_edicts, 0, game.maxentities*sizeof(g_edicts[0]));
	globals.numEdicts = game.maxclients+1;

	// check edict size
	FS_Read (&i, sizeof(i), f);
	if (i != sizeof(edict_t))
	{
		FS_CloseFile (f);
		GameError ("ReadLevel: mismatched edict size");
	}

	// check function pointer base address
	FS_Read (&base, sizeof(base), f);
#ifdef _WIN32
	if (base != (void *)InitGame)
	{
		//FS_CloseFile (f);
		//GameError ("ReadLevel: function pointers have moved");
	}
#else
	gi.dprintf("Function offsets %d\n", ((uint8 *)base) - ((uint8 *)InitGame));
#endif

	// load the level locals
	ReadLevelLocals (f);

	// load all the entities
	while (1)
	{
		if (FS_Read (&entNum, sizeof(entNum), f) == 0)
		{
			FS_CloseFile (f);
			GameError ("ReadLevel: failed to read entNum");
		}
		if (entNum == -1)
			break;
		if (entNum >= globals.numEdicts)
			globals.numEdicts = entNum+1;

		ent = &g_edicts[entNum];
		ReadEdict (f, ent);

		// let the server rebuild world links for this ent
		memset (&ent->area, 0, sizeof(ent->area));
		gi.linkentity (ent);
	}

	FS_CloseFile (f);

	InitPlayers ();
	// mark all clients as unconnected
	for (i=0 ; i<game.maxclients ; i++)
	{
		ent = &g_edicts[i+1];
		ent->client = game.clients + i;

		CPlayerEntity *Player = entity_cast<CPlayerEntity>(ent->Entity);
		Player->Client.Persistent.state = SVCS_FREE;
	}

	// do any load time things at this point
	for (i=0 ; i<globals.numEdicts ; i++)
	{
		ent = &g_edicts[i];

		if (!ent->inUse)
			continue;

		// fire any cross-level triggers
		if ((ent->Entity->ClassName) && strcmp(ent->Entity->ClassName, "target_crosslevel_target") == 0)
			entity_cast<CThinkableEntity>(ent->Entity)->NextThink = level.Frame + (ent->delay * 10);
	}

#ifdef CC_USE_EXCEPTION_HANDLER
CC_EXCEPTION_HANDLER_END
#endif
#endif
}
