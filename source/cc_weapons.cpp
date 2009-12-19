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
// New item system code
//

#include "cc_local.h"
#include "cc_weaponmain.h"

CWeaponItem::CWeaponItem (char *Classname, char *WorldModel, sint32 EffectFlags,
			   char *PickupSound, char *Icon, char *Name, EItemFlags Flags,
			   char *Precache, class CWeapon *Weapon, class CAmmo *Ammo, sint32 Amount, char *VWepModel) :
CBaseItem(Classname, WorldModel, EffectFlags, PickupSound, Icon, Name, Flags, Precache),
Weapon(Weapon),
Ammo(Ammo),
Amount(Amount),
VWepModel(VWepModel)
{
	if (!Weapon)
		DebugPrintf ("Warning: Weapon with no weapon!\n");
	else
		Weapon->Item = this;

	if (!Ammo && Amount)
		DebugPrintf ("Warning: Weapon with no ammo has quantity!\n");
}

CWeaponItem::CWeaponItem ()
{
}

CAmmo::CAmmo (char *Classname, char *WorldModel, sint32 EffectFlags,
			   char *PickupSound, char *Icon, char *Name, EItemFlags Flags,
			   char *Precache, sint32 Quantity, CAmmo::EAmmoTag Tag) :
CBaseItem (Classname, WorldModel, EffectFlags, PickupSound, Icon, Name, Flags,
		   Precache),
Quantity(Quantity),
Tag(Tag)
{
}

CAmmo::CAmmo ()
{
}

CAmmoWeapon::CAmmoWeapon (char *Classname, char *WorldModel, sint32 EffectFlags,
			   char *PickupSound, char *Icon, char *Name, EItemFlags Flags,
			   char *Precache, class CWeapon *Weapon, class CAmmo *Ammo, sint32 Amount, char *VWepModel, sint32 Quantity, CAmmo::EAmmoTag Tag) :
CBaseItem (Classname, WorldModel, EffectFlags, PickupSound, Icon, Name, Flags,
		   Precache)
{
	this->Weapon = Weapon;
	this->Ammo = Ammo;
	this->Amount = Amount;
	this->VWepModel = VWepModel;
	this->Quantity = Quantity;
	this->Tag = Tag;

	if (!Weapon)
		DebugPrintf ("Warning: Weapon with no weapon!\n");
	else
		Weapon->Item = this;

	if (!Ammo && Amount)
		DebugPrintf ("Warning: Weapon with no ammo has quantity!\n");
}

CAmmoWeapon::CAmmoWeapon (char *Classname, char *WorldModel, sint32 EffectFlags,
			   char *PickupSound, char *Icon, char *Name, EItemFlags Flags,
			   char *Precache, class CWeapon *Weapon, sint32 Amount, char *VWepModel, sint32 Quantity, CAmmo::EAmmoTag Tag) :
CBaseItem (Classname, WorldModel, EffectFlags, PickupSound, Icon, Name, Flags,
		   Precache)
{
	this->Weapon = Weapon;
	this->Ammo = this;
	this->Amount = Amount;
	this->VWepModel = VWepModel;
	this->Quantity = Quantity;
	this->Tag = Tag;

	if (!Weapon)
		DebugPrintf ("Warning: Weapon with no weapon!\n");
	else
		Weapon->Item = this;

	if (!Ammo && Amount)
		DebugPrintf ("Warning: Weapon with no ammo has quantity!\n");
}

#ifndef NO_AUTOSWITCH
bool CheckAutoSwitch (CPlayerEntity *other)
{
	sint32 val = atoi(Info_ValueForKey (other->Client.Persistent.UserInfo, "cc_autoswitch").c_str());
	return (val == 1);
}
#endif

bool CWeaponItem::Pickup (class CItemEntity *ent, CPlayerEntity *other)
{
	if ( (dmFlags.dfWeaponsStay.IsEnabled() || game.mode == GAME_COOPERATIVE) 
		&& other->Client.Persistent.Inventory.Has(GetIndex()))
	{
		if (!(ent->SpawnFlags & (DROPPED_ITEM | DROPPED_PLAYER_ITEM) ) )
			return false;	// leave the weapon for others to pickup
	}

	other->Client.Persistent.Inventory += this;

	if (!(ent->SpawnFlags & DROPPED_ITEM) )
	{
		// give them some ammo with it
		if (Ammo)
		{
			if (dmFlags.dfInfiniteAmmo.IsEnabled())
				Ammo->AddAmmo (other, 1000);
			else
				Ammo->AddAmmo (other, Ammo->Quantity);
		}

		if (! (ent->SpawnFlags & DROPPED_PLAYER_ITEM) )
		{
			if (game.mode & GAME_DEATHMATCH)
			{
				if (dmFlags.dfWeaponsStay.IsEnabled())
					ent->Flags |= FL_RESPAWN;
				else
					SetRespawn (ent, 300);
			}
			if (game.mode == GAME_COOPERATIVE)
				ent->Flags |= FL_RESPAWN;
		}
	}
	else if (ent->AmmoCount)
		Ammo->AddAmmo (other, ent->AmmoCount);

	if (Weapon)
	{
		if (other->Client.Persistent.Weapon != Weapon && 
#ifndef NO_AUTOSWITCH
			(CheckAutoSwitch(other) || 
#endif
			((other->Client.Persistent.Inventory.Has(this) == 1)) &&
			( !(game.mode & GAME_DEATHMATCH) || (other->Client.Persistent.Weapon && other->Client.Persistent.Weapon->Item == NItems::Blaster) ) )
#ifndef NO_AUTOSWITCH
			)
#endif
			other->Client.NewWeapon = Weapon;
	}


	return true;
}

void CWeaponItem::Use (CPlayerEntity *ent)
{
	// see if we're already using it
	if (Weapon == ent->Client.Persistent.Weapon)
		return;

	if (Ammo && !g_select_empty->Integer() && !(Flags & ITEMFLAG_AMMO))
	{
		if (!ent->Client.Persistent.Inventory.Has(Ammo->GetIndex()))
		{
			ent->PrintToClient (PRINT_HIGH, "No %s for %s.\n", Ammo->Name, Name);
			return;
		}

		if (ent->Client.Persistent.Inventory.Has(Ammo->GetIndex()) < Amount)
		{
			ent->PrintToClient (PRINT_HIGH, "Not enough %s for %s.\n", Ammo->Name, Name);
			return;
		}
	}

	// change to this weapon when down
	ent->Client.NewWeapon = Weapon;
}

void CWeaponItem::Drop (CPlayerEntity *ent)
{
	if ((Weapon == ent->Client.Persistent.Weapon) && (ent->Client.WeaponState != WS_IDLE))
		return;

	DropItem(ent);

	ent->Client.Persistent.Inventory -= this;

	if (Weapon == ent->Client.Persistent.Weapon)
		ent->Client.Persistent.Weapon->NoAmmoWeaponChange(ent);
}

sint32 maxBackpackAmmoValues[CAmmo::AMMOTAG_MAX] =
{
	200,
	300,
	100,
	100,
	300,
	100
};
sint32 maxBandolierAmmoValues[CAmmo::AMMOTAG_MAX] =
{
	150,
	250,
	50,
	50,
	250,
	75
};

sint32 CAmmo::GetMax (CPlayerEntity *ent)
{
	return ent->Client.Persistent.MaxAmmoValues[Tag];
}

bool CAmmo::Pickup (class CItemEntity *ent, CPlayerEntity *other)
{
	sint32			count = Quantity;

	if (ent->AmmoCount)
		count = ent->AmmoCount;

	if (!AddAmmo (other, count))
		return false;

	if (!(ent->SpawnFlags & (DROPPED_ITEM | DROPPED_PLAYER_ITEM)) && (game.mode & GAME_DEATHMATCH))
		SetRespawn (ent, 300);
	return true;
}

void CAmmo::Use (CPlayerEntity *ent)
{
}

void CAmmo::Drop (CPlayerEntity *ent)
{
	sint32 count = Quantity;
	CItemEntity *dropped = DropItem(ent);

	if (count > ent->Client.Persistent.Inventory.Has(this))
		count = ent->Client.Persistent.Inventory.Has(this);

	dropped->AmmoCount = count;

	ent->Client.Persistent.Inventory.Remove (this, count);
}

bool CAmmoWeapon::Pickup (class CItemEntity *ent, CPlayerEntity *other)
{
	sint32			oldcount = other->Client.Persistent.Inventory.Has(this);
	sint32			count = Quantity;
	bool		weapon = (Flags & ITEMFLAG_WEAPON);

	if (weapon && dmFlags.dfInfiniteAmmo.IsEnabled())
		count = 1000;
	else if (ent->AmmoCount)
		count = ent->AmmoCount;

	if (!AddAmmo (other, count))
		return false;

	if (weapon && !oldcount)
	{
		if (other->Client.Persistent.Weapon != Weapon && (!(game.mode & GAME_DEATHMATCH) ||
			(other->Client.Persistent.Weapon && other->Client.Persistent.Weapon->Item == NItems::Blaster)))
			other->Client.NewWeapon = Weapon;
	}

	if (!(ent->SpawnFlags & (DROPPED_ITEM | DROPPED_PLAYER_ITEM)) && (game.mode & GAME_DEATHMATCH))
		SetRespawn (ent, 300);
	return true;
}

void CAmmoWeapon::Use (CPlayerEntity *ent)
{
	if (!(Flags & ITEMFLAG_WEAPON))
		return;

	// see if we're already using it
	if (Weapon == ent->Client.Persistent.Weapon)
		return;

	if (!g_select_empty->Integer())
	{
		if (!ent->Client.Persistent.Inventory.Has(GetIndex()))
		{
			ent->PrintToClient (PRINT_HIGH, "No %s for %s.\n", Name, Name);
			return;
		}

		if (ent->Client.Persistent.Inventory.Has(GetIndex()) < Amount)
		{
			ent->PrintToClient (PRINT_HIGH, "Not enough %s.\n", Name);
			return;
		}
	}

	// change to this weapon when down
	ent->Client.NewWeapon = Weapon;
}

void CAmmoWeapon::Drop (CPlayerEntity *ent)
{
	sint32 count = Quantity;
	CItemEntity *dropped = DropItem(ent);

	if (count > ent->Client.Persistent.Inventory.Has(this))
		count = ent->Client.Persistent.Inventory.Has(this);

	dropped->AmmoCount = count;

	ent->Client.Persistent.Inventory.Remove (this, count);

	if (Weapon && ent->Client.Persistent.Weapon && (ent->Client.Persistent.Weapon == Weapon) &&
		!ent->Client.Persistent.Inventory.Has(this))
		ent->Client.Persistent.Weapon->NoAmmoWeaponChange(ent);
}

bool CAmmo::AddAmmo (CPlayerEntity *ent, sint32 count)
{
	// YUCK
	sint32 max = GetMax(ent);

	if (!max)
		return false;

	if (ent->Client.Persistent.Inventory.Has(this) < max)
	{
		ent->Client.Persistent.Inventory.Add (this, count);

		if (ent->Client.Persistent.Inventory.Has(this) > max)
			ent->Client.Persistent.Inventory.Set(this, max);

		return true;
	}
	return false;
}

class CAmmoEntity : public CItemEntity
{
public:
	CAmmoEntity() :
	  CBaseEntity(),
	  CItemEntity ()
	  {
	  };

	CAmmoEntity (sint32 Index) :
	  CBaseEntity(Index),
	  CItemEntity (Index)
	  {
	  };

	void Spawn (CBaseItem *item)
	{
		if ((game.mode & GAME_DEATHMATCH) && dmFlags.dfInfiniteAmmo.IsEnabled())
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

LINK_ITEM_TO_CLASS (ammo_shells, CAmmoEntity);
LINK_ITEM_TO_CLASS (ammo_bullets, CAmmoEntity);
LINK_ITEM_TO_CLASS (ammo_slugs, CAmmoEntity);
LINK_ITEM_TO_CLASS (ammo_grenades, CAmmoEntity);
LINK_ITEM_TO_CLASS (ammo_rockets, CAmmoEntity);
LINK_ITEM_TO_CLASS (ammo_cells, CAmmoEntity);

LINK_ITEM_TO_CLASS (weapon_shotgun, CItemEntity);
LINK_ITEM_TO_CLASS (weapon_supershotgun, CItemEntity);
LINK_ITEM_TO_CLASS (weapon_machinegun, CItemEntity);
LINK_ITEM_TO_CLASS (weapon_chaingun, CItemEntity);
LINK_ITEM_TO_CLASS (weapon_grenadelauncher, CItemEntity);
LINK_ITEM_TO_CLASS (weapon_rocketlauncher, CItemEntity);
LINK_ITEM_TO_CLASS (weapon_hyperblaster, CItemEntity);
LINK_ITEM_TO_CLASS (weapon_railgun, CItemEntity);
LINK_ITEM_TO_CLASS (weapon_bfg, CItemEntity);

void AddWeapons (CItemList *List);
void AddAmmoToList ()
{
	NItems::Shells = QNew (com_itemPool, 0) CAmmo("ammo_shells", "models/items/ammo/shells/medium/tris.md2", 0, "misc/am_pkup.wav", "a_shells", "Shells", ITEMFLAG_DROPPABLE|ITEMFLAG_AMMO|ITEMFLAG_GRABBABLE, "", 10, CAmmo::AMMOTAG_SHELLS);
	NItems::Bullets = QNew (com_itemPool, 0) CAmmo("ammo_bullets", "models/items/ammo/bullets/medium/tris.md2", 0, "misc/am_pkup.wav", "a_bullets", "Bullets", ITEMFLAG_DROPPABLE|ITEMFLAG_AMMO|ITEMFLAG_GRABBABLE, "", 50, CAmmo::AMMOTAG_BULLETS);
	NItems::Slugs = QNew (com_itemPool, 0) CAmmo("ammo_slugs", "models/items/ammo/slugs/medium/tris.md2", 0, "misc/am_pkup.wav", "a_slugs", "Slugs", ITEMFLAG_DROPPABLE|ITEMFLAG_AMMO|ITEMFLAG_GRABBABLE, "", 10, CAmmo::AMMOTAG_SLUGS);
	NItems::Grenades = QNew (com_itemPool, 0) CAmmoWeapon("ammo_grenades", "models/items/ammo/grenades/medium/tris.md2", 0, "misc/am_pkup.wav", "a_grenades", "Grenades", ITEMFLAG_DROPPABLE|ITEMFLAG_AMMO|ITEMFLAG_USABLE|ITEMFLAG_GRABBABLE|ITEMFLAG_WEAPON, "", &CHandGrenade::Weapon, 1, "#a_grenades.md2", 5, CAmmo::AMMOTAG_GRENADES);
	NItems::Rockets = QNew (com_itemPool, 0) CAmmo("ammo_rockets", "models/items/ammo/rockets/medium/tris.md2", 0, "misc/am_pkup.wav", "a_rockets", "Rockets", ITEMFLAG_DROPPABLE|ITEMFLAG_AMMO|ITEMFLAG_GRABBABLE, "", 5, CAmmo::AMMOTAG_ROCKETS);
	NItems::Cells = QNew (com_itemPool, 0) CAmmo("ammo_cells", "models/items/ammo/cells/medium/tris.md2", 0, "misc/am_pkup.wav", "a_cells", "Cells", ITEMFLAG_DROPPABLE|ITEMFLAG_AMMO|ITEMFLAG_GRABBABLE, "", 50, CAmmo::AMMOTAG_CELLS);

	AddWeapons (ItemList);
}
