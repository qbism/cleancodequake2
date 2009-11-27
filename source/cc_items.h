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
// cc_items.h
// New, improved, better, stable item system!
//

#if !defined(__CC_ITEMS_H__) || !INCLUDE_GUARDS
#define __CC_ITEMS_H__

CC_ENUM (sint32, EItemFlags)
{
	ITEMFLAG_NONE				= 0,
	ITEMFLAG_WEAPON				= BIT(0),
	ITEMFLAG_AMMO				= BIT(1),
	ITEMFLAG_HEALTH				= BIT(2),
	ITEMFLAG_ARMOR				= BIT(3),
	ITEMFLAG_STAY_COOP			= BIT(4),
	ITEMFLAG_KEY				= BIT(5),
	ITEMFLAG_POWERUP			= BIT(6),
	ITEMFLAG_GRABBABLE			= BIT(7),
	ITEMFLAG_USABLE				= BIT(8),
	ITEMFLAG_DROPPABLE			= BIT(9),
	ITEMFLAG_TECH				= BIT(10),
};

////////////////////////////////////////////////////////////////////////////////////////////////////
/// \class	CBaseItem
///
/// \brief	The base item class. Contains the basic information that an item needs to spawn and be active. 
///
/// \author	Paril
/// \date	5/9/2009
////////////////////////////////////////////////////////////////////////////////////////////////////
void InvalidateItemMedia ();

class CBaseItem
{
	friend void InvalidateItemMedia ();
private:
	/// The index of this item in the item list
	sint32			Index;

	/// Must be friends with itemlist so it can set the item.
	friend class CItemList;
public:
	CBaseItem (char *Classname, char *WorldModel, sint32 EffectFlags,
			   char *PickupSound, char *Icon, char *Name, EItemFlags Flags,
			   char *Precache);

	/// Classname (for maps)
	char		*Classname;
	/// World model
	char		*WorldModel;
	/// Effect flags (EF_ROTATE, etc)
	sint32			EffectFlags;

	/// The sound on pickup
	char		*PickupSound;
private:
	MediaIndex	PickupSoundIndex;

public:
	inline MediaIndex GetPickupSound ()
	{
		return (PickupSound && !PickupSoundIndex) ? (PickupSoundIndex = SoundIndex(PickupSound)) : PickupSoundIndex;
	}

	/// HUD Icon
	char		*Icon;
private:
	MediaIndex	IconIndex;

public:
	inline MediaIndex GetIconIndex ()
	{
		return (Icon && !IconIndex) ? (IconIndex = ImageIndex(Icon)) : IconIndex;
	}

	/// Name on pickup
	char		*Name;

	/// Item flags
	EItemFlags	Flags;

	/// Precached sounds/models/images.
	/// I don't necessarily like the fact that the precache
	/// list here is parsed. Anyone have a better idea? :S
	char		*Precache;

	////////////////////////////////////////////////////////////////////////////////////////////////////
	/// \fn	virtual bool Pickup (edict_t *ent, edict_t *other) = 0
	///
	/// \brief	Attempts to pickup the item. 
	///
	/// \author	Paril
	/// \date	5/9/2009
	///
	/// \param	ent		 - If non-null, the item entity. 
	/// \param	other	 - If non-null, the player who picked the item up. 
	///
	/// \retval	true if it succeeds, false if it fails. 
	////////////////////////////////////////////////////////////////////////////////////////////////////
	virtual	bool	Pickup (class CItemEntity *ent, CPlayerEntity *other) = 0;

	////////////////////////////////////////////////////////////////////////////////////////////////////
	/// \fn	virtual void Use (edict_t *ent) = 0
	///
	/// \brief	Attempts to uses the item. 
	///
	/// \author	Paril
	/// \date	5/9/2009
	///
	/// \param	ent	 - If non-null, the entity that used the item. 
	////////////////////////////////////////////////////////////////////////////////////////////////////
	virtual	void	Use (CPlayerEntity *ent) = 0;

	////////////////////////////////////////////////////////////////////////////////////////////////////
	/// \fn	virtual void Drop (edict_t *ent) = 0
	///
	/// \brief	Attempts to drops the item. 
	///
	/// \author	Paril
	/// \date	5/9/2009
	///
	/// \param	ent	 - If non-null, the ent. 
	////////////////////////////////////////////////////////////////////////////////////////////////////
	virtual	void	Drop (CPlayerEntity *ent) = 0;
	virtual class CItemEntity	*DropItem (CBaseEntity *ent);

	virtual void	SetRespawn (class CItemEntity *ent, FrameNumber_t delay);

	inline sint32		GetIndex ()
	{
		return Index;
	};
	inline sint32		GetConfigStringNumber ()
	{
		return CS_ITEMS+Index;
	};

	virtual void	Add (CPlayerEntity *ent, sint32 quantity);
};

#include "cc_weapons.h"
#include "cc_health.h"
#include "cc_armor.h"
#include "cc_keys.h"
#include "cc_powerups.h"

#if CLEANCTF_ENABLED
#include "cc_ctfitems.h"
#endif
#include "cc_techs.h"

#include "cc_itemlist.h"

#else
FILE_WARNING
#endif