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
// cc_hyperblaster.cpp
// Hyperblaster
//

#include "cc_local.h"
#include "cc_weaponmain.h"
#include "m_player.h"

CHyperBlaster::CHyperBlaster() :
CWeapon("models/weapons/v_hyperb/tris.md2", 0, 5, 6, 20,
		21, 49, 50, 53)
{
}

bool CHyperBlaster::CanFire (CPlayerEntity *ent)
{
	if (ent->Client.PlayerState.GetGunFrame() >= 6 && ent->Client.PlayerState.GetGunFrame() <= 11)
		return true;
	return false;
}

bool CHyperBlaster::CanStopFidgetting (CPlayerEntity *ent)
{
	return false;
}

vec3f hyperblasterOffsetTable[] =
{
	vec3f( -3.46f, 0, 2.0f	),
	vec3f( -3.46f, 0, -2.0f	),
	vec3f( 0, 0, -4.0f		),
	vec3f( 3.46f, 0, -2.0f	),
	vec3f( 3.46f, 0, 2.0f	),
	vec3f( 0, 0, 4.0f		)
};

void CHyperBlaster::Fire (CPlayerEntity *ent)
{
	ent->Client.WeaponSound = SoundIndex("weapons/hyprbl1a.wav");

	if (!(ent->Client.Buttons & BUTTON_ATTACK))
		ent->Client.PlayerState.GetGunFrame()++;
	else
	{
		if (!ent->Client.Persistent.Inventory.Has(ent->Client.Persistent.Weapon->WeaponItem->Ammo) )
		{
			OutOfAmmo(ent);
			NoAmmoWeaponChange (ent);
		}
		else
		{
			const sint32	effect = ((ent->Client.PlayerState.GetGunFrame() == 6) || (ent->Client.PlayerState.GetGunFrame() == 9)) ? EF_HYPERBLASTER : 0,
						damage = (game.mode & GAME_DEATHMATCH) ?
				(isQuad) ? 60 : 15
				:
				(isQuad) ? 80 : 20;

			vec3f	forward, right, start;

			ent->Client.ViewAngle.ToVectors (&forward, &right, NULL);
			// I replaced this part with a table because they are constant.
			ent->P_ProjectSource (vec3f(24, 8, ent->ViewHeight-8) + hyperblasterOffsetTable[ent->Client.PlayerState.GetGunFrame() - 6], forward, right, start);

			ent->Client.KickOrigin = forward * -2;
			ent->Client.KickAngles.X = -1;

			CBlasterProjectile::Spawn (ent, start, forward, damage, 1000, effect, true);

			// send muzzle flash
			Muzzle (ent, MZ_HYPERBLASTER);
			AttackSound (ent);

			ent->PlayerNoiseAt (start, PNOISE_WEAPON);

			if (!dmFlags.dfInfiniteAmmo)
				DepleteAmmo (ent, 1);

			FireAnimation (ent);
		}

		ent->Client.PlayerState.GetGunFrame()++;
		if (ent->Client.PlayerState.GetGunFrame() == 12 && ent->Client.Persistent.Inventory.Has(ent->Client.Persistent.Weapon->WeaponItem->Ammo))
			ent->Client.PlayerState.GetGunFrame() = 6;
	}

	if (ent->Client.PlayerState.GetGunFrame() == 12)
	{
		ent->PlaySound (CHAN_AUTO, SoundIndex("weapons/hyprbd1a.wav"));
		ent->Client.WeaponSound = 0;
	}
}

WEAPON_DEFS (CHyperBlaster);

void CHyperBlaster::CreateItem (CItemList *List)
{
	NItems::HyperBlaster = QNew (com_gamePool, 0) CWeaponItem
		("weapon_hyperblaster", "models/weapons/g_hyperb/tris.md2", EF_ROTATE, "misc/w_pkup.wav",
		"w_hyperblaster", "HyperBlaster", ITEMFLAG_DROPPABLE|ITEMFLAG_WEAPON|ITEMFLAG_GRABBABLE|ITEMFLAG_STAY_COOP|ITEMFLAG_USABLE,
		"", &Weapon, NItems::Cells, 1, "#w_hyperblaster.md2");

	List->AddItemToList (NItems::HyperBlaster);
};
