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
// cc_weaponentities.cpp
// Entities related to weaponry
//

#include "cc_local.h"
#include "cc_weaponmain.h"
#include "cc_tent.h"

void CheckDodge (CBaseEntity *self, vec3f &start, vec3f &dir, int speed)
{
	vec3f	end;
	CTrace	tr;

	// easy mode only ducks one quarter the time
	if (skill->Integer() == 0)
	{
		if (frand() > 0.25)
			return;
	}

	end = start.MultiplyAngles(8192, dir);
	tr (start, end, self, CONTENTS_MASK_SHOT);

#ifdef MONSTER_USE_ROGUE_AI
	if ((tr.ent) && (tr.ent->Entity) && (tr.ent->Entity->EntityFlags & ENT_MONSTER) && (entity_cast<CHurtableEntity>(tr.ent->Entity)->Health > 0) && IsInFront(tr.Ent, self))
	{
		CMonsterEntity *Monster = (entity_cast<CMonsterEntity>(tr.ent->Entity));
		if (Monster->Enemy != self)
			Monster->Enemy = self;

		vec3f v = tr.EndPos - start;
		float eta = (v.LengthFast() - tr.ent->maxs[0]) / speed;
		Monster->Monster->Dodge (self, eta, &tr);
	}
#else
	if ((tr.ent) && (tr.ent->Entity) && (tr.ent->Entity->EntityFlags & ENT_MONSTER) && (entity_cast<CHurtableEntity>(tr.ent->Entity)->Health > 0) && IsInFront(tr.Ent, self))
	{
		CMonsterEntity *Monster = (entity_cast<CMonsterEntity>(tr.ent->Entity));
		if (Monster->Enemy != self)
			Monster->Enemy = self;

		vec3f v = tr.EndPos - start;
		float eta = (v.LengthFast() - tr.ent->maxs[0]) / speed;
		*Monster->Monster->Dodge (self, eta);
	}
#endif
}

/*
=================
CGrenade
=================
*/
enum // EGrenadeFlags
{
	GRENADE_HAND = BIT(0),
	GRENADE_HELD = BIT(1),
};
CGrenade::CGrenade () :
CBounceProjectile(),
CTouchableEntity(),
CThinkableEntity()
{
};

CGrenade::CGrenade (int Index) :
CBounceProjectile(Index),
CTouchableEntity(Index),
CThinkableEntity(Index)
{
};

void CGrenade::Explode ()
{
	EMeansOfDeath			mod;

	vec3f origin = State.GetOrigin ();
	if (gameEntity->owner && gameEntity->owner->client)
		entity_cast<CPlayerEntity>(gameEntity->owner->Entity)->PlayerNoiseAt (origin, PNOISE_IMPACT);

	//FIXME: if we are onground then raise our Z just a bit since we are a point?
	if (Enemy)
	{
		CHurtableEntity *HurtEnemy = entity_cast<CHurtableEntity>(Enemy);

		vec3f v = (HurtEnemy->GetMins() + HurtEnemy->GetMaxs());
		v = (origin - HurtEnemy->State.GetOrigin().MultiplyAngles (0.5f, v));

		float points = Damage - 0.5 * v.Length();
		vec3f dir = HurtEnemy->State.GetOrigin() - origin;

		HurtEnemy->TakeDamage	(this, GetOwner(), dir, origin, vec3fOrigin, (int)points, (int)points,
							DAMAGE_RADIUS, (SpawnFlags & GRENADE_HAND) ? MOD_HANDGRENADE : MOD_GRENADE);
	}

	if (SpawnFlags & GRENADE_HELD)
		mod = MOD_HELD_GRENADE;
	else if (SpawnFlags & GRENADE_HAND)
		mod = MOD_HG_SPLASH;
	else
		mod = MOD_G_SPLASH;
	T_RadiusDamage(this, GetOwner(), Damage, (Enemy) ? Enemy : NULL, RadiusDamage, mod);

	origin = origin.MultiplyAngles (-0.02f, Velocity);
	if (GroundEntity)
		CTempEnt_Explosions::GrenadeExplosion(origin, this, !!WaterInfo.Level);
	else
		CTempEnt_Explosions::RocketExplosion(origin, this, !!WaterInfo.Level);

	Free (); // "delete" the entity
}

void CGrenade::Touch (CBaseEntity *other, plane_t *plane, cmBspSurface_t *surf)
{
	if (other->gameEntity == gameEntity->owner)
		return;

	if (surf && (surf->flags & SURF_TEXINFO_SKY))
	{
		Free (); // "delete" the entity
		return;
	}

	if (!((other->EntityFlags & ENT_HURTABLE) && entity_cast<CHurtableEntity>(other)->CanTakeDamage))
	{
		if (SpawnFlags & GRENADE_HAND)
		{
			if (frand() > 0.5)
				PlaySound (CHAN_VOICE, SoundIndex ("weapons/hgrenb1a.wav"));
			else
				PlaySound (CHAN_VOICE, SoundIndex ("weapons/hgrenb2a.wav"));
		}
		else
			PlaySound (CHAN_VOICE, SoundIndex ("weapons/grenlb1b.wav"));
		return;
	}

	Enemy = other;
	Explode ();
}

void CGrenade::Think ()
{
	Explode();
}

void CGrenade::Spawn (CBaseEntity *Spawner, vec3f start, vec3f aimdir, int damage, int speed, float timer, float damage_radius, bool handNade, bool held)
{
	CGrenade	*Grenade = QNew (com_levelPool, 0) CGrenade();
	vec3f		forward, right, up;

	vec3f dir = aimdir.ToAngles();
	VecToAngles (aimdir, dir);

	dir.ToVectors (&forward, &right, &up);

	Grenade->State.GetOrigin() = start;
	aimdir *= speed;

	Grenade->Velocity = aimdir;
	Grenade->Velocity = Grenade->Velocity.MultiplyAngles (200 + crand() * 10.0f, up);
	Grenade->Velocity = Grenade->Velocity.MultiplyAngles (crand() * 10.0, right);

	Grenade->AngularVelocity.Set (300);
	Grenade->State.GetEffects() = EF_GRENADE;
	Grenade->State.GetModelIndex() = (!handNade) ? ModelIndex ("models/objects/grenade/tris.md2") : ModelIndex ("models/objects/grenade2/tris.md2");
	Grenade->SetOwner(Spawner);
	Grenade->NextThink = level.Frame + (timer * 10);
	Grenade->Damage = damage;
	Grenade->RadiusDamage = damage_radius;
	Grenade->gameEntity->classname = (!handNade) ? "grenade" : "hgrenade";
	if (handNade)
	{
		Grenade->SpawnFlags = (held) ? (GRENADE_HAND|GRENADE_HELD) : GRENADE_HAND;
		Grenade->State.GetSound() = SoundIndex("weapons/hgrenc1b.wav");
	}

	if (timer <= 0.0)
		Grenade->Explode ();
	else
	{
		if (handNade)
			Spawner->PlaySound (CHAN_WEAPON, SoundIndex ("weapons/hgrent1a.wav"));
		Grenade->Link();
	}
}

bool CGrenade::Run ()
{
	return CBounceProjectile::Run();
}

/*
================
CBlasterProjectile
================
*/

CBlasterProjectile::CBlasterProjectile () :
CFlyMissileProjectile(),
CTouchableEntity(),
CThinkableEntity()
{
};

CBlasterProjectile::CBlasterProjectile (int Index) :
CFlyMissileProjectile(Index),
CTouchableEntity(Index),
CThinkableEntity(Index)
{
};

void CBlasterProjectile::Think ()
{
	Free();
}

void CBlasterProjectile::Touch (CBaseEntity *other, plane_t *plane, cmBspSurface_t *surf)
{
	if (other->gameEntity == gameEntity->owner)
		return;

	if (surf && (surf->flags & SURF_TEXINFO_SKY))
	{
		Free (); // "delete" the entity
		return;
	}

	vec3f origin = State.GetOrigin ();
	if (gameEntity->owner->client)
		entity_cast<CPlayerEntity>(gameEntity->owner->Entity)->PlayerNoiseAt (origin, PNOISE_IMPACT);

	if ((other->EntityFlags & ENT_HURTABLE) && entity_cast<CHurtableEntity>(other)->CanTakeDamage)
		entity_cast<CHurtableEntity>(other)->TakeDamage (this, GetOwner(), Velocity, origin, plane ? plane->normal : vec3fOrigin, Damage, 1, DAMAGE_ENERGY, (SpawnFlags & 1) ? MOD_HYPERBLASTER : MOD_BLASTER);
	else
		CTempEnt_Splashes::Blaster(origin, plane ? plane->normal : vec3fOrigin);

	Free (); // "delete" the entity
}

void CBlasterProjectile::Spawn (CBaseEntity *Spawner, vec3f start, vec3f dir,
						int damage, int speed, int effect, bool isHyper)
{
	CBlasterProjectile		*Bolt = QNew (com_levelPool, 0) CBlasterProjectile;

	if (Spawner && Spawner->EntityFlags & ENT_PLAYER)
		CheckDodge (Spawner, start, dir, speed);

	dir.NormalizeFast();

	Bolt->GetSvFlags() |= (SVF_DEADMONSTER | SVF_PROJECTILE);
	Bolt->State.GetOrigin() = start;
	Bolt->State.GetOldOrigin() = start;
	Bolt->State.GetAngles() = dir.ToAngles();
	Bolt->Velocity = dir * speed;

	Bolt->State.GetEffects() = effect;
	Bolt->State.GetModelIndex() = ModelIndex ("models/objects/laser/tris.md2");

	Bolt->State.GetSound() = SoundIndex ("misc/lasfly.wav");
	Bolt->SetOwner (Spawner);
	Bolt->NextThink = level.Frame + 20;
	Bolt->Damage = damage;
	Bolt->gameEntity->classname = "bolt";
	if (isHyper)
		Bolt->SpawnFlags = 1;
	Bolt->Link ();

	CTrace tr ((Spawner) ? Spawner->State.GetOrigin() : start, start, Bolt, CONTENTS_MASK_SHOT);
	if (tr.fraction < 1.0)
	{
		start = start.MultiplyAngles (-10, dir);
		Bolt->State.GetOrigin() = start;
		Bolt->State.GetOldOrigin() = start;

		if (tr.ent->Entity)
			Bolt->Touch (tr.ent->Entity, &tr.plane, tr.surface);
	}
}

bool CBlasterProjectile::Run ()
{
	return CFlyMissileProjectile::Run();
}

CRocket::CRocket () :
CFlyMissileProjectile(),
CTouchableEntity(),
CThinkableEntity()
{
};

CRocket::CRocket (int Index) :
CFlyMissileProjectile(Index),
CTouchableEntity(Index),
CThinkableEntity(Index)
{
};

void CRocket::Think ()
{
	Free (); // "delete" the entity
}

void CRocket::Touch (CBaseEntity *other, plane_t *plane, cmBspSurface_t *surf)
{
	if (other->gameEntity == gameEntity->owner)
		return;

	if (surf && (surf->flags & SURF_TEXINFO_SKY))
	{
		Free ();
		return;
	}

	vec3f origin = State.GetOrigin ();
	if (gameEntity->owner->client)
		entity_cast<CPlayerEntity>(gameEntity->owner->Entity)->PlayerNoiseAt (origin, PNOISE_IMPACT);

	if ((other->EntityFlags & ENT_HURTABLE) && entity_cast<CHurtableEntity>(other)->CanTakeDamage)
		entity_cast<CHurtableEntity>(other)->TakeDamage (this, GetOwner(), Velocity, origin, (plane) ? plane->normal : vec3fOrigin, Damage, 0, 0, MOD_ROCKET);
/*	else
	{
		// don't throw any debris in net games
		if (game.mode == GAME_SINGLEPLAYER)
		{
			if ((surf) && !(surf->flags & (SURF_TEXINFO_WARP|SURF_TEXINFO_TRANS33|SURF_TEXINFO_TRANS66|SURF_TEXINFO_FLOWING)))
			{
				for (int n = 0; n < randomMT()%5; n++)
					ThrowDebris (gameEntity, "models/objects/debris2/tris.md2", 2, origin);
			}
		}
	}*/

	// calculate position for the explosion entity
	origin = origin.MultiplyAngles (-0.02f, Velocity);
	T_RadiusDamage(this, GetOwner(), RadiusDamage, other, DamageRadius, MOD_R_SPLASH);
	CTempEnt_Explosions::RocketExplosion(origin, this, !!WaterInfo.Level);

	Free ();
}

void CRocket::Spawn	(CBaseEntity *Spawner, vec3f start, vec3f dir,
						int damage, int speed, float damage_radius, int radius_damage)
{
	CRocket	*Rocket = QNew (com_levelPool, 0) CRocket;

	Rocket->State.GetOrigin() = start;
	Rocket->State.GetAngles() = dir.ToAngles();
	Rocket->Velocity = dir * speed;
	Rocket->State.GetEffects() = EF_ROCKET;
	Rocket->State.GetModelIndex() = ModelIndex ("models/objects/rocket/tris.md2");
	Rocket->SetOwner (Spawner);
	Rocket->NextThink = level.Frame + 80000/speed;
	Rocket->Damage = damage;
	Rocket->RadiusDamage = radius_damage;
	Rocket->DamageRadius = damage_radius;
	Rocket->State.GetSound() = SoundIndex ("weapons/rockfly.wav");
	Rocket->gameEntity->classname = "rocket";

	if (Spawner->EntityFlags & ENT_PLAYER)
		CheckDodge (Spawner, start, dir, speed);

	Rocket->Link ();
}

bool CRocket::Run ()
{
	return CFlyMissileProjectile::Run();
}

CBFGBolt::CBFGBolt () :
CFlyMissileProjectile(),
CTouchableEntity(),
CThinkableEntity()
{
	Exploded = false;
};

CBFGBolt::CBFGBolt (int Index) :
CFlyMissileProjectile(Index),
CTouchableEntity(Index),
CThinkableEntity(Index)
{
	Exploded = false;
};

void CBFGBolt::Think ()
{
	// bfg_explode
	if (Exploded)
	{
		if (State.GetFrame() == 0)
		{
			vec3f origin = State.GetOrigin ();
			// the BFG effect
			CHurtableEntity *ent = NULL;
			while ((ent = FindRadius<CHurtableEntity, ENT_HURTABLE> (ent, origin, DamageRadius)) != NULL)
			{
				if (!ent->CanTakeDamage)
					continue;
				if (ent == GetOwner())
					continue;
				if (!ent->CanDamage (this) || !ent->CanDamage (GetOwner()))
					continue;

				vec3f v = ent->GetMins() + ent->GetMaxs();
				v = ent->State.GetOrigin().MultiplyAngles (0.5f, v);
				v = origin - v;
				float dist = v.Length();
				float points = Damage * (1.0f - sqrtf(dist/DamageRadius));
				if (ent == GetOwner())
					points = points * 0.5;

				CTempEnt_Explosions::BFGExplosion (ent->State.GetOrigin());
				ent->TakeDamage (this, GetOwner(), Velocity, ent->State.GetOrigin(), vec3fOrigin, (int)points, 0, DAMAGE_ENERGY, MOD_BFG_EFFECT);
			}
		}

		NextThink = level.Frame + FRAMETIME;
		State.GetFrame()++;
		if (State.GetFrame() == 5)
			Free ();

		return;
	}
	else // bfg_think
	{
		if (FreeTime < level.Frame)
		{
			Free();
			return;
		}

		CHurtableEntity	*ent = NULL;

		vec3f origin = State.GetOrigin ();
		const int dmg = (game.mode & GAME_DEATHMATCH) ? 5 : 10;

		while ((ent = FindRadius<CHurtableEntity, ENT_HURTABLE> (ent, origin, 256)) != NULL)
		{
			if (ent == gameEntity->Entity) // Stupid...
				continue;

			if (ent == GetOwner())
				continue;

			if (!ent->CanTakeDamage)
				continue;

			//if (!(ent->svFlags & SVF_MONSTER) && (!ent->client))
			//	continue;

	#ifdef CLEANCTF_ENABLED
	//ZOID
			//don't target players in CTF
			if ((game.mode & GAME_CTF) && (ent->EntityFlags & ENT_PLAYER) &&
				(GetOwner() && (GetOwner()->EntityFlags & ENT_PLAYER)))
			{
				if ((entity_cast<CPlayerEntity>(ent))->Client.Respawn.ctf_team == (entity_cast<CPlayerEntity>(GetOwner()))->Client.Respawn.ctf_team)
					continue;
			}
	//ZOID
	#endif

			vec3f point = ent->GetAbsMin().MultiplyAngles (0.5f, ent->GetSize());

			vec3f dir = point - origin;
			dir.Normalize ();

			CBaseEntity *ignore = this;
			vec3f start = origin;
			vec3f end = start.MultiplyAngles (2048, dir);
			CTrace tr;
			while(1)
			{
				tr (start, end, ignore, CONTENTS_SOLID|CONTENTS_MONSTER|CONTENTS_DEADMONSTER);

				if (!tr.ent || !tr.Ent)
					break;

				// hurt it if we can
				//if ((tr.ent->takedamage) && !(tr.ent->flags & FL_IMMUNE_LASER) && (tr.ent != gameEntity->owner))
				if (((tr.Ent->EntityFlags & ENT_HURTABLE) && entity_cast<CHurtableEntity>(tr.Ent)->CanTakeDamage) && !(tr.Ent->Flags & FL_IMMUNE_LASER) && (tr.Ent != GetOwner()))
					entity_cast<CHurtableEntity>(tr.Ent)->TakeDamage (this, GetOwner(), dir, tr.EndPos, vec3fOrigin, dmg, 1, DAMAGE_ENERGY, MOD_BFG_LASER);

				// if we hit something that's not a monster or player we're done
				//if (!(tr.ent->svFlags & SVF_MONSTER) && (!tr.ent->client))
				if (!(tr.Ent->EntityFlags & ENT_MONSTER) && !(tr.Ent->EntityFlags & ENT_PLAYER))
				{
					CTempEnt_Splashes::Sparks (tr.EndPos, tr.plane.normal, CTempEnt_Splashes::ST_LASER_SPARKS, State.GetSkinNum(), 4);
					break;
				}

				ignore = tr.Ent;
				start = tr.EndPos;
			}

			CTempEnt_Trails::BFGLaser(origin, tr.EndPos);
		}

		NextThink = level.Frame + FRAMETIME;
	}
}

void CBFGBolt::Touch (CBaseEntity *other, plane_t *plane, cmBspSurface_t *surf)
{
	if (Exploded)
		return;

	if (other->gameEntity == gameEntity->owner)
		return;

	if (surf && (surf->flags & SURF_TEXINFO_SKY))
	{
		Free();
		return;
	}

	vec3f boltOrigin = State.GetOrigin();
	if (gameEntity->owner->client)
		entity_cast<CPlayerEntity>(gameEntity->owner->Entity)->PlayerNoiseAt (boltOrigin, PNOISE_IMPACT);

	// core explosion - prevents firing it into the wall/floor
	if ((other->EntityFlags & ENT_HURTABLE) && entity_cast<CHurtableEntity>(other)->CanTakeDamage)
		entity_cast<CHurtableEntity>(other)->TakeDamage (this, GetOwner(), Velocity, boltOrigin, (plane) ? plane->normal : vec3fOrigin, 200, 0, 0, MOD_BFG_BLAST);
	T_RadiusDamage(this, GetOwner(), 200, other, 100, MOD_BFG_BLAST);

	PlaySound (CHAN_VOICE, SoundIndex ("weapons/bfg__x1b.wav"));
	GetSolid() = SOLID_NOT;

	Exploded = true;
	State.GetOrigin() = boltOrigin.MultiplyAngles (-0.1f, Velocity);
	Velocity.Clear ();
	State.GetModelIndex() = ModelIndex ("sprites/s_bfg3.sp2");
	State.GetFrame() = 0;
	State.GetSound() = 0;
	State.GetEffects() = EF_BFG;
	NextThink = level.Frame + FRAMETIME;
	Enemy = other;

	CTempEnt_Explosions::BFGExplosion (boltOrigin, true);
}

void CBFGBolt::Spawn	(CBaseEntity *Spawner, vec3f start, vec3f dir,
						int damage, int speed, float damage_radius)
{
	CBFGBolt	*BFG = QNew (com_levelPool, 0) CBFGBolt;

	BFG->State.GetOrigin() = start;
	BFG->State.GetAngles() = dir.ToAngles();
	BFG->Velocity = (dir.GetNormalizedFast()) * speed;
	BFG->State.GetEffects() = EF_BFG | EF_ANIM_ALLFAST;
	BFG->State.GetModelIndex() = ModelIndex ("sprites/s_bfg1.sp2");
	BFG->SetOwner (Spawner);
	BFG->NextThink = level.Frame + FRAMETIME;
	BFG->Damage = damage;
	BFG->DamageRadius = damage_radius;
	BFG->State.GetSound() = SoundIndex ("weapons/bfg__l1a.wav");
	BFG->gameEntity->classname = "bfg blast";
	BFG->FreeTime = level.Frame + 80000/speed;

	BFG->Link ();
}

bool CBFGBolt::Run ()
{
	return CFlyMissileProjectile::Run();
}

CTrace CHitScan::DoTrace(vec3f &start, vec3f &end, CBaseEntity *ignore, int mask)
{
	return CTrace (start, end, (ignore) ? ignore : NULL, mask);
}

bool CHitScan::DoDamage (CBaseEntity *Attacker, CHurtableEntity *Target, vec3f &dir, vec3f &point, vec3f &normal)
{
	return true;
}

void CHitScan::DoEffect	(vec3f &start, vec3f &end, bool water)
{
};

void CHitScan::DoSolidHit	(CTrace *Trace)
{
};

bool CHitScan::ModifyEnd(vec3f &aimDir, vec3f &start, vec3f &end)
{
	return false;
};

void CHitScan::DoWaterHit (CTrace *Trace)
{
}

#define HITSCANSTEP 100
void CHitScan::DoFire(CBaseEntity *Entity, vec3f start, vec3f aimdir)
{
	vec3f end, from;
	vec3f lastWaterStart, lastWaterEnd;
	
	vec3f lastDrawFrom;
	bool DrawIsWater = false;

	// Calculate end
	if (!ModifyEnd(aimdir, start, end))
		end = start.MultiplyAngles (8192, aimdir);

	from = start;
	lastDrawFrom = from;

	int Mask = CONTENTS_MASK_SHOT|CONTENTS_MASK_WATER;
	bool Water = false;
	CBaseEntity *Ignore = Entity;

	lastWaterStart = start;

	bool hitOutOfWater = false;
	if (PointContents(start) & CONTENTS_MASK_WATER)
	{
		// Copy up our point for the effect
		lastWaterEnd = start;
	
		// Special case if we started in water
		Water = true;
		DrawIsWater = true;
		Mask = CONTENTS_MASK_SHOT;

		// Find the exit point
		int tries = 20; // Cover about 2000 units
		vec3f	stWater;

		stWater = from;
		lastWaterStart.Clear();
		
		while (tries > 0)
		{
			stWater = stWater.MultiplyAngles (HITSCANSTEP, aimdir);

			EBrushContents contents = PointContents(stWater);
			if (contents == 0) // "Clear" or solid
				break; // Got it
			else if (contents & CONTENTS_MASK_SOLID)
			{
				// This is a special case in case we run into a solid.
				// This basically means that the trace is done, so we can skip ahead right to the solid (act like
				// we're not in water)
				tries = 0;
				break;
			}
			tries --;
		}

		if (tries != 0)
		{
			// We reached air
			// Trace backwards and grab the water brush
			vec3f tempOrigin;
			tempOrigin = stWater.MultiplyAngles (-(HITSCANSTEP + 5), aimdir);
			CTrace tempTrace = DoTrace (stWater, tempOrigin, NULL, CONTENTS_MASK_WATER);

			if (tempTrace.contents & CONTENTS_MASK_WATER)// All is good
			{
				// This is our end
				lastWaterStart = tempTrace.EndPos;
				hitOutOfWater = true;
			}
		}
		// We didn't reach air if we got here.
		// Let water handle it, it will act as solid.
	}

	// Main loop
	while (1)
	{
		// Trace from start to our end
		CTrace Trace = DoTrace (from, end, Ignore, Mask);

		// Did we hit an entity?
		if (Trace.ent && Trace.Ent && ((Trace.Ent->EntityFlags & ENT_HURTABLE) && entity_cast<CHurtableEntity>(Trace.Ent)->CanTakeDamage))
		{
			// Convert to base entity
			CHurtableEntity *Target = entity_cast<CHurtableEntity>(Trace.Ent);

			// Hurt it
			// Revision
			// Startsolid mistake..
			if (Trace.startSolid)
			{
				vec3f origin = Target->State.GetOrigin();
				if (!DoDamage (Entity, Target, aimdir, origin, Trace.plane.normal))
				{
					DoEffect (origin, lastDrawFrom, DrawIsWater);
					break; // We wanted to stop
				}

				// Set up the start from where we are now
				vec3f oldFrom = from;
				from = origin;

				// Revision: Stop on solids
				if (Target->GetSolid() == SOLID_BSP)
				{
					// Draw the effect
					//DoEffect (from, oldFrom, false);

					DoSolidHit (&Trace);
					break;
				}

				//DoEffect (from, oldFrom, Water);

				// and ignore the bastard
				Ignore = Target;

				// Continue our loop
				continue;
			}

			if (!DoDamage (Entity, Target, aimdir, Trace.EndPos, Trace.plane.normal))
				break; // We wanted to stop

			// Set up the start from where we are now
			vec3f oldFrom = from;
			from = Trace.EndPos;

			// Revision: Stop on solids
			if (Target->GetSolid() == SOLID_BSP)
			{
				// Draw the effect
				//DoEffect (from, oldFrom, false);

				DoEffect (from, oldFrom, DrawIsWater);
				DoSolidHit (&Trace);
				break;
			}

			//DoEffect (from, oldFrom, Water);

			// and ignore the bastard
			Ignore = Target;

			// Continue our loop
			continue;
		}
		// If we hit something in water...
		else if (Water)
		{
			Water = false;
			// Assume solid
			//if (Trace.contents & CONTENTS_MASK_SOLID)
			{
				// If we didn't grab water last time...
				if (lastWaterStart == vec3fOrigin)
				{
					// We hit the ground!
					// Swap start and end points
					lastWaterStart = lastWaterEnd;
					lastWaterStart = lastWaterStart.MultiplyAngles (5, aimdir);

					// Set end point
					lastWaterEnd = Trace.EndPos;

					// Draw the effect
					DoEffect (lastWaterStart, lastWaterEnd, true);

					DoSolidHit (&Trace);

					break; // We're done
				}
				// Otherwise we had found an exit point
				else
				{
					// Draw from water surface to water end
					DoEffect (lastWaterEnd, lastWaterStart, true);
	
					// We hit the ground!
					// Swap start and end points
					if (!hitOutOfWater)
						lastWaterStart = lastWaterEnd;
					else
						hitOutOfWater = false;

					// Set end point
					lastWaterEnd = Trace.EndPos;

					// Draw the effect
					DoEffect (lastWaterStart, lastWaterEnd, false);

					DoSolidHit (&Trace);

					break; // We're done
				}
			}
			continue;
		}
		// If we hit non-transparent water
		else if ((Trace.contents & CONTENTS_MASK_WATER) &&
			(Trace.surface && !(Trace.surface->flags & (SURF_TEXINFO_TRANS33|SURF_TEXINFO_TRANS66))))
		{
			// Copy up our point for the effect
			lastWaterEnd = Trace.EndPos;

			// Tell the system we're in water
			Water = true;

			// Draw the effect
			DoEffect (lastWaterStart, lastWaterEnd, false);

			DoWaterHit (&Trace);

			// Set up the start from where we are now
			from = lastDrawFrom = Trace.EndPos;
			Mask = CONTENTS_MASK_SHOT;

			// Find the exit point
			int tries = 20; // Cover about 2000 units
			vec3f	stWater = from;
			lastWaterStart.Clear();
			
			while (tries > 0)
			{
				stWater = stWater.MultiplyAngles (HITSCANSTEP, aimdir);

				EBrushContents contents = PointContents(stWater);
				if (contents == 0) // "Clear" or solid
					break; // Got it
				else if (contents & CONTENTS_MASK_SOLID)
				{
					// This is a special case in case we run into a solid.
					// This basically means that the trace is done, so we can skip ahead right to the solid (act like
					// we're not in water)
					tries = 0;
					break;
				}
				tries --;
			}

			if (tries != 0)
			{
				// We reached air
				// Trace backwards and grab the water brush
				vec3f tempOrigin = stWater.MultiplyAngles (-(HITSCANSTEP + 5), aimdir);
				CTrace tempTrace = DoTrace (stWater, tempOrigin, NULL, CONTENTS_MASK_WATER);

				if (tempTrace.contents & CONTENTS_MASK_WATER) // All is good
				{
					// This is our end
					lastWaterStart = tempTrace.EndPos;
					continue; // Head to the next area
				}
			}
			// We didn't reach air if we got here.
			// Let water handle it, it will act as solid.

			// Continue the loop
			continue;
		}
		// Transparent water
		else if ((Trace.contents & CONTENTS_MASK_WATER) &&
			(Trace.surface && (Trace.surface->flags & (SURF_TEXINFO_TRANS33|SURF_TEXINFO_TRANS66))))
		{
			// This won't count as "water" since we can see through it.
			// It has the same PVS, meaning we don't need to
			// do complex tracing.

			// Keep going
			from = Trace.EndPos.MultiplyAngles (0.1f, aimdir);

			// Water hit effect
			DoWaterHit (&Trace);
			continue;
		}
		// Assume solid
		else
		{
			// Draw the effect
			//DoEffect (from, Trace.EndPos, false);
			DoEffect (lastDrawFrom, Trace.EndPos, DrawIsWater);

			DoSolidHit (&Trace);
			break; // We're done
		}
	}
}

bool CRailGunShot::DoDamage (CBaseEntity *Attacker, CHurtableEntity *Target, vec3f &dir, vec3f &point, vec3f &normal)
{
	if (Attacker != Target) // Hurt self protection
		Target->TakeDamage (Attacker, Attacker, dir, point, normal, Damage, Kick, 0, MOD_RAILGUN);
	return ThroughAndThrough;
}

void CRailGunShot::DoEffect	(vec3f &start, vec3f &end, bool water)
{
	CTempEnt_Trails::RailTrail (start, end);
}

void CRailGunShot::Fire(CBaseEntity *Entity, vec3f start, vec3f aimdir, int damage, int kick)
{
	CRailGunShot(damage, kick).DoFire (Entity, start, aimdir);
}

bool CBullet::DoDamage (CBaseEntity *Attacker, CHurtableEntity *Target, vec3f &dir, vec3f &point, vec3f &normal)
{
	Target->TakeDamage (Attacker, Attacker, dir, point, normal, Damage, Kick, DAMAGE_BULLET, MeansOfDeath);
	return ThroughAndThrough;
}

void CBullet::DoSolidHit	(CTrace *Trace)
{
	if (!(Trace->surface->flags & SURF_TEXINFO_SKY))
		CTempEnt_Splashes::Gunshot (Trace->EndPos, Trace->plane.normal);
}

bool CBullet::ModifyEnd (vec3f &aimDir, vec3f &start, vec3f &end)
{
	vec3f dir = aimDir.ToAngles();
	vec3f forward, right, up;
	dir.ToVectors (&forward, &right, &up);

	float r = crand()*hSpread;
	float u = crand()*vSpread;
	end = start.MultiplyAngles (8192, forward);
	end = end.MultiplyAngles (r, right);
	end = end.MultiplyAngles (u, up);
	return true;
}

void CBullet::DoEffect	(vec3f &start, vec3f &end, bool water)
{
	if (water)
		CTempEnt_Trails::BubbleTrail(start, end);
}

void CBullet::DoWaterHit	(CTrace *Trace)
{
	CTempEnt_Splashes::ESplashType color;
	if (Trace->contents & CONTENTS_WATER)
	{
		if (strcmp(Trace->surface->name, "*brwater") == 0)
			color = CTempEnt_Splashes::SPT_MUD;
		else
			color = CTempEnt_Splashes::SPT_WATER;
	}
	else if (Trace->contents & CONTENTS_SLIME)
		color = CTempEnt_Splashes::SPT_SLIME;
	else if (Trace->contents & CONTENTS_LAVA)
		color = CTempEnt_Splashes::SPT_LAVA;
	else
		return;

	CTempEnt_Splashes::Splash (Trace->EndPos, Trace->plane.normal, color);
}

void CBullet::Fire(CBaseEntity *Entity, vec3f start, vec3f aimdir, int damage, int kick, int hSpread, int vSpread, int mod)
{
	CBullet(damage, kick, hSpread, vSpread, mod).DoFire (Entity, start, aimdir);
}

// An overload to handle transparent water
void CBullet::DoFire(CBaseEntity *Entity, vec3f start, vec3f aimdir)
{
	vec3f end, from;
	vec3f lastWaterStart, lastWaterEnd;

	// Calculate end
	if (!ModifyEnd(aimdir, start, end))
		end = start.MultiplyAngles (8192, aimdir);

	from = start;

	int Mask = CONTENTS_MASK_SHOT|CONTENTS_MASK_WATER;
	bool Water = false;
	CBaseEntity *Ignore = Entity;

	lastWaterStart = start;

	bool hitOutOfWater = false;
	if (PointContents(start) & CONTENTS_MASK_WATER)
	{
		// Copy up our point for the effect
		lastWaterEnd = start;
	
		// Special case if we started in water
		Water = true;

		Mask = CONTENTS_MASK_SHOT;

		// Find the exit point
		int tries = 20; // Cover about 2000 units
		vec3f	stWater = from;
		lastWaterStart.Clear ();
		
		while (tries > 0)
		{
			stWater = stWater.MultiplyAngles (HITSCANSTEP, aimdir);

			EBrushContents contents = PointContents(stWater);
			if (contents == 0) // "Clear" or solid
				break; // Got it
			else if (contents & CONTENTS_MASK_SOLID)
			{
				// This is a special case in case we run into a solid.
				// This basically means that the trace is done, so we can skip ahead right to the solid (act like
				// we're not in water)
				tries = 0;
				break;
			}
			tries --;
		}

		if (tries != 0)
		{
			// We reached air
			// Trace backwards and grab the water brush
			vec3f tempOrigin = stWater.MultiplyAngles (-(HITSCANSTEP + 5), aimdir);
			CTrace tempTrace = DoTrace (stWater, tempOrigin, NULL, CONTENTS_MASK_WATER);

			if (tempTrace.contents & CONTENTS_MASK_WATER)// All is good
			{
				// This is our end
				lastWaterStart = tempTrace.EndPos;
				hitOutOfWater = true;
			}
		}
		// We didn't reach air if we got here.
		// Let water handle it, it will act as solid.
	}

	// Main loop
	while (1)
	{
		// Trace from start to our end
		CTrace Trace = DoTrace (from, end, Ignore, Mask);

		// Did we hit an entity?
		if (Trace.ent && Trace.Ent && ((Trace.Ent->EntityFlags & ENT_HURTABLE) && entity_cast<CHurtableEntity>(Trace.Ent)->CanTakeDamage))
		{
			// Convert to base entity
			CHurtableEntity *Target = entity_cast<CHurtableEntity>(Trace.Ent);

			// Hurt it
			// Revision
			// Startsolid mistake..
			if (Trace.startSolid)
			{
				vec3f origin = Target->State.GetOrigin();
				if (!DoDamage (Entity, Target, aimdir, origin, Trace.plane.normal))
					break; // We wanted to stop

				// Set up the start from where we are now
				vec3f oldFrom = from;
				from = origin;
				DoEffect (from, oldFrom, Water);

				// and ignore the bastard
				Ignore = Target;

				// Continue our loop
				continue;
			}
			if (!DoDamage (Entity, Target, aimdir, Trace.EndPos, Trace.plane.normal))
				break; // We wanted to stop

			// Set up the start from where we are now
			vec3f oldFrom = from;
			from = Trace.EndPos;
			DoEffect (from, oldFrom, Water);

			// and ignore the bastard
			Ignore = Target;

			// Continue our loop
			continue;
		}
		// If we hit something in water...
		else if (Water)
		{
			Water = false;
			// Assume solid
			//if (Trace.contents & CONTENTS_MASK_SOLID)
			{
				// If we didn't grab water last time...
				if (lastWaterStart == vec3fOrigin)
				{
					// We hit the ground!
					// Swap start and end points
					lastWaterStart = lastWaterEnd.MultiplyAngles (5, aimdir);

					// Set end point
					lastWaterEnd = Trace.EndPos;

					// Draw the effect
					DoEffect (lastWaterStart, lastWaterEnd, true);
					DoSolidHit (&Trace);

					break; // We're done
				}
				// Otherwise we had found an exit point
				else
				{
					// Draw from water surface to water end
					DoEffect (lastWaterEnd, lastWaterStart, true);
	
					// We hit the ground!
					// Swap start and end points
					if (!hitOutOfWater)
						lastWaterStart = lastWaterEnd;
					else
						hitOutOfWater = false;

					// Set end point
					lastWaterEnd = Trace.EndPos;

					// Draw the effect
					DoEffect (lastWaterStart, lastWaterEnd, false);
					DoSolidHit (&Trace);

					break; // We're done
				}
			}
			continue;
		}
		// If we hit non-transparent water
		else if ((Trace.contents & CONTENTS_MASK_WATER) &&
			(Trace.surface && !(Trace.surface->flags & (SURF_TEXINFO_TRANS33|SURF_TEXINFO_TRANS66))))
		{
			// Copy up our point for the effect
			lastWaterEnd = Trace.EndPos;

			// Tell the system we're in water
			Water = true;

			// Draw the effect
			DoEffect (lastWaterStart, lastWaterEnd, false);
			DoWaterHit (&Trace);

			// Set up the start from where we are now
			from = Trace.EndPos;
			Mask = CONTENTS_MASK_SHOT;

			// Find the exit point
			int tries = 20; // Cover about 2000 units
			vec3f	stWater = from;
			lastWaterStart.Clear();
			
			while (tries > 0)
			{
				stWater = stWater.MultiplyAngles(HITSCANSTEP, aimdir);

				EBrushContents contents = PointContents(stWater);
				if (contents == 0) // "Clear" or solid
					break; // Got it
				else if (contents & CONTENTS_MASK_SOLID)
				{
					// This is a special case in case we run into a solid.
					// This basically means that the trace is done, so we can skip ahead right to the solid (act like
					// we're not in water)
					tries = 0;
					break;
				}
				tries --;
			}

			if (tries != 0)
			{
				// We reached air
				// Trace backwards and grab the water brush
				vec3f tempOrigin = stWater.MultiplyAngles (-(HITSCANSTEP + 5), aimdir);
				CTrace tempTrace = DoTrace (stWater, tempOrigin, NULL, CONTENTS_MASK_WATER);

				if (tempTrace.contents & CONTENTS_MASK_WATER) // All is good
				{
					// This is our end
					lastWaterStart = tempTrace.EndPos;
					continue; // Head to the next area
				}
			}
			// We didn't reach air if we got here.
			// Let water handle it, it will act as solid.

			// Continue the loop
			continue;
		}
		// Transparent water
		else if ((Trace.contents & CONTENTS_MASK_WATER) &&
			(Trace.surface && (Trace.surface->flags & (SURF_TEXINFO_TRANS33|SURF_TEXINFO_TRANS66))))
		{
			// This won't count as "water" since we can see through it.
			// It has the same PVS, meaning we don't need to
			// do complex tracing.
			// Copy up our point for the effect
			lastWaterEnd = Trace.EndPos;

			// Draw the effect we have so far
			DoEffect (from, Trace.EndPos, false);

			// Water hit effect
			DoWaterHit (&Trace);

			// Set up water drawing
			Water = true;
			Mask = CONTENTS_MASK_SHOT;

			// Find the exit point
			int tries = 20; // Cover about 2000 units
			vec3f	stWater = from;
			lastWaterStart.Clear();
			
			while (tries > 0)
			{
				stWater = stWater.MultiplyAngles (HITSCANSTEP, aimdir);

				EBrushContents contents = PointContents(stWater);
				if (contents == 0) // "Clear" or solid
					break; // Got it
				else if (contents & CONTENTS_MASK_SOLID)
				{
					// This is a special case in case we run into a solid.
					// This basically means that the trace is done, so we can skip ahead right to the solid (act like
					// we're not in water)
					tries = 0;
					break;
				}
				tries --;
			}

			if (tries != 0)
			{
				// We reached air
				// Trace backwards and grab the water brush
				vec3f tempOrigin = stWater.MultiplyAngles (-(HITSCANSTEP + 5), aimdir);
				CTrace tempTrace = DoTrace (stWater, tempOrigin, NULL, CONTENTS_MASK_WATER);

				if (tempTrace.contents & CONTENTS_MASK_WATER) // All is good
				{
					// This is our end
					lastWaterStart = tempTrace.EndPos;
					continue; // Head to the next area
				}
			}
			// We didn't reach air if we got here.
			// Let water handle it, it will act as solid.

			// Continue the loop
			continue;
		}
		// Assume solid
		else
		{
			// Draw the effect
			DoEffect (from, Trace.EndPos, false);
			DoSolidHit (&Trace);
			break; // We're done
		}
	}
}

void CShotgunPellets::DoSolidHit	(CTrace *Trace)
{
	if (!(Trace->surface->flags & SURF_TEXINFO_SKY))
		CTempEnt_Splashes::Shotgun (Trace->EndPos, Trace->plane.normal);
}

void CShotgunPellets::Fire(CBaseEntity *Entity, vec3f start, vec3f aimdir, int damage, int kick, int hSpread, int vSpread, int Count, int mod)
{
	for (int i = 0; i < Count; i++)
		CShotgunPellets(damage, kick, hSpread, vSpread, mod).DoFire (Entity, start, aimdir);
}

bool CMeleeWeapon::Fire(CBaseEntity *Entity, vec3f aim, int damage, int kick)
{
	vec3f		forward, right, up, point, dir;
	float		range;

	//see if enemy is in range
	CBaseEntity *Enemy = Entity->Enemy;

	dir = Enemy->State.GetOrigin() - Entity->State.GetOrigin();
	range = dir.Length();
	if (range > aim.X)
		return false;

	if (aim.Y > Entity->GetMins().X && aim.Y < Entity->GetMaxs().X)
		// the hit is straight on so back the range up to the edge of their bbox
		range -= Enemy->GetMaxs().X;
	else
	{
		// this is a side hit so adjust the "right" value out to the edge of their bbox
		if (aim.Y < 0)
			aim.Y = Enemy->GetMins().X;
		else
			aim.Y = Enemy->GetMaxs().X;
	}

	point = Entity->State.GetOrigin().MultiplyAngles (range, dir);

	CTrace tr (Entity->State.GetOrigin(), point, Entity, CONTENTS_MASK_SHOT);
	if (tr.fraction == 1.0)
		return false;

	if (!((tr.Ent->EntityFlags & ENT_HURTABLE) && entity_cast<CHurtableEntity>(tr.Ent)->CanTakeDamage))
		return false;

	// if it will hit any client/monster then hit the one we wanted to hit
	CBaseEntity *Hit = tr.Ent;
	if ((tr.Ent->EntityFlags & ENT_MONSTER) || ((tr.Ent->EntityFlags & ENT_PLAYER)))
		Hit = Enemy;

	Entity->State.GetAngles().ToVectors (&forward, &right, &up);
	point = Entity->State.GetOrigin().MultiplyAngles (range, forward);
	point = point.MultiplyAngles (aim.Y, right);
	point = point.MultiplyAngles (aim.Z, up);
	dir = point - Enemy->State.GetOrigin();

	// do the damage
	if ((Hit->EntityFlags & ENT_HURTABLE) && entity_cast<CHurtableEntity>(Hit)->CanTakeDamage)
		entity_cast<CHurtableEntity>(Hit)->TakeDamage (Entity, Entity, dir, point, vec3fOrigin, damage, kick/2, DAMAGE_NO_KNOCKBACK, MOD_HIT);

	if (!(Hit->EntityFlags & ENT_MONSTER) && (!(Hit->EntityFlags & ENT_PLAYER)))
		return false;

	// do our special form of knockback here
	if (Enemy->EntityFlags & ENT_PHYSICS)
	{
		CPhysicsEntity *PhysEnemy = entity_cast<CPhysicsEntity>(Enemy);
		vec3f v = PhysEnemy->GetAbsMin().MultiplyAngles (0.5f, PhysEnemy->GetSize()) - point;
		v.Normalize ();
		PhysEnemy->Velocity = v.MultiplyAngles (kick, v);
		if (PhysEnemy->Velocity.Z > 0)
			PhysEnemy->GroundEntity = NULL;
	}
	return true;
}

#ifdef CLEANCTF_ENABLED

CGrappleEntity::CGrappleEntity () :
CBaseEntity(),
CFlyMissileProjectile(),
CTouchableEntity()
{
};

CGrappleEntity::CGrappleEntity (int Index) :
CBaseEntity(Index),
CFlyMissileProjectile(Index),
CTouchableEntity(Index)
{
};

void CGrappleEntity::GrappleDrawCable()
{
	vec3f	start, end, f, r, origin = Player->State.GetOrigin ();
	
	Player->Client.ViewAngle.ToVectors (&f, &r, NULL);
	Player->P_ProjectSource (vec3f(16, 16, Player->ViewHeight-8), f, r, start);

	vec3f offset = start - origin;
	vec3f dir = start - State.GetOrigin();

	// don't draw cable if close
	if (dir.LengthFast() < 64)
		return;

	// adjust start for beam origin being in middle of a segment
	end = State.GetOrigin();
	CTempEnt_Trails::GrappleCable (origin, end, Player->State.GetNumber(), offset);
};

void CGrappleEntity::GrapplePull()
{
	byte volume = (Player->Client.Timers.SilencerShots) ? 51 : 255;

	if ((Player->Client.Persistent.Weapon->Item == NItems::Grapple) &&
		!Player->Client.NewWeapon &&
		(Player->Client.WeaponState != WS_FIRING) &&
		(Player->Client.WeaponState != WS_ACTIVATING))
	{
		ResetGrapple();
		return;
	}

	if (Enemy)
	{
		if (Enemy->GetSolid() == SOLID_NOT)
		{
			ResetGrapple();
			return;
		}

		if (Enemy->GetSolid() == SOLID_BBOX)
		{
			State.GetOrigin() = (Enemy->GetSize() * 0.5f) + Enemy->State.GetOrigin() + Enemy->GetMins();
			Link ();
		}
		else if (Enemy->EntityFlags & ENT_PHYSICS)
			Velocity = entity_cast<CPhysicsEntity>(Enemy)->Velocity;

		if (Enemy && ((Enemy->EntityFlags & ENT_HURTABLE) && entity_cast<CHurtableEntity>(Enemy)->CanTakeDamage))
		{
			CHurtableEntity *Hurt = entity_cast<CHurtableEntity>(Enemy);
			if (!Hurt->CheckTeamDamage (Player))
			{
				Hurt->TakeDamage (this, Player, Velocity, State.GetOrigin(), vec3fOrigin, 1, 1, 0, MOD_GRAPPLE);
				PlaySound (CHAN_WEAPON, SoundIndex("weapons/grapple/grhurt.wav"), volume);
			}
		}
		if (Enemy && (Enemy->EntityFlags & ENT_HURTABLE) && entity_cast<CHurtableEntity>(Enemy)->DeadFlag)
		{ // he died
			ResetGrapple();
			return;
		}
	}

	GrappleDrawCable();

	if (Player->Client.ctf_grapplestate > CTF_GRAPPLE_STATE_FLY)
	{
		// pull player toward grapple
		// this causes icky stuff with prediction, we need to extend
		// the prediction layer to include two new fields in the player
		// move stuff: a point and a velocity.  The client should add
		// that velociy in the direction of the point
		vec3f forward, up;

		Player->Client.ViewAngle.ToVectors (&forward, &up, NULL);
		vec3f v = Player->State.GetOrigin ();
		v.Z += Player->ViewHeight;
		vec3f hookdir = State.GetOrigin() - v;

		float vlen = hookdir.LengthFast();

		if ((Player->Client.ctf_grapplestate == CTF_GRAPPLE_STATE_PULL) &&
			vlen < 64)
		{
			Player->Client.PlayerState.GetPMove()->pmFlags |= PMF_NO_PREDICTION;
			Player->PlaySound (CHAN_WEAPON, SoundIndex("weapons/grapple/grhang.wav"), volume);
			Player->Client.ctf_grapplestate = CTF_GRAPPLE_STATE_HANG;
		}

		hookdir.NormalizeFast ();
		hookdir *= CTF_GRAPPLE_PULL_SPEED;
		Player->Velocity = hookdir;
		Player->AddGravity();
	}
};

void CGrappleEntity::ResetGrapple ()
{
	Player->Client.ctf_grapple = NULL;
	Player->Client.ctf_grapplereleasetime = level.Frame;
	Player->Client.ctf_grapplestate = CTF_GRAPPLE_STATE_HANG+1; // we're firing, not on hook
	Player->Client.PlayerState.GetPMove()->pmFlags &= ~PMF_NO_PREDICTION;
	Free ();
};

void CGrappleEntity::Spawn (CPlayerEntity *Spawner, vec3f start, vec3f dir, int damage, int speed)
{
	CGrappleEntity *Grapple = QNew (com_levelPool, 0) CGrappleEntity;
	Grapple->Player = Spawner;
	Grapple->Damage = damage;

	dir.NormalizeFast();

	Grapple->State.GetOrigin() = start;
	Grapple->State.GetOldOrigin() = start;
	Grapple->State.GetAngles() = dir.ToAngles();
	Grapple->Velocity = dir * speed;
	Grapple->GetClipmask() = CONTENTS_MASK_SHOT;
	Grapple->GetSolid() = SOLID_BBOX;
	Grapple->GetMins().Clear ();
	Grapple->GetMaxs().Clear ();
	Grapple->State.GetModelIndex() = ModelIndex ("models/weapons/grapple/hook/tris.md2");
	Grapple->SetOwner (Spawner);
	Spawner->Client.ctf_grapple = Grapple;
	Spawner->Client.ctf_grapplestate = CTF_GRAPPLE_STATE_FLY; // we're firing, not on hook
	Grapple->Link ();

	CTrace tr (Spawner->State.GetOrigin(), Grapple->State.GetOrigin(), Grapple, CONTENTS_MASK_SHOT);
	if (tr.fraction < 1.0)
	{
		Grapple->State.GetOrigin() = Grapple->State.GetOrigin().MultiplyAngles (-10, dir);
		Grapple->Touch (tr.ent->Entity, NULL, NULL);
	}
};

void CGrappleEntity::Touch (CBaseEntity *other, plane_t *plane, cmBspSurface_t *surf)
{
	if (other == Player)
		return;

	if (Player->Client.ctf_grapplestate != CTF_GRAPPLE_STATE_FLY)
		return;

	if (surf && (surf->flags & SURF_TEXINFO_SKY))
	{
		ResetGrapple();
		return;
	}

	Velocity.Clear ();
	Player->PlayerNoiseAt (State.GetOrigin(), PNOISE_IMPACT);

	if ((other->EntityFlags & ENT_HURTABLE) && entity_cast<CHurtableEntity>(other)->CanTakeDamage)
	{
		entity_cast<CHurtableEntity>(other)->TakeDamage (other, this, Player, Velocity, State.GetOrigin(), (plane) ? plane->normal : vec3fOrigin, Damage, 1, 0, MOD_GRAPPLE);
		ResetGrapple();
		return;
	}

	Player->Client.ctf_grapplestate = CTF_GRAPPLE_STATE_PULL; // we're on hook
	Enemy = other;

	GetSolid() = SOLID_NOT;

	byte volume = (Player->Client.Timers.SilencerShots) ? 51 : 255;
	Player->PlaySound (CHAN_WEAPON, SoundIndex("weapons/grapple/grpull.wav"), volume);
	PlaySound (CHAN_WEAPON, SoundIndex("weapons/grapple/grhit.wav"), volume);

	CTempEnt_Splashes::Sparks (State.GetOrigin(), (!plane) ? vec3fOrigin : plane->normal);
};

bool CGrappleEntity::Run ()
{
	return CFlyMissileProjectile::Run();
};
#endif

// Taser test
#if 0
class CTazerProjectile : public CThinkableEntity, public CTouchableEntity, public CFlyMissileProjectile
{
public:
	class CTazerBase	*Base; // Same as owner, here for convenience
	CBaseEntity			*OwnedPlayer;
	CHurtableEntity		*Attached;
	vec3f				Offset;
	FrameNumber_t		NextZapTime;
	byte				NumZaps;
	int					Damage;

	CTazerProjectile () :
	  CBaseEntity (),
	  CThinkableEntity (),
	  CTouchableEntity (),
	  CFlyMissileProjectile (),
	  Base (NULL),
	  Attached(NULL)
	{
	};

	CTazerProjectile (int Index) :
	  CBaseEntity (Index),
	  CThinkableEntity (Index),
	  CTouchableEntity (Index),
	  CFlyMissileProjectile (Index),
	  Base (NULL),
	  Attached(NULL)
	{
	};

	bool Run ()
	{
		return (!Attached) ? CFlyMissileProjectile::Run() : CBaseEntity::Run();
	};

	void Think ();

	void Touch (CBaseEntity *other, plane_t *plane, cmBspSurface_t *surf)
	{
		if (other == World)
		{
			GetSolid() = SOLID_NOT;
			GetClipmask() = 0;
			Velocity.Clear ();
		}

		if (!(other->EntityFlags & ENT_HURTABLE))
			return;

		Attached = entity_cast<CHurtableEntity>(other);
		Velocity.Clear ();
		Offset = State.GetOrigin() - Attached->State.GetOrigin();

		NextThink = level.Frame + FRAMETIME;
		NextZapTime = level.Frame + FRAMETIME + (int)(frand() * 4);
		NumZaps = 0;

		GetSolid() = SOLID_NOT;
		GetClipmask() = 0;
	};
};

class CTazerBase : public CHurtableEntity, public CThinkableEntity, public CTossProjectile
{
public:
	CTazerProjectile	*Projectiles[2];

	CTazerBase () :
	  CBaseEntity (),
	  CHurtableEntity (),
	  CThinkableEntity (),
	  CTossProjectile ()
	{
		Projectiles[0] = Projectiles[1] = NULL;
	};

	CTazerBase (int Index) :
	  CBaseEntity (Index),
	  CHurtableEntity (Index),
	  CThinkableEntity (Index),
	  CTossProjectile (Index)
	{
		Projectiles[0] = Projectiles[1] = NULL;
	};

	bool Run ()
	{
		return CTossProjectile::Run();
	};

	void Pain (CBaseEntity *other, float kick, int damage)
	{
	};
	void Die (CBaseEntity *inflictor, CBaseEntity *attacker, int damage, vec3f &point)
	{
		if (Projectiles[0])
			Projectiles[0]->Free ();
		if (Projectiles[1])
			Projectiles[1]->Free ();

		vec3f explOrigin = State.GetOrigin() + vec3f(0,0,2);
		CTempEnt_Explosions::RocketExplosion (explOrigin, gameEntity);

		Free ();
	};

	void Think ()
	{
		if (Projectiles[0] || Projectiles[1])
		{
			Die (this, this, 0, vec3fOrigin);
			return;
		}

		// Two frames in, spawn both projectiles
		vec3f forward, right;
		State.GetAngles().ToVectors (&forward, &right, NULL);

		vec3f results[2];

		vec3f offset (0, -5, 0);
		G_ProjectSource (State.GetOrigin(), offset, forward, right, results[0]);

		offset = vec3f (0, 5, 0);
		G_ProjectSource (State.GetOrigin(), offset, forward, right, results[1]);

		forward.NormalizeFast ();

		for (byte i = 0; i < 2; i++)
		{
			Projectiles[i] = QNew (com_levelPool, 0) CTazerProjectile;

			Projectiles[i]->State.GetOrigin() = results[i];
			Projectiles[i]->State.GetOldOrigin() = results[i];
			Projectiles[i]->State.GetAngles() = forward.ToAngles();
			Projectiles[i]->Velocity = forward * 600;
			Projectiles[i]->GetClipmask() = CONTENTS_MASK_SHOT;
			Projectiles[i]->GetSolid() = SOLID_BBOX;
			Projectiles[i]->GetMins().Clear ();
			Projectiles[i]->GetMaxs().Clear ();
			Projectiles[i]->State.GetModelIndex() = ModelIndex ("models/monsters/parasite/tip/tris.md2");
			Projectiles[i]->SetOwner (this);
			Projectiles[i]->Base = this;
			Projectiles[i]->NextThink = level.Frame + FRAMETIME;
			Projectiles[i]->OwnedPlayer = GetOwner();

			Projectiles[i]->Link ();
		}

		NextThink = level.Frame + 100;
	};

	static void Spawn (CBaseEntity *Spawner, vec3f origin, vec3f dir, int damage, int speed)
	{
		CTazerBase *Base = QNew (com_levelPool, 0) CTazerBase;

		dir.NormalizeFast ();

		Base->State.GetOrigin() = origin;
		Base->State.GetOldOrigin() = origin;
		Base->State.GetAngles() = dir.ToAngles();
		Base->Velocity = dir * speed;
		Base->GetClipmask() = CONTENTS_MASK_SHOT;
		Base->GetSolid() = SOLID_BBOX;
		Base->GetMins().Clear ();
		Base->GetMaxs().Clear ();
		Base->State.GetModelIndex() = ModelIndex ("models/objects/grenade/tris.md2");
		Base->NextThink = level.Frame + 1;
		Base->SetOwner (Spawner);
		Base->Link ();
	};
};

void SpawnTazerProjectile (CBaseEntity *Spawner, vec3f origin, vec3f dir, int damage, int speed)
{
	CTazerBase::Spawn (Spawner, origin, dir, damage, speed);
}

void CTazerProjectile::Think ()
{
	// Lightning back to base
	vec3f or1 = State.GetOrigin(), or2 = Base->State.GetOrigin();
	::CTempEnt_Trails::FleshCable (or1, or2, State.GetNumber());

	NextThink = level.Frame + FRAMETIME;

	if (!Attached)
		return;

	if (Attached->Health <= Attached->GibHealth)
	{
		Base->Die (this, this, 0, vec3fOrigin);
		return;
	}

	if (!IsVisible (this, Base))
	{
		if (Base->Projectiles[0] == this)
		{
			Base->Projectiles[0]->Free ();
			Base->Projectiles[0] = NULL;
		}
		else
		{
			Base->Projectiles[1]->Free ();
			Base->Projectiles[1] = NULL;
		}

		if (!Base->Projectiles[0] && !Base->Projectiles[1])
			Base->Free ();

		return;
	}

	if (NextZapTime < level.Frame)
	{
		NextZapTime = level.Frame + FRAMETIME + (int)(frand() * 8) + (int)(frand() * 8);
		NumZaps++;

		Attached->PlaySound (CHAN_AUTO, SoundIndex("world/spark3.wav"));
		Attached->TakeDamage (this, OwnedPlayer, vec3fOrigin, State.GetOrigin(), vec3fOrigin, 5, 0, 0, MOD_BLASTER);
	}

	// Attach
	State.GetOrigin() = (Attached->State.GetOrigin() + Offset);
	Link ();
};
#endif