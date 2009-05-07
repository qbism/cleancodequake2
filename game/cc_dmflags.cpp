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
// cc_dmflags.h
// This is so that I don't have to keep doing dmflags->Integer(). Should be faster than bitwise ops every frame!
//

#include "cc_local.h"

dmFlagsConfig dmFlags;

dmFlagsConfig::dmFlagsConfig()
{
	dfNoHealth = dfNoItems = dfWeaponsStay = dfNoFallingDamage =
	dfInstantItems = dfSameLevel = dfSkinTeams = dfModelTeams = dfNoFriendlyFire =
	dfSpawnFarthest = dfForceRespawn = dfNoArmor = dfAllowExit =
	dfInfiniteAmmo = dfQuadDrop = dfFixedFov = dfQuadFireDrop = dfNoMines =
	dfNoStackDouble = dfNoNukes = dfNoSpheres = false;
};

void dmFlagsConfig::UpdateFlags (int dmFlags)
{
	dmFlagsConfig();
	if (dmFlags & DF_NO_HEALTH)
		dfNoHealth = true;
	if (dmFlags & DF_NO_ITEMS)
		dfNoItems = true;
	if (dmFlags & DF_WEAPONS_STAY)
		dfWeaponsStay = true;
	if (dmFlags & DF_NO_FALLING)
		dfNoFallingDamage = true;
	if (dmFlags & DF_INSTANT_ITEMS)
		dfInstantItems = true;
	if (dmFlags & DF_SAME_LEVEL)
		dfSameLevel = true;
	if (dmFlags & DF_SKINTEAMS)
		dfSkinTeams = true;
	if (dmFlags & DF_MODELTEAMS)
		dfModelTeams = true;
	if (dmFlags & DF_NO_FRIENDLY_FIRE)
		dfNoFriendlyFire = true;
	if (dmFlags & DF_SPAWN_FARTHEST)
		dfSpawnFarthest = true;
	if (dmFlags & DF_FORCE_RESPAWN)
		dfForceRespawn = true;
	if (dmFlags & DF_NO_ARMOR)
		dfNoArmor = true;
	if (dmFlags & DF_ALLOW_EXIT)
		dfAllowExit = true;
	if (dmFlags & DF_INFINITE_AMMO)
		dfInfiniteAmmo = true;
	if (dmFlags & DF_QUAD_DROP)
		dfQuadDrop = true;
	if (dmFlags & DF_FIXED_FOV)
		dfFixedFov = true;
	if (dmFlags & DF_QUADFIRE_DROP)
		dfQuadFireDrop = true;
	if (dmFlags & DF_NO_MINES)
		dfNoMines = true;
	if (dmFlags & DF_NO_STACK_DOUBLE)
		dfNoStackDouble = true;
	if (dmFlags & DF_NO_NUKES)
		dfNoNukes = true;
	if (dmFlags & DF_NO_SPHERES)
		dfNoSpheres = true;
};