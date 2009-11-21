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
// cc_soldier_machinegun.cpp
// T-t-t-t-t-t-t.
//

#include "cc_local.h"
#include "cc_soldier_base.h"
#include "cc_soldier_machinegun.h"

CSoldierMachinegun::CSoldierMachinegun (uint32 ID) :
CSoldierBase (ID)
{
	Scale = MODEL_SCALE;
	MonsterName = "Machinegun Soldier";
}

extern CAnim SoldierMoveAttack4;

void CSoldierMachinegun::Attack ()
{
#ifdef MONSTER_USE_ROGUE_AI
	DoneDodge ();
#endif

	CurrentMove = &SoldierMoveAttack4;
}

static sint32 MachinegunFlash [] = {MZ2_SOLDIER_MACHINEGUN_1, MZ2_SOLDIER_MACHINEGUN_2, MZ2_SOLDIER_MACHINEGUN_3, MZ2_SOLDIER_MACHINEGUN_4, MZ2_SOLDIER_MACHINEGUN_5, MZ2_SOLDIER_MACHINEGUN_6, MZ2_SOLDIER_MACHINEGUN_7, MZ2_SOLDIER_MACHINEGUN_8};
void CSoldierMachinegun::FireGun (sint32 FlashNumber)
{
	vec3f	start, forward, right, aim;
	sint32		flashIndex = MachinegunFlash[FlashNumber];

	Entity->State.GetAngles().ToVectors (&forward, &right, NULL);
	G_ProjectSource (Entity->State.GetOrigin(), dumb_and_hacky_monster_MuzzFlashOffset[flashIndex], forward, right, start);

	switch (FlashNumber)
	{
	case 5:
	case 6:
		aim = forward;
		break;
	default:
		{
			CBaseEntity *Enemy = Entity->Enemy;
			vec3f end;

			end = Enemy->State.GetOrigin() + vec3f(0, 0, Enemy->ViewHeight);
			aim = (end - start);

			vec3f dir;
			dir = aim.ToAngles ();

			vec3f up;
			dir.ToVectors (&forward, &right, &up);

			end = start.MultiplyAngles (8192, forward);
			end = end.MultiplyAngles (crand() * 1000, right);
			end = end.MultiplyAngles (crand() * 500, up);

			aim = end - start;
			aim.Normalize ();
		}
		break;
	};

	if (!(AIFlags & AI_HOLD_FRAME))
		Wait = level.Frame + (3 + irandom(8));

	MonsterFireBullet (start, aim, 2, 4, DEFAULT_BULLET_HSPREAD, DEFAULT_BULLET_VSPREAD, flashIndex);

	if (level.Frame >= Wait)
		AIFlags &= ~AI_HOLD_FRAME;
	else
		AIFlags |= AI_HOLD_FRAME;
}

void CSoldierMachinegun::SpawnSoldier ()
{
	Sounds[SOUND_PAIN] = SoundIndex ("soldier/solpain3.wav");
	Sounds[SOUND_DEATH] = SoundIndex ("soldier/soldeth3.wav");

	Entity->State.GetSkinNum() = 4;
	Entity->Health = 40;
	Entity->GibHealth = -30;
}

LINK_MONSTER_CLASSNAME_TO_CLASS ("monster_soldier_ss", CSoldierMachinegun);
