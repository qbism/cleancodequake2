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
// cc_itementity.h
// The entity that lets you pick items up.
// Moved here because it's seperate from the item system.
//

#if !defined(__CC_ITEMENTITY_H__) || !defined(INCLUDE_GUARDS)
#define __CC_ITEMENTITY_H__

// The item entity class
CC_ENUM (uint8, EItemThinkState)
{
	ITS_NONE,
	ITS_DROPTOFLOOR,
	ITS_RESPAWN,
	ITS_FREE,

	ITS_CUSTOM
};

// Item entity
class CItemEntity : public CMapEntity, public CTossProjectile, public CTouchableEntity, public CThinkableEntity, public CUsableEntity
{
public:
	uint32 AmmoCount;
	EItemThinkState ThinkState;

	CItemEntity ();
	CItemEntity (sint32 Index);

	virtual void Spawn (CBaseItem *item);

	virtual bool ParseField (const char *Key, const char *Value)
	{
		return (CUsableEntity::ParseField (Key, Value) || CMapEntity::ParseField (Key, Value));
	}

	// Returns a random team member of ent
	CItemEntity *GetRandomTeamMember (CItemEntity *Master);

	virtual void Think ();
	virtual void Touch (CBaseEntity *other, plane_t *plane, cmBspSurface_t *surf);

	virtual void Use (CBaseEntity *other, CBaseEntity *activator);

	bool Run ();

	void Spawn ();
};

#define LINK_ITEM_TO_CLASS(mapClassName,DLLClassName) \
	CMapEntity *LINK_RESOLVE_CLASSNAME(mapClassName, _Spawn) (sint32 Index) \
	{ \
		DLLClassName *newClass = QNew (com_levelPool, 0) DLLClassName(Index); \
		CBaseItem *Item = FindItemByClassname(#mapClassName); \
		if (Item) \
		{	\
			newClass->ParseFields (); \
			\
			if (newClass->CheckValidity()) \
				newClass->Spawn (Item); \
		}	\
		return newClass; \
	} \
	CClassnameToClassIndex LINK_RESOLVE_CLASSNAME(mapClassName, _Linker) \
	(LINK_RESOLVE_CLASSNAME(mapClassName, _Spawn), #mapClassName);

#ifdef CLEANCTF_ENABLED
#include "cc_ctfitementities.h"
#endif

#else
FILE_WARNING
#endif