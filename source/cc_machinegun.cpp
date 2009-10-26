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
// cc_machinegun.cpp
// Machinegun
//

#include "cc_local.h"
#include "cc_weaponmain.h"
#include "m_player.h"

CMachinegun::CMachinegun() :
CWeapon("models/weapons/v_machn/tris.md2", 0, 3, 4, 5,
		6, 45, 46, 49)
{
}

bool CMachinegun::CanFire (CPlayerEntity *ent)
{
	switch (ent->Client.PlayerState.GetGunFrame())
	{
	case 4:
	case 5:
		return true;
	}
	return false;
}

bool CMachinegun::CanStopFidgetting (CPlayerEntity *ent)
{
	switch (ent->Client.PlayerState.GetGunFrame())
	{
	case 23:
	case 45:
		return true;
	}
	return false;
}

void CMachinegun::FireAnimation (CPlayerEntity *ent)
{
	ent->Client.anim_priority = ANIM_ATTACK;
	if (ent->Client.PlayerState.GetPMove()->pmFlags & PMF_DUCKED)
	{
		ent->State.GetFrame() = (FRAME_crattak1 - (int) (frand()+0.25));
		ent->Client.anim_end = FRAME_crattak9;
	}
	else
	{
		ent->State.GetFrame() = (FRAME_attack1 - (int) (frand()+0.25));
		ent->Client.anim_end = FRAME_attack8;
	}
}

void CMachinegun::Fire (CPlayerEntity *ent)
{
	if (!(ent->Client.Buttons & BUTTON_ATTACK))
	{
		ent->Client.machinegun_shots = 0;
		ent->Client.PlayerState.GetGunFrame()++;
		return;
	}

	if (ent->Client.PlayerState.GetGunFrame() == 5)
		ent->Client.PlayerState.GetGunFrame() = 4;
	else
		ent->Client.PlayerState.GetGunFrame() = 5;

	if (!AttemptToFire(ent))
	{
		ent->Client.PlayerState.GetGunFrame() = 6;
		OutOfAmmo(ent);
		NoAmmoWeaponChange (ent);
		return;
	}

	const int		damage = (isQuad) ? 32 : 8,
					kick = (isQuad) ? 8 : 2;

	for (int i=1 ; i<3 ; i++)
	{
		ent->Client.KickOrigin[i] = crand() * 0.35;
		ent->Client.KickAngles[i] = crand() * 0.7;
	}
	ent->Client.KickOrigin.X = crand() * 0.35;
	ent->Client.KickAngles.X = ent->Client.machinegun_shots * -1.5;

	// raise the gun as it is firing
	if (!(game.mode & GAME_DEATHMATCH))
	{
		ent->Client.machinegun_shots++;
		if (ent->Client.machinegun_shots > 9)
			ent->Client.machinegun_shots = 9;
	}

	// get start / end positions
	vec3f start, forward, right, offset (0, 8, ent->ViewHeight-8), 
		angles = ent->Client.ViewAngle + ent->Client.KickAngles;
	angles.ToVectors (&forward, &right, NULL);
	ent->P_ProjectSource (offset, forward, right, start);

	CBullet::Fire (ent, start, forward, damage, kick, DEFAULT_BULLET_HSPREAD, DEFAULT_BULLET_VSPREAD, MOD_MACHINEGUN);

	Muzzle (ent, MZ_MACHINEGUN);
	AttackSound (ent);

	ent->PlayerNoiseAt (start, PNOISE_WEAPON);

	if (!dmFlags.dfInfiniteAmmo)
		DepleteAmmo(ent, 1);

	FireAnimation (ent);
}

WEAPON_DEFS (CMachinegun);

void CMachinegun::CreateItem (CItemList *List)
{
	NItems::Machinegun = QNew (com_gamePool, 0) CWeaponItem
		("weapon_machinegun", "models/weapons/g_machn/tris.md2", EF_ROTATE, "misc/w_pkup.wav", "w_machinegun",
		"Machinegun", ITEMFLAG_DROPPABLE|ITEMFLAG_WEAPON|ITEMFLAG_GRABBABLE|ITEMFLAG_STAY_COOP|ITEMFLAG_USABLE,
		"", &Weapon, NItems::Bullets, 1, "#w_machinegun.md2");

	List->AddItemToList (NItems::Machinegun);
};
