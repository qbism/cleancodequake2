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
// cc_entitytypes.cpp
// Different base entity classes
//

#include "cc_local.h"

IHurtableEntity::IHurtableEntity () :
IBaseEntity(),
CanTakeDamage (false)
{
	EntityFlags |= ENT_HURTABLE;
};

IHurtableEntity::IHurtableEntity (sint32 Index) :
IBaseEntity(Index),
CanTakeDamage (false)
{
	EntityFlags |= ENT_HURTABLE;
};

ENTITYFIELDS_BEGIN(IHurtableEntity)
{
	CEntityField ("health", EntityMemberOffset(IHurtableEntity,Health), FT_INT | FT_SAVABLE),

	CEntityField ("CanTakeDamage", EntityMemberOffset(IHurtableEntity,CanTakeDamage), FT_BOOL | FT_NOSPAWN | FT_SAVABLE),
	CEntityField ("DeadFlag", EntityMemberOffset(IHurtableEntity,DeadFlag), FT_BOOL | FT_NOSPAWN | FT_SAVABLE),
	CEntityField ("MaxHealth", EntityMemberOffset(IHurtableEntity,MaxHealth), FT_INT | FT_NOSPAWN | FT_SAVABLE),
	CEntityField ("GibHealth", EntityMemberOffset(IHurtableEntity,GibHealth), FT_INT | FT_NOSPAWN | FT_SAVABLE),
};
ENTITYFIELDS_END(IHurtableEntity)

bool			IHurtableEntity::ParseField (const char *Key, const char *Value)
{
	if (CheckFields<IHurtableEntity> (this, Key, Value))
		return true;

	// Couldn't find it here
	return false;
};

void			IHurtableEntity::SaveFields (CFile &File)
{
	SaveEntityFields<IHurtableEntity> (this, File);
};

void			IHurtableEntity::LoadFields (CFile &File)
{
	LoadEntityFields<IHurtableEntity> (this, File);
};

std::string ClientTeam (CPlayerEntity *Player)
{
	std::string val = Q_strlwr(Info_ValueForKey (Player->Client.Persistent.UserInfo, "skin"));

	size_t slash = val.find_first_of('/');
	if (slash == std::string::npos)
		return val;

	if (DeathmatchFlags.dfModelTeams.IsEnabled())
	{
		val.erase (slash);
		return val;
	}

	val.erase (0, 1);
	return val;
}

bool OnSameTeam (CPlayerEntity *Player1, CPlayerEntity *Player2)
{
	if (!(DeathmatchFlags.dfSkinTeams.IsEnabled() || DeathmatchFlags.dfModelTeams.IsEnabled()))
		return false;

	return ClientTeam (Player1) == ClientTeam (Player2);
}

bool IHurtableEntity::CanDamage (IBaseEntity *Inflictor)
{
// bmodels need special checking because their origin is 0,0,0
	if ((EntityFlags & ENT_PHYSICS) && ((entity_cast<IPhysicsEntity>(this))->PhysicsType == PHYSICS_PUSH))
	{
		vec3f dest = (GetAbsMin() + GetAbsMax()) * 0.5f;
		CTrace trace (Inflictor->State.GetOrigin(), dest, Inflictor, CONTENTS_MASK_SOLID);
		if (trace.fraction == 1.0 || trace.Ent == this)
			return true;
		return false;
	}
	
	static const vec3f additions[] = 
	{
		vec3f(0, 0, 0),
		vec3f(15, 15, 0),
		vec3f(15, -15, 0),
		vec3f(-15, 15, 0),
		vec3f(-15, -15, 0)
	};

	for (sint32 i = 0; i < 5; i++)
	{
		vec3f end = State.GetOrigin() + additions[i];
		CTrace trace (Inflictor->State.GetOrigin(), end, Inflictor, CONTENTS_MASK_SOLID);
		if (trace.fraction == 1.0)
			return true;
	};

	return false;
}

bool IHurtableEntity::CheckTeamDamage (IBaseEntity *Attacker)
{
#if CLEANCTF_ENABLED
//ZOID
	if ((Game.GameMode & GAME_CTF) && (EntityFlags & ENT_PLAYER) && (Attacker->EntityFlags & ENT_PLAYER))
	{
		CPlayerEntity *Targ = entity_cast<CPlayerEntity>(this);
		CPlayerEntity *PlayerAttacker = entity_cast<CPlayerEntity>(Attacker);
		if (Targ->Client.Respawn.CTF.Team == PlayerAttacker->Client.Respawn.CTF.Team &&
			(this != PlayerAttacker))
			return true;
	}
//ZOID
#endif

	return false;
}

#include "cc_tent.h"

sint32 IHurtableEntity::CheckPowerArmor (vec3f &point, vec3f &normal, sint32 Damage, sint32 dflags)
{
	if (!Damage)
		return 0;

	if (dflags & DAMAGE_NO_ARMOR)
		return 0;

	static const sint32	index = NItems::Cells->GetIndex();

	sint32			Type, DamagePerCell, Power = 0;
	bool			ScreenSparks = false;

	bool		IsClient = !!(EntityFlags & ENT_PLAYER),
				IsMonster = !!(EntityFlags & ENT_MONSTER);

	CPlayerEntity	*Player = (IsClient) ? entity_cast<CPlayerEntity>(this) : NULL;
	CMonsterEntity	*Monster = (IsMonster) ? entity_cast<CMonsterEntity>(this) : NULL;

	if (IsClient)
	{
		Type = Player->PowerArmorType ();
		if (Type != POWER_ARMOR_NONE)
			Power = Player->Client.Persistent.Inventory.Has(index);
	}
	else if (IsMonster)
	{
		Type = Monster->Monster->PowerArmorType;
		Power = Monster->Monster->PowerArmorPower;
	}
	else
		return 0;

	if (!Power)
		return 0;

	switch (Type)
	{
	case POWER_ARMOR_NONE:
	default:
		return 0;
	case POWER_ARMOR_SCREEN:
		{
			vec3f		vec, forward;

			// only works if damage point is in front
			State.GetAngles().ToVectors(&forward, NULL, NULL);
			vec = point - State.GetOrigin();
			vec.Normalize ();
			if ((vec | forward) <= 0.3)
				return 0;

			DamagePerCell = 1;
			ScreenSparks = true;
			Damage /= 3;
		}
		break;
	case POWER_ARMOR_SHIELD:
		DamagePerCell = 2;
		Damage = (2 * Damage) / 3;
		break;
	};

	sint32 Saved = Power * DamagePerCell;
	if (!Saved)
		return 0;
	if (Saved > Damage)
		Saved = Damage;

	CShieldSparks(point, normal, ScreenSparks).Send();

	sint32 PowerUsed = Saved / DamagePerCell;
	if (!PowerUsed)
		PowerUsed = 1;

	if (IsClient)
	{
		Player->Client.Persistent.Inventory.Remove(index, PowerUsed);
		Player->Client.PowerArmorTime = 2;
	}
	else if (IsMonster)
	{
		Monster->Monster->PowerArmorPower -= PowerUsed;
		Monster->Monster->PowerArmorTime = 2;
	}

	return Saved;
}

void IHurtableEntity::Killed (IBaseEntity *Inflictor, IBaseEntity *Attacker, sint32 Damage, vec3f &point)
{
	if (Health < -999)
		Health = -999;

	Enemy = Attacker;
	CMonsterEntity *Monster = (EntityFlags & ENT_MONSTER) ? entity_cast<CMonsterEntity>(this) : NULL;

#if ROGUE_FEATURES
	if (EntityFlags & ENT_MONSTER)
	{
		if (Monster->Monster->AIFlags & AI_MEDIC)
		{
			if (Monster->Enemy && (Monster->Enemy->EntityFlags & ENT_MONSTER))  // god, I hope so
				entity_cast<CMonsterEntity>(Monster->Enemy)->Monster->CleanupHealTarget ();

			// clean up self
			Monster->Monster->AIFlags &= ~AI_MEDIC;
			Monster->Enemy = Attacker;
		}
	}
#endif

#if ROGUE_FEATURES
	if ((!DeadFlag) && (EntityFlags & ENT_MONSTER))
	{
		if (Monster->Monster->AIFlags & AI_SPAWNED_CARRIER)
		{
			if (Monster->Monster->Commander && Monster->Monster->Commander->GetInUse() && 
				!strcmp(Monster->Monster->Commander->ClassName.c_str(), "monster_carrier"))
				Monster->Monster->Commander->Monster->MonsterSlots++;
		}

		if (Monster->Monster->AIFlags & AI_SPAWNED_MEDIC_C)
		{
			if (Monster->Monster->Commander)
			{
				if (Monster->Monster->Commander->GetInUse() && !strcmp(Monster->Monster->Commander->ClassName.c_str(), "monster_medic_commander"))
					Monster->Monster->Commander->Monster->MonsterSlots++;
			}
		}

		if (Monster->Monster->AIFlags & AI_SPAWNED_WIDOW)
		{
			// need to check this because we can have variable numbers of coop players
			if (Monster->Monster->Commander && Monster->Monster->Commander->GetInUse() && 
				!strncmp(Monster->Monster->Commander->ClassName.c_str(), "monster_widow", 13))
			{
				if (Monster->Monster->Commander->Monster->MonsterUsed > 0)
					Monster->Monster->Commander->Monster->MonsterUsed--;
			}
		}

		if (!(Monster->Monster->AIFlags & AI_GOOD_GUY) && !(Monster->Monster->AIFlags & AI_DO_NOT_COUNT))
		{
			Level.Monsters.Killed++;
			if ((Game.GameMode == GAME_COOPERATIVE) && (Attacker->EntityFlags & ENT_PLAYER))
				(entity_cast<CPlayerEntity>(Attacker))->Client.Respawn.Score++;
		}
	}
#else
	if ((!DeadFlag) && (EntityFlags & ENT_MONSTER))
	{
		if (!(Monster->Monster->AIFlags & AI_GOOD_GUY))
		{
			Level.Monsters.Killed++;
			if ((Game.GameMode == GAME_COOPERATIVE) && (Attacker->EntityFlags & ENT_PLAYER))
				(entity_cast<CPlayerEntity>(Attacker))->Client.Respawn.Score++;
			// medics won't heal monsters that they kill themselves

#if !ROGUE_FEATURES
			if (strcmp(Attacker->ClassName.c_str(), "monster_medic") == 0)
				SetOwner (Attacker);
#endif
		}
	}
#endif

	if ((EntityFlags & ENT_MONSTER) && (!DeadFlag))
	{
		if (EntityFlags & ENT_TOUCHABLE)
			(entity_cast<ITouchableEntity>(this))->Touchable = false;
		if (EntityFlags & ENT_MONSTER)
			(entity_cast<CMonsterEntity>(this))->Monster->MonsterDeathUse();
	}

	if (((EntityFlags & ENT_HURTABLE) && entity_cast<IHurtableEntity>(this)->CanTakeDamage))
		(entity_cast<IHurtableEntity>(this))->Die (Inflictor, Attacker, Damage, point);
}

void IHurtableEntity::DamageEffect (vec3f &dir, vec3f &point, vec3f &normal, sint32 &damage, sint32 &dflags)
{
	if ((EntityFlags & ENT_MONSTER) || (EntityFlags & ENT_PLAYER))
		CBlood(point, normal).Send();
	else
		CSparks(point, normal, (dflags & DAMAGE_BULLET) ? ST_BULLET_SPARKS : ST_SPARKS, SPT_SPARKS).Send();
}

bool LastPelletShot = true;
void IHurtableEntity::TakeDamage (IBaseEntity *Inflictor, IBaseEntity *Attacker,
								vec3f dir, vec3f point, vec3f normal, sint32 Damage,
								sint32 knockback, sint32 dflags, EMeansOfDeath mod)
{
	if (CvarList[CV_MAP_DEBUG].Boolean())
		return;

	sint32			take;
	sint32			save;
	sint32			asave = 0;
	sint32			psave = 0;

	// Needed?
	if (!CanTakeDamage)
		return;

	bool isClient = !!(EntityFlags & ENT_PLAYER);
	CPlayerEntity *Player = (isClient) ? entity_cast<CPlayerEntity>(this) : NULL;
	CClient *Client = (isClient) ? &(Player->Client) : NULL;

	// friendly fire avoidance
	// if enabled you can't hurt teammates (but you can hurt yourself)
	// knockback still occurs
	if ((this != Attacker) && ((Game.GameMode & GAME_DEATHMATCH) && (DeathmatchFlags.dfSkinTeams.IsEnabled() || DeathmatchFlags.dfModelTeams.IsEnabled()) || Game.GameMode == GAME_COOPERATIVE))
	{
		if ((EntityFlags & ENT_PLAYER) && Attacker && (Attacker->EntityFlags & ENT_PLAYER))
		{
			if (OnSameTeam (Player, entity_cast<CPlayerEntity>(Attacker)))
			{
				if (DeathmatchFlags.dfNoFriendlyFire.IsEnabled()
#if ROGUE_FEATURES
					&& (mod != MOD_NUKE)
#endif
					)
					Damage = 0;
				else
					mod |= MOD_FRIENDLY_FIRE;
			}
		}
	}
	meansOfDeath = mod;

	// easy mode takes half damage
#if ROGUE_FEATURES
	if ((CvarList[CV_SKILL].Integer() == 0 && !(Game.GameMode & GAME_DEATHMATCH) && (EntityFlags & ENT_PLAYER)) ||
		(isClient) && (Player->Client.OwnedSphere) && (Player->Client.OwnedSphere->SpawnFlags == 1))
#else
	if (CvarList[CV_SKILL].Integer() == 0 && !(Game.GameMode & GAME_DEATHMATCH) && (EntityFlags & ENT_PLAYER))
#endif
	{
		Damage *= 0.5;
		if (!Damage)
			Damage = 1;
	}


	dir.Normalize ();

// bonus damage for surprising a monster
// Paril revision: Allow multiple pellet weapons to take advantage of this too!
	if (!(dflags & DAMAGE_RADIUS) && (EntityFlags & ENT_MONSTER) && Attacker && (Attacker->EntityFlags & ENT_PLAYER))
	{
		CMonsterEntity *Monster = entity_cast<CMonsterEntity>(this);

		if ((Health > 0) &&
			(!Enemy && (Monster->BonusDamageTime <= Level.Frame)) ||
			(Enemy && (Monster->BonusDamageTime == Level.Frame)))
		{
			Monster->BonusDamageTime = Level.Frame;
			Damage *= 2;
		}
	}

	if (DeathmatchFlags.dfDmTechs.IsEnabled()
#if CLEANCTF_ENABLED
	|| (Game.GameMode & GAME_CTF)
#endif
	)
	{
		if (isClient)
		{
			if (Client->Persistent.Tech && (Client->Persistent.Tech->TechType == CTech::TECH_AGGRESSIVE))
				Client->Persistent.Tech->DoAggressiveTech (Player, Attacker, false, Damage, knockback, dflags, mod, true);
		}

		if (Attacker->EntityFlags & ENT_PLAYER)
		{
			CPlayerEntity *Atk = entity_cast<CPlayerEntity>(Attacker);
			if (Atk->Client.Persistent.Tech && (Atk->Client.Persistent.Tech->TechType == CTech::TECH_AGGRESSIVE))
				entity_cast<CPlayerEntity>(Attacker)->Client.Persistent.Tech->DoAggressiveTech (Atk, Player, false, Damage, knockback, dflags, mod, false);
		}
	}

	if (Flags & FL_NO_KNOCKBACK)
		knockback = 0;

// figure momentum add
	if (knockback && !(dflags & DAMAGE_NO_KNOCKBACK) && (EntityFlags & ENT_PHYSICS))
	{
		IPhysicsEntity *Phys = entity_cast<IPhysicsEntity>(this);
		if (!(Phys->PhysicsType == PHYSICS_NONE ||
			Phys->PhysicsType == PHYSICS_BOUNCE ||
			Phys->PhysicsType == PHYSICS_PUSH ||
			Phys->PhysicsType == PHYSICS_STOP))
			Phys->Velocity += dir * (((isClient && (Attacker == this)) ? 1600.0f : 500.0f) * (float)knockback / Clamp<float> (Phys->Mass, 50, Phys->Mass));
	}

	take = Damage;
	save = 0;

	// check for godmode
	if ( (Flags & FL_GODMODE) && !(dflags & DAMAGE_NO_PROTECTION) )
	{
		take = 0;
		save = Damage;
		CSparks (point, normal, (dflags & DAMAGE_BULLET) ? ST_BULLET_SPARKS : ST_SPARKS, SPT_SPARKS).Send();
	}

	// check for invincibility
	if (((isClient && (Client->Timers.Invincibility > Level.Frame) ) && !(dflags & DAMAGE_NO_PROTECTION))
#if ROGUE_FEATURES
		|| ((((EntityFlags & ENT_MONSTER) && ((entity_cast<CMonsterEntity>(this))->Monster->InvincibleFramenum) > Level.Frame ) && !(dflags & DAMAGE_NO_PROTECTION)))
#endif
		)
	{
#if ROGUE_FEATURES
		if ((isClient && Player->PainDebounceTime < Level.Frame) || ((EntityFlags & ENT_MONSTER) && entity_cast<CMonsterEntity>(this)->Monster->PainDebounceTime < Level.Frame))
#else
		if (Player->PainDebounceTime < Level.Frame)
#endif
		{
			PlaySound (CHAN_ITEM, SoundIndex("items/protect4.wav"));
#if ROGUE_FEATURES
			if (isClient)
				Player->PainDebounceTime = Level.Frame + 20;
			else
				entity_cast<CMonsterEntity>(this)->Monster->PainDebounceTime = Level.Frame + 20;
#else
			Player->PainDebounceTime = Level.Frame + 20;
#endif
		}
		take = 0;
		save = Damage;
	}

#if CLEANCTF_ENABLED
//ZOID
//team armor protect
	if ((Game.GameMode & GAME_CTF) && isClient && (Attacker->EntityFlags & ENT_PLAYER) &&
		(Client->Respawn.CTF.Team == (entity_cast<CPlayerEntity>(Attacker))->Client.Respawn.CTF.Team) &&
		(this != Attacker) && DeathmatchFlags.dfCtfArmorProtect.IsEnabled())
		psave = asave = 0;
	else
	{
//ZOID
#endif
		psave = CheckPowerArmor (point, normal, take, dflags);
		take -= psave;

		if (isClient)
		{
			if (Client->Persistent.Armor)
			{
				asave = Client->Persistent.Armor->CheckArmor (entity_cast<CPlayerEntity>(this), point, normal, take, dflags);
				take -= asave;
			}
		}
#if CLEANCTF_ENABLED
	}
#endif

	//treat cheat/powerup savings the same as armor
	asave += save;

	if (DeathmatchFlags.dfDmTechs.IsEnabled()
#if CLEANCTF_ENABLED
	|| (Game.GameMode & GAME_CTF)
#endif
	)
	{
		if (isClient)
		{
			if (Client->Persistent.Tech && (Client->Persistent.Tech->TechType == CTech::TECH_AGGRESSIVE))
				Client->Persistent.Tech->DoAggressiveTech (entity_cast<CPlayerEntity>(this), Attacker, true, take, knockback, dflags, mod, true);
		}

		if ((EntityFlags & ENT_PLAYER) && (Attacker->EntityFlags & ENT_PLAYER))
		{
			CPlayerEntity *Atk = entity_cast<CPlayerEntity>(Attacker);
			if (Atk->Client.Persistent.Tech && (Atk->Client.Persistent.Tech->TechType == CTech::TECH_AGGRESSIVE))
				entity_cast<CPlayerEntity>(Attacker)->Client.Persistent.Tech->DoAggressiveTech (Atk, entity_cast<CPlayerEntity>(this), true, Damage, knockback, dflags, mod, false);
		}
	}

	// team damage avoidance
	if (!(dflags & DAMAGE_NO_PROTECTION) && CheckTeamDamage (Attacker))
		return;

// ROGUE - this option will do damage both to the armor and person. originally for DPU rounds
	if (dflags & DAMAGE_DESTROY_ARMOR)
	{
		if(!(Flags & FL_GODMODE) && !(dflags & DAMAGE_NO_PROTECTION) &&
		   !(isClient && Player->Client.Timers.Invincibility > Level.Frame))
			take = Damage;
	}
// ROGUE

#if CLEANCTF_ENABLED
//ZOID
	if ((Game.GameMode & GAME_CTF) && (isClient && (Attacker->EntityFlags & ENT_PLAYER)))
		CTFCheckHurtCarrier((entity_cast<CPlayerEntity>(this)), (entity_cast<CPlayerEntity>(Attacker)));
//ZOID
#endif

// do the damage
	if (take)
	{
		DamageEffect (dir, point, normal, take, dflags);

#if CLEANCTF_ENABLED
		if (!CTFMatchSetup())
#endif
			Health -= take;
	}

#if ROGUE_FEATURES
	if(isClient && Player->Client.OwnedSphere)
		Player->Client.OwnedSphere->Pain (Attacker, 0);
#endif

	if (Health <= 0)
	{
		if ((EntityFlags & ENT_MONSTER) || (isClient))
			Flags |= FL_NO_KNOCKBACK;
		Killed (Inflictor, Attacker, take, point);
		return;
	}

	if (EntityFlags & ENT_MONSTER)
	{
		CMonster *Monster = (entity_cast<CMonsterEntity>(this))->Monster;
		Monster->ReactToDamage (Attacker, Inflictor);
		if (!(Monster->AIFlags & AI_DUCKED) && take)
		{
			if (LastPelletShot)
			{
				Pain (Attacker, take);
				if (CvarList[CV_SKILL].Integer() == 3)
					Monster->PainDebounceTime = Level.Frame + 50;
			}
		}
	}
	else if (((EntityFlags & ENT_PLAYER) && take
#if CLEANCTF_ENABLED
		&& !CTFMatchSetup()
#endif
		) || take)
		Pain (Attacker, take);

	// add to the damage inflicted on a player this frame
	// the total will be turned into screen blends and view angle kicks
	// at the end of the frame
	if (isClient)
	{
		Client->DamageValues[DT_POWERARMOR] += psave;
		Client->DamageValues[DT_ARMOR] += asave;
		Client->DamageValues[DT_BLOOD] += take;
		Client->DamageValues[DT_KNOCKBACK] += knockback;
		Client->DamageFrom = point;
	}
}
	
void IHurtableEntity::TakeDamage (IBaseEntity *targ, IBaseEntity *Inflictor,
								IBaseEntity *Attacker, vec3f dir, vec3f point,
								vec3f normal, sint32 Damage, sint32 knockback,
								sint32 dflags, EMeansOfDeath mod)
{
	if ((targ->EntityFlags & ENT_HURTABLE) && entity_cast<IHurtableEntity>(targ)->CanTakeDamage)
		(entity_cast<IHurtableEntity>(targ))->TakeDamage (Inflictor, Attacker, dir, point, normal, Damage, knockback, dflags, mod);
}

IThinkableEntity::IThinkableEntity () :
IBaseEntity()
{
	EntityFlags |= ENT_THINKABLE;
};

IThinkableEntity::IThinkableEntity (sint32 Index) :
IBaseEntity(Index)
{
	EntityFlags |= ENT_THINKABLE;
};

void IThinkableEntity::SaveFields (CFile &File)
{
	// Save NextThink
	File.Write<FrameNumber_t> (NextThink);
};

void IThinkableEntity::LoadFields (CFile &File)
{
	// Load NextThink
	NextThink = File.Read<FrameNumber_t> ();
};

void IThinkableEntity::RunThink ()
{
	if (NextThink <= 0 || NextThink > Level.Frame)
		return;
	
	NextThink = 0;
	Think ();
}

ITouchableEntity::ITouchableEntity () :
IBaseEntity()
{
	EntityFlags |= ENT_TOUCHABLE;
};

ITouchableEntity::ITouchableEntity (sint32 Index) :
IBaseEntity(Index)
{
	EntityFlags |= ENT_TOUCHABLE;
};

void ITouchableEntity::SaveFields (CFile &File)
{
	File.Write<bool> (Touchable);
};

void ITouchableEntity::LoadFields (CFile &File)
{
	Touchable = File.Read<bool> ();
};

void ITouchableEntity::Touch(IBaseEntity *other, plane_t *plane, cmBspSurface_t *surf)
{
}

IPhysicsEntity::IPhysicsEntity () :
IBaseEntity(),
GravityMultiplier(1.0f),
PhysicsFlags(0),
DampeningEffect(1, 1, 1),
ADampeningEffect(1, 1, 1),
GravityVector(0, 0, -1)
{
	EntityFlags |= ENT_PHYSICS;
	PhysicsType = PHYSICS_NONE;
};

IPhysicsEntity::IPhysicsEntity (sint32 Index) :
IBaseEntity(Index),
GravityMultiplier(1.0f),
PhysicsFlags(0),
DampeningEffect(1, 1, 1),
ADampeningEffect(1, 1, 1),
GravityVector(0, 0, -1)
{
	EntityFlags |= ENT_PHYSICS;
	PhysicsType = PHYSICS_NONE;
};

void IPhysicsEntity::AddGravity()
{
	if (GravityVector[2] > 0)
		Velocity = Velocity.MultiplyAngles (GravityMultiplier * CvarList[CV_GRAVITY].Float() * 0.1f, GravityVector);
	else
		Velocity.Z -= GravityMultiplier * CvarList[CV_GRAVITY].Float() * 0.1f;
}

CTrace IPhysicsEntity::PushEntity (vec3f &push)
{
	vec3f		Start = State.GetOrigin();
	vec3f		End = Start + push;

	CTrace Trace;
	while (true)
	{
		Trace (Start, GetMins(), GetMaxs(), End, this, (GetClipmask()) ? GetClipmask() : CONTENTS_MASK_SOLID);
		
		State.GetOrigin() = Trace.EndPos;
		Link();

		if (Trace.fraction != 1.0)
		{
			Impact (&Trace);

			// if the pushed entity went away and the pusher is still there
			if (!Trace.Ent->GetInUse() && GetInUse())
			{
				// move the pusher back and try again
				State.GetOrigin() = Start;
				Link ();
				continue;
			}
		}
		break;
	}

	if (GetInUse())
		G_TouchTriggers (this);

	return Trace;
}

void IPhysicsEntity::Impact (CTrace *trace)
{
	if (!trace->ent->Entity)
		return;

	if (GetSolid() != SOLID_NOT && (EntityFlags & ENT_TOUCHABLE))
	{
		ITouchableEntity *Touched = entity_cast<ITouchableEntity>(this);

		if (Touched->Touchable)
			Touched->Touch (trace->Ent, &trace->plane, trace->surface);
	}

	if ((trace->Ent->EntityFlags & ENT_TOUCHABLE) && trace->Ent->GetSolid() != SOLID_NOT)
	{
		ITouchableEntity *Touched = entity_cast<ITouchableEntity>(trace->Ent);

		if (Touched->Touchable)
			Touched->Touch (this, NULL, NULL);
	}
}

void IPhysicsEntity::PushInDirection (vec3f vel)
{
	if ((EntityFlags & ENT_HURTABLE) && (entity_cast<IHurtableEntity>(this)->Health > 0))
		Velocity = vel;
}

IBounceProjectile::IBounceProjectile () :
  IPhysicsEntity (),
  backOff(1.5f),
  AffectedByGravity(true),
  StopOnEqualPlane(true),
  AimInVelocityDirection(false)
{
	PhysicsType = PHYSICS_BOUNCE;
}

IBounceProjectile::IBounceProjectile (sint32 Index) :
  IBaseEntity(Index),
  IPhysicsEntity (Index),
  backOff(1.5f),
  AffectedByGravity(true),
  StopOnEqualPlane(true),
  AimInVelocityDirection(false)
{
	PhysicsType = PHYSICS_BOUNCE;
}

sint32 ClipVelocity (vec3f &in, vec3f &normal, vec3f &out, float overbounce);
bool IBounceProjectile::Run ()
{
	CTrace	trace;
	vec3f		move, old_origin;
	bool		wasinwater;
	bool		isinwater;

	// if not a team captain, so movement will be handled elsewhere
	if (Flags & FL_TEAMSLAVE)
		return false;

	if (Velocity.Z > 0)
		GroundEntity = NULL;

// check for the groundentity going away
	if (GroundEntity && !GroundEntity->GetInUse())
		GroundEntity = NULL;

// if onground, return without moving
	if (GroundEntity && GravityMultiplier > 0.0)
		return false;

	old_origin = State.GetOrigin();

// add gravity
	if (AffectedByGravity)
		AddGravity ();

// move angles
	State.GetAngles() = State.GetAngles().MultiplyAngles (0.1f, AngularVelocity);

// move origin
	move = Velocity * 0.1f;

	trace = PushEntity (move);
	if (!GetInUse())
		return false;

	if (trace.fraction < 1)
	{
		ClipVelocity (Velocity, trace.plane.normal, Velocity, backOff);

		if (AimInVelocityDirection)
			State.GetAngles() = Velocity.ToAngles();

		// stop if on ground
		if (trace.plane.normal[2] > 0.7 && StopOnEqualPlane)
		{		
			if (Velocity.Z < 60)
			{
				GroundEntity = trace.Ent;
				GroundEntityLinkCount = GroundEntity->GetLinkCount();
				Velocity.Clear ();
				AngularVelocity.Clear ();
			}
		}
	}
	
// check for water transition
	wasinwater = (WaterInfo.Type & CONTENTS_MASK_WATER) ? true : false;
	WaterInfo.Type = PointContents (State.GetOrigin ());
	isinwater = (WaterInfo.Type & CONTENTS_MASK_WATER) ? true : false;

	WaterInfo.Level = (isinwater) ? WATER_FEET : WATER_NONE;
	if (!wasinwater && isinwater)
		World->PlayPositionedSound (old_origin, CHAN_AUTO, SoundIndex("misc/h2ohit1.wav"));
	else if (wasinwater && !isinwater)
		World->PlayPositionedSound (State.GetOrigin (), CHAN_AUTO, SoundIndex("misc/h2ohit1.wav"));

// move teamslaves
	for (IBaseEntity *slave = Team.Chain; slave; slave = slave->Team.Chain)
	{
		slave->State.GetOrigin() = State.GetOrigin();
		slave->Link ();
	}

	// Run physics modifiers
	if (PhysicsFlags & PHYSICFLAG_BUOYANCY)
		entity_cast<IBuoyancyPhysics>(this)->RunBouyancy ();

	if (PhysicsFlags & PHYSICFLAG_RESISTANCE)
		entity_cast<IResistancePhysics>(this)->RunResistance ();

	if (PhysicsFlags & PHYSICFLAG_AERODYNAMICS)
		entity_cast<IAerodynamicPhysics>(this)->RunAerodynamics();

	// Use data
	Velocity *= DampeningEffect;
	Velocity += VelocityEffect;	
	AngularVelocity *= ADampeningEffect;
	AngularVelocity += AVelocityEffect;	
	
	// Reset data
	VelocityEffect = AVelocityEffect = vec3fOrigin;
	DampeningEffect = ADampeningEffect = vec3f (1, 1, 1);

	return true;
}

ITossProjectile::ITossProjectile () :
  IBounceProjectile ()
{
	backOff = 1.0f;

	PhysicsType = PHYSICS_TOSS;
}

ITossProjectile::ITossProjectile (sint32 Index) :
  IBaseEntity (Index),
  IBounceProjectile (Index)
{
	backOff = 1.0f;

	PhysicsType = PHYSICS_TOSS;
}

IFlyMissileProjectile::IFlyMissileProjectile () :
  IBounceProjectile ()
{
	AffectedByGravity = false;
	backOff = 1.0f;
	PhysicsType = PHYSICS_FLYMISSILE;
}

IFlyMissileProjectile::IFlyMissileProjectile (sint32 Index) :
  IBaseEntity (Index),
  IBounceProjectile (Index)
{
	AffectedByGravity = false;
	backOff = 1.0f;
	PhysicsType = PHYSICS_FLYMISSILE;
}

IStepPhysics::IStepPhysics () :
  IPhysicsEntity ()
{
	PhysicsType = PHYSICS_STEP;
}

IStepPhysics::IStepPhysics (sint32 Index) :
  IBaseEntity (Index),
  IPhysicsEntity (Index)
{
	PhysicsType = PHYSICS_STEP;
}

void IStepPhysics::CheckGround ()
{
	if (Velocity.Z > 100)
	{
		GroundEntity = NULL;
		return;
	}

// if the hull point one-quarter unit down is solid the entity is on ground
	vec3f point = State.GetOrigin();
	point.Z -= 0.25f;

	CTrace trace (State.GetOrigin(), GetMins(), GetMaxs(), point, this, CONTENTS_MASK_MONSTERSOLID);

	// check steepness
	if ( trace.plane.normal[2] < 0.7 && !trace.startSolid)
	{
		GroundEntity = NULL;
		return;
	}

	if (!trace.startSolid && !trace.allSolid)
	{
		State.GetOrigin() = trace.EndPos;
		GroundEntity = trace.Ent;
		GroundEntityLinkCount = trace.Ent->GetLinkCount();
		Velocity.Z = 0;
	}
}

#define SV_STOPSPEED		100
#define SV_FRICTION			6
#define SV_WATERFRICTION	1

void IStepPhysics::AddRotationalFriction ()
{
	State.GetAngles() = State.GetAngles().MultiplyAngles (0.1f, AngularVelocity);

	float adjustment = 0.1f * SV_STOPSPEED * SV_FRICTION;
	for (sint32 n = 0; n < 3; n++)
	{
		if (AngularVelocity[n] > 0)
		{
			AngularVelocity[n] -= adjustment;
			if (AngularVelocity[n] < 0)
				AngularVelocity[n] = 0;
		}
		else
		{
			AngularVelocity[n] += adjustment;
			if (AngularVelocity[n] > 0)
				AngularVelocity[n] = 0;
		}
	}
}

#define MAX_CLIP_PLANES	5
sint32 IStepPhysics::FlyMove (float time, sint32 mask)
{
	edict_t		*hit;
	sint32			i, j, blocked = 0, numplanes = 0, numbumps = 4;
	vec3f		planes[MAX_CLIP_PLANES], dir, primal_velocity, original_velocity, new_velocity, end;
	float		d, time_left = time;
	
	original_velocity = primal_velocity = Velocity;
	GroundEntity = NULL;

	for (sint32 bumpcount = 0; bumpcount < numbumps; bumpcount++)
	{
		end = State.GetOrigin () + time_left * Velocity;

		CTrace trace (State.GetOrigin (), GetMins(), GetMaxs(), end, this, mask);

		if (trace.allSolid)
		{
			// entity is trapped in another solid
			Velocity.Clear ();
			return 3;
		}

		if (trace.fraction > 0)
		{
			// actually covered some distance
			State.GetOrigin() = trace.EndPos;
			original_velocity = Velocity;
			numplanes = 0;
		}

		if (trace.fraction == 1)
			 break;		// moved the entire distance

		hit = trace.ent;

		if (trace.plane.normal[2] > 0.7)
		{
			blocked |= 1;		// floor
			if ( hit->solid == SOLID_BSP)
			{
				GroundEntity = hit->Entity;
				GroundEntityLinkCount = GroundEntity->GetLinkCount();
			}
		}
		if (!trace.plane.normal[2])
			blocked |= 2;		// step

//
// run the impact function
//
		Impact (&trace);
		if (!GetInUse())
			break;		// removed by the impact function
	
		time_left -= time_left * trace.fraction;
		
	// cliped to another plane
		if (numplanes >= MAX_CLIP_PLANES)
		{
			// this shouldn't really happen
			Velocity.Clear ();
			return 3;
		}

		planes[numplanes++] = trace.plane.normal;

//
// modify original_velocity so it parallels all of the clip planes
//
		for (i = 0; i < numplanes; i++)
		{
			ClipVelocity (original_velocity, planes[i], new_velocity, 1);

			for (j = 0; j < numplanes; j++)
			{
				if ((j != i) && planes[i] != planes[j])
				{
					if ((new_velocity | planes[j]) < 0)
						break;	// not ok
				}
			}

			if (j == numplanes)
				break;
		}
		
		if (i != numplanes)
		{
			// go along this plane
			Velocity = new_velocity;
		}
		else
		{
			// go along the crease
			if (numplanes != 2)
			{
				Velocity.Clear ();
				return 7;
			}
			dir = planes[0] ^ planes[1];
			d = dir | Velocity;
			Velocity = dir * d;
		}

//
// if original velocity is against the original velocity, stop dead
// to avoid tiny occilations in sloping corners
//
		if ((Velocity | primal_velocity) <= 0)
		{
			Velocity.Clear ();
			return blocked;
		}
	}

	return blocked;
}

bool IStepPhysics::Run ()
{
	bool		hitsound = false;
	float		speed, newspeed, control;
	float		friction;
	vec3f		saveOrigin = State.GetOrigin();

	if (PhysicsDisabled)
		return false;

	// airborn monsters should always check for ground
	if (!GroundEntity && (EntityFlags & ENT_MONSTER))
		(entity_cast<CMonsterEntity>(this))->Monster->CheckGround ();
	else if (!(EntityFlags & ENT_MONSTER))
		CheckGround (); // Specific non-monster checkground

	bool wasonground = (GroundEntity) ? true : false;
		
	if (AngularVelocity != vec3fOrigin)
		AddRotationalFriction ();

	// add gravity except:
	//   flying monsters
	//   swimming monsters who are in the water
	if (!wasonground)
	{
		if (!(Flags & FL_FLY))
		{
			if (!((Flags & FL_SWIM) && (WaterInfo.Level > WATER_WAIST)))
			{
				if (Velocity.Z < CvarList[CV_GRAVITY].Float() * -0.1)
					hitsound = true;
				if (WaterInfo.Level == WATER_NONE)
					AddGravity ();
			}
		}
	}

	// friction for flying monsters that have been given vertical velocity
	if ((Flags & FL_FLY) && (Velocity.Z != 0))
	{
		speed = Q_fabs(Velocity.Z);
		control = (speed < SV_STOPSPEED) ? SV_STOPSPEED : speed;
		friction = SV_FRICTION/3;
		newspeed = speed - (0.1f * control * friction);
		if (newspeed < 0)
			newspeed = 0;
		newspeed /= speed;
		Velocity.Z *= newspeed;
	}

	// friction for flying monsters that have been given vertical velocity
	if ((Flags & FL_SWIM) && (Velocity.Z != 0))
	{
		speed = Q_fabs(Velocity.Z);
		control = (speed < SV_STOPSPEED) ? SV_STOPSPEED : speed;
		newspeed = speed - (0.1f * control * SV_WATERFRICTION * WaterInfo.Level);
		if (newspeed < 0)
			newspeed = 0;
		newspeed /= speed;
		Velocity.Z *= newspeed;
	}

	if (Velocity != vec3fOrigin)
	{
		// apply friction
		// let dead monsters who aren't completely onground slide
		if ((wasonground) || (Flags & (FL_SWIM|FL_FLY)) && !(((EntityFlags & ENT_MONSTER) && entity_cast<CMonsterEntity>(this)->Health <= 0 && !(entity_cast<CMonsterEntity>(this))->Monster->CheckBottom())))
		{
			speed = sqrtf(Velocity.X*Velocity.X + Velocity.Y*Velocity.Y);
			if (speed)
			{
				friction = SV_FRICTION;

				control = (speed < SV_STOPSPEED) ? SV_STOPSPEED : speed;
				newspeed = speed - 0.1f*control*friction;

				if (newspeed < 0)
					newspeed = 0;
				newspeed /= speed;

				Velocity.X *= newspeed;
				Velocity.Y *= newspeed;
			}
		}

		FlyMove (0.1f,  (EntityFlags & ENT_MONSTER) ? CONTENTS_MASK_MONSTERSOLID : CONTENTS_MASK_SOLID);

		Link();

		GravityMultiplier = 1.0f;

		G_TouchTriggers (this);
		if (!GetInUse())
			return false;

		if (GroundEntity && !wasonground && hitsound)
			PlaySound (CHAN_AUTO, SoundIndex("world/land.wav"));
	}

	// if we moved, check and fix origin if needed
	if (State.GetOrigin() != saveOrigin)
	{
		CTrace tr (State.GetOrigin(), GetMins(), GetMaxs(), saveOrigin, this, CONTENTS_MASK_MONSTERSOLID);
		if(tr.allSolid || tr.startSolid)
		{
			State.GetOrigin() = saveOrigin;
			Link ();
		}
	}

	return true;
}

// Move physics
IPushPhysics::IPushPhysics() :
  IPhysicsEntity()
{
	PhysicsType = PHYSICS_PUSH;
};

IPushPhysics::IPushPhysics(sint32 Index) :
  IBaseEntity (Index),
  IPhysicsEntity(Index)
{
	PhysicsType = PHYSICS_PUSH;
};

class CPushed
{
public:
	IBaseEntity		*Entity;
	float			DeltaYaw;
	vec3f			Origin;
	vec3f			Angles;

	CPushed () :
	Entity(NULL),
	DeltaYaw(0),
	Origin(),
	Angles()
	{
	};

	CPushed (IBaseEntity *Entity, float DeltaYaw, vec3f Origin, vec3f Angles) :
	Entity(Entity),
	DeltaYaw(DeltaYaw),
	Origin(Origin),
	Angles(Angles)
	{
	};
};

IBaseEntity *obstacle;

/*
============
SV_TestEntityPosition

============
*/
inline IBaseEntity *SV_TestEntityPosition (IBaseEntity *Entity)
{
	return (CTrace(Entity->State.GetOrigin(), Entity->GetMins(), Entity->GetMaxs(), Entity->State.GetOrigin(), Entity, (Entity->GetClipmask()) ? Entity->GetClipmask() : CONTENTS_MASK_SOLID).startSolid) ? World : NULL;
}

/*
==================
ClipVelocity

Slide off of the impacting object
returns the blocked flags (1 = floor, 2 = step / wall)
==================
*/
#define STOP_EPSILON    0.1

sint32 ClipVelocity (vec3f &in, vec3f &normal, vec3f &out, float overbounce)
{
	sint32 blocked = 0;
	if (normal[2] > 0)
		blocked |= 1;           // floor
	if (!normal[2])
		blocked |= 2;           // step

	float backoff = (in | normal) * overbounce;

	for (sint32 i = 0; i < 3; i++)
	{
		float change = normal[i]*backoff;
		out[i] = in[i] - change;
		if (out[i] > -STOP_EPSILON && out[i] < STOP_EPSILON)
			out[i] = 0;
	}

	return blocked;
}

typedef std::vector<CPushed> TPushedList;

bool Push (TPushedList &Pushed, IBaseEntity *Entity, vec3f &move, vec3f &amove)
{
	// clamp the move to 1/8 units, so the position will
	// be accurate for client side prediction
	for (sint32 i = 0; i < 3; i++)
	{
		float temp = (move[i]*8.0);
		move[i] = 0.125 * (sint32)((temp > 0.0f) ? (temp + 0.5f) : (temp -0.5f));
	}

	// find the bounding box
	vec3f mins = Entity->GetAbsMin() + move;
	vec3f maxs = Entity->GetAbsMax() + move;

	// we need this for pushing things later
	vec3f forward, right, up;
	(-amove).ToVectors (&forward, &right, &up);

	// save the pusher's original position
	Pushed.push_back (CPushed (Entity,
	(Entity->EntityFlags & ENT_PLAYER) ? (entity_cast<CPlayerEntity>(Entity))->Client.PlayerState.GetPMove()->deltaAngles[YAW] : 0,
	Entity->State.GetOrigin(), Entity->State.GetAngles()));

	// move the pusher to it's final position
	Entity->State.GetOrigin() += move;
	Entity->State.GetAngles() += amove;
	Entity->Link ();

	// see if any solid entities are inside the final position
	for (TEntitiesContainer::iterator it = Level.Entities.Closed.begin()++; it != Level.Entities.Closed.end(); ++it)
	{
		IBaseEntity *Check = (*it)->Entity;

		if (!Check || !Check->GetInUse())
			continue;

		if (Check->EntityFlags & ENT_PHYSICS)
		{
			IPhysicsEntity *CheckPhys = entity_cast<IPhysicsEntity>(Check);
			if (CheckPhys->PhysicsType == PHYSICS_PUSH
				|| CheckPhys->PhysicsType == PHYSICS_STOP
				|| CheckPhys->PhysicsType == PHYSICS_NONE
				|| CheckPhys->PhysicsType == PHYSICS_NOCLIP)
				continue;
		}
		else if (Check->GetSolid() != SOLID_BBOX)
			continue;

		if ((Check->EntityFlags & ENT_PLAYER) && (entity_cast<CPlayerEntity>(Check)->NoClip))
			continue;

		if (!Check->GetArea()->prev)
			continue;               // not linked in anywhere

		// if the entity is standing on the pusher, it will definitely be moved
		if (Check->GroundEntity != Entity)
		{
			// see if the ent needs to be tested
			if (Check->GetAbsMin().X >= maxs.X
			|| Check->GetAbsMin().Y >= maxs.Y
			|| Check->GetAbsMin().Z >= maxs.Z
			|| Check->GetAbsMax().X <= mins.X
			|| Check->GetAbsMax().Y <= mins.Y
			|| Check->GetAbsMax().Z <= mins.Z)
				continue;

			// see if the ent's bbox is inside the pusher's final position
			if (!SV_TestEntityPosition (Check))
				continue;
		}

		if ((entity_cast<IPhysicsEntity>(Entity)->PhysicsType == PHYSICS_PUSH) || (Check->GroundEntity == Entity))
		{
			// move this entity
			CPushed PushedEntity (Check, 0, Check->State.GetOrigin(), Check->State.GetAngles());

			// try moving the contacted entity
			Check->State.GetOrigin() += move;

			if (Check->EntityFlags & ENT_PLAYER)
			{
				CPlayerEntity *Player = entity_cast<CPlayerEntity>(Check);
				Player->Client.PlayerState.GetPMove()->deltaAngles[YAW] += amove[YAW];

				//r1: dead-body-on-lift / other random view distortion fix
				PushedEntity.DeltaYaw = Player->Client.PlayerState.GetPMove()->deltaAngles[YAW];
			}
			else
				Check->State.GetAngles().Y += amove.Y;

			Pushed.push_back (PushedEntity);

			// figure movement due to the pusher's amove
			vec3f org = Check->State.GetOrigin() - Entity->State.GetOrigin ();
			Check->State.GetOrigin() += (vec3f (
				org | forward,
				-(org | right),
				org | up
				) - org);

			// may have pushed them off an edge
			if (Check->GroundEntity != Entity)
				Check->GroundEntity = NULL;

			if (!SV_TestEntityPosition (Check))
			{       // pushed ok
				Check->Link ();
				// impact?
				continue;
			}

			// if it is ok to leave in the old position, do it
			// this is only relevent for riding entities, not pushed
			// FIXME: this doesn't acount for rotation
			Check->State.GetOrigin() -= move;
			if (!SV_TestEntityPosition (Check))
			{
				Pushed.pop_back();
				continue;
			}
		}

		// save off the obstacle so we can call the block function
		obstacle = Check;

		// move back any entities we already moved
		// go backwards, so if the same entity was pushed
		// twice, it goes back to the original position
		for (TPushedList::reverse_iterator pit = Pushed.rbegin(); pit < Pushed.rend(); ++pit)
		{
			CPushed &PushedEntity = (*pit);

			PushedEntity.Entity->State.GetOrigin() = PushedEntity.Origin;
			PushedEntity.Entity->State.GetAngles() = PushedEntity.Angles;
			if (PushedEntity.Entity->EntityFlags & ENT_PLAYER)
				(entity_cast<CPlayerEntity>(PushedEntity.Entity))->Client.PlayerState.GetPMove()->deltaAngles[YAW] = PushedEntity.DeltaYaw;
			PushedEntity.Entity->Link ();
		}

		return false;
	}

	// see if anything we moved has touched a trigger
	for (TPushedList::reverse_iterator it = Pushed.rbegin(); it < Pushed.rend(); ++it)
	{
		CPushed &PushedEntity = *it;
		G_TouchTriggers (PushedEntity.Entity);
	}

	return true;
}

bool IPushPhysics::Run ()
{
	vec3f					move, amove;
	IBaseEntity				*part;
	TPushedList				Pushed;

	// if not a team captain, so movement will be handled elsewhere
	if (Flags & FL_TEAMSLAVE)
		return false;

	// make sure all team slaves can move before commiting
	// any moves or calling any think functions
	// if the move is blocked, all moved objects will be backed out
	for (part = this; part; part = part->Team.Chain)
	{
		if (!(part->EntityFlags & ENT_PHYSICS))
			continue;

		IPhysicsEntity *Phys = entity_cast<IPhysicsEntity>(part);

		if ((Phys->Velocity.X || Phys->Velocity.Y || Phys->Velocity.Z) ||
			(Phys->AngularVelocity.X || Phys->AngularVelocity.Y || Phys->AngularVelocity.Z))
		{
			// object is moving
			if (part->EntityFlags & ENT_PHYSICS)
			{
				move = Phys->Velocity;
				amove = Phys->AngularVelocity;
			}
			else
				move = amove = vec3fOrigin;

			if (!Push (Pushed, part, move, amove))
				break;
		}
	}
	if (Pushed.size() > MAX_CS_EDICTS)
		GameError ("Pushed.size() > MAX_CS_EDICTS, memory corrupted");

	if (part)
	{
		// the move failed, bump all nextthink times and back out moves
		for (IBaseEntity *mv = this; mv; mv = mv->Team.Chain)
		{
			if (mv->EntityFlags & ENT_THINKABLE)
			{
				IThinkableEntity *Thinkable = entity_cast<IThinkableEntity>(mv);

				if (Thinkable->NextThink > 0)
					Thinkable->NextThink += FRAMETIME;
			}
		}

		// if the pusher has a "blocked" function, call it
		// otherwise, just stay in place until the obstacle is gone
		if ((part->EntityFlags & ENT_BLOCKABLE) && obstacle)
			(entity_cast<IBlockableEntity>(part))->Blocked (obstacle);
	}
	else
	{
		// the move succeeded, so call all think functions
		for (part = this; part; part = part->Team.Chain)
		{
			if (part->EntityFlags & ENT_THINKABLE)
			{
				IThinkableEntity *Thinkable = entity_cast<IThinkableEntity>(part);
				Thinkable->RunThink ();
			}
		}
	}

	return true;
}



IStopPhysics::IStopPhysics () :
IPushPhysics()
{
	PhysicsType = PHYSICS_STOP;
};

IStopPhysics::IStopPhysics (sint32 Index) :
IPushPhysics(Index)
{
	PhysicsType = PHYSICS_STOP;
};

bool IStopPhysics::Run ()
{
	return IPushPhysics::Run ();
}

IBlockableEntity::IBlockableEntity () :
IBaseEntity ()
{
	EntityFlags |= ENT_BLOCKABLE;
}

IBlockableEntity::IBlockableEntity (sint32 Index) :
IBaseEntity (Index)
{
	EntityFlags |= ENT_BLOCKABLE;
}

IUsableEntity::IUsableEntity () :
IBaseEntity (),
NoiseIndex (0),
Delay (0),
Usable (true)
{
	EntityFlags |= ENT_USABLE;
}

IUsableEntity::IUsableEntity (sint32 Index) :
IBaseEntity (Index),
NoiseIndex (0),
Delay (0),
Usable (true)
{
	EntityFlags |= ENT_USABLE;
}

ENTITYFIELDS_BEGIN(IUsableEntity)
{
	CEntityField ("message",	EntityMemberOffset(IUsableEntity,Message),			FT_STRING | FT_SAVABLE),
	CEntityField ("noise",		EntityMemberOffset(IUsableEntity,NoiseIndex),		FT_SOUND_INDEX | FT_SAVABLE),
	CEntityField ("delay",		EntityMemberOffset(IUsableEntity,Delay),			FT_FRAMENUMBER | FT_SAVABLE),
	CEntityField ("target",		EntityMemberOffset(IUsableEntity,Target),			FT_LEVEL_STRING | FT_SAVABLE),
	CEntityField ("killtarget",	EntityMemberOffset(IUsableEntity,KillTarget),		FT_LEVEL_STRING | FT_SAVABLE),
	CEntityField ("pathtarget", EntityMemberOffset(IUsableEntity,PathTarget),		FT_LEVEL_STRING | FT_SAVABLE),

	CEntityField ("Usable", 	EntityMemberOffset(IUsableEntity,Usable),			FT_BOOL | FT_NOSPAWN | FT_SAVABLE),
	CEntityField ("User", 		EntityMemberOffset(IUsableEntity,User),				FT_ENTITY | FT_NOSPAWN | FT_SAVABLE),
};
ENTITYFIELDS_END(IUsableEntity)

bool			IUsableEntity::ParseField (const char *Key, const char *Value)
{
	if (CheckFields<IUsableEntity> (this, Key, Value))
		return true;

	// Couldn't find it here
	return false;
};

void			IUsableEntity::SaveFields (CFile &File)
{
	SaveEntityFields <IUsableEntity> (this, File);
};

void			IUsableEntity::LoadFields (CFile &File)
{
	LoadEntityFields <IUsableEntity> (this, File);
};

class CDelayedUse : public IThinkableEntity, public IUsableEntity
{
public:
	CDelayedUse () :
	  IBaseEntity (),
	  IThinkableEntity ()
	  {
	  };

	CDelayedUse (sint32 Index) :
	  IBaseEntity (Index),
	  IThinkableEntity (Index)
	  {
	  };

	IMPLEMENT_SAVE_HEADER(CDelayedUse)

	void SaveFields (CFile &File)
	{
		IThinkableEntity::SaveFields (File);
		IUsableEntity::SaveFields (File);
	}

	void LoadFields (CFile &File)
	{
		IThinkableEntity::LoadFields (File);
		IUsableEntity::LoadFields (File);
	}

	void Use (IBaseEntity *, IBaseEntity *)
	{
	};

	void Think ()
	{
		UseTargets (User, Message);
		Free ();
	}
};

IMPLEMENT_SAVE_SOURCE (CDelayedUse)

void IUsableEntity::UseTargets (IBaseEntity *Activator, std::string &Message)
{
//
// check for a delay
//
	if (Delay)
	{
	// create a temp object to fire at a later time
		CDelayedUse *t = QNewEntityOf CDelayedUse;
		t->ClassName = "DelayedUse";

		// Paril: for compatibility
		t->NextThink = Level.Frame + Delay;
		t->User = Activator;

		// Does this EVER happen? It needs to be called with an Activator...
		if (!Activator)
			DebugPrintf ("DelayedUse with no Activator\n");

		t->Message = Message;
		t->Target = Target;
		t->KillTarget = KillTarget;
		return;
	}

//
// print the message
//
	if ((!Message.empty()) && (Activator->EntityFlags & ENT_PLAYER))
	{
		CPlayerEntity *Player = entity_cast<CPlayerEntity>(Activator);
		Player->PrintToClient (PRINT_CENTER, "%s", Message.c_str());
		Player->PlaySound (CHAN_AUTO, (NoiseIndex) ? NoiseIndex : SoundIndex ("misc/talk1.wav"));
	}

//
// kill killtargets
//
	if (KillTarget)
	{
		IMapEntity *t = NULL;
		while ((t = CC_Find<IMapEntity, ENT_MAP, EntityMemberOffset(IMapEntity,TargetName)> (t, KillTarget)) != NULL)
		{
#if ROGUE_FEATURES
			// if this entity is part of a train, cleanly remove it
			if (t->Flags & FL_TEAMSLAVE)
			{
				if (t->Team.Master)
				{
					IBaseEntity *master = t->Team.Master;
					bool done = false;
					while (!done)
					{
						if (master->Team.Chain == t)
						{
							master->Team.Chain = t->Team.Chain;
							done = true;
						}
						master = master->Team.Chain;
					}
				}
			}
#endif

			t->Free ();

			if (!GetInUse())
			{
				MapPrint (MAPPRINT_WARNING, this, State.GetOrigin(), "Entity was removed while using killtargets\n");
				return;
			}
		}
	}

//
// fire targets
//
	if (Target)
	{
		IMapEntity *Ent = NULL;
		while ((Ent = CC_Find<IMapEntity, ENT_MAP, EntityMemberOffset(IMapEntity,TargetName)> (Ent, Target)) != NULL)
		{
			if (!Ent)
				continue;

			// doors fire area portals in a specific way
			if (!Q_stricmp(Ent->ClassName.c_str(), "func_areaportal") &&
				(!Q_stricmp(Ent->ClassName.c_str(), "func_door") || !Q_stricmp(Ent->ClassName.c_str(), "func_door_rotating")))
				continue;

			if (Ent->EntityFlags & ENT_USABLE)
			{
				IUsableEntity *Used = entity_cast<IUsableEntity>(Ent);
				
				if (Used == this)
					MapPrint (MAPPRINT_WARNING, this, State.GetOrigin(), "Entity used itself.\n");

				if (Used->Usable)
					Used->Use (this, Activator);
			}

			if (!GetInUse())
			{
				MapPrint (MAPPRINT_WARNING, this, State.GetOrigin(), "Entity was removed while using targets\n");
				return;
			}
		}
	}
}

// Buoyancy class
IBuoyancyPhysics::IBuoyancyPhysics () :
  IBaseEntity (),
  IPhysicsEntity ()
{
	PhysicsFlags |= PHYSICFLAG_BUOYANCY;
};

IBuoyancyPhysics::IBuoyancyPhysics (sint32 Index) :
  IBaseEntity (Index),
  IPhysicsEntity (Index)
{
	PhysicsFlags |= PHYSICFLAG_BUOYANCY;
};

void IBuoyancyPhysics::RunBouyancy ()
{
	EBrushContents Contents;
	float EndPower = BuoyancyPowerAir;
	bool InWater = false;

	Contents = PointContents(State.GetOrigin());

	if (Contents & CONTENTS_MASK_WATER)
	{
		EndPower = BuoyancyPowerWater;
		InWater = true;
	}

	PhysicsFlags &= ~PHYSICFLAG_FLOATING;

	if (VelocityEffect.Z < 60.0f && VelocityEffect.Z > -60.0f)
	{
		EBrushContents TopContents = PointContents (State.GetOrigin() + vec3f(0, 0, 2.4f));

		if (TopContents != Contents)
		{
			float TopPower = BuoyancyPowerAir;

			if (TopContents & CONTENTS_MASK_WATER)
				TopPower = BuoyancyPowerWater;

			EBrushContents MidContents = PointContents(State.GetOrigin() + vec3f(0, 0, 1.2f));
			float MidPower;

			if (MidContents == TopContents)
				MidPower = TopPower;
			else
				MidPower = EndPower;

			EndPower = (EndPower + TopPower + MidPower) / 3;
			DampeningEffect.Z *= powf (0.2f, FRAMETIME / 10);
			PhysicsFlags |= PHYSICFLAG_FLOATING;
		}
	}

	VelocityEffect.Z += (EndPower * CvarList[CV_GRAVITY].Float() * 0.1f);

	// Add
	Velocity *= DampeningEffect;
	Velocity += VelocityEffect;

	DampeningEffect.Set (1, 1, 1);
	VelocityEffect.Clear();
};

void IBuoyancyPhysics::SetBuoyancy (float Power, float ModAir, float ModWater)
{
	float BuoyPower = Power / Mass;

	BuoyancyPowerAir = BuoyPower * ModAir;
	BuoyancyPowerWater = BuoyPower * ModWater;
};

// Resistance class
IResistancePhysics::IResistancePhysics () :
  IBaseEntity (),
  IPhysicsEntity ()
{
	PhysicsFlags |= PHYSICFLAG_RESISTANCE;
};

IResistancePhysics::IResistancePhysics (sint32 Index) :
  IBaseEntity (Index),
  IPhysicsEntity (Index)
{
	PhysicsFlags |= PHYSICFLAG_RESISTANCE;
};

void IResistancePhysics::RunResistance ()
{
	float ResistPower = ResistPowerAir;
	EBrushContents Contents = PointContents(State.GetOrigin());

	if (Contents & CONTENTS_MASK_WATER)
		ResistPower = ResistPowerWater;

	ResistPower = powf (ResistPower, (float)FRAMETIME / 10);
	DampeningEffect *= ResistPower;
	ADampeningEffect *= ResistPower;
};

void IResistancePhysics::SetResistance (float Power, float ModAir, float ModWater)
{
	ResistPowerAir = 1.0f / (((Power * ModAir * 10.0f) / Mass) + 1.0f);
	ResistPowerWater = 1.0f / (((Power * ModWater * 10.0f) / Mass) + 1.0f);
};

// Aerodynamics class
IAerodynamicPhysics::IAerodynamicPhysics () :
  IBaseEntity (),
  IPhysicsEntity ()
{
	PhysicsFlags |= PHYSICFLAG_AERODYNAMICS;
};

IAerodynamicPhysics::IAerodynamicPhysics (sint32 Index) :
  IBaseEntity (Index),
  IPhysicsEntity (Index)
{
	PhysicsFlags |= PHYSICFLAG_AERODYNAMICS;
};

void IAerodynamicPhysics::RunAerodynamics ()
{
	if (Velocity.IsNearlyZero())
		return;

	float MoveSpeed = Velocity.LengthFast(), Power, AVelMult, ADampMult;
	vec3f MoveAngle;

	if (AeroPower > 0)
	{
		Power = AeroPower * 0.01f;
		MoveAngle = Velocity.ToAngles();
	}
	else
	{
		Power = AeroPower * -0.01f;
		MoveAngle = (vec3fOrigin - Velocity).ToAngles();
	}

	if (PhysicsFlags & PHYSICFLAG_FLOATING)
	{
		MoveAngle.X = 0;
		MoveSpeed += 200;
	}

	AVelMult = MoveSpeed * Power;
	ADampMult = 1.0 / ((MoveSpeed * Power * 0.2) + 1.0f);

	vec3f TurnAngle = MoveAngle - State.GetAngles();
	while (TurnAngle.X > 180)
		TurnAngle.X -= 360;
	while (TurnAngle.X < -180)
		TurnAngle.X += 360;
	while (TurnAngle.Y > 180)
		TurnAngle.Y -= 360;
	while (TurnAngle.Y < -180)
		TurnAngle.Y += 360;
	while (TurnAngle.Z > 180)
		TurnAngle.Z -= 360;
	while (TurnAngle.Z < -180)
		TurnAngle.Z += 360;

	AVelocityEffect += (TurnAngle * AVelMult * ((float)FRAMETIME / 10));
	ADampeningEffect *= powf (ADampMult, (float)FRAMETIME / 10);
};

void IAerodynamicPhysics::SetAerodynamics (float Power)
{
	AeroPower = Power / Mass;
};
