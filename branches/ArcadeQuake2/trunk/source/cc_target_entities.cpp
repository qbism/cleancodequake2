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
#include "cc_tent.h"

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
Multiple identical looping sounds will just increase volume without any speed cost
*/

#define SPEAKER_LOOPED_ON		1
#define SPEAKER_LOOPED_OFF		2
#define SPEAKER_RELIABLE		4

class CTargetSpeaker : public CMapEntity, public CUsableEntity
{
public:
	uint8		Volume;
	sint32		Attenuation;

	CTargetSpeaker () :
	  CBaseEntity (),
	  CMapEntity (),
	  CUsableEntity (),
	  Volume(0),
	  Attenuation(0)
	  {
	  };

	CTargetSpeaker (sint32 Index) :
	  CBaseEntity (Index),
	  CMapEntity (Index),
	  CUsableEntity (Index),
	  Volume(0),
	  Attenuation(0)
	  {
	  };

	ENTITYFIELD_DEFS
	ENTITYFIELDS_SAVABLE(CTargetSpeaker)

	void Use (CBaseEntity *other, CBaseEntity *activator)
	{
		if (SpawnFlags & (SPEAKER_LOOPED_ON|SPEAKER_LOOPED_OFF)) // looping sound toggles
			State.GetSound() = (State.GetSound() ? 0 : NoiseIndex); // start or stop it
		else
			// use a positioned_sound, because this entity won't normally be
			// sent to any clients because it is invisible
			PlayPositionedSound (State.GetOrigin(), (SpawnFlags & SPEAKER_RELIABLE) ? CHAN_VOICE|CHAN_RELIABLE : CHAN_VOICE, NoiseIndex, Volume, Attenuation);
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
			Volume = 255;

		switch (Attenuation)
		{
		case 0:
			Attenuation = 1;
			break;
		case -1: // use -1 so 0 defaults to 1
			Attenuation = 0;
			break;
		};

		// check for prestarted looping sound
		if (SpawnFlags & SPEAKER_LOOPED_ON)
			State.GetSound() = NoiseIndex;

		// must link the entity so we get areas and clusters so
		// the server can determine who to send updates to
		Link ();
	};
};

ENTITYFIELDS_BEGIN(CTargetSpeaker)
{
	CEntityField ("volume", EntityMemberOffset(CTargetSpeaker,Volume), FT_FLOAT | FT_SAVABLE),
	CEntityField ("attenuation", EntityMemberOffset(CTargetSpeaker,Attenuation), FT_INT | FT_SAVABLE)
};
ENTITYFIELDS_END(CTargetSpeaker)

bool			CTargetSpeaker::ParseField (const char *Key, const char *Value)
{
	if (CheckFields<CTargetSpeaker> (this, Key, Value))
		return true;

	return (CUsableEntity::ParseField (Key, Value) || CMapEntity::ParseField (Key, Value));
};

void		CTargetSpeaker::SaveFields (CFile &File)
{
	SaveEntityFields <CTargetSpeaker> (this, File);
	CMapEntity::SaveFields (File);
	CUsableEntity::SaveFields (File);
}

void		CTargetSpeaker::LoadFields (CFile &File)
{
	LoadEntityFields <CTargetSpeaker> (this, File);
	CMapEntity::LoadFields (File);
	CUsableEntity::LoadFields (File);
}

LINK_CLASSNAME_TO_CLASS ("target_speaker", CTargetSpeaker);

class CTargetExplosion : public CMapEntity, public CThinkableEntity, public CUsableEntity
{
public:
	sint32			Damage;

	CTargetExplosion () :
	  CBaseEntity (),
	  CMapEntity (),
	  CThinkableEntity (),
	  CUsableEntity (),
	  Damage(0)
	{
	};

	CTargetExplosion (sint32 Index) :
	  CBaseEntity (Index),
	  CMapEntity (Index),
	  CThinkableEntity (Index),
	  CUsableEntity (Index),
	  Damage(0)
	{
	};

	ENTITYFIELD_DEFS
	ENTITYFIELDS_SAVABLE(CTargetExplosion)

	bool Run ()
	{
		return CBaseEntity::Run();
	};

	void Think ()
	{
		CTempEnt_Explosions::RocketExplosion (State.GetOrigin(), this);

		if (Damage)
			SplashDamage (Activator, Damage, NULL, Damage+40, MOD_EXPLOSIVE);

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

		NextThink = level.Frame + Delay;
	};

	void Spawn ()
	{
		GetSvFlags() = SVF_NOCLIENT;
	};
};

ENTITYFIELDS_BEGIN(CTargetExplosion)
{
	CEntityField ("dmg", EntityMemberOffset(CTargetExplosion,Damage), FT_INT | FT_SAVABLE),
};
ENTITYFIELDS_END(CTargetExplosion)

bool			CTargetExplosion::ParseField (const char *Key, const char *Value)
{
	if (CheckFields<CTargetExplosion> (this, Key, Value))
		return true;

	return (CUsableEntity::ParseField (Key, Value) || CMapEntity::ParseField (Key, Value));
};

void		CTargetExplosion::SaveFields (CFile &File)
{
	SaveEntityFields <CTargetExplosion> (this, File);
	CMapEntity::SaveFields (File);
	CUsableEntity::SaveFields (File);
}

void		CTargetExplosion::LoadFields (CFile &File)
{
	LoadEntityFields <CTargetExplosion> (this, File);
	CMapEntity::LoadFields (File);
	CUsableEntity::LoadFields (File);
}

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

	CTargetSpawner (sint32 Index) :
	  CBaseEntity (Index),
	  CMapEntity (Index),
	  CUsableEntity (Index),
	  Speed (0)
	{
	};

	ENTITYFIELD_DEFS
	ENTITYFIELDS_SAVABLE(CTargetSpawner)

	bool Run ()
	{
		return CBaseEntity::Run();
	};

	void Use (CBaseEntity *other, CBaseEntity *activator)
	{
		CBaseEntity *Entity = CreateEntityFromClassname(Target);

		if (!Entity)
			return;

		Entity->State.GetOrigin() = State.GetOrigin();
		Entity->State.GetAngles() = State.GetAngles();

		Entity->Unlink ();
		Entity->KillBox ();
		Entity->Link ();

		if (Speed && (Entity->EntityFlags & ENT_PHYSICS))
			entity_cast<CPhysicsEntity>(Entity)->Velocity = MoveDir;
	};

	void Spawn ()
	{
		GetSvFlags() = SVF_NOCLIENT;
		if (Speed)
		{
			G_SetMovedir (State.GetAngles(), MoveDir);
			MoveDir *= Speed;
		}
	};
};

ENTITYFIELDS_BEGIN(CTargetSpawner)
{
	CEntityField ("speed", EntityMemberOffset(CTargetSpawner,Speed), FT_FLOAT | FT_SAVABLE),

	CEntityField ("MoveDir", EntityMemberOffset(CTargetSpawner,MoveDir), FT_VECTOR | FT_NOSPAWN | FT_SAVABLE),
};
ENTITYFIELDS_END(CTargetSpawner)

bool			CTargetSpawner::ParseField (const char *Key, const char *Value)
{
	if (CheckFields<CTargetSpawner> (this, Key, Value))
		return true;

	// Couldn't find it here
	return (CUsableEntity::ParseField (Key, Value) || CMapEntity::ParseField (Key, Value));
};

void		CTargetSpawner::SaveFields (CFile &File)
{
	SaveEntityFields <CTargetSpawner> (this, File);
	CMapEntity::SaveFields (File);
	CUsableEntity::SaveFields (File);
}

void		CTargetSpawner::LoadFields (CFile &File)
{
	LoadEntityFields <CTargetSpawner> (this, File);
	CMapEntity::LoadFields (File);
	CUsableEntity::LoadFields (File);
}

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
	sint32	Damage;
	uint8	Color;
	uint8	Count;

	CTargetSplash () :
	  CBaseEntity (),
	  CMapEntity (),
	  CUsableEntity (),
	  Damage (0),
	  Count (0)
	{
	};

	CTargetSplash (sint32 Index) :
	  CBaseEntity (Index),
	  CMapEntity (Index),
	  CUsableEntity (Index),
	  Damage (0),
	  Count (0)
	{
	};

	ENTITYFIELD_DEFS
	ENTITYFIELDS_SAVABLE(CTargetSplash)

	bool Run ()
	{
		return CBaseEntity::Run();
	};

	void Use (CBaseEntity *other, CBaseEntity *activator)
	{
		CTempEnt_Splashes::Splash (State.GetOrigin(), MoveDir, Color, Count);

		if (Damage)
			SplashDamage (activator, Damage, NULL, Damage+40, MOD_SPLASH);
	};

	void Spawn ()
	{
		G_SetMovedir (State.GetAngles(), MoveDir);

		if (!Count)
			Count = 32;

		GetSvFlags() = SVF_NOCLIENT;
	};
};

ENTITYFIELDS_BEGIN(CTargetSplash)
{
	CEntityField ("dmg", EntityMemberOffset(CTargetSplash,Damage), FT_INT | FT_SAVABLE),
	CEntityField ("sounds", EntityMemberOffset(CTargetSplash,Color), FT_BYTE | FT_SAVABLE),
	CEntityField ("count", EntityMemberOffset(CTargetSplash,Count), FT_BYTE | FT_SAVABLE),

	CEntityField ("MoveDir", EntityMemberOffset(CTargetSplash,MoveDir), FT_VECTOR | FT_NOSPAWN | FT_SAVABLE),
};
ENTITYFIELDS_END(CTargetSplash)

bool			CTargetSplash::ParseField (const char *Key, const char *Value)
{
	if (CheckFields<CTargetSplash> (this, Key, Value))
		return true;

	// Couldn't find it here
	return (CUsableEntity::ParseField (Key, Value) || CMapEntity::ParseField (Key, Value));
};

void		CTargetSplash::SaveFields (CFile &File)
{
	SaveEntityFields <CTargetSplash> (this, File);
	CMapEntity::SaveFields (File);
	CUsableEntity::SaveFields (File);
}

void		CTargetSplash::LoadFields (CFile &File)
{
	LoadEntityFields <CTargetSplash> (this, File);
	CMapEntity::LoadFields (File);
	CUsableEntity::LoadFields (File);
}

LINK_CLASSNAME_TO_CLASS ("target_splash", CTargetSplash);

/*QUAKED target_temp_entity (1 0 0) (-8 -8 -8) (8 8 8)
Fire an origin based temp entity event to the clients.
"style"		type uint8
*/
class CTargetTempEntity : public CMapEntity, public CUsableEntity
{
public:
	uint8		Style;

	CTargetTempEntity () :
	  CBaseEntity (),
	  CMapEntity (),
	  CUsableEntity ()
	{
	};

	CTargetTempEntity (sint32 Index) :
	  CBaseEntity (Index),
	  CMapEntity (Index),
	  CUsableEntity (Index)
	{
	};

	ENTITYFIELD_DEFS
	ENTITYFIELDS_SAVABLE(CTargetTempEntity)

	bool Run ()
	{
		return CBaseEntity::Run();
	};

	void Use (CBaseEntity *other, CBaseEntity *activator)
	{
		WriteByte (SVC_TEMP_ENTITY);
		WriteByte (Style);
		WritePosition (State.GetOrigin());
		Cast (CASTFLAG_PVS, State.GetOrigin());
	};

	void Spawn ()
	{
	};
};

ENTITYFIELDS_BEGIN(CTargetTempEntity)
{
	CEntityField ("style", EntityMemberOffset(CTargetTempEntity,Style), FT_BYTE | FT_SAVABLE),
};
ENTITYFIELDS_END(CTargetTempEntity)

bool CTargetTempEntity::ParseField (const char *Key, const char *Value)
{
	if (CheckFields<CTargetTempEntity> (this, Key, Value))
		return true;

	return (CUsableEntity::ParseField (Key, Value) || CMapEntity::ParseField (Key, Value));
}

void		CTargetTempEntity::SaveFields (CFile &File)
{
	SaveEntityFields <CTargetTempEntity> (this, File);
	CMapEntity::SaveFields (File);
	CUsableEntity::SaveFields (File);
}

void		CTargetTempEntity::LoadFields (CFile &File)
{
	LoadEntityFields <CTargetTempEntity> (this, File);
	CMapEntity::LoadFields (File);
	CUsableEntity::LoadFields (File);
}

LINK_CLASSNAME_TO_CLASS ("target_temp_entity", CTargetTempEntity);

/*QUAKED target_changelevel (1 0 0) (-8 -8 -8) (8 8 8)
Changes level to "map" when fired
*/
void BeginIntermission (CTargetChangeLevel *targ)
{
	CBaseEntity	*ent;

	if (level.IntermissionTime)
		return;		// already activated

#if CLEANCTF_ENABLED
//ZOID
	if (game.GameMode & GAME_CTF)
		CTFCalcScores();
//ZOID
#endif

	game.AutoSaved = false;

	// respawn any dead clients
	for (sint32 i = 0; i < game.MaxClients; i++)
	{
		CPlayerEntity *client = entity_cast<CPlayerEntity>((g_edicts + 1 + i)->Entity);
		if (!client->GetInUse())
			continue;
		if (client->Health <= 0)
			client->Respawn();
	}

	level.IntermissionTime = level.Frame;
	level.ChangeMap = targ->Map;

	if (strstr(level.ChangeMap, "*"))
	{
		if (game.GameMode == GAME_COOPERATIVE)
		{
			for (sint32 i = 0; i < game.MaxClients; i++)
			{
				CPlayerEntity *client = entity_cast<CPlayerEntity>((g_edicts + 1 + i)->Entity);
				if (!client->GetInUse())
					continue;
				// strip players of all keys between units
				for (uint16 n = 0; n < MAX_CS_ITEMS; n++)
				{
					if (n >= GetNumItems())
						break;
					if (GetItemByIndex(n)->Flags & ITEMFLAG_KEY)
						client->Client.Persistent.Inventory.Set(n, 0);
				}
			}
		}
	}
	else
	{
		if (!(game.GameMode & GAME_DEATHMATCH))
		{
			level.ExitIntermission = true;		// go immediately to the next level
			if (targ->ExitOnNextFrame)
				level.ExitIntermissionOnNextFrame = true;
			return;
		}
	}

	level.ExitIntermission = false;

	// find an intermission spot
	ent = CC_Find<CBaseEntity, ENT_BASE, EntityMemberOffset(CBaseEntity,ClassName)> (NULL, "info_player_intermission");
	if (!ent)
	{	// the map creator forgot to put in an intermission point...
		ent = CC_Find<CBaseEntity, ENT_BASE, EntityMemberOffset(CBaseEntity,ClassName)> (NULL, "info_player_start");
		if (!ent)
			ent = CC_Find<CBaseEntity, ENT_BASE, EntityMemberOffset(CBaseEntity,ClassName)> (NULL, "info_player_deathmatch");
	}
	else
	{	// chose one of four spots
		sint32 i = irandom(4);
		while (i--)
		{
			ent = CC_Find<CBaseEntity, ENT_BASE, EntityMemberOffset(CBaseEntity,ClassName)> (ent, "info_player_intermission");
			if (!ent)	// wrap around the list
				ent = CC_Find<CBaseEntity, ENT_BASE, EntityMemberOffset(CBaseEntity,ClassName)> (ent, "info_player_intermission");
		}
	}

	level.IntermissionOrigin = ent->State.GetOrigin ();
	level.IntermissionAngles = ent->State.GetAngles ();

	// move all clients to the intermission point
	for (sint32 i = 0; i < game.MaxClients; i++)
	{
		CPlayerEntity *client = entity_cast<CPlayerEntity>((g_edicts + 1 + i)->Entity);
		if (!client->GetInUse())
			continue;
		client->MoveToIntermission();
	}
}

CTargetChangeLevel::CTargetChangeLevel () :
	CBaseEntity (),
	CMapEntity (),
	CUsableEntity (),
	Map(NULL),
	ExitOnNextFrame(false)
{
};

CTargetChangeLevel::CTargetChangeLevel (sint32 Index) :
	CBaseEntity (Index),
	CMapEntity (Index),
	CUsableEntity (Index),
	Map(NULL),
	ExitOnNextFrame(false)
{
};

bool CTargetChangeLevel::Run ()
{
	return CBaseEntity::Run ();
};

void CTargetChangeLevel::Use (CBaseEntity *other, CBaseEntity *activator)
{
	if (level.IntermissionTime)
		return;		// already activated

	if (game.GameMode == GAME_SINGLEPLAYER)
	{
		if (entity_cast<CPlayerEntity>(g_edicts[1].Entity)->Health <= 0)
			return;
	}

	// if noexit, do a ton of damage to other
	if ((game.GameMode & GAME_DEATHMATCH) && !dmFlags.dfAllowExit.IsEnabled() && (other != World))
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
	if (game.GameMode & GAME_DEATHMATCH)
	{
		if (activator && (activator->EntityFlags & ENT_PLAYER))
		{
			CPlayerEntity *Player = entity_cast<CPlayerEntity>(activator);
			BroadcastPrintf (PRINT_HIGH, "%s exited the level.\n", Player->Client.Persistent.Name.c_str());
		}
	}

	// if going to a new unit, clear cross triggers
	if (strstr(Map, "*"))	
		game.ServerFlags &= ~(SFL_CROSS_TRIGGER_MASK);

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
	if ((Q_stricmp(level.ServerLevelName.c_str(), "fact1") == 0) && (Q_stricmp(Map, "fact3") == 0))
	{
		Map = "fact3$secret1";
		// Paril
		// ...
		ExitOnNextFrame = true;
	}

	GetSvFlags() = SVF_NOCLIENT;
};

ENTITYFIELDS_BEGIN(CTargetChangeLevel)
{
	CEntityField ("map", EntityMemberOffset(CTargetChangeLevel,Map), FT_LEVEL_STRING | FT_SAVABLE),
};
ENTITYFIELDS_END(CTargetChangeLevel)

bool			CTargetChangeLevel::ParseField (const char *Key, const char *Value)
{
	if (CheckFields<CTargetChangeLevel> (this, Key, Value))
		return true;

	// Couldn't find it here
	return (CUsableEntity::ParseField (Key, Value) || CMapEntity::ParseField (Key, Value));
};

void			CTargetChangeLevel::SaveFields (CFile &File)
{
	SaveEntityFields <CTargetChangeLevel> (this, File);
	CMapEntity::SaveFields (File);
	CUsableEntity::SaveFields (File);
}

void			CTargetChangeLevel::LoadFields (CFile &File)
{
	LoadEntityFields <CTargetChangeLevel> (this, File);
	CMapEntity::LoadFields (File);
	CUsableEntity::LoadFields (File);
}

LINK_CLASSNAME_TO_CLASS ("target_changelevel", CTargetChangeLevel);

CTargetChangeLevel *CreateTargetChangeLevel(const char *map)
{
	CTargetChangeLevel *Temp = QNewEntityOf CTargetChangeLevel;
	Temp->ClassName = "target_changelevel";

	level.NextMap = map;
	Temp->Map = (char*)level.NextMap.c_str();

	return Temp;
}

/*QUAKED target_crosslevel_trigger (.5 .5 .5) (-8 -8 -8) (8 8 8) trigger1 trigger2 trigger3 trigger4 trigger5 trigger6 trigger7 trigger8
Once this trigger is touched/used, any trigger_crosslevel_target with the same trigger number is automatically used when a level is started within the same unit.  It is OK to check multiple triggers.  Message, delay, target, and killtarget also work.
*/
class CTargetCrossLevelTrigger : public CMapEntity, public CUsableEntity
{
public:
	CTargetCrossLevelTrigger () :
	  CBaseEntity (),
	  CMapEntity (),
	  CUsableEntity ()
	{
	};

	CTargetCrossLevelTrigger (sint32 Index) :
	  CBaseEntity (Index),
	  CMapEntity (Index),
	  CUsableEntity (Index)
	{
	};

	IMPLEMENT_SAVE_HEADER(CTargetCrossLevelTrigger)

	virtual bool ParseField (const char *Key, const char *Value)
	{
		return (CUsableEntity::ParseField (Key, Value) || CMapEntity::ParseField (Key, Value));
	}

	void SaveFields (CFile &File)
	{
		CMapEntity::SaveFields (File);
		CUsableEntity::SaveFields (File);
	};

	void LoadFields (CFile &File)
	{
		CMapEntity::LoadFields (File);
		CUsableEntity::LoadFields (File);
	};

	bool Run ()
	{
		return CBaseEntity::Run();
	};

	void Use (CBaseEntity *other, CBaseEntity *activator)
	{
		game.ServerFlags |= SpawnFlags;
		Free ();
	};

	void Spawn ()
	{
		GetSvFlags() = SVF_NOCLIENT;
	};
};

LINK_CLASSNAME_TO_CLASS ("target_crosslevel_trigger", CTargetCrossLevelTrigger);

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

	CTargetCrossLevelTarget (sint32 Index) :
	  CBaseEntity (Index),
	  CMapEntity (Index),
	  CThinkableEntity (Index)
	{
	};

	IMPLEMENT_SAVE_HEADER(CTargetCrossLevelTarget)

	virtual bool ParseField (const char *Key, const char *Value)
	{
		return (CUsableEntity::ParseField (Key, Value) || CMapEntity::ParseField (Key, Value));
	}

	void SaveFields (CFile &File)
	{
		CMapEntity::SaveFields (File);
		CUsableEntity::SaveFields (File);
		CThinkableEntity::SaveFields (File);
	}

	void LoadFields (CFile &File);

	void Use (CBaseEntity *, CBaseEntity *)
	{
	};

	bool Run ()
	{
		return CBaseEntity::Run();
	};

	void Think ()
	{
		if (SpawnFlags == (game.ServerFlags & SFL_CROSS_TRIGGER_MASK & SpawnFlags))
		{
			UseTargets (this, Message);
			Free ();
		}
	};

	void Spawn ()
	{
		if (!Delay)
			Delay = 1;
		GetSvFlags() = SVF_NOCLIENT;
		
		// Paril: backwards compatibility
		NextThink = level.Frame + Delay;
	};

	void FireTarget ();
};

LINK_CLASSNAME_TO_CLASS ("target_crosslevel_target", CTargetCrossLevelTarget);

typedef std::vector<CTargetCrossLevelTarget*, std::generic_allocator<CTargetCrossLevelTarget*> > CrossLevelTargetList;

CrossLevelTargetList &GetCrossLevelTargetList ()
{
	static CrossLevelTargetList List;
	return List;
}

void CTargetCrossLevelTarget::LoadFields (CFile &File)
{
	CMapEntity::LoadFields (File);
	CUsableEntity::LoadFields (File);
	CThinkableEntity::LoadFields (File);

	GetCrossLevelTargetList().push_back (this);
}

void CTargetCrossLevelTarget::FireTarget ()
{
	NextThink = level.Frame + Delay;
}

void FireCrossLevelTargets ()
{
	for (CrossLevelTargetList::iterator it = GetCrossLevelTargetList().begin(); it < GetCrossLevelTargetList().end(); ++it)
		(*it)->FireTarget ();

	GetCrossLevelTargetList().clear();
}

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

	CTargetSecret (sint32 Index) :
	  CBaseEntity (Index),
	  CMapEntity (Index),
	  CUsableEntity (Index)
	{
	};

	IMPLEMENT_SAVE_HEADER(CTargetSecret)

	virtual bool ParseField (const char *Key, const char *Value);

	void SaveFields (CFile &File)
	{
		CMapEntity::SaveFields (File);
		CUsableEntity::SaveFields (File);
	};

	void LoadFields (CFile &File)
	{
		CMapEntity::LoadFields (File);
		CUsableEntity::LoadFields (File);
	};

	bool Run ()
	{
		return CBaseEntity::Run();
	};

	void Use (CBaseEntity *other, CBaseEntity *activator)
	{
		PlaySound (CHAN_VOICE, NoiseIndex);

		level.Secrets.Found++;

		UseTargets (activator, Message);
		Free ();
	};

	void Spawn ()
	{
		if (game.GameMode & GAME_DEATHMATCH)
		{	// auto-remove for deathmatch
			Free ();
			return;
		}

		if (!NoiseIndex)
			NoiseIndex = SoundIndex("misc/secret.wav");

		GetSvFlags() = SVF_NOCLIENT;
		level.Secrets.Total++;
		// map bug hack

		if (!Q_stricmp(level.ServerLevelName.c_str(), "mine3") && (State.GetOrigin() == vec3f(280, -2048, -624)))
			//(State.GetOrigin().X == 280 && State.GetOrigin().Y == -2048 && State.GetOrigin().Z == -624))
			Message = "You have found a secret area.";
	};
};

bool			CTargetSecret::ParseField (const char *Key, const char *Value)
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

	CTargetGoal (sint32 Index) :
	  CBaseEntity (Index),
	  CMapEntity (Index),
	  CUsableEntity (Index)
	{
	};

	IMPLEMENT_SAVE_HEADER(CTargetGoal)

	virtual bool ParseField (const char *Key, const char *Value);

	void SaveFields (CFile &File)
	{
		CMapEntity::SaveFields (File);
		CUsableEntity::SaveFields (File);
	};

	void LoadFields (CFile &File)
	{
		CMapEntity::LoadFields (File);
		CUsableEntity::LoadFields (File);
	};

	bool Run ()
	{
		return CBaseEntity::Run();
	};

	void Use (CBaseEntity *other, CBaseEntity *activator)
	{
		PlaySound (CHAN_VOICE, NoiseIndex);

		level.Goals.Found++;

		if (level.Goals.Found == level.Goals.Total)
			ConfigString (CS_CDTRACK, "0");

		UseTargets (activator, Message);
		Free ();
	};

	void Spawn ()
	{
		if (game.GameMode & GAME_DEATHMATCH)
		{	// auto-remove for deathmatch
			Free ();
			return;
		}

		if (!NoiseIndex)
			NoiseIndex = SoundIndex ("misc/secret.wav");

		GetSvFlags() = SVF_NOCLIENT;
		level.Goals.Total++;
	};
};

bool			CTargetGoal::ParseField (const char *Key, const char *Value)
{
	return (CUsableEntity::ParseField (Key, Value) || CMapEntity::ParseField (Key, Value));
}

LINK_CLASSNAME_TO_CLASS ("target_goal", CTargetGoal);

/*QUAKED target_blaster (1 0 0) (-8 -8 -8) (8 8 8) NOTRAIL NOEFFECTS
Fires a blaster bolt in the set direction when triggered.

dmg		default is 15
speed	default is 1000
*/

#define BLASTER_NO_TRAIL		1
#define BLASTER_NO_EFFECTS		2

class CTargetBlaster : public CMapEntity, public CUsableEntity
{
public:
	vec3f		MoveDir;
	float		Speed;
	sint32		Damage;

	CTargetBlaster () :
	  CBaseEntity (),
	  CMapEntity (),
	  CUsableEntity (),
	  Speed (0),
	  Damage (0)
	{
	};

	CTargetBlaster (sint32 Index) :
	  CBaseEntity (Index),
	  CMapEntity (Index),
	  CUsableEntity (Index),
	  Speed (0),
	  Damage (0)
	{
	};

	ENTITYFIELD_DEFS
	ENTITYFIELDS_SAVABLE(CTargetBlaster)

	bool Run ()
	{
		return CBaseEntity::Run();
	};

	void Use (CBaseEntity *other, CBaseEntity *activator)
	{
		CBlasterProjectile::Spawn (this, State.GetOrigin(), MoveDir, Damage, Speed, (SpawnFlags & BLASTER_NO_EFFECTS) ? 0 : ((SpawnFlags & BLASTER_NO_TRAIL) ? EF_HYPERBLASTER : EF_BLASTER), true);
		PlaySound (CHAN_VOICE, NoiseIndex);
	};

	void Spawn ()
	{
		G_SetMovedir (State.GetAngles(), MoveDir);
		NoiseIndex = SoundIndex ("weapons/laser2.wav");

		if (!Damage)
			Damage = 15;
		if (!Speed)
			Speed = 1000;

		GetSvFlags() = SVF_NOCLIENT;
	};
};

ENTITYFIELDS_BEGIN(CTargetBlaster)
{
	CEntityField ("speed", EntityMemberOffset(CTargetBlaster,Speed), FT_FLOAT | FT_SAVABLE),
	CEntityField ("dmg", EntityMemberOffset(CTargetBlaster,Damage), FT_INT | FT_SAVABLE),

	CEntityField ("MoveDir", EntityMemberOffset(CTargetBlaster,MoveDir), FT_VECTOR | FT_NOSPAWN | FT_SAVABLE),
};
ENTITYFIELDS_END(CTargetBlaster)

bool			CTargetBlaster::ParseField (const char *Key, const char *Value)
{
	if (CheckFields<CTargetBlaster> (this, Key, Value))
		return true;

	// Couldn't find it here
	return (CUsableEntity::ParseField (Key, Value) || CMapEntity::ParseField (Key, Value));
};

void		CTargetBlaster::SaveFields (CFile &File)
{
	SaveEntityFields <CTargetBlaster> (this, File);
	CMapEntity::SaveFields (File);
	CUsableEntity::SaveFields (File);
}

void		CTargetBlaster::LoadFields (CFile &File)
{
	LoadEntityFields <CTargetBlaster> (this, File);
	CMapEntity::LoadFields (File);
	CUsableEntity::LoadFields (File);
}

LINK_CLASSNAME_TO_CLASS ("target_blaster", CTargetBlaster);

/*QUAKED target_laser (0 .5 .8) (-8 -8 -8) (8 8 8) START_ON RED GREEN BLUE YELLOW ORANGE FAT
When triggered, fires a laser.  You can either set a target
or a direction.
*/

#define	LASER_START_ON		1
#define	LASER_RED			2
#define	LASER_GREEN			4
#define	LASER_BLUE			8
#define	LASER_YELLOW		16
#define	LASER_ORANGE		32
#define	LASER_FAT			64

class CTargetLaser : public CMapEntity, public CThinkableEntity, public CUsableEntity
{
public:
	bool		StartLaser;
	vec3f		MoveDir;
	sint32		Damage;
	bool		MakeEffect;

	CTargetLaser () :
	  CBaseEntity (),
	  CMapEntity (),
	  CThinkableEntity (),
	  CUsableEntity (),
	  StartLaser(true),
	  MakeEffect(false),
	  Damage (0)
	{
	};

	CTargetLaser (sint32 Index) :
	  CBaseEntity (Index),
	  CMapEntity (Index),
	  CThinkableEntity (Index),
	  CUsableEntity (Index),
	  StartLaser(true),
	  MakeEffect(false),
	  Damage (0)
	{
	};

	ENTITYFIELD_DEFS
	ENTITYFIELDS_SAVABLE(CTargetLaser)

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

		CBaseEntity	*ignore;
		vec3f	start;
		vec3f	end;
		const uint8 Count = (MakeEffect) ? 8 : 4;

		if (Enemy)
		{
			vec3f last_movedir = MoveDir;
			vec3f point = Enemy->GetAbsMin().MultiplyAngles (0.5f, Enemy->GetSize());

			MoveDir = point - State.GetOrigin();
			MoveDir.Normalize ();
			if (MoveDir != last_movedir)
				MakeEffect = true;
		}

		ignore = this;
		start = State.GetOrigin();
		end = start.MultiplyAngles (2048, MoveDir);
		CTrace tr;
		while(1)
		{
			tr (start, end, ignore, CONTENTS_SOLID|CONTENTS_MONSTER|CONTENTS_DEADMONSTER);

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
				if (MakeEffect)
				{
					MakeEffect = false;
					CTempEnt_Splashes::Sparks (tr.EndPos,
						tr.plane.normal, 
						CTempEnt_Splashes::ST_LASER_SPARKS,
						(State.GetSkinNum() & 255),
						Count);
				}
				break;
			}

			ignore = tr.Ent;
			start = tr.EndPos;
		}

		State.GetOldOrigin() = tr.EndPos;
		NextThink = level.Frame + FRAMETIME;
	};

	void Use (CBaseEntity *other, CBaseEntity *activator)
	{
		if (!Usable)
			return;

		Activator = activator;
		if (SpawnFlags & LASER_START_ON)
			Off ();
		else
			On ();
	};

	void On ()
	{
		if (!Activator)
			Activator = this;
		SpawnFlags |= LASER_START_ON;
		MakeEffect = true;
		GetSvFlags() &= ~SVF_NOCLIENT;
		Think ();
	};
	void Off ()
	{
		SpawnFlags &= ~LASER_START_ON;
		GetSvFlags() |= SVF_NOCLIENT;
		NextThink = 0;
	};
	void Start ()
	{
		GetSolid() = SOLID_NOT;
		State.GetRenderEffects() |= (RF_BEAM|RF_TRANSLUCENT);
		State.GetModelIndex() = 1;			// must be non-zero

		// set the beam diameter
		State.GetFrame() = (SpawnFlags & LASER_FAT) ? 16 : 4;

		// set the color
		if (SpawnFlags & LASER_RED)
			State.GetSkinNum() = Color_RGBAToHex (NSColor::PatriotRed, NSColor::PatriotRed, NSColor::Red, NSColor::Red);
		else if (SpawnFlags & LASER_GREEN)
			State.GetSkinNum() = Color_RGBAToHex (NSColor::Green, NSColor::Lime, NSColor::FireSpeechGreen, NSColor::Harlequin);
		else if (SpawnFlags & LASER_BLUE)
			State.GetSkinNum() = Color_RGBAToHex (NSColor::PatriotBlue, NSColor::PatriotBlue, NSColor::NeonBlue, NSColor::NeonBlue);
		else if (SpawnFlags & LASER_YELLOW)
			State.GetSkinNum() = Color_RGBAToHex (NSColor::ParisDaisy, NSColor::Gorse, NSColor::Lemon, NSColor::Gold);
		else if (SpawnFlags & LASER_ORANGE)
			State.GetSkinNum() = Color_RGBAToHex (NSColor::HarvestGold, NSColor::RobRoy, NSColor::TulipTree, NSColor::FireBush);

		if (!Enemy)
		{
			if (Target)
			{
				CBaseEntity *ent = CC_Find<CMapEntity, ENT_MAP, EntityMemberOffset(CMapEntity,TargetName)> (NULL, Target);
				if (!ent)
					MapPrint (MAPPRINT_WARNING, this, State.GetOrigin(), "\"%s\" is a bad target\n", Target);
				Enemy = ent;
			}
			else
			{
				G_SetMovedir (State.GetAngles(), MoveDir);
			}
		}

		Usable = true;
		StartLaser = false;

		if (!Damage)
			Damage = 1;

		GetMins().Set (-8);
		GetMaxs().Set (8);
		Link ();

		if (SpawnFlags & LASER_START_ON)
			On ();
		else
			Off ();
	};

	void Spawn ()
	{
		Usable = false;

		// let everything else get spawned before we start firing
		NextThink = level.Frame + 10;
	};
};

ENTITYFIELDS_BEGIN(CTargetLaser)
{
	CEntityField ("dmg", EntityMemberOffset(CTargetLaser,Damage), FT_INT | FT_SAVABLE),

	CEntityField ("MoveDir", EntityMemberOffset(CTargetLaser,MoveDir), FT_VECTOR | FT_NOSPAWN | FT_SAVABLE),
	CEntityField ("StartLaser", EntityMemberOffset(CTargetLaser,StartLaser), FT_BOOL | FT_NOSPAWN | FT_SAVABLE),
};
ENTITYFIELDS_END(CTargetLaser)

bool			CTargetLaser::ParseField (const char *Key, const char *Value)
{
	if (CheckFields<CTargetLaser> (this, Key, Value))
		return true;

	// Couldn't find it here
	return (CUsableEntity::ParseField (Key, Value) || CMapEntity::ParseField (Key, Value));
};

void			CTargetLaser::SaveFields (CFile &File)
{
	SaveEntityFields <CTargetLaser> (this, File);
	CMapEntity::SaveFields (File);
	CUsableEntity::SaveFields (File);
	CThinkableEntity::SaveFields (File);
}

void			CTargetLaser::LoadFields (CFile &File)
{
	LoadEntityFields <CTargetLaser> (this, File);
	CMapEntity::LoadFields (File);
	CUsableEntity::LoadFields (File);
	CThinkableEntity::LoadFields (File);
}

LINK_CLASSNAME_TO_CLASS ("target_laser", CTargetLaser);

/*QUAKED target_help (1 0 1) (-16 -16 -24) (16 16 24) help1
When fired, the "message" key becomes the current personal computer string, and the message light will be set on all clients status bars.
*/

#define HELP_FIRST_MESSAGE	1

class CTargetHelp : public CMapEntity, public CUsableEntity
{
public:
	CTargetHelp () :
	  CBaseEntity (),
	  CMapEntity (),
	  CUsableEntity ()
	{
	};

	CTargetHelp (sint32 Index) :
	  CBaseEntity (Index),
	  CMapEntity (Index),
	  CUsableEntity (Index)
	{
	};

	IMPLEMENT_SAVE_HEADER(CTargetHelp)

	virtual bool ParseField (const char *Key, const char *Value)
	{
		return (CUsableEntity::ParseField (Key, Value) || CMapEntity::ParseField (Key, Value));
	}

	void SaveFields (CFile &File)
	{
		CMapEntity::SaveFields (File);
		CUsableEntity::SaveFields (File);
	};

	void LoadFields (CFile &File)
	{
		CMapEntity::LoadFields (File);
		CUsableEntity::LoadFields (File);
	};

	bool Run ()
	{
		return CBaseEntity::Run();
	};

	void Use (CBaseEntity *other, CBaseEntity *activator)
	{
		game.HelpMessages[(SpawnFlags & HELP_FIRST_MESSAGE) ? 0 : 1] = Message;
		game.HelpChanged++;
	};

	void Spawn ()
	{
		if (game.GameMode & GAME_DEATHMATCH)
		{	// auto-remove for deathmatch
			Free ();
			return;
		}

		if (Message.empty())
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
	FrameNumber_t		Duration;

	CTargetEarthquake () :
	  CBaseEntity (),
	  CMapEntity (),
	  CThinkableEntity (),
	  CUsableEntity (),
	  LastShakeTime (0),
	  Speed (0)
	{
	};

	CTargetEarthquake (sint32 Index) :
	  CBaseEntity (Index),
	  CMapEntity (Index),
	  CThinkableEntity (Index),
	  CUsableEntity (Index),
	  LastShakeTime (0),
	  Speed (0)
	{
	};

	ENTITYFIELD_DEFS
	ENTITYFIELDS_SAVABLE(CTargetEarthquake)

	bool Run ()
	{
		return CBaseEntity::Run();
	};

	void Think ()
	{
		if (LastShakeTime < level.Frame)
		{
			PlayPositionedSound (State.GetOrigin(), CHAN_AUTO, NoiseIndex, 255, ATTN_NONE);
			LastShakeTime = level.Frame + 5;
		}

		//for (i=1, e=g_edicts+i; i < globals.numEdicts; i++,e++)
		for (TEntitiesContainer::iterator it = level.Entities.Closed.begin()++; it != level.Entities.Closed.end(); ++it)
		{
			CBaseEntity *Entity = (*it)->Entity;

			if (!Entity || !Entity->GetInUse())
				continue;

			if (!Entity->GroundEntity)
				continue;

			if (!(Entity->EntityFlags & ENT_PLAYER))
				break;

			CPlayerEntity *Player = entity_cast<CPlayerEntity>(Entity);

			Player->GroundEntity = NULL;
			Player->Velocity.X += crand()* 150;
			Player->Velocity.Y += crand()* 150;
			Player->Velocity.Z = Speed * (100.0 / Player->Mass);
		}

		if (level.Frame < TimeStamp)
			NextThink = level.Frame + FRAMETIME;
	};

	void Use (CBaseEntity *other, CBaseEntity *activator)
	{
		// Paril, Backwards compatibility
		TimeStamp = level.Frame + Duration;
		NextThink = level.Frame + FRAMETIME;
		LastShakeTime = 0;
	};

	void Spawn ()
	{
		if (!TargetName)
			MapPrint (MAPPRINT_ERROR, this, State.GetOrigin(), "No targetname\n");

		if (!Duration)
			Duration = 5;

		if (!Speed)
			Speed = 200;

		GetSvFlags() |= SVF_NOCLIENT;

		NoiseIndex = SoundIndex ("world/quake.wav");
	};
};

ENTITYFIELDS_BEGIN(CTargetEarthquake)
{
	// I found this field in biggun.bsp.
	// Supporting it.
	CEntityField ("duration", EntityMemberOffset(CTargetEarthquake,Duration), FT_FRAMENUMBER),

	CEntityField ("speed", EntityMemberOffset(CTargetEarthquake,Speed), FT_FLOAT | FT_SAVABLE),
	CEntityField ("count", EntityMemberOffset(CTargetEarthquake,Duration), FT_FRAMENUMBER | FT_SAVABLE),

	CEntityField ("LastShakeTime", EntityMemberOffset(CTargetEarthquake,LastShakeTime), FT_FRAMENUMBER | FT_NOSPAWN | FT_SAVABLE),
	CEntityField ("TimeStamp", EntityMemberOffset(CTargetEarthquake,TimeStamp), FT_FRAMENUMBER | FT_NOSPAWN | FT_SAVABLE),
};
ENTITYFIELDS_END(CTargetEarthquake)

bool			CTargetEarthquake::ParseField (const char *Key, const char *Value)
{
	if (CheckFields<CTargetEarthquake> (this, Key, Value))
		return true;

	// Couldn't find it here
	return (CUsableEntity::ParseField (Key, Value) || CMapEntity::ParseField (Key, Value));
};

void			CTargetEarthquake::SaveFields (CFile &File)
{
	SaveEntityFields <CTargetEarthquake> (this, File);
	CMapEntity::SaveFields (File);
	CUsableEntity::SaveFields (File);
}

void			CTargetEarthquake::LoadFields (CFile &File)
{
	LoadEntityFields <CTargetEarthquake> (this, File);
	CMapEntity::LoadFields (File);
	CUsableEntity::LoadFields (File);
}

LINK_CLASSNAME_TO_CLASS ("target_earthquake", CTargetEarthquake);