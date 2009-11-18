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
// cc_baseentity.cpp
// Base entity class code
//

#include "cc_local.h"

#include <cctype>
#include <algorithm>

CEntityField::CEntityField (const char *Name, size_t Offset, EFieldType FieldType) :
Name(Name),
Offset(Offset),
FieldType(FieldType),
StrippedFields(FieldType & ~(FT_GAME_ENTITY | FT_SAVABLE | FT_NOSPAWN))
{
	std::transform(this->Name.begin(), this->Name.end(), this->Name.begin(),
		(sint32(*)(sint32)) std::tolower);
};

CEntityState::CEntityState () :
state(NULL)
{
};

CEntityState::CEntityState (entityStateOld_t *state) :
state(state)
{
};

sint32		&CEntityState::GetNumber		()
{
	return state->number;
}

vec3f	&CEntityState::GetOrigin		()
{
	return state->origin;
}

vec3f	&CEntityState::GetAngles		()
{
	return state->angles;
}

vec3f	&CEntityState::GetOldOrigin	()
{
	return state->oldOrigin;
}

// Can be 1, 2, 3, or 4
sint32		&CEntityState::GetModelIndex	(uint8 index)
{
	switch (index)
	{
	case 1:
		return state->modelIndex;
	case 2:
		return state->modelIndex2;
	case 3:
		return state->modelIndex3;
	case 4:
		return state->modelIndex4;
	default:
		_CC_ASSERT_EXPR(0, "index for GetModelIndex is out of bounds");
		return state->modelIndex;
	}
}

sint32		&CEntityState::GetFrame		()
{
	return state->frame;
}

sint32	&CEntityState::GetSkinNum		()
{
	return state->skinNum;
}

EEntityStateEffects	&CEntityState::GetEffects		()
{
	return state->effects;
}

EEntityStateRenderEffects		&CEntityState::GetRenderEffects	()
{
	return state->renderFx;
}

MediaIndex	&CEntityState::GetSound		()
{
	return (MediaIndex&)state->sound;
}

EEventEffect	&CEntityState::GetEvent			()
{
	return state->event;
}

void G_InitEdict (edict_t *e)
{
	e->inUse = true;
	e->state.number = e - g_edicts;
}

template <typename TCont, typename TType>
void listfill (TCont &List, TType Data, size_t numElements)
{
	List.clear ();

	for (size_t i = 0; i < numElements; i++)
		List.push_back (&Data[i]);
}

void InitEntityLists ()
{
	listfill <TEntitiesContainer, edict_t*> (level.Entities.Open, g_edicts, game.maxentities);
	level.Entities.Closed.clear();

	// Keep the first few entities in the closed list
	for (uint8 i = 0; i < (1 + game.maxclients); i++)
	{
		level.Entities.Closed.push_back (level.Entities.Open.front());
		level.Entities.Open.pop_front();
	}
}

// Removes a free entity from Open, pushes into Closed.
edict_t *GetEntityFromList ()
{
	if (level.Entities.Open.empty())
		return NULL;

	// Take entity off of list, obeying freetime
	edict_t *ent = NULL;
	TEntitiesContainer::iterator it;

	for (it = level.Entities.Open.begin(); it != level.Entities.Open.end(); it++)
	{
		edict_t *check = (*it);

		// the first couple seconds of server time can involve a lot of
		// freeing and allocating, so relax the replacement policy
		if (!check->inUse && (check->freetime < 20 || level.Frame - check->freetime > 5))
		{
			ent = check;
			break;
		}
	}

	if (ent == NULL)
		return NULL;

	level.Entities.Open.erase (it);
	
	// Put into closed
	level.Entities.Closed.push_back (ent);
	return ent; // Give it to us
}

// Removes from Open, puts into end of Closed.
void RemoveEntityFromOpen (edict_t *ent)
{
	level.Entities.Open.remove (ent);
	level.Entities.Closed.push_back (ent);
}

// Removes entity from Closed, pushes into front of into Open
void RemoveEntityFromList (edict_t *ent)
{
	// Take entity out of list
	level.Entities.Closed.remove (ent);

	// Push into Open
	level.Entities.Open.push_front (ent);
}

/*
=================
G_Spawn

Either finds a free edict, or allocates a new one.
Try to avoid reusing an entity that was recently freed, because it
can cause the client to think the entity morphed into something else
instead of being removed and recreated, which can cause interpolated
angles and bad trails.
=================
*/
edict_t *G_Spawn ()
{
	edict_t *e = GetEntityFromList ();
	
	if (e == NULL)
	{
		GameError ("ED_Alloc: no free edicts");
		return NULL;
	}

_CC_DISABLE_DEPRECATION
	G_InitEdict (e);
_CC_ENABLE_DEPRECATION

	if (globals.numEdicts < e->state.number + 1)
		globals.numEdicts = e->state.number + 1;

	return e;
}

/*
=================
G_FreeEdict

Marks the edict as free
=================
*/
void G_FreeEdict (edict_t *ed)
{
	gi.unlinkentity (ed);		// unlink from world

	// Paril, hack
	CBaseEntity *Entity = ed->Entity;
	bool oldUsedBefore = ed->usedBefore;

	memset (ed, 0, sizeof(*ed));
	if (Entity)
	{
		ed->Entity = Entity;
		Entity->ClassName = "freed";
	}
	ed->usedBefore = oldUsedBefore;
	ed->freetime = level.Frame;
	ed->inUse = false;
	ed->state.number = ed - g_edicts;

	RemoveEntityFromList (ed);
}

typedef std::vector <CBaseEntity*, std::generic_allocator<CBaseEntity*> > TPrivateEntitiesContainer;
TPrivateEntitiesContainer PrivateEntities;

void InitPrivateEntities ()
{
	PrivateEntities.clear ();
}

void			RunPrivateEntities ()
{
	TPrivateEntitiesContainer::iterator it = PrivateEntities.begin();
	while (it != PrivateEntities.end())
	{
		CBaseEntity *Entity = (*it);
		
		level.CurrentEntity = Entity;

		if (!Entity->Freed && (Entity->EntityFlags & ENT_THINKABLE)) 
			entity_cast<CThinkableEntity>(Entity)->PreThink ();

		Entity->Run ();	

		if (!Entity->Freed && (Entity->EntityFlags & ENT_THINKABLE))
			entity_cast<CThinkableEntity>(Entity)->RunThink ();

		// Were we freed?
		// This has to be processed after thinking and running, because
		// the entity still has to be intact after that
		if (Entity->Freed)
		{
			it = PrivateEntities.erase (it);
			QDelete Entity;
		}
		else
			++it;
	}
};

// Creating a new entity via constructor.
CBaseEntity::CBaseEntity ()
{
_CC_DISABLE_DEPRECATION
	gameEntity = G_Spawn ();
_CC_ENABLE_DEPRECATION
	gameEntity->Entity = this;
	ClassName = "noclass";

	Freed = false;
	EntityFlags |= ENT_BASE;

	State = CEntityState(&gameEntity->state);
};

CBaseEntity::CBaseEntity (sint32 Index)
{
	if (Index < 0)
	{
		Freed = false;
		EntityFlags |= (ENT_PRIVATE|ENT_BASE);

		gameEntity = NULL;
		PrivateEntities.push_back (this);
	}
	else
	{
		gameEntity = &g_edicts[Index];
		gameEntity->Entity = this;
		gameEntity->state.number = Index;

		Freed = false;
		EntityFlags |= ENT_BASE;
		State = CEntityState(&gameEntity->state);
	}
}

CBaseEntity::~CBaseEntity ()
{
	if (gameEntity)
	{
		gameEntity->Entity = NULL;

_CC_DISABLE_DEPRECATION
		if (!Freed && !(EntityFlags & ENT_JUNK))
			G_FreeEdict (gameEntity); // "delete" the entity
_CC_ENABLE_DEPRECATION
	}
	else
	{
		for (TPrivateEntitiesContainer::iterator it = PrivateEntities.begin(); it < PrivateEntities.end(); it++)
		{
			if ((*it) == this)
			{
				PrivateEntities.erase (it);
				break;
			}
		}
	}
};

void CBaseEntity::WriteBaseEntity (CFile &File)
{
	File.Write (&Freed, sizeof(Freed));
	File.Write (&EntityFlags, sizeof(EntityFlags));
	File.Write (&Flags, sizeof(Flags));

	size_t len = (!ClassName) ? 0 : (strlen(ClassName) + 1);
	File.Write (&len, sizeof(len));

	if (len > 1)
		File.Write (ClassName, len);

	File.Write (&Team.HasTeam, sizeof(Team.HasTeam));

	if (Team.HasTeam)
	{
		sint32 ChainNumber = Team.Chain->gameEntity->state.number;
		sint32 MasterNumber = Team.Master->gameEntity->state.number;

		File.Write (&ChainNumber, sizeof(ChainNumber));
		File.Write (&MasterNumber, sizeof(MasterNumber));
	}

	sint32 GroundEntityNumber = (GroundEntity) ? GroundEntity->gameEntity->state.number : -1;
	File.Write (&GroundEntityNumber, sizeof(GroundEntityNumber));

	File.Write (&GroundEntityLinkCount, sizeof(GroundEntityLinkCount));
	File.Write (&SpawnFlags, sizeof(SpawnFlags));

	sint32 EnemyNumber = (Enemy) ? Enemy->gameEntity->state.number : -1;
	File.Write (&EnemyNumber, sizeof(EnemyNumber));

	File.Write (&ViewHeight, sizeof(ViewHeight));
}

void CBaseEntity::ReadBaseEntity (CFile &File)
{
	File.Read (&Freed, sizeof(Freed));
	File.Read (&EntityFlags, sizeof(EntityFlags));
	File.Read (&Flags, sizeof(Flags));

	size_t len;
	File.Read (&len, sizeof(len));

	if (len > 1)
	{
		ClassName = QNew (com_levelPool, 0) char [len];
		File.Read (ClassName, len);
	}

	File.Read (&Team.HasTeam, sizeof(Team.HasTeam));

	if (Team.HasTeam)
	{
		sint32 ChainNumber;
		sint32 MasterNumber;

		File.Read (&ChainNumber, sizeof(ChainNumber));
		File.Read (&MasterNumber, sizeof(MasterNumber));

		Team.Chain = g_edicts[ChainNumber].Entity;
		Team.Master = g_edicts[MasterNumber].Entity;
	}

	sint32 GroundEntityNumber;
	File.Read (&GroundEntityNumber, sizeof(GroundEntityNumber));

	if (GroundEntityNumber == -1)
		GroundEntity = NULL;
	else
		GroundEntity = g_edicts[GroundEntityNumber].Entity;

	File.Read (&GroundEntityLinkCount, sizeof(GroundEntityLinkCount));
	File.Read (&SpawnFlags, sizeof(SpawnFlags));

	sint32 EnemyNumber;
	File.Read (&EnemyNumber, sizeof(EnemyNumber));
	if (EnemyNumber == -1)
		Enemy = NULL;
	else
		Enemy = g_edicts[EnemyNumber].Entity;

	File.Read (&ViewHeight, sizeof(ViewHeight));
}

// Funtions below are to link the private gameEntity together
CBaseEntity		*CBaseEntity::GetOwner	()
{
	return (gameEntity->owner) ? gameEntity->owner->Entity : NULL;
}
void			CBaseEntity::SetOwner	(CBaseEntity *ent)
{
	if (!ent || !ent->gameEntity)
	{
		gameEntity->owner = NULL;
		return;
	}

	gameEntity->owner = ent->gameEntity;
}

EBrushContents	&CBaseEntity::GetClipmask	()
{
	return gameEntity->clipMask;
}

ESolidType		&CBaseEntity::GetSolid ()
{
	return gameEntity->solid;
}

// Unless, of course, you use the vec3f class :D
vec3f			&CBaseEntity::GetMins ()
{
	return gameEntity->mins;
}
vec3f			&CBaseEntity::GetMaxs ()
{

	return gameEntity->maxs;
}

vec3f			&CBaseEntity::GetAbsMin ()
{
	return gameEntity->absMin;
}
vec3f			&CBaseEntity::GetAbsMax ()
{
	return gameEntity->absMax;
}
vec3f			&CBaseEntity::GetSize ()
{
	return gameEntity->size;
}

EServerFlags	&CBaseEntity::GetSvFlags ()
{
	return gameEntity->svFlags;
}

sint32				CBaseEntity::GetAreaNum (bool second)
{
	return ((second) ? gameEntity->areaNum2 : gameEntity->areaNum);
}

link_t			*CBaseEntity::GetArea ()
{
	return &gameEntity->area;
}
void			CBaseEntity::ClearArea ()
{
	memset (&gameEntity->area, 0, sizeof(gameEntity->area));
}

sint32				CBaseEntity::GetLinkCount ()
{
	return gameEntity->linkCount;
}

bool			&CBaseEntity::GetInUse ()
{
	return (bool&)gameEntity->inUse;
}

void			CBaseEntity::Link ()
{
	gi.linkentity (gameEntity);
}

void			CBaseEntity::Unlink ()
{
	gi.unlinkentity (gameEntity);
}

void			CBaseEntity::Free ()
{
	if (gameEntity)
	{
		Unlink ();

		bool oldUsedBefore = gameEntity->usedBefore;
		memset (gameEntity, 0, sizeof(*gameEntity));
		gameEntity->Entity = this;
		gameEntity->usedBefore = oldUsedBefore;
		ClassName = "freed";
		gameEntity->freetime = level.Frame;
		GetInUse() = false;
		gameEntity->state.number = gameEntity - g_edicts;

		if (!(EntityFlags & ENT_JUNK))
			RemoveEntityFromList (gameEntity);
	}

	Freed = true;
}

void	CBaseEntity::PlaySound (EEntSndChannel channel, MediaIndex soundIndex, uint8 volume, EAttenuation attenuation, uint8 timeOfs)
{
	if ((channel != CHAN_AUTO) && (channel < CHAN_MAX))
	{
		if (PlayedSounds[channel-1])
			return;
		else
			PlayedSounds[channel-1] = true;
	}

	PlaySoundFrom (this, channel, soundIndex, volume, attenuation, timeOfs);
};

void	CBaseEntity::PlayPositionedSound (vec3f origin, EEntSndChannel channel, MediaIndex soundIndex, uint8 volume, EAttenuation attenuation, uint8 timeOfs)
{
	if ((channel != CHAN_AUTO) && (channel < CHAN_MAX))
	{
		if (PlayedSounds[channel-1])
			return;
		else
			PlayedSounds[channel-1] = true;
	}

	PlaySoundAt (origin, this, channel, soundIndex, volume, attenuation, timeOfs);
};

void	CBaseEntity::KillBox ()
{
	while (1)
	{
		CTrace tr (State.GetOrigin(), GetMins(), GetMaxs(), State.GetOrigin(), NULL, CONTENTS_MASK_PLAYERSOLID);
		if (!tr.ent || !tr.Ent)
			break;

		if ((tr.Ent->EntityFlags & ENT_HURTABLE) && entity_cast<CHurtableEntity>(tr.Ent)->CanTakeDamage)
		{
			// nail it
			entity_cast<CHurtableEntity>(tr.Ent)->TakeDamage (this, this, vec3fOrigin, State.GetOrigin(),
																vec3fOrigin, 100000, 0, DAMAGE_NO_PROTECTION, MOD_TELEFRAG);
		}

		if (tr.Ent->GetSolid())
			break;
	}
};

CMapEntity::CMapEntity () : 
CBaseEntity()
{
	EntityFlags |= ENT_MAP;
};

CMapEntity::CMapEntity (sint32 Index) : 
CBaseEntity(Index)
{
	EntityFlags |= ENT_MAP;
};

#include "cc_tent.h"

void CBaseEntity::BecomeExplosion (bool grenade)
{
	if (grenade)
		CTempEnt_Explosions::GrenadeExplosion (State.GetOrigin(), this);
	else
		CTempEnt_Explosions::RocketExplosion (State.GetOrigin(), this);
	Free ();
}

void CBaseEntity::CastTo (ECastFlags CastFlags)
{
	Cast (CastFlags, this);
}

void CBaseEntity::StuffText (char *text)
{
   	WriteByte (SVC_STUFFTEXT);	        
	WriteString (text);
    CastTo (CASTFLAG_RELIABLE);	
}

ENTITYFIELDS_BEGIN(CMapEntity)
{
	CEntityField ("spawnflags",		EntityMemberOffset(CBaseEntity,SpawnFlags),		FT_UINT),
	CEntityField ("origin",			GameEntityMemberOffset(state.origin),			FT_VECTOR | FT_GAME_ENTITY),
	CEntityField ("angles",			GameEntityMemberOffset(state.angles),			FT_VECTOR | FT_GAME_ENTITY),
	CEntityField ("angle",			GameEntityMemberOffset(state.angles),			FT_YAWANGLE | FT_GAME_ENTITY),
	CEntityField ("light",			0,												FT_IGNORE),
	CEntityField ("team",			EntityMemberOffset(CBaseEntity,Team.String),	FT_LEVEL_STRING),
};
ENTITYFIELDS_END(CMapEntity)

const CEntityField CMapEntity::FieldsForParsing_Map[] =
{
	CEntityField ("targetname",		EntityMemberOffset(CMapEntity,TargetName),		FT_LEVEL_STRING),
};
const size_t CMapEntity::FieldsForParsingSize_Map = ArrayCount(CMapEntity::FieldsForParsing_Map);

bool			CMapEntity::ParseField (const char *Key, const char *Value)
{
	if (CheckFields<CMapEntity, CBaseEntity> (this, Key, Value))
		return true;
	else
	{
		for (size_t i = 0; i < CMapEntity::FieldsForParsingSize_Map; i++)
		{
			if (!(CMapEntity::FieldsForParsing_Map[i].FieldType & FT_NOSPAWN) && (strcmp (Key, CMapEntity::FieldsForParsing_Map[i].Name.c_str()) == 0))
			{
				CMapEntity::FieldsForParsing_Map[i].Create<CMapEntity> (this, Value);
				return true;
			}
		}
	}

	// Couldn't find it here
	return false;
};

bool				CMapEntity::CheckValidity ()
{
	// Remove things (except the world) from different skill levels or deathmatch
	if (this != World)
	{
		if (!map_debug->Boolean())
		{
			if (game.mode & GAME_DEATHMATCH)
			{
				if ( SpawnFlags & SPAWNFLAG_NOT_DEATHMATCH )
				{
					Free ();
					return false;
				}
			}
			else
			{
				if ( /* ((game.mode == GAME_COOPERATIVE) && (SpawnFlags & SPAWNFLAG_NOT_COOP)) || */
					((skill->Integer() == 0) && (SpawnFlags & SPAWNFLAG_NOT_EASY)) ||
					((skill->Integer() == 1) && (SpawnFlags & SPAWNFLAG_NOT_MEDIUM)) ||
					((skill->Integer() >= 2) && (SpawnFlags & SPAWNFLAG_NOT_HARD))
					)
					{
						Free ();
						return false;
					}
			}

			SpawnFlags &= ~(SPAWNFLAG_NOT_EASY|SPAWNFLAG_NOT_MEDIUM|SPAWNFLAG_NOT_HARD|SPAWNFLAG_NOT_COOP|SPAWNFLAG_NOT_DEATHMATCH);
		}
	}
	return true;
};

void CMapEntity::ParseFields ()
{
	if (!level.ParseData.size())
		return;

	// Go through all the dictionary pairs
	{
		TKeyValuePairContainer::iterator it = level.ParseData.begin();
		while (it != level.ParseData.end())
		{
			CKeyValuePair *PairPtr = (*it);
			if (ParseField (PairPtr->Key, PairPtr->Value))
				level.ParseData.erase (it++);
			else
				++it;
		}
	}

	// Since this is the last part, go through the rest of the list now
	// and report ones that are still there.
	if (level.ParseData.size())
	{
		for (TKeyValuePairContainer::iterator it = level.ParseData.begin(); it != level.ParseData.end(); ++it)
		{
			CKeyValuePair *PairPtr = (*it);
			MapPrint (MAPPRINT_ERROR, this, State.GetOrigin(), "\"%s\" is not a field (value = \"%s\")\n", PairPtr->Key, PairPtr->Value);
		}
	}
	level.ParseData.clear();
};