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
// cc_xatrix_entities.cpp
// 
//

#include "cc_local.h"

#if XATRIX_FEATURES
#include "cc_tent.h"

/*QUAKED rotating_light (0 .5 .8) (-8 -8 -8) (8 8 8) START_OFF ALARM
"health"	if set, the light may be killed.
*/

// RAFAEL 
// note to self
// the lights will take damage from explosions
// this could leave a player in total darkness very bad
 
#define ROTATING_LIGHT_START_OFF	1
#define ROTATING_LIGHT_ALARM		2

class CRotatingLight : public CMapEntity, public CHurtableEntity, public CThinkableEntity, public CUsableEntity
{
public:
	bool		DoFree;
	MediaIndex	AlarmSound;

	CRotatingLight () :
	  CMapEntity (),
	  CHurtableEntity (),
	  CThinkableEntity (),
	  CUsableEntity (),
	  DoFree (false),
	  AlarmSound (0)
	{
	};

	CRotatingLight (sint32 Index) :
	  CBaseEntity (Index),
	  CMapEntity (Index),
	  CHurtableEntity (Index),
	  CThinkableEntity (Index),
	  CUsableEntity (Index),
	  DoFree (false),
	  AlarmSound (0)
	{
	};

	IMPLEMENT_SAVE_HEADER(CRotatingLight)

	bool			ParseField (const char *Key, const char *Value)
	{
		return (CMapEntity::ParseField (Key, Value) || CHurtableEntity::ParseField (Key, Value) || CUsableEntity::ParseField (Key, Value));
	};

	void			SaveFields (CFile &File)
	{
		File.Write<bool> (DoFree);
		WriteIndex (File, AlarmSound, INDEX_SOUND);

		CMapEntity::SaveFields (File);
		CHurtableEntity::SaveFields (File);
		CThinkableEntity::SaveFields (File);
		CUsableEntity::SaveFields (File);
	};
	void			LoadFields (CFile &File)
	{
		DoFree = File.Read<bool> ();
		ReadIndex (File, AlarmSound, INDEX_SOUND);

		CMapEntity::LoadFields (File);
		CHurtableEntity::LoadFields (File);
		CThinkableEntity::LoadFields (File);
		CUsableEntity::LoadFields (File);
	};

	void Die (CBaseEntity *Inflictor, CBaseEntity *Attacker, sint32 Damage, vec3f &point)
	{
		CSparks(State.GetOrigin(), vec3fOrigin, ST_WELDING_SPARKS, 0xe0 + irandom(7), 30).Send();

		State.GetEffects() &= ~EF_SPINNINGLIGHTS;
		Usable = false;

		DoFree = true;
		NextThink = Level.Frame + 1;
	};

	void Think ()
	{
		if (DoFree)
		{
			Free ();
			return;
		}

		if (SpawnFlags & ROTATING_LIGHT_START_OFF)
			NextThink = 0;	
		else
		{
			PlaySound (CHAN_NO_PHS_ADD+CHAN_VOICE, AlarmSound, 255, ATTN_STATIC, 0);
			NextThink = Level.Frame + 10;
		}
	};

	void Use (CBaseEntity *Other, CBaseEntity *Activator)
	{
		if (SpawnFlags & ROTATING_LIGHT_START_OFF)
		{
			SpawnFlags &= ~ROTATING_LIGHT_START_OFF;
			State.GetEffects() |= EF_SPINNINGLIGHTS;

			if (SpawnFlags & ROTATING_LIGHT_ALARM)
				NextThink = Level.Frame + 1;
		}
		else
		{
			SpawnFlags |= ROTATING_LIGHT_START_OFF;
			State.GetEffects() &= ~EF_SPINNINGLIGHTS;
		}
	};

	void Spawn ()
	{
		GetSolid() = SOLID_BBOX;
		State.GetModelIndex() = ModelIndex ("models/objects/light/tris.md2");	
		State.GetFrame() = 0;
			
		Usable = true;
		
		if (SpawnFlags & ROTATING_LIGHT_START_OFF)
			State.GetEffects() &= ~EF_SPINNINGLIGHTS;
		else
			State.GetEffects() |= EF_SPINNINGLIGHTS;

		if (!Health)
			Health = 10;

		MaxHealth = Health;
		CanTakeDamage = true;
	
		if (SpawnFlags & ROTATING_LIGHT_ALARM)
			AlarmSound = SoundIndex ("misc/alarm.wav");	
		
		Link ();
	};
};

LINK_CLASSNAME_TO_CLASS ("rotating_light", CRotatingLight);

#include "cc_brushmodels.h"

/*QUAKED misc_crashviper (1 .5 0) (-176 -120 -24) (176 120 72) 
This is a large viper about to crash
*/
class CMiscCrashViper : public CTrainBase, public CTouchableEntity
{
	bool MyUse;
public:
	CMiscCrashViper() :
		CBaseEntity (),
		CTrainBase(),
		CTouchableEntity(),
		MyUse(true)
	{
	};

	CMiscCrashViper(sint32 Index) :
		CBaseEntity (Index),
		CTrainBase(Index),
		CTouchableEntity(Index),
		MyUse(true)
	{
	};

	IMPLEMENT_SAVE_HEADER(CMiscCrashViper)

	void SaveFields (CFile &File)
	{
		File.Write<bool> (MyUse);
		CTrainBase::SaveFields (File);
		CTouchableEntity::SaveFields (File);
	}

	void LoadFields (CFile &File)
	{
		MyUse = File.Read<bool> ();
		CTrainBase::LoadFields (File);
		CTouchableEntity::LoadFields (File);
	}

	bool Run ()
	{
		return CTrainBase::Run ();
	};

	virtual void Use (CBaseEntity *Other, CBaseEntity *Activator)
	{
		if (MyUse)
		{
			GetSvFlags() &= ~SVF_NOCLIENT;
			MyUse = false;
		}
		CTrainBase::Use (Other, Activator);
	};

	virtual void Spawn ()
	{
		if (!Target)
		{
			MapPrint (MAPPRINT_ERROR, this, State.GetOrigin(), "No targetname\n");
			Free ();
			return;
		}

		if (!Speed)
			Speed = 300;

		PhysicsType = PHYSICS_PUSH;
		Touchable = true;
		GetSolid() = SOLID_NOT;
		State.GetModelIndex() = ModelIndex ("models/ships/bigviper/tris.md2");
		GetMins().Set (-16, -16, 0);
		GetMaxs().Set (16, 16, 32);

		NextThink = Level.Frame + FRAMETIME;
		ThinkType = TRAINTHINK_FIND;
		GetSvFlags() |= SVF_NOCLIENT;
		Accel = Decel = Speed;

		Link ();
	};
};

LINK_CLASSNAME_TO_CLASS ("misc_viper", CMiscCrashViper);

/*QUAKED misc_amb4 (1 0 0) (-16 -16 -16) (16 16 16)
Mal's amb4 loop entity
*/
class CMiscAmb4 : public CMapEntity, public CThinkableEntity
{
public:
	MediaIndex	Amb4Sound;

	CMiscAmb4 () :
		CBaseEntity (),
		CMapEntity (),
		CThinkableEntity ()
	{
	};

	CMiscAmb4 (sint32 Index) :
		CBaseEntity (Index),
		CMapEntity (Index),
		CThinkableEntity (Index)
	{
	};

	IMPLEMENT_SAVE_HEADER(CMiscAmb4)

	void SaveFields (CFile &File)
	{
		WriteIndex (File, Amb4Sound, INDEX_SOUND);
		CMapEntity::SaveFields (File);
		CThinkableEntity::SaveFields (File);
	}

	void LoadFields (CFile &File)
	{
		ReadIndex (File, Amb4Sound, INDEX_SOUND);
		CMapEntity::LoadFields (File);
		CThinkableEntity::LoadFields (File);
	}

	void Think ()
	{
		NextThink = Level.Frame + 27;
		PlaySound (CHAN_VOICE, Amb4Sound, 255, ATTN_NONE);
	}

	void Spawn ()
	{
		NextThink = Level.Frame + 10;
		Amb4Sound = SoundIndex ("world/amb4.wav");
		Link ();
	}
};

LINK_CLASSNAME_TO_CLASS ("misc_amb4", CMiscAmb4);

/*QUAKED misc_nuke (1 0 0) (-16 -16 -16) (16 16 16)
*/
class CMiscNuke : public CMapEntity, public CUsableEntity
{
public:
	CMiscNuke () :
		CBaseEntity (),
		CMapEntity (),
		CUsableEntity ()
	{
	};

	CMiscNuke (sint32 Index) :
		CBaseEntity (Index),
		CMapEntity (Index),
		CUsableEntity (Index)
	{
	};

	IMPLEMENT_SAVE_HEADER(CMiscNuke)

	void SaveFields (CFile &File)
	{
		CMapEntity::SaveFields (File);
		CUsableEntity::SaveFields (File);
	}

	void LoadFields (CFile &File)
	{
		CMapEntity::LoadFields (File);
		CUsableEntity::LoadFields (File);
	}

	void Use (CBaseEntity *Other, CBaseEntity *Activator)
	{
		for (TEntitiesContainer::iterator it = Level.Entities.Closed.begin(); it != Level.Entities.Closed.end(); ++it)
		{
			edict_t *ent = (*it);
			if (!ent->inUse || !ent->Entity)
				continue;

			if (!(ent->Entity->EntityFlags & ENT_HURTABLE))
				continue;

			if (ent->Entity == this)
				continue;

			CHurtableEntity *Hurtable = entity_cast<CHurtableEntity>(ent->Entity);

			if (Hurtable->EntityFlags & ENT_PLAYER)
				Hurtable->TakeDamage (this, this, vec3fOrigin, Hurtable->State.GetOrigin(), vec3fOrigin, 100000, 1, 0, MOD_TRAP);
			else if (Hurtable->EntityFlags & ENT_MONSTER)
				Hurtable->Free ();
		}

		Usable = false;
	}

	void Spawn ()
	{
		Usable = true;
	}
};

LINK_CLASSNAME_TO_CLASS ("misc_nuke", CMiscNuke);

// RAFAEL
/*QUAKED misc_viper_missile (1 0 0) (-8 -8 -8) (8 8 8)
"dmg"	how much boom should the bomb make? the default value is 250
*/

/*QUAKED misc_viper_bomb (1 0 0) (-8 -8 -8) (8 8 8)
"dmg"	how much boom should the bomb make?
*/
class CMiscViperMissile : public CMapEntity,public CUsableEntity
{
public:
	sint32			Damage;

	CMiscViperMissile () :
	  CBaseEntity (),
	  CMapEntity (),
	  CUsableEntity ()
	{
	};

	CMiscViperMissile (sint32 Index) :
	  CBaseEntity (Index),
	  CMapEntity (Index),
	  CUsableEntity (Index)
	{
	};

	ENTITYFIELD_DEFS
	ENTITYFIELDS_SAVABLE(CMiscViperMissile)

	void Use (CBaseEntity *Other, CBaseEntity *Activator)
	{
		vec3f	start, dir;
		vec3f	vec;
				
		CBaseEntity *target = CC_FindByClassName<CBaseEntity, ENT_BASE> (NULL, Target);
		
		vec = target->State.GetOrigin();
		vec.Z += 16;
		
		start = State.GetOrigin();
		dir = (vec - start).GetNormalized();
		
		CRocket::Spawn (World, start, dir, Damage, 500, Damage, Damage+40);
		CMuzzleFlash(State.GetOrigin(), State.GetNumber(), MZ2_CHICK_ROCKET_1, true).Send();
		
		Free ();
	};

	void Spawn ()
	{
		GetSolid() = SOLID_NOT;
		GetMins().Set (-8);
		GetMaxs().Set (8);

		State.GetModelIndex() = ModelIndex ("models/objects/bomb/tris.md2");

		if (!Damage)
			Damage = 1000;

		GetSvFlags() |= SVF_NOCLIENT;
		Link ();
	};
};

ENTITYFIELDS_BEGIN(CMiscViperMissile)
{
	CEntityField ("dmg", EntityMemberOffset(CMiscViperMissile,Damage), FT_INT | FT_SAVABLE)
};
ENTITYFIELDS_END(CMiscViperMissile)

bool			CMiscViperMissile::ParseField (const char *Key, const char *Value)
{
	if (CheckFields<CMiscViperMissile> (this, Key, Value))
		return true;

	// Couldn't find it here
	return (CUsableEntity::ParseField (Key, Value) || CMapEntity::ParseField (Key, Value));
};

void		CMiscViperMissile::SaveFields (CFile &File)
{
	SaveEntityFields <CMiscViperMissile> (this, File);
	CMapEntity::SaveFields (File);
	CUsableEntity::SaveFields (File);
}

void		CMiscViperMissile::LoadFields (CFile &File)
{
	LoadEntityFields <CMiscViperMissile> (this, File);
	CMapEntity::LoadFields (File);
	CUsableEntity::LoadFields (File);
}

LINK_CLASSNAME_TO_CLASS ("misc_viper_missile", CMiscViperMissile);

#include "cc_misc_entities.h"

// RAFAEL 17-APR-98
/*QUAKED misc_transport (1 0 0) (-8 -8 -8) (8 8 8) TRIGGER_SPAWN
Maxx's transport at end of game
*/

class CMiscTransport : public CMiscViper
{
public:
	CMiscTransport () :
	  CBaseEntity (),
	  CMiscViper ()
	  {
	  };

	CMiscTransport (sint32 Index) :
	  CBaseEntity (Index),
	  CMiscViper (Index)
	  {
	  };

	IMPLEMENT_SAVE_HEADER (CMiscTransport);

	bool Run ()
	{
		return CTrainBase::Run ();
	};

	void Spawn ()
	{
		if (!(SpawnFlags & 1))
			SpawnFlags |= 1;

		CMiscViper::Spawn ();
		State.GetModelIndex() = ModelIndex ("models/objects/ship/tris.md2");
	};
};

LINK_CLASSNAME_TO_CLASS ("misc_transport", CMiscTransport);

// RAFAEL 15-APR-98
/*QUAKED target_mal_laser (1 0 0) (-4 -4 -4) (4 4 4) START_ON RED GREEN BLUE YELLOW ORANGE FAT
Mal's laser
*/
#include "cc_target_entities.h"

class CTargetMalLaser : public CTargetLaser
{
public:
	FrameNumber_t		Wait;
	FrameNumber_t		Delay;

	CTargetMalLaser () :
	  CTargetLaser ()
	  {
	  };

	CTargetMalLaser (sint32 Index) :
	  CBaseEntity (Index),
	  CTargetLaser (Index)
	  {
	  };

	ENTITYFIELD_DEFS
	ENTITYFIELDS_SAVABLE (CTargetMalLaser)

	void On ()
	{
		if (!User)
			User = this;
		SpawnFlags |= 0x80000001;
		GetSvFlags() &= ~SVF_NOCLIENT;

		NextThink = Level.Frame + Wait + Delay;
	};

	void Think ()
	{
		CTargetLaser::Think ();
		NextThink = Level.Frame + Wait + 1;
		SpawnFlags |= 0x80000000;
	};
};

ENTITYFIELDS_BEGIN(CTargetMalLaser)
{
	CEntityField ("wait", EntityMemberOffset(CTargetMalLaser,Wait), FT_FRAMENUMBER | FT_SAVABLE),
	CEntityField ("delay", EntityMemberOffset(CTargetMalLaser,Delay), FT_FRAMENUMBER | FT_SAVABLE),
};
ENTITYFIELDS_END(CTargetMalLaser)

bool			CTargetMalLaser::ParseField (const char *Key, const char *Value)
{
	if (CheckFields<CTargetMalLaser> (this, Key, Value))
		return true;

	// Couldn't find it here
	return CTargetLaser::ParseField (Key, Value);
};

void			CTargetMalLaser::SaveFields (CFile &File)
{
	SaveEntityFields <CTargetLaser> (this, File);
	CTargetLaser::SaveFields (File);
}

void			CTargetMalLaser::LoadFields (CFile &File)
{
	LoadEntityFields <CTargetLaser> (this, File);
	CTargetLaser::LoadFields (File);
}

LINK_CLASSNAME_TO_CLASS ("target_mal_laser", CTargetMalLaser);

/*QUAKED func_object_repair (1 .5 0) (-8 -8 -8) (8 8 8) 
object to be repaired.
The default delay is 1 second
"delay" the delay in seconds for spark to occur
*/

class CFuncObjectRepair : public CMapEntity, public CThinkableEntity, public CHurtableEntity, public CUsableEntity
{
public:
	CC_ENUM (uint8, ERepairThinkType)
	{
		THINK_NONE,
		THINK_SPARKS,
		THINK_DEAD,
		THINK_FX
	};

	FrameNumber_t		Delay;
	ERepairThinkType	ThinkType;

	CFuncObjectRepair () :
	  CMapEntity (),
	  CThinkableEntity (),
	  CHurtableEntity (),
	  CUsableEntity ()
	  {
	  };

	CFuncObjectRepair (sint32 Index) :
	  CBaseEntity (Index),
	  CMapEntity (Index),
	  CThinkableEntity (Index),
	  CHurtableEntity (Index),
	  CUsableEntity (Index)
	  {
	  };

	ENTITYFIELD_DEFS
	ENTITYFIELDS_SAVABLE (CFuncObjectRepair)

	bool Run ()
	{
		return CBaseEntity::Run ();
	};

	void RepairFX ()
	{
		NextThink = Level.Frame + Delay;

		if (Health <= 100)
			Health++;
		else
			CSparks(State.GetOrigin(), vec3fOrigin, ST_WELDING_SPARKS, 0xe0 + irandom(7), 10).Send();
	};

	void Dead ()
	{
		UseTargets (this, Message);
		NextThink = Level.Frame + 1;
		ThinkType = THINK_FX;
	};

	void Sparks ()
	{
		if (Health < 0)
		{
			NextThink = Level.Frame + 1;
			ThinkType = THINK_DEAD;
			return;
		}

		NextThink = Level.Frame + Delay;
	
		CSparks(State.GetOrigin(), vec3fOrigin, ST_WELDING_SPARKS, 0xe0 + irandom(7), 10).Send();
	};
	
	void Think ()
	{
		switch (ThinkType)
		{
		case THINK_NONE:
			return;
		case THINK_SPARKS:
			Sparks ();
			break;
		case THINK_DEAD:
			Dead ();
			break;
		case THINK_FX:
			RepairFX ();
			break;
		};
	};

	void Spawn ()
	{
		GetSolid() = SOLID_BBOX;
		GetMins().Set (-8, -8, 8);
		GetMaxs().Set (8, 8, 8);
		NextThink = Level.Frame + FRAMETIME;
		ThinkType = THINK_SPARKS;
		Health = 100;

		if (!Delay)
			Delay = 10;
	};
};

ENTITYFIELDS_BEGIN(CFuncObjectRepair)
{
	CEntityField ("delay", EntityMemberOffset(CFuncObjectRepair,Delay), FT_FRAMENUMBER | FT_SAVABLE)
};
ENTITYFIELDS_END(CFuncObjectRepair)

bool			CFuncObjectRepair::ParseField (const char *Key, const char *Value)
{
	if (CheckFields<CFuncObjectRepair> (this, Key, Value))
		return true;

	// Couldn't find it here
	return (CUsableEntity::ParseField (Key, Value) || CMapEntity::ParseField(Key, Value) || CHurtableEntity::ParseField (Key, Value));
};

void			CFuncObjectRepair::SaveFields (CFile &File)
{
	SaveEntityFields <CFuncObjectRepair> (this, File);
	CMapEntity::SaveFields (File);
	CUsableEntity::SaveFields (File);
	CHurtableEntity::SaveFields (File);
	CThinkableEntity::SaveFields (File);
}

void			CFuncObjectRepair::LoadFields (CFile &File)
{
	LoadEntityFields <CFuncObjectRepair> (this, File);
	CMapEntity::LoadFields (File);
	CUsableEntity::LoadFields (File);
	CHurtableEntity::LoadFields (File);
	CThinkableEntity::LoadFields (File);
}

LINK_CLASSNAME_TO_CLASS ("func_object_repair", CFuncObjectRepair);
#endif
