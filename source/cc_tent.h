////////////////////////////////////////////////////////////////////////////////////////////////////
/// \file	game\cc_tent.h
///
/// \brief	Declares the temporary entity classes.
////////////////////////////////////////////////////////////////////////////////////////////////////
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
// cc_tent.h
// A huge class containing containers to spawn temp entities with a single function call.
//

#if !defined(CC_GUARD_TENT_H) || !INCLUDE_GUARDS
#define CC_GUARD_TENT_H

#include "cc_colors.h"

namespace NTempEnts
{
	namespace NSplashes
	{
		CC_ENUM (uint8, ESplashType)
		{
			SPT_UNKNOWN,
			SPT_SPARKS,
			SPT_WATER,
			SPT_MUD,
			SPT_SLIME,
			SPT_LAVA,
			SPT_BLOOD
		};

		CC_ENUM (uint8, EBloodType)
		{
			BT_BLOOD = TE_BLOOD,
			BT_MORE_BLOOD = TE_MOREBLOOD,
			BT_GREEN_BLOOD = TE_GREENBLOOD
		};

		CC_ENUM (uint8, EBlasterType)
		{
			BL_BLASTER = TE_BLASTER,
			BL_BLUE_HYPERBLASTER = TE_BLUEHYPERBLASTER,
			BL_FLECHETTE = TE_FLECHETTE,
			BL_GREEN_BLASTER = TE_BLASTER2
		};

		CC_ENUM (uint8, ESparkType)
		{
			ST_SPARKS = TE_SPARKS,
			ST_BULLET_SPARKS = TE_BULLET_SPARKS,
			ST_HEATBEAM_SPARKS = TE_HEATBEAM_SPARKS,
			ST_ELECTRIC_SPARKS = TE_ELECTRIC_SPARKS,

			// Ones that have amount/color
			ST_LASER_SPARKS = TE_LASER_SPARKS,
			ST_WELDING_SPARKS = TE_WELDING_SPARKS,
			ST_TUNNEL_SPARKS = TE_TUNNEL_SPARKS
		};

		void Gunshot	(vec3f &Origin,
						vec3f &Normal);

		void Shotgun	(vec3f &Origin,
						vec3f &Normal);

		void Blood	(vec3f &Origin,
					vec3f &Normal,
					EBloodType BloodType = BT_BLOOD);

		void Blaster	(vec3f &Origin,
						vec3f &Normal,
						EBlasterType BlasterType = BL_BLASTER);

		void Sparks	(vec3f &Origin,
					vec3f &Normal,
					ESparkType SparkType = ST_SPARKS,
					ESplashType color = SPT_UNKNOWN,
					uint8 amount = 8);

		void Splash	(vec3f &Origin,
					vec3f &Normal,
					ESplashType color = SPT_UNKNOWN,
					uint8 amount = 8);

		void ShieldSparks	(vec3f &Origin,
							vec3f &Normal,
							bool Screen = false);

		void Steam	(vec3f &Origin,
					vec3f &Normal,
					uint8 count = 8,
					ESplashType color = SPT_UNKNOWN,
					sint16 magnitude = 12,
					sint16 id = -1,
					long endTime = 0);

		void HeatSteam	(vec3f &Origin,
						vec3f &Normal);

		void ChainfistSmoke	(vec3f &Origin);
	};

	namespace NTrails
	{
		void RailTrail	(vec3f &Start,
						vec3f &End);

		void HeatBeam	(vec3f & Start,
						vec3f &End,
						sint16 Ent,
						bool Monster = false);

		void ForceWall	(vec3f &Start,
						vec3f &End,
						uint8 color = NSColor::Lime);

		void DebugTrail	(vec3f &Start,
						vec3f &End);

		void Lightning	(vec3f &Start,
						vec3f &End,
						sint16 SrcEnt,
						sint16 DestEnt);

		void GrappleCable	(vec3f &Start,
							vec3f &End,
							sint16 Ent,
							vec3f &Offset = vec3fOrigin);

		void BFGLaser	(vec3f &Start,
						vec3f &End);

		void FleshCable		(vec3f &Start,
							vec3f &End,
							sint16 Ent);
		void BubbleTrail	(vec3f &Start,
							vec3f &End);
	};

	namespace NExplosions
	{
		void RocketExplosion	(vec3f &Origin, CBaseEntity *Entity,
								bool Water = false,
								bool Particles = true);

		void GrenadeExplosion	(vec3f &Origin,
								CBaseEntity *Entity,
								bool Water = false);

		void BFGExplosion		(vec3f &Origin,
								bool Big = false);

		void PlasmaExplosion	(vec3f &Origin);

		void TrackerExplosion	(vec3f &Origin);

		void NukeBlast			(vec3f &Origin);
	};

	void Flashlight		(vec3f &Origin,
						sint16 Ent);

	void BossTeleport	(vec3f &Origin);

	void TeleportEffect	(vec3f &Origin);

	void WidowBeamOut	(vec3f &Origin,
						sint16 id = -1);

	void WidowSplash	(vec3f &Origin);

	void MuzzleFlash	(vec3f &Origin,
						sint16 Ent,
						sint16 id);

	void MonsterFlash	(vec3f &Origin,
						sint16 Ent,
						sint16 id);
};

#else
FILE_WARNING
#endif
