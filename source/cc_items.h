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

#if !defined(CC_GUARD_ITEMS_H) || !INCLUDE_GUARDS
#define CC_GUARD_ITEMS_H

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

void InvalidateItemMedia ();

class CBaseItem
{
	friend void InvalidateItemMedia ();
private:
	// The index of this item in the item list
	sint32			Index;

	// Must be friends with itemlist so it can set the item.
	friend class CItemList;
public:
	CBaseItem (const char *Classname, const char *WorldModel, sint32 EffectFlags,
			   const char *PickupSound, const char *Icon, const char *Name, EItemFlags Flags,
			   const char *Precache);

	CBaseItem () {}

	// Classname (for maps)
	const char		*Classname;
	// World model
	const char		*WorldModel;
	// Effect flags (EF_ROTATE, etc)
	sint32			EffectFlags;

	// The sound on pickup
	const char		*PickupSound;
private:
	MediaIndex	PickupSoundIndex;

public:
	inline MediaIndex GetPickupSound ()
	{
		return (PickupSound && !PickupSoundIndex) ? (PickupSoundIndex = SoundIndex(PickupSound)) : PickupSoundIndex;
	}

	/// HUD Icon
	const char		*Icon;
private:
	MediaIndex	IconIndex;

public:
	inline MediaIndex GetIconIndex ()
	{
		return (Icon && !IconIndex) ? (IconIndex = ImageIndex(Icon)) : IconIndex;
	}

	// Name on pickup
	const char		*Name;

	// Item flags
	EItemFlags	Flags;

	// Precached sounds/models/images.
	// FIXME: I don't necessarily like the fact that the precache
	// list here is parsed. Anyone have a better idea? :S
	const char		*Precache;

	// \brief	Attempts to pickup the item. 
	virtual	bool	Pickup (class CItemEntity *Player, CPlayerEntity *Other) = 0;

	// Attempts to uses the item. 
	virtual	void	Use (CPlayerEntity *Player) = 0;

	// Attempts to drops the item. 
	virtual	void	Drop (CPlayerEntity *Player) = 0;
	virtual class CItemEntity	*DropItem (CBaseEntity *Entity);

	virtual void	SetRespawn (class CItemEntity *Player, FrameNumber_t delay);

	inline sint32		GetIndex ()
	{
		return Index;
	};
	inline sint32		GetConfigStringNumber ()
	{
		return CS_ITEMS+Index;
	};

	virtual void	Add (CPlayerEntity *Player, sint32 quantity);
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
#if XATRIX_FEATURES
#include "cc_xatrix_items.h"
#endif

#include "cc_itemlist.h"

#else
FILE_WARNING
#endif
