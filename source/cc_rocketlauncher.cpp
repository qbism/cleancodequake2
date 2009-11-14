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
// cc_grenadelauncher.cpp
// Grenade Launcher
//

#include "cc_local.h"
#include "cc_weaponmain.h"
#include "m_player.h"

CRocketLauncher::CRocketLauncher() :
CWeapon("models/weapons/v_rocket/tris.md2", 0, 4, 5, 12,
		13, 50, 51, 54)
{
}

bool CRocketLauncher::CanFire (CPlayerEntity *ent)
{
	switch (ent->Client.PlayerState.GetGunFrame())
	{
	case 5:
		return true;
	}
	return false;
}

bool CRocketLauncher::CanStopFidgetting (CPlayerEntity *ent)
{
	switch (ent->Client.PlayerState.GetGunFrame())
	{
	case 25:
	case 33:
	case 42:
	case 50:
		return true;
	}
	return false;
}

void CRocketLauncher::Fire (CPlayerEntity *ent)
{
	vec3f	offset (8, 8, ent->ViewHeight-8), start, forward, right;
	const sint32	damage = (isQuad) ? (400 + (sint32)(frand() * 80.0)) : (100 + (sint32)(frand() * 20.0)),
				radius_damage = (isQuad) ? 480 : 120;
	const float	damage_radius = 120;

	ent->Client.ViewAngle.ToVectors (&forward, &right, NULL);

	ent->Client.KickOrigin = forward * -2;
	ent->Client.KickAngles.X = -1;

	ent->P_ProjectSource (offset, forward, right, start);
	CRocket::Spawn (ent, start, forward, damage, 650, damage_radius, radius_damage);

	// send muzzle flash
	Muzzle (ent, MZ_ROCKET);
	AttackSound (ent);

	ent->PlayerNoiseAt (start, PNOISE_WEAPON);
	FireAnimation (ent);

	if (!dmFlags.dfInfiniteAmmo)
		DepleteAmmo(ent, 1);

	ent->Client.PlayerState.GetGunFrame()++;
}

WEAPON_DEFS (CRocketLauncher);

void CRocketLauncher::CreateItem (CItemList *List)
{
	NItems::RocketLauncher = QNew (com_gamePool, 0) CWeaponItem
		("weapon_rocketlauncher", "models/weapons/g_rocket/tris.md2", EF_ROTATE, "misc/w_pkup.wav", "w_rlauncher", "Rocket Launcher",
		ITEMFLAG_DROPPABLE|ITEMFLAG_WEAPON|ITEMFLAG_GRABBABLE|ITEMFLAG_STAY_COOP|ITEMFLAG_USABLE, "",
		&Weapon, NItems::Rockets, 1, "#w_rlauncher.md2");
};
