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
// cc_xatrix_phalanx.cpp
// 
//

#include "cc_local.h"

#if XATRIX_FEATURES
#include "cc_weaponmain.h"
#include "cc_xatrix_phalanx.h"
#include "cc_tent.h"
#include "m_player.h"

CPhalanxPlasma::CPhalanxPlasma () :
CFlyMissileProjectile(),
CTouchableEntity(),
CThinkableEntity()
{
};

CPhalanxPlasma::CPhalanxPlasma (sint32 Index) :
CFlyMissileProjectile(Index),
CTouchableEntity(Index),
CThinkableEntity(Index)
{
};

IMPLEMENT_SAVE_SOURCE(CPhalanxPlasma)

void CPhalanxPlasma::Think ()
{
	Free (); // "delete" the entity
}

void CPhalanxPlasma::Touch (CBaseEntity *other, plane_t *plane, cmBspSurface_t *surf)
{
	if (other == GetOwner())
		return;

	if (surf && (surf->flags & SURF_TEXINFO_SKY))
	{
		Free ();
		return;
	}

	if (GetOwner() && (GetOwner()->EntityFlags & ENT_PLAYER))
		entity_cast<CPlayerEntity>(GetOwner())->PlayerNoiseAt (State.GetOrigin (), PNOISE_IMPACT);

	if ((other->EntityFlags & ENT_HURTABLE) && entity_cast<CHurtableEntity>(other)->CanTakeDamage)
		entity_cast<CHurtableEntity>(other)->TakeDamage (this, GetOwner(), Velocity, State.GetOrigin (), (plane) ? plane->normal : vec3fOrigin, Damage, 0, 0, MOD_ROCKET);

	// calculate position for the explosion entity
	vec3f origin = State.GetOrigin ().MultiplyAngles (-0.02f, Velocity);
	SplashDamage(GetOwner(), RadiusDamage, other, DamageRadius, MOD_R_SPLASH);
	CTempEnt_Explosions::PlasmaExplosion(origin);

	Free ();
}

CPhalanxPlasma *CPhalanxPlasma::Spawn	(CBaseEntity *Spawner, vec3f start, vec3f dir,
						sint32 damage, sint32 speed, float damage_radius, sint32 radius_damage)
{
	CPhalanxPlasma	*Rocket = QNewEntityOf CPhalanxPlasma;

	Rocket->State.GetOrigin() = start;
	Rocket->State.GetAngles() = dir.ToAngles();
	Rocket->Velocity = dir * speed;
	Rocket->State.GetEffects() = EF_PLASMA | EF_ANIM_ALLFAST;
	Rocket->State.GetModelIndex() = ModelIndex ("sprites/s_photon.sp2");
	Rocket->SetOwner (Spawner);
	Rocket->NextThink = level.Frame + 80000/speed;
	Rocket->Damage = damage;
	Rocket->RadiusDamage = radius_damage;
	Rocket->DamageRadius = damage_radius;
	Rocket->State.GetSound() = SoundIndex ("weapons/rockfly.wav");
	Rocket->ClassName = "rocket";
	Rocket->GetClipmask() = CONTENTS_MASK_SHOT;
	Rocket->GetSolid() = SOLID_BBOX;
	Rocket->GetMins().Clear ();
	Rocket->GetMaxs().Clear ();
	Rocket->Touchable = true;

	if (Spawner->EntityFlags & ENT_PLAYER)
		CheckDodge (Spawner, start, dir, speed);

	Rocket->Link ();
	return Rocket;
}

bool CPhalanxPlasma::Run ()
{
	return CFlyMissileProjectile::Run();
}

CPhalanx::CPhalanx() :
CWeapon(9, 1, "models/weapons/v_shotx/tris.md2", 0, 5, 6, 20,
		21, 58, 59, 63, "weapons/phaloop.wav")
{
}

bool CPhalanx::CanFire (CPlayerEntity *ent)
{
	switch (ent->Client.PlayerState.GetGunFrame())
	{
	case 7:
	case 8:
		return true;
	}
	return false;
}

bool CPhalanx::CanStopFidgetting (CPlayerEntity *ent)
{
	switch (ent->Client.PlayerState.GetGunFrame())
	{
	case 29:
	case 42:
	case 55:
		return true;
	}
	return false;
}

void CPhalanx::Fire (CPlayerEntity *ent)
{
	vec3f		start, forward, right, offset (0, 8, ent->ViewHeight-8);
	const sint32	damage = CalcQuadVal(70 + irandom(10.0));

	ent->Client.KickOrigin = forward * -2;
	ent->Client.KickAngles.X = -2;

	ent->Client.ViewAngle.ToVectors (&forward, &right, NULL);
	ent->P_ProjectSource (offset, forward, right, start);

	(ent->Client.ViewAngle + vec3f(0, ((ent->Client.PlayerState.GetGunFrame() == 8)) ? -1.5f : 1.5f, 0)).ToVectors (&forward, &right, NULL);

	switch (ent->Client.PlayerState.GetGunFrame())
	{
	case 8:
		CPhalanxPlasma::Spawn (ent, start, forward, damage, 725, 30, 120);
		DepleteAmmo (ent, 1);
		break;
	default:
		CPhalanxPlasma::Spawn (ent, start, forward, damage, 725, 120, CalcQuadVal(120));

		// send muzzle flash
		Muzzle (ent, MZ_PHALANX);
		AttackSound (ent);

		ent->PlayerNoiseAt (start, PNOISE_WEAPON);
		FireAnimation (ent);
		break;
	};
	ent->Client.PlayerState.GetGunFrame()++;
}

WEAPON_DEFS (CPhalanx);

void CPhalanx::CreateItem (CItemList *List)
{
	NItems::Phalanx = QNew (com_itemPool, 0) CWeaponItem
		("weapon_phalanx", "models/weapons/g_shotx/tris.md2", EF_ROTATE, "misc/w_pkup.wav",
		"w_phallanx", "Phalanx", ITEMFLAG_DROPPABLE|ITEMFLAG_WEAPON|ITEMFLAG_GRABBABLE|ITEMFLAG_STAY_COOP|ITEMFLAG_USABLE,
		"", &Weapon, NItems::MagSlugs, 1, "#w_phalanx.md2");
};
#endif
