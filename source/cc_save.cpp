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
#include "cc_save.h"
#include "cc_exceptionhandler.h"

#if 0
void WriteField1 (fileHandle_t f, field_t *field, uint8 *base)
{
	void		*p;
	size_t		len;
	sint32			index;

	if (field->flags & FFL_SPAWNTEMP)
		return;

	p = (void *)(base + field->ofs);
	switch (field->type)
	{
	case F_INT:
	case F_FLOAT:
	case F_ANGLEHACK:
	case F_VECTOR:
	case F_IGNORE:
		break;

	case F_LSTRING:
	case F_GSTRING:
		if ( *(char **)p )
			len = strlen(*(char **)p) + 1;
		else
			len = 0;
		*(size_t *)p = len;
		break;
	case F_EDICT:
		if ( *(edict_t **)p == NULL)
			index = -1;
		else
			index = *(edict_t **)p - g_edicts;
		*(sint32 *)p = index;
		break;
	case F_CLIENT:
		if ( *(gclient_t **)p == NULL)
			index = -1;
		else
			index = *(gclient_t **)p - game.clients;
		*(sint32 *)p = index;
		break;
	case F_ITEM:
/*		if ( *(edict_t **)p == NULL)
			index = -1;
		else
			index = *(gitem_t **)p - itemlist;
		*(sint32 *)p = index;*/
		break;
	case F_NEWITEM:
		if ( *(edict_t **)p == NULL)
			index = -1;
		else
		{
			CWeapon *Weap = static_cast<CWeapon *>(p);
			CBaseItem *Item = Weap->Item;
			index = Item->GetIndex();
		}
		*(sint32 *)p = -1;
		break;

	//relative to code segment
	case F_FUNCTION:
		if (*(uint8 **)p == NULL)
			index = 0;
		else
			index = *(uint8 **)p - ((uint8 *)InitGame);
		*(sint32 *)p = index;
		break;

	default:
		GameError ("WriteEdict: unknown field type");
	}
}


void WriteField2 (fileHandle_t f, field_t *field, uint8 *base)
{
	size_t		len;
	void		*p;

	if (field->flags & FFL_SPAWNTEMP)
		return;

	p = (void *)(base + field->ofs);
	switch (field->type)
	{
	case F_LSTRING:
		if ( *(char **)p )
		{
			len = strlen(*(char **)p) + 1;
			FS_Write (*(char **)p, len, f);
		}
		break;
	}
}

void ReadField (fileHandle_t f, field_t *field, uint8 *base)
{
	void		*p;
	sint32			len;
	sint32			index;

	if (field->flags & FFL_SPAWNTEMP)
		return;

	p = (void *)(base + field->ofs);
	switch (field->type)
	{
	case F_INT:
	case F_FLOAT:
	case F_ANGLEHACK:
	case F_VECTOR:
	case F_IGNORE:
		break;

	case F_LSTRING:
		len = *(sint32 *)p;
		if (!len)
			*(char **)p = NULL;
		else
		{
			*(char **)p = QNew (com_levelPool, 0) char[len];//(char*)gi.TagMalloc (len, TAG_LEVEL);
			FS_Read (*(char **)p, len, f);
		}
		break;
	case F_EDICT:
		index = *(sint32 *)p;
		if ( index == -1 )
			*(edict_t **)p = NULL;
		else
			*(edict_t **)p = &g_edicts[index];
		break;
	case F_CLIENT:
		index = *(sint32 *)p;
		if ( index == -1 )
			*(gclient_t **)p = NULL;
		else
			*(gclient_t **)p = &game.clients[index];
		break;
	case F_ITEM:
/*		index = *(sint32 *)p;
		if ( index == -1 )
			*(gitem_t **)p = NULL;
		else
			*(gitem_t **)p = &itemlist[index];*/
		break;
	case F_NEWITEM:
		index = *(sint32 *)p;
		if ( index == -1 )
			*(CWeapon **)p = NULL;
		else
		{
		}
		break;
	//relative to code segment
	case F_FUNCTION:
		index = *(sint32 *)p;
		if ( index == 0 )
			*(uint8 **)p = NULL;
		else
			*(uint8 **)p = ((uint8 *)InitGame) + index;
		break;
	default:
		GameError ("ReadEdict: unknown field type");
	}
}

//=========================================================

/*
==============
WriteClient

All pointer variables (except function pointers) must be handled specially.
==============
*/
void WriteClient (fileHandle_t f, CPlayerEntity *Player)
{
	// Write Persistent.weapon and Persistent.newweapon
	sint32 pwIndex = -1, nwIndex = -1, lwIndex = -1;

	if (Player->Client.Persistent.Weapon)
	{
		CBaseItem *Item = Player->Client.Persistent.Weapon->Item;
		pwIndex = Item->GetIndex();
	}
	if (Player->Client.Persistent.LastWeapon)
	{
		CBaseItem *Item = Player->Client.Persistent.LastWeapon->Item;
		lwIndex = Item->GetIndex();
	}
	if (Player->Client.NewWeapon)
	{
		CBaseItem *Item = Player->Client.NewWeapon->Item;
		nwIndex = Item->GetIndex();
	}

	// write the block
	FS_Write (&Player->Client, sizeof(CClient), f);

	// now write any allocated data following the edict
	/*for (field=clientfields ; field->name ; field++)
	{
		WriteField2 (f, field, (uint8 *)client);
	}*/
	FS_Write (&pwIndex, sizeof(sint32), f);
	FS_Write (&lwIndex, sizeof(sint32), f);
	FS_Write (&nwIndex, sizeof(sint32), f);
}

/*
==============
ReadClient

All pointer variables (except function pointers) must be handled specially.
==============
*/
void ReadClient (fileHandle_t f, CPlayerEntity *Player)
{
	FS_Read (&Player->Client, sizeof(CClient), f);

	sint32 pwIndex, nwIndex, lwIndex;
	FS_Read (&pwIndex, sizeof(sint32), f);
	FS_Read (&lwIndex, sizeof(sint32), f);
	FS_Read (&nwIndex, sizeof(sint32), f);

	if (pwIndex != -1)
	{
		CBaseItem *It = GetItemByIndex (pwIndex);
		if (It->Flags & ITEMFLAG_WEAPON)
		{
			if (It->Flags & ITEMFLAG_AMMO)
			{
				CAmmo *Ammo = static_cast<CAmmo*>(It);
				Player->Client.Persistent.Weapon = Ammo->Weapon;
			}
			else
			{
				CWeaponItem *Weapon = static_cast<CWeaponItem*>(It);
				Player->Client.Persistent.Weapon = Weapon->Weapon;
			}
		}
	}
	else
		Player->Client.Persistent.Weapon = NULL;

	if (lwIndex != -1)
	{
		CBaseItem *It = GetItemByIndex (lwIndex);
		if (It->Flags & ITEMFLAG_WEAPON)
		{
			if (It->Flags & ITEMFLAG_AMMO)
			{
				CAmmo *Ammo = static_cast<CAmmo*>(It);
				Player->Client.Persistent.LastWeapon = Ammo->Weapon;
			}
			else
			{
				CWeaponItem *Weapon = static_cast<CWeaponItem*>(It);
				Player->Client.Persistent.LastWeapon = Weapon->Weapon;
			}
		}
	}
	else
		Player->Client.Persistent.LastWeapon = NULL;

	if (nwIndex != -1)
	{
		CBaseItem *It = GetItemByIndex (nwIndex);
		if (It->Flags & ITEMFLAG_WEAPON)
		{
			if (It->Flags & ITEMFLAG_AMMO)
			{
				CAmmo *Ammo = static_cast<CAmmo*>(It);
				Player->Client.NewWeapon = Ammo->Weapon;
			}
			else
			{
				CWeaponItem *Weapon = static_cast<CWeaponItem*>(It);
				Player->Client.NewWeapon = Weapon->Weapon;
			}
		}
	}
	else
		Player->Client.NewWeapon = NULL;
}
#endif

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
WriteEdict

All pointer variables (except function pointers) must be handled specially.
==============
*/
void WriteEdict (fileHandle_t f, edict_t *ent)
{
	field_t		*field;
	edict_t		temp;

	// all of the ints, floats, and vectors stay as they are
	temp = *ent;

	// change the pointers to lengths or indexes
	for (field=fields ; field->name ; field++)
		WriteField1 (f, field, (uint8 *)&temp);

	// write the block
	FS_Write (&temp, sizeof(temp), f);

	// now write any allocated data following the edict
	for (field=fields ; field->name ; field++)
		WriteField2 (f, field, (uint8 *)ent);

	// Write the entity, if one
	bool hasEntity = false;
	if (ent->Entity)
		hasEntity = true;

	FS_Write (&hasEntity, sizeof(bool), f);

	if (hasEntity)
	{
		size_t sz = sizeof(*ent->Entity);
		FS_Write (&sz, sizeof(sz), f);
		FS_Write (ent->Entity, sizeof(*ent->Entity), f);
	}
}

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
ReadEdict

All pointer variables (except function pointers) must be handled specially.
==============
*/
void ReadEdict (fileHandle_t f, edict_t *ent)
{
	field_t		*field;

	FS_Read (ent, sizeof(*ent), f);

	for (field=fields ; field->name ; field++)
		ReadField (f, field, (uint8 *)ent);

	bool hasEntity = false;

	FS_Read (&hasEntity, sizeof(bool), f);

	if (hasEntity)
	{
		size_t sz;
		FS_Read (&sz, sizeof(sz), f);
		FS_Read (ent->Entity, sz, f);

		ent->Entity->gameEntity = ent;
	}
	else
		ent->Entity = NULL;
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
		if (ent->classname)
			if (strcmp(ent->classname, "target_crosslevel_target") == 0)
				// backwards compatoh you get the picture
			{
				entity_cast<CThinkableEntity>(ent->Entity)->NextThink = level.Frame + (ent->delay * 10);
			}
	}

#ifdef CC_USE_EXCEPTION_HANDLER
CC_EXCEPTION_HANDLER_END
#endif
#endif
}
