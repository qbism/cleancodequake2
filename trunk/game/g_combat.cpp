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
// g_combat.c

#include "g_local.h"

/*
============
CanDamage

Returns true if the inflictor can directly damage the target.  Used for
explosions and melee attacks.
============
*/
bool CanDamage (edict_t *targ, edict_t *inflictor)
{
	vec3_t	dest;
	CTrace	trace;

// bmodels need special checking because their origin is 0,0,0
	if (targ->movetype == MOVETYPE_PUSH)
	{
		Vec3Add (targ->absMin, targ->absMax, dest);
		Vec3Scale (dest, 0.5, dest);
		trace = CTrace (inflictor->state.origin, dest, inflictor, CONTENTS_MASK_SOLID);
		if (trace.fraction == 1.0)
			return true;
		if (trace.ent == targ)
			return true;
		return false;
	}
	
	trace = CTrace (inflictor->state.origin, targ->state.origin, inflictor, CONTENTS_MASK_SOLID);
	if (trace.fraction == 1.0)
		return true;

	Vec3Copy (targ->state.origin, dest);
	dest[0] += 15.0;
	dest[1] += 15.0;
	trace = CTrace (inflictor->state.origin, dest, inflictor, CONTENTS_MASK_SOLID);
	if (trace.fraction == 1.0)
		return true;

	Vec3Copy (targ->state.origin, dest);
	dest[0] += 15.0;
	dest[1] -= 15.0;
	trace = CTrace (inflictor->state.origin, dest, inflictor, CONTENTS_MASK_SOLID);
	if (trace.fraction == 1.0)
		return true;

	Vec3Copy (targ->state.origin, dest);
	dest[0] -= 15.0;
	dest[1] += 15.0;
	trace = CTrace (inflictor->state.origin, dest, inflictor, CONTENTS_MASK_SOLID);
	if (trace.fraction == 1.0)
		return true;

	Vec3Copy (targ->state.origin, dest);
	dest[0] -= 15.0;
	dest[1] -= 15.0;
	trace = CTrace (inflictor->state.origin, dest, inflictor, CONTENTS_MASK_SOLID);
	if (trace.fraction == 1.0)
		return true;


	return false;
}


/*
============
Killed
============
*/
void Killed (edict_t *targ, edict_t *inflictor, edict_t *attacker, int damage, vec3_t point)
{
	if (targ->health < -999)
		targ->health = -999;

	targ->enemy = attacker;

	if ((targ->svFlags & SVF_MONSTER) && (targ->deadflag != DEAD_DEAD) && targ->Monster)
	{
//		targ->svFlags |= SVF_DEADMONSTER;	// now treat as a different content type
		if (!(targ->Monster->AIFlags & AI_GOOD_GUY))
		{
			level.killed_monsters++;
			if (coop->Integer() && attacker->client)
				attacker->client->resp.score++;
			// medics won't heal monsters that they kill themselves
			if (strcmp(attacker->classname, "monster_medic") == 0)
				targ->owner = attacker;
		}
	}

	if (targ->movetype == MOVETYPE_PUSH || targ->movetype == MOVETYPE_STOP || targ->movetype == MOVETYPE_NONE)
	{	// doors, triggers, etc
		targ->die (targ, inflictor, attacker, damage, point);
		return;
	}

	if ((targ->svFlags & SVF_MONSTER) && (targ->deadflag != DEAD_DEAD))
	{
		targ->touch = NULL;
		if (targ->Monster)
			targ->Monster->MonsterDeathUse();
		//else
		//	monster_death_use (targ);
	}

	targ->die (targ, inflictor, attacker, damage, point);
}

/*
============
T_Damage

targ		entity that is being damaged
inflictor	entity that is causing the damage
attacker	entity that caused the inflictor to damage targ
	example: targ=monster, inflictor=rocket, attacker=player

dir			direction of the attack
point		point at which the damage is being inflicted
normal		normal vector from that point
damage		amount of damage being inflicted
knockback	force to be applied against targ as a result of the damage

dflags		these flags are used to control how T_Damage works
	DAMAGE_RADIUS			damage was indirect (from a nearby explosion)
	DAMAGE_NO_ARMOR			armor does not protect from this damage
	DAMAGE_ENERGY			damage is from an energy based weapon
	DAMAGE_NO_KNOCKBACK		do not affect velocity, just view angles
	DAMAGE_BULLET			damage is from a bullet (used for ricochets)
	DAMAGE_NO_PROTECTION	kills godmode, armor, everything
============
*/
int PowerArmorType (edict_t *ent);
static int CheckPowerArmor (edict_t *ent, vec3_t point, vec3_t normal, int damage, int dflags)
{
	gclient_t	*client;
	int			save;
	int			power_armor_type;
	int			index = FindItem("Cells")->GetIndex();
	int			damagePerCell;
	int			pa_te_type;
	int			power = 0;
	int			power_used;

	if (!damage)
		return 0;

	client = ent->client;

	if (dflags & DAMAGE_NO_ARMOR)
		return 0;

	if (client)
	{
		power_armor_type = PowerArmorType (ent);
		if (power_armor_type != POWER_ARMOR_NONE)
			power = client->pers.Inventory.Has(FindItem("Cells"));
	}
	else if ((ent->svFlags & SVF_MONSTER) && ent->Monster)
	{
		power_armor_type = ent->Monster->PowerArmorType;
		power = ent->Monster->PowerArmorPower;
	}
	else
		return 0;

	if (power_armor_type == POWER_ARMOR_NONE)
		return 0;
	if (!power)
		return 0;

	if (power_armor_type == POWER_ARMOR_SCREEN)
	{
		vec3_t		vec;
		float		dot;
		vec3_t		forward;

		// only works if damage point is in front
		Angles_Vectors (ent->state.angles, forward, NULL, NULL);
		Vec3Subtract (point, ent->state.origin, vec);
		VectorNormalizef (vec, vec);
		dot = DotProduct (vec, forward);
		if (dot <= 0.3)
			return 0;

		damagePerCell = 1;
		pa_te_type = TE_SCREEN_SPARKS;
		damage = damage / 3;
	}
	else
	{
		damagePerCell = 2;
		pa_te_type = TE_SHIELD_SPARKS;
		damage = (2 * damage) / 3;
	}

	save = power * damagePerCell;
	if (!save)
		return 0;
	if (save > damage)
		save = damage;

	CTempEnt_Splashes::ShieldSparks (point, normal, (pa_te_type == TE_SCREEN_SPARKS) ? true : false);
	ent->powerarmor_time = level.time + 0.2;

	power_used = save / damagePerCell;
	if (!power_used)
		power_used = 1;

	if (client)
		client->pers.Inventory.Remove(index, power_used);
	else if (ent->Monster)
		ent->Monster->PowerArmorPower -= power_used;
	return save;
}

int ArmorIndex (edict_t *ent);
static int CheckArmor (edict_t *ent, vec3_t point, vec3_t normal, int damage, int dflags)
{
	if (!damage)
		return 0;
	if (!ent->client)
		return 0;

	if (dflags & DAMAGE_NO_ARMOR)
		return 0;

	CArmor *armor = ent->client->pers.Armor;

	if (!armor)
		return 0;

	int save = ceil ( ((dflags & DAMAGE_ENERGY) ? armor->energyProtection : armor->normalProtection) * damage);
	if (save >= ent->client->pers.Inventory.Has(armor))
		save = ent->client->pers.Inventory.Has(armor);

	if (!save)
		return 0;

	ent->client->pers.Inventory.Remove(armor, save);
	CTempEnt_Splashes::Sparks (point, normal, (dflags & DAMAGE_BULLET) ? CTempEnt_Splashes::STBulletSparks : CTempEnt_Splashes::STSparks, CTempEnt_Splashes::SPTSparks);

	// Ran out of armor?
	if (!ent->client->pers.Inventory.Has(armor))
		ent->client->pers.Armor = NULL;

	return save;
}

bool CheckTeamDamage (edict_t *targ, edict_t *attacker)
{
		//FIXME make the next line real and uncomment this block
		// if ((ability to damage a teammate == OFF) && (targ's team == attacker's team))
	return false;
}

void T_Damage (edict_t *targ, edict_t *inflictor, edict_t *attacker, vec3_t dir, vec3_t point, vec3_t normal, int damage, int knockback, int dflags, int mod)
{
	gclient_t	*client;
	int			take;
	int			save;
	int			asave;
	int			psave;

	if (!targ->takedamage)
		return;

	// friendly fire avoidance
	// if enabled you can't hurt teammates (but you can hurt yourself)
	// knockback still occurs
	if ((targ != attacker) && (deathmatch->Integer() && (dmFlags.dfSkinTeams || dmFlags.dfModelTeams) || coop->Integer()))
	{
		if (OnSameTeam (targ, attacker))
		{
			if (dmFlags.dfNoFriendlyFire)
				damage = 0;
			else
				mod |= MOD_FRIENDLY_FIRE;
		}
	}
	meansOfDeath = mod;

	// easy mode takes half damage
	if (skill->Integer() == 0 && deathmatch->Integer() == 0 && targ->client)
	{
		damage *= 0.5;
		if (!damage)
			damage = 1;
	}

	client = targ->client;

	VectorNormalizef (dir, dir);

// bonus damage for suprising a monster
	if (!(dflags & DAMAGE_RADIUS) && (targ->svFlags & SVF_MONSTER) && (attacker->client) && (!targ->enemy) && (targ->health > 0))
		damage *= 2;

	if (targ->flags & FL_NO_KNOCKBACK)
		knockback = 0;

// figure momentum add
	if (!(dflags & DAMAGE_NO_KNOCKBACK))
	{
		if ((knockback) && (targ->movetype != MOVETYPE_NONE) && (targ->movetype != MOVETYPE_BOUNCE) && (targ->movetype != MOVETYPE_PUSH) && (targ->movetype != MOVETYPE_STOP))
		{
			vec3_t	kvel;
			float	mass;

			if (targ->mass < 50)
				mass = 50;
			else
				mass = targ->mass;

			if (targ->client  && attacker == targ)
				Vec3Scale (dir, 1600.0 * (float)knockback / mass, kvel);	// the rocket jump hack...
			else
				Vec3Scale (dir, 500.0 * (float)knockback / mass, kvel);

			Vec3Add (targ->velocity, kvel, targ->velocity);
		}
	}

	take = damage;
	save = 0;

	// check for godmode
	if ( (targ->flags & FL_GODMODE) && !(dflags & DAMAGE_NO_PROTECTION) )
	{
		take = 0;
		save = damage;
		CTempEnt_Splashes::Sparks (point, normal, (dflags & DAMAGE_BULLET) ? CTempEnt_Splashes::STBulletSparks : CTempEnt_Splashes::STSparks, CTempEnt_Splashes::SPTSparks);
	}

	// check for invincibility
	if ((client && client->invincible_framenum > level.framenum ) && !(dflags & DAMAGE_NO_PROTECTION))
	{
		if (targ->pain_debounce_time < level.time)
		{
			PlaySoundFrom (targ, CHAN_ITEM, SoundIndex("items/protect4.wav"), 1, ATTN_NORM);
			targ->pain_debounce_time = level.time + 2;
		}
		take = 0;
		save = damage;
	}

	psave = CheckPowerArmor (targ, point, normal, take, dflags);
	take -= psave;

	asave = CheckArmor (targ, point, normal, take, dflags);
	take -= asave;

	//treat cheat/powerup savings the same as armor
	asave += save;

	// team damage avoidance
	if (!(dflags & DAMAGE_NO_PROTECTION) && CheckTeamDamage (targ, attacker))
		return;

// do the damage
	if (take)
	{
		if ((targ->svFlags & SVF_MONSTER) || (client))
			CTempEnt_Splashes::Blood (point, normal);
		else
			CTempEnt_Splashes::Sparks (point, normal, (dflags & DAMAGE_BULLET) ? CTempEnt_Splashes::STBulletSparks : CTempEnt_Splashes::STSparks, CTempEnt_Splashes::SPTSparks);

		targ->health = targ->health - take;
			
		if (targ->health <= 0)
		{
			if ((targ->svFlags & SVF_MONSTER) || (client))
				targ->flags |= FL_NO_KNOCKBACK;
			Killed (targ, inflictor, attacker, take, point);
			return;
		}
	}

	if (targ->svFlags & SVF_MONSTER && !targ->Monster)
	{
		if (take)
			targ->pain (targ, attacker, knockback, take);
	}
	else if (targ->Monster)
	{
		targ->Monster->ReactToDamage (attacker);
		if (!(targ->Monster->AIFlags & AI_DUCKED) && take)
		{
			targ->Monster->Pain (attacker, knockback, take);
			if (skill->Integer() == 3)
				targ->pain_debounce_time = level.time + 5;
		}
	}
	else if (client)
	{
		if (!(targ->flags & FL_GODMODE) && (take))
			targ->pain (targ, attacker, knockback, take);
	}
	else if (take)
	{
		if (targ->pain)
			targ->pain (targ, attacker, knockback, take);
	}

	// add to the damage inflicted on a player this frame
	// the total will be turned into screen blends and view angle kicks
	// at the end of the frame
	if (client)
	{
		client->damage_parmor += psave;
		client->damage_armor += asave;
		client->damage_blood += take;
		client->damage_knockback += knockback;
		Vec3Copy (point, client->damage_from);
	}
}


/*
============
T_RadiusDamage
============
*/
void T_RadiusDamage (edict_t *inflictor, edict_t *attacker, float damage, edict_t *ignore, float radius, int mod)
{
	float	points;
	edict_t	*ent = NULL;
	vec3_t	v;
	vec3_t	dir;

	while ((ent = findradius(ent, inflictor->state.origin, radius)) != NULL)
	{
		if (ent == ignore)
			continue;
		if (!ent->takedamage)
			continue;

		Vec3Add (ent->mins, ent->maxs, v);
		Vec3MA (ent->state.origin, 0.5, v, v);
		Vec3Subtract (inflictor->state.origin, v, v);
		points = damage - 0.5 * Vec3Length (v);
		if (ent == attacker)
			points = points * 0.5;
		if (points > 0)
		{
			if (CanDamage (ent, inflictor))
			{
				Vec3Subtract (ent->state.origin, inflictor->state.origin, dir);
				T_Damage (ent, inflictor, attacker, dir, inflictor->state.origin, vec3Origin, (int)points, (int)points, DAMAGE_RADIUS, mod);
			}
		}
	}
}