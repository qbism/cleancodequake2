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
// cc_shotgun.cpp
// Shotgun
//

#include "cc_local.h"
#include "cc_weaponmain.h"
#include "m_player.h"

CShotgun::CShotgun() :
CWeapon(2, "models/weapons/v_shotg/tris.md2", 0, 7, 8, 18,
		19, 36, 37, 39)
{
}

bool CShotgun::CanFire (CPlayerEntity *ent)
{
	switch (ent->Client.PlayerState.GetGunFrame())
	{
	case 8:
		return true;
	}
	return false;
}

bool CShotgun::CanStopFidgetting (CPlayerEntity *ent)
{
	switch (ent->Client.PlayerState.GetGunFrame())
	{
	case 22:
	case 28:
	case 34:
		return true;
	}
	return false;
}

void CShotgun::Fire (CPlayerEntity *ent)
{
	vec3f		start, forward, right, offset (0, 8,  ent->ViewHeight-8);
	const sint32	damage = (isQuad) ? 12 : 3,
				kick = (isQuad) ? 32 : 8;

	ent->Client.ViewAngle.ToVectors (&forward, &right, NULL);

	ent->Client.KickOrigin = forward * -2;
	ent->Client.KickAngles.X = -2;

	ent->P_ProjectSource (offset, forward, right, start);

	CShotgunPellets::Fire (ent, start, forward, damage, kick, DEFAULT_SHOTGUN_HSPREAD, DEFAULT_SHOTGUN_VSPREAD, DEFAULT_SHOTGUN_COUNT, MOD_SHOTGUN);

	// send muzzle flash
	Muzzle (ent, MZ_SHOTGUN);
	AttackSound (ent);

	ent->Client.PlayerState.GetGunFrame()++;
	ent->PlayerNoiseAt (start, PNOISE_WEAPON);
	FireAnimation (ent);

	if (!dmFlags.dfInfiniteAmmo)
		DepleteAmmo(ent, 1);
}

WEAPON_DEFS (CShotgun);

void CShotgun::CreateItem (CItemList *List)
{
	NItems::Shotgun = QNew (com_itemPool, 0) CWeaponItem
		("weapon_shotgun", "models/weapons/g_shotg/tris.md2", EF_ROTATE, "misc/w_pkup.wav",
		"w_shotgun", "Shotgun", ITEMFLAG_DROPPABLE|ITEMFLAG_WEAPON|ITEMFLAG_GRABBABLE|ITEMFLAG_STAY_COOP|ITEMFLAG_USABLE,
		"", &Weapon, NItems::Shells, 1, "#w_shotgun.md2");
};
