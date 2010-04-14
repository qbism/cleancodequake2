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
// cc_rogue_weaponry.h
// 
//

#if !defined(CC_GUARD_CC_ROGUE_WEAPONRY_H) || !INCLUDE_GUARDS
#define CC_GUARD_CC_ROGUE_WEAPONRY_H

class CHeatBeam : public CHitScan
{
public:
	EMeansOfDeath		MeansOfDeath;
	bool				MonsterBeam;
	CBaseEntity			*Firer;

	CHeatBeam (CBaseEntity *Firer, sint32 Damage, sint32 Kick, sint32 mod, bool MonsterBeam) :
	  CHitScan (Damage, Kick, false),
	  MeansOfDeath(mod),
	  Firer(Firer),
	  MonsterBeam(MonsterBeam)
	  {
	  };

	bool					DoDamage (CBaseEntity *Attacker, CHurtableEntity *Target, vec3f &dir, vec3f &point, vec3f &normal);
	void					DoSolidHit	(CTrace *Trace);
	void					DoWaterHit	(CTrace *Trace);
	void					DoEffect (vec3f &start, vec3f &end, bool water);
	void					DoFire		(CBaseEntity *Entity, vec3f start, vec3f aimdir);

	static void				Fire		(CBaseEntity *Entity, vec3f start, vec3f aimdir, sint32 Damage, sint32 kick, sint32 mod, bool Monster);
};

class CFlechette : public CFlyMissileProjectile, public CThinkableEntity, public CTouchableEntity
{
public:
	sint32		Damage, Kick;

	CFlechette ();
	CFlechette (sint32 Index);

	IMPLEMENT_SAVE_HEADER (CBlasterProjectile)

	void SaveFields (CFile &File)
	{
		File.Write<sint32> (Damage);
		File.Write<sint32> (Kick);

		CThinkableEntity::SaveFields (File);
		CTouchableEntity::SaveFields (File);
		CFlyMissileProjectile::SaveFields (File);
	}

	void LoadFields (CFile &File)
	{
		Damage = File.Read <sint32>();
		Kick = File.Read <sint32>();

		CThinkableEntity::LoadFields (File);
		CTouchableEntity::LoadFields (File);
		CFlyMissileProjectile::LoadFields (File);
	}

	void Think ();
	void Touch (CBaseEntity *Other, plane_t *plane, cmBspSurface_t *surf);

	static void Spawn	(CBaseEntity *Spawner, vec3f Start, vec3f Dir,
						sint32 Damage, sint32 Kick, sint32 Speed);

	bool Run ();
};

class CDisruptorTracker : public CFlyMissileProjectile, public CThinkableEntity, public CTouchableEntity
{
public:
	sint32		Damage;
	sint32		Speed;
	bool		DoFree;

	CDisruptorTracker ();
	CDisruptorTracker (sint32 Index);

	IMPLEMENT_SAVE_HEADER (CDisruptorTracker)

	void SaveFields (CFile &File)
	{
		File.Write<sint32> (Damage);
		File.Write<sint32> (Speed);
		File.Write<bool> (DoFree);

		CThinkableEntity::SaveFields (File);
		CTouchableEntity::SaveFields (File);
		CFlyMissileProjectile::SaveFields (File);
	}

	void LoadFields (CFile &File)
	{
		Damage = File.Read<sint32>();
		Speed = File.Read<sint32>();
		DoFree = File.Read<bool>();

		CThinkableEntity::LoadFields (File);
		CTouchableEntity::LoadFields (File);
		CFlyMissileProjectile::LoadFields (File);
	}

	void Think ();
	void Touch (CBaseEntity *Other, plane_t *plane, cmBspSurface_t *surf);

	void Explode (plane_t *plane);

	static void Spawn	(CBaseEntity *Spawner, vec3f start, vec3f dir,
						sint32 Damage, sint32 speed, CBaseEntity *enemy);

	bool Run ();
};

class CGreenBlasterProjectile : public CFlyMissileProjectile, public CThinkableEntity, public CTouchableEntity
{
public:
	sint32		Damage;

	CGreenBlasterProjectile ();
	CGreenBlasterProjectile (sint32 Index);

	IMPLEMENT_SAVE_HEADER (CGreenBlasterProjectile)

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
	void Touch (CBaseEntity *Other, plane_t *plane, cmBspSurface_t *surf);

	static void Spawn	(CBaseEntity *Spawner, vec3f start, vec3f dir,
						sint32 Damage, sint32 speed, sint32 effect);

	bool Run ();
};

class CBadArea
{
public:
	vec3f			AbsMin, AbsMax, Origin, Mins, Maxs;
	CBaseEntity		*Owner;
	FrameNumber_t	Lifespan;
	bool			Remove;

	CBadArea (vec3f AbsMin, vec3f AbsMax, FrameNumber_t Lifespan, CBaseEntity *Owner);
	CBadArea () : Remove(false) { };

	void Save (CFile &File)
	{
		File.Write<vec3f> (AbsMin);
		File.Write<vec3f> (AbsMax);
		File.Write<vec3f> (Origin);
		File.Write<vec3f> (Mins);
		File.Write<vec3f> (Maxs);
		WriteEntity (File, Owner);
		File.Write<FrameNumber_t> (Lifespan);
	}

	CBadArea (CFile &File)
	{
		AbsMin = File.Read<vec3f> ();
		AbsMax = File.Read<vec3f> ();
		Origin = File.Read<vec3f> ();
		Mins = File.Read<vec3f> ();
		Maxs = File.Read<vec3f> ();
		Owner = ReadEntity (File);
		Lifespan = File.Read<FrameNumber_t> ();
	}

	void Run ()
	{
		if (Remove)
			return;

		if (!Owner || !Owner->GetInUse() || Owner->Freed || (Lifespan != -1 && Lifespan < Level.Frame))
		{
			Remove = true;
			Owner = NULL;
		}
	}
};

class CTesla : public CBounceProjectile, public CHurtableEntity, public CTouchableEntity, public CThinkableEntity
{
public:
	CC_ENUM (uint8, ETeslaThinkType)
	{
		TESLATHINK_NONE,

		TESLATHINK_ACTIVE,
		TESLATHINK_DONEACTIVATE,
		TESLATHINK_ACTIVATE,
	};

	ETeslaThinkType		ThinkType;
	CPlayerEntity		*Firer;
	bool				DoExplosion;
	int					Damage;
	FrameNumber_t		RemoveTime;
	CBadArea			*BadArea;

	CTesla ();
	CTesla (sint32 Index);

	void SaveFields (CFile &File);
	void LoadFields (CFile &File);

	IMPLEMENT_SAVE_HEADER(CTesla);

	bool Run ();
	void Die (CBaseEntity *Inflictor, CBaseEntity *Attacker, sint32 Damage, vec3f &point);

	void Remove ();
	void Explode ();

	void Blow ();
	void Active ();
	void DoneActivate ();
	void Activate ();

	void Touch (CBaseEntity *Other, plane_t *plane, cmBspSurface_t *surf);

	void Think ();

	static void Spawn (CPlayerEntity *Player, vec3f Start, vec3f AimDir, int DamageMultiplier, int Speed);
};

#else
FILE_WARNING
#endif