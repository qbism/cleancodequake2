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

/*QUAKED misc_teleporter_dest (1 0 0) (-32 -32 -24) (32 32 -16)
Point teleporters at these.
*/
class CSpotBase : public CMapEntity
{
public:
	CSpotBase () :
		CBaseEntity (),
		CMapEntity ()
		{
		};

	CSpotBase(int Index) :
		CBaseEntity(Index),
		CMapEntity(Index)
		{
		};

	virtual void Spawn ()
	{
		State.GetModelIndex() = ModelIndex("models/objects/dmspot/tris.md2");
		State.GetSkinNum() = 0;
		GetSolid() = SOLID_BBOX;
		GetMins().Set (-32, -32, -24);
		GetMaxs().Set (32, 32, -16);
		Link ();
	};
};

class CTeleporterDest : public CSpotBase
{
public:
	CTeleporterDest () :
		CBaseEntity (),
		CSpotBase ()
		{
		};

	CTeleporterDest (int Index) :
		CBaseEntity (Index),
		CSpotBase (Index)
		{
		};

	virtual void Spawn ()
	{
		CSpotBase::Spawn ();
	};
};

LINK_CLASSNAME_TO_CLASS ("misc_teleporter_dest", CTeleporterDest);

class CTeleporterTrigger : public CMapEntity, public CTouchableEntity, public CThinkableEntity
{
public:
	CBaseEntity		*Dest;
	char			*Target;

	CTeleporterTrigger() :
	  CBaseEntity (),
	  CMapEntity(),
	  CTouchableEntity (),
	  Dest (NULL)
	  {
	  };

	CTeleporterTrigger(int Index) :
	  CBaseEntity (Index),
	  CMapEntity(),
	  CTouchableEntity (Index),
	  Dest (NULL)
	  {
	  };

	void Touch (CBaseEntity *other, plane_t *plane, cmBspSurface_t *surf)
	{
		CPlayerEntity	*Player = NULL;
		if (other->EntityFlags & ENT_PLAYER)
			Player = entity_cast<CPlayerEntity>(other);

	#ifdef CLEANCTF_ENABLED
		//ZOID
		if (Player)
			CGrapple::PlayerResetGrapple(Player);
		//ZOID
	#endif

		// unlink to make sure it can't possibly interfere with KillBox
		other->Unlink ();

		other->State.GetOrigin() = (Dest->State.GetOrigin() + vec3f(0,0,10));
		other->State.GetOldOrigin() = Dest->State.GetOrigin();

		// clear the velocity and hold them in place briefly
		if (other->EntityFlags & ENT_PHYSICS)
			entity_cast<CPhysicsEntity>(other)->Velocity.Clear ();
		if (Player)
		{
			Player->Client.PlayerState.GetPMove()->pmTime = 160>>3;		// hold time
			Player->Client.PlayerState.GetPMove()->pmFlags |= PMF_TIME_TELEPORT;
		}

		// draw the teleport splash at source and on the player
		other->State.GetEvent() = (Player) ? EV_PLAYER_TELEPORT : EV_OTHER_TELEPORT;

		// set angles
		if (Player)
		{
			for (int i = 0; i < 3; i++)
				Player->Client.PlayerState.GetPMove()->deltaAngles[i] = ANGLE2SHORT(Dest->State.GetAngles()[i] - Player->Client.Respawn.cmd_angles[i]);
		}

		other->State.GetAngles().Clear ();
		if (Player)
		{
			Player->Client.PlayerState.GetViewAngles().Clear ();
			Player->Client.ViewAngle.Clear ();
		}

		// kill anything at the destination
		other->KillBox ();

		other->Link ();
	};

	void Think ()
	{
		Dest = CC_Find<CMapEntity, ENT_MAP, EntityMemberOffset(CMapEntity,TargetName)> (NULL, Target);
	
		if (!Dest)
			DebugPrintf ("Couldn't find teleporter\n");
	};

	virtual void Spawn ()
	{
		NextThink = level.framenum + 1;
	};
};

class CTeleporter : public CSpotBase
{
public:
	char			*Target;

	ENTITYFIELD_DEFS

	CTeleporter () :
		CBaseEntity (),
		CSpotBase ()
		{
		};

	CTeleporter (int Index) :
		CBaseEntity (Index),
		CSpotBase (Index)
		{
		};

	virtual void Spawn ()
	{
		if (!Target)
		{
			//gi.dprintf ("teleporter without a target.\n");
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

		CTeleporterTrigger *trig = QNew (com_levelPool, 0) CTeleporterTrigger;
		trig->Touchable = true;
		trig->GetSolid() = SOLID_TRIGGER;
		trig->Target = Target;
		trig->SetOwner (this);
		trig->State.GetOrigin() = State.GetOrigin();
		trig->GetMins().Set (-8, -8, 8);
		trig->GetMaxs().Set (8, 8, 24);
		trig->NextThink = level.framenum + 1;
		trig->Link ();
	};
};

ENTITYFIELDS_BEGIN(CTeleporter)
{
	CEntityField ("target", EntityMemberOffset(CTeleporter,Target), FT_LEVEL_STRING),
};
ENTITYFIELDS_END(CTeleporter)

bool			CTeleporter::ParseField (char *Key, char *Value)
{
	if (CheckFields<CTeleporter> (this, Key, Value))
		return true;

	// Couldn't find it here
	return (CMapEntity::ParseField (Key, Value));
};

LINK_CLASSNAME_TO_CLASS ("misc_teleporter", CTeleporter);

/*QUAKED trigger_teleport (0.5 0.5 0.5) ?
Players touching this will be teleported
*/
class CTriggerTeleportDest : public CTeleporterTrigger
{
public:
	CTriggerTeleportDest () :
	  CBaseEntity(),
	  CTeleporterTrigger ()
	  {
	  };

	CTriggerTeleportDest (int Index) :
	  CBaseEntity(Index),
	  CTeleporterTrigger (Index)
	  {
	  };

	class NoiseMaker : public CBaseEntity
	{
	public:
		NoiseMaker () :
			CBaseEntity ()
			{
			};
	};
	void Spawn ()
	{
		if (!Target)
		{
			DebugPrintf ("teleporter without a target.\n");
			Free ();
			return;
		}

		GetSvFlags() |= SVF_NOCLIENT;
		GetSolid() = SOLID_TRIGGER;
		Touchable = true;
		SetBrushModel ();
		Link ();

		// noise maker and splash effect dude
		NoiseMaker *s = QNew (com_levelPool, 0) NoiseMaker;
		s->State.GetOrigin() = (GetMins() + ((GetMaxs() - GetMins()) / 2));
		s->State.GetSound() = SoundIndex ("world/hum1.wav");
		s->Link ();
	};
};

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

	CInfoTeleportDest (int Index) :
		CBaseEntity (Index),
		CSpotBase (Index)
		{
		};

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
	CPlayerDeathmatch () :
		CBaseEntity (),
		CSpotBase ()
		{
		};

	CPlayerDeathmatch (int Index) :
		CBaseEntity (Index),
		CSpotBase (Index)
		{
		};

	virtual void Spawn ()
	{
		if (!(game.mode & GAME_DEATHMATCH))
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
	};
};

LINK_CLASSNAME_TO_CLASS ("info_player_deathmatch", CPlayerDeathmatch);

/*QUAKED info_player_coop (1 0 1) (-16 -16 -24) (16 16 32)
potential spawning position for coop games
*/

class CPlayerCoop : public CSpotBase, public CThinkableEntity
{
public:
	CPlayerCoop () :
		CBaseEntity (),
		CThinkableEntity(),
		CSpotBase ()
		{
		};

	CPlayerCoop (int Index) :
		CBaseEntity (Index),
		CThinkableEntity(),
		CSpotBase (Index)
		{
		};

	// this function is an ugly as hell hack to fix some map flaws
	//
	// the coop spawn spots on some maps are SNAFU.  There are coop spots
	// with the wrong targetname as well as spots with no name at all
	//
	// we use carnal knowledge of the maps to fix the coop spot targetnames to match
	// that of the nearest named single player spot
	virtual void Think ()
	{
		CMapEntity *spot = NULL;
		while(1)
		{
			spot = CC_Find<CMapEntity, ENT_MAP, EntityMemberOffset (CMapEntity,TargetName)> (spot, "info_player_start");

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

		if (game.mode != GAME_COOPERATIVE)
		{
			Free ();
			return;
		}

		int i = 0;
		while (CheckNames[i] != NULL)
		{
			// invoke one of our gross, ugly, disgusting hacks
			if (strcmp(level.mapname, CheckNames[i]) == 0)
				NextThink = level.framenum + FRAMETIME;
		
			i++;
		}
	};
};

LINK_CLASSNAME_TO_CLASS ("info_player_coop", CPlayerCoop);

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

	CPlayerIntermission (int Index) :
		CBaseEntity (Index),
		CSpotBase (Index)
		{
		};

	void Spawn ()
	{
	};
};

LINK_CLASSNAME_TO_CLASS ("info_player_intermission", CPlayerIntermission);

/*QUAKED info_player_start (1 0 0) (-16 -16 -24) (16 16 32)
The normal starting point for a level.
*/
class CPlayerStart : public CSpotBase, public CThinkableEntity
{
public:
	CPlayerStart () :
		CBaseEntity (),
		CThinkableEntity(),
		CSpotBase ()
		{
		};

	CPlayerStart (int Index) :
		CBaseEntity (Index),
		CThinkableEntity(),
		CSpotBase (Index)
		{
		};

	// some maps don't have any coop spots at all, so we need to create them
	// where they should have been
	virtual void Think ()
	{
		if(Q_stricmp(level.mapname, "security") == 0)
		{
			vec3f origins[] =
			{
				vec3f(124, -164, 80),
				vec3f(252, -164, 80),
				vec3f(316, -164, 80)
			};

			for (int i = 0; i < 3; i++)
			{
				CPlayerCoop *spot = QNew (com_levelPool, 0) CPlayerCoop;
				spot->gameEntity->classname = "info_player_coop";
				spot->State.GetOrigin() = origins[i];
				spot->TargetName = "jail3";
				spot->State.GetAngles().Set (0, 90, 0);
			}
		}
	};

	virtual void Spawn ()
	{
		if (game.mode != GAME_COOPERATIVE)
			return;
		if(Q_stricmp(level.mapname, "security") == 0)
			// invoke one of our gross, ugly, disgusting hacks
			NextThink = level.framenum + FRAMETIME;
	};
};

LINK_CLASSNAME_TO_CLASS ("info_player_start", CPlayerStart);

#ifdef CLEANCTF_ENABLED
/*QUAKED info_player_team1 (1 0 0) (-16 -16 -24) (16 16 32)
potential team1 spawning position for ctf games
*/
class CPlayerTeam1 : public CSpotBase
{
public:
	CPlayerTeam1 () :
		CBaseEntity (),
		CSpotBase ()
		{
		};

	CPlayerTeam1 (int Index) :
		CBaseEntity (Index),
		CSpotBase (Index)
		{
		};

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
	CPlayerTeam2 () :
		CBaseEntity (),
		CSpotBase ()
		{
		};

	CPlayerTeam2 (int Index) :
		CBaseEntity (Index),
		CSpotBase (Index)
		{
		};

	void Spawn ()
	{
	};
};

LINK_CLASSNAME_TO_CLASS ("info_player_team2", CPlayerTeam2);

#endif

/*QUAKED path_corner (.5 .3 0) (-8 -8 -8) (8 8 8) TELEPORT
Target: next path corner
Pathtarget: gets used when an entity that has
	this path_corner targeted touches it
*/

#include "cc_brushmodels.h"

CPathCorner::CPathCorner () :
  CBaseEntity(),
  CMapEntity (),
  CTouchableEntity ()
  {
  };

CPathCorner::CPathCorner (int Index) :
  CBaseEntity(Index),
  CMapEntity (Index),
  CTouchableEntity (Index)
  {
  };

void CPathCorner::Touch (CBaseEntity *other, plane_t *plane, cmBspSurface_t *surf)
{
	vec3f			v;
	CBaseEntity		*next;

	if (other->EntityFlags & ENT_MONSTER)
	{
		if (entity_cast<CMonsterEntity>(other)->MoveTarget != this)
			return;
	}
	else if (other->EntityFlags & ENT_BRUSHMODEL)
	{
		if (entity_cast<CBrushModel>(other)->BrushType & BRUSH_TRAIN)
		{
			CTrainBase *Train = entity_cast<CTrainBase>(other);

			if (Train->TargetEntity != this)
				return;
		}
	}
	
	if (other->Enemy)
		return;

	if (gameEntity->pathtarget)
	{
		char *savetarget;

		savetarget = Target;
		Target = gameEntity->pathtarget;
		UseTargets (other, Message);
		Target = savetarget;
	}

	next = (Target) ? CC_PickTarget(Target) : NULL;
	if ((next) && (next->SpawnFlags & 1))
	{
		other->State.GetOrigin() = (next->State.GetOrigin() + vec3f(0, 0, next->GetMins().Z - other->GetMins().Z));
		next = CC_PickTarget(entity_cast<CUsableEntity>(next)->Target);
		other->State.GetEvent() = EV_OTHER_TELEPORT;
	}

	CMonsterEntity *Monster = NULL;
	if (other->EntityFlags & ENT_MONSTER)
		Monster = entity_cast<CMonsterEntity>(other);

	if (Monster)
		Monster->GoalEntity = Monster->MoveTarget = next;

	if (Wait)
	{
		if (Monster)
		{
			Monster->Monster->PauseTime = level.framenum + Wait;
			Monster->Monster->Stand();
		}
		return;
	}

	if (Monster)
	{
		if (!Monster->MoveTarget)
		{
			Monster->Monster->PauseTime = level.framenum + 100000000;
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
		//gi.dprintf ("path_corner with no targetname at (%f %f %f)\n", self->state.origin[0], self->state.origin[1], self->state.origin[2]);
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
};

ENTITYFIELDS_BEGIN(CPathCorner)
{
	CEntityField ("wait", EntityMemberOffset(CPathCorner,Wait), FT_FRAMENUMBER),
};
ENTITYFIELDS_END(CPathCorner)

bool			CPathCorner::ParseField (char *Key, char *Value)
{
	if (CheckFields<CPathCorner> (this, Key, Value))
		return true;

	// Couldn't find it here
	return (CUsableEntity::ParseField (Key, Value) || CMapEntity::ParseField (Key, Value));
};

LINK_CLASSNAME_TO_CLASS ("path_corner", CPathCorner);

/*QUAKED point_combat (0.5 0.3 0) (-8 -8 -8) (8 8 8) Hold
Makes this the target of a monster and it will head here
when first activated before going after the activator.  If
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

	CPathCombat (int Index) :
	  CBaseEntity (Index),
	  CPathCorner (Index)
	  {
	  };

	void Touch (CBaseEntity *other, plane_t *plane, cmBspSurface_t *surf)
	{
		CMonsterEntity *Monster = NULL;
		if (other->EntityFlags & ENT_MONSTER)
			Monster = entity_cast<CMonsterEntity>(other);

		if (Monster && (!Monster->MoveTarget || Monster->MoveTarget != this))
			return;

		if (Target)
		{
			if (other->EntityFlags & ENT_USABLE)
			{
				CUsableEntity *Usable = entity_cast<CUsableEntity>(other);
				Usable->Target = Target;

				if (Monster)
					Monster->GoalEntity = Monster->MoveTarget = CC_PickTarget(Usable->Target);
			}
			if (Monster && !Monster->GoalEntity)
			{
				DebugPrintf("%s at (%f %f %f) target %s does not exist\n", gameEntity->classname, State.GetOrigin().X, State.GetOrigin().Y, State.GetOrigin().Z, Target);
				Monster->MoveTarget = this;
			}
			Target = NULL;
		}
		else if ((SpawnFlags & 1) && !(other->Flags & (FL_SWIM|FL_FLY)))
		{
			if (Monster)
			{
				Monster->Monster->PauseTime = level.framenum + 100000000;
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

		if (gameEntity->pathtarget)
		{
			char *savetarget;
			CBaseEntity *activator;

			savetarget = Target;
			Target = gameEntity->pathtarget;
			if (other->Enemy && (other->Enemy->EntityFlags & ENT_PLAYER))
				activator = other->Enemy;
			else if ((Monster) &&
				(Monster->OldEnemy) && (Monster->OldEnemy->EntityFlags & ENT_PLAYER))
				activator = Monster->OldEnemy;
			else if ((other->EntityFlags & ENT_USABLE) && (entity_cast<CUsableEntity>(other)->Activator) && ((entity_cast<CUsableEntity>(other)->Activator)->EntityFlags & ENT_PLAYER))
				activator = (entity_cast<CUsableEntity>(other)->Activator);
			else
				activator = other;
			UseTargets (activator, Message);
			Target = savetarget;
		}
	};

	void Spawn ()
	{
		if (game.mode & GAME_DEATHMATCH)
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
	CInfoNull (int Index) :
	  CBaseEntity (Index),
	  CMapEntity (Index)
	  {
	  };

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
	CFuncGroup (int Index) :
	  CBaseEntity (Index),
	  CInfoNull (Index)
	  {
	  };
};

LINK_CLASSNAME_TO_CLASS ("func_group", CFuncGroup);

/*QUAKED info_notnull (0 0.5 0) (-4 -4 -4) (4 4 4)
Used as a positional target for lightning.
*/
class CInfoNotNull : public CMapEntity
{
public:
	CInfoNotNull (int Index) :
	  CBaseEntity (Index),
	  CMapEntity (Index)
	  {
	  };

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
	CLight (int Index) :
	  CBaseEntity (Index),
	  CMapEntity (Index),
	  CUsableEntity (Index)
	  {
	  };

	virtual bool ParseField (char *Key, char *Value)
	{
		return (CUsableEntity::ParseField (Key, Value) || CMapEntity::ParseField (Key, Value));
	}

	void Use (CBaseEntity *other, CBaseEntity *activator)
	{
		if (!Usable)
			return;

		if (SpawnFlags & START_OFF)
		{
			ConfigString (CS_LIGHTS+gameEntity->style, "m");
			SpawnFlags &= ~START_OFF;
		}
		else
		{
			ConfigString (CS_LIGHTS+gameEntity->style, "a");
			SpawnFlags |= START_OFF;
		}
	};

	void Spawn ()
	{
		// no targeted lights in deathmatch, because they cause global messages
		if (!TargetName || (game.mode & GAME_DEATHMATCH))
		{
			Free ();
			return;
		}

		if (gameEntity->style >= 32)
		{
			Usable = true;
			ConfigString (CS_LIGHTS+gameEntity->style, (SpawnFlags & START_OFF) ? "a" : "m");
		}
	};
};

LINK_CLASSNAME_TO_CLASS ("light", CLight);

/*QUAKED target_lightramp (0 .5 .8) (-8 -8 -8) (8 8 8) TOGGLE
speed		How many seconds the ramping will take
message		two letters; starting lightlevel and ending lightlevel
*/
class CTargetLightRamp : public CMapEntity, public CThinkableEntity, public CUsableEntity
{
public:
	int32			RampMessage[3];
	FrameNumber_t	TimeStamp;
	CLight			*Light;
	float			Speed;

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

	CTargetLightRamp (int Index) :
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

	bool Run ()
	{
		return CBaseEntity::Run();
	};

	void Think ()
	{
		char	style[2] = {'a' + RampMessage[0] + (level.framenum - TimeStamp) / 0.1f * RampMessage[2], 0};
		ConfigString (CS_LIGHTS+Light->gameEntity->style, style);

		if ((level.framenum - TimeStamp) < Speed)
			NextThink = level.framenum + FRAMETIME;
		else if (SpawnFlags & 1)
		{
			int32 temp = RampMessage[0];
			RampMessage[0] = RampMessage[1];
			RampMessage[1] = temp;
			RampMessage[2] *= -1;
		}
	};

	void Use (CBaseEntity *other, CBaseEntity *activator)
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
				if (strcmp(e->gameEntity->classname, "light") != 0)
					MapPrint (MAPPRINT_WARNING, this, State.GetOrigin(), "Target \"%s\" is not a light\n", Target);
				else
					Light = entity_cast<CLight>(e);
			}

			if (!Light)
			{
				//gi.dprintf("%s target %s not found at (%f %f %f)\n", self->classname, self->target, self->state.origin[0], self->state.origin[1], self->state.origin[2]);
				MapPrint (MAPPRINT_ERROR, this, State.GetOrigin(), "Target \"%s\" not found\n", Target);
				Free ();
				return;
			}
		}

		TimeStamp = level.framenum;
		Think ();
	};

	void Spawn ()
	{
		if (!Message || strlen(Message) != 2 || Message[0] < 'a' || Message[0] > 'z' || Message[1] < 'a' || Message[1] > 'z' || Message[0] == Message[1])
		{
			//gi.dprintf("target_lightramp has bad ramp (%s) at (%f %f %f)\n", self->message, self->state.origin[0], self->state.origin[1], self->state.origin[2]);
			MapPrint (MAPPRINT_ERROR, this, State.GetOrigin(), "Bad ramp (%s)\n", Message);
			Free ();
			return;
		}

		if (game.mode & GAME_DEATHMATCH)
		{
			Free ();
			return;
		}

		if (!Target)
		{
			//gi.dprintf("%s with no target at (%f %f %f)\n", self->classname, self->state.origin[0], self->state.origin[1], self->state.origin[2]);
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
	CEntityField ("speed", EntityMemberOffset(CTargetLightRamp,Speed), FT_FLOAT),
};
ENTITYFIELDS_END(CTargetLightRamp)

bool			CTargetLightRamp::ParseField (char *Key, char *Value)
{
	if (CheckFields<CTargetLightRamp> (this, Key, Value))
		return true;

	// Couldn't find it here
	return (CUsableEntity::ParseField (Key, Value) || CMapEntity::ParseField (Key, Value));
};

LINK_CLASSNAME_TO_CLASS ("target_lightramp", CTargetLightRamp);
