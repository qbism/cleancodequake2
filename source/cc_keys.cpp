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
// cc_keys.cpp
// ALL THE WONDER THAT I WANT I FOUND IN HER
//

#include "cc_local.h"

CKey::CKey(char *Classname, char *WorldModel, sint32 EffectFlags,
			   char *PickupSound, char *Icon, char *Name, EItemFlags Flags,
			   char *Precache) :
CBaseItem(Classname, WorldModel, EffectFlags, PickupSound, Icon, Name, Flags,
				Precache)
{
};

void CKey::Use (CPlayerEntity *ent)
{
}

void CKey::Drop (CPlayerEntity *ent)
{
}

CPowerCube::CPowerCube(char *Classname, char *WorldModel, sint32 EffectFlags,
			   char *PickupSound, char *Icon, char *Name, EItemFlags Flags,
			   char *Precache) :
CKey(Classname, WorldModel, EffectFlags, PickupSound, Icon, Name, Flags,
				Precache)
{
};


bool CKey::Pickup (class CItemEntity *ent, CPlayerEntity *other)
{
	if (game.GameMode == GAME_COOPERATIVE)
	{
		if (other->Client.Persistent.Inventory.Has(this))
			return false;
		other->Client.Persistent.Inventory.Set (this, 1);
		return true;
	}
	other->Client.Persistent.Inventory += this;
	return true;
}

bool CPowerCube::Pickup (class CItemEntity *ent, CPlayerEntity *other)
{
	if (game.GameMode == GAME_COOPERATIVE)
	{
		if (other->Client.Persistent.PowerCubeCount & ((ent->SpawnFlags & 0x0000ff00)>> 8))
			return false;
		other->Client.Persistent.Inventory += this;
		other->Client.Persistent.PowerCubeCount |= ((ent->SpawnFlags & 0x0000ff00) >> 8);
		return true;
	}
	other->Client.Persistent.Inventory += this;
	return true;
}

class CPowerCubeEntity : public CItemEntity
{
public:
	CPowerCubeEntity() :
	  CBaseEntity(),
	  CItemEntity ()
	  {
	  };

	CPowerCubeEntity (sint32 Index) :
	  CBaseEntity(Index),
	  CItemEntity (Index)
	  {
	  };

	void Spawn (CBaseItem *item)
	{
		if (game.GameMode == GAME_COOPERATIVE)
		{
			SpawnFlags |= (1 << (8 + level.PowerCubeCount));
			level.PowerCubeCount++;
		}

		LinkedItem = item;
		NextThink = level.Frame + 2;    // items start after other solids
		ThinkState = ITS_DROPTOFLOOR;
		PhysicsType = PHYSICS_NONE;

		State.GetEffects() = item->EffectFlags;
		State.GetRenderEffects() = RF_GLOW;
	};
};

LINK_ITEM_TO_CLASS (key_data_cd, CItemEntity);
LINK_ITEM_TO_CLASS (key_pyramid, CItemEntity);
LINK_ITEM_TO_CLASS (key_data_spinner, CItemEntity);
LINK_ITEM_TO_CLASS (key_pass, CItemEntity);
LINK_ITEM_TO_CLASS (key_blue_key, CItemEntity);
LINK_ITEM_TO_CLASS (key_red_key, CItemEntity);
LINK_ITEM_TO_CLASS (key_commander_head, CItemEntity);
LINK_ITEM_TO_CLASS (key_airstrike_target, CItemEntity);
LINK_ITEM_TO_CLASS (key_power_cube, CPowerCubeEntity);

void AddKeysToList ()
{
	QNew (com_itemPool, 0) CKey("key_data_cd", "models/items/keys/data_cd/tris.md2", EF_ROTATE, "items/pkup.wav", "k_datacd", "Data CD", ITEMFLAG_GRABBABLE|ITEMFLAG_KEY|ITEMFLAG_STAY_COOP, "");
	NItems::PowerCube = QNew (com_itemPool, 0) CPowerCube("key_power_cube", "models/items/keys/power/tris.md2", EF_ROTATE, "items/pkup.wav", "k_powercube", "Power Cube", ITEMFLAG_GRABBABLE|ITEMFLAG_KEY|ITEMFLAG_STAY_COOP, "");
	QNew (com_itemPool, 0) CKey("key_pyramid", "models/items/keys/pyramid/tris.md2", EF_ROTATE, "items/pkup.wav", "k_pyramid", "Pyramid Key", ITEMFLAG_GRABBABLE|ITEMFLAG_KEY|ITEMFLAG_STAY_COOP, "");
	QNew (com_itemPool, 0) CKey("key_data_spinner", "models/items/keys/spinner/tris.md2", EF_ROTATE, "items/pkup.wav", "k_dataspin", "Data Spinner", ITEMFLAG_GRABBABLE|ITEMFLAG_KEY|ITEMFLAG_STAY_COOP, "");
	QNew (com_itemPool, 0) CKey("key_pass", "models/items/keys/pass/tris.md2", EF_ROTATE, "items/pkup.wav", "k_security", "Security Pass", ITEMFLAG_GRABBABLE|ITEMFLAG_KEY|ITEMFLAG_STAY_COOP, "");
	QNew (com_itemPool, 0) CKey("key_blue_key", "models/items/keys/key/tris.md2", EF_ROTATE, "items/pkup.wav", "k_bluekey", "Blue Key", ITEMFLAG_GRABBABLE|ITEMFLAG_KEY|ITEMFLAG_STAY_COOP, "");
	QNew (com_itemPool, 0) CKey("key_red_key", "models/items/keys/red_key/tris.md2", EF_ROTATE, "items/pkup.wav", "k_redkey", "Red Key", ITEMFLAG_GRABBABLE|ITEMFLAG_KEY|ITEMFLAG_STAY_COOP, "");
	QNew (com_itemPool, 0) CKey("key_commander_head", "models/monsters/commandr/head/tris.md2", EF_ROTATE, "items/pkup.wav", "k_comhead", "Commander's Head", ITEMFLAG_GRABBABLE|ITEMFLAG_KEY|ITEMFLAG_STAY_COOP, "");
	QNew (com_itemPool, 0) CKey("key_airstrike_target", "models/items/keys/target/tris.md2", EF_ROTATE, "items/pkup.wav", "i_airstrike", "Airstrike Marker", ITEMFLAG_GRABBABLE|ITEMFLAG_KEY|ITEMFLAG_STAY_COOP, "");
}

