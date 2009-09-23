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

CEntityList EntityList;

void AddToList_Test (CClassnameToClassIndex *const Index)
{
	EntityList.AddToList (Index);
}

CClassnameToClassIndex::CClassnameToClassIndex (CMapEntity				*(*Spawn) (int Index), char *Classname) :
Spawn(Spawn),
Classname(Classname)
{
	hashValue = Com_HashGeneric(Classname, MAX_CLASSNAME_CLASSES_HASH);
	AddToList_Test (this);
};

CEntityList::CEntityList ()
{
};

void CEntityList::Clear ()
{
	memset (EntityList, 0, sizeof(EntityList));
	memset (HashedEntityList, 0, sizeof(HashedEntityList));
	numEntities = 0;
};

void CEntityList::AddToList (CClassnameToClassIndex *const Entity)
{
	EntityList[numEntities] = Entity;

	// Link it in the hash tree
	EntityList[numEntities]->hashNext = HashedEntityList[EntityList[numEntities]->hashValue];
	HashedEntityList[EntityList[numEntities]->hashValue] = EntityList[numEntities];
	numEntities++;
};

void SpawnWorld ();
CBaseEntity *CEntityList::Resolve (edict_t *ent)
{
	CClassnameToClassIndex *Entity;
	uint32 hash = Com_HashGeneric(ent->classname, MAX_CLASSNAME_CLASSES_HASH);

	for (Entity = HashedEntityList[hash]; Entity; Entity=Entity->hashNext)
	{
		if (Q_stricmp(Entity->Classname, ent->classname) == 0)
			return Entity->Spawn(ent->state.number);
	}

	if (Q_stricmp(ent->classname, "worldspawn") == 0)
	{
		SpawnWorld ();
		return g_edicts[0].Entity;
	}

	return NULL;
}

CBaseEntity *ResolveMapEntity (edict_t *ent)
{
	return EntityList.Resolve (ent);
};