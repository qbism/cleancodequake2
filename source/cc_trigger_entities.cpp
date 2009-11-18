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
// cc_trigger_entities.cpp
// 
//

#include "cc_local.h"
#include "cc_trigger_entities.h"

/*QUAKED trigger_always (.5 .5 .5) (-8 -8 -8) (8 8 8)
This trigger will always fire.  It is activated by the world.
*/
class CTriggerAlways : public CMapEntity, public CUsableEntity
{
public:
	CTriggerAlways () :
	  CBaseEntity (),
	  CMapEntity ()
	{
	};

	CTriggerAlways (sint32 Index) :
	  CBaseEntity (Index),
	  CMapEntity (Index)
	{
	};

	IMPLEMENT_SAVE_HEADER(CTriggerAlways)

	bool ParseField (const char *Key, const char *Value)
	{
		return (CUsableEntity::ParseField (Key, Value) || CMapEntity::ParseField (Key, Value));
	}

	void SaveFields (CFile &File)
	{
		CUsableEntity::SaveFields (File);
	}

	void LoadFields (CFile &File)
	{
		CUsableEntity::LoadFields (File);
	}

	void Use (CBaseEntity *, CBaseEntity *)
	{
	};

	bool Run ()
	{
		return CBaseEntity::Run();
	};

	void Spawn ()
	{
		// we must have some delay to make sure our use targets are present
		if (Delay < 2)
			Delay = 2;

		UseTargets (this, Message);
	};
};

LINK_CLASSNAME_TO_CLASS ("trigger_always", CTriggerAlways);

#include "cc_brushmodels.h"

class CTriggerBase : public CMapEntity, public CBrushModel, public CTouchableEntity, public CUsableEntity
{
public:
	enum
	{
		TRIGGER_THINK_NONE,
		TRIGGER_THINK_FREE,
		TRIGGER_THINK_WAIT,

		TRIGGER_THINK_CUSTOM
	};
	uint32			ThinkType;
	vec3f			MoveDir;
	FrameNumber_t	Wait;
	uint8			Sounds;

	CTriggerBase () :
	  CBaseEntity (),
	  CMapEntity (),
	  CTouchableEntity (),
	  CUsableEntity (),
	  CBrushModel (),
	  ThinkType (TRIGGER_THINK_NONE)
	{
	};

	CTriggerBase (sint32 Index) :
	  CBaseEntity (Index),
	  CMapEntity (Index),
	  CTouchableEntity (Index),
	  CUsableEntity (Index),
	  CBrushModel (Index),
	  ThinkType (TRIGGER_THINK_NONE)
	{
	};

	ENTITYFIELD_VIRTUAL_DEFS
	ENTITYFIELDS_SAVABLE_VIRTUAL(CTriggerBase)

	bool Run ()
	{
		return CBaseEntity::Run();
	};

	virtual void Think ()
	{
		switch (ThinkType)
		{
		case TRIGGER_THINK_WAIT:
			NextThink = 0;
			break;
		case TRIGGER_THINK_FREE:
			Free ();
			break;
		default:
			break;
		};
	};

	virtual void Touch (CBaseEntity *other, plane_t *plane, cmBspSurface_t *surf)
	{
		if (!Touchable)
			return;

		if(other->EntityFlags & ENT_PLAYER)
		{
			if (SpawnFlags & 2)
				return;
		}
		else if (other->EntityFlags & ENT_MONSTER)
		{
			if (!(SpawnFlags & 1))
				return;
		}
		else
			return;

		if (MoveDir != vec3fOrigin)
		{
			vec3f	forward;
			other->State.GetOrigin().ToVectors (&forward, NULL, NULL);
			if (forward.Dot(MoveDir) < 0)
				return;
		}

		Activator = other;
		Trigger ();
	};

	virtual void Use (CBaseEntity *other, CBaseEntity *activator)
	{
		Activator = activator;
		Trigger ();
	};

	void Init ()
	{
		if (State.GetAngles() != vec3fOrigin)
			G_SetMovedir (State.GetAngles(), MoveDir);

		GetSolid() = SOLID_TRIGGER;
		SetBrushModel ();

		GetSvFlags() = SVF_NOCLIENT;
	};

	virtual bool CheckValidity ()
	{
		return CMapEntity::CheckValidity();
	};

	// the trigger was just activated
	// ent->activator should be set to the activator so it can be held through a delay
	// so wait for the delay time before firing
	void Trigger ()
	{
		if (NextThink)
			return;		// already been triggered

		UseTargets (Activator, Message);

		if (Wait > 0)	
		{
			ThinkType = TRIGGER_THINK_WAIT;

			// Paril: backwards compatibility
			NextThink = level.Frame + Wait;
		}
		else
		{	// we can't just remove (self) here, because this is a touch function
			// called while looping through area links...
			Touchable = false;
			NextThink = level.Frame + FRAMETIME;
			ThinkType = TRIGGER_THINK_FREE;
		}
	};

	virtual void Spawn () = 0;
};

ENTITYFIELDS_BEGIN(CTriggerBase)
{
	CEntityField ("wait", EntityMemberOffset(CTriggerBase,Wait), FT_FRAMENUMBER | FT_SAVABLE),
	CEntityField ("sounds", EntityMemberOffset(CTriggerBase,Sounds), FT_BYTE | FT_SAVABLE),
}
ENTITYFIELDS_END(CTriggerBase)

bool			CTriggerBase::ParseField (const char *Key, const char *Value)
{
	if (CheckFields<CTriggerBase> (this, Key, Value))
		return true;

	// Couldn't find it here
	return (CBrushModel::ParseField (Key, Value) || CUsableEntity::ParseField (Key, Value) || CMapEntity::ParseField (Key, Value));
};

void			CTriggerBase::SaveFields (CFile &File)
{
	SaveEntityFields <CTriggerBase> (this, File);
	CUsableEntity::SaveFields (File);
	CBrushModel::SaveFields (File);
	CTouchableEntity::SaveFields (File);
}

void			CTriggerBase::LoadFields (CFile &File)
{
	LoadEntityFields <CTriggerBase> (this, File);
	CUsableEntity::LoadFields (File);
	CBrushModel::LoadFields (File);
	CTouchableEntity::LoadFields (File);
}

/*QUAKED trigger_multiple (.5 .5 .5) ? MONSTER NOT_PLAYER TRIGGERED
Variable sized repeatable trigger.  Must be targeted at one or more entities.
If "delay" is set, the trigger waits some time after activating before firing.
"wait" : Seconds between triggerings. (.2 default)
sounds
1)	secret
2)	beep beep
3)	large switch
4)
set "message" to text string
*/
class CTriggerMultiple : public CTriggerBase
{
public:
	bool ActivateUse;

	CTriggerMultiple () :
	  CBaseEntity (),
	  CTriggerBase (),
	  ActivateUse(false)
	  {
	  };

	CTriggerMultiple (sint32 Index) :
	  CBaseEntity(Index),
	  CTriggerBase (Index),
	  ActivateUse(false)
	  {
	  };

	void Use (CBaseEntity *other, CBaseEntity *activator)
	{
		if (ActivateUse)
		{
			ActivateUse = false;
			GetSolid() = SOLID_TRIGGER;
			Link ();
			return;
		}
		CTriggerBase::Use (other, activator);
	};

	virtual void Spawn ()
	{
		Touchable = true;
		switch (Sounds)
		{
		case 1:
			NoiseIndex = SoundIndex ("misc/secret.wav");
			break;
		case 2:
			NoiseIndex = SoundIndex ("misc/talk.wav");
			break;
		case 3:
			NoiseIndex = SoundIndex ("misc/trigger1.wav");
			break;
		}

		if (!Wait)
			Wait = 2;

		if (SpawnFlags & 4)
		{
			GetSolid() = SOLID_NOT;
			ActivateUse = true;
		}
		else
		{
			GetSolid() = SOLID_TRIGGER;
			ActivateUse = false;
		}

		if (State.GetAngles() != vec3fOrigin)
			G_SetMovedir (State.GetAngles(), MoveDir);

		SetBrushModel ();

		if (!map_debug->Boolean())
			GetSvFlags() |= SVF_NOCLIENT;
		else
		{
			GetSolid() = SOLID_BBOX;
			GetSvFlags() = (SVF_MONSTER|SVF_DEADMONSTER);
		}

		Link ();
	};

	virtual bool CheckValidity ()
	{
		return CMapEntity::CheckValidity();
	};
};

LINK_CLASSNAME_TO_CLASS ("trigger_multiple", CTriggerMultiple);

/*QUAKED trigger_once (.5 .5 .5) ? x x TRIGGERED
Triggers once, then removes itself.
You must set the key "target" to the name of another object in the level that has a matching "targetname".

If TRIGGERED, this trigger must be triggered before it is live.

sounds
 1)	secret
 2)	beep beep
 3)	large switch
 4)

"message"	string to be displayed when triggered
*/
class CTriggerOnce : public CTriggerMultiple
{
public:
	CTriggerOnce () :
	  CBaseEntity (),
	  CTriggerMultiple ()
	  {
	  };

	CTriggerOnce (sint32 Index) :
	  CBaseEntity (Index),
	  CTriggerMultiple (Index)
	  {
	  };

	void Spawn ()
	{
		// make old maps work because I messed up on flag assignments here
		// triggered was on bit 1 when it should have been on bit 4
		if (SpawnFlags & 1)
		{
			SpawnFlags &= ~1;
			SpawnFlags |= 4;

			vec3f origin = GetMins().MultiplyAngles (0.5f, GetSize());
			MapPrint (MAPPRINT_WARNING, this, origin, "Fixed TRIGGERED flag\n");
		}

		Wait = -1;
		CTriggerMultiple::Spawn ();
	};

	bool CheckValidity ()
	{
		// Yet another map hack
		if (!Q_stricmp(level.ServerLevelName.c_str(), "command") && !Q_stricmp(Model, "*27"))
			SpawnFlags &= ~SPAWNFLAG_NOT_HARD;
		return CMapEntity::CheckValidity ();
	};
};

LINK_CLASSNAME_TO_CLASS ("trigger_once", CTriggerOnce);

/*QUAKED trigger_counter (.5 .5 .5) ? nomessage
Acts as an intermediary for an action that takes multiple inputs.

If nomessage is not set, t will print "1 more.. " etc when triggered and "sequence complete" when finished.

After the counter has been triggered "count" times (default 2), it will fire all of it's targets and remove itself.
*/
class CTriggerCounter : public CTriggerMultiple
{
public:
	uint8			Count;

	CTriggerCounter () :
	  CBaseEntity (),
	  CTriggerMultiple ()
	  {
	  };

	CTriggerCounter (sint32 Index) :
	  CBaseEntity (Index),
	  CTriggerMultiple (Index)
	  {
	  };

	ENTITYFIELD_DEFS
	ENTITYFIELDS_SAVABLE(CTriggerCounter)

	void Touch (CBaseEntity *other, plane_t *plane, cmBspSurface_t *surf)
	{
	};

	void Use (CBaseEntity *other, CBaseEntity *activator)
	{
		bool IsClient = true;
		if (Count == 0)
			return;
		if (!(activator->EntityFlags & ENT_PLAYER))
			IsClient = false;
		
		CPlayerEntity *Player = (IsClient) ? entity_cast<CPlayerEntity>(activator) : NULL;

		if (--Count)
		{
			if (! (SpawnFlags & 1))
			{
				if (IsClient)
					Player->PrintToClient (PRINT_CENTER, "%i more to go...", Count);
				activator->PlaySound (CHAN_AUTO, SoundIndex ("misc/talk1.wav"));
			}
			return;
		}
		
		if (! (SpawnFlags & 1))
		{
			if (IsClient)
				Player->PrintToClient (PRINT_CENTER, "Sequence completed!");
			activator->PlaySound (CHAN_AUTO, SoundIndex ("misc/talk1.wav"));
		}
		Activator = activator;
		Trigger ();
	};

	void Spawn ()
	{
		Touchable = true;
		Wait = -1;
		if (!Count)
			Count = 2;
	};
};

ENTITYFIELDS_BEGIN(CTriggerCounter)
{
	CEntityField ("count", EntityMemberOffset(CTriggerCounter,Count), FT_BYTE | FT_SAVABLE),
};
ENTITYFIELDS_END(CTriggerCounter);

bool CTriggerCounter::ParseField (const char *Key, const char *Value)
{
	if (CheckFields<CTriggerCounter> (this, Key, Value))
		return true;

	return CTriggerBase::ParseField (Key, Value);
}

void			CTriggerCounter::SaveFields (CFile &File)
{
	SaveEntityFields <CTriggerCounter> (this, File);
	CUsableEntity::SaveFields (File);
}

void			CTriggerCounter::LoadFields (CFile &File)
{
	LoadEntityFields <CTriggerCounter> (this, File);
	CUsableEntity::LoadFields (File);
}

LINK_CLASSNAME_TO_CLASS ("trigger_counter", CTriggerCounter);

#define PUSH_ONCE		1

/*QUAKED trigger_push (.5 .5 .5) ? PUSH_ONCE
Pushes the player
"speed"		defaults to 1000
*/
class CTriggerPush : public CTriggerMultiple
{
public:
	bool	Q3Touch;
	float	Speed;

	CTriggerPush () :
	  CBaseEntity (),
	  CTriggerMultiple (),
	  Q3Touch(false),
	  Speed (0)
	  {
	  };

	CTriggerPush (sint32 Index) :
	  CBaseEntity (Index),
	  CTriggerMultiple (Index),
	  Q3Touch(false),
	  Speed (0)
	  {
	  };

	ENTITYFIELD_DEFS
	ENTITYFIELDS_SAVABLE(CTriggerPush)

	void Touch (CBaseEntity *other, plane_t *plane, cmBspSurface_t *surf)
	{
		vec3f vel = MoveDir * Speed;

		if (Q3Touch)
		{
			if (other->EntityFlags & ENT_PHYSICS)
				entity_cast<CPhysicsEntity>(other)->Velocity = vel;

			if (other->EntityFlags & ENT_PLAYER)
			{
				// don't take falling damage immediately from this
				CPlayerEntity *Player = entity_cast<CPlayerEntity>(other);
				Player->Client.OldVelocity = Player->Velocity;
			}
		}
		else
		{
			// FIXME: replace this shit
			if (strcmp(other->ClassName, "grenade") == 0)
			{
				if (other->EntityFlags & ENT_PHYSICS)
					entity_cast<CPhysicsEntity>(other)->Velocity = vel;
			}
			else if ((other->EntityFlags & ENT_HURTABLE) && (entity_cast<CHurtableEntity>(other)->Health > 0))
			{
				if (other->EntityFlags & ENT_PHYSICS)
					entity_cast<CPhysicsEntity>(other)->Velocity = vel;

				if (other->EntityFlags & ENT_PLAYER)
				{
					CPlayerEntity *Player = entity_cast<CPlayerEntity>(other);

					// don't take falling damage immediately from this
					Player->Client.OldVelocity = Player->Velocity;
					if (Player->FlySoundDebounceTime < level.Frame)
					{
						Player->FlySoundDebounceTime = level.Frame + 15;
						other->PlaySound (CHAN_AUTO, GameMedia.FlySound());
					}
				}
			}
			if (SpawnFlags & PUSH_ONCE)
				Free ();
		}
	};

	void Use (CBaseEntity *other, CBaseEntity *activator)
	{
	};

	void Spawn ()
	{
		Touchable = true;
		Init ();

		if (!Speed)
			Speed = 1000;
		Speed *= 10;

		CBaseEntity *target;
		if (!Target)
			Q3Touch = false;
		else if ((target = CC_Find<CMapEntity, ENT_MAP, EntityMemberOffset(CMapEntity,TargetName)> (NULL, Target)) != NULL)
		{
			// Quake3
			//self->touch = trigger_push_q3touch;
			//Q3Touch = true;
			// TODO FIXME
			Free ();
			return;
		}

		Link ();
	};
};

ENTITYFIELDS_BEGIN(CTriggerPush)
{
	CEntityField ("speed", EntityMemberOffset(CTriggerPush,Speed), FT_FLOAT | FT_SAVABLE),
};
ENTITYFIELDS_END(CTriggerPush)

bool			CTriggerPush::ParseField (const char *Key, const char *Value)
{
	if (CheckFields<CTriggerPush> (this, Key, Value))
		return true;

	// Couldn't find it here
	return CTriggerBase::ParseField (Key, Value);
};

void			CTriggerPush::SaveFields (CFile &File)
{
	SaveEntityFields <CTriggerPush> (this, File);
	CTriggerBase::SaveFields (File);
}

void			CTriggerPush::LoadFields (CFile &File)
{
	LoadEntityFields <CTriggerPush> (this, File);
	CTriggerBase::LoadFields (File);
}

LINK_CLASSNAME_TO_CLASS ("trigger_push", CTriggerPush);

/*QUAKED trigger_hurt (.5 .5 .5) ? START_OFF TOGGLE SILENT NO_PROTECTION SLOW
Any entity that touches this will be hurt.

It does dmg points of damage each server frame

SILENT			supresses playing the sound
SLOW			changes the damage rate to once per second
NO_PROTECTION	*nothing* stops the damage

"dmg"			default 5 (whole numbers only)

*/
class CTriggerHurt : public CTriggerMultiple
{
public:
	FrameNumber_t		NextHurt;
	sint32					Damage;

	CTriggerHurt () :
	  CBaseEntity (),
	  CTriggerMultiple (),
	  NextHurt(0),
	  Damage (0)
	  {
	  };

	CTriggerHurt (sint32 Index) :
	  CBaseEntity (Index),
	  CTriggerMultiple (Index),
	  NextHurt(0),
	  Damage (0)
	  {
	  };

	ENTITYFIELD_DEFS
	ENTITYFIELDS_SAVABLE(CTriggerHurt)

	void Touch (CBaseEntity *other, plane_t *plane, cmBspSurface_t *surf)
	{
		if (!((other->EntityFlags & ENT_HURTABLE) && entity_cast<CHurtableEntity>(other)->CanTakeDamage))
			return;

		if (NextHurt > level.Frame)
			return;

		NextHurt = level.Frame + ((SpawnFlags & 16) ? 10 : FRAMETIME);
		if (!(SpawnFlags & 4))
		{
			if ((level.Frame % 10) == 0)
				other->PlaySound (CHAN_AUTO, NoiseIndex);
		}

		entity_cast<CHurtableEntity>(other)->TakeDamage (this, this, vec3fOrigin, other->State.GetOrigin(),
															vec3fOrigin, Damage, Damage,
															(SpawnFlags & 8) ? DAMAGE_NO_PROTECTION : 0, MOD_TRIGGER_HURT);
	};

	void Use (CBaseEntity *other, CBaseEntity *activator)
	{
		if (ActivateUse)
			return;

		GetSolid() = ((GetSolid() == SOLID_NOT) ? SOLID_TRIGGER : SOLID_NOT);
		Link ();

		if (!(SpawnFlags & 2))
			ActivateUse = true;
	};

	void Spawn ()
	{
		Touchable = true;
		Init ();
		NoiseIndex = SoundIndex ("world/electro.wav");

		if (!Damage)
			Damage = 5;

		GetSolid() = ((SpawnFlags & 1) ? SOLID_NOT : SOLID_TRIGGER);

		ActivateUse = (SpawnFlags & 2) ? false : true;
		Link ();
	};
};

ENTITYFIELDS_BEGIN(CTriggerHurt)
{
	CEntityField ("dmg", EntityMemberOffset(CTriggerHurt,Damage), FT_INT | FT_SAVABLE),
};
ENTITYFIELDS_END(CTriggerHurt)

bool			CTriggerHurt::ParseField (const char *Key, const char *Value)
{
	if (CheckFields<CTriggerHurt> (this, Key, Value))
		return true;

	// Couldn't find it here
	return CTriggerBase::ParseField (Key, Value);
};

void			CTriggerHurt::SaveFields (CFile &File)
{
	SaveEntityFields <CTriggerHurt> (this, File);
	CTriggerBase::SaveFields (File);
}

void			CTriggerHurt::LoadFields (CFile &File)
{
	LoadEntityFields <CTriggerHurt> (this, File);
	CTriggerBase::LoadFields (File);
}

LINK_CLASSNAME_TO_CLASS ("trigger_hurt", CTriggerHurt);

/*QUAKED trigger_monsterjump (.5 .5 .5) ?
Walking monsters that touch this will jump in the direction of the trigger's angle
"speed" default to 200, the speed thrown forward
"height" default to 200, the speed thrown upwards
*/
class CTriggerMonsterJump : public CTriggerMultiple
{
public:
	float	Speed;

	CTriggerMonsterJump () :
	  CBaseEntity (),
	  CTriggerMultiple (),
	  Speed (0)
	  {
	  };

	CTriggerMonsterJump (sint32 Index) :
	  CBaseEntity (Index),
	  CTriggerMultiple (Index),
	  Speed (0)
	  {
	  };

	ENTITYFIELD_DEFS
	ENTITYFIELDS_SAVABLE(CTriggerMonsterJump)

	void Touch (CBaseEntity *other, plane_t *plane, cmBspSurface_t *surf)
	{
	};

	void Use (CBaseEntity *other, CBaseEntity *activator)
	{
		if ( !(other->EntityFlags & ENT_MONSTER))
			return;
		if (other->Flags & (FL_FLY | FL_SWIM) )
			return;
		if (other->GetSvFlags() & SVF_DEADMONSTER)
			return;

	// set XY even if not on ground, so the jump will clear lips
		CMonsterEntity *Monster = entity_cast<CMonsterEntity>(other);
		Monster->Velocity = MoveDir * Speed;
		
		if (!Monster->GroundEntity)
			return;
		
		Monster->GroundEntity = NULL;
		Monster->Velocity.Z = MoveDir.Z;
	};

	void Spawn ()
	{
		Touchable = true;
		if (!Speed)
			Speed = 200;
		Speed *= 10;

		if (!MoveDir.Z)
			MoveDir.Z = 200;

		if (State.GetAngles().Y == 0)
			State.GetAngles().Y = 360;

		Init ();
	};
};

ENTITYFIELDS_BEGIN(CTriggerMonsterJump)
{
	CEntityField ("speed", EntityMemberOffset(CTriggerMonsterJump,Speed), FT_FLOAT | FT_SAVABLE),
	CEntityField ("height", EntityMemberOffset(CTriggerMonsterJump,MoveDir.Z), FT_FLOAT),
};
ENTITYFIELDS_END(CTriggerMonsterJump)

bool			CTriggerMonsterJump::ParseField (const char *Key, const char *Value)
{
	if (CheckFields<CTriggerMonsterJump> (this, Key, Value))
		return true;

	// Couldn't find it here
	return CTriggerBase::ParseField (Key, Value);
};

void			CTriggerMonsterJump::SaveFields (CFile &File)
{
	SaveEntityFields <CTriggerMonsterJump> (this, File);
	CTriggerBase::SaveFields (File);
}

void			CTriggerMonsterJump::LoadFields (CFile &File)
{
	LoadEntityFields <CTriggerMonsterJump> (this, File);
	CTriggerBase::LoadFields (File);
}

LINK_CLASSNAME_TO_CLASS ("trigger_monsterjump", CTriggerMonsterJump);

/*QUAKED trigger_gravity (.5 .5 .5) ?
Changes the touching entites gravity to
the value of "gravity".  1.0 is standard
gravity for the level.
*/
class CTriggerGravity : public CTriggerMultiple
{
public:
	float	Gravity;

	CTriggerGravity () :
	  CBaseEntity (),
	  CTriggerMultiple ()
	  {
	  };

	CTriggerGravity (sint32 Index) :
	  CBaseEntity (Index),
	  CTriggerMultiple (Index)
	  {
	  };

	ENTITYFIELD_DEFS
	ENTITYFIELDS_SAVABLE(CTriggerGravity)

	void Touch (CBaseEntity *other, plane_t *plane, cmBspSurface_t *surf)
	{
		if (other->EntityFlags & ENT_PHYSICS)
			entity_cast<CPhysicsEntity>(other)->GravityMultiplier = Gravity;
	};

	void Use (CBaseEntity *other, CBaseEntity *activator)
	{
	};

	void Spawn ()
	{
		Touchable = true;
		if (!Gravity)
		{
			//gi.dprintf("trigger_gravity without gravity set at (%f %f %f)\n", self->state.origin[0], self->state.origin[1], self->state.origin[2]);
			MapPrint (MAPPRINT_ERROR, this, State.GetOrigin(), "No gravity set\n");
			Free ();
			return;
		}

		Init ();
	};
};

ENTITYFIELDS_BEGIN(CTriggerGravity)
{
	CEntityField ("gravity",		EntityMemberOffset(CTriggerGravity,Gravity),		FT_FLOAT | FT_SAVABLE),
};
ENTITYFIELDS_END(CTriggerGravity)

bool			CTriggerGravity::ParseField (const char *Key, const char *Value)
{
	if (CheckFields<CTriggerGravity> (this, Key, Value))
		return true;

	// Couldn't find it here
	return false;
};

void			CTriggerGravity::SaveFields (CFile &File)
{
	SaveEntityFields <CTriggerGravity> (this, File);
}

void			CTriggerGravity::LoadFields (CFile &File)
{
	LoadEntityFields <CTriggerGravity> (this, File);
}

LINK_CLASSNAME_TO_CLASS ("trigger_gravity", CTriggerGravity);

/*
==============================================================================

trigger_key

==============================================================================
*/

/*QUAKED trigger_key (.5 .5 .5) (-8 -8 -8) (8 8 8)
A relay trigger that only fires it's targets if player has the proper key.
Use "item" to specify the required key, for example "key_data_cd"
*/
class CTriggerKey : public CMapEntity, public CUsableEntity
{
public:
	CBaseItem			*Item;
	FrameNumber_t		TouchDebounce;

	CTriggerKey () :
	  CBaseEntity (),
	  CMapEntity (),
	  CUsableEntity (),
	  TouchDebounce(0),
	  Item(NULL)
	{
	};

	CTriggerKey (sint32 Index) :
	  CBaseEntity (Index),
	  CMapEntity (Index),
	  CUsableEntity (Index),
	  TouchDebounce(0),
	  Item(NULL)
	{
	};

	ENTITYFIELD_DEFS
	ENTITYFIELDS_SAVABLE(CTriggerKey)

	bool Run ()
	{
		return CBaseEntity::Run();
	};

	void Use (CBaseEntity *other, CBaseEntity *activator)
	{
		if (!Usable)
			return;
		if (!Item)
			return;
		if (!(activator->EntityFlags & ENT_PLAYER))
			return;

		CPlayerEntity *Player = entity_cast<CPlayerEntity>(activator);

		sint32 index = Item->GetIndex();
		if (!Player->Client.Persistent.Inventory.Has(index))
		{
			if (level.Frame < TouchDebounce)
				return;
			TouchDebounce = level.Frame + 50;

			if (!Message)
				Player->PrintToClient (PRINT_CENTER, "You need the %s", Item->Name);
			else
				Player->PrintToClient (PRINT_CENTER, "%s", Message);
			Player->PlaySound (CHAN_AUTO, SoundIndex ("misc/keytry.wav"));
			return;
		}

		Player->PlaySound (CHAN_AUTO, SoundIndex ("misc/keyuse.wav"));
		if (game.mode == GAME_COOPERATIVE)
		{
			if (Item == NItems::PowerCube)
			{
				sint32	cube;
				for (cube = 0; cube < 8; cube++)
				{
					if (Player->Client.Persistent.PowerCubeCount & (1 << cube))
						break;
				}

				for (sint32 player = 1; player <= game.maxclients; player++)
				{
					CPlayerEntity *ent = entity_cast<CPlayerEntity>(g_edicts[player].Entity);
					if (!ent->GetInUse())
						continue;
					if (ent->Client.Persistent.PowerCubeCount & (1 << cube))
					{
						ent->Client.Persistent.Inventory -= index;
						ent->Client.Persistent.PowerCubeCount &= ~(1 << cube);
					}
				}
			}
			else
			{
				for (sint32 player = 1; player <= game.maxclients; player++)
				{
					CPlayerEntity *ent = entity_cast<CPlayerEntity>(g_edicts[player].Entity);
					if (!ent->GetInUse())
						continue;
					ent->Client.Persistent.Inventory.Set(index, 0);
				}
			}
		}
		else
			Player->Client.Persistent.Inventory -= index;

		UseTargets (activator, Message);

		Usable = false;
	};

	void Spawn ()
	{
		if (!Target)
		{
			MapPrint (MAPPRINT_ERROR, this, State.GetOrigin(), "No target\n");
			return;
		}

		SoundIndex ("misc/keytry.wav");
		SoundIndex ("misc/keyuse.wav");
	};
};

ENTITYFIELDS_BEGIN(CTriggerKey)
{
	CEntityField ("item", EntityMemberOffset(CTriggerKey,Item), FT_ITEM | FT_SAVABLE),
};
ENTITYFIELDS_END(CTriggerKey)

bool CTriggerKey::ParseField (const char *Key, const char *Value)
{
	if (CheckFields<CTriggerKey> (this, Key, Value))
		return true;

	return (CUsableEntity::ParseField (Key, Value) || CMapEntity::ParseField (Key, Value));
}

void			CTriggerKey::SaveFields (CFile &File)
{
	SaveEntityFields <CTriggerKey> (this, File);
	CUsableEntity::SaveFields (File);
}

void			CTriggerKey::LoadFields (CFile &File)
{
	LoadEntityFields <CTriggerKey> (this, File);
	CUsableEntity::LoadFields (File);
}

LINK_CLASSNAME_TO_CLASS ("trigger_key", CTriggerKey);