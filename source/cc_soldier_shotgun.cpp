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
// cc_soldier_shotgun.cpp
// Kablow chickchick.
//

#include "cc_local.h"
#include "cc_soldier_base.h"
#include "cc_soldier_shotgun.h"

CSoldierShotgun::CSoldierShotgun (uint32 ID) :
CSoldierBase (ID)
{
	Scale = MODEL_SCALE;
	MonsterName = "Shotgun Soldier";
}

extern CAnim SoldierMoveAttack1;
extern CAnim SoldierMoveAttack2;
extern CAnim SoldierMoveAttack6;

void CSoldierShotgun::Attack ()
{
#if MONSTER_USE_ROGUE_AI
	DoneDodge ();

	float r = frand();
	if ((!(AIFlags & (AI_BLOCKED|AI_STAND_GROUND))) &&
		(Range(Entity, Entity->Enemy) == RANGE_MID) && 
		(r < (skill->Integer()*0.05))) // Very low chance for shotty soldier
		CurrentMove = &SoldierMoveAttack6;
	else
#endif
	{
		if (frand() < 0.5)
			CurrentMove = &SoldierMoveAttack1;
		else
			CurrentMove = &SoldierMoveAttack2;
	}
}

static sint32 ShotgunFlash [] = {MZ2_SOLDIER_SHOTGUN_1, MZ2_SOLDIER_SHOTGUN_2, MZ2_SOLDIER_SHOTGUN_3, MZ2_SOLDIER_SHOTGUN_4, MZ2_SOLDIER_SHOTGUN_5, MZ2_SOLDIER_SHOTGUN_6, MZ2_SOLDIER_SHOTGUN_7, MZ2_SOLDIER_SHOTGUN_8};
void CSoldierShotgun::FireGun (sint32 FlashNumber)
{
	vec3f	start, forward, right, aim;
	sint32		flashIndex = ShotgunFlash[FlashNumber];

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

	MonsterFireShotgun (start, aim, 2, 1, DEFAULT_SHOTGUN_HSPREAD, DEFAULT_SHOTGUN_VSPREAD, DEFAULT_SHOTGUN_COUNT, flashIndex);
}

void CSoldierShotgun::SpawnSoldier ()
{
	Sounds[SOUND_PAIN] = SoundIndex ("soldier/solpain1.wav");
	Sounds[SOUND_DEATH] = SoundIndex ("soldier/soldeth1.wav");

	Entity->State.GetSkinNum() = 2;
	SoldierAI = AI_SHOTGUN;
	Entity->Health = 30;
	Entity->GibHealth = -30;
}

LINK_MONSTER_CLASSNAME_TO_CLASS ("monster_soldier", CSoldierShotgun);
