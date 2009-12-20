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
// cc_handgrenade.cpp
// Handgrenades
//

#include "cc_local.h"
#include "cc_weaponmain.h"
#include "m_player.h"

CHandGrenade::CHandGrenade() :
CWeapon(7, "models/weapons/v_handgr/tris.md2", 0, 0, 0, 16,
		17, 48, 0, 0)
{
}

bool CHandGrenade::CanStopFidgetting (CPlayerEntity *ent)
{
	switch (ent->Client.PlayerState.GetGunFrame())
	{
	case 29:
	case 34:
	case 39:
	case 48:
		return true;
	}
	return false;
}

#define GRENADE_TIMER		35
#define GRENADE_MINSPEED	400
#define GRENADE_MAXSPEED	800

void CHandGrenade::Hold (CPlayerEntity *ent)
{
	if (!ent->Client.Grenade.Time)
	{
		ent->Client.Grenade.Time = level.Frame + GRENADE_TIMER + 2;
		ent->Client.WeaponSound = SoundIndex("weapons/hgrenc1b.wav");
	}

	// they waited too long, detonate it in their hand
	if (!ent->Client.Grenade.BlewUp && (level.Frame >= ent->Client.Grenade.Time))
	{
		ent->Client.WeaponSound = 0;
		FireGrenade (ent, true);
		ent->Client.Grenade.BlewUp = true;

		ent->Client.PlayerState.GetGunFrame() = 15;
		return;
	}

	if (ent->Client.Buttons & BUTTON_ATTACK)
		return;

	ent->Client.PlayerState.GetGunFrame()++;
}

void CHandGrenade::FireGrenade (CPlayerEntity *ent, bool inHand)
{
	vec3f	offset (8, 8, ent->ViewHeight-8), forward, right, start;
	const sint32		damage = (isQuad) ? 500 : 125;
	const float		radius = 165;

	ent->Client.Grenade.Thrown = true;

	ent->Client.ViewAngle.ToVectors (&forward, &right, NULL);
	ent->P_ProjectSource (offset, forward, right, start);

	float timer = (float)(ent->Client.Grenade.Time - level.Frame) / 10;
	const sint32 speed = (ent->Client.Persistent.Weapon) ? 
		(GRENADE_MINSPEED + ((GRENADE_TIMER/10) - timer) * ((GRENADE_MAXSPEED - GRENADE_MINSPEED) / (GRENADE_TIMER/10)))
		: 25; // If we're dead, don't toss it 5 yards.
	CGrenade::Spawn (ent, start, forward, damage, speed, timer, radius, true, inHand);

	ent->Client.Grenade.Time = level.Frame + (((
#if CLEANCTF_ENABLED
	(game.mode & GAME_CTF) || 
#endif
	dmFlags.dfDmTechs.IsEnabled()) && ent->ApplyHaste()) ? 5 : 10);
	DepleteAmmo(ent, 1);

	if (ent->Health <= 0 || ent->DeadFlag || ent->State.GetModelIndex() != 255) // VWep animations screw up corpses
		return;

	AttackSound (ent);

	if (ent->Client.PlayerState.GetPMove()->pmFlags & PMF_DUCKED)
	{
		ent->Client.Anim.Priority = ANIM_ATTACK;
		ent->State.GetFrame() = FRAME_crattak1 - 1;
		ent->Client.Anim.EndFrame = FRAME_crattak3;
	}
	else
	{
		ent->Client.Anim.Priority = ANIM_REVERSE;
		ent->State.GetFrame() = FRAME_wave08;
		ent->Client.Anim.EndFrame = FRAME_wave01;
	}
	ent->Client.PlayerState.GetGunFrame()++;
}

void CHandGrenade::Wait (CPlayerEntity *ent)
{
	ent->Client.Grenade.BlewUp = false;
	if (level.Frame < ent->Client.Grenade.Time)
		return;

	if (!ent->DeadFlag)
		ent->Client.Grenade.Thrown = false;
	ent->Client.PlayerState.GetGunFrame()++;
}

void CHandGrenade::Fire (CPlayerEntity *ent)
{
	switch (ent->Client.PlayerState.GetGunFrame())
	{
	case 11:
		Hold (ent);
		break;
	case 12:
		ent->Client.WeaponSound = 0;
		FireGrenade(ent, false);
		break;
	case 15:
		Wait(ent);
		break;
	}
}

bool CHandGrenade::CanFire (CPlayerEntity *ent)
{
	switch (ent->Client.PlayerState.GetGunFrame())
	{
	case 5:
		ent->PlaySound (CHAN_WEAPON, SoundIndex("weapons/hgrena1b.wav"));
		return false;
	case 11:
	case 12:
	case 15:
		return true;
	}
	return false;
}

void CHandGrenade::WeaponGeneric (CPlayerEntity *ent)
{
	// Idea from Brazen source
	sint32 newFrame = -1, newState = -1;

	switch (ent->Client.WeaponState)
	{
	case WS_ACTIVATING:
		newFrame = IdleStart;
		newState = WS_IDLE;
		break;
	case WS_IDLE:
		if (ent->Client.NewWeapon && ent->Client.NewWeapon != this)
		{
			// We want to go away!
			newState = WS_DEACTIVATING;
			newFrame = DeactStart;
		}
		else if ((ent->Client.Buttons|ent->Client.LatchedButtons) & BUTTON_ATTACK)
		{
			ent->Client.LatchedButtons &= ~BUTTON_ATTACK;

			// We want to attack!
			// First call, check AttemptToFire
			if (AttemptToFire(ent))
			{
				// Got here, we can fire!
				ent->Client.PlayerState.GetGunFrame() = FireStart;
				ent->Client.WeaponState = WS_FIRING;
				ent->Client.Grenade.Time = 0;

				// We need to check against us right away for first-frame firing
				WeaponGeneric(ent);
				return;
			}
			else
			{
				OutOfAmmo(ent);
				NoAmmoWeaponChange (ent);
			}
		}

		// Either we are still idle or a failed fire.
		if (newState == -1)
		{
			if (ent->Client.PlayerState.GetGunFrame() == IdleEnd)
				newFrame = IdleStart;
			else
			{
				if (CanStopFidgetting(ent) && (randomMT()&15))
					newFrame = ent->Client.PlayerState.GetGunFrame();
			}
		}
		break;
	case WS_FIRING:
		// Check if this is a firing frame.
		if (CanFire(ent))
		{
			Fire(ent);

			// Now, this call above CAN change the underlying frame and state.
			// We need this block to make sure we are still doing what we are supposed to.
			newState = ent->Client.WeaponState;
			newFrame = ent->Client.PlayerState.GetGunFrame();
		}

		// Only do this if we haven't been explicitely set a newFrame
		// because we might want to keep firing beyond this point
		if (newFrame == -1 && ent->Client.PlayerState.GetGunFrame() == FireEnd)
		{
			if (!ent->Client.Persistent.Inventory.Has(Item))
			{
				NoAmmoWeaponChange (ent);
				ent->Client.Grenade.Time = 0;
				ent->Client.Grenade.Thrown = false;
			}
			else
			{
				ent->Client.Grenade.Time = 0;
				ent->Client.Grenade.Thrown = false;
				newFrame = IdleStart;
				newState = WS_IDLE;
			}
		}
		break;
	case WS_DEACTIVATING:
		// Change weapon
		ChangeWeapon (ent);
		return;
		break;
	}

	if (newFrame != -1)
		ent->Client.PlayerState.GetGunFrame() = newFrame;
	if (newState != -1)
		ent->Client.WeaponState = newState;

	if (newFrame == -1 && newState == -1)
		ent->Client.PlayerState.GetGunFrame()++;
}

WEAPON_DEFS (CHandGrenade);

void CHandGrenade::CreateItem (CItemList *List)
{
};
