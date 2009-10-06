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
// cc_target_entities.cpp
// 
//

#include "cc_local.h"
#include "cc_target_entities.h"

/*QUAKED target_speaker (1 0 0) (-8 -8 -8) (8 8 8) looped-on looped-off reliable
"noise"		wav file to play
"attenuation"
-1 = none, send to whole level
1 = normal fighting sounds
2 = idle sound level
3 = ambient sound level
"volume"	0.0 to 1.0

Normal sounds play each time the target is used.  The reliable flag can be set for crucial voiceovers.

Looped sounds are always atten 3 / vol 1, and the use function toggles it on/off.
Multiple identical looping sounds will just increase volume without any speed cost.
*/

class CTargetSpeaker : public CMapEntity, public CUsableEntity
{
public:
	float		Volume;
	int			Attenuation;

	CTargetSpeaker () :
	  CBaseEntity (),
	  CMapEntity (),
	  CUsableEntity (),
	  Volume(0),
	  Attenuation(0)
	  {
	  };

	CTargetSpeaker (int Index) :
	  CBaseEntity (Index),
	  CMapEntity (Index),
	  CUsableEntity (Index),
	  Volume(0),
	  Attenuation(0)
	  {
	  };

	static const CEntityField FieldsForParsing[];
	static const size_t FieldsForParsingSize;
	virtual bool ParseField (char *Key, char *Value);

	void Use (CBaseEntity *other, CBaseEntity *activator)
	{
		if (SpawnFlags & 3) // looping sound toggles
			State.SetSound (State.GetSound() ? 0 : NoiseIndex); // start or stop it
		else
			// use a positioned_sound, because this entity won't normally be
			// sent to any clients because it is invisible
			PlayPositionedSound (State.GetOrigin(), (SpawnFlags & 4) ? CHAN_VOICE|CHAN_RELIABLE : CHAN_VOICE, NoiseIndex, Volume, Attenuation);
	};

	void Spawn ()
	{
		if(!NoiseIndex)
		{
			//gi.dprintf("target_speaker with no noise set at (%f %f %f)\n", ent->state.origin[0], ent->state.origin[1], ent->state.origin[2]);
			MapPrint (MAPPRINT_ERROR, this, State.GetOrigin(), "No or missing noise set\n");
			return;
		}

		if (!Volume)
			Volume = 1.0;

		if (!Attenuation)
			Attenuation = 1;
		else if (Attenuation == -1)	// use -1 so 0 defaults to 1
			Attenuation = 0;

		// check for prestarted looping sound
		if (SpawnFlags & 1)
			State.SetSound (NoiseIndex);

		// must link the entity so we get areas and clusters so
		// the server can determine who to send updates to
		Link ();
	};
};

const CEntityField CTargetSpeaker::FieldsForParsing[] =
{
	CEntityField ("volume", EntityMemberOffset(CTargetSpeaker,Volume), FT_FLOAT),
	CEntityField ("attenuation", EntityMemberOffset(CTargetSpeaker,Attenuation), FT_INT)
};
const size_t CTargetSpeaker::FieldsForParsingSize = FieldSize<CTargetSpeaker>();

bool			CTargetSpeaker::ParseField (char *Key, char *Value)
{
	if (CheckFields<CTargetSpeaker> (this, Key, Value))
		return true;

	return (CUsableEntity::ParseField (Key, Value) || CMapEntity::ParseField (Key, Value));
};

LINK_CLASSNAME_TO_CLASS ("target_speaker", CTargetSpeaker);

class CTargetExplosion : public CMapEntity, public CThinkableEntity, public CUsableEntity
{
public:
	int			Damage;

	CTargetExplosion () :
	  CBaseEntity (),
	  CMapEntity (),
	  CThinkableEntity (),
	  CUsableEntity (),
	  Damage(0)
	{
	};

	CTargetExplosion (int Index) :
	  CBaseEntity (Index),
	  CMapEntity (Index),
	  CThinkableEntity (Index),
	  CUsableEntity (Index),
	  Damage(0)
	{
	};

	static const CEntityField FieldsForParsing[];
	static const size_t FieldsForParsingSize;
	virtual bool ParseField (char *Key, char *Value);

	bool Run ()
	{
		return CBaseEntity::Run();
	};

	void Think ()
	{
		vec3f or = State.GetOrigin();
		CTempEnt_Explosions::RocketExplosion (or, this);

		if (Damage)
			T_RadiusDamage (this, Activator, Damage, NULL, Damage+40, MOD_EXPLOSIVE);

		FrameNumber_t save = Delay;
		Delay = 0;
		UseTargets (Activator, Message);
		Delay = save;
	};

	void Use (CBaseEntity *other, CBaseEntity *activator)
	{
		Activator = activator;

		if (!Delay)
		{
			Think ();
			return;
		}

		NextThink = level.framenum + Delay;
	};

	void Spawn ()
	{
		SetSvFlags (SVF_NOCLIENT);
	};
};

const CEntityField CTargetExplosion::FieldsForParsing[] =
{
	CEntityField ("dmg", EntityMemberOffset(CTargetExplosion,Damage), FT_INT),
};
const size_t CTargetExplosion::FieldsForParsingSize = FieldSize<CTargetExplosion>();

bool			CTargetExplosion::ParseField (char *Key, char *Value)
{
	if (CheckFields<CTargetExplosion> (this, Key, Value))
		return true;

	return (CUsableEntity::ParseField (Key, Value) || CMapEntity::ParseField (Key, Value));
};


LINK_CLASSNAME_TO_CLASS ("target_explosion", CTargetExplosion);

/*QUAKED target_spawner (1 0 0) (-8 -8 -8) (8 8 8)
Set target to the type of entity you want spawned.
Useful for spawning monsters and gibs in the factory levels.

For monsters:
	Set direction to the facing you want it to have.

For gibs:
	Set direction if you want it moving and
	speed how fast it should be moving otherwise it
	will just be dropped
*/
class CTargetSpawner : public CMapEntity, public CUsableEntity
{
public:
	vec3f	MoveDir;
	float	Speed;

	CTargetSpawner () :
	  CBaseEntity (),
	  CMapEntity (),
	  CUsableEntity (),
	  Speed (0)
	{
	};

	CTargetSpawner (int Index) :
	  CBaseEntity (Index),
	  CMapEntity (Index),
	  CUsableEntity (Index),
	  Speed (0)
	{
	};

	static const class CEntityField FieldsForParsing[];
	static const size_t FieldsForParsingSize;
	virtual bool			ParseField (char *Key, char *Value);

	bool Run ()
	{
		return CBaseEntity::Run();
	};

	void Use (CBaseEntity *other, CBaseEntity *activator)
	{
		CBaseEntity *Entity = CreateEntityFromClassname(gameEntity->target);

		if (!Entity)
			return;

		Entity->State.SetOrigin (State.GetOrigin());
		Entity->State.SetAngles (State.GetAngles());

		Entity->Unlink ();
		Entity->KillBox ();
		Entity->Link ();

		if (Speed && (Entity->EntityFlags & ENT_PHYSICS))
			entity_cast<CPhysicsEntity>(Entity)->Velocity = MoveDir;
	};

	void Spawn ()
	{
		SetSvFlags (SVF_NOCLIENT);
		if (Speed)
		{
			vec3f angles = State.GetAngles();
			G_SetMovedir (angles, MoveDir);
			MoveDir *= Speed;
			State.SetAngles (angles);
		}
	};
};

const CEntityField CTargetSpawner::FieldsForParsing[] =
{
	CEntityField ("speed", EntityMemberOffset(CTargetSpawner,Speed), FT_FLOAT),
};
const size_t CTargetSpawner::FieldsForParsingSize = FieldSize<CTargetSpawner>();

bool			CTargetSpawner::ParseField (char *Key, char *Value)
{
	if (CheckFields<CTargetSpawner> (this, Key, Value))
		return true;

	// Couldn't find it here
	return (CUsableEntity::ParseField (Key, Value) || CMapEntity::ParseField (Key, Value));
};

LINK_CLASSNAME_TO_CLASS ("target_spawner", CTargetSpawner);

/*QUAKED target_splash (1 0 0) (-8 -8 -8) (8 8 8)
Creates a particle splash effect when used.

Set "sounds" to one of the following:
  1) sparks
  2) blue water
  3) brown water
  4) slime
  5) lava
  6) blood

"count"	how many pixels in the splash
"dmg"	if set, does a radius damage at this location when it splashes
		useful for lava/sparks
*/
class CTargetSplash : public CMapEntity, public CUsableEntity
{
public:
	vec3f	MoveDir;
	int		Damage;

	CTargetSplash () :
	  CBaseEntity (),
	  CMapEntity (),
	  CUsableEntity (),
	  Damage (0)
	{
	};

	CTargetSplash (int Index) :
	  CBaseEntity (Index),
	  CMapEntity (Index),
	  CUsableEntity (Index),
	  Damage (0)
	{
	};

  	static const CEntityField FieldsForParsing[];
	static const size_t FieldsForParsingSize;
	virtual bool ParseField (char *Key, char *Value);

	bool Run ()
	{
		return CBaseEntity::Run();
	};

	void Use (CBaseEntity *other, CBaseEntity *activator)
	{
		CTempEnt_Splashes::Splash (State.GetOrigin(), MoveDir, gameEntity->sounds, gameEntity->count);

		if (Damage)
			T_RadiusDamage (this, activator, Damage, NULL, Damage+40, MOD_SPLASH);
	};

	void Spawn ()
	{
		vec3f angles = State.GetAngles();
		G_SetMovedir (angles, MoveDir);
		State.SetAngles (angles);

		if (!gameEntity->count)
			gameEntity->count = 32;

		SetSvFlags (SVF_NOCLIENT);
	};
};

const CEntityField CTargetSplash::FieldsForParsing[] =
{
	CEntityField ("dmg", EntityMemberOffset(CTargetSplash,Damage), FT_INT),
};
const size_t CTargetSplash::FieldsForParsingSize = FieldSize<CTargetSplash>();

bool			CTargetSplash::ParseField (char *Key, char *Value)
{
	if (CheckFields<CTargetSplash> (this, Key, Value))
		return true;

	// Couldn't find it here
	return (CUsableEntity::ParseField (Key, Value) || CMapEntity::ParseField (Key, Value));
};

LINK_CLASSNAME_TO_CLASS ("target_splash", CTargetSplash);

/*QUAKED target_temp_entity (1 0 0) (-8 -8 -8) (8 8 8)
Fire an origin based temp entity event to the clients.
"style"		type byte
*/
class CTargetTempEntity : public CMapEntity, public CUsableEntity
{
public:
	CTargetTempEntity () :
	  CBaseEntity (),
	  CMapEntity (),
	  CUsableEntity ()
	{
	};

	CTargetTempEntity (int Index) :
	  CBaseEntity (Index),
	  CMapEntity (Index),
	  CUsableEntity (Index)
	{
	};

	virtual bool ParseField (char *Key, char *Value)
	{
		return (CUsableEntity::ParseField (Key, Value) || CMapEntity::ParseField (Key, Value));
	}

	bool Run ()
	{
		return CBaseEntity::Run();
	};

	void Use (CBaseEntity *other, CBaseEntity *activator)
	{
		WriteByte (SVC_TEMP_ENTITY);
		WriteByte (gameEntity->style);
		WritePosition (State.GetOrigin());
		Cast (CASTFLAG_PVS, State.GetOrigin());
	};

	void Spawn ()
	{
	};
};

LINK_CLASSNAME_TO_CLASS ("target_temp_entity", CTargetTempEntity);

/*QUAKED target_changelevel (1 0 0) (-8 -8 -8) (8 8 8)
Changes level to "map" when fired
*/
void BeginIntermission (CTargetChangeLevel *targ)
{
	CBaseEntity	*ent;

	if (level.intermissiontime)
		return;		// already activated

#ifdef CLEANCTF_ENABLED
//ZOID
	if (game.mode & GAME_CTF)
		CTFCalcScores();
//ZOID
#endif

	game.autosaved = false;

	// respawn any dead clients
	for (int i=0 ; i<game.maxclients ; i++)
	{
		CPlayerEntity *client = entity_cast<CPlayerEntity>((g_edicts + 1 + i)->Entity);
		if (!client->IsInUse())
			continue;
		if (client->Health <= 0)
			client->Respawn();
	}

	level.intermissiontime = level.framenum;
	level.changemap = targ->Map;

	if (strstr(level.changemap, "*"))
	{
		if (game.mode == GAME_COOPERATIVE)
		{
			for (int i=0 ; i<game.maxclients ; i++)
			{
				CPlayerEntity *client = entity_cast<CPlayerEntity>((g_edicts + 1 + i)->Entity);
				if (!client->IsInUse())
					continue;
				// strip players of all keys between units
				for (int n = 0; n < MAX_CS_ITEMS; n++)
				{
					if (n >= GetNumItems())
						break;
					if (GetItemByIndex(n)->Flags & ITEMFLAG_KEY)
						client->Client.pers.Inventory.Set(n, 0);
				}
			}
		}
	}
	else
	{
		if (!(game.mode & GAME_DEATHMATCH))
		{
			level.exitintermission = 1;		// go immediately to the next level
			return;
		}
	}

	level.exitintermission = 0;

	// find an intermission spot
	ent = CC_Find (NULL, FOFS(classname), "info_player_intermission");
	if (!ent)
	{	// the map creator forgot to put in an intermission point...
		ent = CC_Find (NULL, FOFS(classname), "info_player_start");
		if (!ent)
			ent = CC_Find (NULL, FOFS(classname), "info_player_deathmatch");
	}
	else
	{	// chose one of four spots
		int i = irandom(4);
		while (i--)
		{
			ent = CC_Find (ent, FOFS(classname), "info_player_intermission");
			if (!ent)	// wrap around the list
				ent = CC_Find (ent, FOFS(classname), "info_player_intermission");
		}
	}

	level.IntermissionOrigin = ent->State.GetOrigin ();
	level.IntermissionAngles = ent->State.GetAngles ();

	// move all clients to the intermission point
	for (int i=0 ; i<game.maxclients ; i++)
	{
		CPlayerEntity *client = entity_cast<CPlayerEntity>((g_edicts + 1 + i)->Entity);
		if (!client->IsInUse())
			continue;
		client->MoveToIntermission();
	}
}

CTargetChangeLevel::CTargetChangeLevel () :
	CBaseEntity (),
	CMapEntity (),
	CUsableEntity (),
	Map(NULL)
{
};

CTargetChangeLevel::CTargetChangeLevel (int Index) :
	CBaseEntity (Index),
	CMapEntity (Index),
	CUsableEntity (Index),
	Map(NULL)
{
};

bool CTargetChangeLevel::Run ()
{
	return CBaseEntity::Run ();
};

void CTargetChangeLevel::Use (CBaseEntity *other, CBaseEntity *activator)
{
	if (level.intermissiontime)
		return;		// already activated

	if (game.mode == GAME_SINGLEPLAYER)
	{
		if (entity_cast<CPlayerEntity>(g_edicts[1].Entity)->Health <= 0)
			return;
	}

	// if noexit, do a ton of damage to other
	if ((game.mode & GAME_DEATHMATCH) && !dmFlags.dfAllowExit && (other != world->Entity))
	{
		if ((other->EntityFlags & ENT_HURTABLE))
		{
			CHurtableEntity *Other = entity_cast<CHurtableEntity>(other);

			if (Other->CanTakeDamage)
				Other->TakeDamage (this, this, vec3fOrigin, Other->State.GetOrigin(), vec3fOrigin, 10 * Other->MaxHealth, 1000, 0, MOD_EXIT);
		}
		return;
	}

	// if multiplayer, let everyone know who hit the exit
	if (game.mode & GAME_DEATHMATCH)
	{
		if (activator && (activator->EntityFlags & ENT_PLAYER))
		{
			CPlayerEntity *Player = entity_cast<CPlayerEntity>(activator);
			BroadcastPrintf (PRINT_HIGH, "%s exited the level.\n", Player->Client.pers.netname);
		}
	}

	// if going to a new unit, clear cross triggers
	if (strstr(Map, "*"))	
		game.serverflags &= ~(SFL_CROSS_TRIGGER_MASK);

	BeginIntermission (this);
};

void CTargetChangeLevel::Spawn ()
{
	if (!Map)
	{
		//gi.dprintf("target_changelevel with no map at (%f %f %f)\n", ent->state.origin[0], ent->state.origin[1], ent->state.origin[2]);
		MapPrint (MAPPRINT_ERROR, this, State.GetOrigin(), "No map\n");
		Free ();
		return;
	}

	// ugly hack because *SOMEBODY* screwed up their map
   if((Q_stricmp(level.mapname, "fact1") == 0) && (Q_stricmp(Map, "fact3") == 0))
	   Map = "fact3$secret1";

	SetSvFlags (SVF_NOCLIENT);
};

const CEntityField CTargetChangeLevel::FieldsForParsing[] =
{
	CEntityField ("map", EntityMemberOffset(CTargetChangeLevel,Map), FT_LEVEL_STRING),
};
const size_t CTargetChangeLevel::FieldsForParsingSize = FieldSize<CTargetChangeLevel>();

bool			CTargetChangeLevel::ParseField (char *Key, char *Value)
{
	if (CheckFields<CTargetChangeLevel> (this, Key, Value))
		return true;

	// Couldn't find it here
	return (CUsableEntity::ParseField (Key, Value) || CMapEntity::ParseField (Key, Value));
};

LINK_CLASSNAME_TO_CLASS ("target_changelevel", CTargetChangeLevel);

CTargetChangeLevel *CreateTargetChangeLevel(char *map)
{
	CTargetChangeLevel *Temp = QNew (com_levelPool, 0) CTargetChangeLevel;
	Temp->gameEntity->classname = "target_changelevel";

	Q_snprintfz (level.nextmap, sizeof(level.nextmap), "%s", map);
	Temp->Map = level.nextmap;

	return Temp;
}

/*QUAKED target_crosslevel_trigger (.5 .5 .5) (-8 -8 -8) (8 8 8) trigger1 trigger2 trigger3 trigger4 trigger5 trigger6 trigger7 trigger8
Once this trigger is touched/used, any trigger_crosslevel_target with the same trigger number is automatically used when a level is started within the same unit.  It is OK to check multiple triggers.  Message, delay, target, and killtarget also work.
*/
class CCTargetCrossLevelTrigger : public CMapEntity, public CUsableEntity
{
public:
	CCTargetCrossLevelTrigger () :
	  CBaseEntity (),
	  CMapEntity (),
	  CUsableEntity ()
	{
	};

	CCTargetCrossLevelTrigger (int Index) :
	  CBaseEntity (Index),
	  CMapEntity (Index),
	  CUsableEntity (Index)
	{
	};

	virtual bool ParseField (char *Key, char *Value)
	{
		return (CUsableEntity::ParseField (Key, Value) || CMapEntity::ParseField (Key, Value));
	}

	bool Run ()
	{
		return CBaseEntity::Run();
	};

	void Use (CBaseEntity *other, CBaseEntity *activator)
	{
		game.serverflags |= SpawnFlags;
		Free ();
	};

	void Spawn ()
	{
		SetSvFlags (SVF_NOCLIENT);
	};
};

LINK_CLASSNAME_TO_CLASS ("target_crosslevel_trigger", CCTargetCrossLevelTrigger);

/*QUAKED target_crosslevel_target (.5 .5 .5) (-8 -8 -8) (8 8 8) trigger1 trigger2 trigger3 trigger4 trigger5 trigger6 trigger7 trigger8
Triggered by a trigger_crosslevel elsewhere within a unit.  If multiple triggers are checked, all must be true.  Delay, target and
killtarget also work.

"delay"		delay before using targets if the trigger has been activated (default 1)
*/
class CTargetCrossLevelTarget : public CMapEntity, public CThinkableEntity, public CUsableEntity
{
public:
	CTargetCrossLevelTarget () :
	  CBaseEntity (),
	  CMapEntity (),
	  CThinkableEntity ()
	{
	};

	CTargetCrossLevelTarget (int Index) :
	  CBaseEntity (Index),
	  CMapEntity (Index),
	  CThinkableEntity (Index)
	{
	};

	virtual bool ParseField (char *Key, char *Value)
	{
		return (CUsableEntity::ParseField (Key, Value) || CMapEntity::ParseField (Key, Value));
	}

	void Use (CBaseEntity *, CBaseEntity *)
	{
	};

	bool Run ()
	{
		return CBaseEntity::Run();
	};

	void Think ()
	{
		if (SpawnFlags == (game.serverflags & SFL_CROSS_TRIGGER_MASK & SpawnFlags))
		{
			UseTargets (this, Message);
			Free ();
		}
	};

	void Spawn ()
	{
		if (!Delay)
			Delay = 1;
		SetSvFlags (SVF_NOCLIENT);
		
		// Paril: backwards compatibility
		NextThink = level.framenum + Delay;
	};
};

LINK_CLASSNAME_TO_CLASS ("target_crosslevel_target", CTargetCrossLevelTarget);

/*QUAKED target_secret (1 0 1) (-8 -8 -8) (8 8 8)
Counts a secret found.
These are single use targets.
*/
class CTargetSecret : public CMapEntity, public CUsableEntity
{
public:
	CTargetSecret () :
	  CBaseEntity (),
	  CMapEntity (),
	  CUsableEntity ()
	{
	};

	CTargetSecret (int Index) :
	  CBaseEntity (Index),
	  CMapEntity (Index),
	  CUsableEntity (Index)
	{
	};

	virtual bool ParseField (char *Key, char *Value);

	bool Run ()
	{
		return CBaseEntity::Run();
	};

	void Use (CBaseEntity *other, CBaseEntity *activator)
	{
		PlaySound (CHAN_VOICE, NoiseIndex);

		level.found_secrets++;

		UseTargets (activator, Message);
		Free ();
	};

	void Spawn ()
	{
		if (game.mode & GAME_DEATHMATCH)
		{	// auto-remove for deathmatch
			Free ();
			return;
		}

		if (!NoiseIndex)
			NoiseIndex = SoundIndex("misc/secret.wav");

		SetSvFlags (SVF_NOCLIENT);
		level.total_secrets++;
		// map bug hack

		if (!Q_stricmp(level.mapname, "mine3") && (State.GetOrigin() == vec3f(280, -2048, -624)))
			//(State.GetOrigin().X == 280 && State.GetOrigin().Y == -2048 && State.GetOrigin().Z == -624))
			Message = "You have found a secret area.";
	};
};

bool			CTargetSecret::ParseField (char *Key, char *Value)
{
	return (CUsableEntity::ParseField (Key, Value) || CMapEntity::ParseField (Key, Value));
}

LINK_CLASSNAME_TO_CLASS ("target_secret", CTargetSecret);

/*QUAKED target_goal (1 0 1) (-8 -8 -8) (8 8 8)
Counts a goal completed.
These are single use targets.
*/
class CTargetGoal : public CMapEntity, public CUsableEntity
{
public:
	CTargetGoal () :
	  CBaseEntity (),
	  CMapEntity (),
	  CUsableEntity ()
	{
	};

	CTargetGoal (int Index) :
	  CBaseEntity (Index),
	  CMapEntity (Index),
	  CUsableEntity (Index)
	{
	};

	virtual bool ParseField (char *Key, char *Value);

	bool Run ()
	{
		return CBaseEntity::Run();
	};

	void Use (CBaseEntity *other, CBaseEntity *activator)
	{
		PlaySound (CHAN_VOICE, NoiseIndex);

		level.found_goals++;

		if (level.found_goals == level.total_goals)
			ConfigString (CS_CDTRACK, "0");

		UseTargets (activator, Message);
		Free ();
	};

	void Spawn ()
	{
		if (game.mode & GAME_DEATHMATCH)
		{	// auto-remove for deathmatch
			Free ();
			return;
		}

		if (!NoiseIndex)
			NoiseIndex = SoundIndex ("misc/secret.wav");

		SetSvFlags (SVF_NOCLIENT);
		level.total_goals++;
	};
};

bool			CTargetGoal::ParseField (char *Key, char *Value)
{
	return (CUsableEntity::ParseField (Key, Value) || CMapEntity::ParseField (Key, Value));
}

LINK_CLASSNAME_TO_CLASS ("target_goal", CTargetGoal);

/*QUAKED target_blaster (1 0 0) (-8 -8 -8) (8 8 8) NOTRAIL NOEFFECTS
Fires a blaster bolt in the set direction when triggered.

dmg		default is 15
speed	default is 1000
*/
class CTargetBlaster : public CMapEntity, public CUsableEntity
{
public:
	vec3f		MoveDir;
	float		Speed;
	int			Damage;

	CTargetBlaster () :
	  CBaseEntity (),
	  CMapEntity (),
	  CUsableEntity (),
	  Speed (0),
	  Damage (0)
	{
	};

	CTargetBlaster (int Index) :
	  CBaseEntity (Index),
	  CMapEntity (Index),
	  CUsableEntity (Index),
	  Speed (0),
	  Damage (0)
	{
	};

	static const class CEntityField FieldsForParsing[];
	static const size_t FieldsForParsingSize;
	virtual bool			ParseField (char *Key, char *Value);

	bool Run ()
	{
		return CBaseEntity::Run();
	};

	void Use (CBaseEntity *other, CBaseEntity *activator)
	{
		CBlasterProjectile::Spawn (this, State.GetOrigin(), MoveDir, Damage, Speed, (SpawnFlags & 2) ? 0 : ((SpawnFlags & 1) ? EF_HYPERBLASTER : EF_BLASTER), true);
		PlaySound (CHAN_VOICE, NoiseIndex);
	};

	void Spawn ()
	{
		vec3f ang = State.GetAngles();
		G_SetMovedir (ang, MoveDir);
		State.SetAngles (ang);

		NoiseIndex = SoundIndex ("weapons/laser2.wav");

		if (!Damage)
			Damage = 15;
		if (!Speed)
			Speed = 1000;

		SetSvFlags (SVF_NOCLIENT);
	};
};

const CEntityField CTargetBlaster::FieldsForParsing[] =
{
	CEntityField ("speed", EntityMemberOffset(CTargetBlaster,Speed), FT_FLOAT),
	CEntityField ("dmg", EntityMemberOffset(CTargetBlaster,Damage), FT_INT),
};
const size_t CTargetBlaster::FieldsForParsingSize = FieldSize<CTargetBlaster>();

bool			CTargetBlaster::ParseField (char *Key, char *Value)
{
	if (CheckFields<CTargetBlaster> (this, Key, Value))
		return true;

	// Couldn't find it here
	return (CUsableEntity::ParseField (Key, Value) || CMapEntity::ParseField (Key, Value));
};

LINK_CLASSNAME_TO_CLASS ("target_blaster", CTargetBlaster);

#define	START_ON		1
#define	RED				2
#define	GREEN			4
#define	BLUE			8
#define	YELLOW			16
#define	ORANGE			32
#define	FAT				64

/*QUAKED target_laser (0 .5 .8) (-8 -8 -8) (8 8 8) START_ON RED GREEN BLUE YELLOW ORANGE FAT
When triggered, fires a laser.  You can either set a target
or a direction.
*/
class CTargetLaser : public CMapEntity, public CThinkableEntity, public CUsableEntity
{
public:
	bool		StartLaser;
	vec3f		MoveDir;
	int			Damage;

	CTargetLaser () :
	  CBaseEntity (),
	  CMapEntity (),
	  CThinkableEntity (),
	  CUsableEntity (),
	  StartLaser(true),
	  Damage (0)
	{
	};

	CTargetLaser (int Index) :
	  CBaseEntity (Index),
	  CMapEntity (Index),
	  CThinkableEntity (Index),
	  CUsableEntity (Index),
	  StartLaser(true),
	  Damage (0)
	{
	};

  	static const CEntityField FieldsForParsing[];
	static const size_t FieldsForParsingSize;
	virtual bool ParseField (char *Key, char *Value);

	bool Run ()
	{
		return CBaseEntity::Run();
	};

	void Think ()
	{
		if (StartLaser)
		{
			Start ();
			return;
		}

		edict_t	*ignore;
		vec3f	start;
		vec3f	end;

		if (Enemy)
		{
			vec3f last_movedir = MoveDir;
			vec3f point = Enemy->GetAbsMin().MultiplyAngles (0.5f, Enemy->GetSize());

			MoveDir = point - State.GetOrigin();
			MoveDir.Normalize ();
			if (MoveDir != last_movedir)
				SpawnFlags |= 0x80000000;
		}

		ignore = gameEntity;
		start = State.GetOrigin();
		end = start.MultiplyAngles (2048, MoveDir);
		CTrace tr;
		while(1)
		{
			tr = CTrace (start, end, ignore, CONTENTS_SOLID|CONTENTS_MONSTER|CONTENTS_DEADMONSTER);

			if (!tr.ent)
				break;
			if (!tr.ent->Entity)
				break;

			CBaseEntity *Entity = tr.ent->Entity;
			// hurt it if we can
			if (((Entity->EntityFlags & ENT_HURTABLE) && entity_cast<CHurtableEntity>(Entity)->CanTakeDamage) && !(Entity->Flags & FL_IMMUNE_LASER))
				entity_cast<CHurtableEntity>(Entity)->TakeDamage (this, Activator, MoveDir, tr.EndPos, vec3fOrigin, Damage, 1, DAMAGE_ENERGY, MOD_TARGET_LASER);

			// if we hit something that's not a monster or player or is immune to lasers, we're done
			if (!(Entity->EntityFlags & ENT_MONSTER) && (!(Entity->EntityFlags & ENT_PLAYER)))
			{
				if (SpawnFlags & 0x80000000)
				{
					SpawnFlags &= ~0x80000000;
					CTempEnt_Splashes::Sparks (tr.EndPos,
						tr.Plane.normal, 
						CTempEnt_Splashes::ST_LASER_SPARKS,
						(State.GetSkinNum() & 255),
						(SpawnFlags & 0x80000000) ? 8 : 4);
				}
				break;
			}

			ignore = tr.ent;
			start = tr.EndPos;
		}

		State.SetOldOrigin (tr.EndPos);
		NextThink = level.framenum + FRAMETIME;
	};

	void Use (CBaseEntity *other, CBaseEntity *activator)
	{
		if (!Usable)
			return;

		Activator = activator;
		if (SpawnFlags & 1)
			Off ();
		else
			On ();
	};

	void On ()
	{
		if (!Activator)
			Activator = this;
		SpawnFlags |= 0x80000001;
		SetSvFlags (GetSvFlags() & ~SVF_NOCLIENT);
		Think ();
	};
	void Off ()
	{
		SpawnFlags &= ~1;
		SetSvFlags (GetSvFlags() | SVF_NOCLIENT);
		NextThink = 0;
	};
	void Start ()
	{
		SetSolid (SOLID_NOT);
		State.AddRenderEffects (RF_BEAM|RF_TRANSLUCENT);
		State.SetModelIndex (1);			// must be non-zero

		// set the beam diameter
		if (SpawnFlags & FAT)
			State.SetFrame (16);
		else
			State.SetFrame (4);

		// set the color
		if (SpawnFlags & RED)
			State.SetSkinNum (Color_RGBAToHex (NSColor::PatriotRed, NSColor::PatriotRed, NSColor::Red, NSColor::Red));
		else if (SpawnFlags & GREEN)
			State.SetSkinNum (Color_RGBAToHex (NSColor::Green, NSColor::Lime, NSColor::FireSpeechGreen, NSColor::Harlequin));
		else if (SpawnFlags & BLUE)
			State.SetSkinNum (Color_RGBAToHex (NSColor::PatriotBlue, NSColor::PatriotBlue, NSColor::NeonBlue, NSColor::NeonBlue));
		else if (SpawnFlags & YELLOW)
			State.SetSkinNum (Color_RGBAToHex (NSColor::ParisDaisy, NSColor::Gorse, NSColor::Lemon, NSColor::Gold));
		else if (SpawnFlags & ORANGE)
			State.SetSkinNum (Color_RGBAToHex (NSColor::HarvestGold, NSColor::RobRoy, NSColor::TulipTree, NSColor::FireBush));

		if (!Enemy)
		{
			if (gameEntity->target)
			{
				CBaseEntity *ent = CC_Find (NULL, FOFS(targetname), gameEntity->target);
				if (!ent)
					//gi.dprintf ("%s at (%f %f %f): %s is a bad target\n", self->classname, self->state.origin[0], self->state.origin[1], self->state.origin[2], self->target);
					MapPrint (MAPPRINT_WARNING, this, State.GetOrigin(), "\"%s\" is a bad target\n", gameEntity->target);
				Enemy = ent;
			}
			else
			{
				vec3f angles = State.GetAngles ();
				G_SetMovedir (angles, MoveDir);
				State.SetAngles (angles);
			}
		}

		Usable = true;
		StartLaser = false;

		if (!Damage)
			Damage = 1;

		SetMins (vec3f(-8, -8, -8));
		SetMaxs (vec3f(8, 8, 8));
		Link ();

		if (SpawnFlags & START_ON)
			On ();
		else
			Off ();
	};

	void Spawn ()
	{
		Usable = false;

		// let everything else get spawned before we start firing
		NextThink = level.framenum + 10;
	};
};

const CEntityField CTargetLaser::FieldsForParsing[] =
{
	CEntityField ("dmg", EntityMemberOffset(CTargetLaser,Damage), FT_INT),
};
const size_t CTargetLaser::FieldsForParsingSize = FieldSize<CTargetLaser>();

bool			CTargetLaser::ParseField (char *Key, char *Value)
{
	if (CheckFields<CTargetLaser> (this, Key, Value))
		return true;

	// Couldn't find it here
	return (CUsableEntity::ParseField (Key, Value) || CMapEntity::ParseField (Key, Value));
};

LINK_CLASSNAME_TO_CLASS ("target_laser", CTargetLaser);

/*QUAKED target_help (1 0 1) (-16 -16 -24) (16 16 24) help1
When fired, the "message" key becomes the current personal computer string, and the message light will be set on all clients status bars.
*/
class CTargetHelp : public CMapEntity, public CUsableEntity
{
public:
	CTargetHelp () :
	  CBaseEntity (),
	  CMapEntity (),
	  CUsableEntity ()
	{
	};

	CTargetHelp (int Index) :
	  CBaseEntity (Index),
	  CMapEntity (Index),
	  CUsableEntity (Index)
	{
	};

	virtual bool ParseField (char *Key, char *Value)
	{
		return (CUsableEntity::ParseField (Key, Value) || CMapEntity::ParseField (Key, Value));
	}

	bool Run ()
	{
		return CBaseEntity::Run();
	};

	void Use (CBaseEntity *other, CBaseEntity *activator)
	{
		Q_strncpyz ((SpawnFlags & 1) ? game.helpmessage1 : game.helpmessage2, Message, sizeof(game.helpmessage1)-1);
		game.helpchanged++;
	};

	void Spawn ()
	{
		if (game.mode & GAME_DEATHMATCH)
		{	// auto-remove for deathmatch
			Free ();
			return;
		}

		if (!Message)
		{
			MapPrint (MAPPRINT_ERROR, this, State.GetOrigin(), "No message\n");
			Free ();
			return;
		}
	};
};

LINK_CLASSNAME_TO_CLASS ("target_help", CTargetHelp);

//==========================================================

/*QUAKED target_earthquake (1 0 0) (-8 -8 -8) (8 8 8)
When triggered, this initiates a level-wide earthquake.
All players and monsters are affected.
"speed"		severity of the quake (default:200)
"count"		duration of the quake (default:5)
*/
class CTargetEarthquake : public CMapEntity, public CThinkableEntity, public CUsableEntity
{
public:
	FrameNumber_t		LastShakeTime;
	FrameNumber_t		TimeStamp;
	float				Speed;

	CTargetEarthquake () :
	  CBaseEntity (),
	  CMapEntity (),
	  CThinkableEntity (),
	  CUsableEntity (),
	  LastShakeTime (0),
	  Speed (0)
	{
	};

	CTargetEarthquake (int Index) :
	  CBaseEntity (Index),
	  CMapEntity (Index),
	  CThinkableEntity (Index),
	  CUsableEntity (Index),
	  LastShakeTime (0),
	  Speed (0)
	{
	};

	static const class CEntityField FieldsForParsing[];
	static const size_t FieldsForParsingSize;
	virtual bool			ParseField (char *Key, char *Value);

	bool Run ()
	{
		return CBaseEntity::Run();
	};

	void Think ()
	{
		int		i;
		edict_t	*e;

		if (LastShakeTime < level.framenum)
		{
			PlayPositionedSound (State.GetOrigin(), CHAN_AUTO, NoiseIndex, 1.0, ATTN_NONE);
			LastShakeTime = level.framenum + 5;
		}

		for (i=1, e=g_edicts+i; i < globals.numEdicts; i++,e++)
		{
			if (!e->inUse)
				continue;
			if (!e->Entity)
				continue;

			CBaseEntity *Entity = e->Entity;

			if (!Entity->GroundEntity)
				continue;

			if (!(Entity->EntityFlags & ENT_PLAYER))
				continue;

			CPlayerEntity *Player = entity_cast<CPlayerEntity>(Entity);

			Player->GroundEntity = NULL;
			Player->Velocity.X += crandom()* 150;
			Player->Velocity.Y += crandom()* 150;
			Player->Velocity.Z = Speed * (100.0 / Player->Mass);
		}

		if (level.framenum < TimeStamp)
			NextThink = level.framenum + FRAMETIME;
	};

	void Use (CBaseEntity *other, CBaseEntity *activator)
	{
		// Paril, Backwards compatibility
		TimeStamp = level.framenum + (gameEntity->count * 10);
		NextThink = level.framenum + FRAMETIME;
		LastShakeTime = 0;
	};

	void Spawn ()
	{
		if (!gameEntity->targetname)
			MapPrint (MAPPRINT_ERROR, this, State.GetOrigin(), "No targetname\n");
			//gi.dprintf("untargeted %s at (%f %f %f)\n", self->classname, self->state.origin[0], self->state.origin[1], self->state.origin[2]);

		if (!gameEntity->count)
			gameEntity->count = 5;

		if (!Speed)
			Speed = 200;

		SetSvFlags (GetSvFlags() | SVF_NOCLIENT);

		NoiseIndex = SoundIndex ("world/quake.wav");
	};
};

const CEntityField CTargetEarthquake::FieldsForParsing[] =
{
	CEntityField ("speed", EntityMemberOffset(CTargetEarthquake,Speed), FT_FLOAT),
};
const size_t CTargetEarthquake::FieldsForParsingSize = FieldSize<CTargetEarthquake>();

bool			CTargetEarthquake::ParseField (char *Key, char *Value)
{
	if (CheckFields<CTargetEarthquake> (this, Key, Value))
		return true;

	// Couldn't find it here
	return (CUsableEntity::ParseField (Key, Value) || CMapEntity::ParseField (Key, Value));
};

LINK_CLASSNAME_TO_CLASS ("target_earthquake", CTargetEarthquake);