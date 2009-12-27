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
// cc_xatrix_trap.cpp
// 
//

#include "cc_local.h"

#if XATRIX_FEATURES
#include "cc_weaponmain.h"
#include "m_player.h"
#include "cc_xatrix_trap.h"
#include "cc_tent.h"

/*
=================
CTrapProjectile
=================
*/

CTrapProjectile::CTrapProjectile () :
CBounceProjectile(),
CTouchableEntity(),
CThinkableEntity()
{
};

CTrapProjectile::CTrapProjectile (sint32 Index) :
CBounceProjectile(Index),
CTouchableEntity(Index),
CThinkableEntity(Index)
{
};

IMPLEMENT_SAVE_SOURCE(CTrapProjectile)

void CTrapProjectile::Touch (CBaseEntity *other, plane_t *plane, cmBspSurface_t *surf)
{
}

class CJustAModel : public CBaseEntity
{
public:
	CJustAModel () :
	  CBaseEntity ()
	  {
	  };

	ENTITYFIELDS_NONSAVABLE
	const char *SAVE_GetName () { return "NonSavable"; }
};

// Foodcube
CFoodCube::CFoodCube () :
  CBasePowerUp ("item_foodcube", "models/objects/trapfx/tris.md2", 0, "items/m_health.wav", "i_health", "Health", ITEMFLAG_HEALTH|ITEMFLAG_POWERUP|ITEMFLAG_GRABBABLE, "", 0)
{
};

void CFoodCube::DoPickup (CItemEntity *ent, CPlayerEntity *other)
{
	other->Health += ent->AmmoCount;
	ent->PlaySound (CHAN_ITEM, GetPickupSound());
};

void CTrapProjectile::Think ()
{
	if (DoFree)
	{
		Free ();
		return;
	}
	
	if (TimeStamp < level.Frame)
	{
		BecomeExplosion(false);
		// note to self
		// cause explosion damage???
		return;
	}
	
	NextThink = level.Frame + 1;
	
	if (!GroundEntity)
		return;

	// ok lets do the blood effect
	if (State.GetFrame() > 4)
	{
		if (State.GetFrame() == 5)
		{
			if (Wait == 64)
				PlaySound (CHAN_VOICE, SoundIndex ("weapons/trapdown.wav"), 255, ATTN_IDLE);

			Wait -= 2;
			Delay += level.Frame;

			for (uint8 i = 0; i < 3; i++)
			{
				CBaseEntity *best;
				if (!TrapEntities[i])
				{
					best = QNewEntityOf CJustAModel;
					TrapEntities[i] = best;

					best->GetSolid() = SOLID_NOT;
					best->GetSvFlags() |= SVF_MONSTER;
					best->GetMins().Clear();
					best->GetMaxs().Clear();

					/*if (strcmp (ent->enemy->classname, "monster_gekk") == 0)
					{
						best->s.modelindex = gi.modelindex ("models/objects/gekkgib/torso/tris.md2");	
						best->s.effects |= EF_GREENGIB;
					}
					else */if (Mass >= 200)
					{
						best->State.GetModelIndex() = GameMedia.Gib_Chest;	
						best->State.GetEffects() |= EF_GIB;
					}
					else
					{
						best->State.GetModelIndex() = GameMedia.Gib_SmallMeat;	
						best->State.GetEffects() |= EF_GIB;
					}
				}
				else
					best = TrapEntities[i];

				vec3f forward, right, up, vec;
				State.GetAngles().ToVectors (&forward, &right, &up);

				RotatePointAroundVector ((float*)&vec.X, (float*)&up.X, (float*)&right.X, ((360.0/3)* i)+Delay);
				vec = vec.MultiplyAngles (Wait/2, vec) + State.GetOrigin();
				best->State.GetOrigin() = vec + forward;
				best->State.GetOrigin().Z = State.GetOrigin().Z + Wait;
  				best->State.GetAngles() = State.GetAngles();

				best->Link ();
			}
				
			if (Wait < 19)
			{
				State.GetFrame() ++;
				for (int i = 0; i < 3; i++)
					TrapEntities[i]->Free ();
			}

			return;
		}
		State.GetFrame() ++;
		if (State.GetFrame() == 8)
		{
			NextThink = level.Frame + 10;
			DoFree = true;

			CItemEntity *it_ent = QNewEntityOf CItemEntity;
			it_ent->ClassName = "item_foodcube";
			it_ent->SpawnFlags |= DROPPED_ITEM;
			it_ent->Spawn (FoodCubeItem);
			it_ent->State.GetOrigin() = State.GetOrigin() + vec3f(0, 0, 16);
			it_ent->Velocity.Z = 400;
			it_ent->AmmoCount = Mass;
			it_ent->Link ();

			return;
		}
		return;
	}
	
	State.GetEffects() &= ~EF_TRAP;
	if (State.GetFrame() >= 4)
	{
		State.GetEffects() |= EF_TRAP;
		GetMins().Clear();
		GetMaxs().Clear();

	}

	if (State.GetFrame() < 4)
		State.GetFrame()++;

	CHurtableEntity *target = NULL;
	CHurtableEntity *best = NULL;
	float oldlen = 99999;
	while ((target = FindRadius<CHurtableEntity, ENT_HURTABLE> (target, State.GetOrigin (), 256)) != NULL)
	{
		if (target->Health <= 0)
			continue;
		if (!(target->EntityFlags & ENT_PLAYER) && !(target->EntityFlags & ENT_MONSTER))
			continue;
		if (!IsVisible (this, target))
		 	continue;

		if (!best)
		{
			best = target;
			continue;
		}

		float Len = (State.GetOrigin() - target->State.GetOrigin()).Length();
		if (Len < oldlen)
		{
			oldlen = Len;
			best = target;
		}
	}

	// pull the enemy in
	if (best)
	{
		vec3f	forward;

		if (best->GroundEntity)
		{
			best->State.GetOrigin().Z += 1;
			best->GroundEntity = NULL;
			best->Link ();
		}

		vec3f vec = State.GetOrigin() - best->State.GetOrigin();

		if (best->EntityFlags & ENT_PLAYER)
		{
			CPlayerEntity *Player = entity_cast<CPlayerEntity>(best);
			Player->Velocity = Player->Velocity.MultiplyAngles (250, vec.GetNormalized());
		}
		else
		{
			CMonsterEntity *Monster = entity_cast<CMonsterEntity>(best);
			Monster->Monster->IdealYaw = vec.ToYaw();	
			Monster->Monster->ChangeYaw ();
			best->State.GetAngles().ToVectors (&forward, NULL, NULL);
			Monster->Velocity = forward * 256;
		}

		PlaySound(CHAN_VOICE, SoundIndex ("weapons/trapsuck.wav"), 255, ATTN_IDLE);
		
		if (vec.Length() < 32)
		{
			if (best->EntityFlags & ENT_PHYSICS)
			{
				CPhysicsEntity *Phys = entity_cast<CPhysicsEntity>(best);
				
				if (Phys->Mass < 400)
				{
					best->TakeDamage (this, GetOwner(), vec3fOrigin, best->State.GetOrigin(), vec3fOrigin, 100000, 1, 0, MOD_TRAP);
					Enemy = best;
					Wait = 64;
					State.GetOldOrigin() = State.GetOrigin();
					TimeStamp = level.Frame + 300;
					if (game.GameMode & GAME_DEATHMATCH)
						Mass = Phys->Mass/4;
					else
						Mass = Phys->Mass/10;
					// ok spawn the food cube
					State.GetFrame() = 5;	
				}
			}
			else
			{
				BecomeExplosion(false);
				// note to self
				// cause explosion damage???
				return;
			}	
		}
	}
}

#define TRAP_RADIUS_DAMAGE 165
void CTrapProjectile::Explode ()
{
	if (GetOwner() && (GetOwner()->EntityFlags & ENT_PLAYER))
		entity_cast<CPlayerEntity>(GetOwner())->PlayerNoiseAt (State.GetOrigin (), PNOISE_IMPACT);

	//FIXME: if we are onground then raise our Z just a bit since we are a point?
	if (Enemy)
	{
		CHurtableEntity *HurtEnemy = entity_cast<CHurtableEntity>(Enemy);

		vec3f v = (HurtEnemy->GetMins() + HurtEnemy->GetMaxs());
		v = (State.GetOrigin () - HurtEnemy->State.GetOrigin().MultiplyAngles (0.5f, v));

		float points = ((float)Damage) - 0.5 * v.Length();
		vec3f dir = HurtEnemy->State.GetOrigin() - State.GetOrigin ();

		HurtEnemy->TakeDamage	(this, GetOwner(), dir, State.GetOrigin (), vec3fOrigin, (sint32)points, (sint32)points,
							DAMAGE_RADIUS, MOD_HANDGRENADE);
	}

	SplashDamage(GetOwner(), Damage, (Enemy) ? Enemy : NULL, TRAP_RADIUS_DAMAGE, MOD_HELD_GRENADE);

	vec3f origin = State.GetOrigin ().MultiplyAngles (-0.02f, Velocity);
	if (GroundEntity)
		CTempEnt_Explosions::GrenadeExplosion(origin, this, !!WaterInfo.Level);
	else
		CTempEnt_Explosions::RocketExplosion(origin, this, !!WaterInfo.Level);

	Free (); // "delete" the entity
}

void CTrapProjectile::Spawn (CBaseEntity *Spawner, vec3f start, vec3f aimdir, int damage, float timer, sint32 speed)
{
	CTrapProjectile	*Grenade = QNewEntityOf CTrapProjectile;
	vec3f		forward, right, up;

	vec3f dir = aimdir.ToAngles();

	dir.ToVectors (&forward, &right, &up);

	Grenade->State.GetOrigin() = start;
	aimdir *= speed;

	Grenade->Velocity = aimdir;
	Grenade->Velocity = Grenade->Velocity.MultiplyAngles (200 + crand() * 10.0f, up);
	Grenade->Velocity = Grenade->Velocity.MultiplyAngles (crand() * 10.0, right);

	Grenade->AngularVelocity.Set (0, 300, 0);
	Grenade->State.GetModelIndex() = ModelIndex ("models/weapons/z_trap/tris.md2");
	Grenade->SetOwner(Spawner);
	Grenade->NextThink = level.Frame + 10;
	Grenade->ClassName = "htrap";
	Grenade->GetClipmask() = CONTENTS_MASK_SHOT;
	Grenade->GetSolid() = SOLID_BBOX;
	Grenade->GetMins().Set (-4, -4, 0);
	Grenade->GetMaxs().Set (4, 4, 8);
	Grenade->Touchable = true;
	Grenade->State.GetSound() = SoundIndex("weapons/traploop.wav");
	Grenade->TimeStamp = level.Frame + 300;
	Grenade->Damage = damage;

	if (timer <= 0.0)
		Grenade->Explode ();
	else
		Grenade->Link();
}

bool CTrapProjectile::Run ()
{
	return CBounceProjectile::Run();
}

CTrap::CTrap() :
CWeapon(14, "models/weapons/v_trap/tris.md2", 0, 0, 0, 16,
		17, 48, 0, 0)
{
}

bool CTrap::CanStopFidgetting (CPlayerEntity *ent)
{
	switch (ent->Client.PlayerState.GetGunFrame())
	{
	case 29:
	case 34:
	case 39:
	case 48:
		return true;
	}
	return false;
}


#define TRAP_TIMER			50
#define TRAP_MINSPEED		300
#define TRAP_MAXSPEED		700

void CTrap::Hold (CPlayerEntity *ent)
{
	if (!ent->Client.Grenade.Time)
	{
		ent->Client.Grenade.Time = level.Frame + TRAP_TIMER + 2;
		ent->Client.WeaponSound = SoundIndex("weapons/traploop.wav");
	}

	// they waited too long, detonate it in their hand
	if (!ent->Client.Grenade.BlewUp && (level.Frame >= ent->Client.Grenade.Time))
	{
		ent->Client.WeaponSound = 0;
		FireGrenade (ent, true);
		ent->Client.Grenade.BlewUp = true;

		ent->Client.PlayerState.GetGunFrame() = 15;
		return;
	}

	if (ent->Client.Buttons & BUTTON_ATTACK)
		return;

	ent->Client.PlayerState.GetGunFrame()++;
}

void CTrap::FireGrenade (CPlayerEntity *ent, bool inHand)
{
	vec3f	offset (8, 8, ent->ViewHeight-8), forward, right, start;
	const sint32		damage = CalcQuadVal(125);

	ent->Client.Grenade.Thrown = true;

	ent->Client.ViewAngle.ToVectors (&forward, &right, NULL);
	ent->P_ProjectSource (offset, forward, right, start);

	float timer = (float)(ent->Client.Grenade.Time - level.Frame) / 10;
	const sint32 speed = (ent->Client.Persistent.Weapon) ? 
		(TRAP_MINSPEED + ((TRAP_TIMER/10) - timer) * ((TRAP_MAXSPEED - TRAP_MINSPEED) / (TRAP_TIMER/10)))
		: 25; // If we're dead, don't toss it 5 yards.
	CTrapProjectile::Spawn (ent, start, forward, damage, timer, speed);

	ent->Client.Grenade.Time = level.Frame + (((
#if CLEANCTF_ENABLED
	(game.GameMode & GAME_CTF) || 
#endif
	dmFlags.dfDmTechs.IsEnabled()) && ent->ApplyHaste()) ? 5 : 10);
	DepleteAmmo(ent, 1);

	if (ent->Health <= 0 || ent->DeadFlag || ent->State.GetModelIndex() != 255) // VWep animations screw up corpses
		return;

	AttackSound (ent);

	if (ent->Client.PlayerState.GetPMove()->pmFlags & PMF_DUCKED)
	{
		ent->Client.Anim.Priority = ANIM_ATTACK;
		ent->State.GetFrame() = FRAME_crattak1 - 1;
		ent->Client.Anim.EndFrame = FRAME_crattak3;
	}
	else
	{
		ent->Client.Anim.Priority = ANIM_REVERSE;
		ent->State.GetFrame() = FRAME_wave08;
		ent->Client.Anim.EndFrame = FRAME_wave01;
	}
	ent->Client.PlayerState.GetGunFrame()++;
}

void CTrap::Wait (CPlayerEntity *ent)
{
	ent->Client.Grenade.BlewUp = false;
	if (level.Frame < ent->Client.Grenade.Time)
		return;

	if (!ent->DeadFlag)
		ent->Client.Grenade.Thrown = false;
	ent->Client.PlayerState.GetGunFrame()++;
}

void CTrap::Fire (CPlayerEntity *ent)
{
	switch (ent->Client.PlayerState.GetGunFrame())
	{
	case 11:
		Hold (ent);
		break;
	case 12:
		ent->Client.WeaponSound = 0;
		FireGrenade(ent, false);
		break;
	case 15:
		Wait(ent);
		break;
	}
}

bool CTrap::CanFire (CPlayerEntity *ent)
{
	switch (ent->Client.PlayerState.GetGunFrame())
	{
	case 5:
		ent->PlaySound (CHAN_WEAPON, SoundIndex("weapons/trapcock.wav"));
		return false;
	case 11:
	case 12:
	case 15:
		return true;
	}
	return false;
}

void CTrap::WeaponGeneric (CPlayerEntity *ent)
{
	// Idea from Brazen source
	sint32 newFrame = -1, newState = -1;

	switch (ent->Client.WeaponState)
	{
	case WS_ACTIVATING:
		newFrame = IdleStart;
		newState = WS_IDLE;
		break;
	case WS_IDLE:
		if (ent->Client.NewWeapon && ent->Client.NewWeapon != this)
		{
			// We want to go away!
			newState = WS_DEACTIVATING;
			newFrame = DeactStart;
		}
		else if ((ent->Client.Buttons|ent->Client.LatchedButtons) & BUTTON_ATTACK)
		{
			ent->Client.LatchedButtons &= ~BUTTON_ATTACK;

			// We want to attack!
			// First call, check AttemptToFire
			if (AttemptToFire(ent))
			{
				// Got here, we can fire!
				ent->Client.PlayerState.GetGunFrame() = FireStart;
				ent->Client.WeaponState = WS_FIRING;
				ent->Client.Grenade.Time = 0;

				// We need to check against us right away for first-frame firing
				WeaponGeneric(ent);
				return;
			}
			else
			{
				OutOfAmmo(ent);
				NoAmmoWeaponChange (ent);
			}
		}

		// Either we are still idle or a failed fire.
		if (newState == -1)
		{
			if (ent->Client.PlayerState.GetGunFrame() == IdleEnd)
				newFrame = IdleStart;
			else
			{
				if (CanStopFidgetting(ent) && (randomMT()&15))
					newFrame = ent->Client.PlayerState.GetGunFrame();
			}
		}
		break;
	case WS_FIRING:
		// Check if this is a firing frame.
		if (CanFire(ent))
		{
			Fire(ent);

			// Now, this call above CAN change the underlying frame and state.
			// We need this block to make sure we are still doing what we are supposed to.
			newState = ent->Client.WeaponState;
			newFrame = ent->Client.PlayerState.GetGunFrame();
		}

		// Only do this if we haven't been explicitely set a newFrame
		// because we might want to keep firing beyond this point
		if (newFrame == -1 && ent->Client.PlayerState.GetGunFrame() == FireEnd)
		{
			if (!ent->Client.Persistent.Inventory.Has(Item))
			{
				NoAmmoWeaponChange (ent);
				ent->Client.Grenade.Time = 0;
				ent->Client.Grenade.Thrown = false;
			}
			else
			{
				ent->Client.Grenade.Time = 0;
				ent->Client.Grenade.Thrown = false;
				newFrame = IdleStart;
				newState = WS_IDLE;
			}
		}
		break;
	case WS_DEACTIVATING:
		// Change weapon
		ChangeWeapon (ent);
		return;
		break;
	}

	if (newFrame != -1)
		ent->Client.PlayerState.GetGunFrame() = newFrame;
	if (newState != -1)
		ent->Client.WeaponState = newState;

	if (newFrame == -1 && newState == -1)
		ent->Client.PlayerState.GetGunFrame()++;
}

WEAPON_DEFS (CTrap);

void CTrap::CreateItem (CItemList *List)
{
};
#endif