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
// cc_infoentities.cpp
// Info_ and some Target_ entities
//

#include "cc_local.h"
#include "cc_infoentities.h"
#include "cc_weaponmain.h"
#include "cc_brushmodels.h"

/*QUAKED misc_teleporter_dest (1 0 0) (-32 -32 -24) (32 32 -16)
Point teleporters at these.
*/
CSpotBase::CSpotBase () :
	CBaseEntity (),
	CMapEntity ()
	{
	};

CSpotBase::CSpotBase(sint32 Index) :
	CBaseEntity(Index),
	CMapEntity(Index)
	{
	};

void CSpotBase::Spawn ()
{
	State.GetModelIndex() = ModelIndex("models/objects/dmspot/tris.md2");
	State.GetSkinNum() = 0;
	GetSolid() = SOLID_BBOX;
	GetMins().Set (-32, -32, -24);
	GetMaxs().Set (32, 32, -16);
	Link ();
};

class CTeleporterDest : public CSpotBase
{
public:
	CTeleporterDest () :
		CBaseEntity (),
		CSpotBase ()
		{
		};

	CTeleporterDest (sint32 Index) :
		CBaseEntity (Index),
		CSpotBase (Index)
		{
		};

	IMPLEMENT_SAVE_HEADER(CTeleporterDest)

	virtual void Spawn ()
	{
		CSpotBase::Spawn ();
	};
};

LINK_CLASSNAME_TO_CLASS ("misc_teleporter_dest", CTeleporterDest);

class CTeleporterTrigger : public CMapEntity, public CTouchableEntity, public CBrushModel
{
public:
	CBaseEntity		*Dest;
	char			*Target;

	CTeleporterTrigger() :
	  CBaseEntity (),
	  CMapEntity(),
	  CBrushModel (),
	  Dest (NULL),
	  Target (NULL)
	  {
	  };

	CTeleporterTrigger(sint32 Index) :
	  CBaseEntity (Index),
	  CMapEntity(),
	  CBrushModel (Index),
	  Dest (NULL),
	  Target (NULL)
	  {
	  };

	IMPLEMENT_SAVE_HEADER(CTeleporterTrigger)

	void Touch (CBaseEntity *Other, plane_t *plane, cmBspSurface_t *surf)
	{
		if (!Dest)
			return;

		CPlayerEntity	*Player = NULL;
		if (Other->EntityFlags & ENT_PLAYER)
			Player = entity_cast<CPlayerEntity>(Other);

	#if CLEANCTF_ENABLED
		//ZOID
		if (Player)
			CGrapple::PlayerResetGrapple(Player);
		//ZOID
	#endif

		// unlink to make sure it can't possibly interfere with KillBox
		Other->Unlink ();

		Other->State.GetOrigin() = (Dest->State.GetOrigin() + vec3f(0,0,10));
		Other->State.GetOldOrigin() = Dest->State.GetOrigin();

		// clear the velocity and hold them in place briefly
		if (Other->EntityFlags & ENT_PHYSICS)
			entity_cast<CPhysicsEntity>(Other)->Velocity.Clear ();
		if (Player)
		{
			Player->Client.PlayerState.GetPMove()->pmTime = 160>>3;		// hold time
			Player->Client.PlayerState.GetPMove()->pmFlags |= PMF_TIME_TELEPORT;
		}

		// draw the teleport splash at source and on the player
		Other->State.GetEvent() = (Player) ? EV_PLAYER_TELEPORT : EV_OTHER_TELEPORT;

		// set angles
		if (Player)
		{
			for (sint32 i = 0; i < 3; i++)
				Player->Client.PlayerState.GetPMove()->deltaAngles[i] = ANGLE2SHORT(Dest->State.GetAngles()[i] - Player->Client.Respawn.CmdAngles[i]);
		}

		Other->State.GetAngles().Clear ();
		if (Player)
		{
			Player->Client.PlayerState.GetViewAngles().Clear ();
			Player->Client.ViewAngle.Clear ();
		}

		// kill anything at the destination
		Other->KillBox ();

		Other->Link ();
	};

	void Think ()
	{
		Dest = CC_Find<CMapEntity, ENT_MAP, EntityMemberOffset(CMapEntity,TargetName)> (NULL, Target);
	
		if (!Dest)
			MapPrint (MAPPRINT_WARNING, this, State.GetOrigin(), "Couldn't find destination target \"%s\"\n", Target);
	};

	bool ParseField (const char *Key, const char *Value)
	{
		return (CMapEntity::ParseField (Key, Value) || CBrushModel::ParseField (Key, Value));
	};

	void SaveFields (CFile &File)
	{
		File.Write<sint32> ((Dest) ? Dest->State.GetNumber() : -1);

		CMapEntity::SaveFields (File);
		CBrushModel::SaveFields (File);
		CTouchableEntity::SaveFields (File);
	}

	void LoadFields (CFile &File);

	bool Run ()
	{
		return CMapEntity::Run ();
	};

	virtual void Spawn ()
	{
		NextThink = Level.Frame + 1;
	};
};

IMPLEMENT_SAVE_STRUCTURE (CTeleporterTrigger,CTeleporterTrigger)

class CTeleporter : public CSpotBase
{
public:
	char			*Target;

	ENTITYFIELD_DEFS
	ENTITYFIELDS_SAVABLE(CTeleporter)

	CTeleporter () :
		CBaseEntity (),
		CSpotBase (),
		Target (NULL)
		{
		};

	CTeleporter (sint32 Index) :
		CBaseEntity (Index),
		CSpotBase (Index),
		Target (NULL)
		{
		};

	virtual void Spawn ()
	{
		if (!Target)
		{
			MapPrint (MAPPRINT_ERROR, this, State.GetOrigin(), "No target\n");
			Free ();
			return;
		}

		State.GetModelIndex() = ModelIndex("models/objects/dmspot/tris.md2");
		State.GetSkinNum() = 1;
		State.GetEffects() = EF_TELEPORTER;
		State.GetSound() = SoundIndex ("world/amb10.wav");
		GetSolid() = SOLID_BBOX;

		GetMins().Set (-32, -32, -24);
		GetMaxs().Set (32, 32, -16);
		Link ();

		CTeleporterTrigger *trig = QNewEntityOf CTeleporterTrigger;
		trig->Touchable = true;
		trig->GetSolid() = SOLID_TRIGGER;
		trig->Target = Target;
		trig->SetOwner (this);
		trig->State.GetOrigin() = State.GetOrigin();
		trig->GetMins().Set (-8, -8, 8);
		trig->GetMaxs().Set (8, 8, 24);
		trig->NextThink = Level.Frame + 1;
		trig->Link ();
	};
};

ENTITYFIELDS_BEGIN(CTeleporter)
{
	CEntityField ("target", EntityMemberOffset(CTeleporter,Target), FT_LEVEL_STRING | FT_SAVABLE),
};
ENTITYFIELDS_END(CTeleporter)

bool			CTeleporter::ParseField (const char *Key, const char *Value)
{
	if (CheckFields<CTeleporter> (this, Key, Value))
		return true;

	// Couldn't find it here
	return (CMapEntity::ParseField (Key, Value));
};

void		CTeleporter::SaveFields (CFile &File)
{
	SaveEntityFields <CTeleporter> (this, File);
}

void		CTeleporter::LoadFields (CFile &File)
{
	LoadEntityFields <CTeleporter> (this, File);
}

LINK_CLASSNAME_TO_CLASS ("misc_teleporter", CTeleporter);

void CTeleporterTrigger::LoadFields (CFile &File)
{
	sint32 Index = File.Read<sint32> ();
	Dest = (Index != -1) ? Game.Entities[Index].Entity : NULL;

	Target = entity_cast<CTeleporter>(GetOwner())->Target;

	CMapEntity::LoadFields (File);
	CBrushModel::LoadFields (File);
	CTouchableEntity::LoadFields (File);
}

/*QUAKED trigger_teleport (0.5 0.5 0.5) ?
Players touching this will be teleported
*/
class CNoiseMaker : public CBaseEntity
{
public:
	CNoiseMaker () :
		CBaseEntity ()
		{
		};

	CNoiseMaker (int Index) :
		CBaseEntity (Index)
		{
		};

	IMPLEMENT_SAVE_HEADER(CNoiseMaker)
};
class CTriggerTeleportDest : public CTeleporterTrigger
{
public:
	CTriggerTeleportDest () :
	  CBaseEntity(),
	  CTeleporterTrigger ()
	  {
	  };

	CTriggerTeleportDest (sint32 Index) :
	  CBaseEntity(Index),
	  CTeleporterTrigger (Index)
	  {
	  };

	IMPLEMENT_SAVE_HEADER(CTriggerTeleportDest)

	void Spawn ()
	{
		if (!Target)
		{
			MapPrint (MAPPRINT_WARNING, this, State.GetOrigin(), "No target\n");
			Free ();
			return;
		}

		GetSvFlags() |= SVF_NOCLIENT;
		GetSolid() = SOLID_TRIGGER;
		Touchable = true;
		SetBrushModel ();
		Link ();

		// noise maker and splash effect dude
		CNoiseMaker *s = QNewEntityOf CNoiseMaker;
		s->State.GetOrigin() = (GetMins() + ((GetMaxs() - GetMins()) / 2));
		s->State.GetSound() = SoundIndex ("world/hum1.wav");
		s->Link ();
	};
};

IMPLEMENT_SAVE_SOURCE(CNoiseMaker)

LINK_CLASSNAME_TO_CLASS ("trigger_teleport", CTriggerTeleportDest);

/*QUAKED info_teleport_destination (0.5 0.5 0.5) (-16 -16 -24) (16 16 32)
Point trigger_teleports at these.
*/
class CInfoTeleportDest : public CSpotBase
{
public:
	CInfoTeleportDest () :
		CBaseEntity (),
		CSpotBase ()
		{
		};

	CInfoTeleportDest (sint32 Index) :
		CBaseEntity (Index),
		CSpotBase (Index)
		{
		};

	IMPLEMENT_SAVE_HEADER(CInfoTeleportDest)

	void Spawn ()
	{
		State.GetOrigin().Z += 16;
	};
};

LINK_CLASSNAME_TO_CLASS ("info_teleport_destination", CInfoTeleportDest);

// Undefine this if you need to use DM spots in SP, etc
#define FREE_UNUSED_SPOTS

/*QUAKED info_player_deathmatch (1 0 1) (-16 -16 -24) (16 16 32)
potential spawning position for deathmatch games
*/
class CPlayerDeathmatch : public CSpotBase
{
public:
	typedef std::vector<CPlayerDeathmatch*> TSpawnPointsType;
	static inline TSpawnPointsType &SpawnPoints ()
	{
		static TSpawnPointsType Points;
		return Points;
	}

	static void ClearSpawnPoints ()
	{
		SpawnPoints().clear();
	}

	CPlayerDeathmatch () :
		CBaseEntity (),
		CSpotBase ()
		{
		};

	CPlayerDeathmatch (sint32 Index) :
		CBaseEntity (Index),
		CSpotBase (Index)
		{
		};

	IMPLEMENT_SAVE_HEADER(CPlayerDeathmatch)

	void LoadFields (CFile &File)
	{
		CSpotBase::LoadFields (File);
		SpawnPoints().push_back (this);
	}

	virtual void Spawn ()
	{
		if (!(Game.GameMode & GAME_DEATHMATCH))
		{
#ifndef FREE_UNUSED_SPOTS
			GetSolid() = SOLID_NOT;
			GetSvFlags() = SVF_NOCLIENT;
			Link ();
#else
			Free ();
#endif
			return;
		}

		CSpotBase::Spawn ();
		SpawnPoints().push_back (this);
	};
};

LINK_CLASSNAME_TO_CLASS ("info_player_deathmatch", CPlayerDeathmatch);

/*
=======================================================================

  SelectSpawnPoint

=======================================================================
*/

/*
================
PlayersRangeFromSpot

Returns the distance to the nearest player from the given spot
================
*/
float	PlayersRangeFromSpot (CBaseEntity *spot)
{
	float	bestplayerdistance = 9999999;

	for (sint32 n = 1; n <= Game.MaxClients; n++)
	{
		CPlayerEntity *player = entity_cast<CPlayerEntity>(Game.Entities[n].Entity);

		if (!player->GetInUse())
			continue;

		if (player->Health <= 0)
			continue;

		float length = (spot->State.GetOrigin() - player->State.GetOrigin()).Length();
		if (length < bestplayerdistance)
			bestplayerdistance = length;
	}

	return bestplayerdistance;
}

/*
================
SelectRandomDeathmatchSpawnPoint

go to a random point, but NOT the two points closest
to other players
================
*/

CSpotBase *SelectRandomDeathmatchSpawnPoint ()
{
	CBaseEntity *spot1 = NULL, *spot2 = NULL;
	float range1 = 99999, range2 = 99999;

	if (!CPlayerDeathmatch::SpawnPoints().size())
		return NULL;

	for (CPlayerDeathmatch::TSpawnPointsType::iterator it = CPlayerDeathmatch::SpawnPoints().begin(); it < CPlayerDeathmatch::SpawnPoints().end(); ++it)
	{
		CPlayerDeathmatch *Found = (*it);

		float range = PlayersRangeFromSpot (Found);
		if (range < range1)
		{
			range1 = range;
			spot1 = Found;
		}
		else if (range < range2)
		{
			range2 = range;
			spot2 = Found;
		}
	}

	size_t count = CPlayerDeathmatch::SpawnPoints().size();
	if (count <= 2)
		spot1 = spot2 = NULL;
	else
		count -= 2;

	uint32 selection = irandom(count);

	CSpotBase *spot = NULL;
	for (size_t i = 0; i < count; i++)
	{
		spot = CPlayerDeathmatch::SpawnPoints()[i];

		if (spot == spot1 || spot == spot2)
			selection++;

		if (!selection--)
			break;
	}

	return spot;
}

/*
================
SelectFarthestDeathmatchSpawnPoint

================
*/
CSpotBase *SelectFarthestDeathmatchSpawnPoint ()
{
	CSpotBase	*bestspot = NULL;
	float		bestdistance = 0;

	for (CPlayerDeathmatch::TSpawnPointsType::iterator it = CPlayerDeathmatch::SpawnPoints().begin(); it < CPlayerDeathmatch::SpawnPoints().end(); ++it)
	{
		CPlayerDeathmatch *Found = (*it);

		float bestplayerdistance = PlayersRangeFromSpot (Found);
		if (bestplayerdistance > bestdistance)
		{
			bestspot = Found;
			bestdistance = bestplayerdistance;
		}
	}

	if (bestspot)
		return bestspot;

	// if there is a player just spawned on each and every start spot
	// we have no choice to turn one into a telefrag meltdown
	return CPlayerDeathmatch::SpawnPoints()[0];
}

/*QUAKED info_player_coop (1 0 1) (-16 -16 -24) (16 16 32)
potential spawning position for coop games
*/

class CPlayerCoop : public CSpotBase, public CThinkableEntity
{
public:
	typedef std::vector<CPlayerCoop*> TSpawnPointsType;
	static inline TSpawnPointsType &SpawnPoints ()
	{
		static TSpawnPointsType Points;
		return Points;
	}

	static void ClearSpawnPoints ()
	{
		SpawnPoints().clear();
	}

	CPlayerCoop () :
		CBaseEntity (),
		CThinkableEntity(),
		CSpotBase ()
		{
		};

	CPlayerCoop (sint32 Index) :
		CBaseEntity (Index),
		CThinkableEntity(),
		CSpotBase (Index)
		{
		};

	IMPLEMENT_SAVE_HEADER(CPlayerCoop)

	void SaveFields (CFile &File)
	{
		CThinkableEntity::SaveFields (File);
		CSpotBase::SaveFields (File);
	};

	void LoadFields (CFile &File)
	{
		CThinkableEntity::LoadFields (File);
		CSpotBase::LoadFields (File);
		SpawnPoints().push_back (this);
	};

	bool ParseField (const char *Key, const char *Value)
	{
		return (CSpotBase::ParseField (Key, Value));
	};

	// this function is an ugly as hell hack to fix some map flaws
	//
	// the coop spawn spots on some maps are SNAFU.  There are coop spots
	// with the wrong targetname as well as spots with no name at all
	//
	// we use carnal knowledge of the maps to fix the coop spot targetnames to match
	// that of the nearest named single player spot
	virtual void Think ();;

	virtual void Spawn ()
	{
		const static char *CheckNames[] = {
			"jail2",
			"jail3",
			"mine1",
			"mine2",
			"mine3",
			"mine4",
			"lab",
			"boss1",
			"fact3",
			"biggun",
			"space",
			"command",
			"power2",
			"strike",
			NULL
		};

		if (Game.GameMode != GAME_COOPERATIVE)
		{
			Free ();
			return;
		}

		sint32 i = 0;
		while (CheckNames[i] != NULL)
		{
			// invoke one of our gross, ugly, disgusting hacks
			if (strcmp(Level.ServerLevelName.c_str(), CheckNames[i]) == 0)
			{
				NextThink = Level.Frame + FRAMETIME;
				break;
			}

			i++;
		}

		SpawnPoints().push_back (this);
	};
};

LINK_CLASSNAME_TO_CLASS ("info_player_coop", CPlayerCoop);

CSpotBase *CPlayerEntity::SelectCoopSpawnPoint ()
{
	sint32 index = State.GetNumber()-1;

	// player 0 starts in normal player spawn point
	if (!index || !CPlayerCoop::SpawnPoints().size())
		return NULL;

	CPlayerCoop *spot = NULL;

	// assume there are four coop spots at each spawnpoint
	for (CPlayerCoop::TSpawnPointsType::iterator it = CPlayerCoop::SpawnPoints().begin(); it < CPlayerCoop::SpawnPoints().end(); ++it)
	{
		spot = (*it);

		if (!spot)
			return NULL; // wut

		const char *target = spot->TargetName;
		if (!target)
			target = "";
		if (Q_stricmp(Game.SpawnPoint.c_str(), target) == 0)
		{
			// this is a coop spawn point for one of the clients here
			if (!--index)
				return spot; // got it
		}
	}

	return spot;
}

/*QUAKED info_player_intermission (1 0 1) (-16 -16 -24) (16 16 32)
The deathmatch intermission point will be at one of these
Use 'angles' instead of 'angle', so you can set pitch or roll as well as yaw.  'pitch yaw roll'
*/
class CPlayerIntermission : public CSpotBase
{
public:
	CPlayerIntermission () :
		CBaseEntity (),
		CSpotBase ()
		{
		};

	CPlayerIntermission (sint32 Index) :
		CBaseEntity (Index),
		CSpotBase (Index)
		{
		};

	IMPLEMENT_SAVE_HEADER(CPlayerIntermission)

	void Spawn ()
	{
	};
};

LINK_CLASSNAME_TO_CLASS ("info_player_intermission", CPlayerIntermission);

/*QUAKED info_player_start (1 0 0) (-16 -16 -24) (16 16 32)
The normal starting point for a Level.
*/
class CPlayerStart : public CSpotBase, public CThinkableEntity
{
public:
	typedef std::vector<CPlayerStart*> TSpawnPointsType;
	static inline TSpawnPointsType &SpawnPoints ()
	{
		static TSpawnPointsType Points;
		return Points;
	}

	static void ClearSpawnPoints ()
	{
		SpawnPoints().clear();
	}

	CPlayerStart () :
		CBaseEntity (),
		CThinkableEntity(),
		CSpotBase ()
		{
		};

	CPlayerStart (sint32 Index) :
		CBaseEntity (Index),
		CThinkableEntity(),
		CSpotBase (Index)
		{
		};

	IMPLEMENT_SAVE_HEADER(CPlayerStart)

	void SaveFields (CFile &File)
	{
		CThinkableEntity::SaveFields (File);
		CSpotBase::SaveFields (File);
	};

	void LoadFields (CFile &File)
	{
		CThinkableEntity::LoadFields (File);
		CSpotBase::LoadFields (File);
		SpawnPoints().push_back (this);
	};

	bool ParseField (const char *Key, const char *Value)
	{
		return (CSpotBase::ParseField (Key, Value));
	};

	// some maps don't have any coop spots at all, so we need to create them
	// where they should have been
	virtual void Think ()
	{
		if (Q_stricmp(Level.ServerLevelName.c_str(), "security") == 0)
		{
			static const float origins[] =
			{
				124,
				252,
				316
			};

			for (sint32 i = 0; i < 3; i++)
			{
				CPlayerCoop *spot = QNewEntityOf CPlayerCoop;
				spot->ClassName = "info_player_coop";
				spot->State.GetOrigin().Set (origins[i], -164, 80);
				spot->TargetName = "jail3";
				spot->State.GetAngles().Set (0, 90, 0);

				CPlayerCoop::SpawnPoints().push_back (spot);
			}
		}
	};

	virtual void Spawn ()
	{
		if ((Game.GameMode == GAME_COOPERATIVE) && Q_stricmp(Level.ServerLevelName.c_str(), "security") == 0)
			// invoke one of our gross, ugly, disgusting hacks
			NextThink = Level.Frame + FRAMETIME;

		SpawnPoints().push_back (this);
	};
};

LINK_CLASSNAME_TO_CLASS ("info_player_start", CPlayerStart);

void CPlayerCoop::Think ()
{
	for (CPlayerStart::TSpawnPointsType::iterator it = CPlayerStart::SpawnPoints().begin(); it < CPlayerStart::SpawnPoints().end(); ++it)
	{
		CSpotBase *spot = (*it);

		if (!spot)
			return;
		if (!spot->TargetName)
			continue;
		
		if ((State.GetOrigin() - spot->State.GetOrigin()).Length() < 384)
		{
			if ((!TargetName) || Q_stricmp(TargetName, spot->TargetName) != 0)
				TargetName = spot->TargetName;
			return;
		}
	}
};

#if CLEANCTF_ENABLED
/*QUAKED info_player_team1 (1 0 0) (-16 -16 -24) (16 16 32)
potential team1 spawning position for ctf games
*/
class CPlayerTeam1 : public CSpotBase
{
public:
	typedef std::vector<CPlayerTeam1*> TSpawnPointsType;
	static inline TSpawnPointsType &SpawnPoints ()
	{
		static TSpawnPointsType Points;
		return Points;
	}

	static void ClearSpawnPoints ()
	{
		SpawnPoints().clear();
	}

	CPlayerTeam1 () :
		CBaseEntity (),
		CSpotBase ()
		{
		};

	CPlayerTeam1 (sint32 Index) :
		CBaseEntity (Index),
		CSpotBase (Index)
		{
		};

	IMPLEMENT_SAVE_HEADER(CPlayerTeam1)

	void Spawn ()
	{
	};
};

LINK_CLASSNAME_TO_CLASS ("info_player_team1", CPlayerTeam1);

/*QUAKED info_player_team2 (0 0 1) (-16 -16 -24) (16 16 32)
potential team2 spawning position for ctf games
*/
class CPlayerTeam2 : public CSpotBase
{
public:
	typedef std::vector<CPlayerTeam2*> TSpawnPointsType;
	static inline TSpawnPointsType &SpawnPoints ()
	{
		static TSpawnPointsType Points;
		return Points;
	}

	static void ClearSpawnPoints ()
	{
		SpawnPoints().clear();
	}

	CPlayerTeam2 () :
		CBaseEntity (),
		CSpotBase ()
		{
		};

	CPlayerTeam2 (sint32 Index) :
		CBaseEntity (Index),
		CSpotBase (Index)
		{
		};

	IMPLEMENT_SAVE_HEADER(CPlayerTeam2)

	void Spawn ()
	{
	};
};

LINK_CLASSNAME_TO_CLASS ("info_player_team2", CPlayerTeam2);

/*
================
SelectCTFSpawnPoint

go to a ctf point, but NOT the two points closest
to other players
================
*/
template <class TIterator>
void CheckCTFSpotRanges (TIterator Begin, TIterator End, float &range1, float &range2, CSpotBase **spot1, CSpotBase **spot2)
{
	for (TIterator it = Begin; it < End; ++it)
	{
		CSpotBase *spot = (*it);

		float range = PlayersRangeFromSpot(spot);
		if (range < range1)
		{
			range1 = range;
			*spot1 = spot;
		}
		else if (range < range2)
		{
			range2 = range;
			*spot2 = spot;
		}
	}
}

CSpotBase *CPlayerEntity::SelectCTFSpawnPoint ()
{
	CSpotBase	*spot1 = NULL, *spot2 = NULL;
	float	range1 = 99999, range2 = 99999;

	if (Client.Respawn.CTF.State)
		return (DeathmatchFlags.dfSpawnFarthest.IsEnabled()) ? SelectFarthestDeathmatchSpawnPoint () : SelectRandomDeathmatchSpawnPoint ();

	Client.Respawn.CTF.State++;

	size_t count;
	switch (Client.Respawn.CTF.Team)
	{
	case CTF_TEAM1:
		count = CPlayerTeam1::SpawnPoints().size();
		CheckCTFSpotRanges<CPlayerTeam1::TSpawnPointsType::iterator> (CPlayerTeam1::SpawnPoints().begin(), CPlayerTeam1::SpawnPoints().end(), range1, range2, &spot1, &spot2); 
		break;
	case CTF_TEAM2:
		count = CPlayerTeam2::SpawnPoints().size();
		CheckCTFSpotRanges<CPlayerTeam2::TSpawnPointsType::iterator> (CPlayerTeam2::SpawnPoints().begin(), CPlayerTeam2::SpawnPoints().end(), range1, range2, &spot1, &spot2); 
		break;
	default:
		return SelectRandomDeathmatchSpawnPoint();
	}

	if (!count)
		return SelectRandomDeathmatchSpawnPoint();

	if (count <= 2)
		spot1 = spot2 = NULL;
	else
		count -= 2;

	size_t selection = irandom(count);

	CSpotBase *spot = NULL;
	for (size_t i = 0; i < count; i++)
	{
		switch (Client.Respawn.CTF.Team)
		{
		case CTF_TEAM1:
			spot = CPlayerTeam1::SpawnPoints()[i];
			break;
		case CTF_TEAM2:
			spot = CPlayerTeam2::SpawnPoints()[i];
			break;
		}

		if (spot == spot1 || spot == spot2)
			selection++;

		if (!selection--)
			break;
	}

	return spot;
}

#endif

void ClearSpawnPoints ()
{
	CPlayerDeathmatch::ClearSpawnPoints ();
	CPlayerCoop::ClearSpawnPoints ();
	CPlayerStart::ClearSpawnPoints ();
#if CLEANCTF_ENABLED
	CPlayerTeam1::ClearSpawnPoints ();
	CPlayerTeam2::ClearSpawnPoints ();
#endif
}

/*
===========
SelectSpawnPoint

Chooses a player start, deathmatch start, coop start, etc
============
*/
void	CPlayerEntity::SelectSpawnPoint (vec3f &origin, vec3f &angles)
{
	CSpotBase	*spot = NULL;

	if (!(Game.GameMode & GAME_SINGLEPLAYER))
		spot = 
#if CLEANCTF_ENABLED
		(Game.GameMode & GAME_CTF) ? SelectCTFSpawnPoint() :
#endif
		(Game.GameMode & GAME_DEATHMATCH) ? SelectDeathmatchSpawnPoint () : SelectCoopSpawnPoint ();

	// find a single player start spot
	if (!spot)
	{
		for (CPlayerStart::TSpawnPointsType::iterator it = CPlayerStart::SpawnPoints().begin(); it < CPlayerStart::SpawnPoints().end(); ++it)
		{
			spot = (*it);

			if (Game.SpawnPoint.empty() && !spot->TargetName)
				break;

			if (Game.SpawnPoint.empty()|| !spot->TargetName)
				continue;

			if (Q_stricmp(Game.SpawnPoint.c_str(), spot->TargetName) == 0)
				break;
		}

		if (!spot)
		{
			// There wasn't a spawnpoint without a target, so use any
			if (Game.SpawnPoint.empty())
			{
				if (CPlayerStart::SpawnPoints().size() && CPlayerStart::SpawnPoints().at(0))
					spot = CPlayerStart::SpawnPoints().at(0);

				if (!spot && CPlayerDeathmatch::SpawnPoints().size())
					spot = CPlayerDeathmatch::SpawnPoints()[0];
			}

			if (!spot)
			{
				MapPrint (MAPPRINT_ERROR, NULL, vec3fOrigin, "Couldn't find a suitable spawn point!\n");

				origin.Set (0, 0, 0);
				angles.Set (0, 0, 0);
				
				return;
			}
		}
	}

	origin = spot->State.GetOrigin () + vec3f(0, 0, 9);
	angles = spot->State.GetAngles ();
}

/*QUAKED path_corner (.5 .3 0) (-8 -8 -8) (8 8 8) TELEPORT
Target: next path corner
Pathtarget: gets used when an entity that has
	this path_corner targeted touches it
*/

#define CORNER_TELEPORT		1

CPathCorner::CPathCorner () :
  CBaseEntity(),
  CMapEntity (),
  CTouchableEntity (),
  CUsableEntity ()
  {
  };

CPathCorner::CPathCorner (sint32 Index) :
  CBaseEntity(Index),
  CMapEntity (Index),
  CTouchableEntity (Index),
  CUsableEntity (Index)
  {
  };

void CPathCorner::Think ()
{
	//NextTarget = (Target) ? CC_PickTarget (Target) : NULL;
	if (Target)
		NextTargets = CC_GetTargets (Target);
}

void CPathCorner::Touch (CBaseEntity *Other, plane_t *plane, cmBspSurface_t *surf)
{
	if (!(Other->EntityFlags & ENT_MONSTER))
		return;

	CMonsterEntity	*Monster = entity_cast<CMonsterEntity>(Other);

	if (Monster->MoveTarget != this)
		return;
	
	if (Monster->Enemy)
		return;

	if (PathTarget)
	{
		char *savetarget = Target;
		Target = PathTarget;
		UseTargets (Other, Message);
		Target = savetarget;
	}

	CBaseEntity *TempNextTarget = (NextTargets.size()) ? NextTargets[irandom(NextTargets.size())] : NULL;

	if ((TempNextTarget) && (TempNextTarget->SpawnFlags & CORNER_TELEPORT))
	{
		Other->State.GetOrigin() = (TempNextTarget->State.GetOrigin() + vec3f(0, 0, TempNextTarget->GetMins().Z - Other->GetMins().Z));
		TempNextTarget = entity_cast<CPathCorner>(TempNextTarget)->NextTargets[irandom(NextTargets.size())];
		Other->State.GetEvent() = EV_OTHER_TELEPORT;
	}

	Monster->GoalEntity = Monster->MoveTarget = TempNextTarget;

	if (Wait)
	{
		Monster->Monster->PauseTime = Level.Frame + Wait;
		Monster->Monster->Stand();
	}
	else
	{
		if (!Monster->MoveTarget)
		{
			Monster->Monster->PauseTime = Level.Frame + 100000000;
			Monster->Monster->Stand ();
		}
		else
			Monster->Monster->IdealYaw = (Monster->GoalEntity->State.GetOrigin() - Monster->State.GetOrigin()).ToYaw();
	}
};

void CPathCorner::Spawn ()
{
	if (!TargetName)
	{
		MapPrint (MAPPRINT_ERROR, this, State.GetOrigin(), "No targetname\n");
		Free ();
		return;
	}

	GetSolid() = SOLID_TRIGGER;
	Touchable = true;
	GetMins().Set (-8);
	GetMaxs().Set (8);
	GetSvFlags() |= SVF_NOCLIENT;
	Link ();
	NextThink = Level.Frame + 1;
};

ENTITYFIELDS_BEGIN(CPathCorner)
{
	CEntityField ("wait", EntityMemberOffset(CPathCorner,Wait), FT_FRAMENUMBER | FT_SAVABLE),
};
ENTITYFIELDS_END(CPathCorner)

bool			CPathCorner::ParseField (const char *Key, const char *Value)
{
	if (CheckFields<CPathCorner> (this, Key, Value))
		return true;

	// Couldn't find it here
	return (CUsableEntity::ParseField (Key, Value) || CMapEntity::ParseField (Key, Value));
};

void		CPathCorner::SaveFields (CFile &File)
{
	// Save NextTargets
	File.Write<uint32> (NextTargets.size());
	for (size_t i = 0; i < NextTargets.size(); i++)
		File.Write<sint32> (NextTargets[i]->State.GetNumber());

	SaveEntityFields <CPathCorner> (this, File);
	CMapEntity::SaveFields (File);
	CUsableEntity::SaveFields (File);
	CTouchableEntity::SaveFields (File);
}

void		CPathCorner::LoadFields (CFile &File)
{
	uint32 Num = File.Read<uint32> ();
	for (size_t i = 0; i < Num; i++)
		NextTargets.push_back (Game.Entities[File.Read<sint32> ()].Entity);

	LoadEntityFields <CPathCorner> (this, File);
	CMapEntity::LoadFields (File);
	CUsableEntity::LoadFields (File);
	CTouchableEntity::LoadFields (File);
}

LINK_CLASSNAME_TO_CLASS ("path_corner", CPathCorner);

/*QUAKED point_combat (0.5 0.3 0) (-8 -8 -8) (8 8 8) Hold
Makes this the target of a monster and it will head here
when first activated before going after the Activator.  If
hold is selected, it will stay here.
*/
class CPathCombat : public CPathCorner
{
public:
	CPathCombat () :
	  CBaseEntity (),
	  CPathCorner ()
	  {
	  };

	CPathCombat (sint32 Index) :
	  CBaseEntity (Index),
	  CPathCorner (Index)
	  {
	  };

	const char *SAVE_GetName () { return "CPathCombat"; }

	void Touch (CBaseEntity *Other, plane_t *plane, cmBspSurface_t *surf)
	{
		CMonsterEntity *Monster = NULL;
		if (Other->EntityFlags & ENT_MONSTER)
			Monster = entity_cast<CMonsterEntity>(Other);

		if (Monster && (!Monster->MoveTarget || Monster->MoveTarget != this))
			return;

		if (Target)
		{
			if (Other->EntityFlags & ENT_USABLE)
			{
				CUsableEntity *Usable = entity_cast<CUsableEntity>(Other);
				Usable->Target = Target;

				if (Monster)
					Monster->GoalEntity = Monster->MoveTarget = CC_PickTarget(Usable->Target);
			}

			if (Monster && !Monster->GoalEntity)
			{
				MapPrint (MAPPRINT_WARNING, this, State.GetOrigin(), "Target %s does not exist\n", Target);
				Monster->MoveTarget = this;
			}

			Target = NULL;
		}
		else if ((SpawnFlags & CORNER_TELEPORT) && !(Other->Flags & (FL_SWIM|FL_FLY)))
		{
			if (Monster)
			{
				Monster->Monster->PauseTime = Level.Frame + 100000000;
				Monster->Monster->AIFlags |= AI_STAND_GROUND;
				Monster->Monster->Stand ();
			}
		}

		if (Monster && (Monster->MoveTarget == this))
		{
			Monster->Target = NULL;
			Monster->MoveTarget = NULL;
			Monster->GoalEntity = Monster->Enemy;

			Monster->Monster->AIFlags &= ~AI_COMBAT_POINT;
		}

		if (PathTarget)
		{
			CBaseEntity *Activator;

			char *savetarget = Target;
			Target = PathTarget;
			if (Other->Enemy && (Other->Enemy->EntityFlags & ENT_PLAYER))
				Activator = Other->Enemy;
			else if ((Monster) &&
				(Monster->OldEnemy) && (Monster->OldEnemy->EntityFlags & ENT_PLAYER))
				Activator = Monster->OldEnemy;
			else if ((Other->EntityFlags & ENT_USABLE) && (entity_cast<CUsableEntity>(Other)->User) && ((entity_cast<CUsableEntity>(Other)->User)->EntityFlags & ENT_PLAYER))
				Activator = (entity_cast<CUsableEntity>(Other)->User);
			else
				Activator = Other;
			UseTargets (Activator, Message);
			Target = savetarget;
		}
	};

	void Spawn ()
	{
		if (Game.GameMode & GAME_DEATHMATCH)
		{
			Free ();
			return;
		}

		GetSolid() = SOLID_TRIGGER;
		Touchable = true;
		GetMins().Set (-8, -8, -16);
		GetMaxs().Set (8, 8, 16);
		GetSvFlags() = SVF_NOCLIENT;
		Link ();
	};
};

LINK_CLASSNAME_TO_CLASS ("point_combat", CPathCombat);

/*QUAKED info_null (0 0.5 0) (-4 -4 -4) (4 4 4)
Used as a positional target for spotlights, etc.
*/
class CInfoNull : public CMapEntity
{
public:
	CInfoNull (sint32 Index) :
	  CBaseEntity (Index),
	  CMapEntity (Index)
	  {
	  };

	IMPLEMENT_SAVE_HEADER(CInfoNull)

	void Spawn ()
	{
		Free ();
	};
};

LINK_CLASSNAME_TO_CLASS ("info_null", CInfoNull);

/*QUAKED func_group (0 0 0) ?
Used to group brushes together just for editor convenience.
*/
class CFuncGroup : public CInfoNull
{
public:
	CFuncGroup (sint32 Index) :
	  CBaseEntity (Index),
	  CInfoNull (Index)
	  {
	  };

	  IMPLEMENT_SAVE_HEADER(CFuncGroup)
};

LINK_CLASSNAME_TO_CLASS ("func_group", CFuncGroup);

/*QUAKED info_notnull (0 0.5 0) (-4 -4 -4) (4 4 4)
Used as a positional target for lightning.
*/
class CInfoNotNull : public CMapEntity
{
public:
	CInfoNotNull (sint32 Index) :
	  CBaseEntity (Index),
	  CMapEntity (Index)
	  {
	  };

	IMPLEMENT_SAVE_HEADER(CInfoNotNull)

	void Spawn ()
	{
		GetAbsMin() = State.GetOrigin();
		GetAbsMax() = State.GetOrigin();
	};
};

LINK_CLASSNAME_TO_CLASS ("info_notnull", CInfoNotNull);

/*QUAKED light (0 1 0) (-8 -8 -8) (8 8 8) START_OFF
Non-displayed light.
Default light value is 300.
Default style is 0.
If targeted, will toggle between on and off.
Default _cone value is 10 (used to set size of light for spotlights)
*/

#define START_OFF	1

class CLight : public CMapEntity, public CUsableEntity
{
public:
	uint8		Style;

	CLight (sint32 Index) :
	  CBaseEntity (Index),
	  CMapEntity (Index),
	  CUsableEntity (Index)
	  {
	  };

	ENTITYFIELD_DEFS
	ENTITYFIELDS_SAVABLE(CLight)

	void Use (CBaseEntity *Other, CBaseEntity *Activator)
	{
		if (!Usable)
			return;

		if (SpawnFlags & START_OFF)
		{
			ConfigString (CS_LIGHTS+Style, "m");
			SpawnFlags &= ~START_OFF;
		}
		else
		{
			ConfigString (CS_LIGHTS+Style, "a");
			SpawnFlags |= START_OFF;
		}
	};

	void Spawn ()
	{
		// no targeted lights in deathmatch, because they cause global messages
		if (!TargetName || (Game.GameMode & GAME_DEATHMATCH))
		{
			Free ();
			return;
		}

		if (Style >= 32)
		{
			Usable = true;
			ConfigString (CS_LIGHTS+Style, (SpawnFlags & START_OFF) ? "a" : "m");
		}
	};
};

ENTITYFIELDS_BEGIN(CLight)
{
	CEntityField ("style", EntityMemberOffset(CLight,Style), FT_BYTE | FT_SAVABLE),
};
ENTITYFIELDS_END(CLight)

bool CLight::ParseField (const char *Key, const char *Value)
{
	if (CheckFields<CLight> (this, Key, Value))
		return true;

	return (CUsableEntity::ParseField (Key, Value) || CMapEntity::ParseField (Key, Value));
}

void		CLight::SaveFields (CFile &File)
{
	SaveEntityFields <CLight> (this, File);
	CMapEntity::SaveFields (File);
	CUsableEntity::SaveFields (File);
}

void		CLight::LoadFields (CFile &File)
{
	LoadEntityFields <CLight> (this, File);
	CMapEntity::LoadFields (File);
	CUsableEntity::LoadFields (File);
}

LINK_CLASSNAME_TO_CLASS ("light", CLight);

/*QUAKED target_lightramp (0 .5 .8) (-8 -8 -8) (8 8 8) TOGGLE
speed		How many seconds the ramping will take
message		two letters; starting lightlevel and ending lightlevel
*/

#define LIGHTRAMP_TOGGLE	1

class CTargetLightRamp : public CMapEntity, public CThinkableEntity, public CUsableEntity
{
public:
	sint32			RampMessage[3];
	FrameNumber_t	TimeStamp;
	CLight			*Light;
	float			Speed;
	uint8			Style;

	CTargetLightRamp () :
	  CBaseEntity (),
	  CMapEntity (),
	  CThinkableEntity (),
	  CUsableEntity (),
	  Light (NULL),
	  Speed (0)
	{
		RampMessage[0] = RampMessage[1] = RampMessage[2] = 0;
	};

	CTargetLightRamp (sint32 Index) :
	  CBaseEntity (Index),
	  CMapEntity (Index),
	  CThinkableEntity (Index),
	  CUsableEntity (Index),
	  Light (NULL),
	  Speed (0)
	{
		RampMessage[0] = RampMessage[1] = RampMessage[2] = 0;
	};

	ENTITYFIELD_DEFS
	ENTITYFIELDS_SAVABLE(CTargetLightRamp)

	bool Run ()
	{
		return CBaseEntity::Run();
	};

	void Think ()
	{
		char	style[2] = {'a' + RampMessage[0] + (Level.Frame - TimeStamp) / 0.1f * RampMessage[2], 0};
		ConfigString (CS_LIGHTS+Light->Style, style);

		if ((Level.Frame - TimeStamp) < Speed)
			NextThink = Level.Frame + FRAMETIME;
		else if (SpawnFlags & LIGHTRAMP_TOGGLE)
		{
			sint32 temp = RampMessage[0];
			RampMessage[0] = RampMessage[1];
			RampMessage[1] = temp;
			RampMessage[2] *= -1;
		}
	};

	void Use (CBaseEntity *Other, CBaseEntity *Activator)
	{
		if (!Light)
		{
			// check all the targets
			CMapEntity *e = NULL;
			while (1)
			{
				e = CC_Find<CMapEntity, ENT_MAP, EntityMemberOffset(CMapEntity,TargetName)> (e, Target);
				if (!e)
					break;
				if (strcmp(e->ClassName.c_str(), "light") != 0)
					MapPrint (MAPPRINT_WARNING, this, State.GetOrigin(), "Target \"%s\" is not a light\n", Target);
				else
					Light = entity_cast<CLight>(e);
			}

			if (!Light)
			{
				MapPrint (MAPPRINT_ERROR, this, State.GetOrigin(), "Target \"%s\" not found\n", Target);
				Free ();
				return;
			}
		}

		TimeStamp = Level.Frame;
		Think ();
	};

	void Spawn ()
	{
		if (Message.empty() || Message.length() != 2 || Message[0] < 'a' || Message[0] > 'z' || Message[1] < 'a' || Message[1] > 'z' || Message[0] == Message[1])
		{
			MapPrint (MAPPRINT_ERROR, this, State.GetOrigin(), "Bad ramp (%s)\n", Message.c_str());
			Free ();
			return;
		}

		if (Game.GameMode & GAME_DEATHMATCH)
		{
			Free ();
			return;
		}

		if (!Target)
		{
			MapPrint (MAPPRINT_ERROR, this, State.GetOrigin(), "No target\n");
			Free ();
			return;
		}

		GetSvFlags() |= SVF_NOCLIENT;

		RampMessage[0] = Message[0] - 'a';
		RampMessage[1] = Message[1] - 'a';
		RampMessage[2] = (RampMessage[1] - RampMessage[0]) / (Speed * 10);
	};
};

ENTITYFIELDS_BEGIN(CTargetLightRamp)
{
	CEntityField ("speed", EntityMemberOffset(CTargetLightRamp,Speed), FT_FLOAT | FT_SAVABLE),
	CEntityField ("style", EntityMemberOffset(CTargetLightRamp,Style), FT_BYTE | FT_SAVABLE),

	CEntityField ("RampMessage[0]", EntityMemberOffset(CTargetLightRamp,RampMessage[0]), FT_INT | FT_NOSPAWN | FT_SAVABLE),
	CEntityField ("RampMessage[1]", EntityMemberOffset(CTargetLightRamp,RampMessage[1]), FT_INT | FT_NOSPAWN | FT_SAVABLE),
	CEntityField ("RampMessage[2]", EntityMemberOffset(CTargetLightRamp,RampMessage[2]), FT_INT | FT_NOSPAWN | FT_SAVABLE),
	CEntityField ("TimeStamp", EntityMemberOffset(CTargetLightRamp,TimeStamp), FT_FRAMENUMBER | FT_NOSPAWN | FT_SAVABLE),
	CEntityField ("Speed", EntityMemberOffset(CTargetLightRamp,Speed), FT_FLOAT | FT_NOSPAWN | FT_SAVABLE),
	CEntityField ("Style", EntityMemberOffset(CTargetLightRamp,Style), FT_BYTE | FT_NOSPAWN | FT_SAVABLE),
};
ENTITYFIELDS_END(CTargetLightRamp)

bool			CTargetLightRamp::ParseField (const char *Key, const char *Value)
{
	if (CheckFields<CTargetLightRamp> (this, Key, Value))
		return true;

	// Couldn't find it here
	return (CUsableEntity::ParseField (Key, Value) || CMapEntity::ParseField (Key, Value));
};

void		CTargetLightRamp::SaveFields (CFile &File)
{
	File.Write<sint32> ((Light) ? Light->State.GetNumber() : -1);

	SaveEntityFields <CTargetLightRamp> (this, File);
	CMapEntity::SaveFields (File);
	CUsableEntity::SaveFields (File);
	CThinkableEntity::SaveFields (File);
}

void		CTargetLightRamp::LoadFields (CFile &File)
{
	sint32 Index = File.Read<sint32> ();

	if (Index != -1)
		Light = entity_cast<CLight>(Game.Entities[Index].Entity);

	LoadEntityFields <CTargetLightRamp> (this, File);
	CMapEntity::LoadFields (File);
	CUsableEntity::LoadFields (File);
	CThinkableEntity::LoadFields (File);
}

LINK_CLASSNAME_TO_CLASS ("target_lightramp", CTargetLightRamp);

