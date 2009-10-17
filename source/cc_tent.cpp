////////////////////////////////////////////////////////////////////////////////////////////////////
/// \file	game\cc_tent.cpp
///
/// \brief	Implements the temporary entity classes.
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
// cc_tent.cpp
// A huge class containing containers to spawn temp entities with a single function call.
//

#include "cc_local.h"

// A small class to automatically multicast and do the first two writebytes.
class TECast
{
	vec3f Origin;
	ECastFlags castType;

public:
	TECast	(vec3_t Origin,
		byte Enum,
		ECastFlags castFlags = CASTFLAG_PVS)
	{
		this->Origin.Set (Origin);
		castType = castFlags;

		WriteByte (SVC_TEMP_ENTITY);
		WriteByte (Enum);
	};
	TECast	(vec3f &Origin,
		byte Enum,
		ECastFlags castFlags = CASTFLAG_PVS)
	{
		this->Origin = Origin;
		castType = castFlags;

		WriteByte (SVC_TEMP_ENTITY);
		WriteByte (Enum);
	};

	~TECast ()
	{
		Cast (castType, Origin);
	};
};

// Automatically sets up the multicasting for regular casts
class MultiCast
{
	vec3f Origin;
	ECastFlags castType;

public:
	MultiCast	(vec3_t Origin,
				byte Enum,
				ECastFlags castFlags = CASTFLAG_PVS)
	{
		this->Origin.Set (Origin);
		castType = castFlags;

		WriteByte (Enum);
	};
	MultiCast	(vec3f &Origin,
				byte Enum,
				ECastFlags castFlags = CASTFLAG_PVS)
	{
		this->Origin = Origin;
		castType = castFlags;

		WriteByte (Enum);
	};


	~MultiCast ()
	{
		Cast (castType, Origin);
	};
};

void CTempEnt_Splashes::Gunshot (vec3_t Origin, vec3_t Plane)
{
	TECast cast(Origin, TE_GUNSHOT);
	WritePosition (Origin);
	WriteDirection (Plane ? Plane : vec3Origin);
}

void CTempEnt_Splashes::Shotgun (vec3_t Origin, vec3_t Plane)
{
	TECast cast(Origin, TE_SHOTGUN);
	WritePosition (Origin);
	WriteDirection (Plane ? Plane : vec3Origin);
}

void CTempEnt_Splashes::Blood (vec3_t Origin, vec3_t Plane, EBloodType BloodType)
{
	TECast cast(Origin, BloodType);
	WritePosition (Origin);
	WriteDirection (Plane ? Plane : vec3Origin);
}

void CTempEnt_Splashes::Blaster (vec3_t Origin, vec3_t Plane, EBlasterType BlasterType)
{
	TECast cast(Origin, BlasterType);
	WritePosition (Origin);

	// ...
	// ID, you impress me.
	if (BlasterType == BL_BLUE_HYPERBLASTER)
		WritePosition (Plane ? Plane : vec3Origin);
	else
		WriteDirection (Plane ? Plane : vec3Origin);
}

void CTempEnt_Splashes::Sparks (vec3_t Origin, vec3_t Plane, ESparkType SparkType, ESplashType color, byte amount)
{
	TECast cast(Origin, SparkType);

	if (SparkType != ST_LASER_SPARKS && SparkType != ST_WELDING_SPARKS && SparkType != ST_TUNNEL_SPARKS)
	{
		WritePosition (Origin);
		WriteDirection (Plane ? Plane : vec3Origin);
	}
	else
	{
		WriteByte (amount);
		WritePosition (Origin);
		WriteDirection (Plane ? Plane : vec3Origin);
		WriteByte (color);
	}
}

void CTempEnt_Splashes::Splash (vec3_t Origin, vec3_t Plane, ESplashType color, byte amount)
{
	TECast cast(Origin, TE_SPLASH);
	WriteByte (amount);
	WritePosition (Origin);
	WriteDirection (Plane ? Plane : vec3Origin);
	WriteByte (color);
}

void CTempEnt_Splashes::ShieldSparks (vec3_t Origin, vec3_t Plane, bool Screen)
{
	TECast cast(Origin, Screen ? TE_SCREEN_SPARKS : TE_SHIELD_SPARKS);
	WritePosition (Origin);
	WriteDirection (Plane ? Plane : vec3Origin);
}

void CTempEnt_Splashes::Steam (vec3_t Origin, vec3_t Normal, byte count, ESplashType color, short magnitude, short id, long endTime)
{
	TECast cast(Origin, TE_STEAM);
	WriteShort (id);
	WriteByte (count);
	WritePosition (Origin);
	WriteDirection (Normal);
	WriteByte (color);
	WriteShort (magnitude);

	// FIXME: Test
	if (id != -1)
		WriteLong (endTime);
}

void CTempEnt_Splashes::HeatSteam (vec3_t Origin, vec3_t Normal)
{
	TECast cast(Origin, TE_HEATBEAM_STEAM);
	WritePosition (Origin);
	WriteDirection (Normal ? Normal : vec3Origin);
}

void CTempEnt_Splashes::ChainfistSmoke (vec3_t Origin)
{
	TECast cast(Origin, TE_CHAINFIST_SMOKE);
	WritePosition (Origin);
}

static void BaseTrail (vec3_t Start, vec3_t End, short Ent = -1)
{
	if (Ent != -1)
		WriteShort (Ent);
	WritePosition (Start);
	WritePosition (End);
}

void CTempEnt_Trails::RailTrail (vec3_t Start, vec3_t End)
{
	TECast cast(Start, TE_RAILTRAIL);
	BaseTrail(Start, End);
}

void CTempEnt_Trails::BubbleTrail (vec3_t Start, vec3_t End)
{
	TECast cast(Start, TE_BUBBLETRAIL);
	BaseTrail(Start, End);
}

void CTempEnt_Trails::FleshCable (vec3_t Start, vec3_t End, short Ent)
{
	TECast cast(Start, TE_PARASITE_ATTACK);
	BaseTrail(Start, End, Ent);
}

void CTempEnt_Trails::BFGLaser (vec3_t Start, vec3_t End)
{
	TECast cast(Start, TE_BFG_LASER);
	BaseTrail(Start, End);
}

void CTempEnt_Trails::GrappleCable (vec3_t Start, vec3_t End, short Ent, vec3_t Offset)
{
	TECast cast(Start, TE_GRAPPLE_CABLE);
	BaseTrail(Start, End, Ent);
	WritePosition (Offset);
}

void CTempEnt_Trails::Lightning (vec3_t Start, vec3_t End, short SrcEnt, short DestEnt)
{
	TECast cast(Start, TE_LIGHTNING);
	WriteShort (SrcEnt);
	WriteShort (DestEnt);
	BaseTrail(Start, End);
}

void CTempEnt_Trails::DebugTrail (vec3_t Start, vec3_t End)
{
	TECast cast(Start, TE_DEBUGTRAIL);
	BaseTrail(Start, End);
}

void CTempEnt_Trails::ForceWall (vec3_t Start, vec3_t End, byte color)
{
	TECast cast(Start, TE_FORCEWALL);
	BaseTrail(Start, End);
	WriteByte (color);
}

void CTempEnt_Trails::HeatBeam (vec3_t Start, vec3_t End, short Ent, bool Monster)
{
	TECast cast(Start, Monster ? TE_MONSTER_HEATBEAM : TE_HEATBEAM);
	BaseTrail(Start, End, Ent);
}

void CTempEnt_Explosions::RocketExplosion (vec3_t Start, edict_t *ent, bool Water, bool Particles)
{
	// Water and NoParticles fight over a spot.. water will win, in the end
	TECast cast (ent->state.origin, Water ? TE_ROCKET_EXPLOSION_WATER : (Particles ? TE_ROCKET_EXPLOSION : TE_EXPLOSION1_NP), CASTFLAG_PVS);
	WritePosition (Start);
}

void CTempEnt_Explosions::GrenadeExplosion (vec3_t Start, edict_t *ent, bool Water)
{
	TECast cast (ent->state.origin, Water ? TE_GRENADE_EXPLOSION_WATER : TE_GRENADE_EXPLOSION, CASTFLAG_PVS);
	WritePosition (Start);
}

void CTempEnt_Explosions::BFGExplosion (vec3_t Origin, bool Big)
{
	TECast cast (Origin, Big ? TE_BFG_BIGEXPLOSION : TE_BFG_EXPLOSION);
	WritePosition (Origin);
}

void CTempEnt_Explosions::PlasmaExplosion (vec3_t Origin)
{
	TECast cast (Origin, TE_PLASMA_EXPLOSION);
	WritePosition (Origin);
}

void CTempEnt_Explosions::TrackerExplosion (vec3_t Origin)
{
	TECast cast (Origin, TE_TRACKER_EXPLOSION);
	WritePosition (Origin);
}

void CTempEnt_Explosions::NukeBlast (vec3_t Origin)
{
	TECast cast (Origin, TE_NUKEBLAST);
	WritePosition (Origin);
}

void CTempEnt::Flashlight (vec3_t Origin, short Ent)
{
	TECast cast(Origin, TE_FLASHLIGHT);
	WritePosition(Origin);
	WriteShort (Ent);
}

void CTempEnt::BossTeleport (vec3_t Origin)
{
	TECast cast(Origin, TE_BOSSTPORT);
	WritePosition(Origin);
}

void CTempEnt::TeleportEffect (vec3_t Origin)
{
	TECast cast(Origin, TE_TELEPORT_EFFECT);
	WritePosition(Origin);
}

void CTempEnt::WidowBeamOut (vec3_t Origin, short id)
{
	TECast cast(Origin, TE_WIDOWBEAMOUT);
	WriteShort (id);
	WritePosition(Origin);
}

void CTempEnt::WidowSplash (vec3_t Origin)
{
	TECast cast(Origin, TE_WIDOWSPLASH);
	WritePosition(Origin);
}

void CTempEnt::MuzzleFlash (vec3_t Origin, short Ent, short id)
{
	MultiCast cast (Origin, SVC_MUZZLEFLASH);
	WriteShort (Ent);
	WriteByte (id);
}

void CTempEnt::MonsterFlash (vec3_t Origin, short Ent, short id)
{
	MultiCast cast (Origin, SVC_MUZZLEFLASH2);
	WriteShort (Ent);
	WriteByte (id);
}


//
// vec3f overloads
//


void CTempEnt_Splashes::Gunshot (vec3f &Origin, vec3f &Plane)
{
	TECast cast(Origin, TE_GUNSHOT);
	WritePosition (Origin);
	WriteDirection (Plane ? Plane : vec3fOrigin);
}

void CTempEnt_Splashes::Shotgun (vec3f &Origin, vec3f &Plane)
{
	TECast cast(Origin, TE_SHOTGUN);
	WritePosition (Origin);
	WriteDirection (Plane ? Plane : vec3fOrigin);
}

void CTempEnt_Splashes::Blood (vec3f &Origin, vec3f &Plane, EBloodType BloodType)
{
	TECast cast(Origin, BloodType);
	WritePosition (Origin);
	WriteDirection (Plane ? Plane : vec3fOrigin);
}

void CTempEnt_Splashes::Blaster (vec3f &Origin, vec3f &Plane, EBlasterType BlasterType)
{
	TECast cast(Origin, BlasterType);
	WritePosition (Origin);

	// ...
	// ID, you impress me.
	if (BlasterType == BL_BLUE_HYPERBLASTER)
		WritePosition (Plane ? Plane : vec3fOrigin);
	else
		WriteDirection (Plane ? Plane : vec3fOrigin);
}

void CTempEnt_Splashes::Sparks (vec3f &Origin, vec3f &Plane, ESparkType SparkType, ESplashType color, byte amount)
{
	TECast cast(Origin, SparkType);

	if (SparkType != ST_LASER_SPARKS && SparkType != ST_WELDING_SPARKS && SparkType != ST_TUNNEL_SPARKS)
	{
		WritePosition (Origin);
		WriteDirection (Plane ? Plane : vec3fOrigin);
	}
	else
	{
		WriteByte (amount);
		WritePosition (Origin);
		WriteDirection (Plane ? Plane : vec3fOrigin);
		WriteByte (color);
	}
}

void CTempEnt_Splashes::Splash (vec3f &Origin, vec3f &Plane, ESplashType color, byte amount)
{
	TECast cast(Origin, TE_SPLASH);
	WriteByte (amount);
	WritePosition (Origin);
	WriteDirection (Plane ? Plane : vec3fOrigin);
	WriteByte (color);
}

void CTempEnt_Splashes::ShieldSparks (vec3f &Origin, vec3f &Plane, bool Screen)
{
	TECast cast(Origin, Screen ? TE_SCREEN_SPARKS : TE_SHIELD_SPARKS);
	WritePosition (Origin);
	WriteDirection (Plane ? Plane : vec3fOrigin);
}

void CTempEnt_Splashes::Steam (vec3f &Origin, vec3f &Normal, byte count, ESplashType color, short magnitude, short id, long endTime)
{
	TECast cast(Origin, TE_STEAM);
	WriteShort (id);
	WriteByte (count);
	WritePosition (Origin);
	WriteDirection (Normal);
	WriteByte (color);
	WriteShort (magnitude);

	// FIXME: Test
	if (id != -1)
		WriteLong (endTime);
}

void CTempEnt_Splashes::HeatSteam (vec3f &Origin, vec3f &Normal)
{
	TECast cast(Origin, TE_HEATBEAM_STEAM);
	WritePosition (Origin);
	WriteDirection (Normal ? Normal : vec3fOrigin);
}

void CTempEnt_Splashes::ChainfistSmoke (vec3f &Origin)
{
	TECast cast(Origin, TE_CHAINFIST_SMOKE);
	WritePosition (Origin);
}

static void BaseTrail (vec3f &Start, vec3f &End, short Ent = -1)
{
	if (Ent != -1)
		WriteShort (Ent);
	WritePosition (Start);
	WritePosition (End);
}

void CTempEnt_Trails::RailTrail (vec3f &Start, vec3f &End)
{
	TECast cast(Start, TE_RAILTRAIL);
	BaseTrail(Start, End);
}

void CTempEnt_Trails::BubbleTrail (vec3f &Start, vec3f &End)
{
	TECast cast(Start, TE_BUBBLETRAIL);
	BaseTrail(Start, End);
}

void CTempEnt_Trails::FleshCable (vec3f &Start, vec3f &End, short Ent)
{
	TECast cast(Start, TE_PARASITE_ATTACK);
	BaseTrail(Start, End, Ent);
}

void CTempEnt_Trails::BFGLaser (vec3f &Start, vec3f &End)
{
	TECast cast(Start, TE_BFG_LASER);
	BaseTrail(Start, End);
}

void CTempEnt_Trails::GrappleCable (vec3f &Start, vec3f &End, short Ent, vec3f &Offset)
{
	TECast cast(Start, TE_GRAPPLE_CABLE);
	BaseTrail(Start, End, Ent);
	WritePosition (Offset);
}

void CTempEnt_Trails::Lightning (vec3f &Start, vec3f &End, short SrcEnt, short DestEnt)
{
	TECast cast(Start, TE_LIGHTNING);
	WriteShort (SrcEnt);
	WriteShort (DestEnt);
	BaseTrail(Start, End);
}

void CTempEnt_Trails::DebugTrail (vec3f &Start, vec3f &End)
{
	TECast cast(Start, TE_DEBUGTRAIL);
	BaseTrail(Start, End);
}

void CTempEnt_Trails::ForceWall (vec3f &Start, vec3f &End, byte color)
{
	TECast cast(Start, TE_FORCEWALL);
	BaseTrail(Start, End);
	WriteByte (color);
}

void CTempEnt_Trails::HeatBeam (vec3f &Start, vec3f &End, short Ent, bool Monster)
{
	TECast cast(Start, Monster ? TE_MONSTER_HEATBEAM : TE_HEATBEAM);
	BaseTrail(Start, End, Ent);
}

void CTempEnt_Explosions::RocketExplosion (vec3f &Start, CBaseEntity *ent, bool Water, bool Particles)
{
	// Water and NoParticles fight over a spot.. water will win, in the end
	TECast cast (ent->State.GetOrigin(), Water ? TE_ROCKET_EXPLOSION_WATER : (Particles ? TE_ROCKET_EXPLOSION : TE_EXPLOSION1_NP), CASTFLAG_PVS);
	WritePosition (Start);
}

void CTempEnt_Explosions::GrenadeExplosion (vec3f &Start, CBaseEntity *ent, bool Water)
{
	TECast cast (ent->State.GetOrigin(), Water ? TE_GRENADE_EXPLOSION_WATER : TE_GRENADE_EXPLOSION, CASTFLAG_PVS);
	WritePosition (Start);
}

void CTempEnt_Explosions::BFGExplosion (vec3f &Origin, bool Big)
{
	TECast cast (Origin, Big ? TE_BFG_BIGEXPLOSION : TE_BFG_EXPLOSION);
	WritePosition (Origin);
}

void CTempEnt_Explosions::PlasmaExplosion (vec3f &Origin)
{
	TECast cast (Origin, TE_PLASMA_EXPLOSION);
	WritePosition (Origin);
}

void CTempEnt_Explosions::TrackerExplosion (vec3f &Origin)
{
	TECast cast (Origin, TE_TRACKER_EXPLOSION);
	WritePosition (Origin);
}

void CTempEnt_Explosions::NukeBlast (vec3f &Origin)
{
	TECast cast (Origin, TE_NUKEBLAST);
	WritePosition (Origin);
}

void CTempEnt::Flashlight (vec3f &Origin, short Ent)
{
	TECast cast(Origin, TE_FLASHLIGHT);
	WritePosition(Origin);
	WriteShort (Ent);
}

void CTempEnt::BossTeleport (vec3f &Origin)
{
	TECast cast(Origin, TE_BOSSTPORT);
	WritePosition(Origin);
}

void CTempEnt::TeleportEffect (vec3f &Origin)
{
	TECast cast(Origin, TE_TELEPORT_EFFECT);
	WritePosition(Origin);
}

void CTempEnt::WidowBeamOut (vec3f &Origin, short id)
{
	TECast cast(Origin, TE_WIDOWBEAMOUT);
	WriteShort (id);
	WritePosition(Origin);
}

void CTempEnt::WidowSplash (vec3f &Origin)
{
	TECast cast(Origin, TE_WIDOWSPLASH);
	WritePosition(Origin);
}

void CTempEnt::MuzzleFlash (vec3f &Origin, short Ent, short id)
{
	MultiCast cast (Origin, SVC_MUZZLEFLASH);
	WriteShort (Ent);
	WriteByte (id);
}

void CTempEnt::MonsterFlash (vec3f &Origin, short Ent, short id)
{
	MultiCast cast (Origin, SVC_MUZZLEFLASH2);
	WriteShort (Ent);
	WriteByte (id);
}