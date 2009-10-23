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

CWeaponItem::CWeaponItem (char *Classname, char *WorldModel, int EffectFlags,
			   char *PickupSound, char *Icon, char *Name, EItemFlags Flags,
			   char *Precache, class CWeapon *Weapon, class CAmmo *Ammo, int Quantity, char *VWepModel) :
CBaseItem(Classname, WorldModel, EffectFlags, PickupSound, Icon, Name, Flags, Precache),
Weapon(Weapon),
Ammo(Ammo),
Quantity(Quantity),
VWepModel(VWepModel)
{
	if (!Weapon)
		DebugPrintf ("Warning: Weapon with no weapon!\n");
	else
	{
		Weapon->Item = this;
		Weapon->WeaponItem = this;
	}
	if (!Ammo && Quantity)
		DebugPrintf ("Warning: Weapon with no ammo has quantity!\n");
}

CAmmo::CAmmo (char *Classname, char *WorldModel, int EffectFlags,
			   char *PickupSound, char *Icon, char *Name, EItemFlags Flags,
			   char *Precache, int Quantity, CAmmo::EAmmoTag Tag, CWeapon *Weapon, int Amount, char *VWepModel) :
CBaseItem (Classname, WorldModel, EffectFlags, PickupSound, Icon, Name, Flags,
		   Precache),
Quantity(Quantity),
Tag(Tag),
Weapon(Weapon),
Amount(Amount),
VWepModel(VWepModel)
{
	if (Weapon)
		Weapon->Item = this;
}

#ifndef NO_AUTOSWITCH
bool CheckAutoSwitch (CPlayerEntity *other)
{
	int val = atoi(Info_ValueForKey (other->Client.Persistent.UserInfo, "cc_autoswitch").c_str());
	return (val == 1);
}
#endif

bool CWeaponItem::Pickup (class CItemEntity *ent, CPlayerEntity *other)
{
	if ( (dmFlags.dfWeaponsStay || game.mode == GAME_COOPERATIVE) 
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
			if (dmFlags.dfInfiniteAmmo)
				Ammo->AddAmmo (other, 1000);
			else
				Ammo->AddAmmo (other, Ammo->Quantity);
		}

		if (! (ent->SpawnFlags & DROPPED_PLAYER_ITEM) )
		{
			if (game.mode & GAME_DEATHMATCH)
			{
				if (dmFlags.dfWeaponsStay)
					ent->Flags |= FL_RESPAWN;
				else
					SetRespawn (ent, 300);
			}
			if (game.mode == GAME_COOPERATIVE)
				ent->Flags |= FL_RESPAWN;
		}
	}
	else if (ent->gameEntity->count)
		Ammo->AddAmmo (other, ent->gameEntity->count);

	if (Weapon)
	{
		if (other->Client.Persistent.Weapon != Weapon && 
#ifndef NO_AUTOSWITCH
			(CheckAutoSwitch(other) || 
#endif
			((other->Client.Persistent.Inventory.Has(this) == 1)) &&
			( !(game.mode & GAME_DEATHMATCH) || (other->Client.Persistent.Weapon && other->Client.Persistent.Weapon->WeaponItem == NItems::Blaster) ) )
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

		if (ent->Client.Persistent.Inventory.Has(Ammo->GetIndex()) < Quantity)
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

int maxBackpackAmmoValues[CAmmo::AMMOTAG_MAX] =
{
	200,
	300,
	100,
	100,
	300,
	100
};
int maxBandolierAmmoValues[CAmmo::AMMOTAG_MAX] =
{
	150,
	250,
	50,
	50,
	250,
	75
};

int CAmmo::GetMax (CPlayerEntity *ent)
{
	return ent->Client.Persistent.maxAmmoValues[Tag];
}

void CAmmo::Use (CPlayerEntity *ent)
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

void CAmmo::Drop (CPlayerEntity *ent)
{
	int count = Quantity;
	CItemEntity *dropped = DropItem(ent);

	if (count > ent->Client.Persistent.Inventory.Has(this))
		count = ent->Client.Persistent.Inventory.Has(this);

	dropped->gameEntity->count = count;

	ent->Client.Persistent.Inventory.Remove (this, count);

	if (Weapon && ent->Client.Persistent.Weapon && (ent->Client.Persistent.Weapon == Weapon) &&
		!ent->Client.Persistent.Inventory.Has(this))
		ent->Client.Persistent.Weapon->NoAmmoWeaponChange(ent);
}

bool CAmmo::AddAmmo (CPlayerEntity *ent, int count)
{
	// YUCK
	int max = GetMax(ent);

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

bool CAmmo::Pickup (class CItemEntity *ent, CPlayerEntity *other)
{
	int			oldcount;
	int			count;
	bool		weapon;

	weapon = (Flags & ITEMFLAG_WEAPON);

	if ( weapon && dmFlags.dfInfiniteAmmo )
		count = 1000;
	else if (ent->gameEntity->count)
		count = ent->gameEntity->count;
	else
		count = Quantity;

	oldcount = other->Client.Persistent.Inventory.Has(this);

	if (!AddAmmo (other, count))
		return false;

	if (weapon && !oldcount)
	{
		if (other->Client.Persistent.Weapon != Weapon && (!(game.mode & GAME_DEATHMATCH) ||
			(other->Client.Persistent.Weapon && other->Client.Persistent.Weapon->WeaponItem == NItems::Blaster)))
			other->Client.NewWeapon = Weapon;
	}

	if (!(ent->SpawnFlags & (DROPPED_ITEM | DROPPED_PLAYER_ITEM)) && (game.mode & GAME_DEATHMATCH))
		SetRespawn (ent, 300);
	return true;
}

class CAmmoEntity : public CItemEntity
{
public:
	CAmmoEntity() :
	  CBaseEntity(),
	  CItemEntity ()
	  {
	  };

	CAmmoEntity (int Index) :
	  CBaseEntity(Index),
	  CItemEntity (Index)
	  {
	  };

	void Spawn (CBaseItem *item)
	{
		if ((game.mode & GAME_DEATHMATCH) && dmFlags.dfInfiniteAmmo)
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

void AddAmmoToList ()
{
	NItems::Shells = QNew (com_gamePool, 0) CAmmo("ammo_shells", "models/items/ammo/shells/medium/tris.md2", 0, "misc/am_pkup.wav", "a_shells", "Shells", ITEMFLAG_DROPPABLE|ITEMFLAG_AMMO|ITEMFLAG_GRABBABLE, "", 10, CAmmo::AMMOTAG_SHELLS, NULL, -1, NULL);
	NItems::Bullets = QNew (com_gamePool, 0) CAmmo("ammo_bullets", "models/items/ammo/bullets/medium/tris.md2", 0, "misc/am_pkup.wav", "a_bullets", "Bullets", ITEMFLAG_DROPPABLE|ITEMFLAG_AMMO|ITEMFLAG_GRABBABLE, "", 50, CAmmo::AMMOTAG_BULLETS, NULL, -1, NULL);
	NItems::Slugs = QNew (com_gamePool, 0) CAmmo("ammo_slugs", "models/items/ammo/slugs/medium/tris.md2", 0, "misc/am_pkup.wav", "a_slugs", "Slugs", ITEMFLAG_DROPPABLE|ITEMFLAG_AMMO|ITEMFLAG_GRABBABLE, "", 10, CAmmo::AMMOTAG_SLUGS, NULL, -1, NULL);
	NItems::Grenades = QNew (com_gamePool, 0) CAmmo("ammo_grenades", "models/items/ammo/grenades/medium/tris.md2", 0, "misc/am_pkup.wav", "a_grenades", "Grenades", ITEMFLAG_DROPPABLE|ITEMFLAG_AMMO|ITEMFLAG_USABLE|ITEMFLAG_GRABBABLE|ITEMFLAG_WEAPON, "", 5, CAmmo::AMMOTAG_GRENADES, &WeaponGrenades, 1, "#a_grenades.md2");
	NItems::Rockets = QNew (com_gamePool, 0) CAmmo("ammo_rockets", "models/items/ammo/rockets/medium/tris.md2", 0, "misc/am_pkup.wav", "a_rockets", "Rockets", ITEMFLAG_DROPPABLE|ITEMFLAG_AMMO|ITEMFLAG_GRABBABLE, "", 5, CAmmo::AMMOTAG_ROCKETS, NULL, -1, NULL);
	NItems::Cells = QNew (com_gamePool, 0) CAmmo("ammo_cells", "models/items/ammo/cells/medium/tris.md2", 0, "misc/am_pkup.wav", "a_cells", "Cells", ITEMFLAG_DROPPABLE|ITEMFLAG_AMMO|ITEMFLAG_GRABBABLE, "", 50, CAmmo::AMMOTAG_CELLS, NULL, -1, NULL);

	ItemList->AddItemToList (NItems::Shells);
	ItemList->AddItemToList (NItems::Bullets);
	ItemList->AddItemToList (NItems::Slugs);
	ItemList->AddItemToList (NItems::Grenades);
	ItemList->AddItemToList (NItems::Rockets);
	ItemList->AddItemToList (NItems::Cells);

	// Weapons
	NItems::Blaster = QNew (com_gamePool, 0) CWeaponItem(NULL, NULL, 0, NULL, "w_blaster", "Blaster", ITEMFLAG_WEAPON|ITEMFLAG_USABLE, "", &WeaponBlaster, NULL, 0, "#w_blaster.md2");
	NItems::Shotgun = QNew (com_gamePool, 0) CWeaponItem("weapon_shotgun", "models/weapons/g_shotg/tris.md2", EF_ROTATE, "misc/w_pkup.wav", "w_shotgun", "Shotgun", ITEMFLAG_DROPPABLE|ITEMFLAG_WEAPON|ITEMFLAG_GRABBABLE|ITEMFLAG_STAY_COOP|ITEMFLAG_USABLE, "", &WeaponShotgun, NItems::Shells, 1, "#w_shotgun.md2");
	NItems::SuperShotgun = QNew (com_gamePool, 0) CWeaponItem("weapon_supershotgun", "models/weapons/g_shotg2/tris.md2", EF_ROTATE, "misc/w_pkup.wav", "w_sshotgun", "Super Shotgun", ITEMFLAG_DROPPABLE|ITEMFLAG_WEAPON|ITEMFLAG_GRABBABLE|ITEMFLAG_STAY_COOP|ITEMFLAG_USABLE, "", &WeaponSuperShotgun, NItems::Shells, 2, "#w_sshotgun.md2");
	NItems::Machinegun = QNew (com_gamePool, 0) CWeaponItem("weapon_machinegun", "models/weapons/g_machn/tris.md2", EF_ROTATE, "misc/w_pkup.wav", "w_machinegun", "Machinegun", ITEMFLAG_DROPPABLE|ITEMFLAG_WEAPON|ITEMFLAG_GRABBABLE|ITEMFLAG_STAY_COOP|ITEMFLAG_USABLE, "", &WeaponMachinegun, NItems::Bullets, 1, "#w_machinegun.md2");
	NItems::Chaingun = QNew (com_gamePool, 0) CWeaponItem("weapon_chaingun", "models/weapons/g_chain/tris.md2", EF_ROTATE, "misc/w_pkup.wav", "w_chaingun", "Chaingun", ITEMFLAG_DROPPABLE|ITEMFLAG_WEAPON|ITEMFLAG_GRABBABLE|ITEMFLAG_STAY_COOP|ITEMFLAG_USABLE, "", &WeaponChaingun, NItems::Bullets, 1, "#w_chaingun.md2");
	NItems::GrenadeLauncher = QNew (com_gamePool, 0) CWeaponItem("weapon_grenadelauncher", "models/weapons/g_launch/tris.md2", EF_ROTATE, "misc/w_pkup.wav", "w_glauncher", "Grenade Launcher", ITEMFLAG_DROPPABLE|ITEMFLAG_WEAPON|ITEMFLAG_GRABBABLE|ITEMFLAG_STAY_COOP|ITEMFLAG_USABLE, "", &WeaponGrenadeLauncher, NItems::Grenades, 1, "#w_glauncher.md2");
	NItems::RocketLauncher = QNew (com_gamePool, 0) CWeaponItem("weapon_rocketlauncher", "models/weapons/g_rocket/tris.md2", EF_ROTATE, "misc/w_pkup.wav", "w_rlauncher", "Rocket Launcher", ITEMFLAG_DROPPABLE|ITEMFLAG_WEAPON|ITEMFLAG_GRABBABLE|ITEMFLAG_STAY_COOP|ITEMFLAG_USABLE, "", &WeaponRocketLauncher, NItems::Rockets, 1, "#w_rlauncher.md2");
	NItems::HyperBlaster = QNew (com_gamePool, 0) CWeaponItem("weapon_hyperblaster", "models/weapons/g_hyperb/tris.md2", EF_ROTATE, "misc/w_pkup.wav", "w_hyperblaster", "HyperBlaster", ITEMFLAG_DROPPABLE|ITEMFLAG_WEAPON|ITEMFLAG_GRABBABLE|ITEMFLAG_STAY_COOP|ITEMFLAG_USABLE, "", &WeaponHyperBlaster, NItems::Cells, 1, "#w_hyperblaster.md2");
	NItems::Railgun = QNew (com_gamePool, 0) CWeaponItem("weapon_railgun", "models/weapons/g_rail/tris.md2", EF_ROTATE, "misc/w_pkup.wav", "w_railgun", "Railgun", ITEMFLAG_DROPPABLE|ITEMFLAG_WEAPON|ITEMFLAG_GRABBABLE|ITEMFLAG_STAY_COOP|ITEMFLAG_USABLE, "", &WeaponRailgun, NItems::Slugs, 1, "#w_railgun.md2");
	NItems::BFG = QNew (com_gamePool, 0) CWeaponItem("weapon_bfg", "models/weapons/g_bfg/tris.md2", EF_ROTATE, "misc/w_pkup.wav", "w_bfg", "BFG10k", ITEMFLAG_DROPPABLE|ITEMFLAG_WEAPON|ITEMFLAG_GRABBABLE|ITEMFLAG_STAY_COOP|ITEMFLAG_USABLE, "", &WeaponBFG, NItems::Cells, 50, "#w_bfg.md2");

#ifdef CLEANCTF_ENABLED
	NItems::Grapple = QNew (com_gamePool, 0) CWeaponItem (NULL, NULL, 0, NULL, "w_grapple", "Grapple", ITEMFLAG_WEAPON|ITEMFLAG_USABLE, "", &WeaponGrapple, NULL, 0, "#w_grapple.md2");
#endif

	ItemList->AddItemToList (NItems::Blaster);
	ItemList->AddItemToList (NItems::Shotgun);
	ItemList->AddItemToList (NItems::SuperShotgun);
	ItemList->AddItemToList (NItems::Machinegun);
	ItemList->AddItemToList (NItems::Chaingun);
	ItemList->AddItemToList (NItems::GrenadeLauncher);
	ItemList->AddItemToList (NItems::RocketLauncher);
	ItemList->AddItemToList (NItems::HyperBlaster);
	ItemList->AddItemToList (NItems::Railgun);
	ItemList->AddItemToList (NItems::BFG);

#ifdef CLEANCTF_ENABLED
	ItemList->AddItemToList (NItems::Grapple);
#endif

	if (map_debug->Boolean())
	{
		ItemList->AddItemToList (QNew (com_gamePool, 0) CWeaponItem (NULL, NULL, 0, NULL, NULL, "Surface Picker", ITEMFLAG_WEAPON|ITEMFLAG_USABLE, NULL, &Debug_SurfacePicker, NULL, 0, NULL));
	}
}

CC_ENUM (uint8, EWeaponVwepIndices)
{
	WEAP_NONE,
	WEAP_BLASTER,
	WEAP_SHOTGUN,
	WEAP_SUPERSHOTGUN,
	WEAP_MACHINEGUN,
	WEAP_CHAINGUN,
	WEAP_GRENADES,
	WEAP_GRENADELAUNCHER,
	WEAP_ROCKETLAUNCHER,
	WEAP_HYPERBLASTER,
	WEAP_RAILGUN,
	WEAP_BFG,
#ifdef CLEANCTF_ENABLED
	WEAP_GRAPPLE,
#endif
};

void DoWeaponVweps ()
{
	int takeAway = ModelIndex(NItems::Blaster->VWepModel) - 1;

	NItems::Blaster->Weapon->vwepIndex = ModelIndex(NItems::Blaster->VWepModel) - takeAway;
	NItems::Shotgun->Weapon->vwepIndex = ModelIndex(NItems::Shotgun->VWepModel) - takeAway;
	NItems::SuperShotgun->Weapon->vwepIndex = ModelIndex(NItems::SuperShotgun->VWepModel) - takeAway;
	NItems::Machinegun->Weapon->vwepIndex = ModelIndex(NItems::Machinegun->VWepModel) - takeAway;
	NItems::Chaingun->Weapon->vwepIndex = ModelIndex(NItems::Chaingun->VWepModel) - takeAway;
	NItems::GrenadeLauncher->Weapon->vwepIndex = ModelIndex(NItems::GrenadeLauncher->VWepModel) - takeAway;
	NItems::Grenades->Weapon->vwepIndex = ModelIndex(NItems::Grenades->VWepModel) - takeAway;
	NItems::RocketLauncher->Weapon->vwepIndex = ModelIndex(NItems::RocketLauncher->VWepModel) - takeAway;
	NItems::HyperBlaster->Weapon->vwepIndex = ModelIndex(NItems::HyperBlaster->VWepModel) - takeAway;
	NItems::Railgun->Weapon->vwepIndex = ModelIndex(NItems::Railgun->VWepModel) - takeAway;
	NItems::BFG->Weapon->vwepIndex = ModelIndex(NItems::BFG->VWepModel) - takeAway;
#ifdef CLEANCTF_ENABLED
	NItems::Grapple->Weapon->vwepIndex = ModelIndex(NItems::Grapple->VWepModel) - takeAway;
#endif
}