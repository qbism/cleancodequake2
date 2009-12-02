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
// cc_health.cpp
// New, improved, better, stable item system!
//

#include "cc_local.h"

CHealth::CHealth (char *Classname, char *WorldModel, sint32 EffectFlags,
			   char *PickupSound, char *Icon, char *Name, EItemFlags Flags,
			   char *Precache, sint32 Amount, EHealthFlags HealthFlags) :
CBaseItem (Classname, WorldModel, EffectFlags, PickupSound, Icon, Name, Flags,
		   Precache),
Amount(Amount),
HealthFlags(HealthFlags)
{
};

bool CHealth::Pickup (CItemEntity *ent, CPlayerEntity *other)
{
	if (!(HealthFlags & HEALTHFLAG_IGNOREMAX) && (other->Health >= other->MaxHealth))
		return false;

#if CLEANCTF_ENABLED
//ZOID
	if (other->Health >= 250 && Amount > 25)
		return false;
//ZOID
#endif

	other->Health += Amount;

#if CLEANCTF_ENABLED
//ZOID
	if (other->Health > 250 && Amount > 25)
		other->Health = 250;
//ZOID
#endif

	if (!(HealthFlags & HEALTHFLAG_IGNOREMAX))
	{
		if (other->Health > other->MaxHealth)
			other->Health = other->MaxHealth;
	}

	if (!(ent->SpawnFlags & DROPPED_ITEM) && (game.mode & GAME_DEATHMATCH))
		SetRespawn (ent, 300);

	return true;
}

class CHealthEntity : public CItemEntity
{
public:
	CHealthEntity() :
	  CBaseEntity(),
	  CItemEntity ()
	  {
	  };

	CHealthEntity (sint32 Index) :
	  CBaseEntity(Index),
	  CItemEntity (Index)
	  {
	  };

	void Spawn (CBaseItem *item)
	{
		if ((game.mode & GAME_DEATHMATCH) && dmFlags.dfNoHealth)
		{
			Free ();
			return;
		}

		LinkedItem = item;
		NextThink = level.Frame + 2;    // items start after other solids
		ThinkState = ITS_DROPTOFLOOR;
		PhysicsType = PHYSICS_NONE;

		State.GetEffects() = item->EffectFlags;
		State.GetRenderEffects() = RF_GLOW;
	};
};

LINK_ITEM_TO_CLASS (item_health_small, CHealthEntity);
LINK_ITEM_TO_CLASS (item_health, CHealthEntity);
LINK_ITEM_TO_CLASS (item_health_large, CHealthEntity);

void AddHealthToList ()
{
	NItems::StimPack = QNew (com_itemPool, 0) CHealth("item_health_small", "models/items/healing/stimpack/tris.md2", 0, "items/s_health.wav", "i_health", "Stimpack", ITEMFLAG_HEALTH|ITEMFLAG_GRABBABLE, "", 2, HEALTHFLAG_IGNOREMAX);
	NItems::SmallHealth = QNew (com_itemPool, 0) CHealth("item_health", "models/items/healing/medium/tris.md2", 0, "items/n_health.wav", "i_health", "Medium Health", ITEMFLAG_HEALTH|ITEMFLAG_GRABBABLE, "", 10, HEALTHFLAG_NONE);
	NItems::LargeHealth = QNew (com_itemPool, 0) CHealth("item_health_large", "models/items/healing/large/tris.md2", 0, "items/l_health.wav", "i_health", "Large Health", ITEMFLAG_HEALTH|ITEMFLAG_GRABBABLE, "", 25, HEALTHFLAG_NONE);
}