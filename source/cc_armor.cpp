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
// cc_armor.cpp
// New, improved, better, stable item system!
//

#include "cc_local.h"

CArmor::CArmor (char *Classname, char *WorldModel, sint32 EffectFlags,
			   char *PickupSound, char *Icon, char *Name, EItemFlags Flags,
			   char *Precache, sint32 baseCount, sint32 maxCount, float normalProtection,
			   float energyProtection) :
CBaseItem(Classname, WorldModel, EffectFlags, PickupSound, Icon, Name, Flags,
		Precache),
baseCount(baseCount),
maxCount(maxCount),
normalProtection(normalProtection),
energyProtection(energyProtection)
{
};

bool CArmor::Pickup (class CItemEntity *ent, CPlayerEntity *other)
{
	if (normalProtection == -1)
	{
		if (other->Client.Persistent.Armor == NULL)
		{
			other->Client.Persistent.Inventory.Set (NItems::JacketArmor, 2);
			other->Client.Persistent.Armor = dynamic_cast<CArmor*>(NItems::JacketArmor);
		}
		else
		{
			if (maxCount != -1 && (other->Client.Persistent.Inventory.Has(other->Client.Persistent.Armor) >= maxCount))
				return false;

			other->Client.Persistent.Inventory.Add (other->Client.Persistent.Armor, 2);
			if (maxCount != -1 && (other->Client.Persistent.Inventory.Has(other->Client.Persistent.Armor) > maxCount))
				other->Client.Persistent.Inventory.Set(other->Client.Persistent.Armor, maxCount);
		}
	}
	else if (other->Client.Persistent.Armor != NULL)
	{
		if (normalProtection > other->Client.Persistent.Armor->normalProtection)
		{
			// calc new armor values
			sint32 newCount = baseCount + (((float)other->Client.Persistent.Armor->normalProtection / (float)normalProtection) * other->Client.Persistent.Inventory.Has(other->Client.Persistent.Armor));
			if (newCount > maxCount)
				newCount = maxCount;

			// zero count of old armor so it goes away
			other->Client.Persistent.Inventory.Set(other->Client.Persistent.Armor, 0);

			// change armor to new item with computed value
			other->Client.Persistent.Inventory.Set(this, newCount);
			other->Client.Persistent.Armor = this;
		}
		else
		{
			// calc new armor values
			sint32 newCount = other->Client.Persistent.Inventory.Has(other->Client.Persistent.Armor) + (((float)normalProtection / (float)other->Client.Persistent.Armor->normalProtection) * baseCount);
			if (newCount > other->Client.Persistent.Armor->maxCount)
				newCount = other->Client.Persistent.Armor->maxCount;

			// if we're already maxed out then we don't need the new armor
			if (other->Client.Persistent.Inventory.Has(other->Client.Persistent.Armor) >= newCount)
				return false;

			// update current armor value
			other->Client.Persistent.Inventory.Set(other->Client.Persistent.Armor, newCount);
		}
	}
	// Player has no other armor, just use it
	else
	{
		other->Client.Persistent.Armor = this;
		other->Client.Persistent.Inventory.Set(this, baseCount);
	}

	if (!(ent->SpawnFlags & DROPPED_ITEM) && (game.mode & GAME_DEATHMATCH))
		SetRespawn (ent, 200);

	return true;
}

// No dropping or using armor.
void CArmor::Use(CPlayerEntity *ent)
{
}

void CArmor::Drop (CPlayerEntity *ent)
{
}

#include "cc_tent.h"

sint32 CArmor::CheckArmor (CPlayerEntity *Player, vec3f &point, vec3f &normal, sint32 damage, sint32 dflags)
{
	if (!damage)
		return 0;
	if (dflags & DAMAGE_NO_ARMOR)
		return 0;

	sint32 save = ceil (((dflags & DAMAGE_ENERGY) ? ((float)energyProtection / 100) : ((float)normalProtection / 100)) * damage);
	if (save >= Player->Client.Persistent.Inventory.Has(this))
		save = Player->Client.Persistent.Inventory.Has(this);

	if (!save)
		return 0;

	Player->Client.Persistent.Inventory.Remove(GetIndex(), save);
	CTempEnt_Splashes::Sparks (point, normal, (dflags & DAMAGE_BULLET) ? CTempEnt_Splashes::ST_BULLET_SPARKS : CTempEnt_Splashes::ST_SPARKS, CTempEnt_Splashes::SPT_SPARKS);

	// Ran out of armor?
	if (!Player->Client.Persistent.Inventory.Has(this))
		Player->Client.Persistent.Armor = NULL;

	return save;
}

class CArmorEntity : public CItemEntity
{
public:
	CArmorEntity() :
	  CBaseEntity(),
	  CItemEntity ()
	  {
	  };

	CArmorEntity (sint32 Index) :
	  CBaseEntity(Index),
	  CItemEntity (Index)
	  {
	  };

	void Spawn (CBaseItem *item)
	{
		if ((game.mode & GAME_DEATHMATCH) && dmFlags.dfNoArmor)
		{
			Free ();
			return;
		}

		gameEntity->item = item;
		NextThink = level.Frame + 2;    // items start after other solids
		ThinkState = ITS_DROPTOFLOOR;
		PhysicsType = PHYSICS_NONE;

		State.GetEffects() = item->EffectFlags;
		State.GetRenderEffects() = RF_GLOW;
	};
};

LINK_ITEM_TO_CLASS (item_armor_jacket, CArmorEntity);
LINK_ITEM_TO_CLASS (item_armor_combat, CArmorEntity);
LINK_ITEM_TO_CLASS (item_armor_body, CArmorEntity);
LINK_ITEM_TO_CLASS (item_armor_shard, CArmorEntity);

void AddArmorToList ()
{
	NItems::JacketArmor = QNew (com_gamePool, 0) CArmor ("item_armor_jacket", "models/items/armor/jacket/tris.md2", EF_ROTATE, "misc/ar1_pkup.wav", "i_jacketarmor", "Jacket Armor", ITEMFLAG_GRABBABLE|ITEMFLAG_ARMOR, "", 25, 50, 30, 0);
	NItems::CombatArmor = QNew (com_gamePool, 0) CArmor ("item_armor_combat", "models/items/armor/combat/tris.md2", EF_ROTATE, "misc/ar1_pkup.wav", "i_combatarmor", "Combat Armor", ITEMFLAG_GRABBABLE|ITEMFLAG_ARMOR, "", 50, 100, 60, 30);
	NItems::BodyArmor = QNew (com_gamePool, 0) CArmor ("item_armor_body", "models/items/armor/body/tris.md2", EF_ROTATE, "misc/ar1_pkup.wav", "i_bodyarmor", "Body Armor", ITEMFLAG_GRABBABLE|ITEMFLAG_ARMOR, "", 100, 200, 80, 60);
	NItems::ArmorShard = QNew (com_gamePool, 0) CArmor ("item_armor_shard", "models/items/armor/shard/tris.md2", EF_ROTATE, "misc/ar2_pkup.wav", "i_jacketarmor", "Armor Shard", ITEMFLAG_GRABBABLE|ITEMFLAG_ARMOR, "", 2, -1, -1, -1);

	ItemList->AddItemToList (NItems::JacketArmor);
	ItemList->AddItemToList (NItems::CombatArmor);
	ItemList->AddItemToList (NItems::BodyArmor);
	ItemList->AddItemToList (NItems::ArmorShard);
}
