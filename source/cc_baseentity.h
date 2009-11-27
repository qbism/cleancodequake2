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
// cc_baseentity.h
// Base entity class
//

#if !defined(__CC_BASEENTITY_H__) || !INCLUDE_GUARDS
#define __CC_BASEENTITY_H__

class CEntityState
{
private:
	entityStateOld_t	*state;

public:
	CEntityState			(entityStateOld_t *state);
	CEntityState			();

	sint32		&GetNumber		();

	vec3f	&GetOrigin		();
	vec3f	&GetAngles		();
	vec3f	&GetOldOrigin	();

	// Can be 1, 2, 3, or 4
	sint32		&GetModelIndex	(uint8 index = 1);

	sint32		&GetFrame		();
	sint32		&GetSkinNum		();

	EEntityStateEffects			&GetEffects		();
	EEntityStateRenderEffects	&GetRenderEffects	();

	MediaIndex		&GetSound		();
	EEventEffect	&GetEvent			();
};

// FIXME: merge some of these flags elsewhere?
CC_ENUM (uint32, EEdictFlags)
{
	FL_FLY				= BIT(0),
	FL_SWIM				= BIT(1),
	FL_IMMUNE_LASER		= BIT(2),
	FL_INWATER			= BIT(3),
	FL_GODMODE			= BIT(4),
	FL_NOTARGET			= BIT(5),
	FL_IMMUNE_SLIME		= BIT(6),
	FL_IMMUNE_LAVA		= BIT(7),
	FL_PARTIALGROUND	= BIT(8),
	FL_TEAMSLAVE		= BIT(9),
	FL_NO_KNOCKBACK		= BIT(10),
	FL_POWER_ARMOR		= BIT(11),
	FL_RESPAWN			= BIT(12)
};

// CBaseEntity is abstract.
// A base entity can't do anything
class CBaseEntity
{
//private:
public: // Kept public for now because there are lots of functions that require edict_t
	edict_t			*gameEntity;	// The "game entity" this is linked with.
									// Kept private to make sure no mistakes are made.
public:
	bool			Freed;
	uint32			EntityFlags;
	CEntityState	State;
	EEdictFlags		Flags;
	char			*ClassName;

	struct entityTeam_t
	{
		char			*String;

		bool			HasTeam;
		CBaseEntity		*Chain;
		CBaseEntity		*Master;
	} Team;

	CBaseEntity		*GroundEntity;
	sint32			GroundEntityLinkCount;
	uint32			SpawnFlags;
	CBaseEntity		*Enemy;
	sint32			ViewHeight;

	void WriteBaseEntity (CFile &File);
	void ReadBaseEntity (CFile &File);

	CBaseEntity ();
	CBaseEntity (sint32 Index);
	virtual ~CBaseEntity ();

	virtual		bool		Run () {return true;}; // Runs the entity

	// Funtions below are to link the private gameEntity together
	CBaseEntity		*GetOwner	();
	void			SetOwner	(CBaseEntity *ent);

	EBrushContents	&GetClipmask	();
	ESolidType		&GetSolid ();

	// Unless, of course, you use the vec3f class :D
	vec3f			&GetMins ();
	vec3f			&GetMaxs ();

	vec3f			&GetAbsMin ();
	vec3f			&GetAbsMax ();
	vec3f			&GetSize ();

	EServerFlags	&GetSvFlags ();

	sint32				GetNumClusters ();
	sint32				*GetClusterNums ();
	sint32				GetHeadNode ();

	// Not a reference; don't let people change it.
	sint32				GetAreaNum (bool second = false);

	link_t			*GetArea ();
	void			ClearArea ();

	sint32				GetLinkCount ();

	virtual bool	&GetInUse ();

	void			Link ();
	void			Unlink ();

	virtual void	Free ();

	// Sound functions
	bool			PlayedSounds[CHAN_MAX-1];
	void			PlaySound (EEntSndChannel channel, MediaIndex soundIndex, uint8 volume = 255, EAttenuation attenuation = ATTN_NORM, uint8 timeOfs = 0);
	void			PlayPositionedSound (vec3f origin, EEntSndChannel channel, MediaIndex soundIndex, uint8 volume = 255, EAttenuation attenuation = ATTN_NORM, uint8 timeOfs = 0);

	virtual void	BecomeExplosion (bool grenade);

	void			CastTo (ECastFlags castFlags);
	void			StuffText (char *text);

	void			KillBox ();

	// Save functions
	// By default, all entities are savable.
	virtual inline bool Savable ()
	{
		return true;
	};

	virtual void SaveFields (CFile &File)
	{
	};

	virtual void LoadFields (CFile &File)
	{
	};

	virtual const char *__GetName () = 0;
};

// EntityFlags values
enum
{
	ENT_BASE		=	BIT(0), // Can be casted to CBaseEntity
	ENT_HURTABLE	=	BIT(1), // Can be casted to CHurtableEntity
	ENT_THINKABLE	=	BIT(2), // Can be casted to CThinkableEntity
	ENT_TOUCHABLE	=	BIT(3), // Can be casted to CTouchableEntity
	ENT_PLAYER		=	BIT(4), // Can be casted to CPlayerEntity
	ENT_MONSTER		=	BIT(5), // Can be casted to CMonsterEntity
	ENT_PHYSICS		=	BIT(6), // Can be casted to CPhysicsEntity
	ENT_BLOCKABLE	=	BIT(7), // Can be casted to CBlockableEntity
	ENT_USABLE		=	BIT(8), // Can be casted to CUsableEntity
	ENT_ITEM		=	BIT(9), // Can be casted to CItemEntity
	ENT_MAP			=	BIT(10), // Can be casted to CMapEntity
	ENT_BRUSHMODEL	=	BIT(11), // Can be casted to CBrushModel
	ENT_PRIVATE		=	BIT(12), // Can be casted to CPrivateEntity
	ENT_JUNK		=	BIT(13), // Can be casted to CJunkEntity
};

template <class TType>
inline TType *entity_cast (CBaseEntity *Entity)
{
	if (Entity == NULL)
		return NULL;

	TType *Casted = dynamic_cast<TType*> (Entity);

	if (Casted == NULL)
		_CC_ASSERT_EXPR (0, "Attempted cast of an entity uncastable to this type");

	return Casted;
}

template <>
inline CBaseEntity *entity_cast<CBaseEntity> (CBaseEntity *Entity)
{
	return Entity; // Implicit cast already done
}

inline char *CopyStr (const char *In, struct memPool_t *Pool)
{
	std::cc_string newString (In);
	
	size_t i = 0;
	while (true)
	{
		if (i >= newString.size())
			break;

		if (newString[i] == '\\' && newString[i+1] == 'n')
		{
			newString[i] = '\n';
			newString.erase (i+1, 1);
		}

		i++;
	}

	return Mem_PoolStrDup (newString.c_str(), Pool, 0);
}

inline uint32 atou (const char *Str)
{
	return (uint32)atol(Str);
}

#define EntityMemberOffset(y,x) (size_t)&(((y*)0)->x)
#define GameEntityMemberOffset(x) (size_t)&(((edict_t*)0)->x)
#define SpawnTempMemberOffset(x) (size_t)&(((spawn_temp_t*)0)->x)

// Convenience macros
// CAREFUL WITH THESE!

// Map field defs
#define ENTITYFIELD_DEFS \
	static const CEntityField FieldsForParsing[]; \
	static const size_t FieldsForParsingSize; \
	bool				ParseField (const char *Key, const char *Value);

#define ENTITYFIELD_VIRTUAL_DEFS \
	static const CEntityField FieldsForParsing[]; \
	static const size_t FieldsForParsingSize; \
	virtual bool			ParseField (const char *Key, const char *Value);

#define ENTITYFIELD_SETSIZE(x) const size_t x::FieldsForParsingSize = FieldSize<x>();

#define ENTITYFIELDS_BEGIN(c) \
	const CEntityField c::FieldsForParsing[] =
#define ENTITYFIELDS_END(c) \
	;ENTITYFIELD_SETSIZE(c)

// Save field defs
#define ENTITYFIELDS_NONSAVABLE \
	inline bool Savable () \
	{ \
		return false; \
	}; \
	void SaveFields (CFile &File) \
	{ \
	}; \
	void LoadFields (CFile &File) \
	{ \
	};
#define ENTITYFIELDS_SAVABLE(x) \
	void SaveFields (CFile &File); \
	void LoadFields (CFile &File); \
	const char *__GetName () { return TO_STRING(x); }

#define ENTITYFIELDS_NONSAVABLE_VIRTUAL(x) \
	inline virtual bool Savable () \
	{ \
		return false; \
	} \
	virtual const char *__GetName () { return NULL; }

#define ENTITYFIELDS_SAVABLE_VIRTUAL(x) \
	virtual void SaveFields (CFile &File); \
	virtual void LoadFields (CFile &File); \
	virtual const char *__GetName () { return TO_STRING(x); }

#define QNewEntityOf QNew (com_entityPool, 0) 

CC_ENUM (uint32, EFieldType)
{
	FT_BOOL,			// Stores value as bool, but takes any number (non 0 = true)
	FT_CHAR,			// Stores value as sint8
	FT_BYTE,			// Stores value as uint8
	FT_SHORT,			// Stores value as sint16
	FT_USHORT,			// Stores value as uint16
	FT_INT,				// Stores value as sint32
	FT_UINT,			// Stores value as uint32
	FT_FLOAT,			// Stores value as float
	FT_VECTOR,			// Stores value as vec3f (or float[3])
	FT_YAWANGLE,		// Only stores yaw, vec3f or float[3]
	FT_IGNORE,			// Nothing happens
	FT_LEVEL_STRING,	// String allocated on level pool
	FT_GAME_STRING,		// Ditto, on game pool
	FT_SOUND_INDEX,		// String stored as sound index
	FT_IMAGE_INDEX,		// String stored as image index
	FT_MODEL_INDEX,		// String stored as model index
	FT_FRAMENUMBER,		// Stores value as FrameNumber (val * 10)
	FT_ITEM,			// Stores value as CBaseItem (finds the item and stores it in the ptr)
	FT_ENTITY,			// Saved as an index to an entity
	FT_FLOAT_TO_BYTE,	// Accepted float input, stores as uint8 (0-255)
	FT_CC_STRING,		// String, dynamic.

	// Flags
	FT_GAME_ENTITY	=	BIT(10),		// Stored in gameEntity instead of TClass
	FT_SAVABLE		=	BIT(11),		// Field will be saved/loaded
	FT_NOSPAWN		=	BIT(12),		// Field cannot be used as a spawn field
};

#define OFS_TO_TYPE_(x,y) (*((x*)(y)))
#define OFS_TO_TYPE(x) OFS_TO_TYPE_(x,ClassOffset)

class CEntityField
{
public:
	std::cc_string	Name;
	size_t			Offset;
	EFieldType		FieldType, StrippedFields;

	CEntityField (const char *Name, size_t Offset, EFieldType FieldType);

	template <class TClass>
	void Create (TClass *Entity, const char *Value) const
	{
		uint8 *ClassOffset = ((FieldType & FT_GAME_ENTITY) ? (uint8*)Entity->gameEntity : (uint8*)Entity) + Offset;

		switch (StrippedFields)
		{
		case FT_BOOL:
			OFS_TO_TYPE(bool) = (atoi(Value) != 0);
			break;
		case FT_CHAR:
			OFS_TO_TYPE(sint8) = Clamp<sint32>(atoi(Value), SCHAR_MIN, SCHAR_MAX);
			break;
		case FT_BYTE:
			OFS_TO_TYPE(uint8) = Clamp<uint32>(atou(Value), 0, UCHAR_MAX);
			break;
		case FT_SHORT:
			OFS_TO_TYPE(sint16) = Clamp<sint32>(atoi(Value), SHRT_MIN, SHRT_MAX);
			break;
		case FT_USHORT:
			OFS_TO_TYPE(uint16) = Clamp<uint32>(atoi(Value), 0, USHRT_MAX);
			break;
		case FT_INT:
			OFS_TO_TYPE(sint32) = atoi(Value);
			break;
		case FT_UINT:
			OFS_TO_TYPE(uint32) = atou(Value);
			break;
		case FT_FLOAT:
			OFS_TO_TYPE(float) = atof(Value);
			break;
		case FT_FLOAT_TO_BYTE:
			OFS_TO_TYPE(uint8) = (uint8)Clamp<sint32> ((sint32)(atof(Value) * 255), 0, 255);
			break;
		case FT_VECTOR:
			{
				vec3f v;
				sscanf_s (Value, "%f %f %f", &v.X, &v.Y, &v.Z);
				OFS_TO_TYPE(vec3f) = v;
			}
			break;
		case FT_YAWANGLE:
			OFS_TO_TYPE(vec3f) = vec3f(0, atof(Value), 0);
			break;
		case FT_IGNORE:
			break;
		case FT_LEVEL_STRING:
		case FT_GAME_STRING:
			if (strlen(Value))
				OFS_TO_TYPE(char*) = CopyStr(Value, (FieldType == FT_LEVEL_STRING) ? com_levelPool : com_gamePool);
			else
				OFS_TO_TYPE(char*) = NULL;
			break;
		case FT_SOUND_INDEX:
			{
				std::cc_string temp = Value;
				if (temp.find (".wav") == std::cc_string::npos)
					temp.append (".wav");

				OFS_TO_TYPE(MediaIndex) = SoundIndex (temp.c_str());
			}
			break;
		case FT_IMAGE_INDEX:
			OFS_TO_TYPE(MediaIndex) = ImageIndex (Value);
			break;
		case FT_MODEL_INDEX:
			OFS_TO_TYPE(MediaIndex) = ModelIndex (Value);
			break;
		case FT_FRAMENUMBER:
			{
				float Val = atof (Value);
				OFS_TO_TYPE(FrameNumber_t) = (Val != -1) ? (Val * 10) : -1;
			}
			break;
		case FT_ITEM:
			{
				CBaseItem *Item = FindItemByClassname (Value);

				if (!Item)
					Item = FindItem (Value);

				if (!Item)
				{
					MapPrint (MAPPRINT_WARNING, Entity, Entity->State.GetOrigin(), "Bad item: \"%s\"\n", Value);
					break;
				}

				OFS_TO_TYPE(CBaseItem *) = Item;
			}
			break;
		case FT_CC_STRING:
			OFS_TO_TYPE(std::cc_string) = Value;
			break;
		};
	};

	template <class TClass>
	void Save (TClass *Entity, CFile &File) const
	{
		uint8 *ClassOffset = ((FieldType & FT_GAME_ENTITY) ? (uint8*)Entity->gameEntity : (uint8*)Entity) + Offset;

		switch (StrippedFields)
		{
		case FT_BOOL:
			File.Write<bool> (OFS_TO_TYPE(bool));
			break;
		case FT_CHAR:
			File.Write<sint8> (OFS_TO_TYPE(sint8));
			break;
		case FT_BYTE:
			File.Write<uint8> (OFS_TO_TYPE(uint8));
			break;
		case FT_SHORT:
			File.Write<sint16> (OFS_TO_TYPE(sint16));
			break;
		case FT_USHORT:
			File.Write<uint16> (OFS_TO_TYPE(uint16));
			break;
		case FT_INT:
			File.Write<sint32> (OFS_TO_TYPE(sint32));
			break;
		case FT_UINT:
			File.Write<uint32> (OFS_TO_TYPE(uint32));
			break;
		case FT_FLOAT:
			File.Write<float> (OFS_TO_TYPE(float));
			break;
		case FT_FLOAT_TO_BYTE:
			File.Write<uint8> (OFS_TO_TYPE(uint8));
			break;
		case FT_VECTOR:
			File.Write<vec3f> (OFS_TO_TYPE(vec3f));
			break;
		case FT_YAWANGLE:
		case FT_IGNORE:
			break;
		case FT_LEVEL_STRING:
		case FT_GAME_STRING:
			File.WriteString (OFS_TO_TYPE(char *));
			break;
		case FT_SOUND_INDEX:
			if (OFS_TO_TYPE(MediaIndex))
				File.WriteString (StringFromSoundIndex (OFS_TO_TYPE(MediaIndex)));
			else
				File.Write<sint32> (-1);
			break;
		case FT_IMAGE_INDEX:
			if (*((MediaIndex *)(ClassOffset)))
				File.WriteString (StringFromImageIndex (OFS_TO_TYPE(MediaIndex)));
			else
				File.Write<sint32> (-1);
		break;
		case FT_MODEL_INDEX:
			if (*((MediaIndex *)(ClassOffset)))
				File.WriteString (StringFromModelIndex (OFS_TO_TYPE(MediaIndex)));
			else
				File.Write<sint32> (-1);
			break;
		case FT_FRAMENUMBER:
			File.Write<FrameNumber_t> (OFS_TO_TYPE(FrameNumber_t));
			break;
		case FT_ITEM:
			{
				sint32 Index = -1;
				if (OFS_TO_TYPE(CBaseItem*))
					Index = OFS_TO_TYPE(CBaseItem *)->GetIndex();
				
				File.Write<sint32> (Index);
			}
			break;
		case FT_ENTITY:
			{
				sint32 Index = (OFS_TO_TYPE(CBaseEntity*) && OFS_TO_TYPE(CBaseEntity*)->gameEntity) ? OFS_TO_TYPE(CBaseEntity*)->State.GetNumber() : -1;
				File.Write<sint32> (Index);
			}
			break;
		case FT_CC_STRING:
			File.WriteCCString (OFS_TO_TYPE(std::cc_string));
			break;
		};
	};

	template <class TClass>
	void Load (TClass *Entity, CFile &File) const
	{
		uint8 *ClassOffset = ((FieldType & FT_GAME_ENTITY) ? (uint8*)Entity->gameEntity : (uint8*)Entity) + Offset;

		switch (StrippedFields)
		{
		case FT_BOOL:
			OFS_TO_TYPE(bool) = File.Read<bool> ();
			break;
		case FT_CHAR:
			OFS_TO_TYPE(sint8) = File.Read<sint8> ();
			break;
		case FT_BYTE:
			OFS_TO_TYPE(uint8) = File.Read<uint8> ();
			break;
		case FT_SHORT:
			OFS_TO_TYPE(sint16) = File.Read<sint16> ();
			break;
		case FT_USHORT:
			OFS_TO_TYPE(uint16) = File.Read<uint16> ();
			break;
		case FT_INT:
			OFS_TO_TYPE(sint32) = File.Read<sint32> ();
			break;
		case FT_UINT:
			OFS_TO_TYPE(uint32) = File.Read<uint32> ();
			break;
		case FT_FLOAT:
			OFS_TO_TYPE(float) = File.Read<float> ();
			break;
		case FT_FLOAT_TO_BYTE:
			OFS_TO_TYPE(uint8) = File.Read<uint8> ();
			break;
		case FT_VECTOR:
			OFS_TO_TYPE(vec3f) = File.Read<vec3f> ();
			break;
		case FT_YAWANGLE:
		case FT_IGNORE:
			break;
		case FT_LEVEL_STRING:
		case FT_GAME_STRING:
			OFS_TO_TYPE(char*) = File.ReadString ((FieldType == FT_LEVEL_STRING) ? com_levelPool : com_gamePool);
			break;
		case FT_SOUND_INDEX:
			{
				char *str = File.ReadString ();
				if (str)
				{
					OFS_TO_TYPE(MediaIndex) = SoundIndex (str);
					QDelete[] str;
				}
			}
			break;
		case FT_IMAGE_INDEX:
			{
				char *str = File.ReadString ();
				if (str)
				{
					OFS_TO_TYPE(MediaIndex) = ImageIndex (str);
					QDelete[] str;
				}
			}
			break;
		case FT_MODEL_INDEX:
			{
				char *str = File.ReadString ();
				if (str)
				{
					OFS_TO_TYPE(MediaIndex) = ModelIndex (str);
					QDelete[] str;
				}
			}
			break;
		case FT_FRAMENUMBER:
			OFS_TO_TYPE(FrameNumber_t) = File.Read<FrameNumber_t> ();
			break;
		case FT_ITEM:
			{
				sint32 Index = File.Read<sint32> ();
				OFS_TO_TYPE(CBaseItem *) = (Index != -1) ? GetItemByIndex(Index) : NULL;
			}
			break;
		case FT_ENTITY:
			{
				sint32 Index = File.Read<sint32> ();
				OFS_TO_TYPE(CBaseEntity *) = (Index == -1) ? NULL : g_edicts[Index].Entity;
			}
			break;
		case FT_CC_STRING:
			OFS_TO_TYPE(std::cc_string) = File.ReadCCString ();
		};
	};
};

template <class TClass, class TPassClass>
bool CheckFields (TClass *Me, const char *Key, const char *Value)
{
	for (size_t i = 0; i < TClass::FieldsForParsingSize; i++)
	{
#if (MSVS_VERSION >= VS_9)
#pragma warning (suppress : 6385 6386)
#endif
		if (TClass::FieldsForParsing[i].FieldType & FT_NOSPAWN)
			break; // if we reach a NOSPAWN we can stop

		if (strcmp (Key, TClass::FieldsForParsing[i].Name.c_str()) == 0)
		{
#if (MSVS_VERSION >= VS_9)
#pragma warning (suppress : 6385 6386)
#endif
			TClass::FieldsForParsing[i].Create<TPassClass> (Me, Value);
			return true;
		}
	}
	return false;
};

template <class TClass>
bool CheckFields (TClass *Me, const char *Key, const char *Value)
{
	for (size_t i = 0; i < TClass::FieldsForParsingSize; i++)
	{
#if (MSVS_VERSION >= VS_9)
#pragma warning (suppress : 6385 6386)
#endif
		if (TClass::FieldsForParsing[i].FieldType & FT_NOSPAWN)
			break; // if we reach a NOSPAWN we can stop

		if (strcmp (Key, TClass::FieldsForParsing[i].Name.c_str()) == 0)
		{
#if (MSVS_VERSION >= VS_9)
#pragma warning (suppress : 6385 6386)
#endif
			TClass::FieldsForParsing[i].Create<TClass> (Me, Value);
			return true;
		}
	}
	return false;
};

template <class TClass>
void SaveEntityFields (TClass *Me, CFile &File)
{
	for (size_t i = 0; i < TClass::FieldsForParsingSize; i++)
	{
#if (MSVS_VERSION >= VS_9)
#pragma warning (suppress : 6385 6386)
#endif
		if (TClass::FieldsForParsing[i].FieldType & FT_SAVABLE)
		{
#if (MSVS_VERSION >= VS_9)
#pragma warning (suppress : 6385 6386)
#endif
			TClass::FieldsForParsing[i].Save<TClass> (Me, File);
		}
	}
};

template <class TClass>
void LoadEntityFields (TClass *Me, CFile &File)
{
	for (size_t i = 0; i < TClass::FieldsForParsingSize; i++)
	{
#if (MSVS_VERSION >= VS_9)
#pragma warning (suppress : 6385 6386)
#endif
		if (TClass::FieldsForParsing[i].FieldType & FT_SAVABLE)
		{
#if (MSVS_VERSION >= VS_9)
#pragma warning (suppress : 6385 6386)
#endif
			TClass::FieldsForParsing[i].Load<TClass> (Me, File);
		}
	}
};

template <class TClass>
const size_t FieldSize ()
{
	return ArrayCount(TClass::FieldsForParsing);
}

// Base classes
#include "cc_entitytypes.h"

// Derivitives
#include "cc_playerentity.h"
#include "cc_weaponentities.h"

// An entity that can be seen via a map.
// Just to bypass the damn abstractness I did.
class CMapEntity : public virtual CBaseEntity
{
public:
	char		*Classname;
	char		*TargetName;

	CMapEntity ();
	CMapEntity (sint32 Index);
	virtual ~CMapEntity ();

	virtual void Spawn() = 0;

	static const class CEntityField FieldsForParsing[];
	static const size_t FieldsForParsingSize;

	static const class CEntityField FieldsForParsing_Map[];
	static const size_t FieldsForParsingSize_Map;

	virtual bool			ParseField (const char *Key, const char *Value);
	void					ParseFields ();
	virtual bool			CheckValidity ();

	virtual void SaveFields (CFile &File);
	virtual void LoadFields (CFile &File);
};

typedef std::list<CMapEntity*, std::generic_allocator <CMapEntity*> >	TMapEntityListType;
extern TMapEntityListType		MapEntities;

#include "cc_itementity.h"
#include "cc_junk_entities.h"

#else
FILE_WARNING
#endif