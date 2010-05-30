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

#if !defined(CC_GUARD_BASEENTITY_H) || !INCLUDE_GUARDS
#define CC_GUARD_BASEENTITY_H

/**
\class	CEntityState

\brief	Entity state class.
		Holds an SEntityState and provides functions
		to interface it.

\author	Paril
\date	29/05/2010
**/
class CEntityState
{
private:
	SEntityState			*state;	// The entity state pointer

public:
	CEntityState			(SEntityState *state);
	CEntityState			();

	void		Initialize (SEntityState *state);

	/**
	\fn	sint32 &GetNumber ()
	
	\brief	Gets the entity's number.
	
	\return	The entity number. 
	**/
	sint32		&GetNumber		();

	/**
	\fn	vec3f &GetOrigin ()
	
	\brief	Gets the entity's origin. 
	
	\return	The origin. 
	**/
	vec3f		&GetOrigin		();

	/**
	\fn	vec3f &GetAngles ()
	
	\brief	Gets the entity's angles. 
	
	\return	The angles. 
	**/
	vec3f		&GetAngles		();

	/**
	\fn	vec3f &GetOldOrigin ()
	
	\brief	Gets the entity's old origin. 
	
	\return	The old origin. 
	**/
	vec3f		&GetOldOrigin	();

	/**
	\fn	sint32 &GetModelIndex (uint8 index = 1)
	
	\brief	Gets a modelindex. 
	
	\author	Paril
	\date	29/05/2010
	
	\param	index	One-based index.
					Defaults to 1; 2 is modelIndex2, etc.
	
	\return	The modelindex. 
	**/
	sint32		&GetModelIndex	(uint8 index = 1);

	/**
	\fn	sint32 &GetFrame ()
	
	\brief	Gets the entity's animation frame. 
	
	\return	The frame. 
	**/
	sint32		&GetFrame		();

	/**
	\fn	sint32 &GetSkinNum ()
	
	\brief	Gets the entity's skin number. 
	
	\return	The skin number. 
	**/
	sint32		&GetSkinNum		();

	/**
	\fn	EEntityStateEffects &GetEffects ()
	
	\brief	Gets the entity's effects. 
	
	\return	The effects. 
	**/
	EEntityStateEffects			&GetEffects		();

	/**
	\fn	EEntityStateRenderEffects &GetRenderEffects ()
	
	\brief	Gets the entity's render effects. 
	
	\return	The render effects. 
	**/
	EEntityStateRenderEffects	&GetRenderEffects	();

	/**
	\fn	MediaIndex &GetSound ()
	
	\brief	Gets the entity's sound. 
	
	\return	The sound. 
	**/
	MediaIndex		&GetSound		();

	/**
	\fn	EEventEffect &GetEvent ()
	
	\brief	Gets the entity's event. 
	
	\return	The event. 
	**/
	EEventEffect	&GetEvent		();
};

/**
\typedef	uint32 EEdictFlags

\brief	Defines an alias representing entity flags.
**/
typedef uint32 EEdictFlags;

/**
\enum	

\brief	Values that represent entity flags. 

\todo merge some of these flags elsewhere
**/
enum
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
	FL_RESPAWN			= BIT(12),
	FL_NOGIB			= BIT(13),
	FL_MECHANICAL		= BIT(14),

#if ROGUE_FEATURES
	FL_SAM_RAIMI		= BIT(15),
	FL_DISGUISED		= BIT(16),
#endif
};

/**
\typedef	uint32 ESpawnflags

\brief	Defines an alias representing spawnflags.
		6 bits are reserved for editor flags.
		8 bits are used as power cube ID bits for coop
**/

typedef uint32 ESpawnflags;

/**
\enum	

\brief	Values that represent base spawnflags that work for every entity. 
**/
enum
{
	SPAWNFLAG_NOT_EASY			= BIT(8),
	SPAWNFLAG_NOT_MEDIUM		= BIT(9),
	SPAWNFLAG_NOT_HARD			= BIT(10),
	SPAWNFLAG_NOT_DEATHMATCH	= BIT(11),
	SPAWNFLAG_NOT_COOP			= BIT(12)
};

// IBaseEntity is abstract.
// A base entity can't do anything
class IBaseEntity
{
protected:
	edict_t			*gameEntity;	// The "game entity" this is linked with.
									// Kept private to make sure no mistakes are made.
public:
	bool			Freed;			// true if freed
	uint32			EntityFlags;	// The entity type flags
	CEntityState	State;			// The entity state
	EEdictFlags		Flags;			// The entity flags
	std::string		ClassName;		// classname of this entity

	/**
	\struct	SEntityTeam
	
	\brief	Entity team structure.
			Contains team data.
	
	\author	Paril
	\date	29/05/2010
	**/
	struct SEntityTeam
	{
		char			*String;	// Team string. Only valid during spawn frame.

		bool			HasTeam;	// true if has team
		IBaseEntity		*Chain;		// Team chain
		IBaseEntity		*Master;	// Team master
	} Team;

	IBaseEntity		*GroundEntity;			// The ground entity
	sint32			GroundEntityLinkCount;	// Number of ground entity links
	uint32			SpawnFlags;				// Entity spawnflags
	IBaseEntity		*Enemy;					// Entity's enemy
	sint32			ViewHeight;				// Entity's Viewheight

	/**
	\fn	IBaseEntity ()
	
	\brief	Default constructor. Creates a new entity from a free entity. 
	
	\author	Paril
	\date	29/05/2010
	**/
	IBaseEntity ();

	/**
	\fn	IBaseEntity (sint32 Index)
	
	\brief	Constructor. This constructor initializes an entity using a specific entity index.
			
			\attention	If writing a new entity class, you NEED (and this is 100% REQUIRED)
				to call this constructor, even if IBaseEntity isn't a direct base class. This is due to
				a C++98 limitation. 
	
	\author	Paril
	\date	29/05/2010
	
	\param	Index	Zero-based index of the. 
	**/
	IBaseEntity (sint32 Index);

	/**
	\fn	virtual ~IBaseEntity ()
	
	\brief	Destructor.
	
	\author	Paril
	\date	29/05/2010
	**/
	virtual ~IBaseEntity ();

	/**
	\fn	inline edict_t *GetGameEntity ()
	
	\brief	Gets the game entity.
	
	\return	null if it fails, else the game entity. 
	**/
	inline edict_t *GetGameEntity ()
	{
		return gameEntity;
	}

	/**
	\fn	void WriteBaseEntity (CFile &File)
	
	\brief	Writes this entity's base members to a file.
	
	\author	Paril
	\date	29/05/2010
	
	\param [in,out]	File	The file. 
	**/
	void WriteBaseEntity (CFile &File);

	/**
	\fn	void ReadBaseEntity (CFile &File)
	
	\brief	Reads this entity's base members from a file. 
	
	\author	Paril
	\date	29/05/2010
	
	\param [in,out]	File	The file. 
	**/
	void ReadBaseEntity (CFile &File);

	/**
	\fn	virtual bool Run ()
	
	\brief	Runs this entity.
			See "Entities - Run Function" for more information.
	
	\return	true if it succeeds, false if it fails. 
	**/
	virtual	bool	Run ()
	{
		return true;
	};

	/**
	\fn	IBaseEntity *GetOwner ()
	
	\brief	Gets the owner of this entity.
			See "Entities - Owners" for details.
	
	\return	null if it fails, else the entity. 
	**/
	IBaseEntity		*GetOwner	();

	/**
	\fn	void SetOwner (IBaseEntity *Entity)
	
	\brief	Sets the owner of this entity.
			See "Entities - Owners" for details.
	
	\author	Paril
	\date	29/05/2010
	
	\param [in,out]	Entity	If non-null, the new owner.
							If null, owner is set to nothing.
	**/
	void			SetOwner	(IBaseEntity *Entity);

	/**
	\fn	EBrushContents &GetClipmask ()
	
	\brief	Gets the clipmask of this entity.
	
	\return	The clipmask. 
	**/
	EBrushContents	&GetClipmask	();

	/**
	\fn	ESolidType &GetSolid ()
	
	\brief	Gets the solidity of this entity.
	
	\return	The solidity. 
	**/
	ESolidType		&GetSolid ();

	/**
	\fn	vec3f &GetMins ()
	
	\brief	Gets the min bounds of this entity. 
	
	\return	The mins. 
	**/
	vec3f			&GetMins ();

	/**
	\fn	vec3f &GetMaxs ()
	
	\brief	Gets the max bounds of this entity. 
	
	\return	The maxs. 
	**/
	vec3f			&GetMaxs ();

	/**
	\fn	vec3f &GetAbsMin ()
	
	\brief	Gets the absolute mins of this entity. 
	
	\return	The absolute mins (essentially origin + mins). 
	**/
	vec3f			&GetAbsMin ();

	/**
	\fn	vec3f &GetAbsMax ()
	
	\brief	Gets the absolute maxs of this entity. 
	
	\return	The absolute maxs (essentially origin + maxs). 
	**/
	vec3f			&GetAbsMax ();

	/**
	\fn	vec3f &GetSize ()
	
	\brief	Gets the size of this entity. 
	
	\return	The size. 
	**/
	vec3f			&GetSize ();

	/**
	\fn	EServerFlags &GetSvFlags ()
	
	\brief	Gets the server flags of this entity. 
	
	\return	The server flags. 
	**/
	EServerFlags	&GetSvFlags ();

	/**
	\fn	sint32 GetAreaNum (bool second = false)
	
	\brief	Gets this entity's area number. 
	
	\author	Paril
	\date	29/05/2010
	
	\param	second	true if you want areaNum2. 
	
	\return	areaNum2 if second, otherwise areaNum. 
	**/
	sint32			GetAreaNum (bool second = false);

	/**
	\fn	SAreaLink *GetArea ()
	
	\brief	Gets this entity's area pointer.
	
	\return	null if it fails, else the area. 
	**/
	SAreaLink		*GetArea ();

	/**
	\fn	void ClearArea ()
	
	\brief	Clears this entity's area. 
	**/
	void			ClearArea ();

	/**
	\fn	sint32 GetLinkCount ()
	
	\brief	Gets this entity's link count. 
	
	\return	The link count. 
	**/
	sint32			GetLinkCount ();

	/**
	\fn	virtual bool &GetInUse ()
	
	\brief	Gets the usage state of this entity.
	
	\return	True if in-use, false if not.
	**/
	virtual bool	&GetInUse ();

	/**
	\fn	void Link ()
	
	\brief	Links this entity to the world.
			See "Entities - Linking"
	**/
	void			Link ();

	/**
	\fn	void Unlink ()
	
	\brief	Unlinks this entity from the world.
			See "Entities - Linking"
	**/
	void			Unlink ();

	/**
	\fn	virtual void Free ()
	
	\brief	Frees this entity.
	**/
	virtual void	Free ();

	// Sound functions
	bool			PlayedSounds[CHAN_MAX-1];	// played sound channels

	/**
	\fn	void PlaySound (ESoundChannel Channel, MediaIndex Sound, uint8 Volume = 255,
		EAttenuation Attenuation = ATTN_NORM, uint8 TimeOfs = 0)
	
	\brief	Plays a sound 'soundIndex' on channel 'channel' with volume 'volume', attenuation
			'attenuation' and time offset 'timeOfs'. 
	
	\author	Paril
	\date	29/05/2010
	
	\param	Channel		The channel. 
	\param	Sound		The sound. 
	\param	Volume		The volume. 
	\param	Attenuation	The attenuation. 
	\param	TimeOfs		The time ofs. 
	**/
	void			PlaySound (ESoundChannel Channel, MediaIndex Sound, uint8 Volume = 255, EAttenuation Attenuation = ATTN_NORM, uint8 TimeOfs = 0);

	/**
	\fn	void PlayPositionedSound (vec3f Origin, ESoundChannel Channel, MediaIndex Sound,
		uint8 Volume = 255, EAttenuation Attenuation = ATTN_NORM, uint8 TimeOfs = 0)
	
	\brief	Plays a positioned sound 'soundIndex' at position 'Origin' on channel 'channel' with
			volume 'volume', attenuation 'attenuation' and time offset 'timeOfs'. 
	
	\author	Paril
	\date	29/05/2010
	
	\param	Origin		The origin. 
	\param	Channel		The channel. 
	\param	Sound		The sound. 
	\param	Volume		The volume. 
	\param	Attenuation	The attenuation. 
	\param	TimeOfs		The time offset. 
	**/
	void			PlayPositionedSound (vec3f Origin, ESoundChannel Channel, MediaIndex Sound, uint8 Volume = 255, EAttenuation Attenuation = ATTN_NORM, uint8 TimeOfs = 0);

	/**
	\fn	virtual void BecomeExplosion (bool Grenade)
	
	\brief	Visually 'turns' the entity into an explosion and frees it.
	
	\author	Paril
	\date	29/05/2010
	
	\param	Grenade	true if grenade explosion, false if rocket. 
	**/
	virtual void	BecomeExplosion (bool Grenade);

	/**
	\fn	void KillBox ()
	
	\brief	Creates a killbox around this player, instantly
			gibbing anything inside of it.
	**/
	void			KillBox ();

	/**
	\fn	void SplashDamage (IBaseEntity *Attacker, float Damage, IBaseEntity *Ignore, float Radius,
		EMeansOfDeath MeansOfDeath)
	
	\brief	Performs splash damage around this entity. 
	
	\author	Paril
	\date	29/05/2010
	
	\param [in,out]	Attacker	If non-null, the attacker. 
	\param	Damage				The damage. 
	\param [in,out]	Ignore		If non-null, the entity to ignore. 
	\param	Radius				The radius. 
	\param	MeansOfDeath		The means of death.  
	**/
	void			SplashDamage (IBaseEntity *Attacker, float Damage, IBaseEntity *Ignore, float Radius, EMeansOfDeath MeansOfDeath);

#if ROGUE_FEATURES
	/**
	\fn	void NukeSplashDamage (IBaseEntity *Attacker, float Damage, IBaseEntity *Ignore, float Radius,
		EMeansOfDeath MeansOfDeath)
	
	\brief	Performs nuke splash damage around this entity.
			Nuke damage ignores walls and makes entities not gib (vaporize).
	
	\author	Paril
	\date	29/05/2010
	
	\param [in,out]	Attacker	If non-null, the attacker. 
	\param	Damage				The damage. 
	\param [in,out]	Ignore		If non-null, the ignore. 
	\param	Radius				The radius. 
	\param	MeansOfDeath		The means of death. 
	**/
	void			NukeSplashDamage (IBaseEntity *Attacker, float Damage, IBaseEntity *Ignore, float Radius, EMeansOfDeath MeansOfDeath);
#endif

	/**
	\fn	bool CanSee (IBaseEntity *Other)
	
	\brief	Queries if we can see 'Other'.
			This is an entire bbox trace check, not just
			visibility.
	
	\author	Paril
	\date	29/05/2010
	
	\param [in,out]	Other	If non-null, the other. 
	
	\return	true if it succeeds, false if it fails. 
	**/
	bool CanSee		(IBaseEntity *Other);

	/**
	\fn	virtual inline bool Savable ()
	
	\brief	Queries if this entity is savable.
			See "Entities - Saving/Loading"
	
	\return	true if it is, false if it is not. 
	**/
	virtual inline bool Savable ()
	{
		return true;
	};

	/**
	\fn	virtual void SaveFields (CFile &File)
	
	\brief	Saves custom entity fields.
			See "Entities - Saving/Loading"
	
	\author	Paril
	\date	29/05/2010
	
	\param [in,out]	File	The file. 
	**/
	virtual void SaveFields (CFile &File)
	{
	};

	/**
	\fn	virtual void LoadFields (CFile &File)
	
	\brief	Loads custom entity fields. 
			See "Entities - Saving/Loading"
	
	\author	Paril
	\date	29/05/2010
	
	\param [in,out]	File	The file. 
	**/
	virtual void LoadFields (CFile &File)
	{
	};

	/**
	\fn	virtual const char *SAVE_GetName () = 0
	
	\brief	Gets the save name of this entity.
			Replaced by macros in other entities.
			See "Entities - Saving/Loading"
	
	\return	null if it fails, else. 
	**/
	virtual const char *SAVE_GetName () = 0;
};

/**
\typedef	uint32 EEntityFlags

\brief	Defines an alias representing entity flags.
**/
typedef uint32 EEntityFlags;

/**
\enum	

\brief	Values that represent what base classes this entity has inherited. 
**/
enum
{
	ENT_BASE		=	BIT(0), // Can be casted to IBaseEntity
	ENT_HURTABLE	=	BIT(1), // Can be casted to IHurtableEntity
	ENT_THINKABLE	=	BIT(2), // Can be casted to IThinkableEntity
	ENT_TOUCHABLE	=	BIT(3), // Can be casted to ITouchableEntity
	ENT_PLAYER		=	BIT(4), // Can be casted to CPlayerEntity
	ENT_MONSTER		=	BIT(5), // Can be casted to CMonsterEntity
	ENT_PHYSICS		=	BIT(6), // Can be casted to IPhysicsEntity
	ENT_BLOCKABLE	=	BIT(7), // Can be casted to IBlockableEntity
	ENT_USABLE		=	BIT(8), // Can be casted to IUsableEntity
	ENT_ITEM		=	BIT(9), // Can be casted to CItemEntity
	ENT_MAP			=	BIT(10), // Can be casted to IMapEntity
	ENT_BRUSHMODEL	=	BIT(11), // Can be casted to IBrushModel
	ENT_PRIVATE		=	BIT(12), // Does not have a gameEntity structure
	ENT_JUNK		=	BIT(13), // Can be casted to IJunkEntity
	ENT_NOISE		=	BIT(14), // Can be casted to CPlayerNoise
};

/**
\fn	template <class TType> inline TType *entity_cast (IBaseEntity *Entity)

\brief	Casts entity types from one type to another.

\author	Paril
\date	29/05/2010

\param [in,out]	Entity	If non-null, the entity. 

\return	null if it fails, else the entity casted to another type. 
**/
template <class TType>
inline TType *entity_cast (IBaseEntity *Entity)
{
	if (Entity == NULL)
		return NULL;

	TType *Casted = dynamic_cast<TType*> (Entity);
	CC_ASSERT_EXPR (!(Casted == NULL), "Attempted cast of an entity uncastable to this type");

	return Casted;
}

/**
\fn	template <> inline IBaseEntity *entity_cast<IBaseEntity> (IBaseEntity *Entity)

\brief	Template specialization for IBaseEntity casts.

\author	Paril
\date	29/05/2010

\typeparam	IBaseEntity	Base entity specialization. 
\param [in,out]	Entity	If non-null, the entity. 

\return	null if it fails, else. 
**/
template <>
inline IBaseEntity *entity_cast<IBaseEntity> (IBaseEntity *Entity)
{
	return Entity; // Implicit cast already done
}

/**
\fn	inline char *CopyStr (const char *In, const sint16 Tag)

\brief	Copies string 'In' under memory tag 'Tag' and returns it.
		Processes newlines as well.

\author	Paril
\date	29/05/2010

\param	In	The string. 
\param	Tag	The tag. 

\return	null if it fails, else. 
**/
inline char *CopyStr (const char *In, const EMemoryTag Tag)
{
	std::string newString (In);
	
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

	return Mem_TagStrDup (newString.c_str(), Tag);
}

/**
\fn	inline std::string CopyStr (const char *In)

\brief	Copies string 'In' and returns an std::string copy.
		Processes newlines as well.

\author	Paril
\date	29/05/2010

\param	In	The in. 

\return	The new string. 
**/
inline std::string CopyStr (const char *In)
{
	std::string newString (In);
	
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

	return newString;
}

/**
\fn	inline uint32 atou (const char *Str)

\brief	Converts a string to an unsigned integer.

\author	Paril
\date	29/05/2010

\param	Str	The string. 

\return	Unsigned integral value described by string. 
**/
inline uint32 atou (const char *Str)
{
	return (uint32)atol(Str);
}

/**
\def	EntityMemberOffset(y,x) offsetof(y,x)

\brief	Gets the byte offset of member 'x' in class 'y'

\remarks	Paril, 29/05/2010. 

\param	y	Class name. 
\param	x	Member name. 
**/
#define EntityMemberOffset(y,x) offsetof(y,x)

/**
\def	GameEntityMemberOffset(x) offsetof(edict_t,x)

\brief	Gets the byte offset of member 'x' in edict_t

\remarks	Paril, 29/05/2010. 

\param	x	Member name. 
**/
#define GameEntityMemberOffset(x) EntityMemberOffset(edict_t,x)

/**
\def	SpawnTempMemberOffset(x) offsetof(spawn_temp_t,x)

\brief	Gets the byte offset of member 'x' in spawn_temp_t 

\remarks	Paril, 29/05/2010. 

\param	x	Member name. 
**/
#define SpawnTempMemberOffset(x) EntityMemberOffset(spawn_temp_t,x)

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
	};\
	const char *SAVE_GetName () { return NULL; }

#define ENTITYFIELDS_SAVABLE(x) \
	void SaveFields (CFile &File); \
	void LoadFields (CFile &File); \
	const char *SAVE_GetName () { return TO_STRING(x); }

#define ENTITYFIELDS_INHERIT(x) \
	const char *SAVE_GetName () { return TO_STRING(x); }

#define ENTITYFIELDS_NONSAVABLE_VIRTUAL(x) \
	inline virtual bool Savable () \
	{ \
		return false; \
	} \
	virtual const char *SAVE_GetName () { return NULL; }

#define ENTITYFIELDS_SAVABLE_VIRTUAL(x) \
	virtual void SaveFields (CFile &File); \
	virtual void LoadFields (CFile &File); \
	virtual const char *SAVE_GetName () { return TO_STRING(x); }

/**
\def	QNewEntityOf

\brief	New entity of

\remarks	Paril, 29/05/2010. 
**/
#define QNewEntityOf QNew (TAG_ENTITY) 

/**
\typedef	uint32 EFieldType

\brief	Defines an alias representing type of a save/load field.
**/
typedef uint32 EFieldType;

/**
\enum	

\brief	Values that represent the type of a save/load field. Also contains flags.
**/
enum
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
	FT_SOUND_INDEX,		// String stored as sound index
	FT_IMAGE_INDEX,		// String stored as image index
	FT_MODEL_INDEX,		// String stored as model index
	FT_FRAMENUMBER,		// Stores value as FrameNumber (val * 10)
	FT_ITEM,			// Stores value as CBaseItem (finds the item and stores it in the ptr)
	FT_ENTITY,			// Saved as an index to an entity
	FT_FLOAT_TO_BYTE,	// Accepted float input, stores as uint8 (0-255)
	FT_STRING,			// String, dynamic.

	// Flags
	FT_GAME_ENTITY	=	BIT(10),		// Stored in gameEntity instead of TClass
	FT_SAVABLE		=	BIT(11),		// Field will be saved/loaded
	FT_NOSPAWN		=	BIT(12),		// Field cannot be used as a spawn field
};

/**
\def	OFS_TO_TYPE(x) (*((x*)(ClassOffset)))

\brief	Converts the pointer to a member of a class to another type.

\remarks	Paril, 29/05/2010. 

\param	x	Pointer to member of a class. 
**/
#define OFS_TO_TYPE(x) (*((x*)(ClassOffset)))

// Forward declaration, defined in cc_utils.
IBaseEntity *LoadEntity (uint32 Index);

/**
\typedef	bool (*TValidateFieldFunction) (IBaseEntity *Entity, uint8 *ClassOffset,
			const char *Value, void *ExtraData)

\brief	Defines an alias representing information describing a field validation function.
**/
typedef bool (*TValidateFieldFunction) (IBaseEntity *Entity, uint8 *ClassOffset, const char *Value, void *ExtraData);

#define FIELD_IS_VALID(d) ((!ValidateField) || ValidateField(Entity, ClassOffset, Value, d))

/**
\class	CEntityField

\brief	A single entity field.
		See "Entities - Fields" for more info.

\author	Paril
\date	29/05/2010
**/
class CEntityField
{
public:
	std::string				Name;						// The field name
	size_t					Offset;						// The offset offset
	EFieldType				FieldType, StrippedFields;	// The field type & stripped fields
	TValidateFieldFunction	ValidateField;				// The field validation function

	/**
	\fn	CEntityField (const char *Name, size_t Offset, EFieldType FieldType,
		TValidateFieldFunction ValidateField = NULL)
	
	\brief	Constructor.
	
	\author	Paril
	\date	29/05/2010
	
	\param	Name			The name. 
	\param	Offset			The offset. 
	\param	FieldType		Type of the field. 
	\param	ValidateField	The field validation function. 
	**/
	CEntityField (const char *Name, size_t Offset, EFieldType FieldType, TValidateFieldFunction ValidateField = NULL);

	/**
	\fn	template <class TClass> void Create (TClass *Entity, const char *Value) const
	
	\brief	Fills this field in 'Entity' with the data stored in 'Value'.
	
	\author	Paril
	\date	29/05/2010
	
	\param [in,out]	Entity	If non-null, the entity. 
	\param	Value			The value. 
	**/
	template <class TClass>
	void Create (TClass *Entity, const char *Value) const
	{
		uint8 *ClassOffset = ((FieldType & FT_GAME_ENTITY) ? (uint8*)Entity->GetGameEntity() : (uint8*)Entity) + Offset;

		switch (StrippedFields)
		{
		case FT_BOOL:
			if (FIELD_IS_VALID(NULL))
				OFS_TO_TYPE(bool) = (atoi(Value) != 0);
			break;
		case FT_CHAR:
			if (FIELD_IS_VALID(NULL))
				OFS_TO_TYPE(sint8) = Clamp<sint32>(atoi(Value), SCHAR_MIN, SCHAR_MAX);
			break;
		case FT_BYTE:
			if (FIELD_IS_VALID(NULL))
				OFS_TO_TYPE(uint8) = Clamp<uint32>(atou(Value), 0, UCHAR_MAX);
			break;
		case FT_SHORT:
			if (FIELD_IS_VALID(NULL))
				OFS_TO_TYPE(sint16) = Clamp<sint32>(atoi(Value), SHRT_MIN, SHRT_MAX);
			break;
		case FT_USHORT:
			if (FIELD_IS_VALID(NULL))
				OFS_TO_TYPE(uint16) = Clamp<uint32>(atoi(Value), 0, USHRT_MAX);
			break;
		case FT_INT:
			if (FIELD_IS_VALID(NULL))
				OFS_TO_TYPE(sint32) = atoi(Value);
			break;
		case FT_UINT:
			if (FIELD_IS_VALID(NULL))
				OFS_TO_TYPE(uint32) = atou(Value);
			break;
		case FT_FLOAT:
			if (FIELD_IS_VALID(NULL))
				OFS_TO_TYPE(float) = atof(Value);
			break;
		case FT_FLOAT_TO_BYTE:
			if (FIELD_IS_VALID(NULL))
				OFS_TO_TYPE(uint8) = (uint8)Clamp<sint32> ((sint32)(atof(Value) * 255), 0, 255);
			break;
		case FT_VECTOR:
			{
				vec3f v;

				std::istringstream strm (Value);
				strm >> v.X >> v.Y >> v.Z;

				if (FIELD_IS_VALID(&v))
					OFS_TO_TYPE(vec3f) = v;
			}
			break;
		case FT_YAWANGLE:
			if (FIELD_IS_VALID(NULL))
				OFS_TO_TYPE(vec3f).Set (0, atof(Value), 0);
			break;
		case FT_IGNORE:
			break;
		case FT_LEVEL_STRING:
			if (FIELD_IS_VALID(NULL))
			{
				if (strlen(Value))
					OFS_TO_TYPE(char*) = CopyStr(Value, TAG_LEVEL);
				else
					OFS_TO_TYPE(char*) = NULL;
			}
			break;
		case FT_SOUND_INDEX:
			{
				std::string temp = Value;
				if (temp.find (".wav") == std::string::npos)
					temp.append (".wav");

				if (FIELD_IS_VALID(NULL))
					OFS_TO_TYPE(MediaIndex) = SoundIndex (temp.c_str());
			}
			break;
		case FT_IMAGE_INDEX:
			if (FIELD_IS_VALID(NULL))
				OFS_TO_TYPE(MediaIndex) = ImageIndex (Value);
			break;
		case FT_MODEL_INDEX:
			if (FIELD_IS_VALID(NULL))
				OFS_TO_TYPE(MediaIndex) = ModelIndex (Value);
			break;
		case FT_FRAMENUMBER:
			{
				float Val = atof (Value);

				if (FIELD_IS_VALID(NULL))
					OFS_TO_TYPE(FrameNumber) = (Val != -1) ? (Val * 10) : -1;
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

				if (FIELD_IS_VALID(NULL))
					OFS_TO_TYPE(CBaseItem *) = Item;
			}
			break;
		case FT_STRING:
			if (FIELD_IS_VALID(NULL))
				OFS_TO_TYPE(std::string) = CopyStr(Value);
			break;
		};
	};

	/**
	\fn	template <class TClass> void Save (TClass *Entity, CFile &File) const
	
	\brief	Saves a field to a file.
	
	\author	Paril
	\date	29/05/2010
	
	\param [in,out]	Entity	If non-null, the entity. 
	\param [in,out]	File	The file. 
	**/
	template <class TClass>
	void Save (TClass *Entity, CFile &File) const
	{
		uint8 *ClassOffset = ((FieldType & FT_GAME_ENTITY) ? (uint8*)Entity->GetGameEntity() : (uint8*)Entity) + Offset;

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
			File.Write<FrameNumber> (OFS_TO_TYPE(FrameNumber));
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
				sint32 Index = (OFS_TO_TYPE(IBaseEntity*) && OFS_TO_TYPE(IBaseEntity*)->GetGameEntity()) ? OFS_TO_TYPE(IBaseEntity*)->State.GetNumber() : -1;
				File.Write<sint32> (Index);
			}
			break;
		case FT_STRING:
			File.Write (OFS_TO_TYPE(std::string));
			break;
		};
	};

	/**
	\fn	template <class TClass> void Load (TClass *Entity, CFile &File) const
	
	\brief	Loads a field from a file.
	
	\author	Paril
	\date	29/05/2010
	
	\param [in,out]	Entity	If non-null, the entity. 
	\param [in,out]	File	The file. 
	**/
	template <class TClass>
	void Load (TClass *Entity, CFile &File) const
	{
		uint8 *ClassOffset = ((FieldType & FT_GAME_ENTITY) ? (uint8*)Entity->GetGameEntity() : (uint8*)Entity) + Offset;

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
			OFS_TO_TYPE(char*) = File.ReadString (TAG_LEVEL);
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
			OFS_TO_TYPE(FrameNumber) = File.Read<FrameNumber> ();
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
				OFS_TO_TYPE(IBaseEntity *) = LoadEntity (Index);
			}
			break;
		case FT_STRING:
			OFS_TO_TYPE(std::string) = File.ReadCCString ();
			break;
		};
	};
};

/**
\fn	template <class TClass, class TPassClass> bool CheckFields (TClass *Me, const char *Key,
	const char *Value)

\brief	Template function that goes through each field in a class, looks for the field 'Key' and
		fills it in with 'Value'. 

\author	Paril
\date	29/05/2010

\param [in,out]	Me	If non-null, me. 
\param	Key			The key. 
\param	Value		The value. 

\return	true if it succeeds, false if it fails. 
**/
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
			TClass::FieldsForParsing[i]. template Create<TPassClass> (Me, Value);
			return true;
		}
	}
	return false;
};

/**
\fn	template <class TClass> bool CheckFields (TClass *Me, const char *Key, const char *Value)

\brief	Template function that goes through each field in a class, looks for the field 'Key' and
		fills it in with 'Value'. 
		This version is used for non-special classes.

\author	Paril
\date	29/05/2010

\param [in,out]	Me	If non-null, me. 
\param	Key			The key. 
\param	Value		The value. 

\return	true if it succeeds, false if it fails. 
**/
template <class TClass>
bool CheckFields (TClass *Me, const char *Key, const char *Value)
{
	return CheckFields<TClass, TClass> (Me, Key, Value);
};

/**
\fn	template <class TClass> void SaveEntityFields (TClass *Me, CFile &File)

\brief	Saves all entity fields of TClass to File.

\author	Paril
\date	29/05/2010

\param [in,out]	Me		If non-null, me. 
\param [in,out]	File	The file. 
**/
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
			TClass::FieldsForParsing[i]. template Save<TClass> (Me, File);
		}
	}
};

/**
\fn	template <class TClass> void LoadEntityFields (TClass *Me, CFile &File)

\brief	Loads all entity fields for TClass from File.

\author	Paril
\date	29/05/2010

\param [in,out]	Me		If non-null, me. 
\param [in,out]	File	The file. 
**/
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
			TClass::FieldsForParsing[i]. template Load<TClass> (Me, File);
		}
	}
};

/**
\fn	template <class TClass> size_t FieldSize ()

\brief	Returns the size of the fields of an entity.

\return	. 
**/
template <class TClass>
size_t FieldSize ()
{
	return ArrayCount(TClass::FieldsForParsing);
}

/**
\fn	IBaseEntity *GetGameEntity (sint32 Index)

\brief	Gets the game entity for index 'Index'.
		Required magic.

\author	Paril
\date	29/05/2010

\param	Index	Zero-based index of the entity. 

\return	null if it fails, else the game entity. 
**/
IBaseEntity *GetGameEntity (sint32 Index);

/**
\fn	inline void WriteEntity (CFile &File, IBaseEntity *Entity)

\brief	Writes entity 'Entity''s index to file 'File'.

\author	Paril
\date	29/05/2010

\param [in,out]	File	The file. 
\param [in,out]	Entity	If non-null, the entity. 
**/
inline void WriteEntity (CFile &File, IBaseEntity *Entity)
{
	File.Write<sint32> ((Entity && Entity->GetInUse()) ? Entity->State.GetNumber() : -1);
}

/**
\fn	inline IBaseEntity *ReadEntity (CFile &File)

\brief	Reads an entity's index from file, and returns the entity if it exists. 

\author	Paril
\date	29/05/2010

\param [in,out]	File	The file. 

\return	null if it fails, else the entity. 
**/
inline IBaseEntity *ReadEntity (CFile &File)
{
	sint32 Index = File.Read<sint32> ();

	if (Index != -1)
		return GetGameEntity(Index);
	return NULL;
}

/**
\fn	template <class TType> inline TType *ReadEntity (CFile &File)

\brief	Reads an entity's index from file, and returns the entity if it exists. 

\author	Paril
\date	29/05/2010

\param [in,out]	File	The file. 

\return	null if it fails, else the entity. 
**/
template <class TType>
inline TType *ReadEntity (CFile &File)
{
	sint32 Index = File.Read<sint32> ();

	if (Index != -1)
		return entity_cast<TType>(GetGameEntity(Index));
	return NULL;
}

/**
\fn	void InitEntityLists ()

\brief	Initialises the entity lists.
**/
void InitEntityLists ();

/**
\fn	edict_t *G_Spawn ()

\deprecated	Use CreateEntityFromClassname instead.

\brief	Either finds a free edict, or allocates a new one.
		Try to avoid reusing an entity that was recently freed, because it
		can cause the client to think the entity morphed into something else
		instead of being removed and recreated, which can cause interpolated
		angles and bad trails.

\return	null if it fails, else. 
**/
CC_INSECURE_DEPRECATE (CreateEntityFromClassname)
edict_t	*G_Spawn ();

/**
\fn	void G_InitEdict (edict_t *e)

\brief	Initialize entity.

\deprecated	Deprecated. Do not use.
			Function is not required (only internally)

\author	Paril
\date	29/05/2010

\param	e	Entity to initialize. 
**/
CC_INSECURE_DEPRECATE (Function not needed)
void	G_InitEdict (edict_t *e);

/**
\fn	void G_FreeEdict (edict_t *ed)

\deprecated	Use IBaseEntity::Free instead.

\brief	Marks the entity as freed. 

\author	Paril
\date	29/05/2010

\param [in,out]	ed	If non-null, the entity. 
**/
CC_INSECURE_DEPRECATE (Entity->Free)
void	G_FreeEdict (edict_t *e);

/**
\fn	void ED_CallSpawn (edict_t *ent)

\brief	Calls the spawn function for a given entity.
		Internal.

\author	Paril
\date	29/05/2010

\param [in,out]	ent	If non-null, the entity. 
**/
void	ED_CallSpawn (edict_t *ent);

extern IBaseEntity *World;	// The world entity

/**
\fn	inline IBaseEntity *CreateEntityFromClassname (const char *classname)

\brief	Creates an entity from a classname.

\author	Paril
\date	29/05/2010

\param	classname	The classname. 

\return	null if it fails, else the created entity. 
**/
inline IBaseEntity *CreateEntityFromClassname (const char *classname)
{
CC_DISABLE_DEPRECATION
	edict_t *ent = G_Spawn ();

	Level.ClassName = classname;
	ED_CallSpawn (ent);

	if (ent->Entity && ent->Entity->GetInUse() && !ent->Entity->Freed)
		return ent->Entity;
	return NULL;
CC_ENABLE_DEPRECATION
}

// Base classes
#include "cc_entity_types.h"

// Derivitives
#include "cc_player_entity.h"
#include "cc_weapon_entities.h"

/**
\class	IMapEntity

\brief	The map entity class. This contains common functions and fields that any entity that
		wants to be allowed to spawn in a map requires. 

\author	Paril
\date	29/05/2010
**/
class IMapEntity : public virtual IBaseEntity
{
public:
	char		*Classname;	// The classname
	char		*TargetName;	// Target's name

	/**
	\fn	IMapEntity ()
	
	\brief	Default constructor. 
	
	\author	Paril
	\date	29/05/2010
	**/
	IMapEntity ();

	/**
	\fn	IMapEntity (sint32 Index)
	
	\brief	Entity constructor. 
	
	\author	Paril
	\date	29/05/2010
	
	\param	Index	Zero-based index of the entity. 
	**/
	IMapEntity (sint32 Index);

	/**
	\fn	virtual ~IMapEntity ()
	
	\brief	Destructor.
	
	\author	Paril
	\date	29/05/2010
	**/
	virtual ~IMapEntity ()
	{
	};

	/**
	\fn	virtual void Spawn() = 0
	
	\brief	Spawn function.
			Every map class must have this.
	**/
	virtual void Spawn() = 0;

	static const class CEntityField FieldsForParsing[];
	static const size_t FieldsForParsingSize;

	static const class CEntityField FieldsForParsing_Map[];
	static const size_t FieldsForParsingSize_Map;

	/**
	\fn	virtual bool ParseField (const char *Key, const char *Value)
	
	\brief	Parse a single field from an entity.
			See "Entities - Saving/Loading"
	
	\author	Paril
	\date	29/05/2010
	
	\param	Key		The key. 
	\param	Value	The value. 
	
	\return	true if it succeeds, false if it fails. 
	**/
	virtual bool			ParseField (const char *Key, const char *Value);

	/**
	\fn	void ParseFields ()
	
	\brief	Parse the fields from an entity.
			See "Entities - Saving/Loading"		
	**/
	void					ParseFields ();

	/**
	\fn	virtual bool CheckValidity ()
	
	\brief	Checks the validity of this entity. An entity that was invalid will not spawn, and will
			be subsequently freed. 
	
	\return	true if valid, false if not. 
	**/
	virtual bool			CheckValidity ();

	virtual void SaveFields (CFile &File);
	virtual void LoadFields (CFile &File);
};

#include "cc_item_entity.h"
#include "cc_junk_entities.h"

#if ROGUE_FEATURES
#include "cc_rogue_spheres.h"
#endif

#else
FILE_WARNING
#endif
