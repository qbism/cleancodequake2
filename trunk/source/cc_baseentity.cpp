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

CEntityState::CEntityState () :
state(NULL)
{
};

CEntityState::CEntityState (entityStateOld_t *state) :
state(state)
{
};

int		&CEntityState::GetNumber		()
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

void	CEntityState::SetOrigin		(vec3f in)
{
	state->origin = in;
}

void	CEntityState::SetAngles		(vec3f in)
{
	state->angles = in;
}

// Can be 1, 2, 3, or 4
int		&CEntityState::GetModelIndex	(uint8 index)
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

int		&CEntityState::GetFrame		()
{
	return state->frame;
}

int	&CEntityState::GetSkinNum		()
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
	e->classname = "noclass";
	e->gravity = 1.0;
	e->state.number = e - g_edicts;
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
edict_t *G_Spawn (void)
{
	int			i;
	edict_t		*e;

	e = &g_edicts[game.maxclients+1];
	for (i = game.maxclients+1; i < globals.numEdicts; i++, e++)
	{
		// the first couple seconds of server time can involve a lot of
		// freeing and allocating, so relax the replacement policy
		if (!e->inUse && (e->freetime < 20 || level.framenum - e->freetime > 5))
		{
			if (e->Entity && e->Entity->Freed)
			{
				QDelete e->Entity;
				e->Entity = NULL;
			}

_CC_DISABLE_DEPRECATION
			G_InitEdict (e);
_CC_ENABLE_DEPRECATION

			return e;
		}
	}
	
	if (i == game.maxentities)
		GameError ("ED_Alloc: no free edicts");
		
	globals.numEdicts++;

_CC_DISABLE_DEPRECATION
	G_InitEdict (e);
_CC_ENABLE_DEPRECATION

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
	memset (ed, 0, sizeof(*ed));
	ed->Entity = Entity;
	ed->classname = "freed";
	ed->freetime = level.framenum;
	ed->inUse = false;
}


// Creating a new entity via constructor.
CBaseEntity::CBaseEntity ()
{
_CC_DISABLE_DEPRECATION
	gameEntity = G_Spawn ();
_CC_ENABLE_DEPRECATION
	gameEntity->Entity = this;

	Freed = false;
	EntityFlags |= ENT_BASE;

	State = CEntityState(&gameEntity->state);
};

CBaseEntity::CBaseEntity (int Index)
{
	if (Index < 0)
	{
		Freed = false;
		EntityFlags |= ENT_BASE;
		gameEntity = NULL;
		return;
	}

	gameEntity = &g_edicts[Index];
	gameEntity->Entity = this;
	gameEntity->state.number = Index;

	Freed = false;
	EntityFlags |= ENT_BASE;
	State = CEntityState(&gameEntity->state);
}

CBaseEntity::~CBaseEntity ()
{
	gameEntity->Entity = NULL;

_CC_DISABLE_DEPRECATION
	G_FreeEdict (gameEntity); // "delete" the entity
_CC_ENABLE_DEPRECATION
};

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

EBrushContents	CBaseEntity::GetClipmask	()
{
	return gameEntity->clipMask;
}
void			CBaseEntity::SetClipmask (EBrushContents mask)
{
	gameEntity->clipMask = mask;
}

ESolidType		CBaseEntity::GetSolid ()
{
	return gameEntity->solid;
}
void			CBaseEntity::SetSolid (ESolidType solid)
{
	gameEntity->solid = solid;
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

// Vec3f
void			CBaseEntity::SetMins (vec3f in)
{
	gameEntity->mins = in;
}
void			CBaseEntity::SetMaxs (vec3f in)
{
	gameEntity->maxs = in;
}

void			CBaseEntity::SetAbsMin (vec3f in)
{
	gameEntity->absMin = in;
}
void			CBaseEntity::SetAbsMax (vec3f in)
{
	gameEntity->absMax = in;
}
void			CBaseEntity::SetSize (vec3f in)
{
	gameEntity->size = in;
}

EServerFlags	CBaseEntity::GetSvFlags ()
{
	return gameEntity->svFlags;
}
void			CBaseEntity::SetSvFlags (EServerFlags SVFlags)
{
	gameEntity->svFlags = SVFlags;
}

int				CBaseEntity::GetAreaNum (bool second)
{
	return ((second) ? gameEntity->areaNum2 : gameEntity->areaNum);
}
void			CBaseEntity::SetAreaNum (int num, bool second)
{
	((second) ? gameEntity->areaNum2 : gameEntity->areaNum) = num;
}

link_t			*CBaseEntity::GetArea ()
{
	return &gameEntity->area;
}
void			CBaseEntity::ClearArea ()
{
	memset (&gameEntity->area, 0, sizeof(gameEntity->area));
}

int				CBaseEntity::GetLinkCount ()
{
	return gameEntity->linkCount;
}

bool			CBaseEntity::IsInUse ()
{
	return gameEntity->inUse;
}
void			CBaseEntity::SetInUse (bool inuse)
{
	gameEntity->inUse = (inuse == true) ? 1 : 0;
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
	Unlink ();

	memset (gameEntity, 0, sizeof(*gameEntity));
	gameEntity->Entity = this;
	gameEntity->classname = "freed";
	gameEntity->freetime = level.framenum;
	SetInUse(false);

	Freed = true;
}

void	CBaseEntity::PlaySound (EEntSndChannel channel, MediaIndex soundIndex, byte volume, EAttenuation attenuation, byte timeOfs)
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

void	CBaseEntity::PlayPositionedSound (vec3f origin, EEntSndChannel channel, MediaIndex soundIndex, byte volume, EAttenuation attenuation, byte timeOfs)
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
	CTrace		tr;

	while (1)
	{
		tr = CTrace (State.GetOrigin(), GetMins(), GetMaxs(), State.GetOrigin(), NULL, CONTENTS_MASK_PLAYERSOLID);
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

CMapEntity::CMapEntity (int Index) : 
CBaseEntity(Index)
{
	EntityFlags |= ENT_MAP;
};

CPrivateEntity::CPrivateEntity (int Index)
{
};

CPrivateEntity::CPrivateEntity ()
{
	InUse = true;
};

bool			CPrivateEntity::IsInUse ()
{
	return InUse;
}
void			CPrivateEntity::SetInUse (bool inuse)
{
	InUse = inuse;
}

void CPrivateEntity::Free ()
{
	Freed = true;
}

void CBaseEntity::BecomeExplosion (bool grenade)
{
	if (grenade)
		CTempEnt_Explosions::GrenadeExplosion (State.GetOrigin(), gameEntity);
	else
		CTempEnt_Explosions::RocketExplosion (State.GetOrigin(), gameEntity);
	Free ();
}

void CBaseEntity::SetBrushModel ()
{
	if (!gameEntity->model || gameEntity->model[0] != '*')
	{
		DebugPrintf ("CleanCode warning: SetBrushModel on a non-brush model!\n");
		State.GetModelIndex() = ModelIndex(gameEntity->model);
		return;
	}

_CC_DISABLE_DEPRECATION
	gi.setmodel (gameEntity, gameEntity->model);
_CC_ENABLE_DEPRECATION
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
	CEntityField ("model",			GameEntityMemberOffset(model),					FT_LEVEL_STRING | FT_GAME_ENTITY),
	CEntityField ("light",			0,												FT_IGNORE),

	CEntityField ("item",			GameEntityMemberOffset(item),					FT_ITEM | FT_GAME_ENTITY),
	CEntityField ("pathtarget",		GameEntityMemberOffset(pathtarget),				FT_LEVEL_STRING | FT_GAME_ENTITY),
	CEntityField ("team",			GameEntityMemberOffset(team),					FT_LEVEL_STRING | FT_GAME_ENTITY),
	CEntityField ("style",			GameEntityMemberOffset(style),					FT_INT | FT_GAME_ENTITY),
	CEntityField ("count",			GameEntityMemberOffset(count),					FT_INT | FT_GAME_ENTITY),
	CEntityField ("sounds",			GameEntityMemberOffset(sounds),					FT_INT | FT_GAME_ENTITY),

	CEntityField ("owner",			GameEntityMemberOffset(owner),					FT_ENTITY | FT_GAME_ENTITY | FT_NOSPAWN | FT_SAVABLE),

	// temp spawn vars -- only valid when the spawn function is called
	CEntityField ("lip",			SpawnTempMemberOffset(lip),						FT_INT | FT_SPAWNTEMP),
	CEntityField ("height",			SpawnTempMemberOffset(height),					FT_INT | FT_SPAWNTEMP),

	CEntityField ("gravity",		SpawnTempMemberOffset(gravity),					FT_LEVEL_STRING | FT_SPAWNTEMP),
	CEntityField ("sky",			SpawnTempMemberOffset(sky),						FT_LEVEL_STRING | FT_SPAWNTEMP),
	CEntityField ("skyrotate",		SpawnTempMemberOffset(skyrotate),				FT_FLOAT | FT_SPAWNTEMP),
	CEntityField ("skyaxis",		SpawnTempMemberOffset(skyaxis),					FT_VECTOR | FT_SPAWNTEMP),
	CEntityField ("nextmap",		SpawnTempMemberOffset(nextmap),					FT_LEVEL_STRING | FT_SPAWNTEMP),
};
ENTITYFIELDS_END(CMapEntity)

const CEntityField CMapEntity::FieldsForParsing_Map[] =
{
	CEntityField ("targetname",		EntityMemberOffset(CMapEntity,TargetName),		FT_LEVEL_STRING),
};
const size_t CMapEntity::FieldsForParsingSize_Map = sizeof(CMapEntity::FieldsForParsing_Map) / sizeof(CMapEntity::FieldsForParsing_Map[0]);

bool			CMapEntity::ParseField (char *Key, char *Value)
{
	if (CheckFields<CMapEntity, CBaseEntity> (this, Key, Value))
		return true;
	else
	{
		for (size_t i = 0; i < CMapEntity::FieldsForParsingSize_Map; i++)
		{
			if (!(CMapEntity::FieldsForParsing_Map[i].FieldType & FT_NOSPAWN) && (strcmp (Key, CMapEntity::FieldsForParsing_Map[i].Name) == 0))
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
	// Yet another map hack
	if (!Q_stricmp(level.mapname, "command") && !Q_stricmp(gameEntity->classname, "trigger_once") && !Q_stricmp(gameEntity->model, "*27"))
		SpawnFlags &= ~SPAWNFLAG_NOT_HARD;

	// Remove things (except the world) from different skill levels or deathmatch
	if (this != World)
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
				(((skill->Integer() == 2) || (skill->Integer() == 3)) && (SpawnFlags & SPAWNFLAG_NOT_HARD))
				)
				{
					Free ();
					return false;
				}
		}

		SpawnFlags &= ~(SPAWNFLAG_NOT_EASY|SPAWNFLAG_NOT_MEDIUM|SPAWNFLAG_NOT_HARD|SPAWNFLAG_NOT_COOP|SPAWNFLAG_NOT_DEATHMATCH);
	}
	return true;
};

void CMapEntity::ParseFields ()
{
	if (!gameEntity->ParseData || !gameEntity->ParseData->size())
		return;

	// Go through all the dictionary pairs
	{
		std::list<CKeyValuePair*>::iterator it = gameEntity->ParseData->begin();
		while (it != gameEntity->ParseData->end())
		{
			CKeyValuePair *PairPtr = (*it);
			if (ParseField (PairPtr->Key, PairPtr->Value))
				gameEntity->ParseData->erase (it++);
			else
				++it;
		}
	}

	// Since this is the last part, go through the rest of the list now
	// and report ones that are still there.
	if (gameEntity->ParseData->size())
	{
		for (std::list<CKeyValuePair*>::iterator it = gameEntity->ParseData->begin(); it != gameEntity->ParseData->end(); ++it)
		{
			CKeyValuePair *PairPtr = (*it);
			MapPrint (MAPPRINT_ERROR, this, State.GetOrigin(), "\"%s\" is not a field (value = \"%s\")\n", PairPtr->Key, PairPtr->Value);
		}
	}
	QDelete gameEntity->ParseData;
};