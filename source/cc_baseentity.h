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

#if !defined(__CC_BASEENTITY_H__) || !defined(INCLUDE_GUARDS)
#define __CC_BASEENTITY_H__

class CEntityState
{
private:
	entityStateOld_t	*state;

public:
	CEntityState			(entityStateOld_t *state);
	CEntityState			();

	int		&GetNumber		();

	vec3f	&GetOrigin		();
	vec3f	&GetAngles		();
	vec3f	&GetOldOrigin	();

	// Can be 1, 2, 3, or 4
	int		&GetModelIndex	(uint8 index = 1);

	int		&GetFrame		();
	int		&GetSkinNum		();

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

	struct Team_t
	{
		CBaseEntity		*Chain;
		CBaseEntity		*Master;
	} Team;

	CBaseEntity		*GroundEntity;
	int				GroundEntityLinkCount;
	uint32			SpawnFlags;
	CBaseEntity		*Enemy;
	int				ViewHeight;

	CBaseEntity ();
	CBaseEntity (int Index);
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

	int				GetNumClusters ();
	int				*GetClusterNums ();
	int				GetHeadNode ();

	// Not a reference; don't let people change it.
	int				GetAreaNum (bool second = false);

	link_t			*GetArea ();
	void			ClearArea ();

	int				GetLinkCount ();

	virtual bool	&GetInUse ();

	void			Link ();
	void			Unlink ();

	virtual void	Free ();

	// Sound functions
	bool			PlayedSounds[CHAN_MAX-1];
	void			PlaySound (EEntSndChannel channel, MediaIndex soundIndex, byte volume = 255, EAttenuation attenuation = ATTN_NORM, byte timeOfs = 0);
	void			PlayPositionedSound (vec3f origin, EEntSndChannel channel, MediaIndex soundIndex, byte volume = 255, EAttenuation attenuation = ATTN_NORM, byte timeOfs = 0);

	virtual void	BecomeExplosion (bool grenade);

	void			SetBrushModel ();
	void			CastTo (ECastFlags castFlags);
	void			StuffText (char *text);

	void			KillBox ();
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

	char *string = new char[newString.size()+1];
	Q_snprintfz (string, newString.size()+1, "%s", newString.c_str());

	return string;
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

CC_ENUM (uint32, EFieldType)
{
	FT_BOOL,			// Stores value as bool, but takes any number (non 0 = true)
	FT_CHAR,			// Stores value as sint8
	FT_BYTE,			// Stores value as uint8
	FT_SHORT,			// Stores value as int16
	FT_USHORT,			// Stores value as uint16
	FT_INT,				// Stores value as int
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
	FT_FLOAT_TO_BYTE,	// Accepted float input, stores as byte (0-255)

	// Flags
	FT_GAME_ENTITY	=	BIT(10),		// Stored in gameEntity instead of TClass
	FT_SAVABLE		=	BIT(11),		// Field will be saved/loaded
	FT_NOSPAWN		=	BIT(12),		// Field cannot be used as a spawn field
};

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
		byte *ClassOffset = ((FieldType & FT_GAME_ENTITY) ? (byte*)Entity->gameEntity : (byte*)Entity) + Offset;

		switch (StrippedFields)
		{
		case FT_BOOL:
			*((bool*)(ClassOffset)) = (atoi(Value) != 0);
			break;
		case FT_CHAR:
			*((sint8*)(ClassOffset)) = Clamp<int>(atoi(Value), SCHAR_MIN, SCHAR_MAX);
			break;
		case FT_BYTE:
			*((uint8*)(ClassOffset)) = Clamp<uint32>(atou(Value), 0, UCHAR_MAX);
			break;
		case FT_SHORT:
			*((int16*)(ClassOffset)) = Clamp<int32>(atoi(Value), SHRT_MIN, SHRT_MAX);
			break;
		case FT_USHORT:
			*((uint16*)(ClassOffset)) = Clamp<uint32>(atoi(Value), 0, USHRT_MAX);
			break;
		case FT_INT:
			*((int*)(ClassOffset)) = atoi(Value);
			break;
		case FT_UINT:
			*((uint32*)(ClassOffset)) = atou(Value);
			break;
		case FT_FLOAT:
			*((float*)(ClassOffset)) = atof(Value);
			break;
		case FT_FLOAT_TO_BYTE:
			*((byte*)(ClassOffset)) = (byte)Clamp<int> ((int)(atof(Value) * 255), 0, 255);
			break;
		case FT_VECTOR:
			{
				vec3f v;
				sscanf_s (Value, "%f %f %f", &v.X, &v.Y, &v.Z);
				memcpy (ClassOffset, &v, sizeof(float)*3);
			}
			break;
		case FT_YAWANGLE:
			{
				vec3f v (0, atof(Value), 0);
				memcpy (ClassOffset, &v, sizeof(float)*3);
			}
			break;
		case FT_IGNORE:
			break;
		case FT_LEVEL_STRING:
		case FT_GAME_STRING:
			*((char **)(ClassOffset)) = CopyStr(Value, (FieldType == FT_LEVEL_STRING) ? com_levelPool : com_gamePool);
			break;
		case FT_SOUND_INDEX:
			{
				std::cc_string temp = Value;
				if (!temp.find (".wav"))
					temp.append (".wav");

				*((MediaIndex *)(ClassOffset)) = SoundIndex (temp.c_str());
			}
			break;
		case FT_IMAGE_INDEX:
			*((MediaIndex *)(ClassOffset)) = ImageIndex (Value);
			break;
		case FT_MODEL_INDEX:
			*((MediaIndex *)(ClassOffset)) = ModelIndex (Value);
			break;
		case FT_FRAMENUMBER:
			{
				float Val = atof (Value);
				*((FrameNumber_t *)(ClassOffset)) = (Val != -1) ? (Val * 10) : -1;
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

				*((CBaseItem **)(ClassOffset)) = Item;
			}
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
		if (!(TClass::FieldsForParsing[i].FieldType & FT_NOSPAWN) && (strcmp (Key, TClass::FieldsForParsing[i].Name.c_str()) == 0))
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
		if (!(TClass::FieldsForParsing[i].FieldType & FT_NOSPAWN) && (strcmp (Key, TClass::FieldsForParsing[i].Name.c_str()) == 0))
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
const size_t FieldSize ()
{
	return (sizeof(TClass::FieldsForParsing) / sizeof(TClass::FieldsForParsing[0]));
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
	CMapEntity (int Index);

	virtual void Spawn() = 0;

	static const class CEntityField FieldsForParsing[];
	static const size_t FieldsForParsingSize;

	static const class CEntityField FieldsForParsing_Map[];
	static const size_t FieldsForParsingSize_Map;

	virtual bool			ParseField (const char *Key, const char *Value);
	void					ParseFields ();
	virtual bool			CheckValidity ();
};

#include "cc_itementity.h"
#include "cc_junk_entities.h"

#else
FILE_WARNING
#endif