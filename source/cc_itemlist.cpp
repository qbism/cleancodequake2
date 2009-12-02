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
// cc_itemlist.cpp
// Itemlist
// Basically the link between items and the real world
//

#include "cc_local.h"

namespace NItems
{
	CArmor *JacketArmor;
	CArmor *CombatArmor;
	CArmor *BodyArmor;
	CArmor *ArmorShard;

	CHealth *StimPack;
	CHealth *SmallHealth;
	CHealth *LargeHealth;

	CPowerCube *PowerCube;

	CWeaponItem *Blaster;
	CWeaponItem *Shotgun;
	CWeaponItem *SuperShotgun;
	CWeaponItem *Machinegun;
	CWeaponItem *Chaingun;
	CWeaponItem *GrenadeLauncher;
	CWeaponItem *RocketLauncher;
	CWeaponItem *HyperBlaster;
	CWeaponItem *Railgun;
	CWeaponItem *BFG;
	#if CLEANCTF_ENABLED
	CWeaponItem	*Grapple;

	CFlag *RedFlag;
	CFlag *BlueFlag;
	#endif

	CAmmo *Shells;
	CAmmo *Bullets;
	CAmmo *Slugs;
	CAmmo *Rockets;
	CAmmo *Cells;
	CAmmo *Grenades;

	CMegaHealth *MegaHealth;
	CBackPack *BackPack;
	CQuadDamage *Quad;
	CInvulnerability *Invul;
	CSilencer *Silencer;
	CRebreather *Rebreather;
	CEnvironmentSuit *EnvironmentSuit;
	CBandolier *Bandolier;
	CAdrenaline *Adrenaline;
	CAncientHead *AncientHead;
	CPowerShield *PowerShield;
	CPowerShield *PowerScreen;
};

CItemList *ItemList;

CItemList::CItemList() :
numItems(0)
{
};

void CItemList::AddItemToList (CBaseItem *Item)
{
	Items.push_back (Item);
	Item->Index = numItems++;

	// Hash!
	if (Item->Classname)
		HashedClassnameItemList.insert (std::make_pair<size_t, size_t> (Com_HashGeneric(Item->Classname, MAX_ITEMS_HASH), Item->Index));
	if (Item->Name)
		HashedNameItemList.insert (std::make_pair<size_t, size_t> (Com_HashGeneric(Item->Name, MAX_ITEMS_HASH), Item->Index));
}

void CItemList::SendItemNames ()
{
	for (uint8 i = 0; i < numItems; i++)
		ConfigString (Items[i]->GetConfigStringNumber(), Items[i]->Name);
}

CBaseItem *FindItem (const char *name)
{
	// Check through the itemlist
	static uint32 hash;
	hash = Com_HashGeneric(name, MAX_ITEMS_HASH);

	for (THashedItemListType::iterator it = ItemList->HashedNameItemList.equal_range(hash).first; it != ItemList->HashedNameItemList.equal_range(hash).second; ++it)
	{
		static CBaseItem *Item;
		Item = ItemList->Items.at((*it).second);

		if (Q_stricmp(Item->Name, name) == 0)
			return Item;
	}
	return NULL;
}

CBaseItem *FindItemByClassname (const char *name)
{
	// Check through the itemlist
	static uint32 hash;
	hash = Com_HashGeneric(name, MAX_ITEMS_HASH);

	for (THashedItemListType::iterator it = ItemList->HashedClassnameItemList.equal_range(hash).first; it != ItemList->HashedClassnameItemList.equal_range(hash).second; ++it)
	{
		static CBaseItem *Item;
		Item = ItemList->Items.at((*it).second);

		if (Q_stricmp(Item->Classname, name) == 0)
			return Item;
	}
	return NULL;
}

////////////////////////////////////////////////////////////////////////////////////////////////////
/// \fn	void CBaseItem::Add (edict_t *ent, sint32 quantity)
///
/// \brief	Adds 'quantity' amount of this to 'ent' (ignores any max)
///
/// \author	Paril
/// \date	5/9/2009
///
/// \param	ent		 - If non-null, the entity to add the amount to. 
/// \param	quantity - The amount to add. 
////////////////////////////////////////////////////////////////////////////////////////////////////
void CBaseItem::Add (CPlayerEntity *ent, sint32 quantity)
{
	ent->Client.Persistent.Inventory.Add(this, quantity);
}

CBaseItem *GetItemByIndex (uint32 Index)
{
	if (Index >= MAX_ITEMS || Index >= ItemList->Items.size() || Index < 0)
		return NULL;
	return ItemList->Items[Index];
}

inline sint32 GetNumItems ()
{
	return ItemList->numItems;
}

void InitItemlist ()
{
	ItemList = QNew (com_itemPool, 0) CItemList;

	AddAmmoToList();
	AddHealthToList();
	AddArmorToList();
	AddPowerupsToList();
	AddKeysToList();
#if CLEANCTF_ENABLED
	if (game.mode & GAME_CTF)
		AddFlagsToList();
#endif

	if (dmFlags.dfDmTechs || (game.mode & GAME_CTF))
		AddTechsToList();
}

void SetItemNames ()
{
	ItemList->SendItemNames();
}

void InitItemMedia ()
{
}

#include "cc_weaponmain.h"

// This is a required function that will
// go through each item and invalidate any variables that we used.
void InvalidateItemMedia ()
{
	for (sint32 i = 0; i < ItemList->numItems; i++)
	{
		CBaseItem *Item = ItemList->Items[i];

		Item->IconIndex = Item->PickupSoundIndex = 0;
		if (Item->Flags & ITEMFLAG_WEAPON)
		{
			if (Item->Flags & ITEMFLAG_AMMO)
			{
				CAmmo *Weapon = dynamic_cast<CAmmo*>(Item);
				Weapon->Weapon->WeaponModelIndex = Weapon->Weapon->WeaponSoundIndex = 0;
			}
			else
			{
				CWeaponItem *Weapon = dynamic_cast<CWeaponItem*>(Item);
				Weapon->Weapon->WeaponModelIndex = Weapon->Weapon->WeaponSoundIndex = 0;
			}
		}
	}
}