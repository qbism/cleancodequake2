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

CHurtableEntity::CHurtableEntity () :
CBaseEntity(),
CanTakeDamage (false)
{
	EntityFlags |= ENT_HURTABLE;
};

CHurtableEntity::CHurtableEntity (sint32 Index) :
CBaseEntity(Index),
CanTakeDamage (false)
{
	EntityFlags |= ENT_HURTABLE;
};

ENTITYFIELDS_BEGIN(CHurtableEntity)
{
	CEntityField ("health", EntityMemberOffset(CHurtableEntity,Health), FT_INT | FT_SAVABLE),

	CEntityField ("CanTakeDamage", EntityMemberOffset(CHurtableEntity,CanTakeDamage), FT_BOOL | FT_NOSPAWN | FT_SAVABLE),
	CEntityField ("DeadFlag", EntityMemberOffset(CHurtableEntity,DeadFlag), FT_BOOL | FT_NOSPAWN | FT_SAVABLE),
	CEntityField ("MaxHealth", EntityMemberOffset(CHurtableEntity,MaxHealth), FT_INT | FT_NOSPAWN | FT_SAVABLE),
	CEntityField ("GibHealth", EntityMemberOffset(CHurtableEntity,GibHealth), FT_INT | FT_NOSPAWN | FT_SAVABLE),
};
ENTITYFIELDS_END(CHurtableEntity)

bool			CHurtableEntity::ParseField (const char *Key, const char *Value)
{
	if (CheckFields<CHurtableEntity> (this, Key, Value))
		return true;

	// Couldn't find it here
	return false;
};

void			CHurtableEntity::SaveFields (CFile &File)
{
	SaveEntityFields<CHurtableEntity> (this, File);
};

void			CHurtableEntity::LoadFields (CFile &File)
{
	LoadEntityFields<CHurtableEntity> (this, File);
};

char *ClientTeam (CPlayerEntity *ent)
{
	char		*p;
	static char	value[512];

	value[0] = 0;

	Q_strncpyz(value, Info_ValueForKey (ent->Client.Persistent.UserInfo, "skin").c_str(), sizeof(value));
	p = strchr(value, '/');
	if (!p)
		return value;

	if (dmFlags.dfModelTeams)
	{
		*p = 0;
		return value;
	}

	// if ((sint32)(dmflags->value) & DF_SKINTEAMS)
	return ++p;
}

bool OnSameTeam (CPlayerEntity *ent1, CPlayerEntity *ent2)
{
	char	ent1Team [512];
	char	ent2Team [512];

	if (!(dmFlags.dfSkinTeams || dmFlags.dfModelTeams))
		return false;

	Q_strncpyz (ent1Team, ClientTeam (ent1), sizeof(ent1Team));
	Q_strncpyz (ent2Team, ClientTeam (ent2), sizeof(ent2Team));

	if (strcmp(ent1Team, ent2Team) == 0)
		return true;
	return false;
}

bool CHurtableEntity::CanDamage (CBaseEntity *inflictor)
{
// bmodels need special checking because their origin is 0,0,0
	if ((EntityFlags & ENT_PHYSICS) && ((entity_cast<CPhysicsEntity>(this))->PhysicsType == PHYSICS_PUSH))
	{
		vec3f dest = (GetAbsMin() + GetAbsMax()) * 0.5f;
		CTrace trace (inflictor->State.GetOrigin(), dest, inflictor, CONTENTS_MASK_SOLID);
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
		CTrace trace (inflictor->State.GetOrigin(), end, inflictor, CONTENTS_MASK_SOLID);
		if (trace.fraction == 1.0)
			return true;
	};

	return false;
}

bool CHurtableEntity::CheckTeamDamage (CBaseEntity *attacker)
{
#if CLEANCTF_ENABLED
//ZOID
	if ((game.mode & GAME_CTF) && (EntityFlags & ENT_PLAYER) && (attacker->EntityFlags & ENT_PLAYER))
	{
		CPlayerEntity *Targ = entity_cast<CPlayerEntity>(this);
		CPlayerEntity *Attacker = entity_cast<CPlayerEntity>(attacker);
		if (Targ->Client.Respawn.CTF.Team == Attacker->Client.Respawn.CTF.Team &&
			(this != attacker))
			return true;
	}
//ZOID
#endif

	return false;
}

#include "cc_tent.h"

sint32 CHurtableEntity::CheckPowerArmor (vec3f &point, vec3f &normal, sint32 damage, sint32 dflags)
{
	if (!damage)
		return 0;

	if (dflags & DAMAGE_NO_ARMOR)
		return 0;

	static const sint32	index = NItems::Cells->GetIndex();

	sint32			power_armor_type, damagePerCell, pa_te_type, power = 0, power_used;

	bool		IsClient = !!(EntityFlags & ENT_PLAYER),
				IsMonster = !!(EntityFlags & ENT_MONSTER);
	CPlayerEntity	*Player = (IsClient) ? entity_cast<CPlayerEntity>(this) : NULL;
	CMonsterEntity	*Monster = (IsMonster) ? entity_cast<CMonsterEntity>(this) : NULL;

	if (IsClient)
	{
		power_armor_type = Player->PowerArmorType ();
		if (power_armor_type != POWER_ARMOR_NONE)
			power = Player->Client.Persistent.Inventory.Has(index);
	}
	else if (IsMonster)
	{
		power_armor_type = Monster->Monster->PowerArmorType;
		power = Monster->Monster->PowerArmorPower;
	}
	else
		return 0;

	if (!power)
		return 0;

	switch (power_armor_type)
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
			if (vec.Dot (forward) <= 0.3)
				return 0;

			damagePerCell = 1;
			pa_te_type = TE_SCREEN_SPARKS;
			damage = damage / 3;
		}
		break;
	case POWER_ARMOR_SHIELD:
		damagePerCell = 2;
		pa_te_type = TE_SHIELD_SPARKS;
		damage = (2 * damage) / 3;
		break;
	};

	sint32 save = power * damagePerCell;
	if (!save)
		return 0;
	if (save > damage)
		save = damage;

	CTempEnt_Splashes::ShieldSparks (point, normal, (pa_te_type == TE_SCREEN_SPARKS) ? true : false);

	power_used = save / damagePerCell;
	if (!power_used)
		power_used = 1;

	if (IsClient)
	{
		Player->Client.Persistent.Inventory.Remove(index, power_used);
		Player->Client.PowerArmorTime = 2;
	}
	else if (IsMonster)
	{
		Monster->Monster->PowerArmorPower -= power_used;
		Monster->Monster->PowerArmorTime = 2;
	}
	return save;
}

#if MONSTER_USE_ROGUE_AI
/*
ROGUE
clean up heal targets for medic
*/
void CleanupHealTarget (CMonsterEntity *ent)
{
	ent->Monster->Healer = NULL;
	ent->CanTakeDamage = true;
	ent->Monster->AIFlags &= ~AI_RESURRECTING;
	ent->Monster->SetEffects ();
}
#endif

void CHurtableEntity::Killed (CBaseEntity *inflictor, CBaseEntity *attacker, sint32 damage, vec3f &point)
{
	if (Health < -999)
		Health = -999;

	Enemy = attacker;

#if MONSTER_USE_ROGUE_AI
	if (EntityFlags & ENT_MONSTER)
	{
		CMonsterEntity *Monster = entity_cast<CMonsterEntity>(this);
		if (Monster->Monster->AIFlags & AI_MEDIC)
		{
			if (Monster->Enemy && (Monster->Enemy->EntityFlags & ENT_MONSTER))  // god, I hope so
				CleanupHealTarget (entity_cast<CMonsterEntity>(Monster->Enemy));

			// clean up self
			Monster->Monster->AIFlags &= ~AI_MEDIC;
			Monster->Enemy = attacker;
		}
	}
#endif

	if ((!DeadFlag) && (EntityFlags & ENT_MONSTER))
	{
		if (!((entity_cast<CMonsterEntity>(this))->Monster->AIFlags & AI_GOOD_GUY))
		{
			level.Monsters.Killed++;
			if ((game.mode == GAME_COOPERATIVE) && (attacker->EntityFlags & ENT_PLAYER))
				(entity_cast<CPlayerEntity>(attacker))->Client.Respawn.Score++;
			// medics won't heal monsters that they kill themselves

#if !MONSTER_USE_ROGUE_AI
			if (strcmp(attacker->ClassName, "monster_medic") == 0)
				SetOwner (attacker);
#endif
		}
	}

	if ((EntityFlags & ENT_MONSTER) && (!DeadFlag))
	{
		if (EntityFlags & ENT_TOUCHABLE)
			(entity_cast<CTouchableEntity>(this))->Touchable = false;
		if (EntityFlags & ENT_MONSTER)
			(entity_cast<CMonsterEntity>(this))->Monster->MonsterDeathUse();
	}

	if (((EntityFlags & ENT_HURTABLE) && entity_cast<CHurtableEntity>(this)->CanTakeDamage))
		(entity_cast<CHurtableEntity>(this))->Die (inflictor, attacker, damage, point);
}

void CHurtableEntity::DamageEffect (vec3f &dir, vec3f &point, vec3f &normal, sint32 &damage, sint32 &dflags)
{
	if ((EntityFlags & ENT_MONSTER) || (EntityFlags & ENT_PLAYER))
		CTempEnt_Splashes::Blood (point, normal);
	else
		CTempEnt_Splashes::Sparks (point, normal, (dflags & DAMAGE_BULLET) ? CTempEnt_Splashes::ST_BULLET_SPARKS : CTempEnt_Splashes::ST_SPARKS, CTempEnt_Splashes::SPT_SPARKS);
}

void CHurtableEntity::TakeDamage (CBaseEntity *inflictor, CBaseEntity *attacker,
								vec3f dir, vec3f point, vec3f normal, sint32 damage,
								sint32 knockback, sint32 dflags, EMeansOfDeath mod)
{
	if (map_debug->Boolean())
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
	if ((this != attacker) && ((game.mode & GAME_DEATHMATCH) && (dmFlags.dfSkinTeams || dmFlags.dfModelTeams) || game.mode == GAME_COOPERATIVE))
	{
		if ((EntityFlags & ENT_PLAYER) && attacker && (attacker->EntityFlags & ENT_PLAYER))
		{
			if (OnSameTeam (Player, entity_cast<CPlayerEntity>(attacker)))
			{
				if (dmFlags.dfNoFriendlyFire)
					damage = 0;
				else
					mod |= MOD_FRIENDLY_FIRE;
			}
		}
	}
	meansOfDeath = mod;

	// easy mode takes half damage
	if (skill->Integer() == 0 && !(game.mode & GAME_DEATHMATCH) && (EntityFlags & ENT_PLAYER))
	{
		damage *= 0.5;
		if (!damage)
			damage = 1;
	}

	dir.Normalize ();

// bonus damage for surprising a monster
// Paril revision: Allow multiple pellet weapons to take advantage of this too!
	if (!(dflags & DAMAGE_RADIUS) && (EntityFlags & ENT_MONSTER) && (attacker->EntityFlags & ENT_PLAYER))
	{
		CMonsterEntity *Monster = entity_cast<CMonsterEntity>(this);

		if ((Health > 0) &&
			(!Enemy && (Monster->BonusDamageTime <= level.Frame)) ||
			(Enemy && (Monster->BonusDamageTime == level.Frame)))
		{
			Monster->BonusDamageTime = level.Frame;
			damage *= 2;
		}
	}

	if (dmFlags.dfDmTechs
#if CLEANCTF_ENABLED
	|| (game.mode & GAME_CTF)
#endif
	)
	{
		if (isClient)
		{
			if (Client->Persistent.Tech && (Client->Persistent.Tech->TechType == CTech::TECH_AGGRESSIVE))
				Client->Persistent.Tech->DoAggressiveTech (Player, attacker, false, damage, knockback, dflags, mod, true);
		}

		if (attacker->EntityFlags & ENT_PLAYER)
		{
			CPlayerEntity *Atk = entity_cast<CPlayerEntity>(attacker);
			if (Atk->Client.Persistent.Tech && (Atk->Client.Persistent.Tech->TechType == CTech::TECH_AGGRESSIVE))
				entity_cast<CPlayerEntity>(attacker)->Client.Persistent.Tech->DoAggressiveTech (Atk, Player, false, damage, knockback, dflags, mod, false);
		}
	}

	if (Flags & FL_NO_KNOCKBACK)
		knockback = 0;

// figure momentum add
	if (knockback && !(dflags & DAMAGE_NO_KNOCKBACK) && (EntityFlags & ENT_PHYSICS))
	{
		CPhysicsEntity *Phys = entity_cast<CPhysicsEntity>(this);
		if (!(Phys->PhysicsType == PHYSICS_NONE ||
			Phys->PhysicsType == PHYSICS_BOUNCE ||
			Phys->PhysicsType == PHYSICS_PUSH ||
			Phys->PhysicsType == PHYSICS_STOP))
			Phys->Velocity += dir * (((isClient && (attacker == this)) ? 1600.0f : 500.0f) * (float)knockback / Clamp<float> (Phys->Mass, 50, Phys->Mass));
	}

	take = damage;
	save = 0;

	// check for godmode
	if ( (Flags & FL_GODMODE) && !(dflags & DAMAGE_NO_PROTECTION) )
	{
		take = 0;
		save = damage;
		CTempEnt_Splashes::Sparks (point, normal, (dflags & DAMAGE_BULLET) ? CTempEnt_Splashes::ST_BULLET_SPARKS : CTempEnt_Splashes::ST_SPARKS, CTempEnt_Splashes::SPT_SPARKS);
	}

	// check for invincibility
	if ((isClient && (Client->Timers.Invincibility > level.Frame) ) && !(dflags & DAMAGE_NO_PROTECTION))
	{
		if (Player->PainDebounceTime < level.Frame)
		{
			PlaySound (CHAN_ITEM, SoundIndex("items/protect4.wav"));
			Player->PainDebounceTime = level.Frame + 20;
		}
		take = 0;
		save = damage;
	}

#if CLEANCTF_ENABLED
//ZOID
//team armor protect
	if ((game.mode & GAME_CTF) && isClient && (attacker->EntityFlags & ENT_PLAYER) &&
		(Client->Respawn.CTF.Team == (entity_cast<CPlayerEntity>(attacker))->Client.Respawn.CTF.Team) &&
		(this != attacker) && dmFlags.dfCtfArmorProtect)
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

	if (dmFlags.dfDmTechs
#if CLEANCTF_ENABLED
	|| (game.mode & GAME_CTF)
#endif
	)
	{
		if (isClient)
		{
			if (Client->Persistent.Tech && (Client->Persistent.Tech->TechType == CTech::TECH_AGGRESSIVE))
				Client->Persistent.Tech->DoAggressiveTech (entity_cast<CPlayerEntity>(this), attacker, true, take, knockback, dflags, mod, true);
		}

		if ((EntityFlags & ENT_PLAYER) && (attacker->EntityFlags & ENT_PLAYER))
		{
			CPlayerEntity *Atk = entity_cast<CPlayerEntity>(attacker);
			if (Atk->Client.Persistent.Tech && (Atk->Client.Persistent.Tech->TechType == CTech::TECH_AGGRESSIVE))
				entity_cast<CPlayerEntity>(attacker)->Client.Persistent.Tech->DoAggressiveTech (Atk, entity_cast<CPlayerEntity>(this), true, damage, knockback, dflags, mod, false);
		}
	}

	// team damage avoidance
	if (!(dflags & DAMAGE_NO_PROTECTION) && CheckTeamDamage (attacker))
		return;

#if CLEANCTF_ENABLED
//ZOID
	if ((game.mode & GAME_CTF) && (isClient && (attacker->EntityFlags & ENT_PLAYER)))
		CTFCheckHurtCarrier((entity_cast<CPlayerEntity>(this)), (entity_cast<CPlayerEntity>(attacker)));
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
			
		if (Health <= 0)
		{
			if ((EntityFlags & ENT_MONSTER) || (isClient))
				Flags |= FL_NO_KNOCKBACK;
			Killed (inflictor, attacker, take, point);
			return;
		}
	}

	if ((EntityFlags & ENT_MONSTER))
	{
		CMonster *Monster = (entity_cast<CMonsterEntity>(this))->Monster;
		Monster->ReactToDamage (attacker);
		if (!(Monster->AIFlags & AI_DUCKED) && take)
		{
			Pain (attacker, knockback, take);
			if (skill->Integer() == 3)
				Monster->PainDebounceTime = level.Frame + 50;
		}
	}
	else if (((EntityFlags & ENT_PLAYER) && take
#if CLEANCTF_ENABLED
		&& !CTFMatchSetup()
#endif
		) || take)
		Pain (attacker, knockback, take);

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
	
void CHurtableEntity::TakeDamage (CBaseEntity *targ, CBaseEntity *inflictor,
								CBaseEntity *attacker, vec3f dir, vec3f point,
								vec3f normal, sint32 damage, sint32 knockback,
								sint32 dflags, EMeansOfDeath mod)
{
	if ((targ->EntityFlags & ENT_HURTABLE) && entity_cast<CHurtableEntity>(targ)->CanTakeDamage)
		(entity_cast<CHurtableEntity>(targ))->TakeDamage (inflictor, attacker, dir, point, normal, damage, knockback, dflags, mod);
}

CThinkableEntity::CThinkableEntity () :
CBaseEntity()
{
	EntityFlags |= ENT_THINKABLE;
};

CThinkableEntity::CThinkableEntity (sint32 Index) :
CBaseEntity(Index)
{
	EntityFlags |= ENT_THINKABLE;
};

void CThinkableEntity::SaveFields (CFile &File)
{
	// Save NextThink
	File.Write<FrameNumber_t> (NextThink);
};

void CThinkableEntity::LoadFields (CFile &File)
{
	// Load NextThink
	NextThink = File.Read<FrameNumber_t> ();
};

void CThinkableEntity::RunThink ()
{
	if (NextThink <= 0 || NextThink > level.Frame)
		return;
	
	NextThink = 0;
	Think ();
}

CTouchableEntity::CTouchableEntity () :
CBaseEntity()
{
	EntityFlags |= ENT_TOUCHABLE;
};

CTouchableEntity::CTouchableEntity (sint32 Index) :
CBaseEntity(Index)
{
	EntityFlags |= ENT_TOUCHABLE;
};

void CTouchableEntity::SaveFields (CFile &File)
{
	File.Write<bool> (Touchable);
};

void CTouchableEntity::LoadFields (CFile &File)
{
	Touchable = File.Read<bool> ();
};

void CTouchableEntity::Touch(CBaseEntity *other, plane_t *plane, cmBspSurface_t *surf)
{
}

CPhysicsEntity::CPhysicsEntity () :
CBaseEntity(),
GravityMultiplier(1.0f)
{
	EntityFlags |= ENT_PHYSICS;
	PhysicsType = PHYSICS_NONE;
};

CPhysicsEntity::CPhysicsEntity (sint32 Index) :
CBaseEntity(Index),
GravityMultiplier(1.0f)
{
	EntityFlags |= ENT_PHYSICS;
	PhysicsType = PHYSICS_NONE;
};

void CPhysicsEntity::AddGravity()
{
	Velocity.Z -= GravityMultiplier * sv_gravity->Float() * 0.1f;
}

CTrace CPhysicsEntity::PushEntity (vec3f &push)
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

void CPhysicsEntity::Impact (CTrace *trace)
{
	if (!trace->ent->Entity)
		return;

	if (GetSolid() != SOLID_NOT && (EntityFlags & ENT_TOUCHABLE))
	{
		CTouchableEntity *Touched = entity_cast<CTouchableEntity>(this);

		if (Touched->Touchable)
			Touched->Touch (trace->Ent, &trace->plane, trace->surface);
	}

	if ((trace->Ent->EntityFlags & ENT_TOUCHABLE) && trace->Ent->GetSolid() != SOLID_NOT)
	{
		CTouchableEntity *Touched = entity_cast<CTouchableEntity>(trace->Ent);

		if (Touched->Touchable)
			Touched->Touch (this, NULL, NULL);
	}
}

CBounceProjectile::CBounceProjectile () :
backOff(1.5f),
CPhysicsEntity ()
{
	PhysicsType = PHYSICS_BOUNCE;
}

CBounceProjectile::CBounceProjectile (sint32 Index) :
backOff(1.5f),
CPhysicsEntity (Index)
{
	PhysicsType = PHYSICS_BOUNCE;
}

sint32 ClipVelocity (vec3f &in, vec3f &normal, vec3f &out, float overbounce);
bool CBounceProjectile::Run ()
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
	if ( GroundEntity )
		return false;

	old_origin = State.GetOrigin();

// add gravity
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

		// stop if on ground
		if (trace.plane.normal[2] > 0.9)
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
	for (CBaseEntity *slave = Team.Chain; slave; slave = slave->Team.Chain)
	{
		slave->State.GetOrigin() = State.GetOrigin();
		slave->Link ();
	}

	return true;
}

CTossProjectile::CTossProjectile () :
CBounceProjectile ()
{
	backOff = 1.0f;

	PhysicsType = PHYSICS_TOSS;
}

CTossProjectile::CTossProjectile (sint32 Index) :
CBounceProjectile (Index)
{
	backOff = 1.0f;

	PhysicsType = PHYSICS_TOSS;
}

CFlyMissileProjectile::CFlyMissileProjectile () :
CPhysicsEntity ()
{
	PhysicsType = PHYSICS_FLYMISSILE;
}

CFlyMissileProjectile::CFlyMissileProjectile (sint32 Index) :
CPhysicsEntity (Index)
{
	PhysicsType = PHYSICS_FLYMISSILE;
}

bool CFlyMissileProjectile::Run ()
{
	// if not a team captain, so movement will be handled elsewhere
	if (Flags & FL_TEAMSLAVE)
		return false;

	if (Velocity.Z > 0)
		GroundEntity = NULL;

// check for the groundentity going away
	if (GroundEntity && !GroundEntity->GetInUse())
		GroundEntity = NULL;

// if onground, return without moving
	if ( GroundEntity )
		return false;

	vec3f old_origin = State.GetOrigin();

// move angles
	State.GetAngles() = State.GetAngles().MultiplyAngles (0.1f, AngularVelocity);

// move origin
	vec3f move = Velocity * 0.1f;
	CTrace trace = PushEntity (move);

	if (!GetInUse())
		return false;

	if (trace.fraction < 1)
	{
		ClipVelocity (Velocity, trace.plane.normal, Velocity, 1.0);

		// stop if on ground
		if (trace.plane.normal[2] > 0.9)
		{		
			GroundEntity = trace.Ent;
			GroundEntityLinkCount = GroundEntity->GetLinkCount();
			Velocity.Clear ();
			AngularVelocity.Clear ();
		}
	}
	
// check for water transition
	bool wasinwater = (WaterInfo.Type & CONTENTS_MASK_WATER) ? true : false;
	WaterInfo.Type = PointContents (State.GetOrigin());
	bool isinwater = (WaterInfo.Type & CONTENTS_MASK_WATER) ? true : false;

	WaterInfo.Level = (isinwater) ? WATER_FEET : WATER_NONE;

	if (!wasinwater && isinwater)
		World->PlayPositionedSound (old_origin, CHAN_AUTO, SoundIndex("misc/h2ohit1.wav"));
	else if (wasinwater && !isinwater)
		World->PlayPositionedSound (State.GetOrigin(), CHAN_AUTO, SoundIndex("misc/h2ohit1.wav"));

// move teamslaves
	for (CBaseEntity *slave = Team.Chain; slave; slave = slave->Team.Chain)
	{
		slave->State.GetOrigin() = State.GetOrigin();
		slave->Link ();
	}

	return true;
}

CStepPhysics::CStepPhysics () :
CPhysicsEntity ()
{
	PhysicsType = PHYSICS_STEP;
}

CStepPhysics::CStepPhysics (sint32 Index) :
CPhysicsEntity (Index)
{
	PhysicsType = PHYSICS_STEP;
}

void CStepPhysics::CheckGround ()
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

void CStepPhysics::AddRotationalFriction ()
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
sint32 CStepPhysics::FlyMove (float time, sint32 mask)
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
					if (new_velocity.Dot (planes[j]) < 0)
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
				//gi.dprintf ("clip velocity, numplanes == %i\n",numplanes);
				Velocity.Clear ();
				return 7;
			}
			dir = planes[0].Cross (planes[1]);
			d = dir.Dot (Velocity);
			Velocity = dir * d;
		}

//
// if original velocity is against the original velocity, stop dead
// to avoid tiny occilations in sloping corners
//
		if (Velocity.Dot (primal_velocity) <= 0)
		{
			Velocity.Clear ();
			return blocked;
		}
	}

	return blocked;
}

bool CStepPhysics::Run ()
{
	bool		hitsound = false;
	float		speed, newspeed, control;
	float		friction;

	if (PhysicsDisabled)
		return false;

	// airborn monsters should always check for ground
	if (!GroundEntity && (EntityFlags & ENT_MONSTER))
		(entity_cast<CMonsterEntity>(this))->Monster->CheckGround ();
	else
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
				if (Velocity.Z < sv_gravity->Float() * -0.1)
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
		G_TouchTriggers (this);
		if (!GetInUse())
			return false;

		if (GroundEntity && !wasonground && hitsound)
			PlaySound (CHAN_AUTO, SoundIndex("world/land.wav"));
	}
	return true;
}

// Move physics
CPushPhysics::CPushPhysics() :
CPhysicsEntity()
{
	PhysicsType = PHYSICS_PUSH;
};

CPushPhysics::CPushPhysics(sint32 Index) :
CPhysicsEntity(Index)
{
	PhysicsType = PHYSICS_PUSH;
};

class CPushed
{
public:
	CBaseEntity		*Entity;
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

	CPushed (CBaseEntity *Entity, float DeltaYaw, vec3f Origin, vec3f Angles) :
	Entity(Entity),
	DeltaYaw(DeltaYaw),
	Origin(Origin),
	Angles(Angles)
	{
	};
};

CBaseEntity *obstacle;

/*
============
SV_TestEntityPosition

============
*/
CBaseEntity *SV_TestEntityPosition (CBaseEntity *ent)
{
	return (CTrace(ent->State.GetOrigin(), ent->GetMins(), ent->GetMaxs(), ent->State.GetOrigin(), ent, (ent->GetClipmask()) ? ent->GetClipmask() : CONTENTS_MASK_SOLID).startSolid) ? World : NULL;
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

	float backoff = in.Dot (normal) * overbounce;

	for (sint32 i = 0; i < 3; i++)
	{
		float change = normal[i]*backoff;
		out[i] = in[i] - change;
		if (out[i] > -STOP_EPSILON && out[i] < STOP_EPSILON)
			out[i] = 0;
	}

	return blocked;
}

typedef std::vector<CPushed, std::generic_allocator<CPushed> > TPushedList;

bool Push (TPushedList &Pushed, CBaseEntity *Entity, vec3f &move, vec3f &amove)
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
	for (TEntitiesContainer::iterator it = level.Entities.Closed.begin()++; it != level.Entities.Closed.end(); ++it)
	{
		CBaseEntity *Check = (*it)->Entity;

		if (!Check || !Check->GetInUse())
			continue;

		if (Check->EntityFlags & ENT_PHYSICS)
		{
			CPhysicsEntity *CheckPhys = entity_cast<CPhysicsEntity>(Check);
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

		if ((entity_cast<CPhysicsEntity>(Entity)->PhysicsType == PHYSICS_PUSH) || (Check->GroundEntity == Entity))
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
				Check->State.GetAngles() += vec3f (0, amove.Y, 0);

			Pushed.push_back (PushedEntity);

			// figure movement due to the pusher's amove
			vec3f org = Check->State.GetOrigin() - Entity->State.GetOrigin ();
			Check->State.GetOrigin() += (vec3f (
				org.Dot (forward),
				-org.Dot (right),
				org.Dot (up)
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

bool CPushPhysics::Run ()
{
	vec3f					move, amove;
	CBaseEntity				*part;
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

		CPhysicsEntity *Phys = entity_cast<CPhysicsEntity>(part);

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
		for (CBaseEntity *mv = this; mv; mv = mv->Team.Chain)
		{
			if (mv->EntityFlags & ENT_THINKABLE)
			{
				CThinkableEntity *Thinkable = entity_cast<CThinkableEntity>(mv);

				if (Thinkable->NextThink > 0)
					Thinkable->NextThink += FRAMETIME;
			}
		}

		// if the pusher has a "blocked" function, call it
		// otherwise, just stay in place until the obstacle is gone
		if ((part->EntityFlags & ENT_BLOCKABLE) && obstacle)
			(entity_cast<CBlockableEntity>(part))->Blocked (obstacle);
	}
	else
	{
		// the move succeeded, so call all think functions
		for (part = this; part; part = part->Team.Chain)
		{
			if (part->EntityFlags & ENT_THINKABLE)
			{
				CThinkableEntity *Thinkable = entity_cast<CThinkableEntity>(part);
				Thinkable->RunThink ();
			}
		}
	}

	return true;
}



CStopPhysics::CStopPhysics () :
CPushPhysics()
{
	PhysicsType = PHYSICS_STOP;
};

CStopPhysics::CStopPhysics (sint32 Index) :
CPushPhysics(Index)
{
	PhysicsType = PHYSICS_STOP;
};

bool CStopPhysics::Run ()
{
	return CPushPhysics::Run ();
}

CBlockableEntity::CBlockableEntity () :
CBaseEntity ()
{
	EntityFlags |= ENT_BLOCKABLE;
}

CBlockableEntity::CBlockableEntity (sint32 Index) :
CBaseEntity (Index)
{
	EntityFlags |= ENT_BLOCKABLE;
}

CUsableEntity::CUsableEntity () :
CBaseEntity (),
NoiseIndex (0),
Delay (0),
Usable (true)
{
	EntityFlags |= ENT_USABLE;
}

CUsableEntity::CUsableEntity (sint32 Index) :
CBaseEntity (Index),
NoiseIndex (0),
Delay (0),
Usable (true)
{
	EntityFlags |= ENT_USABLE;
}

ENTITYFIELDS_BEGIN(CUsableEntity)
{
	CEntityField ("message",	EntityMemberOffset(CUsableEntity,Message),			FT_CC_STRING | FT_SAVABLE),
	CEntityField ("noise",		EntityMemberOffset(CUsableEntity,NoiseIndex),		FT_SOUND_INDEX | FT_SAVABLE),
	CEntityField ("delay",		EntityMemberOffset(CUsableEntity,Delay),			FT_FRAMENUMBER | FT_SAVABLE),
	CEntityField ("target",		EntityMemberOffset(CUsableEntity,Target),			FT_LEVEL_STRING | FT_SAVABLE),
	CEntityField ("killtarget",	EntityMemberOffset(CUsableEntity,KillTarget),		FT_LEVEL_STRING | FT_SAVABLE),
	CEntityField ("pathtarget", EntityMemberOffset(CUsableEntity,PathTarget),		FT_LEVEL_STRING | FT_SAVABLE),

	CEntityField ("Usable", 	EntityMemberOffset(CUsableEntity,Usable),			FT_BOOL | FT_NOSPAWN | FT_SAVABLE),
	CEntityField ("Activator", 	EntityMemberOffset(CUsableEntity,Activator),		FT_ENTITY | FT_NOSPAWN | FT_SAVABLE),
};
ENTITYFIELDS_END(CUsableEntity)

bool			CUsableEntity::ParseField (const char *Key, const char *Value)
{
	if (CheckFields<CUsableEntity> (this, Key, Value))
		return true;

	// Couldn't find it here
	return false;
};

void			CUsableEntity::SaveFields (CFile &File)
{
	SaveEntityFields <CUsableEntity> (this, File);
};

void			CUsableEntity::LoadFields (CFile &File)
{
	LoadEntityFields <CUsableEntity> (this, File);
};

class CDelayedUse : public CThinkableEntity, public CUsableEntity
{
public:
	CDelayedUse () :
	  CBaseEntity (),
	  CThinkableEntity ()
	  {
	  };

	CDelayedUse (sint32 Index) :
	  CBaseEntity (Index),
	  CThinkableEntity (Index)
	  {
	  };

	IMPLEMENT_SAVE_HEADER(CDelayedUse)

	void SaveFields (CFile &File)
	{
		CThinkableEntity::SaveFields (File);
		CUsableEntity::SaveFields (File);
	}

	void LoadFields (CFile &File)
	{
		CThinkableEntity::LoadFields (File);
		CUsableEntity::LoadFields (File);
	}

	void Use (CBaseEntity *, CBaseEntity *)
	{
	};

	void Think ()
	{
		UseTargets (Activator, Message);
		Free ();
	}
};

IMPLEMENT_SAVE_SOURCE (CDelayedUse)

void CUsableEntity::UseTargets (CBaseEntity *activator, std::cc_string &Message)
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
		t->NextThink = level.Frame + Delay;
		t->Activator = activator;
		if (!activator)
			DebugPrintf ("DelayedUse with no activator\n");
		t->Message = Message;
		t->Target = Target;
		t->KillTarget = KillTarget;
		return;
	}

//
// print the message
//
	if ((!Message.empty()) && (activator->EntityFlags & ENT_PLAYER))
	{
		CPlayerEntity *Player = entity_cast<CPlayerEntity>(activator);
		Player->PrintToClient (PRINT_CENTER, "%s", Message.c_str());
		Player->PlaySound (CHAN_AUTO, (NoiseIndex) ? NoiseIndex : SoundIndex ("misc/talk1.wav"));
	}

//
// kill killtargets
//
	if (KillTarget)
	{
		CMapEntity *t = NULL;
		while ((t = CC_Find<CMapEntity, ENT_MAP, EntityMemberOffset(CMapEntity,TargetName)> (t, KillTarget)) != NULL)
		{
			t->Free ();

			if (!GetInUse())
			{
				DebugPrintf("entity was removed while using killtargets\n");
				return;
			}
		}
	}

//
// fire targets
//
	if (Target)
	{
		CMapEntity *Ent = NULL;
		while ((Ent = CC_Find<CMapEntity, ENT_MAP, EntityMemberOffset(CMapEntity,TargetName)> (Ent, Target)) != NULL)
		{
			if (!Ent)
				continue;

			// doors fire area portals in a specific way
			if (!Q_stricmp(Ent->ClassName, "func_areaportal") &&
				(!Q_stricmp(Ent->ClassName, "func_door") || !Q_stricmp(Ent->ClassName, "func_door_rotating")))
				continue;

			if (Ent->EntityFlags & ENT_USABLE)
			{
				CUsableEntity *Used = entity_cast<CUsableEntity>(Ent);
				
				if (Used == this)
					DebugPrintf ("WARNING: Entity used itself.\n");

				if (Used->Usable)
					Used->Use (this, activator);
			}
			if (!GetInUse())
			{
				DebugPrintf("entity was removed while using targets\n");
				return;
			}
		}
	}
}