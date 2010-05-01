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

CInventory::CInventory () :
Array (),
SelectedItem (-1)
{
}

CInventory::~CInventory ()
{
};

void CInventory::Add (CBaseItem *Item, sint32 Num)
{
	Array[Item->GetIndex()] += Num;
}

void CInventory::Remove (CBaseItem *Item, sint32 Num)
{
	TInventoryMapType::iterator it = Array.find (Item->GetIndex());

	if (it == Array.end())
		return;

	if (Array[Item->GetIndex()] <= Num)
		Array.erase (it);
	else
		Array[Item->GetIndex()] -= Num;
}

void CInventory::Add (uint8 Index, sint32 Num)
{
	Array[Index] += Num;
}

void CInventory::Remove (uint8 Index, sint32 Num)
{
	TInventoryMapType::iterator it = Array.find (Index);

	if (it == Array.end())
		return;

	if (Array[Index] <= Num)
		Array.erase (it);
	else
		Array[Index] -= Num;
}

void CInventory::Draw (CPlayerEntity *Player)
{
	Player->Client.LayoutFlags &= ~(LF_SHOWSCORES | LF_SHOWHELP);
	if (Player->Client.LayoutFlags & LF_SHOWINVENTORY)
	{
		Player->Client.LayoutFlags &= ~LF_SHOWINVENTORY;
		return;
	}

	Player->Client.LayoutFlags |= LF_SHOWINVENTORY;

	WriteByte (SVC_INVENTORY);
	for (uint16 i = 0; i < MAX_CS_ITEMS; i++)
	{
		TInventoryMapType::iterator it = Array.find(i);

		WriteShort ((it == Array.end()) ? 0 : (*it).second);
	}
	Player->CastTo (CASTFLAG_RELIABLE);
}

void CInventory::SelectNextItem(EItemFlags Flags)
{
	sint32			index;

	// scan  for the next valid one
	for (uint16 i = 1; i <= MAX_CS_ITEMS; i++)
	{
		index = (SelectedItem + i) % MAX_CS_ITEMS;
		if (Array.find(index) == Array.end())
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
	sint32			index;

	// scan  for the next valid one
	for (uint16 i = 1; i <= MAX_CS_ITEMS; i++)
	{
		index = (SelectedItem + MAX_CS_ITEMS - i) % MAX_CS_ITEMS;
		if (Array.find(index) == Array.end())
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

sint32 CInventory::Has (uint8 Index)
{
	TInventoryMapType::iterator it = Array.find(Index);

	if (it == Array.end())
		return 0;

	return (*it).second;
}

sint32 CInventory::Has (CBaseItem *Item)
{
	TInventoryMapType::iterator it = Array.find(Item->GetIndex());

	if (it == Array.end())
		return 0;

	return (*it).second;
}

void CInventory::Set (CBaseItem *Item, sint32 Num)
{
	if (Num == 0)
	{
		TInventoryMapType::iterator it = Array.find(Item->GetIndex());

		if (it != Array.end())
			Array.erase (it);
	}
	else
		Array[Item->GetIndex()] = Num;
}

void CInventory::Set (uint8 Index, sint32 Num)
{
	if (Num == 0)
	{
		TInventoryMapType::iterator it = Array.find(Index);

		if (it != Array.end())
			Array.erase (it);
	}
	else
		Array[Index] = Num;
}

void CInventory::ValidateSelectedItem()
{
	if (SelectedItem == -1 || Array.find(SelectedItem) != Array.end())
		return;		// valid

	SelectNextItem (-1);
}

void CInventory::operator += (CBaseItem *Item)
{
	Add(Item, 1);
}

void CInventory::operator += (uint8 Index)
{
	Add(GetItemByIndex(Index), 1);
}

void CInventory::operator -= (CBaseItem *Item)
{
	Remove(Item, 1);
}

void CInventory::operator -= (uint8 Index)
{
	Remove (GetItemByIndex(Index), 1);
}

/*
==================
Cmd_Use_f

Use an inventory item
==================
*/
void CUseCommand::operator () ()
{
	std::string s = ArgGetConcatenatedString();
	CBaseItem *Item = FindItem(s.c_str());

	if (!Item)
	{
		Item = FindItemByClassname(s.c_str());

		if (!Item)
		{
			Player->PrintToClient (PRINT_HIGH, "Unknown item: %s\n", s.c_str());
			return;
		}
	}
	if (!(Item->Flags & ITEMFLAG_USABLE))
	{
		Player->PrintToClient (PRINT_HIGH, "Item is not usable.\n");
		return;
	}
	if ((!(Item->Flags & ITEMFLAG_WEAPON)) && !Player->Client.Persistent.Inventory.Has(Item))
	{
		Player->PrintToClient (PRINT_HIGH, "Out of item: %s\n", s.c_str());
		return;
	}

	Item->Use (Player);
	Player->Client.Persistent.Inventory.ValidateSelectedItem();
}


/*
==================
Cmd_UseList_f

Tries to use the items in the list from left to right, stopping
when one gets used
==================
*/
void CUseListCommand::operator () ()
{
	for (sint32 i = 1; i < ArgCount(); i++)
	{
		std::string s = ArgGets(i);
		CBaseItem *Item = FindItem(s.c_str());

		if (!Item)
		{
			Item = FindItemByClassname(s.c_str());

			if (!Item)
			{
				Player->PrintToClient (PRINT_HIGH, "Unknown item: %s\n", s.c_str());
				continue;
			}
		}
		if (!(Item->Flags & ITEMFLAG_USABLE))
		{
			Player->PrintToClient (PRINT_HIGH, "Item is not usable.\n");
			continue;
		}

		// Only warn us if it's unknown or not usable; not if we don't have it
		if (!Player->Client.Persistent.Inventory.Has(Item))
			continue;

		// If it's a weapon and we don't have ammo, don't bother
		if ((Item->Flags & ITEMFLAG_WEAPON) && !(Item->Flags & ITEMFLAG_AMMO))
		{
			CWeaponItem *Weapon = dynamic_cast<CWeaponItem*>(Item);
			if (!Player->Client.Persistent.Inventory.Has(Weapon->Ammo))
				continue;
		}

		Item->Use (Player);
		Player->Client.Persistent.Inventory.ValidateSelectedItem();
		return;
	}
	Player->PrintToClient (PRINT_HIGH, "No item in list found!\n");
}

/*
==================
Cmd_Drop_f

Drop an inventory item
==================
*/
void CDropCommand::operator () ()
{
	std::string s = ArgGetConcatenatedString();

	if (Q_stricmp (s.c_str(), "tech") == 0)
	{
		if (Player->Client.Persistent.Tech)
		{
			Player->Client.Persistent.Tech->Drop (Player);
			Player->Client.Persistent.Inventory.ValidateSelectedItem();
		}
		return;
	}

	CBaseItem *Item = FindItem(s.c_str());
	if (!Item)
	{
		Item = FindItemByClassname(s.c_str());

		if (!Item)
		{
			Player->PrintToClient (PRINT_HIGH, "Unknown item: %s\n", s.c_str());
			return;
		}
	}
	if (!(Item->Flags & ITEMFLAG_DROPPABLE))
	{
		Player->PrintToClient (PRINT_HIGH, "Item is not dropable.\n");
		return;
	}
	if (!Player->Client.Persistent.Inventory.Has(Item))
	{
		Player->PrintToClient (PRINT_HIGH, "Out of item: %s\n", s.c_str());
		return;
	}

	Item->Drop (Player);
	Player->Client.Persistent.Inventory.ValidateSelectedItem();
}


/*
=================
Cmd_Inven_f
=================
*/
void CInventoryCommand::operator () ()
{
	if (Level.IntermissionTime)
		return;

	if (Player->Client.Respawn.MenuState.InMenu)
	{
		Player->Client.Respawn.MenuState.CloseMenu();
		Player->Client.LayoutFlags |= LF_UPDATECHASE;
		return;
	}
#if CLEANCTF_ENABLED
//ZOID
	if ((Game.GameMode & GAME_CTF) && Player->Client.Respawn.CTF.Team == CTF_NOTEAM)
	{
		CTFOpenJoinMenu(Player);
		return;
	}
//ZOID
#endif

	Player->Client.Persistent.Inventory.Draw(Player);
}

/*
=================
Cmd_InvUse_f
=================
*/
void CInvUseCommand::operator () ()
{
	if (Player->Client.Respawn.MenuState.InMenu)
	{
		Player->Client.Respawn.MenuState.Select();
		return;
	}
	if (Player->Health <= 0 || Player->DeadFlag)
		return;

	Player->Client.Persistent.Inventory.ValidateSelectedItem ();

	if (Player->Client.Persistent.Inventory.SelectedItem == -1)
	{
		Player->PrintToClient (PRINT_HIGH, "No item to use.\n");
		return;
	}

	CBaseItem *it = GetItemByIndex(Player->Client.Persistent.Inventory.SelectedItem);
	if (!(it->Flags & ITEMFLAG_USABLE))
	{
		Player->PrintToClient (PRINT_HIGH, "Item is not usable.\n");
		return;
	}
	it->Use (Player);
	Player->Client.Persistent.Inventory.ValidateSelectedItem ();
}

/*
=================
Cmd_WeapPrev_f
=================
*/
void CWeapPrevCommand::operator () ()
{
	if (!Player->Client.Persistent.Weapon)
		return;
	if (Player->Health <= 0 || Player->DeadFlag)	
		return;

	sint32 selectedWeaponIndex = Player->Client.Persistent.Weapon->Item->GetIndex();

	// scan  for the next valid one
	for (uint8 i = 1; i <= GetNumItems(); i++)
	{
		sint32 index = (selectedWeaponIndex + MAX_ITEMS - i) % MAX_ITEMS;
		if (!Player->Client.Persistent.Inventory.Has(index))
			continue;
		CBaseItem *Item = GetItemByIndex(index);
		if (!(Item->Flags & ITEMFLAG_USABLE))
			continue;
		if (! (Item->Flags & ITEMFLAG_WEAPON) )
			continue;
		Item->Use (Player);
		if (Player->Client.NewWeapon && Player->Client.NewWeapon->Item == Item)
			return;	// successful
	}
}

/*
=================
Cmd_WeapNext_f
=================
*/
void CWeapNextCommand::operator () ()
{
	if (!Player->Client.Persistent.Weapon)
		return;
	if (Player->Health <= 0 || Player->DeadFlag)
		return;

	sint32 selectedWeaponIndex = Player->Client.Persistent.Weapon->Item->GetIndex();

	// scan  for the next valid one
	for (uint8 i = 1; i <= GetNumItems(); i++)
	{
		sint32 index = (selectedWeaponIndex + i) % MAX_ITEMS;
		if (!Player->Client.Persistent.Inventory.Has(index))
			continue;
		CBaseItem *Item = GetItemByIndex(index);
		if (!(Item->Flags & ITEMFLAG_USABLE))
			continue;
		if (! (Item->Flags & ITEMFLAG_WEAPON) )
			continue;
		Item->Use (Player);
		if (Player->Client.NewWeapon && Player->Client.NewWeapon->Item == Item)
			return;	// successful
	}
}

/*
=================
Cmd_WeapLast_f
=================
*/
void CWeapLastCommand::operator () ()
{
	if (!Player->Client.Persistent.Weapon || !Player->Client.Persistent.LastWeapon)
		return;

	if (!Player->Client.Persistent.Inventory.Has(Player->Client.Persistent.LastWeapon->Item))
		return;
	if (!(Player->Client.Persistent.LastWeapon->Item->Flags & ITEMFLAG_USABLE))
		return;
	if (! (Player->Client.Persistent.LastWeapon->Item->Flags & ITEMFLAG_WEAPON) )
		return;
	Player->Client.Persistent.LastWeapon->Item->Use (Player);
}

/*
=================
Cmd_InvDrop_f
=================
*/
void CInvDropCommand::operator () ()
{
	if (Player->Health <= 0 || Player->DeadFlag)
		return;

	Player->Client.Persistent.Inventory.ValidateSelectedItem ();

	if (Player->Client.Persistent.Inventory.SelectedItem == -1)
	{
		Player->PrintToClient (PRINT_HIGH, "No item to drop.\n");
		return;
	}

	CBaseItem *Item = GetItemByIndex(Player->Client.Persistent.Inventory.SelectedItem);
	if (!(Item->Flags & ITEMFLAG_DROPPABLE))
	{
		Player->PrintToClient (PRINT_HIGH, "Item is not dropable.\n");
		return;
	}
	Item->Drop (Player);
	Player->Client.Persistent.Inventory.ValidateSelectedItem();
}

void CInvNextCommand::operator () ()
{
	if (Player->Health <= 0 || Player->DeadFlag)
		return;

	if (Player->Client.Respawn.MenuState.InMenu)
	{
		Player->Client.Respawn.MenuState.SelectNext();
		return;
	}

	if (Player->Client.Chase.Target)
	{
		Player->ChaseNext();
		return;
	}

	Player->Client.Persistent.Inventory.SelectNextItem (-1);
}
void CInvPrevCommand::operator () ()
{
	if (Player->Health <= 0 || Player->DeadFlag)
		return;

	if (Player->Client.Respawn.MenuState.InMenu)
	{
		Player->Client.Respawn.MenuState.SelectPrev();
		return;
	}

	if (Player->Client.Chase.Target)
	{
		Player->ChasePrev();
		return;
	}

	Player->Client.Persistent.Inventory.SelectPrevItem (-1);
}
void CInvNextWCommand::operator () ()
{
	if (Player->Health <= 0 || Player->DeadFlag)
		return;

	Player->Client.Persistent.Inventory.SelectNextItem (ITEMFLAG_WEAPON);
}
void CInvPrevWCommand::operator () ()
{
	if (Player->Health <= 0 || Player->DeadFlag)
		return;

	Player->Client.Persistent.Inventory.SelectPrevItem (ITEMFLAG_WEAPON);
}
void CInvNextPCommand::operator () ()
{
	if (Player->Health <= 0 || Player->DeadFlag)
		return;

	Player->Client.Persistent.Inventory.SelectNextItem (ITEMFLAG_POWERUP);
}
void CInvPrevPCommand::operator () ()
{
	if (Player->Health <= 0 || Player->DeadFlag)
		return;

	Player->Client.Persistent.Inventory.SelectPrevItem (ITEMFLAG_POWERUP);
}

/*
==================
Cmd_Give_f

Give items to a client
Old-style "give"
==================
*/

inline IBaseEntity *SpawnEntityAtPlace (std::string ClassName, vec3f Origin, vec3f Angles)
{
	IBaseEntity *Spawned = CreateEntityFromClassname(ClassName.c_str());
	if (Spawned && Spawned->GetInUse())
	{
		Spawned->State.GetOrigin() = Origin;
		Spawned->State.GetAngles() = Angles;

		Spawned->Link ();
		
		return Spawned;
	}

	return NULL;
}

void Cmd_Give (CPlayerEntity *Player)
{
	CBaseItem *it;

	std::string name = ArgGetConcatenatedString();
	bool give_all = (Q_stricmp (name.c_str(), "all") == 0);

	if (give_all || Q_stricmp (name.c_str(), "health") == 0)
	{
		if (ArgCount() == 3)
			Player->Health = ArgGeti(2);
		else
			Player->Health = Player->MaxHealth;
		if (!give_all)
			return;
	}

	if (give_all || Q_stricmp (name.c_str(), "weapons") == 0)
	{
		for (sint32 i = 0; i < GetNumItems(); i++)
		{
			it = GetItemByIndex(i);
			if (!(it->Flags & ITEMFLAG_WEAPON))
				continue;
			Player->Client.Persistent.Inventory += it;
		}
		if (!give_all)
			return;
	}

	if (give_all || Q_stricmp (name.c_str(), "ammo") == 0)
	{
		for (sint32 i = 0; i < GetNumItems(); i++)
		{
			it = GetItemByIndex(i);
			if (!(it->Flags & ITEMFLAG_GRABBABLE))
				continue;
			if (!(it->Flags & ITEMFLAG_AMMO))
				continue;

			CAmmo *Ammo = dynamic_cast<CAmmo*>(it);
			Ammo->AddAmmo (Player, 1000);
		}
		if (!give_all)
			return;
	}

	if (give_all || Q_stricmp (name.c_str(), "armor") == 0)
	{
		Player->Client.Persistent.Inventory.Set(NItems::JacketArmor->GetIndex(), 0);
		Player->Client.Persistent.Inventory.Set(NItems::CombatArmor->GetIndex(), 0);

		CArmor *Armor = dynamic_cast<CArmor*>(NItems::BodyArmor);
		Player->Client.Persistent.Inventory.Set(Armor->GetIndex(), Armor->maxCount);
		Player->Client.Persistent.Armor = Armor;

		if (ArgCount() >= 3)
			Player->Client.Persistent.Inventory.Set(Armor->GetIndex(), ArgGeti(2));

		if (!give_all)
			return;
	}

	if (give_all || Q_stricmp (name.c_str(), "power shield") == 0)
	{
		NItems::PowerShield->Add (Player, 1);

		if (!give_all)
			return;
	}

	if (give_all)
	{
		for (sint32 i = 0; i < GetNumItems(); i++)
		{
			it = GetItemByIndex(i);
			if (!(it->Flags & ITEMFLAG_GRABBABLE))
				continue;
			if (it->Flags & (ITEMFLAG_HEALTH|ITEMFLAG_ARMOR|ITEMFLAG_WEAPON|ITEMFLAG_AMMO|ITEMFLAG_NOT_GIVEABLE))
				continue;
			Player->Client.Persistent.Inventory.Set (i, 1);
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
				Player->PrintToClient (PRINT_HIGH, "Unknown Item\n");
				return;
			}
		}
	}

	if (it->Flags & ITEMFLAG_NOT_GIVEABLE)		
	{
		Player->PrintToClient (PRINT_HIGH, "Item cannot be given\n");
		return;							
	}

	if (!(it->Flags & ITEMFLAG_GRABBABLE))
	{
		Player->Client.Persistent.Inventory += it;
		return;
	}

	if (it->Flags & ITEMFLAG_AMMO)
	{
		CAmmo *Ammo = dynamic_cast<CAmmo*>(it);
		if (ArgCount() == 3)
			Player->Client.Persistent.Inventory.Set (Ammo, ArgGeti(2));
		else
			Player->Client.Persistent.Inventory.Add(Ammo, Ammo->Quantity);
	}
	else
	{
		ITouchableEntity *ItemEntity = entity_cast<ITouchableEntity>(SpawnEntityAtPlace (it->Classname, Player->State.GetOrigin(), Player->State.GetAngles()));
		ItemEntity->Touch (Player, NULL, NULL);
		if (ItemEntity->GetInUse())
			ItemEntity->Free ();
	}
}

void CGiveCommand::operator () ()
{
	Cmd_Give (Player);
}

// Paril
// This is a different style of "give".
// Allows you to spawn the item instead of giving it.
// Also, you can spawn monsters and other goodies from here.
void Cmd_Spawn (CPlayerEntity *Player)
{
	// Handle give all from old give.
	if (Q_stricmp (ArgGets(1).c_str(), "all") == 0)
	{
		Cmd_Give (Player);
		return;
	}

	// Calculate spawning direction
	vec3f Origin, Angles, forward;

	Angles = Player->State.GetAngles();
	Angles.X = Angles.Z = 0; // only yaw

	Angles.ToVectors (&forward, NULL, NULL);
	Origin = Player->State.GetOrigin().MultiplyAngles (150, forward);

	if (PointContents(Origin) & CONTENTS_SOLID)
		return;

	SpawnEntityAtPlace (ArgGetConcatenatedString(), Origin, Angles);
}

void CSpawnCommand::operator () ()
{
	Cmd_Spawn (Player);
}
