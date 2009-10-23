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
// cc_brushmodels.cpp
// 
//

#include "cc_local.h"
#include "cc_brushmodels.h"

#pragma region Brush_Model
void CBrushModel::Think ()
{
	switch (ThinkType)
	{
	case BRUSHTHINK_NONE:
		break;
		
	case BRUSHTHINK_MOVEBEGIN:
		MoveBegin ();
		break;
	case BRUSHTHINK_MOVEFINAL:
		MoveFinal ();
		break;
	case BRUSHTHINK_MOVEDONE:
		MoveDone ();
		break;

	case BRUSHTHINK_MOVEACCEL:
		ThinkAccelMove ();
		break;
	
	case BRUSHTHINK_AMOVEBEGIN:
		AngleMoveBegin ();
		break;
	case BRUSHTHINK_AMOVEFINAL:
		AngleMoveFinal ();
		break;
	case BRUSHTHINK_AMOVEDONE:
		AngleMoveDone ();
		break;
	};
};

bool CBrushModel::Run ()
{
	return (PhysicsType == PHYSICS_STOP) ? CStopPhysics::Run() : CPushPhysics::Run();
}

CBrushModel::CBrushModel () :
CBaseEntity (),
CThinkableEntity (),
CPushPhysics (),
CStopPhysics (),
BrushType(0)
{
	EntityFlags |= ENT_BRUSHMODEL;
	BrushType |= BRUSH_BASE;
};

CBrushModel::CBrushModel (int Index) :
CBaseEntity (Index),
CThinkableEntity (Index),
CPushPhysics (Index),
CStopPhysics (Index),
BrushType(0)
{
	EntityFlags |= ENT_BRUSHMODEL;
	BrushType |= BRUSH_BASE;
};

ENTITYFIELDS_BEGIN(CBrushModel)
{
	CEntityField ("wait", EntityMemberOffset(CBrushModel,Wait), FT_FRAMENUMBER),
	CEntityField ("speed", EntityMemberOffset(CBrushModel,Speed), FT_FLOAT),
	CEntityField ("accel", EntityMemberOffset(CBrushModel,Accel), FT_FLOAT),
	CEntityField ("decel", EntityMemberOffset(CBrushModel,Decel), FT_FLOAT),
	CEntityField ("distance", EntityMemberOffset(CBrushModel,Distance), FT_INT),
	CEntityField ("dmg", EntityMemberOffset(CBrushModel,Damage), FT_INT),
	CEntityField ("lip", EntityMemberOffset(CBrushModel,Lip), FT_INT),
};
ENTITYFIELDS_END(CBrushModel)

bool			CBrushModel::ParseField (const char *Key, const char *Value)
{
	if (CheckFields<CBrushModel> (this, Key, Value))
		return true;

	// Couldn't find it here
	return false;
};

void CBrushModel::MoveDone ()
{
	Velocity.Clear ();
	DoEndFunc ();
}

void CBrushModel::MoveFinal ()
{
	if (RemainingDistance == 0)
	{
		MoveDone ();
		return;
	}

	Velocity = Dir * RemainingDistance;

	ThinkType = BRUSHTHINK_MOVEDONE;
	NextThink = level.Frame + FRAMETIME;
}

void CBrushModel::MoveBegin ()
{
	if ((Speed * 0.1f) >= RemainingDistance)
	{
		MoveFinal ();
		return;
	}
	Velocity = (Dir * Speed) / 10;

	float frames = floor((RemainingDistance / Speed) / 0.1f);
	RemainingDistance -= (frames * Speed / 10);
	NextThink = level.Frame + frames;
	ThinkType = BRUSHTHINK_MOVEFINAL;
}

void CBrushModel::MoveCalc (vec3f &dest, uint32 EndFunc)
{
	Velocity.Clear ();
	Dir = dest - State.GetOrigin();
	RemainingDistance = Dir.Normalize();
	this->EndFunc = EndFunc;

	if (Speed == Accel && Speed == Decel)
	{
		if (level.CurrentEntity == ((Flags & FL_TEAMSLAVE) ? TeamMaster : this))
			MoveBegin ();
		else
		{
			NextThink = level.Frame + FRAMETIME;
			ThinkType = BRUSHTHINK_MOVEBEGIN;
		}
	}
	else
	{
		// accelerative
		CurrentSpeed = 0;
		ThinkType = BRUSHTHINK_MOVEACCEL;
		NextThink = level.Frame + FRAMETIME;
	}
}


//
// Support routines for angular movement (changes in angle using avelocity)
//

void CBrushModel::AngleMoveDone ()
{
	AngularVelocity.Clear ();
	DoEndFunc ();
}

void CBrushModel::AngleMoveFinal ()
{
	vec3f move = ((MoveState == STATE_UP) ? EndAngles : StartAngles) - State.GetAngles();

	if (move == vec3fOrigin)
	{
		AngleMoveDone ();
		return;
	}

	AngularVelocity = move;

	ThinkType = BRUSHTHINK_AMOVEDONE;
	NextThink = level.Frame + FRAMETIME;
}

void CBrushModel::AngleMoveBegin ()
{
	// set destdelta to the vector needed to move
	vec3f	destdelta = ((MoveState == STATE_UP) ? EndAngles : StartAngles) - State.GetAngles();
	
	// calculate length of vector
	float len = destdelta.Length();
	
	// divide by speed to get time to reach dest
	float traveltime = len / Speed;

	if (traveltime < 0.1f)
	{
		AngleMoveFinal ();
		return;
	}

	float frames = floor(traveltime / 0.1f);

	// scale the destdelta vector by the time spent traveling to get velocity
	//Vec3Scale (destdelta, 1.0 / (traveltime * 10), gameEntity->avelocity);
	AngularVelocity = destdelta * 1.0 / (traveltime * 10);

	// set nextthink to trigger a think when dest is reached
	NextThink = level.Frame + frames;
	ThinkType = BRUSHTHINK_AMOVEFINAL;
}

void CBrushModel::AngleMoveCalc (uint32 EndFunc)
{
	AngularVelocity.Clear ();
	this->EndFunc = EndFunc;
	if (level.CurrentEntity == ((Flags & FL_TEAMSLAVE) ? TeamMaster : this))
		AngleMoveBegin ();
	else
	{
		NextThink = level.Frame + FRAMETIME;
		ThinkType = BRUSHTHINK_AMOVEBEGIN;
	}
}

/*
==============
Think_AccelMove

The team has completed a frame of movement, so
change the speed for the next frame
==============
*/

void CBrushModel::CalcAcceleratedMove()
{
	float	accel_dist;
	float	decel_dist;

	MoveSpeed = Speed;

	if (RemainingDistance < Accel)
	{
		CurrentSpeed = RemainingDistance;
		return;
	}

	accel_dist = AccelerationDistance (Speed, Accel);
	decel_dist = AccelerationDistance (Speed, Decel);

	if ((RemainingDistance - accel_dist - decel_dist) < 0)
	{
		float	f;

		f = (Accel + Decel) / (Accel * Decel);
		MoveSpeed = (-2 + sqrtf(4 - 4 * f * (-2 * RemainingDistance))) / (2 * f);
		decel_dist = AccelerationDistance (MoveSpeed, Decel);
	}

	DecelDistance = decel_dist;
};

void CBrushModel::Accelerate ()
{
	// are we decelerating?
	if (RemainingDistance <= DecelDistance)
	{
		if (RemainingDistance < DecelDistance)
		{
			if (NextSpeed)
			{
				CurrentSpeed = NextSpeed;
				NextSpeed = 0;
				return;
			}
			if (CurrentSpeed > Decel)
				CurrentSpeed -= Decel;
		}
		return;
	}

	// are we at full speed and need to start decelerating during this move?
	if (CurrentSpeed == MoveSpeed)
		if ((RemainingDistance - CurrentSpeed) < DecelDistance)
		{
			float	p1_distance;
			float	p2_distance;
			float	distance;

			p1_distance = RemainingDistance - DecelDistance;
			p2_distance = MoveSpeed * (1.0 - (p1_distance / MoveSpeed));
			distance = p1_distance + p2_distance;
			CurrentSpeed = MoveSpeed;
			NextSpeed = MoveSpeed - Decel * (p2_distance / distance);
			return;
		}

	// are we accelerating?
	if (CurrentSpeed < Speed)
	{
		float	old_speed;
		float	p1_distance;
		float	p1_speed;
		float	p2_distance;
		float	distance;

		old_speed = CurrentSpeed;

		// figure simple acceleration up to move_speed
		CurrentSpeed += Accel;
		if (CurrentSpeed > Speed)
			CurrentSpeed = Speed;

		// are we accelerating throughout this entire move?
		if ((RemainingDistance - CurrentSpeed) >= DecelDistance)
			return;

		// during this move we will accelrate from current_speed to move_speed
		// and cross over the decel_distance; figure the average speed for the
		// entire move
		p1_distance = RemainingDistance - DecelDistance;
		p1_speed = (old_speed + MoveSpeed) / 2.0;
		p2_distance = MoveSpeed * (1.0 - (p1_distance / p1_speed));
		distance = p1_distance + p2_distance;
		CurrentSpeed = (p1_speed * (p1_distance / distance)) + (MoveSpeed * (p2_distance / distance));
		NextSpeed = MoveSpeed - Decel * (p2_distance / distance);
		return;
	}

	// we are at constant velocity (move_speed)
	return;
};

void CBrushModel::ThinkAccelMove ()
{
	RemainingDistance -= CurrentSpeed;

	if (CurrentSpeed == 0)		// starting or blocked
		CalcAcceleratedMove();

	Accelerate ();

	// will the entire move complete on next frame?
	if (RemainingDistance <= CurrentSpeed)
	{
		MoveFinal ();
		return;
	}

	Velocity = Dir * CurrentSpeed;
	NextThink = level.Frame + FRAMETIME;
	ThinkType = BRUSHTHINK_MOVEACCEL;
}
#pragma endregion Brush_Model

#pragma region Platforms
#define PLAT_LOW_TRIGGER	1

CPlatForm::CPlatForm() :
CBaseEntity(),
CMapEntity(),
CBlockableEntity(),
CUsableEntity(),
CBrushModel(),
Height (0)
{
	BrushType |= BRUSH_PLATFORM;
};

CPlatForm::CPlatForm(int Index) :
CBaseEntity(Index),
CMapEntity(Index),
CBlockableEntity(Index),
CUsableEntity(Index),
CBrushModel(Index),
Height (0)
{
	BrushType |= BRUSH_PLATFORM;
};

bool CPlatForm::Run ()
{
	return CBrushModel::Run();
};

void CPlatForm::Blocked (CBaseEntity *other)
{
	if (!(other->GetSvFlags() & SVF_MONSTER) && !(other->EntityFlags & ENT_PLAYER) )
	{
		// give it a chance to go away on it's own terms (like gibs)
		if ((other->EntityFlags & ENT_HURTABLE) && entity_cast<CHurtableEntity>(other)->CanTakeDamage)
			entity_cast<CHurtableEntity>(other)->TakeDamage (this, this, vec3fOrigin, other->State.GetOrigin(), vec3fOrigin, 100000, 1, 0, MOD_CRUSH);

		// if it's still there, nuke it
		if (!other->Freed)
			other->BecomeExplosion(false);
		return;
	}

	if ((other->EntityFlags & ENT_HURTABLE) && entity_cast<CHurtableEntity>(other)->CanTakeDamage)
		entity_cast<CHurtableEntity>(other)->TakeDamage (this, this, vec3fOrigin, other->State.GetOrigin(), vec3fOrigin, Damage, 1, 0, MOD_CRUSH);

	if (MoveState == STATE_UP)
		GoDown ();
	else if (MoveState == STATE_DOWN)
		GoUp ();
};

void CPlatForm::Use (CBaseEntity *other, CBaseEntity *activator)
{
	if (ThinkType)
		return;		// already down
	GoDown ();
};

void CPlatForm::HitTop ()
{
	if (!(Flags & FL_TEAMSLAVE))
	{
		if (SoundEnd)
			PlaySound (CHAN_NO_PHS_ADD+CHAN_VOICE, SoundEnd, 255, ATTN_STATIC);
		State.GetSound() = 0;
	}
	MoveState = STATE_TOP;

	ThinkType = PLATTHINK_GO_DOWN;
	NextThink = level.Frame + 30;
}

void CPlatForm::HitBottom ()
{
	if (!(Flags & FL_TEAMSLAVE))
	{
		if (SoundEnd)
			PlaySound (CHAN_NO_PHS_ADD+CHAN_VOICE, SoundEnd, 255, ATTN_STATIC);
		State.GetSound() = 0;
	}
	MoveState = STATE_BOTTOM;
}

void CPlatForm::DoEndFunc ()
{
	switch (EndFunc)
	{
	case PLATENDFUNC_HITBOTTOM:
		HitBottom ();
		break;
	case PLATENDFUNC_HITTOP:
		HitTop ();
		break;
	};
};

void CPlatForm::GoDown ()
{
	if (!(Flags & FL_TEAMSLAVE))
	{
		if (SoundStart)
			PlaySound (CHAN_NO_PHS_ADD+CHAN_VOICE, SoundStart, 255, ATTN_STATIC);
		State.GetSound() = SoundMiddle;
	}
	MoveState = STATE_DOWN;
	MoveCalc (EndOrigin, PLATENDFUNC_HITBOTTOM);
}

void CPlatForm::GoUp ()
{
	if (!(Flags & FL_TEAMSLAVE))
	{
		if (SoundStart)
			PlaySound (CHAN_NO_PHS_ADD+CHAN_VOICE, SoundStart, 255, ATTN_STATIC);
		State.GetSound() = SoundMiddle;
	}
	MoveState = STATE_UP;
	MoveCalc (StartOrigin, PLATENDFUNC_HITTOP);
}

void CPlatForm::Think ()
{
	switch (ThinkType)
	{
	case PLATTHINK_GO_DOWN:
		GoDown ();
		break;
	default:
		CBrushModel::Think ();
	};
}

CPlatForm::CPlatFormInsideTrigger::CPlatFormInsideTrigger () :
CBaseEntity(),		
CTouchableEntity()
{
};

CPlatForm::CPlatFormInsideTrigger::CPlatFormInsideTrigger (int Index) :
CBaseEntity(Index),
CTouchableEntity(Index)
{
};

void CPlatForm::CPlatFormInsideTrigger::Touch (CBaseEntity *other, plane_t *plane, cmBspSurface_t *surf)
{
	if (!(other->EntityFlags & ENT_HURTABLE) || entity_cast<CHurtableEntity>(other)->Health <= 0)
		return;
	if (!other->gameEntity->client)
		return;

	if (Owner->MoveState == STATE_BOTTOM)
		Owner->GoUp ();
	else if (Owner->MoveState == STATE_TOP)
		Owner->NextThink = level.Frame + 10;	// the player is still on the plat, so delay going down
};

void CPlatForm::SpawnInsideTrigger ()
{
	CPlatFormInsideTrigger	*trigger = QNew (com_levelPool, 0) CPlatFormInsideTrigger;
	vec3f	tmin, tmax;

	//
	// middle trigger
	//	
	trigger->GetSolid() = SOLID_TRIGGER;
	trigger->Owner = this;

	tmin = GetMins();
	tmin.X += 25;
	tmin.Y += 25;

	tmax = GetMaxs();
	tmax.X -= 25;
	tmax.Y -= 25;
	tmax.Z += 8;

	tmin.Z = tmax.Z - (Positions[0].Z - Positions[1].Z + Lip);

	if (SpawnFlags & PLAT_LOW_TRIGGER)
		tmax.Z = tmin.Z + 8;

	if (tmax.X - tmin.X <= 0)
	{
		tmin.X = (GetMins().X + GetMaxs().X) *0.5;
		tmax.X = tmin.X + 1;
	}
	if (tmax.Y - tmin.Y <= 0)
	{
		tmin.Y = (GetMins().Y + GetMaxs().Y) *0.5;
		tmax.Y = tmin.Y + 1;
	}

	trigger->GetMins() = tmin;
	trigger->GetMaxs() = tmax;

	trigger->Link ();
};

void CPlatForm::Spawn ()
{
	State.GetAngles().Clear ();
	GetSolid() = SOLID_BSP;
	PhysicsType = PHYSICS_PUSH;

	SetBrushModel ();

	if (!Speed)
		Speed = 20;
	else
		Speed *= 0.1f;

	if (!Accel)
		Accel = 5;
	else
		Accel *= 0.1f;

	if (!Decel)
		Decel = 5;
	else
		Decel *= 0.1f;

	if (!Damage)
		Damage = 2;

	if (!Lip)
		Lip = 8;

	// pos1 is the top position, pos2 is the bottom
	Positions[0] = Positions[1] = State.GetOrigin ();
	Positions[1].Z -= (Height) ? Height : ((GetMaxs().Z - GetMins().Z) - Lip);

	if (TargetName)
		MoveState = STATE_UP;
	else
	{
		State.GetOrigin() = Positions[1];
		Link ();
		MoveState = STATE_BOTTOM;
	}

	StartOrigin = Positions[0];
	StartAngles = State.GetAngles ();
	EndAngles = State.GetAngles ();
	EndOrigin = Positions[1];

	SoundStart = SoundIndex ("plats/pt1_strt.wav");
	SoundMiddle = SoundIndex ("plats/pt1_mid.wav");
	SoundEnd = SoundIndex ("plats/pt1_end.wav");

	if (!map_debug->Boolean())
		SpawnInsideTrigger ();	// the "start moving" trigger	
	else
	{
		GetSolid() = SOLID_BBOX;
		GetSvFlags() = (SVF_MONSTER|SVF_DEADMONSTER);
		Link ();
	}
};

ENTITYFIELDS_BEGIN(CPlatForm)
{
	CEntityField ("height", EntityMemberOffset(CPlatForm,Height), FT_INT),
};
ENTITYFIELDS_END(CPlatForm)

bool			CPlatForm::ParseField (const char *Key, const char *Value)
{
	if (CheckFields<CPlatForm> (this, Key, Value))
		return true;

	// Couldn't find it here
	return (CBrushModel::ParseField (Key, Value) || CUsableEntity::ParseField (Key, Value) || CMapEntity::ParseField (Key, Value));
};

LINK_CLASSNAME_TO_CLASS ("func_plat", CPlatForm);
#pragma endregion Platforms

#pragma region Doors
#pragma region Base Door

#define DOOR_START_OPEN		1
#define DOOR_REVERSE		2
#define DOOR_CRUSHER		4
#define DOOR_NOMONSTER		8
#define DOOR_TOGGLE			32
#define DOOR_X_AXIS			64
#define DOOR_Y_AXIS			128


CDoor::CDoor() :
CBaseEntity(),
CMapEntity(),
CBrushModel(),
CBlockableEntity(),
CUsableEntity(),
CHurtableEntity(),
CTouchableEntity()
{
	BrushType |= BRUSH_DOOR;
};

CDoor::CDoor(int Index) :
CBaseEntity(Index),
CMapEntity(Index),
CBrushModel(Index),
CBlockableEntity(Index),
CUsableEntity(Index),
CHurtableEntity(Index),
CTouchableEntity(Index)
{
	BrushType |= BRUSH_DOOR;
};

bool CDoor::Run ()
{
	return CBrushModel::Run();
};

void CDoor::UseAreaPortals (bool isOpen)
{
	CMapEntity	*t = NULL;

	if (!Target)
		return;

	while ((t = CC_Find<CMapEntity, ENT_MAP, EntityMemberOffset(CMapEntity,TargetName)> (t, Target)) != NULL)
	{
		if (Q_stricmp(t->gameEntity->classname, "func_areaportal") == 0)
			gi.SetAreaPortalState (t->gameEntity->style, isOpen);
	}
}

void CDoor::HitTop ()
{
	if (!(Flags & FL_TEAMSLAVE))
	{
		if (SoundEnd)
			PlaySound (CHAN_NO_PHS_ADD+CHAN_VOICE, SoundEnd, 255, ATTN_STATIC);
		State.GetSound() = 0;
	}
	MoveState = STATE_TOP;
	if (SpawnFlags & DOOR_TOGGLE)
		return;
	if (Wait >= 0)
	{
		ThinkType = DOORTHINK_GODOWN;
		NextThink = level.Frame + Wait;
	}
}

void CDoor::HitBottom ()
{
	if (!(Flags & FL_TEAMSLAVE))
	{
		if (SoundEnd)
			PlaySound (CHAN_NO_PHS_ADD+CHAN_VOICE, SoundEnd, 255, ATTN_STATIC);
		State.GetSound() = 0;
	}
	MoveState = STATE_BOTTOM;
	UseAreaPortals (false);
}

void CDoor::GoDown ()
{
	if (!(Flags & FL_TEAMSLAVE))
	{
		if (SoundStart)
			PlaySound (CHAN_NO_PHS_ADD+CHAN_VOICE, SoundStart, 255, ATTN_STATIC);
		State.GetSound() = SoundMiddle;
	}
	if (MaxHealth)
	{
		CanTakeDamage = true;
		Health = MaxHealth;
	}
	
	MoveState = STATE_DOWN;
	//if (strcmp(self->classname, "func_door") == 0)
	MoveCalc (StartOrigin, DOORENDFUNC_HITBOTTOM);
	//else if (strcmp(self->classname, "func_door_rotating") == 0)
	//	AngleMove_Calc (self, door_hit_bottom);
}

void CDoor::GoUp (CBaseEntity *activator)
{
	if (MoveState == STATE_UP)
		return;		// already going up

	if (MoveState == STATE_TOP)
	{	// reset top wait time
		if (Wait >= 0)
			NextThink = level.Frame + Wait;
		return;
	}
	
	if (!(Flags & FL_TEAMSLAVE))
	{
		if (SoundStart)
			PlaySound (CHAN_NO_PHS_ADD+CHAN_VOICE, SoundStart, 255, ATTN_STATIC);
		State.GetSound() = SoundMiddle;
	}
	MoveState = STATE_UP;
	//if (strcmp(self->classname, "func_door") == 0)
	MoveCalc (EndOrigin, DOORENDFUNC_HITTOP);
	//else if (strcmp(self->classname, "func_door_rotating") == 0)
	//	AngleMove_Calc (self, door_hit_top);

	UseTargets (activator, Message);
	UseAreaPortals (true);
}

void CDoor::Use (CBaseEntity *other, CBaseEntity *activator)
{
	if (Flags & FL_TEAMSLAVE)
		return;

	if (SpawnFlags & DOOR_TOGGLE)
	{
		if (MoveState == STATE_UP || MoveState == STATE_TOP)
		{
			// trigger all paired doors
			for (CDoor *Door = this ; Door ; Door = entity_cast<CDoor>(Door->TeamChain))	
			{
				if (Door->Message)
					QDelete Door->Message;
				Door->Message = NULL;
				Door->Touchable = false;
				Door->GoDown();
			}
			return;
		}
	}
	
	// trigger all paired doors
	for (CDoor *Door = this; Door; Door = entity_cast<CDoor>(Door->TeamChain))
	{
		if (Door->Message)
			QDelete Door->Message;
		Door->Message = NULL;
		Door->Touchable = false;
		Door->GoUp (activator);
	}
};

CDoor::CDoorTrigger::CDoorTrigger () :
CBaseEntity(),		
CTouchableEntity()
{
};

CDoor::CDoorTrigger::CDoorTrigger (int Index) :
CBaseEntity(Index),
CTouchableEntity(Index)
{
};

void CDoor::CDoorTrigger::Touch (CBaseEntity *other, plane_t *plane, cmBspSurface_t *surf)
{
	if (!(other->EntityFlags & ENT_HURTABLE) || entity_cast<CHurtableEntity>(other)->Health <= 0)
		return;

	if (!(other->GetSvFlags() & SVF_MONSTER) && (!(other->EntityFlags & ENT_PLAYER)))
		return;

	if ((GetOwner()->SpawnFlags & DOOR_NOMONSTER) && (other->EntityFlags & ENT_MONSTER))
		return;

	if (level.Frame < TouchDebounce)
		return;
	TouchDebounce = level.Frame + 10;

	(entity_cast<CDoor>(GetOwner()))->Use (other, other);
}

void CDoor::CalcMoveSpeed ()
{
	if (Flags & FL_TEAMSLAVE)
		return;		// only the team master does this

	// find the smallest distance any member of the team will be moving
	float min = Q_fabs(Distance);
	for (CDoor *Door = entity_cast<CDoor>(TeamChain); Door; Door = entity_cast<CDoor>(Door->TeamChain))
	{
		float dist = Q_fabs(Door->Distance);
		if (dist < min)
			min = dist;
	}

	float time = min / Speed;

	// adjust speeds so they will all complete at the same time
	for (CDoor *Door = this; Door; Door = entity_cast<CDoor>(Door->TeamChain))
	{
		float newspeed = Q_fabs(Door->Distance) / time;
		float ratio = newspeed / Door->Speed;
		if (Door->Accel == Door->Speed)
			Door->Accel = newspeed;
		else
			Door->Accel *= ratio;
		if (Door->Decel == Door->Speed)
			Door->Decel = newspeed;
		else
			Door->Decel *= ratio;
		Door->Speed = newspeed;
	}
}

void CDoor::SpawnDoorTrigger ()
{
	vec3f		mins, maxs;

	if (Flags & FL_TEAMSLAVE)
		return;		// only the team leader spawns a trigger

	mins = GetAbsMin ();
	maxs = GetAbsMax ();

	for (CBaseEntity *other = TeamChain; other; other = other->TeamChain)
	{
		AddPointToBounds (other->GetAbsMin(), mins, maxs);
		AddPointToBounds (other->GetAbsMax(), mins, maxs);
	}

	// expand 
	mins.X -= 60;
	mins.Y -= 60;
	maxs.X += 60;
	maxs.Y += 60;

	CDoorTrigger *Trigger = QNew (com_levelPool, 0) CDoorTrigger();
	Trigger->GetMins() = mins;
	Trigger->GetMaxs() = maxs;
	Trigger->SetOwner (this);
	Trigger->Touchable = true;
	Trigger->GetSolid() = SOLID_TRIGGER;
	Trigger->Link ();

	if (SpawnFlags & DOOR_START_OPEN)
		UseAreaPortals (true);

	CalcMoveSpeed ();
}

void CDoor::Blocked (CBaseEntity *other)
{
	if (!(other->EntityFlags & ENT_PLAYER) && !(other->EntityFlags & ENT_MONSTER) )
	{
		// give it a chance to go away on it's own terms (like gibs)
		if ((other->EntityFlags & ENT_HURTABLE) && entity_cast<CHurtableEntity>(other)->CanTakeDamage)
			entity_cast<CHurtableEntity>(other)->TakeDamage (this, this, vec3fOrigin, other->State.GetOrigin(), vec3fOrigin, 100000, 1, 0, MOD_CRUSH);

		// if it's still there, nuke it
		if (other->GetInUse())
			other->BecomeExplosion (false);
		return;
	}

	if ((other->EntityFlags & ENT_HURTABLE) && entity_cast<CHurtableEntity>(other)->CanTakeDamage)
		entity_cast<CHurtableEntity>(other)->TakeDamage (this, this, vec3fOrigin, other->State.GetOrigin(), vec3fOrigin, Damage, 1, 0, MOD_CRUSH);

	if (SpawnFlags & DOOR_CRUSHER)
		return;


// if a door has a negative wait, it would never come back if blocked,
// so let it just squash the object to death real fast
	if (Wait >= 0)
	{
		if (MoveState == STATE_DOWN)
		{
			for (CBaseEntity *ent = TeamMaster ; ent ; ent = ent->TeamChain)
				(entity_cast<CDoor>(ent))->GoUp ((Activator) ? Activator : NULL);
		}
		else
		{
			for (CBaseEntity *ent = TeamMaster ; ent ; ent = ent->TeamChain)
				(entity_cast<CDoor>(ent))->GoDown ();
		}
	}
}

void CDoor::Die (CBaseEntity *inflictor, CBaseEntity *attacker, int damage, vec3f &point)
{
	for (CBaseEntity *ent = TeamMaster ; ent ; ent = ent->TeamChain)
	{
		CDoor *Door = entity_cast<CDoor>(ent);
		Door->Health = Door->MaxHealth;
		Door->CanTakeDamage = false;
	}

	(entity_cast<CDoor>(TeamMaster))->Use (attacker, attacker);
}

void CDoor::Pain (CBaseEntity *other, float kick, int damage)
{
}

void CDoor::Touch (CBaseEntity *other, plane_t *plane, cmBspSurface_t *surf)
{
	if (!(other->EntityFlags & ENT_PLAYER))
		return;

	if (level.Frame < TouchDebounce)
		return;

	TouchDebounce = level.Frame + 50;

	(entity_cast<CPlayerEntity>(other))->PrintToClient (PRINT_CENTER, "%s", Message);
	other->PlaySound (CHAN_AUTO, SoundIndex ("misc/talk1.wav"));
}

void CDoor::DoEndFunc ()
{
	switch (EndFunc)
	{
	case DOORENDFUNC_HITBOTTOM:
		HitBottom ();
		break;
	case DOORENDFUNC_HITTOP:
		HitTop ();
		break;
	};
}

void CDoor::Think ()
{
	switch (ThinkType)
	{
	case DOORTHINK_SPAWNDOORTRIGGER:
		SpawnDoorTrigger ();
		break;
	case DOORTHINK_CALCMOVESPEED:
		CalcMoveSpeed ();
		break;
	case DOORTHINK_GODOWN:
		GoDown ();
		break;
	default:
		CBrushModel::Think ();
	};
}

void CDoor::Spawn ()
{
	if (gameEntity->sounds != 1)
	{
		SoundStart = SoundIndex  ("doors/dr1_strt.wav");
		SoundMiddle = SoundIndex  ("doors/dr1_mid.wav");
		SoundEnd = SoundIndex  ("doors/dr1_end.wav");
	}

	SetMoveDir ();

	PhysicsType = PHYSICS_PUSH;
	GetSolid() = SOLID_BSP;
	SetBrushModel ();
	
	if (!Speed)
		Speed = 100;
	if (game.mode & GAME_DEATHMATCH)
		Speed *= 2;

	if (!Accel)
		Accel = Speed;
	if (!Decel)
		Decel = Speed;

	if (!Wait)
		Wait = 30;
	if (!Lip)
		Lip = 8;
	if (!Damage)
		Damage = 2;

	// calculate second position
	Positions[0] = State.GetOrigin ();

	Distance = Q_fabs(MoveDir.X) * GetSize().X + Q_fabs(MoveDir.Y) * GetSize().Y + Q_fabs(MoveDir.Z) * GetSize().Z - Lip;
	Positions[1] = Positions[0].MultiplyAngles (Distance, MoveDir);

	// if it starts open, switch the positions
	if (SpawnFlags & DOOR_START_OPEN)
	{
		State.GetOrigin() = Positions[1];
		Positions[1] = Positions[0];
		Positions[0] = State.GetOrigin ();
	}

	MoveState = STATE_BOTTOM;

	Touchable = false;
	if (Health)
	{
		CanTakeDamage = true;
		MaxHealth = Health;
	}
	else if (TargetName && Message)
	{
		SoundIndex ("misc/talk.wav");
		Touchable = true;
	}
	
	StartOrigin = Positions[0];
	StartAngles = State.GetAngles ();
	EndOrigin = Positions[1];
	EndAngles = State.GetAngles ();

	if (SpawnFlags & 16)
		State.GetEffects() |= EF_ANIM_ALL;
	if (SpawnFlags & 64)
		State.GetEffects() |= EF_ANIM_ALLFAST;

	// to simplify logic elsewhere, make non-teamed doors into a team of one
	if (!gameEntity->team)
		TeamMaster = this;

	Link ();

	NextThink = level.Frame + FRAMETIME;

	if (map_debug->Boolean())
	{
		GetSolid() = SOLID_BSP;
		GetSvFlags() = (SVF_MONSTER|SVF_DEADMONSTER);
		Link ();
	}
	else if (Health || TargetName)
		ThinkType = DOORTHINK_CALCMOVESPEED;
	else
		ThinkType = DOORTHINK_SPAWNDOORTRIGGER;
}

LINK_CLASSNAME_TO_CLASS ("func_door", CDoor);
#pragma endregion Base Door

#pragma region Rotating Door
CRotatingDoor::CRotatingDoor () :
CBaseEntity(),
CDoor ()
{
	BrushType |= BRUSH_ROTATINGDOOR;
};

CRotatingDoor::CRotatingDoor (int Index) :
CBaseEntity(Index),
CDoor(Index)
{
	BrushType |= BRUSH_ROTATINGDOOR;
};

void CRotatingDoor::GoDown ()
{
	if (!(Flags & FL_TEAMSLAVE))
	{
		if (SoundStart)
			PlaySound (CHAN_NO_PHS_ADD+CHAN_VOICE, SoundStart, 255, ATTN_STATIC);
		State.GetSound() = SoundMiddle;
	}
	if (MaxHealth)
	{
		CanTakeDamage = true;
		Health = MaxHealth;
	}
	
	MoveState = STATE_DOWN;
	AngleMoveCalc (DOORENDFUNC_HITBOTTOM);
}

void CRotatingDoor::GoUp (CBaseEntity *activator)
{
	if (MoveState == STATE_UP)
		return;		// already going up

	if (MoveState == STATE_TOP)
	{	// reset top wait time
		if (Wait >= 0)
			NextThink = level.Frame + Wait;
		return;
	}
	
	if (!(Flags & FL_TEAMSLAVE))
	{
		if (SoundStart)
			PlaySound (CHAN_NO_PHS_ADD+CHAN_VOICE, SoundStart, 255, ATTN_STATIC);
		State.GetSound() = SoundMiddle;
	}
	MoveState = STATE_UP;
	AngleMoveCalc (DOORENDFUNC_HITTOP);

	UseTargets (activator, Message);
	UseAreaPortals (true);
}

void CRotatingDoor::Spawn ()
{
	State.GetAngles().Clear ();

	// set the axis of rotation
	MoveDir.Clear ();
	if (SpawnFlags & DOOR_X_AXIS)
		MoveDir.Z = 0.1f;
	else if (SpawnFlags & DOOR_Y_AXIS)
		MoveDir.X = 0.1f;
	else // Z_AXIS
		MoveDir.Y = 0.1f;

	// check for reverse rotation
	if (SpawnFlags & DOOR_REVERSE)
		MoveDir.Invert ();

	if (!Distance)
	{
		//gi.dprintf("%s at (%f %f %f) with no distance set\n", ent->classname, ent->state.origin[0], ent->state.origin[1], ent->state.origin[2]);
		MapPrint (MAPPRINT_WARNING, this, State.GetOrigin(), "No distance set\n");
		Distance = 90;
	}

	Positions[0] = State.GetAngles ();
	Positions[1] = Positions[0].MultiplyAngles (Distance, MoveDir);

	PhysicsType = PHYSICS_PUSH;
	GetSolid() = SOLID_BSP;
	SetBrushModel ();

	if (!Speed)
		Speed = 100;
	if (!Accel)
		Accel = Speed;
	if (!Decel)
		Decel = Speed;

	if (!Wait)
		Wait = 30;
	if (!Damage)
		Damage = 2;

	if (gameEntity->sounds != 1)
	{
		SoundStart = SoundIndex  ("doors/dr1_strt.wav");
		SoundMiddle = SoundIndex  ("doors/dr1_mid.wav");
		SoundEnd = SoundIndex  ("doors/dr1_end.wav");
	}

	Positions[0] *= 10;
	Positions[1] *= 10;

	// if it starts open, switch the positions
	if (SpawnFlags & DOOR_START_OPEN)
	{
		State.GetAngles() = Positions[1];
		Positions[1] = Positions[0];
		Positions[0] = State.GetAngles ();
		MoveDir.Invert ();
	}

	if (Health)
	{
		CanTakeDamage = true;
		MaxHealth = Health;
	}
	
	if (TargetName && Message)
	{
		SoundIndex ("misc/talk.wav");
		Touchable = true;
	}
	else
		Touchable = false;

	MoveState = STATE_BOTTOM;
	StartOrigin = State.GetOrigin ();
	EndOrigin = State.GetOrigin ();
	StartAngles = Positions[0];
	EndAngles = Positions[1];

	if (SpawnFlags & 16)
		State.GetEffects() |= EF_ANIM_ALL;

	// to simplify logic elsewhere, make non-teamed doors into a team of one
	if (!gameEntity->team)
		TeamMaster = this;

	Link ();

	NextThink = level.Frame + FRAMETIME;
	if (Health || TargetName)
		ThinkType = DOORTHINK_CALCMOVESPEED;
	else
		ThinkType = DOORTHINK_SPAWNDOORTRIGGER;
}

LINK_CLASSNAME_TO_CLASS ("func_door_rotating", CRotatingDoor);
#pragma endregion Rotating Door

#pragma region Moving Water
CMovableWater::CMovableWater () :
CBaseEntity(),
CDoor ()
{
};

CMovableWater::CMovableWater (int Index) :
CBaseEntity(Index),
CDoor(Index)
{
};

void CMovableWater::Spawn ()
{
	SetMoveDir ();

	PhysicsType = PHYSICS_PUSH;
	GetSolid() = SOLID_BSP;
	SetBrushModel ();

	switch (gameEntity->sounds)
	{
		default:
			break;

		case 1: // water
		case 2: // lava
			SoundStart = SoundIndex  ("world/mov_watr.wav");
			SoundEnd = SoundIndex  ("world/stp_watr.wav");
			break;
	}

	// calculate second position
	Positions[0] = State.GetOrigin ();

	Distance = Q_fabs(MoveDir.X) * GetSize().X + Q_fabs(MoveDir.Y) * GetSize().Y + Q_fabs(MoveDir.Z) * GetSize().Z - Lip;
	Positions[1] = Positions[0].MultiplyAngles (Distance, MoveDir);

	// if it starts open, switch the positions
	if (SpawnFlags & DOOR_START_OPEN)
	{
		State.GetOrigin() = Positions[1];
		Positions[1] = Positions[0];
		Positions[0] = State.GetOrigin ();
	}

	StartOrigin = Positions[0];
	StartAngles = State.GetAngles();
	EndOrigin = Positions[1];
	EndAngles = State.GetAngles();

	MoveState = STATE_BOTTOM;

	if (!Speed)
		Speed = 25;
	Accel = Decel = Speed;

	if (!Wait)
		Wait = -1;

	Touchable = false;

	if (Wait == -1)
		SpawnFlags |= DOOR_TOGGLE;

	//gameEntity->classname = "func_door";

	Link ();
};

LINK_CLASSNAME_TO_CLASS ("func_water", CMovableWater);
#pragma endregion Moving Water

#pragma region Secret Door
/*QUAKED func_door_secret (0 .5 .8) ? always_shoot 1st_left 1st_down
A secret door.  Slide back and then to the side.

open_once		doors never closes
1st_left		1st move is left of arrow
1st_down		1st move is down from arrow
always_shoot	door is shootebale even if targeted

"angle"		determines the direction
"dmg"		damage to inflic when blocked (default 2)
"wait"		how long to hold in the open position (default 5, -1 means hold)
*/

#define SECRET_ALWAYS_SHOOT	1
#define SECRET_1ST_LEFT		2
#define SECRET_1ST_DOWN		4

CDoorSecret::CDoorSecret () :
CBaseEntity(),
CDoor ()
{
	BrushType |= BRUSH_SECRETDOOR;
};

CDoorSecret::CDoorSecret (int Index) :
CBaseEntity(Index),
CDoor(Index)
{
	BrushType |= BRUSH_SECRETDOOR;
};

void CDoorSecret::DoEndFunc ()
{
	switch (EndFunc)
	{
		case DOORSECRETENDFUNC_DONE:
			if (!(TargetName) || (SpawnFlags & SECRET_ALWAYS_SHOOT))
			{
				Health = 0;
				CanTakeDamage = true;
			}
			UseAreaPortals (false);
			break;
		case DOORSECRETENDFUNC_5:
			NextThink = level.Frame + 10;
			ThinkType = DOORSECRETTHINK_6;
			break;
		case DOORSECRETENDFUNC_3:
			if (Wait == -1)
				return;

			// Backcompat
			NextThink = level.Frame + Wait;
			ThinkType = DOORSECRETTHINK_4;
			break;
		case DOORSECRETENDFUNC_1:
			NextThink = level.Frame + 10;
			ThinkType = DOORSECRETTHINK_2;
			break;
	};
}

void CDoorSecret::Think ()
{
	switch (ThinkType)
	{
	case DOORSECRETTHINK_6:
		MoveCalc (vec3fOrigin, DOORSECRETENDFUNC_DONE);
		break;
	case DOORSECRETTHINK_4:
		MoveCalc (Positions[0], DOORSECRETENDFUNC_5);
		break;
	case DOORSECRETTHINK_2:
		MoveCalc (Positions[1], DOORSECRETENDFUNC_3);
		break;
	default:
		CBrushModel::Think ();
	}
}

void CDoorSecret::Use (CBaseEntity *other, CBaseEntity *activator)
{
	// make sure we're not already moving
	if (State.GetOrigin() != vec3fOrigin)
		return;

	MoveCalc (Positions[0], DOORSECRETENDFUNC_1);
	UseAreaPortals (true);
}

void CDoorSecret::Blocked (CBaseEntity *other)
{
	if (!(other->EntityFlags & ENT_MONSTER) && (!(other->EntityFlags & ENT_PLAYER)) )
	{
		// give it a chance to go away on it's own terms (like gibs)
		if ((other->EntityFlags & ENT_HURTABLE) && entity_cast<CHurtableEntity>(other)->CanTakeDamage)
			entity_cast<CHurtableEntity>(other)->TakeDamage (this, this, vec3fOrigin, State.GetOrigin(), vec3fOrigin, 100000, 1, 0, MOD_CRUSH);

		// if it's still there, nuke it
		if (other->GetInUse())
			other->BecomeExplosion(false);
		return;
	}

	if (level.Frame < TouchDebounce)
		return;
	TouchDebounce = level.Frame + 5;

	if ((other->EntityFlags & ENT_HURTABLE) && entity_cast<CHurtableEntity>(other)->CanTakeDamage)
		entity_cast<CHurtableEntity>(other)->TakeDamage (this, this, vec3fOrigin, State.GetOrigin(), vec3fOrigin, Damage, 1, 0, MOD_CRUSH);
}

void CDoorSecret::Die (CBaseEntity *inflictor, CBaseEntity *attacker, int damage, vec3f &point)
{
	CanTakeDamage = false;
	Use (attacker, attacker);
}

void CDoorSecret::Spawn ()
{
	SoundStart = SoundIndex  ("doors/dr1_strt.wav");
	SoundMiddle = SoundIndex  ("doors/dr1_mid.wav");
	SoundEnd = SoundIndex  ("doors/dr1_end.wav");

	PhysicsType = PHYSICS_PUSH;
	GetSolid() = SOLID_BSP;
	SetBrushModel ();

	if (!(TargetName) || (SpawnFlags & SECRET_ALWAYS_SHOOT))
	{
		Health = 0;
		CanTakeDamage = true;
	}

	Touchable = false;

	if (!Damage)
		Damage = 2;

	if (!Wait)
		Wait = 50;

	Accel =
	Decel =
	Speed = 50;

	// calculate positions
	vec3f	forward, right, up;
	State.GetAngles().ToVectors (&forward, &right, &up);
	State.GetAngles().Clear ();

	float width = (SpawnFlags & SECRET_1ST_DOWN) ? Q_fabs(up.Dot (GetSize())) : Q_fabs(right.Dot (GetSize()));
	float length = Q_fabs(forward.Dot (GetSize()));
	if (SpawnFlags & SECRET_1ST_DOWN)
		Positions[0] = State.GetOrigin ().MultiplyAngles (-1 * width, up);
	else
		Positions[0] = State.GetOrigin().MultiplyAngles ((1.0 - (SpawnFlags & SECRET_1ST_LEFT)) * width, right);
	Positions[1] = Positions[0].MultiplyAngles (length, forward);

	if (Health)
	{
		CanTakeDamage = true;
		MaxHealth = Health;
	}
	else if (TargetName && Message)
	{
		SoundIndex ("misc/talk.wav");
		Touchable = true;
	}
	
	//gameEntity->classname = "func_door";

	Link ();
}

LINK_CLASSNAME_TO_CLASS ("func_door_secret", CDoorSecret);
#pragma endregion Secret Door
#pragma endregion Doors

#pragma region Button
/*
======================================================================

BUTTONS

======================================================================
*/

/*QUAKED func_button (0 .5 .8) ?
When a button is touched, it moves some distance in the direction of it's angle, triggers all of it's targets, waits some time, then returns to it's original position where it can be triggered again.

"angle"		determines the opening direction
"target"	all entities with a matching targetname will be used
"speed"		override the default 40 speed
"wait"		override the default 1 second wait (-1 = never return)
"lip"		override the default 4 pixel lip remaining at end of move
"health"	if set, the button must be killed instead of touched
"sounds"
1) silent
2) steam metal
3) wooden clunk
4) metallic click
5) in-out
*/

CButton::CButton() :
CBaseEntity(),
CMapEntity(),
CBrushModel(),
CUsableEntity(),
CHurtableEntity(),
CTouchableEntity()
{
	BrushType |= BRUSH_BUTTON;
};

CButton::CButton(int Index) :
CBaseEntity(Index),
CMapEntity(Index),
CBrushModel(Index),
CUsableEntity(Index),
CHurtableEntity(Index),
CTouchableEntity(Index)
{
	BrushType |= BRUSH_BUTTON;
};

bool CButton::Run ()
{
	return CBrushModel::Run ();
};

void CButton::Pain (CBaseEntity *other, float kick, int damage)
{
};

void CButton::DoEndFunc ()
{
	switch (EndFunc)
	{
	case BUTTONENDFUNC_DONE:
		MoveState = STATE_BOTTOM;
		State.GetEffects() &= ~EF_ANIM23;
		State.GetEffects() |= EF_ANIM01;
		break;
	case BUTTONENDFUNC_WAIT:
		MoveState = STATE_TOP;
		State.GetEffects() &= ~EF_ANIM01;
		State.GetEffects() |= EF_ANIM23;

		UseTargets (Activator, Message);
		State.GetFrame() = 1;
		if (Wait >= 0)
		{
			NextThink = level.Frame + Wait;
			ThinkType = BUTTONTHINK_RETURN;
		}
		break;
	};
}

void CButton::Think ()
{
	switch (ThinkType)
	{
	case BUTTONTHINK_RETURN:
		MoveState = STATE_DOWN;
		MoveCalc (StartOrigin, BUTTONENDFUNC_DONE);
		State.GetFrame() = 0;

		if (Health)
			CanTakeDamage = true;
		break;
	default:
		CBrushModel::Think ();
	};
}

void CButton::Fire ()
{
	if (MoveState == STATE_UP || MoveState == STATE_TOP)
		return;

	MoveState = STATE_UP;
	if (SoundStart && !(Flags & FL_TEAMSLAVE))
		PlaySound (CHAN_NO_PHS_ADD+CHAN_VOICE, SoundStart, 255, ATTN_STATIC);
	MoveCalc (EndOrigin, BUTTONENDFUNC_WAIT);
}

void CButton::Use (CBaseEntity *other, CBaseEntity *activator)
{
	Activator = activator;
	Fire ();
}

void CButton::Touch (CBaseEntity *other, plane_t *plane, cmBspSurface_t *surf)
{
	if (!(other->EntityFlags & ENT_PLAYER))
		return;

	if (entity_cast<CPlayerEntity>(other)->Health <= 0)
		return;

	Activator = other;
	Fire ();
}

void CButton::Die (CBaseEntity *inflictor, CBaseEntity *attacker, int damage, vec3f &point)
{
	Activator = attacker;
	Health = MaxHealth;
	CanTakeDamage = false;
	Fire ();
}

void CButton::Spawn ()
{
	SetMoveDir ();

	PhysicsType = PHYSICS_STOP;
	GetSolid() = SOLID_BSP;
	SetBrushModel ();

	if (gameEntity->sounds != 1)
		SoundStart = SoundIndex ("switches/butn2.wav");
	
	if (!Speed)
		Speed = 40;
	if (!Accel)
		Accel = Speed;
	if (!Decel)
		Decel = Speed;

	if (!Wait)
		Wait = 30;
	if (!Lip)
		Lip = 4;

	Positions[0] = State.GetOrigin ();
	Positions[1] = Positions[0].MultiplyAngles (
		Q_fabs(MoveDir.X) * GetSize().X + Q_fabs(MoveDir.Y) * GetSize().Y + Q_fabs(MoveDir.Z) * GetSize().Z - Lip, MoveDir);

	State.GetEffects() |= EF_ANIM01;

	Touchable = false;
	if (Health)
	{
		MaxHealth = Health;
		CanTakeDamage = true;
	}
	else if (!TargetName)
		Touchable = true;

	MoveState = STATE_BOTTOM;

	StartOrigin = Positions[0];
	StartAngles = State.GetAngles ();
	EndOrigin = Positions[1];
	EndAngles = State.GetAngles ();

	Link ();
}

LINK_CLASSNAME_TO_CLASS ("func_button", CButton);
#pragma endregion Button

#pragma region Train
#define TRAIN_START_ON		1
#define TRAIN_TOGGLE		2
#define TRAIN_BLOCK_STOPS	4

/*QUAKED func_train (0 .5 .8) ? START_ON TOGGLE BLOCK_STOPS
Trains are moving platforms that players can ride.
The targets origin specifies the min point of the train at each corner.
The train spawns at the first target it is pointing at.
If the train is the target of a button or trigger, it will not begin moving until activated.
speed	default 100
dmg		default	2
noise	looping sound to play when the train is in motion

*/

CTrainBase::CTrainBase() :
CBaseEntity(),
CMapEntity(),
CBrushModel(),
CBlockableEntity(),
CUsableEntity()
{
	BrushType |= BRUSH_TRAIN;
};

CTrainBase::CTrainBase(int Index) :
CBaseEntity(Index),
CMapEntity(Index),
CBrushModel(Index),
CBlockableEntity(Index),
CUsableEntity(Index)
{
	BrushType |= BRUSH_TRAIN;
};

bool CTrainBase::Run ()
{
	return CBrushModel::Run ();
};

void CTrainBase::Blocked (CBaseEntity *other)
{
	if (!(other->EntityFlags & ENT_MONSTER) && (!(other->EntityFlags & ENT_PLAYER)) )
	{
		// give it a chance to go away on it's own terms (like gibs)
		if ((other->EntityFlags & ENT_HURTABLE) && entity_cast<CHurtableEntity>(other)->CanTakeDamage)
			entity_cast<CHurtableEntity>(other)->TakeDamage (this, this, vec3fOrigin, other->State.GetOrigin(), vec3fOrigin, 100000, 1, 0, MOD_CRUSH);

		// if it's still there, nuke it
		if (other->GetInUse())
			other->BecomeExplosion (false);
		return;
	}

	if (level.Frame < TouchDebounce)
		return;

	if (!Damage)
		return;

	TouchDebounce = level.Frame + 5;

	if ((other->EntityFlags & ENT_HURTABLE) && entity_cast<CHurtableEntity>(other)->CanTakeDamage)
		entity_cast<CHurtableEntity>(other)->TakeDamage (this, this, vec3fOrigin, other->State.GetOrigin(), vec3fOrigin, Damage, 1, 0, MOD_CRUSH);
}

void CTrainBase::TrainWait ()
{
	if (TargetEntity->gameEntity->pathtarget)
	{
		char	*savetarget;
		savetarget = TargetEntity->Target;
		TargetEntity->Target = TargetEntity->gameEntity->pathtarget;
		TargetEntity->UseTargets (Activator, Message);
		TargetEntity->Target = savetarget;

		// make sure we didn't get killed by a killtarget
		if (!GetInUse())
			return;
	}

	if (Wait)
	{
		if (Wait > 0)
		{
			NextThink = level.Frame + Wait;
			ThinkType = TRAINTHINK_NEXT;
		}
		else if (SpawnFlags & TRAIN_TOGGLE)  // && wait < 0
		{
			Next ();
			SpawnFlags &= ~TRAIN_START_ON;
			Velocity.Clear ();
			NextThink = 0;
		}

		if (!(Flags & FL_TEAMSLAVE))
		{
			if (SoundEnd)
				PlaySound (CHAN_NO_PHS_ADD+CHAN_VOICE, SoundEnd, 255, ATTN_STATIC);
			State.GetSound() = 0;
		}
	}
	else
		Next ();	
}

#include "cc_infoentities.h"

void CTrainBase::Next ()
{
	bool		first = true;

	CUsableEntity *ent = NULL;
	while (true)
	{
		if (!Target)
			return;

		ent = entity_cast<CUsableEntity> (CC_PickTarget (Target));
		if (!ent)
		{
			DebugPrintf ("train_next: bad target %s\n", Target);
			return;
		}

		Target = ent->Target;

		// check for a teleport path_corner
		if (ent->SpawnFlags & 1)
		{
			if (!first)
			{
				DebugPrintf ("connected teleport path_corners, see %s at (%f %f %f)\n", ent->gameEntity->classname, ent->State.GetOrigin().X, ent->State.GetOrigin().Y, ent->State.GetOrigin().Z);
				return;
			}
			first = false;
			State.GetOrigin() = (ent->State.GetOrigin() - GetMins());
			State.GetOldOrigin () = State.GetOrigin();
			State.GetEvent() = EV_OTHER_TELEPORT;
			Link ();
			continue;
		}
		break;
	}

	CPathCorner *Corner = entity_cast<CPathCorner>(ent);

	if (Corner)
		Wait = Corner->Wait;

	TargetEntity = entity_cast<CUsableEntity>(ent);

	if (!(Flags & FL_TEAMSLAVE))
	{
		if (SoundStart)
			PlaySound (CHAN_NO_PHS_ADD+CHAN_VOICE, SoundStart, 255, ATTN_STATIC);
		State.GetSound() = SoundMiddle;
	}

	MoveState = STATE_TOP;
	StartOrigin = State.GetOrigin ();
	EndOrigin = (ent->State.GetOrigin() - GetMins());
	MoveCalc (EndOrigin, TRAINENDFUNC_WAIT);

	SpawnFlags |= TRAIN_START_ON;
}

void CTrainBase::Resume ()
{
	CBaseEntity *ent = TargetEntity;

	EndOrigin = ent->State.GetOrigin() - GetMins();
	MoveState = STATE_TOP;
	StartOrigin = State.GetOrigin ();
	MoveCalc (EndOrigin, TRAINENDFUNC_WAIT);
	SpawnFlags |= TRAIN_START_ON;
}

void CTrainBase::Find ()
{
	if (!Target)
	{
		DebugPrintf ("train_find: no target\n");
		return;
	}
	CUsableEntity *ent = entity_cast<CUsableEntity>(CC_PickTarget (Target));
	if (!ent)
	{
		DebugPrintf ("train_find: target %s not found\n", Target);
		return;
	}
	Target = ent->Target;
	State.GetOrigin() = (ent->State.GetOrigin() - GetMins());

	Link ();

	// if not triggered, start immediately
	if (!TargetName)
		SpawnFlags |= TRAIN_START_ON;

	if (SpawnFlags & TRAIN_START_ON)
	{
		NextThink = level.Frame + FRAMETIME;
		ThinkType = TRAINTHINK_NEXT;
		Activator = this;
	}
}

void CTrainBase::Use (CBaseEntity *other, CBaseEntity *activator)
{
	Activator = activator;

	if (SpawnFlags & TRAIN_START_ON)
	{
		if (!(SpawnFlags & TRAIN_TOGGLE))
			return;
		SpawnFlags &= ~TRAIN_START_ON;
		Velocity.Clear ();
		NextThink = 0;
	}
	else
	{
		if (TargetEntity)
			Resume();
		else
			Next();
	}
}

void CTrainBase::DoEndFunc ()
{
	switch (EndFunc)
	{
	case TRAINENDFUNC_WAIT:
		TrainWait ();
		break;
	}
}

void CTrainBase::Think ()
{
	switch (ThinkType)
	{
	case TRAINTHINK_FIND:
		Find ();
		break;
	case TRAINTHINK_NEXT:
		Next ();
		break;
	default:
		CBrushModel::Think ();
	}
}

void CTrainBase::Spawn ()
{
};

CTrain::CTrain () :
CBaseEntity (),
CTrainBase ()
{
};

CTrain::CTrain (int Index) :
CBaseEntity (Index),
CTrainBase (Index)
{
};

void CTrain::Spawn ()
{
	PhysicsType = PHYSICS_PUSH;

	State.GetAngles().Clear ();
	if (SpawnFlags & TRAIN_BLOCK_STOPS)
		Damage = 0;
	else
	{
		if (!Damage)
			Damage = 100;
	}
	GetSolid() = SOLID_BSP;
	SetBrushModel ();

	if (NoiseIndex)
		SoundMiddle = NoiseIndex;

	if (!Speed)
		Speed = 100;

	Accel = Decel = Speed;

	Link ();

	if (Target)
	{
		// start trains on the second frame, to make sure their targets have had
		// a chance to spawn
		NextThink = level.Frame + FRAMETIME;
		ThinkType = TRAINTHINK_FIND;
	}
	else
	{
		//gi.dprintf ("func_train without a target at (%f %f %f)\n", self->absMin[0], self->absMin[1], self->absMin[2]);
		MapPrint (MAPPRINT_ERROR, this, GetAbsMin(), "No target\n");
	}
}

LINK_CLASSNAME_TO_CLASS ("func_train", CTrain);

/*QUAKED trigger_elevator (0.3 0.1 0.6) (-8 -8 -8) (8 8 8)
*/

CTriggerElevator::CTriggerElevator () :
CBaseEntity (),
CMapEntity (),
CThinkableEntity (),
CUsableEntity ()
{
};

CTriggerElevator::CTriggerElevator (int Index) :
CBaseEntity (Index),
CMapEntity (Index),
CThinkableEntity (Index),
CUsableEntity (Index)
{
};

void CTriggerElevator::Use (CBaseEntity *other, CBaseEntity *activator)
{
	if (!MoveTarget)
		return;

	if (MoveTarget->NextThink)
		return;

	if (!other->gameEntity->pathtarget)
	{
		DebugPrintf("elevator used with no pathtarget\n");
		return;
	}

	CUsableEntity *target = entity_cast<CUsableEntity>(CC_PickTarget (other->gameEntity->pathtarget));
	if (!target)
	{
		DebugPrintf("elevator used with bad pathtarget: %s\n", other->gameEntity->pathtarget);
		return;
	}

	MoveTarget->TargetEntity = target;
	MoveTarget->Resume ();
}

void CTriggerElevator::Think ()
{
	if (!Target)
	{
		//gi.dprintf("trigger_elevator has no target\n");
		MapPrint (MAPPRINT_ERROR, this, GetAbsMin(), "No target\n");
		return;
	}
	CBaseEntity *newTarg = CC_PickTarget (Target);
	if (!newTarg)
	{
		//gi.dprintf("trigger_elevator unable to find target %s\n", self->target);
		MapPrint (MAPPRINT_ERROR, this, GetAbsMin(), "Unable to find target \"%s\"\n", Target);
		return;
	}
	if (strcmp(newTarg->gameEntity->classname, "func_train") != 0)
	{
		//gi.dprintf("trigger_elevator target %s is not a train\n", self->target);
		MapPrint (MAPPRINT_ERROR, this, GetAbsMin(), "Target \"%s\" is not a train\n", Target);
		return;
	}

	MoveTarget = entity_cast<CTrain>(newTarg);
	Usable = true;
	GetSvFlags() = SVF_NOCLIENT;
}

void CTriggerElevator::Spawn ()
{
	NextThink = level.Frame + FRAMETIME;
}

LINK_CLASSNAME_TO_CLASS ("trigger_elevator", CTriggerElevator);

#pragma endregion Train

#pragma region World
#include "cc_bodyqueue.h"

CWorldEntity::CWorldEntity () : 
CBaseEntity(),
CMapEntity(),
CBrushModel()
{
};

CWorldEntity::CWorldEntity (int Index) : 
CBaseEntity(Index),
CMapEntity(Index),
CBrushModel(Index)
{
};

ENTITYFIELDS_BEGIN(CWorldEntity)
{
	CEntityField ("message",		EntityMemberOffset(CWorldEntity,Message),		FT_LEVEL_STRING),
	CEntityField ("gravity",		EntityMemberOffset(CWorldEntity,Gravity),		FT_LEVEL_STRING),
	CEntityField ("sky",			EntityMemberOffset(CWorldEntity,Sky),			FT_LEVEL_STRING),
	CEntityField ("skyrotate",		EntityMemberOffset(CWorldEntity,SkyRotate),		FT_FLOAT),
	CEntityField ("skyaxis",		EntityMemberOffset(CWorldEntity,SkyAxis),		FT_VECTOR),
	CEntityField ("nextmap",		EntityMemberOffset(CWorldEntity,NextMap),		FT_LEVEL_STRING),
};
ENTITYFIELDS_END(CWorldEntity)

bool			CWorldEntity::ParseField (const char *Key, const char *Value)
{
	if (CheckFields<CWorldEntity> (this, Key, Value))
		return true;

	// Couldn't find it here
	return CMapEntity::ParseField (Key, Value);
};

bool CWorldEntity::Run ()
{
	return CBrushModel::Run();
};

void CreateDMStatusbar ();
void CreateSPStatusbar ();
void CreateMapDebugStatusbar ();
void SetItemNames ();

void SetupLights ()
{
	static char *LightTable[] =
	{
		"m",														// 0 normal
		"mmnmmommommnonmmonqnmmo",									// 1 FLICKER (first variety)
		"abcdefghijklmnopqrstuvwxyzyxwvutsrqponmlkjihgfedcba",		// 2 SLOW STRONG PULSE
		"mmmmmaaaaammmmmaaaaaabcdefgabcdefg",						// 3 CANDLE (first variety)
		"mamamamamama",												// 4 FAST STROBE
		"jklmnopqrstuvwxyzyxwvutsrqponmlkj",						// 5 GENTLE PULSE 1
		"nmonqnmomnmomomno",										// 6 FLICKER (second variety)
		"mmmaaaabcdefgmmmmaaaammmaamm",								// 7 CANDLE (second variety)
		"mmmaaammmaaammmabcdefaaaammmmabcdefmmmaaaa",				// 8 CANDLE (third variety)
		"aaaaaaaazzzzzzzz",											// 9 SLOW STROBE (fourth variety)
		"mmamammmmammamamaaamammma",								// 10 FLUORESCENT FLICKER
		"abcdefghijklmnopqrrqponmlkjihgfedcba",						// 11 SLOW PULSE NOT FADE TO BLACK
		NULL														// END OF TABLE
	};

	for (int i = 0; ; i++)
	{
		if (!LightTable[i])
			break;

		if (i >= 32)
			DebugPrintf ("CleanCode Warning: Mod is assigning a light table value to a switchable light configstring!\n");

		ConfigString(CS_LIGHTS+i, LightTable[i]);
	}

	// styles 32-62 are assigned by the light program for switchable lights
	// 63 testing
	ConfigString(CS_LIGHTS+63, "a");
}

CBaseEntity *World;
void InitPrivateEntities ();

void CWorldEntity::Spawn ()
{
	World = this;
	ClearList(); // Do this before ANYTHING
	InvalidateItemMedia ();

	PhysicsType = PHYSICS_PUSH;
	GetSolid() = SOLID_BSP;
	GetInUse() = true;			// since the world doesn't use G_Spawn()
	State.GetModelIndex() = 1;	// world model is always index 1

	// reserve some spots for dead player bodies for coop / deathmatch
	BodyQueue_Init ();
	Init_Junk();

	if (NextMap)
		level.NextMap = NextMap;

	// make some data visible to the server
	if (Message && Message[0])
	{
		ConfigString (CS_NAME, Message);
		level.FullLevelName = Message;
	}
	else
		level.FullLevelName = level.ServerLevelName;

	ConfigString (CS_SKY, (Sky && Sky[0]) ? Sky : "unit1_");
	ConfigString (CS_SKYROTATE, Q_VarArgs ("%f", SkyRotate));

	ConfigString (CS_SKYAXIS, Q_VarArgs ("%f %f %f",
		SkyAxis.X, SkyAxis.Y, SkyAxis.Z));

	ConfigString (CS_CDTRACK, Q_VarArgs ("%i", gameEntity->sounds));

	ConfigString (CS_MAXCLIENTS, maxclients->String());

	// status bar program
	if (map_debug->Boolean())
		CreateMapDebugStatusbar();
	else if (game.mode & GAME_DEATHMATCH)
	{
#ifdef CLEANCTF_ENABLED
//ZOID
		if (game.mode & GAME_CTF)
		{
			CreateCTFStatusbar();

			//precaches
			ImageIndex("i_ctf1");
			ImageIndex("i_ctf2");
			ImageIndex("i_ctf1d");
			ImageIndex("i_ctf2d");
			ImageIndex("i_ctf1t");
			ImageIndex("i_ctf2t");
			ImageIndex("i_ctfj");
		}
		else
//ZOID
#endif
		CreateDMStatusbar();
	}
	else
		CreateSPStatusbar();

	//---------------
	SetItemNames();

	sv_gravity->Set((Gravity) ? Gravity : "800");

	SoundIndex ("player/lava1.wav");
	SoundIndex ("player/lava2.wav");

	SoundIndex ("misc/pc_up.wav");
	SoundIndex ("misc/talk1.wav");

	SoundIndex ("misc/udeath.wav");

	SoundIndex ("items/respawn1.wav");

	DoWeaponVweps ();
	//-------------------

	SoundIndex ("player/gasp1.wav");		// gasping for air
	SoundIndex ("player/gasp2.wav");		// head breaking surface, not gasping
	SoundIndex ("player/watr_in.wav");	// feet hitting water
	SoundIndex ("player/watr_out.wav");	// feet leaving water
	SoundIndex ("player/watr_un.wav");	// head going underwater
	SoundIndex ("player/u_breath1.wav");
	SoundIndex ("player/u_breath2.wav");
	SoundIndex ("world/land.wav");		// landing thud
	SoundIndex ("misc/h2ohit1.wav");		// landing splash
	SoundIndex ("weapons/noammo.wav");
	SoundIndex ("infantry/inflies1.wav");

	InitGameMedia ();

//
// Setup light animation tables. 'a' is total darkness, 'z' is doublebright.
//
	SetupLights ();

	dmFlags.UpdateFlags(dmflags->Integer());

	InitPrivateEntities ();
};

void SpawnWorld ()
{
	(entity_cast<CWorldEntity>(g_edicts[0].Entity))->ParseFields ();
	(entity_cast<CWorldEntity>(g_edicts[0].Entity))->Spawn ();
};
#pragma endregion World

#pragma region Rotating Brush
/*QUAKED func_rotating (0 .5 .8) ? START_ON REVERSE X_AXIS Y_AXIS TOUCH_PAIN STOP ANIMATED ANIMATED_FAST
You need to have an origin brush as part of this entity.  The center of that brush will be
the point around which it is rotated. It will rotate around the Z axis by default.  You can
check either the X_AXIS or Y_AXIS box to change that.

"speed" determines how fast it moves; default value is 100.
"dmg"	damage to inflict when blocked (2 default)

REVERSE will cause the it to rotate in the opposite direction.
STOP mean it will stop moving instead of pushing entities
*/

CRotatingBrush::CRotatingBrush () :
	CBaseEntity(),
	CMapEntity(),
	CBrushModel(),
	CUsableEntity(),
	CBlockableEntity(),
	CTouchableEntity()
{
	BrushType |= BRUSH_ROTATING;
};

CRotatingBrush::CRotatingBrush (int Index) :
	CBaseEntity(Index),
	CMapEntity(Index),
	CBrushModel(Index),
	CUsableEntity(Index),
	CBlockableEntity(Index),
	CTouchableEntity(Index)
{
	BrushType |= BRUSH_ROTATING;
};

bool CRotatingBrush::Run ()
{
	return CBrushModel::Run ();
};

void CRotatingBrush::Blocked (CBaseEntity *other)
{
	if (!Blockable)
		return;

	if ((other->EntityFlags & ENT_HURTABLE) && entity_cast<CHurtableEntity>(other)->CanTakeDamage)
		entity_cast<CHurtableEntity>(other)->TakeDamage (this, this, vec3fOrigin, other->State.GetOrigin(), vec3fOrigin, Damage, 1, 0, MOD_CRUSH);
}

void CRotatingBrush::Touch (CBaseEntity *other, plane_t *plane, cmBspSurface_t *surf)
{
	if ((AngularVelocity != vec3fOrigin) && ((other->EntityFlags & ENT_HURTABLE) && entity_cast<CHurtableEntity>(other)->CanTakeDamage))
		entity_cast<CHurtableEntity>(other)->TakeDamage (this, this, vec3fOrigin, other->State.GetOrigin(), vec3fOrigin, Damage, 1, 0, MOD_CRUSH);
}

void CRotatingBrush::Use (CBaseEntity *other, CBaseEntity *activator)
{
	if (AngularVelocity != vec3fOrigin)
	{
		State.GetSound() = 0;
		AngularVelocity.Clear();
		Touchable = false;
	}
	else
	{
		State.GetSound() = SoundMiddle;
		AngularVelocity = MoveDir * Speed;

		if (SpawnFlags & 16)
			Touchable = true;
	}
}

void CRotatingBrush::Spawn ()
{
	Touchable = false;

	GetSolid() = SOLID_BSP;
	if (SpawnFlags & 32)
		PhysicsType = PHYSICS_STOP;
	else
		PhysicsType = PHYSICS_PUSH;

	// set the axis of rotation
	MoveDir.Clear ();
	if (SpawnFlags & 4)
		MoveDir.Z = 0.1f;
	else if (SpawnFlags & 8)
		MoveDir.X = 0.1f;
	else // Z_AXIS
		MoveDir.Y = 0.1f;

	// check for reverse rotation
	if (SpawnFlags & 2)
		MoveDir.Invert ();

	if (!Speed)
		Speed = 100;
	if (!Damage)
		Damage = 2;

	Blockable = false;
	if (Damage)
		Blockable = true;

	if (SpawnFlags & 1)
		Use (NULL, NULL);

	if (SpawnFlags & 64)
		State.GetEffects() |= EF_ANIM_ALL;
	if (SpawnFlags & 128)
		State.GetEffects() |= EF_ANIM_ALLFAST;

	SetBrushModel ();
	Link ();
}

LINK_CLASSNAME_TO_CLASS ("func_rotating", CRotatingBrush);
#pragma endregion Rotating Brush

#pragma region Conveyor
/*QUAKED func_conveyor (0 .5 .8) ? START_ON TOGGLE
Conveyors are stationary brushes that move what's on them.
The brush should be have a surface with at least one current content enabled.
speed	default 100
*/

CConveyor::CConveyor () :
	CBaseEntity (),
	CMapEntity (),
	CBrushModel (),
	CUsableEntity ()
	{
		BrushType |= BRUSH_CONVEYOR;
	};

CConveyor::CConveyor (int Index) :
	CBaseEntity (Index),
	CMapEntity (Index),
	CBrushModel (Index),
	CUsableEntity (Index)
	{
		BrushType |= BRUSH_CONVEYOR;
	};

void CConveyor::Use (CBaseEntity *other, CBaseEntity *activator)
{
	if (SpawnFlags & 1)
	{
		Speed = 0;
		SpawnFlags &= ~1;
	}
	else
	{
		Speed = gameEntity->count;
		SpawnFlags |= 1;
	}

	if (!(SpawnFlags & 2))
		gameEntity->count = 0;
}

bool CConveyor::Run ()
{
	return CBrushModel::Run ();
}

void CConveyor::Spawn ()
{
	if (!Speed)
		Speed = 100;

	if (!(SpawnFlags & 1))
	{
		gameEntity->count = Speed;
		Speed = 0;
	}

	SetBrushModel ();
	GetSolid() = SOLID_BSP;
	Link ();
}

LINK_CLASSNAME_TO_CLASS ("func_conveyor", CConveyor);
#pragma endregion Conveyor

#pragma region Area Portal
/*QUAKED func_areaportal (0 0 0) ?

This is a non-visible object that divides the world into
areas that are seperated when this portal is not activated.
Usually enclosed in the middle of a door.
*/
CAreaPortal::CAreaPortal () :
	CBaseEntity (),
	CMapEntity (),
	CUsableEntity ()
	{
	};

CAreaPortal::CAreaPortal (int Index) :
	CBaseEntity (Index),
	CMapEntity (Index),
	CUsableEntity (Index)
	{
	};


void CAreaPortal::Use (CBaseEntity *other, CBaseEntity *activator)
{
	gameEntity->count ^= 1;		// toggle state
	gi.SetAreaPortalState (gameEntity->style, (gameEntity->count != 0));
}

bool CAreaPortal::Run ()
{
	return CBaseEntity::Run ();
}

void CAreaPortal::Spawn ()
{
	gameEntity->count = 0;		// always start closed;
}

LINK_CLASSNAME_TO_CLASS ("func_areaportal", CAreaPortal);

#pragma endregion Area Portal

#pragma region Wall
/*QUAKED func_wall (0 .5 .8) ? TRIGGER_SPAWN TOGGLE START_ON ANIMATED ANIMATED_FAST
This is just a solid wall if not inhibited

TRIGGER_SPAWN	the wall will not be present until triggered
				it will then blink in to existance; it will
				kill anything that was in it's way

TOGGLE			only valid for TRIGGER_SPAWN walls
				this allows the wall to be turned on and off

START_ON		only valid for TRIGGER_SPAWN walls
				the wall will initially be present
*/

CFuncWall::CFuncWall () :
	CBaseEntity (),
	CMapEntity (),
	CBrushModel (),
	CUsableEntity ()
	{
		BrushType |= BRUSH_WALL;
	};

CFuncWall::CFuncWall (int Index) :
	CBaseEntity (Index),
	CMapEntity (Index),
	CBrushModel (Index),
	CUsableEntity (Index)
	{
		BrushType |= BRUSH_WALL;
	};

void CFuncWall::Use (CBaseEntity *other, CBaseEntity *activator)
{
	if (!Usable)
		return;

	if (GetSolid() == SOLID_NOT)
	{
		GetSolid() = SOLID_BSP;
		GetSvFlags() &= ~SVF_NOCLIENT;
		KillBox ();
	}
	else
	{
		GetSolid() = SOLID_NOT;
		GetSvFlags() |= SVF_NOCLIENT;
	}
	Link ();

	if (!(SpawnFlags & 2))
		Usable = false;
}

bool CFuncWall::Run ()
{
	return CBrushModel::Run ();
}

void CFuncWall::Spawn ()
{
	PhysicsType = PHYSICS_PUSH;
	SetBrushModel ();

	if (SpawnFlags & 8)
		State.GetEffects() |= EF_ANIM_ALL;
	if (SpawnFlags & 16)
		State.GetEffects() |= EF_ANIM_ALLFAST;

	// just a wall
	if ((SpawnFlags & 7) == 0)
	{
		GetSolid() = SOLID_BSP;
		Link ();
		return;
	}

	// it must be TRIGGER_SPAWN
	if (!(SpawnFlags & 1))
		SpawnFlags |= 1;

	// yell if the spawnflags are odd
	if (SpawnFlags & 4)
	{
		if (!(SpawnFlags & 2))
		{
			//gi.dprintf("func_wall START_ON without TOGGLE\n");
			MapPrint (MAPPRINT_WARNING, this, GetAbsMin(), "Invalid spawnflags: START_ON without TOGGLE\n");
			SpawnFlags |= 2;
		}
	}

	GetSolid() = (SpawnFlags & 4) ? SOLID_BSP : SOLID_NOT;
	if (!(SpawnFlags & 4))
		GetSvFlags() |= SVF_NOCLIENT;

	Link ();
}

LINK_CLASSNAME_TO_CLASS ("func_wall", CFuncWall);

#pragma endregion Wall

#pragma region Object
/*QUAKED func_object (0 .5 .8) ? TRIGGER_SPAWN ANIMATED ANIMATED_FAST
This is solid bmodel that will fall if it's support it removed.
*/

CFuncObject::CFuncObject () :
	CBaseEntity (),
	CMapEntity (),
	CBrushModel (),
	CTouchableEntity (),
	CUsableEntity (),
	CTossProjectile ()
	{
		BrushType |= BRUSH_OBJECT;
	};

CFuncObject::CFuncObject (int Index) :
	CBaseEntity (Index),
	CMapEntity (Index),
	CBrushModel (Index),
	CTouchableEntity (Index),
	CUsableEntity (Index),
	CTossProjectile (Index)
	{
		BrushType |= BRUSH_OBJECT;
	};

bool CFuncObject::Run ()
{
	switch (PhysicsType)
	{
	case PHYSICS_TOSS:
		return CTossProjectile::Run ();
	default:
		return CBrushModel::Run ();
	};
};

void CFuncObject::Touch (CBaseEntity *other, plane_t *plane, cmBspSurface_t *surf)
{
	// only squash thing we fall on top of
	if (!plane)
		return;
	if (plane->normal[2] < 1.0)
		return;
	if (!(other->EntityFlags & ENT_HURTABLE))
		return;
	if (!entity_cast<CHurtableEntity>(other)->CanTakeDamage)
		return;

	entity_cast<CHurtableEntity>(other)->TakeDamage (this, this, vec3fOrigin, State.GetOrigin(), vec3fOrigin, Damage, 1, 0, MOD_CRUSH);
};

void CFuncObject::Think ()
{
	PhysicsType = PHYSICS_TOSS;
	Touchable = true;
};

void CFuncObject::Use (CBaseEntity *other, CBaseEntity *activator)
{
	GetSolid() = SOLID_BSP;
	GetSvFlags() &= ~SVF_NOCLIENT;
	Usable = false;
	KillBox ();
	Think (); // release us
}

void CFuncObject::Spawn ()
{
	Touchable = false;
	SetBrushModel ();

	GetMins() += 1;
	GetMaxs() -= 1;

	if (!Damage)
		Damage = 100;

	if (SpawnFlags == 0)
	{
		GetSolid() = SOLID_BSP;
		PhysicsType = PHYSICS_PUSH;
		NextThink = level.Frame + 2;
	}
	else
	{
		GetSolid() = SOLID_NOT;
		PhysicsType = PHYSICS_PUSH;
		Usable = true;
		GetSvFlags() |= SVF_NOCLIENT;
	}

	if (SpawnFlags & 2)
		State.GetEffects() |= EF_ANIM_ALL;
	if (SpawnFlags & 4)
		State.GetEffects() |= EF_ANIM_ALLFAST;

	GetClipmask() = CONTENTS_MASK_MONSTERSOLID;
	Link ();
}
#pragma endregion Object

#pragma region Explosive

/*QUAKED func_explosive (0 .5 .8) ? Trigger_Spawn ANIMATED ANIMATED_FAST
Any brush that you want to explode or break apart.  If you want an
ex0plosion, set dmg and it will do a radius explosion of that amount
at the center of the bursh.

If targeted it will not be shootable.

health defaults to 100.

mass defaults to 75.  This determines how much debris is emitted when
it explodes.  You get one large chunk per 100 of mass (up to 8) and
one small chunk per 25 of mass (up to 16).  So 800 gives the most->
*/
CFuncExplosive::CFuncExplosive () :
	CBaseEntity (),
	CMapEntity (),
	CBrushModel (),
	CUsableEntity (),
	CHurtableEntity (),
	UseType(FUNCEXPLOSIVE_USE_NONE),
	Explosivity (75)
	{
		BrushType |= BRUSH_EXPLOSIVE;
	};

CFuncExplosive::CFuncExplosive (int Index) :
	CBaseEntity (Index),
	CMapEntity (Index),
	CBrushModel (Index),
	CUsableEntity (Index),
	CHurtableEntity (Index),
	UseType(FUNCEXPLOSIVE_USE_NONE),
	Explosivity (75)
	{
		BrushType |= BRUSH_EXPLOSIVE;
	};

ENTITYFIELDS_BEGIN(CFuncExplosive)
{
	CEntityField ("mass", EntityMemberOffset(CFuncExplosive,Explosivity), FT_INT),
};
ENTITYFIELDS_END(CFuncExplosive)

bool			CFuncExplosive::ParseField (const char *Key, const char *Value)
{
	if (CheckFields<CFuncExplosive> (this, Key, Value))
		return true;

	return (CBrushModel::ParseField (Key, Value) || CUsableEntity::ParseField (Key, Value) || CHurtableEntity::ParseField (Key, Value) || CMapEntity::ParseField (Key, Value));
};

void CFuncExplosive::Pain (CBaseEntity *other, float kick, int damage)
{
};

void CFuncExplosive::Die (CBaseEntity *inflictor, CBaseEntity *attacker, int damage, vec3f &point)
{
	// bmodel origins are (0 0 0), we need to adjust that here
	State.GetOrigin() = (GetAbsMin() + (GetSize() * 0.5f));

	CanTakeDamage = false;

	if (Damage)
		T_RadiusDamage (this, attacker->gameEntity->Entity, Damage, NULL, Damage+40, MOD_EXPLOSIVE);

	Velocity = State.GetOrigin() - inflictor->State.GetOrigin();
	Velocity.Normalize ();
	Velocity *= 150;

	// start chunks towards the center
	//size *= 0.5f;

	if (!Explosivity)
		Explosivity = 75;

	// big chunks
	/*if (Explosivity >= 100)
	{
		int count = Explosivity / 100;
		if (count > 8)
			count = 8;
		while(count--)
			ThrowDebris (gameEntity, "models/objects/debris1/tris.md2", 1, vec3f (origin + (crand() * size)));
	}

	// small chunks
	int count = Explosivity / 25;
	if (count > 16)
		count = 16;
	while(count--)
		ThrowDebris (gameEntity, "models/objects/debris2/tris.md2", 2, vec3f (origin + (crand() * size)));*/

	UseTargets (attacker, Message);

	if (Damage)
		BecomeExplosion (true);
	else
		Free ();
}

void CFuncExplosive::Use (CBaseEntity *other, CBaseEntity *activator)
{
	switch (UseType)
	{
	case FUNCEXPLOSIVE_USE_EXPLODE:
		Die (this, other, Health, vec3fOrigin);
		break;
	case FUNCEXPLOSIVE_USE_SPAWN:
		DoSpawn ();
		break;
	default:
		break;
	}
}

void CFuncExplosive::DoSpawn ()
{
	GetSolid() = SOLID_BSP;
	GetSvFlags() &= ~SVF_NOCLIENT;
	UseType = FUNCEXPLOSIVE_USE_NONE;
	KillBox ();
	Link ();
}

bool CFuncExplosive::Run ()
{
	return CBrushModel::Run ();
}

void CFuncExplosive::Spawn ()
{
	if (game.mode & GAME_DEATHMATCH)
	{	// auto-remove for deathmatch
		Free ();
		return;
	}

	PhysicsType = PHYSICS_PUSH;

	ModelIndex ("models/objects/debris1/tris.md2");
	ModelIndex ("models/objects/debris2/tris.md2");

	if (SpawnFlags & 1)
	{
		GetSvFlags() |= SVF_NOCLIENT;
		GetSolid() = SOLID_NOT;
		UseType = FUNCEXPLOSIVE_USE_SPAWN;
	}
	else
	{
		GetSolid() = SOLID_BSP;
		if (TargetName)
			UseType = FUNCEXPLOSIVE_USE_EXPLODE;
	}

	SetBrushModel ();

	if (SpawnFlags & 2)
		State.GetEffects() |= EF_ANIM_ALL;
	if (SpawnFlags & 4)
		State.GetEffects() |= EF_ANIM_ALLFAST;

	if (UseType != FUNCEXPLOSIVE_USE_EXPLODE)
	{
		if (!Health)
			Health = 100;
		CanTakeDamage = true;
	}

	Link ();
};

LINK_CLASSNAME_TO_CLASS ("func_explosive", CFuncExplosive);

#pragma endregion Explosive

#pragma region Killbox
/*QUAKED func_killbox (1 0 0) ?
Kills everything inside when fired, irrespective of protection.
*/
CKillbox::CKillbox () :
	CBaseEntity (),
	CMapEntity (),
	CUsableEntity ()
	{
	};

CKillbox::CKillbox (int Index) :
	CBaseEntity (Index),
	CMapEntity (Index),
	CUsableEntity (Index)
	{
	};

void CKillbox::Use (CBaseEntity *other, CBaseEntity *activator)
{
	KillBox ();
}

void CKillbox::Spawn ()
{
	SetBrushModel ();
	GetSvFlags() = SVF_NOCLIENT;
}

LINK_CLASSNAME_TO_CLASS ("func_killbox", CKillbox);

#pragma endregion Killbox

class CTriggerRelay : public CMapEntity, public CUsableEntity
{
public:
	CTriggerRelay () :
	  CBaseEntity (),
	  CMapEntity (),
	  CUsableEntity ()
	  {
	  };

	CTriggerRelay (int Index) :
	  CBaseEntity (Index),
	  CMapEntity (Index),
	  CUsableEntity (Index)
	  {
	  };

	bool ParseField (const char *Key, const char *Value)
	{
		return (CUsableEntity::ParseField (Key, Value) || CMapEntity::ParseField (Key, Value));
	}

	void Use (CBaseEntity *other, CBaseEntity *activator)
	{
		UseTargets (activator, Message);
	};

	void Spawn ()
	{
	};
};

LINK_CLASSNAME_TO_CLASS ("trigger_relay", CTriggerRelay);
