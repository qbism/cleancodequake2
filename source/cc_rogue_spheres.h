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
// cc_rogue_spheres.h
// 
//

#if !defined(CC_GUARD_CC_ROGUE_SPHERES_H) || !INCLUDE_GUARDS
#define CC_GUARD_CC_ROGUE_SPHERES_H

CC_ENUM (uint8, ESphereType)
{
	SPHERE_DEFENDER,
	SPHERE_HUNTER,
	SPHERE_VENGEANCE
};

CC_ENUM (uint8, ESphereFlags)
{
	SPHERE_DOPPLEGANGER		= BIT(0),
};

class CRogueBaseSphere : public IFlyMissileProjectile, public IHurtableEntity, public ITouchableEntity, public IThinkableEntity
{
public:
	FrameNumber_t		Wait;
	ESphereType			SphereType;
	ESphereFlags		SphereFlags;
	CPlayerEntity		*OwnedPlayer;
	IHurtableEntity		*SphereEnemy;
	vec3f				SavedGoal;

	CRogueBaseSphere () :
	  IHurtableEntity(),
	  ITouchableEntity(),
	  IThinkableEntity(),
	  IFlyMissileProjectile()
	  {
	  };

	CRogueBaseSphere (sint32 Index) :
	  IBaseEntity(Index),
	  IHurtableEntity(Index),
	  ITouchableEntity(Index),
	  IThinkableEntity(Index),
	  IFlyMissileProjectile(Index)
	  {
	  };

	virtual void SaveFields (CFile &File);
	virtual void LoadFields (CFile &File);
	IMPLEMENT_SAVE_HEADER(CRogueBaseSphere);

	bool Run ()
	{
		return ((PhysicsType == PHYSICS_FLYMISSILE) ? IFlyMissileProjectile::Run() : false);
	}

	virtual void 	Pain (IBaseEntity *Other, sint32 Damage) = 0;
	virtual void 	Die (IBaseEntity *Inflictor, IBaseEntity *Attacker, sint32 Damage, vec3f &point);

	virtual void	Touch (IBaseEntity *Other, plane_t *plane, cmBspSurface_t *surf) {};

	virtual void	Think () = 0;

	void			BaseTouch (IBaseEntity *Other, plane_t *plane, cmBspSurface_t *surf, EMeansOfDeath Mod);
	void			Explode ();
	void			Fly ();
	void			Chase (bool stupidChase);

	template <class TType>
	static TType	*CreateBaseSphere (IBaseEntity *Owner, ESphereType Type, ESphereFlags Flags)
	{
		TType *Sphere = QNewEntityOf TType;

		Sphere->State.GetOrigin() = Owner->State.GetOrigin() + vec3f(0, 0, Owner->GetMaxs().Z);
		Sphere->State.GetAngles().Y = Owner->State.GetAngles().Y;
		Sphere->GetSolid() = SOLID_BBOX;
		Sphere->GetClipmask() = CONTENTS_MASK_SHOT;
		Sphere->State.GetRenderEffects() = RF_FULLBRIGHT | RF_IR_VISIBLE;

		if (Flags & SPHERE_DOPPLEGANGER)
			Sphere->Team.Master = Owner->Team.Master;
		else
		{
			Sphere->SetOwner (Owner);
			Sphere->OwnedPlayer = entity_cast<CPlayerEntity>(Owner);
			Sphere->OwnSphere (Sphere->OwnedPlayer);
		}

		Sphere->ClassName = "sphere";
		Sphere->SphereType = Type;
		Sphere->SphereFlags = Flags;
		Sphere->CanTakeDamage = true;

		Sphere->NextThink = Level.Frame + FRAMETIME;

		Sphere->Link ();

		return Sphere;
	}

	void		OwnSphere (CPlayerEntity *Player);
};

class CRogueDefenderSphere : public CRogueBaseSphere
{
public:
	FrameNumber_t		AttackFinished;

	CRogueDefenderSphere () :
	  CRogueBaseSphere ()
	  {
	  };

	CRogueDefenderSphere (sint32 Index) :
	  IBaseEntity (Index),
	  CRogueBaseSphere (Index)
	  {
	  };

	virtual void SaveFields (CFile &File)
	{
		CRogueBaseSphere::SaveFields (File);

		File.Write<FrameNumber_t> (AttackFinished);
	}

	virtual void LoadFields (CFile &File)
	{
		CRogueBaseSphere::LoadFields (File);

		AttackFinished = File.Read<FrameNumber_t> ();
	}

	IMPLEMENT_SAVE_HEADER(CRogueDefenderSphere);

	void 			Pain (IBaseEntity *Other, sint32 Damage);
	void			Think ();

	void			Shoot (IHurtableEntity *At);

	static void		Create (IBaseEntity *Owner, ESphereFlags Flags);
};

class CRogueHunterSphere : public CRogueBaseSphere
{
public:
	CRogueHunterSphere () :
	  CRogueBaseSphere ()
	  {
	  };

	CRogueHunterSphere (sint32 Index) :
	  IBaseEntity (Index),
	  CRogueBaseSphere (Index)
	  {
	  };

	virtual void SaveFields (CFile &File)
	{
		CRogueBaseSphere::SaveFields (File);
	}

	virtual void LoadFields (CFile &File)
	{
		CRogueBaseSphere::LoadFields (File);
	}

	IMPLEMENT_SAVE_HEADER(CRogueVengeanceSphere);

	void 			Pain (IBaseEntity *Other, sint32 Damage);
	void			Think ();
	void			Touch (IBaseEntity *Other, plane_t *plane, cmBspSurface_t *surf);

	void			ChangeYaw (float IdealYaw);
	static void		Create (IBaseEntity *Owner, ESphereFlags Flags);
};

class CRogueVengeanceSphere : public CRogueBaseSphere
{
public:
	CRogueVengeanceSphere () :
	  CRogueBaseSphere ()
	  {
	  };

	CRogueVengeanceSphere (sint32 Index) :
	  IBaseEntity (Index),
	  CRogueBaseSphere (Index)
	  {
	  };

	virtual void SaveFields (CFile &File)
	{
		CRogueBaseSphere::SaveFields (File);
	}

	virtual void LoadFields (CFile &File)
	{
		CRogueBaseSphere::LoadFields (File);
	}

	IMPLEMENT_SAVE_HEADER(CRogueVengeanceSphere);

	void 			Pain (IBaseEntity *Other, sint32 Damage);
	void			Think ();
	void			Touch (IBaseEntity *Other, plane_t *plane, cmBspSurface_t *surf);

	static void		Create (IBaseEntity *Owner, ESphereFlags Flags);
};

#else
FILE_WARNING
#endif