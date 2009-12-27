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
// cc_weaponentities.h
// Entities related to weaponry
//

#if !defined(CC_GUARD_WEAPONENTITIES_H) || !INCLUDE_GUARDS
#define CC_GUARD_WEAPONENTITIES_H

class CGrenade : public CBounceProjectile, public CThinkableEntity, public CTouchableEntity
{
public:
	sint32		Damage;
	float		RadiusDamage;

	CGrenade ();
	CGrenade (sint32 Index);

	IMPLEMENT_SAVE_HEADER(CGrenade)

	void SaveFields (CFile &File)
	{
		File.Write<sint32> (Damage);
		File.Write<float> (RadiusDamage);

		CThinkableEntity::SaveFields (File);
		CTouchableEntity::SaveFields (File);
		CBounceProjectile::SaveFields (File);
	}

	void LoadFields (CFile &File)
	{
		Damage = File.Read<sint32> ();
		RadiusDamage = File.Read<float> ();

		CThinkableEntity::LoadFields (File);
		CTouchableEntity::LoadFields (File);
		CBounceProjectile::LoadFields (File);
	}

	void Think ();
	void Touch (CBaseEntity *other, plane_t *plane, cmBspSurface_t *surf);

	void Explode ();

	static void Spawn	(CBaseEntity *Spawner, vec3f start, vec3f aimdir,
						sint32 damage, sint32 speed, FrameNumber_t timer, float damage_radius, bool handNade = false, bool held = false);

	bool Run ();
};

class CBlasterProjectile : public CFlyMissileProjectile, public CThinkableEntity, public CTouchableEntity
{
public:
	sint32		Damage;

	CBlasterProjectile ();
	CBlasterProjectile (sint32 Index);

	IMPLEMENT_SAVE_HEADER (CBlasterProjectile)

	void SaveFields (CFile &File)
	{
		File.Write<sint32> (Damage);

		CThinkableEntity::SaveFields (File);
		CTouchableEntity::SaveFields (File);
		CFlyMissileProjectile::SaveFields (File);
	}

	void LoadFields (CFile &File)
	{
		Damage = File.Read <sint32>();

		CThinkableEntity::LoadFields (File);
		CTouchableEntity::LoadFields (File);
		CFlyMissileProjectile::LoadFields (File);
	}

	void Think ();
	void Touch (CBaseEntity *other, plane_t *plane, cmBspSurface_t *surf);

	static void Spawn	(CBaseEntity *Spawner, vec3f start, vec3f dir,
						sint32 damage, sint32 speed, sint32 effect, bool isHyper);

	bool Run ();
};

class CRocket : public CFlyMissileProjectile, public CThinkableEntity, public CTouchableEntity
{
public:
	sint32		Damage, RadiusDamage;
	float	DamageRadius;

	CRocket ();
	CRocket (sint32 Index);

	IMPLEMENT_SAVE_HEADER(CRocket)

	void SaveFields (CFile &File)
	{
		File.Write<sint32> (Damage);
		File.Write<sint32> (RadiusDamage);
		File.Write<float> (DamageRadius);

		CThinkableEntity::SaveFields (File);
		CTouchableEntity::SaveFields (File);
		CFlyMissileProjectile::SaveFields (File);
	}

	void LoadFields (CFile &File)
	{
		Damage = File.Read<sint32> ();
		RadiusDamage = File.Read<sint32> ();
		DamageRadius = File.Read<float> ();

		CThinkableEntity::LoadFields (File);
		CTouchableEntity::LoadFields (File);
		CFlyMissileProjectile::LoadFields (File);
	}

	void Think ();
	void Touch (CBaseEntity *other, plane_t *plane, cmBspSurface_t *surf);

	static CRocket *Spawn	(CBaseEntity *Spawner, vec3f start, vec3f dir,
						sint32 damage, sint32 speed, float damage_radius, sint32 radius_damage);

	bool Run ();
};

class CBFGBolt : public CFlyMissileProjectile, public CThinkableEntity, public CTouchableEntity
{
public:
	bool			Exploded;
	sint32			Damage;
	float			DamageRadius;
	FrameNumber_t	FreeTime;

	CBFGBolt ();
	CBFGBolt (sint32 Index);

	IMPLEMENT_SAVE_HEADER(CBFGBolt)

	void SaveFields (CFile &File)
	{
		File.Write<bool> (Exploded);
		File.Write<sint32> (Damage);
		File.Write<float> (DamageRadius);
		File.Write<FrameNumber_t> (FreeTime);

		CThinkableEntity::SaveFields (File);
		CTouchableEntity::SaveFields (File);
		CFlyMissileProjectile::SaveFields (File);
	}

	void LoadFields (CFile &File)
	{
		Exploded = File.Read<bool> ();
		Damage = File.Read<sint32> ();
		DamageRadius = File.Read<float> ();
		FreeTime = File.Read<FrameNumber_t> ();

		CThinkableEntity::LoadFields (File);
		CTouchableEntity::LoadFields (File);
		CFlyMissileProjectile::LoadFields (File);
	}

	void Think ();
	void Touch (CBaseEntity *other, plane_t *plane, cmBspSurface_t *surf);

	static void Spawn	(CBaseEntity *Spawner, vec3f start, vec3f dir,
						sint32 damage, sint32 speed, float damage_radius);

	bool Run ();
};

class CHitScan
{
public:
	bool ThroughAndThrough;
	bool HasEffect;
	sint32 Damage;
	sint32 Kick;

	CHitScan (sint32 damage, sint32 kick, bool HasEffect, bool throughAndThrough) :
	Damage(damage),
	Kick(kick),
	ThroughAndThrough(throughAndThrough) {};

	virtual inline CTrace	DoTrace		(vec3f &start, vec3f &end, CBaseEntity *ignore, sint32 mask);
	virtual inline bool		DoDamage	(CBaseEntity *Attacker, CHurtableEntity *Target, vec3f &dir, vec3f &point, vec3f &normal);
	virtual inline void		DoEffect	(vec3f &start, vec3f &end, bool water);
	virtual inline void		DoSolidHit	(CTrace *Trace);
	virtual inline void		DoWaterHit	(CTrace *Trace);
	virtual bool			ModifyEnd	(vec3f &aimDir, vec3f &start, vec3f &end);

	virtual void			DoFire		(CBaseEntity *Entity, vec3f start, vec3f aimdir);
};

class CRailGunShot : public CHitScan
{
public:
	CRailGunShot (sint32 damage, sint32 kick) :
	CHitScan (damage, kick, true, true) {};

	inline bool		DoDamage (CBaseEntity *Attacker, CHurtableEntity *Target, vec3f &dir, vec3f &point, vec3f &normal);
	inline void		DoEffect (vec3f &start, vec3f &end, bool water);

	static void		Fire		(CBaseEntity *Entity, vec3f start, vec3f aimdir, sint32 damage, sint32 kick);
};

class CBullet : public CHitScan
{
public:
	EMeansOfDeath MeansOfDeath;
	sint32 vSpread, hSpread;

	CBullet (sint32 damage, sint32 kick, sint32 hSpread, sint32 vSpread, sint32 mod) :
	CHitScan (damage, kick, false, false),
	vSpread(vSpread),
	hSpread(hSpread),
	MeansOfDeath(mod) {};

	inline bool				DoDamage (CBaseEntity *Attacker, CHurtableEntity *Target, vec3f &dir, vec3f &point, vec3f &normal);
	virtual inline void		DoSolidHit	(CTrace *Trace);
	inline void				DoWaterHit	(CTrace *Trace);
	bool					ModifyEnd (vec3f &aimDir, vec3f &start, vec3f &end);
	void					DoEffect (vec3f &start, vec3f &end, bool water);
	virtual void			DoFire		(CBaseEntity *Entity, vec3f start, vec3f aimdir);

	static void				Fire		(CBaseEntity *Entity, vec3f start, vec3f aimdir, sint32 damage, sint32 kick, sint32 hSpread, sint32 vSpread, sint32 mod);
};

class CShotgunPellets : public CBullet
{
public:
	CShotgunPellets (sint32 damage, sint32 kick, sint32 hSpread, sint32 vSpread, sint32 mod) :
	CBullet (damage, kick, hSpread, vSpread, mod)
	{
	};

	inline void				DoSolidHit (CTrace *Trace);

	static void				Fire		(CBaseEntity *Entity, vec3f start, vec3f aimdir, sint32 damage, sint32 kick, sint32 hSpread, sint32 vSpread, sint32 Count, sint32 mod);
};

class CMeleeWeapon
{
public:
	CMeleeWeapon();

	static bool		Fire (CBaseEntity *Entity, vec3f aim, sint32 damage, sint32 kick);
};

#if CLEANCTF_ENABLED
// Grappling class
class CGrappleEntity : public CFlyMissileProjectile, public CTouchableEntity
{
	CPlayerEntity		*Player;
	float				Damage;

public:
	CGrappleEntity ();
	CGrappleEntity (sint32 Index);

	ENTITYFIELDS_NONSAVABLE
	IMPLEMENT_SAVE_HEADER(CGrappleEntity)

	void ResetGrapple ();
	void GrapplePull ();
	void GrappleDrawCable ();

	void Touch (CBaseEntity *other, plane_t *plane, cmBspSurface_t *surf);
	bool Run ();

	static void Spawn (CPlayerEntity *Spawner, vec3f start, vec3f dir, sint32 damage, sint32 speed);
};
#endif

void CheckDodge (CBaseEntity *self, vec3f &start, vec3f &dir, sint32 speed);

#else
FILE_WARNING
#endif