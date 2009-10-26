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
// cc_chaingun.cpp
// Chaingun
//

#include "cc_local.h"
#include "cc_weaponmain.h"
#include "m_player.h"

CChaingun::CChaingun() :
CWeapon("models/weapons/v_chain/tris.md2", 0, 4, 5, 31,
		32, 61, 62, 64)
{
}

bool CChaingun::CanFire (CPlayerEntity *ent)
{
	if (ent->Client.PlayerState.GetGunFrame() >= 5 && ent->Client.PlayerState.GetGunFrame() <= 21)
		return true;
	return false;
}

bool CChaingun::CanStopFidgetting (CPlayerEntity *ent)
{
	switch (ent->Client.PlayerState.GetGunFrame())
	{
	case 38:
	case 43:
	case 51:
	case 61:
		return true;
	}
	return false;
}

void CChaingun::FireAnimation (CPlayerEntity *ent)
{
	ent->Client.anim_priority = ANIM_ATTACK;
	if (ent->Client.PlayerState.GetPMove()->pmFlags & PMF_DUCKED)
	{
		ent->State.GetFrame() = (FRAME_crattak1 - (ent->Client.PlayerState.GetGunFrame() & 1));
		ent->Client.anim_end = FRAME_crattak9;
	}
	else
	{
		ent->State.GetFrame() = (FRAME_attack1 - (ent->Client.PlayerState.GetGunFrame() & 1));
		ent->Client.anim_end = FRAME_attack8;
	}
}

void CChaingun::Fire (CPlayerEntity *ent)
{
	int			shots;
	vec3f		start, forward, right, up, offset;
	const int	damage = (game.mode & GAME_DEATHMATCH) ?
				(isQuad) ? 24 : 6
				:
				(isQuad) ? 32 : 8,
				kick = (isQuad) ? 8 : 2;

	if (ent->Client.PlayerState.GetGunFrame() == 5)
		ent->PlaySound (CHAN_AUTO, SoundIndex("weapons/chngnu1a.wav"));

	if ((ent->Client.PlayerState.GetGunFrame() == 14) && !(ent->Client.Buttons & BUTTON_ATTACK))
	{
		ent->Client.PlayerState.GetGunFrame() = 31;
		ent->Client.weapon_sound = 0;
		return;
	}
	else if ((ent->Client.PlayerState.GetGunFrame() == 21) && (ent->Client.Buttons & BUTTON_ATTACK)
		&& ent->Client.Persistent.Inventory.Has(ent->Client.Persistent.Weapon->WeaponItem->Ammo))
	{
		ent->Client.PlayerState.GetGunFrame() = 15;
	}
	else
		ent->Client.PlayerState.GetGunFrame()++;

	if (ent->Client.PlayerState.GetGunFrame() == 22)
	{
		ent->Client.weapon_sound = 0;
		ent->PlaySound (CHAN_AUTO, SoundIndex("weapons/chngnd1a.wav"));
	}
	else
		ent->Client.weapon_sound = SoundIndex("weapons/chngnl1a.wav");

	FireAnimation (ent);

	if (ent->Client.PlayerState.GetGunFrame() <= 9)
		shots = 1;
	else if (ent->Client.PlayerState.GetGunFrame() <= 14)
	{
		if (ent->Client.Buttons & BUTTON_ATTACK)
			shots = 2;
		else
			shots = 1;
	}
	else
		shots = 3;

	if (ent->Client.Persistent.Inventory.Has(ent->Client.Persistent.Weapon->WeaponItem->Ammo) < shots)
		shots = ent->Client.Persistent.Inventory.Has(ent->Client.Persistent.Weapon->WeaponItem->Ammo);

	if (!shots)
	{
		OutOfAmmo(ent);
		NoAmmoWeaponChange (ent);
		return;
	}

	for (int i=0 ; i<3 ; i++)
	{
		ent->Client.KickOrigin[i] = crand() * 0.35;
		ent->Client.KickAngles[i] = crand() * 0.7;
	}

	for (int i=0 ; i<shots ; i++)
	{
		// get start / end positions
		ent->Client.ViewAngle.ToVectors (&forward, &right, &up);
		offset.Set (0, 7 + crand()*4, crand()*4 + ent->ViewHeight-8);
		ent->P_ProjectSource (offset, forward, right, start);

		CBullet::Fire (ent, start, forward, damage, kick, DEFAULT_BULLET_HSPREAD, DEFAULT_BULLET_VSPREAD, MOD_CHAINGUN);
	}

	// send muzzle flash
	Muzzle (ent, MZ_CHAINGUN1 + shots - 1);
	AttackSound (ent);

	ent->PlayerNoiseAt (start, PNOISE_WEAPON);

	if (!dmFlags.dfInfiniteAmmo)
		DepleteAmmo (ent, shots);
}

WEAPON_DEFS (CChaingun);

void CChaingun::CreateItem (CItemList *List)
{
	NItems::Chaingun = QNew (com_gamePool, 0) CWeaponItem
		("weapon_chaingun", "models/weapons/g_chain/tris.md2", EF_ROTATE, "misc/w_pkup.wav",
		"w_chaingun", "Chaingun", ITEMFLAG_DROPPABLE|ITEMFLAG_WEAPON|ITEMFLAG_GRABBABLE|ITEMFLAG_STAY_COOP|ITEMFLAG_USABLE,
		"", &Weapon, NItems::Bullets, 1, "#w_chaingun.md2");

	List->AddItemToList (NItems::Chaingun);
};
