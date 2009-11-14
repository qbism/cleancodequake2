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

CSuperShotgun::CSuperShotgun() :
CWeapon("models/weapons/v_shotg2/tris.md2", 0, 6, 7, 17,
		18, 57, 58, 61)
{
}

bool CSuperShotgun::CanFire (CPlayerEntity *ent)
{
	switch (ent->Client.PlayerState.GetGunFrame())
	{
	case 7:
		return true;
	}
	return false;
}

bool CSuperShotgun::CanStopFidgetting (CPlayerEntity *ent)
{
	switch (ent->Client.PlayerState.GetGunFrame())
	{
	case 29:
	case 42:
	case 57:
		return true;
	}
	return false;
}

void CSuperShotgun::Fire (CPlayerEntity *ent)
{
	vec3f		start, forward, right, offset (0, 8, ent->ViewHeight-8);
	const sint32	damage = (isQuad) ? 24 : 6,
				kick = (isQuad) ? 48 : 12;

	ent->Client.ViewAngle.ToVectors (&forward, &right, NULL);

	ent->Client.KickOrigin = forward * -2;
	ent->Client.KickAngles.X = -2;

	ent->P_ProjectSource (offset, forward, right, start);

	vec3f v = ent->Client.ViewAngle;
	v.Y -= 5;
	v.ToVectors (&forward, NULL, NULL);
	CShotgunPellets::Fire (ent, start, forward, damage, kick, DEFAULT_SHOTGUN_HSPREAD, DEFAULT_SHOTGUN_VSPREAD, DEFAULT_SSHOTGUN_COUNT/2, MOD_SSHOTGUN);

	v.Y += 10;
	v.ToVectors (&forward, NULL, NULL);
	CShotgunPellets::Fire (ent, start, forward, damage, kick, DEFAULT_SHOTGUN_HSPREAD, DEFAULT_SHOTGUN_VSPREAD, DEFAULT_SSHOTGUN_COUNT/2, MOD_SSHOTGUN);

	// send muzzle flash
	Muzzle (ent, MZ_SSHOTGUN);
	FireAnimation (ent);

	AttackSound (ent);

	ent->Client.PlayerState.GetGunFrame()++;
	ent->PlayerNoiseAt (start, PNOISE_WEAPON);

	if (!dmFlags.dfInfiniteAmmo)
		DepleteAmmo(ent, 2);
}

WEAPON_DEFS (CSuperShotgun);

void CSuperShotgun::CreateItem (CItemList *List)
{
	NItems::SuperShotgun = QNew (com_gamePool, 0) CWeaponItem
		("weapon_supershotgun", "models/weapons/g_shotg2/tris.md2", EF_ROTATE, "misc/w_pkup.wav", "w_sshotgun",
		"Super Shotgun", ITEMFLAG_DROPPABLE|ITEMFLAG_WEAPON|ITEMFLAG_GRABBABLE|ITEMFLAG_STAY_COOP|ITEMFLAG_USABLE, "",
		&Weapon, NItems::Shells, 2, "#w_sshotgun.md2");
};
