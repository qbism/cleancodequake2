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
// cc_brushmodels.h
// 
//

#if !defined(CC_GUARD_BRUSHMODELS_H) || !INCLUDE_GUARDS
#define CC_GUARD_BRUSHMODELS_H

CC_ENUM (uint8, EBrushThinkType)
{
	BRUSHTHINK_NONE,

	BRUSHTHINK_MOVEBEGIN,
	BRUSHTHINK_MOVEFINAL,
	BRUSHTHINK_MOVEDONE,

	BRUSHTHINK_MOVEACCEL,

	BRUSHTHINK_AMOVEBEGIN,
	BRUSHTHINK_AMOVEFINAL,
	BRUSHTHINK_AMOVEDONE,

	BRUSHTHINK_CUSTOM_START,
};

CC_ENUM (uint32, EBrushType)
{
	BRUSH_BASE			= BIT(0),

	BRUSH_DOOR			= BIT(1),
	BRUSH_PLATFORM		= BIT(2),
	BRUSH_TRAIN			= BIT(3),
	BRUSH_ROTATINGDOOR	= BIT(4),
	BRUSH_MOVABLEWATER	= BIT(5),
	BRUSH_SECRETDOOR	= BIT(6),
	BRUSH_BUTTON		= BIT(7),
	BRUSH_ROTATING		= BIT(8),
	BRUSH_CONVEYOR		= BIT(9),
	BRUSH_WALL			= BIT(10),
	BRUSH_OBJECT		= BIT(11),
	BRUSH_EXPLOSIVE		= BIT(12),
};

CC_ENUM (uint8, EMoveState)
{
	STATE_TOP,
	STATE_BOTTOM,
	STATE_UP,
	STATE_DOWN
};

// Contains code common to brush models
class IBrushModel : public virtual IBaseEntity, public IThinkableEntity, public IStopPhysics
{
public:
	EBrushType	BrushType;

	char		*Model;
	float		Accel;
	float		Speed;
	float		Decel;
	sint32		Distance;
	sint32		Damage;
	uint8		Sounds;

	FrameNumber_t		Wait;

	FrameNumber_t		TouchDebounce;

	// fixed data
	vec3f		StartOrigin;
	vec3f		StartAngles;
	vec3f		EndOrigin;
	vec3f		EndAngles;

	MediaIndex	SoundStart;
	MediaIndex	SoundMiddle;
	MediaIndex	SoundEnd;
	vec3f		MoveDir;
	vec3f		Positions[2];
	vec3f		MoveOrigin, MoveAngles;

	sint32		Lip;

	// state data
	EMoveState	MoveState;
	vec3f		Dir;
	float		CurrentSpeed;
	float		AccelMoveSpeed, MoveSpeed, MoveAccel, MoveDecel;
	float		NextSpeed;
	float		RemainingDistance;
	float		DecelDistance;
	uint8		EndFunc;

	EBrushThinkType		ThinkType;

	ENTITYFIELD_VIRTUAL_DEFS
	ENTITYFIELDS_SAVABLE_VIRTUAL(IBrushModel)

	IBrushModel ();
	IBrushModel (sint32 Index);

	void SetBrushModel ();

	inline void SetMoveDir ()
	{
		if (State.GetAngles().Y == -1)
			MoveDir.Set (0, 0, 1);
		else if (State.GetAngles().Y == -2)
			MoveDir.Set (0, 0, -1);
		else
			State.GetAngles().ToVectors (&MoveDir, NULL, NULL);

		State.GetAngles().Clear ();
	}


	virtual void	DoEndFunc () {};

	// Origin/velocity
	void MoveDone ();
	void MoveFinal ();
	void MoveBegin ();
	void MoveCalc (vec3f &dest, uint32 EndFunc);

	// Angle/avelocity
	void AngleMoveDone ();
	void AngleMoveFinal ();
	void AngleMoveBegin ();
	void AngleMoveCalc (uint32 EndFunc);

	// Accel
	inline float AccelerationDistance (const float target, const float rate)
	{
		return (target * ((target / rate) + 1) / 2);
	}

	void CalcAcceleratedMove();
	void Accelerate ();
	void ThinkAccelMove ();

	virtual void Think ();
	bool Run ();
};

class CPlatFormInsideTrigger : public ITouchableEntity
{
public:
	class CPlatForm	*Owner;

	CPlatFormInsideTrigger ();
	CPlatFormInsideTrigger (sint32 Index);

	ENTITYFIELDS_SAVABLE(CPlatFormInsideTrigger)

	virtual void Touch (IBaseEntity *Other, plane_t *plane, cmBspSurface_t *surf);
};

class CPlatForm2InsideTrigger : public CPlatFormInsideTrigger
{
public:
	CPlatForm2InsideTrigger ();
	CPlatForm2InsideTrigger (sint32 Index);

	ENTITYFIELDS_INHERIT(CPlatForm2InsideTrigger)

	void Touch (IBaseEntity *Other, plane_t *plane, cmBspSurface_t *surf);
};

CC_ENUM (uint8, EPlatformSpawnflags)
{
	PLAT_LOW_TRIGGER	= BIT(0),
};

class CPlatForm : public IMapEntity, public IBrushModel, public IBlockableEntity, public IUsableEntity
{
public:
	sint32					Height;
	CPlatFormInsideTrigger	*InsideTrigger;

	enum
	{
		PLATTHINK_GO_DOWN = BRUSHTHINK_CUSTOM_START,
		PLATTHINK_GO_UP
	};

	CPlatForm();
	CPlatForm(sint32 Index);

	bool Run ();
	virtual void Blocked (IBaseEntity *Other);
	virtual void Use (IBaseEntity *Other, IBaseEntity *Activator);
	virtual void HitTop ();
	virtual void HitBottom ();

	ENTITYFIELD_DEFS
	ENTITYFIELDS_SAVABLE(CPlatForm)

	enum
	{
		PLATENDFUNC_HITBOTTOM,
		PLATENDFUNC_HITTOP
	};

	virtual void DoEndFunc ();
	virtual void GoDown ();
	virtual void GoUp ();
	virtual void Think ();

	void SpawnInsideTrigger ();
	virtual void Spawn ();
};

#if ROGUE_FEATURES

class CPlatForm2 : public CPlatForm
{
public:
	CC_ENUM (uint8, EPlat2Flags)
	{
		PLAT2_CALLED	= BIT(0),
		PLAT2_MOVING	= BIT(1),
		PLAT2_WAITING	= BIT(2)
	};

	bool			RequiresActivation;
	FrameNumber_t	LastMoveTime;
	EPlat2Flags		PlatFlags;
	class CBadArea	*BadArea;

	CPlatForm2();
	CPlatForm2(sint32 Index);

	void Blocked (IBaseEntity *Other);
	void Use (IBaseEntity *Other, IBaseEntity *Activator);
	void HitTop ();
	void HitBottom ();

	ENTITYFIELDS_SAVABLE(CPlatForm2)

	void DoEndFunc ();
	void GoDown ();
	void GoUp ();
	void Operate (IBaseEntity *Other);
	void Think ();
	void SpawnDangerArea ();
	void KillDangerArea ();

	CPlatFormInsideTrigger *SpawnInsideTrigger ();
	void Spawn ();
};
#endif

class CDoorTrigger : public ITouchableEntity
{
public:
	FrameNumber_t		TouchDebounce;

	CDoorTrigger ();
	CDoorTrigger (sint32 Index);

	ENTITYFIELDS_SAVABLE(CDoorTrigger)

	void Touch (IBaseEntity *Other, plane_t *plane, cmBspSurface_t *surf);
};

// Base door class
class CDoor : public IMapEntity, public IBrushModel, public IHurtableEntity, public IBlockableEntity, public ITouchableEntity, public IUsableEntity
{
public:
#if ROGUE_FEATURES
	CC_ENUM (uint8, EDoorUse)
	{
		DOORUSE_NONE,
		DOORUSE_NORMAL,
		DOORUSE_ACTIVATE
	};
	EDoorUse	UseType;
#endif

	enum
	{
		DOORTHINK_SPAWNDOORTRIGGER = BRUSHTHINK_CUSTOM_START,
		DOORTHINK_CALCMOVESPEED,

		DOORTHINK_GODOWN,

#if ROGUE_FEATURES
		DOORTHINK_SMARTWATER_GOUP,
#endif
	};

	CDoor();
	CDoor(sint32 Index);

	IMPLEMENT_SAVE_HEADER(CDoor)

	void UseAreaPortals (bool isOpen);
	bool Run ();

	virtual bool			ParseField (const char *Key, const char *Value)
	{
		return (IBrushModel::ParseField (Key, Value) || IUsableEntity::ParseField (Key, Value) || IHurtableEntity::ParseField (Key, Value) || IMapEntity::ParseField (Key, Value));
	};

	virtual void SaveFields (CFile &File)
	{
		IMapEntity::SaveFields (File);
		IBrushModel::SaveFields (File);
		IUsableEntity::SaveFields (File);
		IHurtableEntity::SaveFields (File);
		ITouchableEntity::SaveFields (File);
	}

	virtual void LoadFields (CFile &File)
	{
		IMapEntity::LoadFields (File);
		IBrushModel::LoadFields (File);
		IUsableEntity::LoadFields (File);
		IHurtableEntity::LoadFields (File);
		ITouchableEntity::LoadFields (File);
	}

	enum
	{
		DOORENDFUNC_HITBOTTOM,
		DOORENDFUNC_HITTOP
	};

	virtual void HitTop ();
	virtual void HitBottom ();
	virtual void GoDown ();
	virtual void GoUp (IBaseEntity *Activator);
	virtual void DoEndFunc ();
	virtual void Think ();

#if ROGUE_FEATURES
	void SmartWaterGoUp ();
	void Activate ();
#endif

	virtual void Blocked (IBaseEntity *Other);
	virtual void Use (IBaseEntity *Other, IBaseEntity *Activator);
	virtual void Die (IBaseEntity *Inflictor, IBaseEntity *Attacker, sint32 Damage, vec3f &point);
	virtual void Pain (IBaseEntity *Other, sint32 Damage);
	virtual void Touch (IBaseEntity *Other, plane_t *plane, cmBspSurface_t *surf);

	// Thinks
	void SpawnDoorTrigger ();
	void CalcMoveSpeed ();

	virtual void Spawn ();
};

class CRotatingDoor : public CDoor
{
public:
	CRotatingDoor ();
	CRotatingDoor (sint32 Index);

	IMPLEMENT_SAVE_HEADER(CRotatingDoor)

	void GoDown();
	void GoUp (IBaseEntity *Activator);

	void Spawn ();
};

class CMovableWater : public CDoor
{
public:
	CMovableWater ();
	CMovableWater (sint32 Index);

	IMPLEMENT_SAVE_HEADER(CMovableWater)

	void Spawn ();
};

class CDoorSecret : public CDoor
{
public:
	enum
	{
		DOORSECRETTHINK_6 = BRUSHTHINK_CUSTOM_START,
		DOORSECRETTHINK_4,
		DOORSECRETTHINK_2
	};

	enum
	{
		DOORSECRETENDFUNC_DONE,
		DOORSECRETENDFUNC_5,
		DOORSECRETENDFUNC_3,
		DOORSECRETENDFUNC_1
	};

	CDoorSecret ();
	CDoorSecret (sint32 Index);

	IMPLEMENT_SAVE_HEADER(CDoorSecret)

	void Blocked (IBaseEntity *Other);
	void Use (IBaseEntity *Other, IBaseEntity *Activator);
	void Die (IBaseEntity *Inflictor, IBaseEntity *Attacker, sint32 Damage, vec3f &point);

	void DoEndFunc ();
	void Think ();

	void Spawn ();
};

class CButton : public IMapEntity, public IBrushModel, public IHurtableEntity, public ITouchableEntity, public IUsableEntity
{
public:
	enum
	{
		BUTTONTHINK_RETURN = BRUSHTHINK_CUSTOM_START,
	};

	CButton();
	CButton(sint32 Index);

	IMPLEMENT_SAVE_HEADER(CButton)

	bool Run ();

	virtual bool			ParseField (const char *Key, const char *Value)
	{
		return (IBrushModel::ParseField (Key, Value) || IUsableEntity::ParseField (Key, Value) || IHurtableEntity::ParseField (Key, Value) || IMapEntity::ParseField (Key, Value));
	};

	virtual void SaveFields (CFile &File)
	{
		IMapEntity::SaveFields (File);
		IBrushModel::SaveFields (File);
		IUsableEntity::SaveFields (File);
		IHurtableEntity::SaveFields (File);
		ITouchableEntity::SaveFields (File);
	}

	virtual void LoadFields (CFile &File)
	{
		IMapEntity::LoadFields (File);
		IBrushModel::LoadFields (File);
		IUsableEntity::LoadFields (File);
		IHurtableEntity::LoadFields (File);
		ITouchableEntity::LoadFields (File);
	}

	enum
	{
		BUTTONENDFUNC_WAIT,
		BUTTONENDFUNC_DONE
	};

	virtual void DoEndFunc ();
	virtual void Think ();
	virtual void Fire ();

	virtual void Use (IBaseEntity *Other, IBaseEntity *Activator);
	virtual void Die (IBaseEntity *Inflictor, IBaseEntity *Attacker, sint32 Damage, vec3f &point);
	virtual void Pain (IBaseEntity *Other, sint32 Damage);
	virtual void Touch (IBaseEntity *Other, plane_t *plane, cmBspSurface_t *surf);

	virtual void Spawn ();
};

class CTrainBase : public IMapEntity, public IBrushModel, public IBlockableEntity, public IUsableEntity
{
public:
	class CPathCorner	*TargetEntity;

	enum
	{
		TRAINTHINK_FIND = BRUSHTHINK_CUSTOM_START,
		TRAINTHINK_NEXT,
	};
	CTrainBase();
	CTrainBase(sint32 Index);

	IMPLEMENT_SAVE_HEADER(CTrainBase)

	virtual bool Run ();
	virtual bool ParseField (const char *Key, const char *Value);

	virtual void SaveFields (CFile &File);
	virtual void LoadFields (CFile &File);

	enum
	{
		TRAINENDFUNC_NONE,
		TRAINENDFUNC_WAIT,
	};

	void TrainWait ();
	void Next ();
	void Resume ();
	void Find ();

	virtual void DoEndFunc ();
	virtual void Think ();

	virtual void Blocked (IBaseEntity *Other);
	virtual void Use (IBaseEntity *Other, IBaseEntity *Activator);

	virtual void Spawn ();
};

class CTrain : public CTrainBase
{
public:
	CTrain ();
	CTrain (sint32 Index);

	IMPLEMENT_SAVE_HEADER(CTrain)

	void Spawn ();
};

class CTriggerElevator : public IMapEntity, public IThinkableEntity, public IUsableEntity
{
public:
	CTrain			*MoveTarget;

	CTriggerElevator ();
	CTriggerElevator (sint32 Index);

	IMPLEMENT_SAVE_HEADER(CTriggerElevator)

	bool ParseField (const char *Key, const char *Value)
	{
		return (IUsableEntity::ParseField (Key, Value) || IMapEntity::ParseField (Key, Value));
	}

	void SaveFields (CFile &File)
	{
		File.Write<sint32> ((MoveTarget) ? MoveTarget->State.GetNumber() : -1);
		IMapEntity::SaveFields (File);
		IUsableEntity::SaveFields (File);
		IThinkableEntity::SaveFields (File);
	}

	void LoadFields (CFile &File)
	{
		sint32 Index = File.Read<sint32> ();
		if (Index != -1)
			MoveTarget = entity_cast<CTrain>(Game.Entities[Index].Entity);

		IMapEntity::LoadFields (File);
		IUsableEntity::LoadFields (File);
		IThinkableEntity::LoadFields (File);
	}

	void Think ();
	void Use (IBaseEntity *Other, IBaseEntity *Activator);

	void Spawn ();
};

class CWorldEntity : public IMapEntity, public IBrushModel
{
public:
	char	*Message;
	char	*Gravity, *Sky, *NextMap;
	vec3f	SkyAxis;
	float	SkyRotate;

	CWorldEntity ();
	CWorldEntity (sint32 Index);

	ENTITYFIELD_DEFS
	IMPLEMENT_SAVE_HEADER(CWorldEntity)

	void SaveFields (CFile &File);
	void LoadFields (CFile &File);

	bool Run ();
	void Spawn ();
};

class CRotatingBrush : public IMapEntity, public IBrushModel, public IUsableEntity, public IBlockableEntity, public ITouchableEntity
{
public:
	bool Blockable;

	CRotatingBrush ();
	CRotatingBrush (sint32 Index);

	IMPLEMENT_SAVE_HEADER(CRotatingBrush)

	void Use (IBaseEntity *Other, IBaseEntity *Activator);
	void Blocked (IBaseEntity *Other);
	void Touch (IBaseEntity *Other, plane_t *plane, cmBspSurface_t *surf);

	virtual bool ParseField (const char *Key, const char *Value)
	{
		return (IBrushModel::ParseField (Key, Value) || IUsableEntity::ParseField (Key, Value) || IMapEntity::ParseField (Key, Value));
	}

	void SaveFields (CFile &File)
	{
		IMapEntity::SaveFields (File);
		IBrushModel::SaveFields (File);
		IUsableEntity::SaveFields (File);
		ITouchableEntity::SaveFields (File);
	}

	void LoadFields (CFile &File)
	{
		IMapEntity::LoadFields (File);
		IBrushModel::LoadFields (File);
		IUsableEntity::LoadFields (File);
		ITouchableEntity::LoadFields (File);
	}

#if ROGUE_FEATURES
	CC_ENUM (uint8, ERotatingBrushThinkType)
	{
		ROTATINGTHINK_ACCEL,
		ROTATINGTHINK_DECEL
	};

	void Accelerate ();
	void Decelerate ();
	void Think ();
#endif

	bool Run ();
	void Spawn ();
};

class CConveyor : public IMapEntity, public IBrushModel, public IUsableEntity
{
public:
	sint32			SavedSpeed;

	CConveyor ();
	CConveyor (sint32 Index);

	ENTITYFIELD_DEFS
	ENTITYFIELDS_SAVABLE(CConveyor)

	void Use (IBaseEntity *Other, IBaseEntity *Activator);

	bool Run ();
	void Spawn ();
};

class CAreaPortal : public IMapEntity, public IUsableEntity
{
public:
	bool		PortalState;
	uint8		Style;

	CAreaPortal ();
	CAreaPortal (sint32 Index);

	ENTITYFIELD_DEFS
	ENTITYFIELDS_SAVABLE(CAreaPortal)

	void Use (IBaseEntity *Other, IBaseEntity *Activator);

	bool Run ();
	void Spawn ();
};

class CFuncWall : public IMapEntity, public IBrushModel, public IUsableEntity
{
public:
	CFuncWall ();
	CFuncWall (sint32 Index);

	IMPLEMENT_SAVE_HEADER(CFuncWall)

	virtual bool ParseField (const char *Key, const char *Value)
	{
		return (IBrushModel::ParseField (Key, Value) || IUsableEntity::ParseField (Key, Value) || IMapEntity::ParseField (Key, Value));
	}

	virtual void SaveFields (CFile &File)
	{
		IMapEntity::SaveFields (File);
		IBrushModel::SaveFields (File);
		IUsableEntity::SaveFields (File);
	}

	virtual void LoadFields (CFile &File)
	{
		IMapEntity::LoadFields (File);
		IBrushModel::LoadFields (File);
		IUsableEntity::LoadFields (File);
	}

	void Use (IBaseEntity *Other, IBaseEntity *Activator);

	bool Run ();
	void Spawn ();
};

class CFuncObject : public IMapEntity, public IBrushModel, public ITossProjectile, public ITouchableEntity, public IUsableEntity
{
public:
	CFuncObject ();
	CFuncObject (sint32 Index);

	IMPLEMENT_SAVE_HEADER(CFuncObject)

	void Think ();
	void Use (IBaseEntity *Other, IBaseEntity *Activator);
	void Touch (IBaseEntity *Other, plane_t *plane, cmBspSurface_t *surf);

	virtual bool ParseField (const char *Key, const char *Value)
	{
		return (IBrushModel::ParseField (Key, Value) || IUsableEntity::ParseField (Key, Value) || IMapEntity::ParseField (Key, Value));
	}

	virtual void SaveFields (CFile &File)
	{
		IMapEntity::SaveFields (File);
		IBrushModel::SaveFields (File);
		IUsableEntity::SaveFields (File);
		ITouchableEntity::SaveFields (File);
		ITossProjectile::SaveFields (File);
	}

	virtual void LoadFields (CFile &File)
	{
		IMapEntity::LoadFields (File);
		IBrushModel::LoadFields (File);
		IUsableEntity::LoadFields (File);
		ITouchableEntity::LoadFields (File);
		ITossProjectile::LoadFields (File);
	}

	bool Run ();
	void Spawn ();
};

class CFuncExplosive : public IMapEntity, public IBrushModel, public IUsableEntity, public IHurtableEntity
{
public:
	CC_ENUM (uint8, EFuncExplosiveUseType)
	{
		FUNCEXPLOSIVE_USE_NONE,
		FUNCEXPLOSIVE_USE_SPAWN,
		FUNCEXPLOSIVE_USE_EXPLODE,
#if ROGUE_FEATURES
		FUNCEXPLOSIVE_USE_ACTIVATE,
#endif
	};

	EFuncExplosiveUseType	UseType;
	sint32					Explosivity;

	ENTITYFIELD_DEFS
	ENTITYFIELDS_SAVABLE(CFuncExplosive)

	CFuncExplosive ();
	CFuncExplosive (sint32 Index);

	void DoSpawn ();

	void Use (IBaseEntity *Other, IBaseEntity *Activator);
#if ROGUE_FEATURES
	void Activate (IBaseEntity *Other, IBaseEntity *Activator);
#endif
	void Pain (IBaseEntity *Other, sint32 Damage);
	void Die (IBaseEntity *Inflictor, IBaseEntity *Attacker, sint32 Damage, vec3f &point);

	bool Run ();
	void Spawn ();
};

class CKillbox : public IMapEntity, public IUsableEntity, public IBrushModel
{
public:
	CKillbox ();
	CKillbox (sint32 Index);

	IMPLEMENT_SAVE_HEADER(CKillbox)

	bool ParseField (const char *Key, const char *Value)
	{
		return (IBrushModel::ParseField (Key, Value) || IUsableEntity::ParseField (Key, Value) || IMapEntity::ParseField (Key, Value));
	}

	void SaveFields (CFile &File)
	{
		IMapEntity::SaveFields (File);
		IUsableEntity::SaveFields (File);
		IBrushModel::SaveFields (File);
	}

	void LoadFields (CFile &File)
	{
		IMapEntity::LoadFields (File);
		IUsableEntity::LoadFields (File);
		IBrushModel::LoadFields (File);
	}

	void Use (IBaseEntity *Other, IBaseEntity *Activator);

	bool Run ()
	{
		return IBaseEntity::Run ();
	};

	void Spawn ();
};

#else
FILE_WARNING
#endif
