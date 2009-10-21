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
// cc_weapons.cpp
// New, improved, better, stable item system!
//

#include "cc_local.h"
#include "cc_weaponmain.h"

CInventory::CInventory ()
{
	memset (Array, 0, sizeof(int)*MAX_CS_ITEMS);
	SelectedItem = -1;
}

void CInventory::Add (CBaseItem *Item, int Num)
{
	Array[Item->GetIndex()] += Num;
}

void CInventory::Remove (CBaseItem *Item, int Num)
{
	Array[Item->GetIndex()] -= Num;
}

void CInventory::Add (int Index, int Num)
{
	Array[Index] += Num;
}

void CInventory::Remove (int Index, int Num)
{
	Array[Index] -= Num;
}

void CInventory::Draw (CPlayerEntity *ent)
{
	ent->Client.LayoutFlags &= ~(LF_SHOWSCORES | LF_SHOWHELP);
	if (ent->Client.LayoutFlags & LF_SHOWINVENTORY)
	{
		ent->Client.LayoutFlags &= ~LF_SHOWINVENTORY;
		return;
	}

	ent->Client.LayoutFlags |= LF_SHOWINVENTORY;

	WriteByte (SVC_INVENTORY);
	for (int i=0 ; i<MAX_CS_ITEMS ; i++)
		WriteShort (Array[i]);
	ent->CastTo (CASTFLAG_RELIABLE);
}

void CInventory::SelectNextItem(EItemFlags Flags)
{
	int			index;

	// scan  for the next valid one
	for (int i=1 ; i<=MAX_CS_ITEMS ; i++)
	{
		index = (SelectedItem + i)%MAX_CS_ITEMS;
		if (!Array[index])
			continue;
		CBaseItem *it = GetItemByIndex(index);
		if (!(it->Flags & ITEMFLAG_USABLE))
			continue;
		if (!(it->Flags & Flags))
			continue;

		SelectedItem = index;
		return;
	}

	SelectedItem = -1;
	ValidateSelectedItem ();
}

void CInventory::SelectPrevItem(EItemFlags Flags)
{
	int			index;

	// scan  for the next valid one
	for (int i=1 ; i<=MAX_CS_ITEMS ; i++)
	{
		index = (SelectedItem + MAX_CS_ITEMS - i)%MAX_CS_ITEMS;
		if (!Array[index])
			continue;
		CBaseItem *it = GetItemByIndex(index);
		if (!(it->Flags & ITEMFLAG_USABLE))
			continue;
		if (!(it->Flags & Flags))
			continue;

		SelectedItem = index;
		return;
	}

	SelectedItem = -1;
	ValidateSelectedItem ();
}

int CInventory::Has (int Index)
{
	return Array[Index];
}

int CInventory::Has (CBaseItem *Item)
{
	return Array[Item->GetIndex()];
}

void CInventory::Set (CBaseItem *Item, int Num)
{
	Array[Item->GetIndex()] = Num;
}

void CInventory::Set (int Index, int Num)
{
	Array[Index] = Num;
}

void CInventory::ValidateSelectedItem()
{
	if (Array[SelectedItem])
		return;		// valid

	SelectNextItem (-1);
}

void CInventory::operator += (CBaseItem *Item)
{
	Add(Item, 1);
}

void CInventory::operator += (int Index)
{
	Add(GetItemByIndex(Index), 1);
}

void CInventory::operator -= (CBaseItem *Item)
{
	Remove(Item, 1);
}

void CInventory::operator -= (int Index)
{
	Remove (GetItemByIndex(Index), 1);
}

/*
==================
Cmd_Use_f

Use an inventory item
==================
*/
void Cmd_Use_f (CPlayerEntity *ent)
{
	std::cc_string s = ArgGetConcatenatedString();
	CBaseItem *Item = FindItem(s.c_str());

	if (!Item)
	{
		Item = FindItemByClassname(s.c_str());

		if (!Item)
		{
			ent->PrintToClient (PRINT_HIGH, "Unknown item: %s\n", s);
			return;
		}
	}
	if (!(Item->Flags & ITEMFLAG_USABLE))
	{
		ent->PrintToClient (PRINT_HIGH, "Item is not usable.\n");
		return;
	}
	if (!ent->Client.Persistent.Inventory.Has(Item))
	{
		ent->PrintToClient (PRINT_HIGH, "Out of item: %s\n", s);
		return;
	}

	Item->Use (ent);
	ent->Client.Persistent.Inventory.ValidateSelectedItem();
}


/*
==================
Cmd_UseList_f

Tries to use the items in the list from left to right, stopping
when one gets used
==================
*/
void Cmd_UseList_f (CPlayerEntity *ent)
{
	for (int i = 1; i < ArgCount(); i++)
	{
		std::cc_string s = ArgGets(i);
		CBaseItem *Item = FindItem(s.c_str());

		if (!Item)
		{
			Item = FindItemByClassname(s.c_str());

			if (!Item)
			{
				ent->PrintToClient (PRINT_HIGH, "Unknown item: %s\n", s);
				continue;
			}
		}
		if (!(Item->Flags & ITEMFLAG_USABLE))
		{
			ent->PrintToClient (PRINT_HIGH, "Item is not usable.\n");
			continue;
		}

		// Only warn us if it's unknown or not usable; not if we don't have it
		if (!ent->Client.Persistent.Inventory.Has(Item))
			continue;

		// If it's a weapon and we don't have ammo, don't bother
		if ((Item->Flags & ITEMFLAG_WEAPON) && !(Item->Flags & ITEMFLAG_AMMO))
		{
			CWeaponItem *Weapon = dynamic_cast<CWeaponItem*>(Item);
			if (!ent->Client.Persistent.Inventory.Has(Weapon->Ammo))
				continue;
		}

		Item->Use (ent);
		ent->Client.Persistent.Inventory.ValidateSelectedItem();
		return;
	}
	ent->PrintToClient (PRINT_HIGH, "No item in list found!\n");
}

/*
==================
Cmd_Drop_f

Drop an inventory item
==================
*/
void Cmd_Drop_f (CPlayerEntity *ent)
{
	std::cc_string s = ArgGetConcatenatedString();

	if (Q_stricmp (s.c_str(), "tech") == 0)
	{
		if (ent->Client.Persistent.Tech)
		{
			ent->Client.Persistent.Tech->Drop (ent);
			ent->Client.Persistent.Inventory.ValidateSelectedItem();
		}
		return;
	}

	CBaseItem *Item = FindItem(s.c_str());
	if (!Item)
	{
		Item = FindItemByClassname(s.c_str());

		if (!Item)
		{
			ent->PrintToClient (PRINT_HIGH, "Unknown item: %s\n", s);
			return;
		}
	}
	if (!(Item->Flags & ITEMFLAG_DROPPABLE))
	{
		ent->PrintToClient (PRINT_HIGH, "Item is not dropable.\n");
		return;
	}
	if (!ent->Client.Persistent.Inventory.Has(Item))
	{
		ent->PrintToClient (PRINT_HIGH, "Out of item: %s\n", s);
		return;
	}

	Item->Drop (ent);
	ent->Client.Persistent.Inventory.ValidateSelectedItem();
}


/*
=================
Cmd_Inven_f
=================
*/
void Cmd_Inven_f (CPlayerEntity *ent)
{
	if (ent->Client.Respawn.MenuState.InMenu)
	{
		ent->Client.Respawn.MenuState.CloseMenu();
		ent->Client.LayoutFlags |= LF_UPDATECHASE;
		return;
	}
#ifdef CLEANCTF_ENABLED
//ZOID
	if ((game.mode & GAME_CTF) && ent->Client.Respawn.ctf_team == CTF_NOTEAM)
	{
		CTFOpenJoinMenu(ent);
		return;
	}
//ZOID
#endif

	ent->Client.Persistent.Inventory.Draw(ent);
}

/*
=================
Cmd_InvUse_f
=================
*/
void Cmd_InvUse_f (CPlayerEntity *ent)
{
	if (ent->Client.Respawn.MenuState.InMenu)
	{
		ent->Client.Respawn.MenuState.Select();
		return;
	}
	if (ent->Health <= 0 || ent->DeadFlag)
		return;

	ent->Client.Persistent.Inventory.ValidateSelectedItem ();

	if (ent->Client.Persistent.Inventory.SelectedItem == -1)
	{
		ent->PrintToClient (PRINT_HIGH, "No item to use.\n");
		return;
	}

	CBaseItem *it = GetItemByIndex(ent->Client.Persistent.Inventory.SelectedItem);
	if (!(it->Flags & ITEMFLAG_USABLE))
	{
		ent->PrintToClient (PRINT_HIGH, "Item is not usable.\n");
		return;
	}
	it->Use (ent);
}

/*
=================
Cmd_WeapPrev_f
=================
*/
void Cmd_WeapPrev_f (CPlayerEntity *ent)
{
	if (!ent->Client.Persistent.Weapon)
		return;
	if (ent->Health <= 0 || ent->DeadFlag)
		return;

	int selectedWeaponIndex = ent->Client.Persistent.Weapon->Item->GetIndex();

	// scan  for the next valid one
	for (int i=0 ; i<=GetNumItems() ; i++)
	{
		int index = (selectedWeaponIndex + MAX_ITEMS - i)%MAX_CS_ITEMS;
		if (!ent->Client.Persistent.Inventory.Has(index))
			continue;
		CBaseItem *Item = GetItemByIndex(index);
		if (!(Item->Flags & ITEMFLAG_USABLE))
			continue;
		if (! (Item->Flags & ITEMFLAG_WEAPON) )
			continue;
		Item->Use (ent);
		if (ent->Client.NewWeapon && ent->Client.NewWeapon->Item == Item || ent->Client.Persistent.Weapon->Item == Item)
			return;	// successful
	}
}

/*
=================
Cmd_WeapNext_f
=================
*/
void Cmd_WeapNext_f (CPlayerEntity *ent)
{
	if (!ent->Client.Persistent.Weapon)
		return;
	if (ent->Health <= 0 || ent->DeadFlag)
		return;

	int selectedWeaponIndex = ent->Client.Persistent.Weapon->Item->GetIndex();

	// scan  for the next valid one
	for (int i=0 ; i<=GetNumItems() ; i++)
	{
		int index = (selectedWeaponIndex + i)%MAX_CS_ITEMS;
		if (!ent->Client.Persistent.Inventory.Has(index))
			continue;
		CBaseItem *Item = GetItemByIndex(index);
		if (!(Item->Flags & ITEMFLAG_USABLE))
			continue;
		if (! (Item->Flags & ITEMFLAG_WEAPON) )
			continue;
		Item->Use (ent);
		if (ent->Client.NewWeapon && ent->Client.NewWeapon->Item == Item || ent->Client.Persistent.Weapon->Item == Item)
			return;	// successful
	}
}

/*
=================
Cmd_WeapLast_f
=================
*/
void Cmd_WeapLast_f (CPlayerEntity *ent)
{
	if (!ent->Client.Persistent.Weapon || !ent->Client.Persistent.LastWeapon)
		return;

	if (!ent->Client.Persistent.Inventory.Has(ent->Client.Persistent.LastWeapon->Item))
		return;
	if (!(ent->Client.Persistent.LastWeapon->Item->Flags & ITEMFLAG_USABLE))
		return;
	if (! (ent->Client.Persistent.LastWeapon->Item->Flags & ITEMFLAG_WEAPON) )
		return;
	ent->Client.Persistent.LastWeapon->Item->Use (ent);
}

/*
=================
Cmd_InvDrop_f
=================
*/
void Cmd_InvDrop_f (CPlayerEntity *ent)
{
	if (ent->Health <= 0 || ent->DeadFlag)
		return;

	ent->Client.Persistent.Inventory.ValidateSelectedItem ();

	if (ent->Client.Persistent.Inventory.SelectedItem == -1)
	{
		ent->PrintToClient (PRINT_HIGH, "No item to drop.\n");
		return;
	}

	CBaseItem *Item = GetItemByIndex(ent->Client.Persistent.Inventory.SelectedItem);
	if (!(Item->Flags & ITEMFLAG_DROPPABLE))
	{
		ent->PrintToClient (PRINT_HIGH, "Item is not dropable.\n");
		return;
	}
	Item->Drop (ent);
	ent->Client.Persistent.Inventory.ValidateSelectedItem();
}

void Cmd_SelectNextItem_f (CPlayerEntity *ent)
{
	if (ent->Health <= 0 || ent->DeadFlag)
		return;

	if (ent->Client.Respawn.MenuState.InMenu)
	{
		ent->Client.Respawn.MenuState.SelectNext();
		return;
	}

	if (ent->Client.chase_target)
	{
		ent->ChaseNext();
		return;
	}

	ent->Client.Persistent.Inventory.SelectNextItem (-1);
}
void Cmd_SelectPrevItem_f (CPlayerEntity *ent)
{
	if (ent->Health <= 0 || ent->DeadFlag)
		return;

	if (ent->Client.Respawn.MenuState.InMenu)
	{
		ent->Client.Respawn.MenuState.SelectPrev();
		return;
	}

	if (ent->Client.chase_target)
	{
		ent->ChasePrev();
		return;
	}

	ent->Client.Persistent.Inventory.SelectPrevItem (-1);
}
void Cmd_SelectNextWeapon_f (CPlayerEntity *ent)
{
	if (ent->Health <= 0 || ent->DeadFlag)
		return;

	ent->Client.Persistent.Inventory.SelectNextItem (ITEMFLAG_WEAPON);
}
void Cmd_SelectPrevWeapon_f (CPlayerEntity *ent)
{
	if (ent->Health <= 0 || ent->DeadFlag)
		return;

	ent->Client.Persistent.Inventory.SelectPrevItem (ITEMFLAG_WEAPON);
}
void Cmd_SelectNextPowerup_f (CPlayerEntity *ent)
{
	if (ent->Health <= 0 || ent->DeadFlag)
		return;

	ent->Client.Persistent.Inventory.SelectNextItem (ITEMFLAG_POWERUP);
}
void Cmd_SelectPrevPowerup_f (CPlayerEntity *ent)
{
	if (ent->Health <= 0 || ent->DeadFlag)
		return;

	ent->Client.Persistent.Inventory.SelectPrevItem (ITEMFLAG_POWERUP);
}

/*
==================
Cmd_Give_f

Give items to a client
Old-style "give"
==================
*/
void Cmd_Give_f (CPlayerEntity *ent)
{
	CBaseItem *it;

	std::cc_string name = ArgGets(1);
	bool give_all = (Q_stricmp (name.c_str(), "all") == 0);

	if (give_all || Q_stricmp (name.c_str(), "health") == 0)
	{
		if (ArgCount() == 3)
			ent->Health = ArgGeti(2);
		else
			ent->Health = ent->MaxHealth;
		if (!give_all)
			return;
	}

	if (give_all || Q_stricmp (name.c_str(), "weapons") == 0)
	{
		for (int i = 0; i < GetNumItems(); i++)
		{
			it = GetItemByIndex(i);
			if (!(it->Flags & ITEMFLAG_WEAPON))
				continue;
			ent->Client.Persistent.Inventory += it;
		}
		if (!give_all)
			return;
	}

	if (give_all || Q_stricmp (name.c_str(), "ammo") == 0)
	{
		for (int i = 0; i < GetNumItems(); i++)
		{
			it = GetItemByIndex(i);
			if (!(it->Flags & ITEMFLAG_GRABBABLE))
				continue;
			if (!(it->Flags & ITEMFLAG_AMMO))
				continue;

			CAmmo *Ammo = static_cast<CAmmo*>(it);
			Ammo->AddAmmo (ent, 1000);
		}
		if (!give_all)
			return;
	}

	if (give_all || Q_stricmp (name.c_str(), "armor") == 0)
	{
		ent->Client.Persistent.Inventory.Set(NItems::JacketArmor->GetIndex(), 0);
		ent->Client.Persistent.Inventory.Set(NItems::CombatArmor->GetIndex(), 0);

		CArmor *Armor = dynamic_cast<CArmor*>(NItems::BodyArmor);
		ent->Client.Persistent.Inventory.Set(Armor->GetIndex(), Armor->maxCount);
		ent->Client.Persistent.Armor = Armor;

		if (ArgCount() >= 3)
			ent->Client.Persistent.Inventory.Set(Armor->GetIndex(), ArgGeti(2));

		if (!give_all)
			return;
	}

	if (give_all || Q_stricmp (name.c_str(), "power shield") == 0)
	{
		/*it = NItems::PowerShield;
		CItemEntity *it_ent = QNew (com_levelPool, 0) CItemEntity();
		it_ent->gameEntity->classname = it->Classname;
		it_ent->Spawn (it);

		it_ent->Touch (ent, NULL, NULL);
		if (it_ent->GetInUse())
			it_ent->Free();*/
		NItems::PowerShield->Add (ent, 1);

		if (!give_all)
			return;
	}

	if (give_all)
	{
		for (int i = 0; i < GetNumItems(); i++)
		{
			it = GetItemByIndex(i);
			if (!(it->Flags & ITEMFLAG_GRABBABLE))
				continue;
			if (it->Flags & (ITEMFLAG_HEALTH|ITEMFLAG_ARMOR|ITEMFLAG_WEAPON|ITEMFLAG_AMMO))
				continue;
			ent->Client.Persistent.Inventory.Set (i, 1);
		}
		return;
	}

	it = FindItem (name.c_str());
	if (!it)
	{
		name = ArgGetConcatenatedString();
		it = FindItem(name.c_str());

		if (!it)
		{
			it = FindItemByClassname (name.c_str());
			if (!it)
			{
				ent->PrintToClient (PRINT_HIGH, "Unknown Item\n");
				return;
			}
		}
	}

	if (!(it->Flags & ITEMFLAG_GRABBABLE))
	{
		ent->Client.Persistent.Inventory += it;
		return;
	}

	if (it->Flags & ITEMFLAG_AMMO)
	{
		CAmmo *Ammo = dynamic_cast<CAmmo*>(it);
		if (ArgCount() == 3)
			ent->Client.Persistent.Inventory.Set (Ammo, ArgGeti(2));
		else
			ent->Client.Persistent.Inventory.Add(Ammo, Ammo->Quantity);
	}
	else
	{
		CItemEntity *it_ent = QNew (com_levelPool, 0) CItemEntity();
		it_ent->gameEntity->classname = it->Classname;
		it_ent->Spawn(it);
		it_ent->Touch (ent, NULL, NULL);
		if (it_ent->GetInUse())
			it_ent->Free ();
		//it->Add (ent, 1);
	}
}

// Paril
// This is a different style of "give".
// Allows you to spawn the item instead of giving it.
// Also, you can spawn monsters and other goodies from here.
void Cmd_Give (CPlayerEntity *ent)
{
	// Handle give all from old give.
	if (Q_stricmp (ArgGets(1).c_str(), "all") == 0)
	{
		Cmd_Give_f (ent);
		return;
	}

	// Calculate spawning direction
	vec3f Origin, Angles, forward;

	Angles = ent->State.GetAngles();
	Angles.X = Angles.Z = 0; // only yaw

	Angles.ToVectors (&forward, NULL, NULL);
	Origin = ent->State.GetOrigin().MultiplyAngles (150, forward);

	if (PointContents(Origin) & CONTENTS_SOLID)
		return;

	CBaseEntity *Spawned = CreateEntityFromClassname(ArgGetConcatenatedString().c_str());
	if (Spawned && Spawned->GetInUse())
	{
		Spawned->State.GetOrigin() = Origin;
		Spawned->State.GetAngles() = Angles;

		Spawned->Link ();
	}
}
