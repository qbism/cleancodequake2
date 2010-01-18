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

CGrenadeLauncher::CGrenadeLauncher() :
CWeapon(5, 0, "models/weapons/v_launch/tris.md2", 0, 5, 6, 16,
		17, 59, 60, 64)
{
}

bool CGrenadeLauncher::CanFire (CPlayerEntity *Player)
{
	switch (Player->Client.PlayerState.GetGunFrame())
	{
	case 6:
		return true;
	}
	return false;
}

bool CGrenadeLauncher::CanStopFidgetting (CPlayerEntity *Player)
{
	switch (Player->Client.PlayerState.GetGunFrame())
	{
	case 34:
	case 51:
	case 59:
		return true;
	}
	return false;
}

void CGrenadeLauncher::Fire (CPlayerEntity *Player)
{
	vec3f	offset (8, 8, Player->ViewHeight-8), forward, right, start;
	const sint32	damage = CalcQuadVal(120);
	const float	radius = 160;

	FireAnimation (Player);

	Player->Client.ViewAngle.ToVectors (&forward, &right, NULL);
	Player->P_ProjectSource (offset, forward, right, start);

	Player->Client.KickOrigin = forward * -2;
	Player->Client.KickAngles.X = -1;

	CGrenade::Spawn (Player, start, forward, damage, 600, 25, radius);

	Muzzle (Player, MZ_GRENADE);
	AttackSound (Player);

	Player->PlayerNoiseAt (start, PNOISE_WEAPON);
	DepleteAmmo(Player, 1);

	Player->Client.PlayerState.GetGunFrame()++;
}

WEAPON_DEFS (CGrenadeLauncher);

void CGrenadeLauncher::CreateItem (CItemList *List)
{
	NItems::GrenadeLauncher = QNew (com_itemPool, 0) CWeaponItem
		("weapon_grenadelauncher", "models/weapons/g_launch/tris.md2", EF_ROTATE, "misc/w_pkup.wav", "w_glauncher", "Grenade Launcher",
		ITEMFLAG_DROPPABLE|ITEMFLAG_WEAPON|ITEMFLAG_GRABBABLE|ITEMFLAG_STAY_COOP|ITEMFLAG_USABLE, "", &Weapon,
		NItems::Grenades, 1, "#w_glauncher.md2");
};

