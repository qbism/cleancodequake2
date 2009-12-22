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
// cc_bfg.cpp
// BFG10k
//

#include "cc_local.h"
#include "cc_weaponmain.h"
#include "m_player.h"

CBFG::CBFG() :
CWeapon(11, "models/weapons/v_bfg/tris.md2", 0, 8, 9, 31,
		32, 54, 55, 58, "weapons/bfg_hum.wav")
{
}

bool CBFG::CanFire (CPlayerEntity *ent)
{
	switch (ent->Client.PlayerState.GetGunFrame())
	{
	case 9:
	case 17:
		return true;
	}
	return false;
}

bool CBFG::CanStopFidgetting (CPlayerEntity *ent)
{
	switch (ent->Client.PlayerState.GetGunFrame())
	{
	case 39:
	case 45:
	case 50:
	case 55:
		return true;
	}
	return false;
}

void CBFG::Fire (CPlayerEntity *ent)
{
	switch (ent->Client.PlayerState.GetGunFrame())
	{
	case 9:
		MuzzleEffect (ent);
		break;
	case 17:
		FireBFG (ent);
		break;
	}
}

void CBFG::MuzzleEffect (CPlayerEntity *ent)
{
	// send muzzle flash
	Muzzle (ent, MZ_BFG);

	ent->Client.PlayerState.GetGunFrame()++;
}

void CBFG::FireBFG (CPlayerEntity *ent)
{
	vec3f	offset (8, 8, ent->ViewHeight-8), start, forward, right;
	const sint32		damage = (game.GameMode & GAME_DEATHMATCH) ?
					CalcQuadVal(200)
					:
					CalcQuadVal(500);

	// cells can go down during windup (from power armor hits), so
	// check again and abort firing if we don't have enough now
	if (ent->Client.Persistent.Inventory.Has(ent->Client.Persistent.Weapon->Item->Ammo) < 50)
	{
		ent->Client.PlayerState.GetGunFrame();
		return;
	}

	FireAnimation (ent);
	ent->Client.ViewAngle.ToVectors (&forward, &right, NULL);

	ent->Client.KickOrigin = forward * -2;

	// make a big pitch kick with an inverse fall
	ent->Client.ViewDamage.Set (-40, crand()*8);
	ent->Client.ViewDamageTime = level.Frame + DAMAGE_TIME;

	ent->P_ProjectSource (offset, forward, right, start);
	CBFGBolt::Spawn (ent, start, forward, damage, 400, 1000);

	AttackSound (ent);
	ent->Client.PlayerState.GetGunFrame()++;

	ent->PlayerNoiseAt (start, PNOISE_WEAPON);
	DepleteAmmo (ent, 50);
}

WEAPON_DEFS (CBFG);

void CBFG::CreateItem (CItemList *List)
{
	NItems::BFG = QNew (com_itemPool, 0) CWeaponItem
		("weapon_bfg", "models/weapons/g_bfg/tris.md2", EF_ROTATE,
		"misc/w_pkup.wav", "w_bfg", "BFG10k",
		ITEMFLAG_DROPPABLE|ITEMFLAG_WEAPON|ITEMFLAG_GRABBABLE|ITEMFLAG_STAY_COOP|ITEMFLAG_USABLE,
		"", &Weapon, NItems::Cells, 50, "#w_bfg.md2");
};