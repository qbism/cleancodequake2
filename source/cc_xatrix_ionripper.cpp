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
// cc_xatrix_ionripper.cpp
// 
//

#include "cc_local.h"

#if XATRIX_FEATURES
#include "cc_weaponmain.h"
#include "cc_xatrix_ionripper.h"
#include "m_player.h"

CIonRipperBoomerang::CIonRipperBoomerang () :
CFlyMissileProjectile(),
CTouchableEntity(),
CThinkableEntity()
{
};

CIonRipperBoomerang::CIonRipperBoomerang (sint32 Index) :
CFlyMissileProjectile(Index),
CTouchableEntity(Index),
CThinkableEntity(Index)
{
};

IMPLEMENT_SAVE_SOURCE(CIonRipperBoomerang)

#include "cc_tent.h"

void CIonRipperBoomerang::Think ()
{
	CTempEnt_Splashes::Sparks (State.GetOrigin(), vec3fOrigin, CTempEnt_Splashes::ST_WELDING_SPARKS, 0xe4 + (rand()&3), 0);
	Free();
}

void CIonRipperBoomerang::Touch (CBaseEntity *other, plane_t *plane, cmBspSurface_t *surf)
{
	if (other == GetOwner())
		return;

	if (surf && (surf->flags & SURF_TEXINFO_SKY))
	{
		Free (); // "delete" the entity
		return;
	}

	if (GetOwner() && (GetOwner()->EntityFlags & ENT_PLAYER))
		entity_cast<CPlayerEntity>(GetOwner())->PlayerNoiseAt (State.GetOrigin (), PNOISE_IMPACT);

	if ((other->EntityFlags & ENT_HURTABLE) && entity_cast<CHurtableEntity>(other)->CanTakeDamage)
	{
		entity_cast<CHurtableEntity>(other)->TakeDamage (this, GetOwner(), Velocity, State.GetOrigin (), plane ? plane->normal : vec3fOrigin, Damage, 1, DAMAGE_ENERGY, MOD_RIPPER);
		Free (); // "delete" the entity
	}
}

void CIonRipperBoomerang::Spawn (CBaseEntity *Spawner, vec3f start, vec3f dir,
						sint32 damage, sint32 speed)
{
	CIonRipperBoomerang		*Bolt = QNewEntityOf CIonRipperBoomerang;

	Bolt->GetSvFlags() |= (SVF_DEADMONSTER | SVF_PROJECTILE);
	Bolt->State.GetOrigin() = start;
	Bolt->State.GetOldOrigin() = start;
	Bolt->State.GetAngles() = dir.ToAngles();
	Bolt->Velocity = dir.GetNormalizedFast() * speed;

	Bolt->State.GetEffects() = EF_IONRIPPER;
	Bolt->State.GetRenderEffects() = RF_FULLBRIGHT;
	Bolt->State.GetModelIndex() = ModelIndex ("models/objects/boomrang/tris.md2");

	Bolt->State.GetSound() = SoundIndex ("misc/lasfly.wav");
	Bolt->SetOwner (Spawner);
	Bolt->NextThink = level.Frame + 30;
	Bolt->Damage = damage;
	Bolt->ClassName = "boomrang";
	Bolt->GetClipmask() = CONTENTS_MASK_SHOT;
	Bolt->GetSolid() = SOLID_BBOX;
	Bolt->GetMins().Clear ();
	Bolt->GetMaxs().Clear ();
	Bolt->Touchable = true;
	Bolt->backOff = 2.0f;
	Bolt->StopOnEqualPlane = false;
	Bolt->AimInVelocityDirection = true;
	Bolt->Link ();

	CTrace tr ((Spawner) ? Spawner->State.GetOrigin() : start, start, Bolt, CONTENTS_MASK_SHOT);
	if (tr.fraction < 1.0)
	{
		start = start.MultiplyAngles (-10, dir.GetNormalizedFast());
		Bolt->State.GetOrigin() = start;
		Bolt->State.GetOldOrigin() = start;

		if (tr.ent->Entity)
			Bolt->Touch (tr.ent->Entity, &tr.plane, tr.surface);
	}
	else if (Spawner && (Spawner->EntityFlags & ENT_PLAYER))
		CheckDodge (Spawner, start, dir, speed);
}

bool CIonRipperBoomerang::Run ()
{
	return CFlyMissileProjectile::Run();
}

CIonRipper::CIonRipper() :
CWeapon(8, 1, "models/weapons/v_boomer/tris.md2", 0, 4, 5, 6,
		7, 36, 37, 39)
{
}

bool CIonRipper::CanFire (CPlayerEntity *ent)
{
	switch (ent->Client.PlayerState.GetGunFrame())
	{
	case 5:
		return true;
	}
	return false;
}

bool CIonRipper::CanStopFidgetting (CPlayerEntity *ent)
{
	switch (ent->Client.PlayerState.GetGunFrame())
	{
	case 36:
		return true;
	}
	return false;
}

void CIonRipper::Fire (CPlayerEntity *ent)
{
	vec3f		start, forward, right, offset (16, 7,  ent->ViewHeight-8);
	const sint32	damage = (game.GameMode & GAME_DEATHMATCH) ? CalcQuadVal(30) : CalcQuadVal(50);

	(ent->Client.ViewAngle + vec3f(0, crand(), 0)).ToVectors (&forward, &right, NULL);

	ent->Client.KickOrigin = forward * -3;
	ent->Client.KickAngles.X = -3;

	ent->P_ProjectSource (offset, forward, right, start);

	CIonRipperBoomerang::Spawn (ent, start, forward, damage, 500);

	// send muzzle flash
	Muzzle (ent, MZ_IONRIPPER);
	AttackSound (ent);

	ent->Client.PlayerState.GetGunFrame()++;
	ent->PlayerNoiseAt (start, PNOISE_WEAPON);
	FireAnimation (ent);

	DepleteAmmo(ent, 2);
}

WEAPON_DEFS (CIonRipper);

void CIonRipper::CreateItem (CItemList *List)
{
	NItems::IonRipper = QNew (com_itemPool, 0) CWeaponItem
		("weapon_boomer", "models/weapons/g_boom/tris.md2", EF_ROTATE, "misc/w_pkup.wav",
		"w_ripper", "Ionripper", ITEMFLAG_DROPPABLE|ITEMFLAG_WEAPON|ITEMFLAG_GRABBABLE|ITEMFLAG_STAY_COOP|ITEMFLAG_USABLE,
		"", &Weapon, NItems::Cells, 2, "#w_boomer.md2");
};
#endif