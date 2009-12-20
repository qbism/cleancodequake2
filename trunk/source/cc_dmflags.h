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

#if !defined(__CC_DMFLAGS_H__) || !INCLUDE_GUARDS
#define __CC_DMFLAGS_H__

class dmFlagsConfig
{
public:
	class dmFlagValue
	{
		bool					Enabled;
		const EDeathmatchFlags	Flag;

	public:
		dmFlagValue (EDeathmatchFlags Flag) :
		  Flag (Flag),
		  Enabled (false)
		  {
		  };

		dmFlagValue &operator= (dmFlagValue&) { return *this; }

		inline bool IsEnabled ()
		{
			return Enabled;
		};

		void Check (EDeathmatchFlags WantedFlags)
		{
			Enabled = !!(WantedFlags & Flag);
		};

		friend class dmFlagsConfig;
	};

	dmFlagValue				dfNoHealth;
	dmFlagValue				dfNoItems;
	dmFlagValue				dfWeaponsStay;
	dmFlagValue				dfNoFallingDamage;
	dmFlagValue				dfInstantItems;
	dmFlagValue				dfSameLevel;
	dmFlagValue				dfSkinTeams;
	dmFlagValue				dfModelTeams;
	dmFlagValue				dfNoFriendlyFire;
	dmFlagValue				dfSpawnFarthest;
	dmFlagValue				dfForceRespawn;
	dmFlagValue				dfNoArmor;
	dmFlagValue				dfAllowExit;
	dmFlagValue				dfInfiniteAmmo;
	dmFlagValue				dfQuadDrop;
	dmFlagValue				dfFixedFov;

	dmFlagValue				dfQuadFireDrop;
	dmFlagValue				dfNoMines;
	dmFlagValue				dfNoStackDouble;
	dmFlagValue				dfNoNukes;
	dmFlagValue				dfNoSpheres;

#if CLEANCTF_ENABLED
	dmFlagValue				dfCtfNoTech;
	dmFlagValue				dfCtfForceJoin;
	dmFlagValue				dfCtfArmorProtect;
#endif

	dmFlagValue				dfDmTechs;

	dmFlagsConfig();
	void UpdateFlags (EDeathmatchFlags wantedFlags);
};

extern dmFlagsConfig dmFlags;

#else
FILE_WARNING
#endif