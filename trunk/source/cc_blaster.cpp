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
// cc_blaster.cpp
// Blaster!
//

#include "cc_local.h"
#include "cc_weaponmain.h"
#include "m_player.h"

CBlaster::CBlaster() :
CWeapon(1, "models/weapons/v_blast/tris.md2", 0, 4, 5, 8,
		9, 52, 53, 55)
{
}

bool CBlaster::CanFire (CPlayerEntity *ent)
{
	switch (ent->Client.PlayerState.GetGunFrame())
	{
	case 5:
		return true;
	}
	return false;
}

bool CBlaster::CanStopFidgetting (CPlayerEntity *ent)
{
	switch (ent->Client.PlayerState.GetGunFrame())
	{
	case 19:
	case 32:
		return true;
	}
	return false;
}

bool CBlaster::AttemptToFire (CPlayerEntity *ent)
{
	return true;
}

void CBlaster::Fire (CPlayerEntity *ent)
{
	const sint32 Damage = deathmatch->Integer() ? 
			(isQuad) ? 60 : 15
			:
			(isQuad) ? 40 : 10;
	vec3f	Forward, Start, Right, Offset (24, 8, ent->ViewHeight - 8);

	ent->Client.ViewAngle.ToVectors (&Forward, &Right, NULL);
	ent->P_ProjectSource (Offset, Forward, Right, Start);

	ent->Client.KickOrigin = Forward * -2;
	ent->Client.KickAngles.X = -1;

	CBlasterProjectile::Spawn (ent, Start, Forward, Damage, 1000, EF_BLASTER, false);

	// send muzzle flash
	Muzzle (ent, MZ_BLASTER);
	AttackSound (ent);

	ent->PlayerNoiseAt(Start, PNOISE_WEAPON);
	FireAnimation(ent);

	ent->Client.PlayerState.GetGunFrame()++;
}

WEAPON_DEFS (CBlaster);

void CBlaster::CreateItem (CItemList *List)
{
	NItems::Blaster = QNew (com_itemPool, 0)
		CWeaponItem(NULL, NULL, 0, NULL, "w_blaster", "Blaster",
		ITEMFLAG_WEAPON|ITEMFLAG_USABLE, "", &Weapon, NULL,
		0, "#w_blaster.md2");
};
