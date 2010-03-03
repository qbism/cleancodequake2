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
#include "cc_tent.h"

const CTempEntFlags CTempEntFlags::DefaultTempEntFlags (CAST_MULTI, CASTFLAG_PVS);

void CForEachPlayerMulticastCallback::Query (bool MustBeInUse)
{
	for (uint8 i = 1; i <= Game.MaxClients; i++)
	{
		CPlayerEntity *Player = entity_cast<CPlayerEntity>(Game.Entities[i].Entity);

		if (MustBeInUse && (!Player->GetInUse() || Player->Client.Persistent.State != SVCS_SPAWNED))
			continue;

		Index = i;
		if (Callback (Player))
			TempEnt->SendTo (Player);
	}
};

void CTempEnt::Send ()
{
	if (Flags.Type == CAST_UNI)
		SendTo (Player);
	else
	{
		SendHeader ();
		SendData ();
		
		Cast (Flags.Type, Flags.Flags, Origin, NULL, true);
	}
};

void CTempEnt::SendTo (CPlayerEntity *Player)
{
	SendHeader ();
	SendData ();

	Cast (Flags.Type, Flags.Flags, (ToPlayerOrigin && Player) ? Player->State.GetOrigin() : Origin, Player, true);
};

void CRocketExplosion::SendHeader ()
{
	WriteByte (SVC_TEMP_ENTITY);
	WriteByte (Water ? TE_ROCKET_EXPLOSION_WATER : (Particles ? TE_ROCKET_EXPLOSION : TE_EXPLOSION1_NP));
};

void CRocketExplosion::SendData ()
{
	WritePosition (ExplosionOrigin);
};

// A small class to automatically multicast and do the first two writebytes.
class TECast
{
	vec3f Origin;
	ECastFlags castType;

public:
	TECast	(vec3f &Origin,
		uint8 Enum,
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
	MultiCast	(vec3f &Origin,
				uint8 Enum,
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

void NTempEnts::NSplashes::Gunshot (vec3f &Origin, vec3f &Plane)
{
	TECast cast(Origin, TE_GUNSHOT);
	WritePosition (Origin);
	WriteDirection (Plane ? Plane : vec3fOrigin);
}

void NTempEnts::NSplashes::Shotgun (vec3f &Origin, vec3f &Plane)
{
	TECast cast(Origin, TE_SHOTGUN);
	WritePosition (Origin);
	WriteDirection (Plane ? Plane : vec3fOrigin);
}

void NTempEnts::NSplashes::Blood (vec3f &Origin, vec3f &Plane, EBloodType BloodType)
{
	TECast cast(Origin, BloodType);
	WritePosition (Origin);
	WriteDirection (Plane ? Plane : vec3fOrigin);
}

void NTempEnts::NSplashes::Blaster (vec3f &Origin, vec3f &Plane, EBlasterType BlasterType)
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

void NTempEnts::NSplashes::Sparks (vec3f &Origin, vec3f &Plane, ESparkType SparkType, ESplashType color, uint8 amount)
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

void NTempEnts::NSplashes::Splash (vec3f &Origin, vec3f &Plane, ESplashType color, uint8 amount)
{
	TECast cast(Origin, TE_SPLASH);
	WriteByte (amount);
	WritePosition (Origin);
	WriteDirection (Plane ? Plane : vec3fOrigin);
	WriteByte (color);
}

void NTempEnts::NSplashes::ShieldSparks (vec3f &Origin, vec3f &Plane, bool Screen)
{
	TECast cast(Origin, Screen ? TE_SCREEN_SPARKS : TE_SHIELD_SPARKS);
	WritePosition (Origin);
	WriteDirection (Plane ? Plane : vec3fOrigin);
}

void NTempEnts::NSplashes::Steam (vec3f &Origin, vec3f &Normal, uint8 count, ESplashType color, sint16 magnitude, sint16 id, long endTime)
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

void NTempEnts::NSplashes::HeatSteam (vec3f &Origin, vec3f &Normal)
{
	TECast cast(Origin, TE_HEATBEAM_STEAM);
	WritePosition (Origin);
	WriteDirection (Normal ? Normal : vec3fOrigin);
}

void NTempEnts::NSplashes::ChainfistSmoke (vec3f &Origin)
{
	TECast cast(Origin, TE_CHAINFIST_SMOKE);
	WritePosition (Origin);
}

static void BaseTrail (vec3f &Start, vec3f &End, sint16 Ent = -1)
{
	if (Ent != -1)
		WriteShort (Ent);
	WritePosition (Start);
	WritePosition (End);
}

void NTempEnts::NTrails::RailTrail (vec3f &Start, vec3f &End)
{
	TECast cast(Start, TE_RAILTRAIL);
	BaseTrail(Start, End);
}

void NTempEnts::NTrails::BubbleTrail (vec3f &Start, vec3f &End)
{
	TECast cast(Start, TE_BUBBLETRAIL);
	BaseTrail(Start, End);
}

void NTempEnts::NTrails::FleshCable (vec3f &Start, vec3f &End, sint16 Ent)
{
	TECast cast(Start, TE_PARASITE_ATTACK);
	BaseTrail(Start, End, Ent);
}

void NTempEnts::NTrails::BFGLaser (vec3f &Start, vec3f &End)
{
	TECast cast(Start, TE_BFG_LASER);
	BaseTrail(Start, End);
}

void NTempEnts::NTrails::GrappleCable (vec3f &Start, vec3f &End, sint16 Ent, vec3f &Offset)
{
	TECast cast(Start, TE_GRAPPLE_CABLE);
	BaseTrail(Start, End, Ent);
	WritePosition (Offset);
}

void NTempEnts::NTrails::Lightning (vec3f &Start, vec3f &End, sint16 SrcEnt, sint16 DestEnt)
{
	TECast cast(Start, TE_LIGHTNING);
	WriteShort (SrcEnt);
	WriteShort (DestEnt);
	BaseTrail(Start, End);
}

void NTempEnts::NTrails::DebugTrail (vec3f &Start, vec3f &End)
{
	TECast cast(Start, TE_DEBUGTRAIL);
	BaseTrail(Start, End);
}

void NTempEnts::NTrails::ForceWall (vec3f &Start, vec3f &End, uint8 color)
{
	TECast cast(Start, TE_FORCEWALL);
	BaseTrail(Start, End);
	WriteByte (color);
}

void NTempEnts::NTrails::HeatBeam (vec3f &Start, vec3f &End, sint16 Ent, bool Monster)
{
	TECast cast(Start, Monster ? TE_MONSTER_HEATBEAM : TE_HEATBEAM);
	BaseTrail(Start, End, Ent);
}

void NTempEnts::NExplosions::GrenadeExplosion (vec3f &Start, CBaseEntity *Entity, bool Water)
{
	TECast cast (Entity->State.GetOrigin(), Water ? TE_GRENADE_EXPLOSION_WATER : TE_GRENADE_EXPLOSION);
	WritePosition (Start);
}

void NTempEnts::NExplosions::BFGExplosion (vec3f &Origin, bool Big)
{
	TECast cast (Origin, Big ? TE_BFG_BIGEXPLOSION : TE_BFG_EXPLOSION);
	WritePosition (Origin);
}

void NTempEnts::NExplosions::PlasmaExplosion (vec3f &Origin)
{
	TECast cast (Origin, TE_PLASMA_EXPLOSION);
	WritePosition (Origin);
}

void NTempEnts::NExplosions::TrackerExplosion (vec3f &Origin)
{
	TECast cast (Origin, TE_TRACKER_EXPLOSION);
	WritePosition (Origin);
}

void NTempEnts::NExplosions::NukeBlast (vec3f &Origin)
{
	TECast cast (Origin, TE_NUKEBLAST);
	WritePosition (Origin);
}

void NTempEnts::Flashlight (vec3f &Origin, sint16 Ent)
{
	TECast cast(Origin, TE_FLASHLIGHT);
	WritePosition(Origin);
	WriteShort (Ent);
}

void NTempEnts::BossTeleport (vec3f &Origin)
{
	TECast cast(Origin, TE_BOSSTPORT);
	WritePosition(Origin);
}

void NTempEnts::TeleportEffect (vec3f &Origin)
{
	TECast cast(Origin, TE_TELEPORT_EFFECT);
	WritePosition(Origin);
}

void NTempEnts::WidowBeamOut (vec3f &Origin, sint16 id)
{
	TECast cast(Origin, TE_WIDOWBEAMOUT);
	WriteShort (id);
	WritePosition(Origin);
}

void NTempEnts::WidowSplash (vec3f &Origin)
{
	TECast cast(Origin, TE_WIDOWSPLASH);
	WritePosition(Origin);
}

void NTempEnts::MuzzleFlash (vec3f &Origin, sint16 Ent, sint16 id)
{
	MultiCast cast (Origin, SVC_MUZZLEFLASH);
	WriteShort (Ent);
	WriteByte (id);
}

void NTempEnts::MonsterFlash (vec3f &Origin, sint16 Ent, sint16 id)
{
	MultiCast cast (Origin, SVC_MUZZLEFLASH2);
	WriteShort (Ent);
	WriteByte (id);
}
