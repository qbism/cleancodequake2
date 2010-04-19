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
// cc_rogue_widow_stand.cpp
// 
//

#include "cc_local.h"

#if ROGUE_FEATURES

#include "cc_rogue_widow_stand.h"
#include "cc_rogue_monster_spawning.h"
#include "cc_tent.h"
#include "m_widow.h"
#include "cc_rogue_black_widow.h"

#define	MAX_LEGSFRAME	23
#define	LEG_WAIT_TIME	10

class CWidowLegs : public IThinkableEntity
{
public:
	FrameNumber_t		BoomTime;
	bool				Exploded;

	CWidowLegs() :
	  IThinkableEntity()
	  {
	  };

	CWidowLegs (sint32 Index) :
	  IBaseEntity (Index),
	  IThinkableEntity (Index)
	  {
	  };

	IMPLEMENT_SAVE_HEADER (CWidowLegs);
	
	void SaveFields (CFile &File)
	{
		File.Write<FrameNumber_t> (BoomTime);
		File.Write<bool> (Exploded);
		IThinkableEntity::SaveFields (File);
	}

	void LoadFields (CFile &File)
	{
		BoomTime = File.Read<FrameNumber_t> ();
		Exploded = File.Read<bool> ();
		IThinkableEntity::LoadFields (File);
	}

	void Think ()
	{
		vec3f f, r, u;
		State.GetAngles().ToVectors (&f, &r, &u);

		if (State.GetFrame() == 17)
		{
			static const vec3f offset (11.77f, -7.24f, 23.31f);
			vec3f point;

			G_ProjectSource (State.GetOrigin(), offset, f, r, point, u);
			CRocketExplosion(CTempEntFlags(CAST_MULTI, CASTFLAG_NONE, point), point).Send();
			//ThrowSmallStuff (self, point);
		}

		if (State.GetFrame() < MAX_LEGSFRAME)
		{
			State.GetFrame()++;
			NextThink = Level.Frame + FRAMETIME;
			return;
		}
		else if (BoomTime == 0)
			BoomTime = Level.Frame + LEG_WAIT_TIME;

		if (Level.Frame > BoomTime)
		{
			static const vec3f offset (-65.6f, -8.44f, 28.59f);
			vec3f point;

			G_ProjectSource (State.GetOrigin(), offset, f, r, point, u);
			CRocketExplosion(CTempEntFlags(CAST_MULTI, CASTFLAG_NONE, point), point).Send();
			ThrowSmallStuff (this, point);

			ThrowWidowGibSized (this, ModelIndex("models/monsters/blackwidow/gib1/tris.md2"), 80 + (int)(frand()*20.0), GIB_METALLIC, point, 0, true);
			ThrowWidowGibSized (this, ModelIndex("models/monsters/blackwidow/gib2/tris.md2"), 80 + (int)(frand()*20.0), GIB_METALLIC, point, 0, true);

			static const vec3f offset2 (-1.04f, -51.18f, 7.04f);
			G_ProjectSource (State.GetOrigin(), offset2, f, r, point, u);
			CRocketExplosion(CTempEntFlags(CAST_MULTI, CASTFLAG_NONE, point), point).Send();
			ThrowSmallStuff (this, point);

			ThrowWidowGibSized (this, ModelIndex("models/monsters/blackwidow/gib1/tris.md2"), 80 + (int)(frand()*20.0), GIB_METALLIC, point, 0, true);
			ThrowWidowGibSized (this, ModelIndex("models/monsters/blackwidow/gib2/tris.md2"), 80 + (int)(frand()*20.0), GIB_METALLIC, point, 0, true);
			ThrowWidowGibSized (this, ModelIndex("models/monsters/blackwidow/gib3/tris.md2"), 80 + (int)(frand()*20.0), GIB_METALLIC, point, 0, true);

			Free ();
			return;
		}

		if ((Level.Frame > (BoomTime - 5)) && (!Exploded))
		{
			Exploded = true;

			static const vec3f offset (31, -88.7f, 10.96f);
			vec3f point;
			G_ProjectSource (State.GetOrigin(), offset, f, r, point, u);
			CRocketExplosion(CTempEntFlags(CAST_MULTI, CASTFLAG_NONE, point), point).Send();

			static const vec3f offset2 (-12.67f, -4.39f, 15.68f);
			G_ProjectSource (State.GetOrigin(), offset2, f, r, point, u);
			CRocketExplosion(CTempEntFlags(CAST_MULTI, CASTFLAG_NONE, point), point).Send();

			NextThink = Level.Frame + FRAMETIME;
			return;
		}
		NextThink = Level.Frame + FRAMETIME;
	};

	static void Spawn (vec3f &origin, vec3f &angles)
	{
		CWidowLegs *Legs = QNewEntityOf CWidowLegs;

		Legs->State.GetOrigin() = origin;
		Legs->State.GetAngles() = angles;
		Legs->GetSolid() = SOLID_NOT;
		Legs->State.GetRenderEffects() = RF_IR_VISIBLE;

		Legs->State.GetModelIndex() = ModelIndex("models/monsters/legs/tris.md2");

		Legs->BoomTime = 0;
		Legs->NextThink = Level.Frame + FRAMETIME;
		Legs->Link ();
	};
};

IMPLEMENT_SAVE_SOURCE (CWidowLegs);

CWidowStand::CWidowStand (uint32 ID) :
CMonster(ID)
{
	Scale = MODEL_SCALE;
	MonsterName = "Black Widow";
};

#define	NUM_STALKERS_SPAWNED		6		// max # of stalkers she can spawn

#define	RAIL_TIME					30
#define	BLASTER_TIME				20
#define	BLASTER2_DAMAGE				10
#define	WIDOW_RAIL_DAMAGE			50

vec3f spawnpoints[] = {
	vec3f(30,  100, 16),
	vec3f(30, -100, 16)
};

vec3f beameffects[] = {
	vec3f(12.58f, -43.71f, 68.88f),
	vec3f(3.43f, 58.72f, 68.41f)
};

float sweep_angles[] = {
	32.0, 26.0, 20.0, 10.0, 0.0, -6.5f, -13.0, -27.0, -41.0
};

vec3f stalker_mins (-28, -28, -18);
vec3f stalker_maxs (28, 28, 18);

void CWidowStand::Search ()
{
}

void CWidowStand::Sight ()
{
	BlasterDebounceTime = 0;
}

float CWidowStand::TargetAngle ()
{
	float enemy_yaw = (Entity->State.GetOrigin() - Entity->Enemy->State.GetOrigin()).ToYaw();
	if (enemy_yaw < 0)
		enemy_yaw += 360.0;
	enemy_yaw -= 180.0;

	return enemy_yaw;
}

CFrame WidowFramesAttackPreBlaster [] =
{
	CFrame (&CMonster::AI_Charge,	0),
	CFrame (&CMonster::AI_Charge,	0),
	CFrame (&CMonster::AI_Charge,	0,	ConvertDerivedFunction(&CWidowStand::AttackBlaster))
};
CAnim WidowMoveAttackPreBlaster (FRAME_fired01, FRAME_fired02a, WidowFramesAttackPreBlaster);

// Loop this
CFrame WidowFramesAttackBlaster [] =
{
	CFrame (&CMonster::AI_Charge,	0,	ConvertDerivedFunction(&CWidowStand::ReAttackBlaster)),		// straight ahead
	CFrame (&CMonster::AI_Charge,	0,	ConvertDerivedFunction(&CWidowStand::ReAttackBlaster)),		// 100 degrees right
	CFrame (&CMonster::AI_Charge,	0,	ConvertDerivedFunction(&CWidowStand::ReAttackBlaster)),
	CFrame (&CMonster::AI_Charge,	0,	ConvertDerivedFunction(&CWidowStand::ReAttackBlaster)),
	CFrame (&CMonster::AI_Charge,	0,	ConvertDerivedFunction(&CWidowStand::ReAttackBlaster)),
	CFrame (&CMonster::AI_Charge,	0,	ConvertDerivedFunction(&CWidowStand::ReAttackBlaster)),
	CFrame (&CMonster::AI_Charge,	0,	ConvertDerivedFunction(&CWidowStand::ReAttackBlaster)),		// 50 degrees right
	CFrame (&CMonster::AI_Charge,	0,	ConvertDerivedFunction(&CWidowStand::ReAttackBlaster)),
	CFrame (&CMonster::AI_Charge,	0,	ConvertDerivedFunction(&CWidowStand::ReAttackBlaster)),
	CFrame (&CMonster::AI_Charge,	0,	ConvertDerivedFunction(&CWidowStand::ReAttackBlaster)),
	CFrame (&CMonster::AI_Charge,	0,	ConvertDerivedFunction(&CWidowStand::ReAttackBlaster)),
	CFrame (&CMonster::AI_Charge,	0,	ConvertDerivedFunction(&CWidowStand::ReAttackBlaster)),		// straight
	CFrame (&CMonster::AI_Charge,	0,	ConvertDerivedFunction(&CWidowStand::ReAttackBlaster)),
	CFrame (&CMonster::AI_Charge,	0,	ConvertDerivedFunction(&CWidowStand::ReAttackBlaster)),
	CFrame (&CMonster::AI_Charge,	0,	ConvertDerivedFunction(&CWidowStand::ReAttackBlaster)),
	CFrame (&CMonster::AI_Charge,	0,	ConvertDerivedFunction(&CWidowStand::ReAttackBlaster)),
	CFrame (&CMonster::AI_Charge,	0,	ConvertDerivedFunction(&CWidowStand::ReAttackBlaster)),		// 50 degrees left
	CFrame (&CMonster::AI_Charge,	0,	ConvertDerivedFunction(&CWidowStand::ReAttackBlaster)),
	CFrame (&CMonster::AI_Charge,	0,	ConvertDerivedFunction(&CWidowStand::ReAttackBlaster))		// 70 degrees left
};
CAnim WidowMoveAttackBlaster (FRAME_fired02a, FRAME_fired20, WidowFramesAttackBlaster);

CFrame WidowFramesAttackPostBlaster [] =
{
	CFrame (&CMonster::AI_Charge,	0),
	CFrame (&CMonster::AI_Charge,	0)
};
CAnim WidowMoveAttackPostBlaster (FRAME_fired21, FRAME_fired22, WidowFramesAttackPostBlaster, &CMonster::Run);

CFrame WidowFramesAttackPostBlasterR [] =
{
	CFrame (&CMonster::AI_Charge,	-2),
	CFrame (&CMonster::AI_Charge,	-10),
	CFrame (&CMonster::AI_Charge,	-2),
	CFrame (&CMonster::AI_Charge,	0),
	CFrame (&CMonster::AI_Charge,	0,	ConvertDerivedFunction(&CWidowStand::StartRun12))
};
CAnim WidowMoveAttackPostBlasterR (FRAME_transa01, FRAME_transa05, WidowFramesAttackPostBlasterR);

CFrame WidowFramesAttackPostBlasterL [] =
{
	CFrame (&CMonster::AI_Charge,	0),
	CFrame (&CMonster::AI_Charge,	14),
	CFrame (&CMonster::AI_Charge,	-2),
	CFrame (&CMonster::AI_Charge,	10),
	CFrame (&CMonster::AI_Charge,	10,	ConvertDerivedFunction(&CWidowStand::StartRun12))
};
CAnim WidowMoveAttackPostBlasterL (FRAME_transb01, FRAME_transb05, WidowFramesAttackPostBlasterL);

int CWidowStand::Torso ()
{
	float enemy_yaw = TargetAngle ();

	if (enemy_yaw >= 105)
	{
		CurrentMove = &WidowMoveAttackPostBlasterR;
		AIFlags &= ~AI_MANUAL_STEERING;
		return 0;
	}

	if (enemy_yaw <= -75.0)
	{
		CurrentMove = &WidowMoveAttackPostBlasterL;
		AIFlags &= ~AI_MANUAL_STEERING;
		return 0;
	}

	if (enemy_yaw >= 95)
		return FRAME_fired03;
	else if (enemy_yaw >= 85)
		return FRAME_fired04;
	else if (enemy_yaw >= 75)
		return FRAME_fired05;
	else if (enemy_yaw >= 65)
		return FRAME_fired06;
	else if (enemy_yaw >= 55)
		return FRAME_fired07;
	else if (enemy_yaw >= 45)
		return FRAME_fired08;
	else if (enemy_yaw >= 35)
		return FRAME_fired09;
	else if (enemy_yaw >= 25)
		return FRAME_fired10;
	else if (enemy_yaw >= 15)
		return FRAME_fired11;
	else if (enemy_yaw >= 5)
		return FRAME_fired12;
	else if (enemy_yaw >= -5)
		return FRAME_fired13;
	else if (enemy_yaw >= -15)
		return FRAME_fired14;
	else if (enemy_yaw >= -25)
		return FRAME_fired15;
	else if (enemy_yaw >= -35)
		return FRAME_fired16;
	else if (enemy_yaw >= -45)
		return FRAME_fired17;
	else if (enemy_yaw >= -55)
		return FRAME_fired18;
	else if (enemy_yaw >= -65)
		return FRAME_fired19;
	else if (enemy_yaw >= -75)
		return FRAME_fired20;
	return 0;
}

#define	VARIANCE 15.0

void CWidowStand::FireBlaster ()
{
	if (!Entity->Enemy)
		return;

	ShotsFired++;
	EEntityStateEffects effect = (!(ShotsFired % 4)) ? EF_BLASTER : 0;

	vec3f forward, right, start;
	Entity->State.GetAngles().ToVectors (&forward, &right, NULL);
	if ((Entity->State.GetFrame() >= FRAME_spawn05) && (Entity->State.GetFrame() <= FRAME_spawn13))
	{
		// sweep
		EMuzzleFlash flashnum = MZ2_WIDOW_BLASTER_SWEEP1 + Entity->State.GetFrame() - FRAME_spawn05;
		G_ProjectSource (Entity->State.GetOrigin(), MonsterFlashOffsets[flashnum], forward, right, start);
		
		vec3f targ_angles = (Entity->Enemy->State.GetOrigin() - start).ToAngles();
		vec3f vec = Entity->State.GetAngles();
		vec.X += targ_angles.X;
		vec.Y -= sweep_angles[flashnum-MZ2_WIDOW_BLASTER_SWEEP1];

		vec.ToVectors (&forward, NULL, NULL);
		MonsterFireBlaster2 (start, forward, BLASTER2_DAMAGE*WidowDamageMultiplier, 1000, flashnum, effect);
	}
	else if ((Entity->State.GetFrame() >= FRAME_fired02a) && (Entity->State.GetFrame() <= FRAME_fired20))
	{
		AIFlags |= AI_MANUAL_STEERING;

		NextFrame = Torso ();

		if (!NextFrame)
			NextFrame = Entity->State.GetFrame();

		EMuzzleFlash flashnum;
		if (Entity->State.GetFrame() == FRAME_fired02a)
			flashnum = MZ2_WIDOW_BLASTER_0;
		else
			flashnum = MZ2_WIDOW_BLASTER_100 + Entity->State.GetFrame() - FRAME_fired03;

		G_ProjectSource (Entity->State.GetOrigin(), MonsterFlashOffsets[flashnum], forward, right, start);

		PredictAim (Entity->Enemy, start, 1000, true, ((frand()*0.1f)-0.05f), &forward, NULL);

		// clamp it to within 10 degrees of the aiming angle (where she's facing)
		vec3f angles = forward.ToAngles();
		// give me 100 -> -70
		float aim_angle = 100 - (10*(flashnum-MZ2_WIDOW_BLASTER_100));
		if (aim_angle <= 0)
			aim_angle += 360;
		float  target_angle = Entity->State.GetAngles().Y - angles.Y;
		if (target_angle <= 0)
			target_angle += 360;

		float error = aim_angle - target_angle;

		// positive error is to entity's left, aka positive direction in engine
		// unfortunately, I decided that for the aim_angle, positive was right.  *sigh*
		if (error > VARIANCE)
			angles.Y = (Entity->State.GetAngles().Y - aim_angle) + VARIANCE;
		else if (error < -VARIANCE)
			angles.Y = (Entity->State.GetAngles().Y - aim_angle) - VARIANCE;
		angles.ToVectors (&forward, NULL, NULL);

		MonsterFireBlaster2 (start, forward, BLASTER2_DAMAGE*WidowDamageMultiplier, 1000, flashnum, effect);
	}
	else if ((Entity->State.GetFrame() >= FRAME_run01) && (Entity->State.GetFrame() <= FRAME_run08))
	{
		EMuzzleFlash flashnum = MZ2_WIDOW_RUN_1 + Entity->State.GetFrame() - FRAME_run01;
		G_ProjectSource (Entity->State.GetOrigin(), MonsterFlashOffsets[flashnum], forward, right, start);

		MonsterFireBlaster2 (start, Entity->Enemy->State.GetOrigin() - start + vec3f(0, 0, Entity->Enemy->ViewHeight), BLASTER2_DAMAGE*WidowDamageMultiplier, 1000, flashnum, effect);
	}
}	

void CWidowStand::DoSpawn ()
{
	vec3f	f, r, u;

	Entity->State.GetAngles().ToVectors (&f, &r, &u);

	for (uint8 i = 0; i < 2; i++)
	{
		vec3f offset = spawnpoints[i], startpoint, spawnpoint;
		G_ProjectSource (Entity->State.GetOrigin(), offset, f, r, startpoint, u);

		if (FindSpawnPoint (startpoint, stalker_mins, stalker_maxs, spawnpoint, 64))
		{
			CMonsterEntity *ent = CreateGroundMonster (spawnpoint, Entity->State.GetAngles(), stalker_mins, stalker_maxs, "monster_stalker", 256);

			if (!ent)
				continue;
			
			MonsterUsed++;
			ent->Monster->Commander = Entity;

			ent->NextThink = Level.Frame;
			void	(CMonster::*TheThink) () = ent->Monster->Think;
			(ent->Monster->*TheThink) ();
			
			ent->Monster->AIFlags |= AI_SPAWNED_WIDOW|AI_DO_NOT_COUNT|AI_IGNORE_SHOTS;

			IBaseEntity *designated_enemy = Entity->Enemy;
			if (Game.GameMode & GAME_COOPERATIVE)
			{
				designated_enemy = PickCoopTarget(ent);
				if (designated_enemy)
				{
					// try to avoid using my enemy
					if (designated_enemy == Entity->Enemy)
					{
						designated_enemy = PickCoopTarget(ent);
						if (!designated_enemy)
							designated_enemy = Entity->Enemy;
					}
				}
				else
					designated_enemy = Entity->Enemy;
			}

			if ((designated_enemy->GetInUse()) && ((designated_enemy->EntityFlags & ENT_HURTABLE) && entity_cast<IHurtableEntity>(designated_enemy)->Health > 0))
			{
				ent->Enemy = designated_enemy;
				ent->Monster->FoundTarget ();
				ent->Monster->Attack();
			}
		}
	}
}

void CWidowStand::SpawnCheck ()
{
	FireBlaster ();
	DoSpawn ();
}

void CWidowStand::ReadySpawn ()
{
	vec3f	f, r, u;

	FireBlaster();
	Entity->State.GetAngles().ToVectors (&f, &r, &u);

	for (uint8 i = 0; i < 2; i++)
	{
		vec3f offset = spawnpoints[i], startpoint, spawnpoint;

		G_ProjectSource (Entity->State.GetOrigin(), offset, f, r, startpoint, u);
		if (FindSpawnPoint (startpoint, stalker_mins, stalker_maxs, spawnpoint, 64))
			CSpawnGrow::Spawn (spawnpoint, 1);
	}
}

void CWidowStand::Step ()
{
	Entity->PlaySound (CHAN_BODY, SoundIndex("widow/bwstep3.wav"));
}

CFrame WidowFramesStand [] =
{
	CFrame (&CMonster::AI_Stand, 0),
	CFrame (&CMonster::AI_Stand, 0),
	CFrame (&CMonster::AI_Stand, 0),
	CFrame (&CMonster::AI_Stand, 0),
	CFrame (&CMonster::AI_Stand, 0),
	CFrame (&CMonster::AI_Stand, 0),
	CFrame (&CMonster::AI_Stand, 0),
	CFrame (&CMonster::AI_Stand, 0),
	CFrame (&CMonster::AI_Stand, 0),
	CFrame (&CMonster::AI_Stand, 0),
	CFrame (&CMonster::AI_Stand, 0)
};
CAnim	WidowMoveStand (FRAME_idle01, FRAME_idle11, WidowFramesStand);

CFrame WidowFramesWalk [] =
{
	// auto generated numbers
	CFrame (&CMonster::AI_Walk,	2.79f,	ConvertDerivedFunction(&CWidowStand::Step)),
	CFrame (&CMonster::AI_Walk,	2.77f),
	CFrame (&CMonster::AI_Walk,	3.53f),
	CFrame (&CMonster::AI_Walk,	3.97f),
	CFrame (&CMonster::AI_Walk,	4.13f),			//5
	CFrame (&CMonster::AI_Walk,	4.09f),
	CFrame (&CMonster::AI_Walk,	3.84f),
	CFrame (&CMonster::AI_Walk,	3.62f,	ConvertDerivedFunction(&CWidowStand::Step)),
	CFrame (&CMonster::AI_Walk,	3.29f),
	CFrame (&CMonster::AI_Walk,	6.08f),			//10
	CFrame (&CMonster::AI_Walk,	6.94f),
	CFrame (&CMonster::AI_Walk,	5.73f),
	CFrame (&CMonster::AI_Walk,	2.85f)
};
CAnim WidowMoveWalk (FRAME_walk01, FRAME_walk13, WidowFramesWalk);

CFrame WidowFramesRun [] =
{
	CFrame (&CMonster::AI_Run,	2.79f,	ConvertDerivedFunction(&CWidowStand::Step)),
	CFrame (&CMonster::AI_Run,	2.77f),
	CFrame (&CMonster::AI_Run,	3.53f),
	CFrame (&CMonster::AI_Run,	3.97f),
	CFrame (&CMonster::AI_Run,	4.13f),			//5
	CFrame (&CMonster::AI_Run,	4.09f),
	CFrame (&CMonster::AI_Run,	3.84f),
	CFrame (&CMonster::AI_Run,	3.62f,	ConvertDerivedFunction(&CWidowStand::Step)),
	CFrame (&CMonster::AI_Run,	3.29f),
	CFrame (&CMonster::AI_Run,	6.08f),			//10
	CFrame (&CMonster::AI_Run,	6.94f),
	CFrame (&CMonster::AI_Run,	5.73f),
	CFrame (&CMonster::AI_Run,	2.85f)
};
CAnim WidowMoveRun (FRAME_walk01, FRAME_walk13, WidowFramesRun);

void CWidowStand::StepShoot ()
{
	Entity->PlaySound (CHAN_BODY, SoundIndex("widow/bwstep2.wav"));
	FireBlaster ();
}

CFrame WidowFramesRunAttack [] =
{
	CFrame (&CMonster::AI_Charge,	13,	ConvertDerivedFunction(&CWidowStand::StepShoot)),
	CFrame (&CMonster::AI_Charge,	11.72f,	ConvertDerivedFunction(&CWidowStand::FireBlaster)),
	CFrame (&CMonster::AI_Charge,	18.04f,	ConvertDerivedFunction(&CWidowStand::FireBlaster)),
	CFrame (&CMonster::AI_Charge,	14.58f,	ConvertDerivedFunction(&CWidowStand::FireBlaster)),
	CFrame (&CMonster::AI_Charge,	13,	ConvertDerivedFunction(&CWidowStand::StepShoot)),			//5
	CFrame (&CMonster::AI_Charge,	12.12f,	ConvertDerivedFunction(&CWidowStand::FireBlaster)),
	CFrame (&CMonster::AI_Charge,	19.63f,	ConvertDerivedFunction(&CWidowStand::FireBlaster)),
	CFrame (&CMonster::AI_Charge,	11.37f,	ConvertDerivedFunction(&CWidowStand::FireBlaster))
};
CAnim WidowMoveRunAttack (FRAME_run01, FRAME_run08, WidowFramesRunAttack, &CMonster::Run);

//
// These three allow specific entry into the run sequence
//

void CWidowStand::StartRun12 ()
{
	CurrentMove = &WidowMoveRun;
	NextFrame = FRAME_walk12;
}

CFrame WidowFramesAttackPreRail [] =
{
	CFrame (&CMonster::AI_Charge,	0,	ConvertDerivedFunction(&CWidowStand::StartRail)),
	CFrame (&CMonster::AI_Charge,	0),
	CFrame (&CMonster::AI_Charge,	0),
	CFrame (&CMonster::AI_Charge,	0,	ConvertDerivedFunction(&CWidowStand::AttackRail))
};
CAnim WidowMoveAttackPreRail (FRAME_transc01, FRAME_transc04, WidowFramesAttackPreRail);

CFrame WidowFramesAttackRail [] =
{
	CFrame (&CMonster::AI_Charge, 0),
	CFrame (&CMonster::AI_Charge, 0),
	CFrame (&CMonster::AI_Charge, 0, ConvertDerivedFunction(&CWidowStand::SaveLoc)),
	CFrame (&CMonster::AI_Charge, -10, ConvertDerivedFunction(&CWidowStand::FireRail)),
	CFrame (&CMonster::AI_Charge, 0),
	CFrame (&CMonster::AI_Charge, 0),
	CFrame (&CMonster::AI_Charge, 0),
	CFrame (&CMonster::AI_Charge, 0),
	CFrame (&CMonster::AI_Charge, 0, ConvertDerivedFunction(&CWidowStand::RailDone))
};
CAnim WidowMoveAttackRail (FRAME_firea01, FRAME_firea09, WidowFramesAttackRail, &CMonster::Run);

CFrame WidowFramesAttackRailR [] =
{
	CFrame (&CMonster::AI_Charge, 0),
	CFrame (&CMonster::AI_Charge, 0),
	CFrame (&CMonster::AI_Charge, 0, ConvertDerivedFunction(&CWidowStand::SaveLoc)),
	CFrame (&CMonster::AI_Charge, -10, ConvertDerivedFunction(&CWidowStand::FireRail)),
	CFrame (&CMonster::AI_Charge, 0),
	CFrame (&CMonster::AI_Charge, 0),
	CFrame (&CMonster::AI_Charge, 0),
	CFrame (&CMonster::AI_Charge, 0),
	CFrame (&CMonster::AI_Charge, 0, ConvertDerivedFunction(&CWidowStand::RailDone))
};
CAnim WidowMoveAttackRailR (FRAME_fireb01, FRAME_fireb09, WidowFramesAttackRailR, &CMonster::Run);

CFrame WidowFramesAttackRailL [] =
{
	CFrame (&CMonster::AI_Charge, 0),
	CFrame (&CMonster::AI_Charge, 0),
	CFrame (&CMonster::AI_Charge, 0, ConvertDerivedFunction(&CWidowStand::SaveLoc)),
	CFrame (&CMonster::AI_Charge, -10, ConvertDerivedFunction(&CWidowStand::FireRail)),
	CFrame (&CMonster::AI_Charge, 0),
	CFrame (&CMonster::AI_Charge, 0),
	CFrame (&CMonster::AI_Charge, 0),
	CFrame (&CMonster::AI_Charge, 0),
	CFrame (&CMonster::AI_Charge, 0, ConvertDerivedFunction(&CWidowStand::RailDone))
};
CAnim WidowMoveAttackRailL (FRAME_firec01, FRAME_firec09, WidowFramesAttackRailL, &CMonster::Run);

void CWidowStand::FireRail ()
{
	vec3f	start, forward, right;

	Entity->State.GetAngles().ToVectors (&forward, &right, NULL);

	EMuzzleFlash flash = (CurrentMove == &WidowMoveAttackRailL) ? MZ2_WIDOW_RAIL_LEFT : (CurrentMove == &WidowMoveAttackRailR) ? MZ2_WIDOW_RAIL_RIGHT : MZ2_WIDOW_RAIL;

	G_ProjectSource (Entity->State.GetOrigin(), MonsterFlashOffsets[flash], forward, right, start);

	// calc direction to where we targeted
	vec3f dir = (RailPos - start).GetNormalized();

	MonsterFireRailgun (start, dir, WIDOW_RAIL_DAMAGE*WidowDamageMultiplier, 100, flash);
	RailDebounceTime = Level.Frame + RAIL_TIME;
}

void CWidowStand::SaveLoc ()
{
	RailPos = Entity->Enemy->State.GetOrigin() + vec3f(0, 0, Entity->Enemy->ViewHeight); //save for aiming the shot
};

void CWidowStand::StartRail ()
{
	AIFlags |= AI_MANUAL_STEERING;
}

void CWidowStand::RailDone ()
{
	AIFlags &= ~AI_MANUAL_STEERING;
}

void CWidowStand::AttackRail ()
{
	float enemy_angle = TargetAngle ();

	if (enemy_angle < -15)
		CurrentMove = &WidowMoveAttackRailL;
	else if (enemy_angle > 15)
		CurrentMove = &WidowMoveAttackRailR;
	else
		CurrentMove = &WidowMoveAttackRail;
}

void CWidowStand::StartSpawn ()
{
	AIFlags |= AI_MANUAL_STEERING;
}

void CWidowStand::DoneSpawn ()
{
	AIFlags &= ~AI_MANUAL_STEERING;
}

CFrame WidowFramesSpawn [] =
{
	CFrame (&CMonster::AI_Charge,	0),						//1
	CFrame (&CMonster::AI_Charge,	0),
	CFrame (&CMonster::AI_Charge,	0),
	CFrame (&CMonster::AI_Charge,	0,	ConvertDerivedFunction(&CWidowStand::StartSpawn)),
	CFrame (&CMonster::AI_Charge,	0),						//5
	CFrame (&CMonster::AI_Charge,	0,	ConvertDerivedFunction(&CWidowStand::FireBlaster)),				//6
	CFrame (&CMonster::AI_Charge,	0,	ConvertDerivedFunction(&CWidowStand::ReadySpawn)),			//7
	CFrame (&CMonster::AI_Charge,	0,	ConvertDerivedFunction(&CWidowStand::FireBlaster)),
	CFrame (&CMonster::AI_Charge,	0,	ConvertDerivedFunction(&CWidowStand::FireBlaster)),				//9
	CFrame (&CMonster::AI_Charge,	0,	ConvertDerivedFunction(&CWidowStand::SpawnCheck)),
	CFrame (&CMonster::AI_Charge,	0,	ConvertDerivedFunction(&CWidowStand::FireBlaster)),				//11
	CFrame (&CMonster::AI_Charge,	0,	ConvertDerivedFunction(&CWidowStand::FireBlaster)),
	CFrame (&CMonster::AI_Charge,	0,	ConvertDerivedFunction(&CWidowStand::FireBlaster)),				//13
	CFrame (&CMonster::AI_Charge,	0),
	CFrame (&CMonster::AI_Charge,	0),
	CFrame (&CMonster::AI_Charge,	0),
	CFrame (&CMonster::AI_Charge,	0),
	CFrame (&CMonster::AI_Charge,	0,	ConvertDerivedFunction(&CWidowStand::DoneSpawn))
};
CAnim WidowMoveSpawn (FRAME_spawn01, FRAME_spawn18, WidowFramesSpawn, &CMonster::Run);

CFrame WidowFramesPainHeavy [] =
{
	CFrame (&CMonster::AI_Move,	0),
	CFrame (&CMonster::AI_Move,	0),
	CFrame (&CMonster::AI_Move,	0),
	CFrame (&CMonster::AI_Move,	0),
	CFrame (&CMonster::AI_Move,	0),
	CFrame (&CMonster::AI_Move,	0),
	CFrame (&CMonster::AI_Move,	0),
	CFrame (&CMonster::AI_Move,	0),
	CFrame (&CMonster::AI_Move,	0),
	CFrame (&CMonster::AI_Move,	0),
	CFrame (&CMonster::AI_Move,	0),
	CFrame (&CMonster::AI_Move,	0),
	CFrame (&CMonster::AI_Move,	0)
};
CAnim WidowMovePainHeavy (FRAME_pain01, FRAME_pain13, WidowFramesPainHeavy, &CMonster::Run);

CFrame WidowFramesPainLight [] =
{
	CFrame (&CMonster::AI_Move,	0),
	CFrame (&CMonster::AI_Move,	0),
	CFrame (&CMonster::AI_Move,	0)
};
CAnim WidowMovePainLight (FRAME_pain201, FRAME_pain203, WidowFramesPainLight, &CMonster::Run);

void CWidowStand::SpawnOutStart ()
{
	vec3f startpoint, f, r, u;

	Entity->State.GetAngles().ToVectors (&f, &r, &u);

	G_ProjectSource (Entity->State.GetOrigin(), beameffects[0], f, r, startpoint, u);
	CWidowBeamOut(CTempEntFlags(CAST_MULTI, CASTFLAG_NONE, startpoint), startpoint, 20001).Send();

	G_ProjectSource (Entity->State.GetOrigin(), beameffects[1], f, r, startpoint, u);
	CWidowBeamOut(CTempEntFlags(CAST_MULTI, CASTFLAG_NONE, startpoint), startpoint, 20002).Send();

	Entity->PlaySound (CHAN_VOICE, SoundIndex ("misc/bwidowbeamout.wav"));
}

void CWidowStand::SpawnOutDo ()
{
	vec3f startpoint, f, r, u;

	Entity->State.GetAngles().ToVectors (&f, &r, &u);
	G_ProjectSource (Entity->State.GetOrigin(), beameffects[0], f, r, startpoint, u);
	CWidowSplash (CTempEntFlags(CAST_MULTI, CASTFLAG_NONE, startpoint), startpoint).Send();

	G_ProjectSource (Entity->State.GetOrigin(), beameffects[1], f, r, startpoint, u);
	CWidowSplash (CTempEntFlags(CAST_MULTI, CASTFLAG_NONE, startpoint), startpoint).Send();

	startpoint = Entity->State.GetOrigin() + vec3f(0, 0, 36);
	CBossTeleport (startpoint).Send();

	CWidowLegs::Spawn (Entity->State.GetOrigin(), Entity->State.GetAngles());
	
	Entity->Free ();
}

CFrame WidowFramesDeath [] =
{
	CFrame (&CMonster::AI_Move,	0),
	CFrame (&CMonster::AI_Move,	0),
	CFrame (&CMonster::AI_Move,	0),
	CFrame (&CMonster::AI_Move,	0),
	CFrame (&CMonster::AI_Move,	0),		//5
	CFrame (&CMonster::AI_Move,	0),
	CFrame (&CMonster::AI_Move,	0),
	CFrame (&CMonster::AI_Move,	0),
	CFrame (&CMonster::AI_Move,	0),
	CFrame (&CMonster::AI_Move,	0,	ConvertDerivedFunction(&CWidowStand::SpawnOutStart)),	//10
	CFrame (&CMonster::AI_Move,	0),		
	CFrame (&CMonster::AI_Move,	0),		
	CFrame (&CMonster::AI_Move,	0),		
	CFrame (&CMonster::AI_Move,	0),		
	CFrame (&CMonster::AI_Move,	0),				//15	
	CFrame (&CMonster::AI_Move,	0),		
	CFrame (&CMonster::AI_Move,	0),		
	CFrame (&CMonster::AI_Move,	0),		
	CFrame (&CMonster::AI_Move,	0),		
	CFrame (&CMonster::AI_Move,	0),				//20	
	CFrame (&CMonster::AI_Move,	0),		
	CFrame (&CMonster::AI_Move,	0),		
	CFrame (&CMonster::AI_Move,	0),		
	CFrame (&CMonster::AI_Move,	0),		
	CFrame (&CMonster::AI_Move,	0),				//25
	CFrame (&CMonster::AI_Move,	0),		
	CFrame (&CMonster::AI_Move,	0),		
	CFrame (&CMonster::AI_Move,	0),		
	CFrame (&CMonster::AI_Move,	0),		
	CFrame (&CMonster::AI_Move,	0),				//30	
	CFrame (&CMonster::AI_Move,	0,	ConvertDerivedFunction(&CWidowStand::SpawnOutDo))
};
CAnim WidowMoveDeath (FRAME_death01, FRAME_death31, WidowFramesDeath);

void CWidowStand::AttackKick ()
{
	static const vec3f aim (100, 0, 4);

	if (Entity->Enemy->GroundEntity)
		CMeleeWeapon::Fire (Entity, aim, (50 + (randomMT() % 6)), 500);
	else	// not as much kick if they're in the air .. makes it harder to land on her head
		CMeleeWeapon::Fire (Entity, aim, (50 + (randomMT() % 6)), 250);
}

CFrame WidowFramesAttackKick [] =
{
	CFrame (&CMonster::AI_Move, 0),
	CFrame (&CMonster::AI_Move, 0),
	CFrame (&CMonster::AI_Move, 0),
	CFrame (&CMonster::AI_Move, 0, ConvertDerivedFunction(&CWidowStand::AttackKick)),
	CFrame (&CMonster::AI_Move, 0),				// 5
	CFrame (&CMonster::AI_Move, 0),
	CFrame (&CMonster::AI_Move, 0),
	CFrame (&CMonster::AI_Move, 0)
};
CAnim WidowMoveAttackKick (FRAME_kick01, FRAME_kick08, WidowFramesAttackKick, &CMonster::Run);

void CWidowStand::Stand ()
{
	Entity->PlaySound (CHAN_WEAPON, SoundIndex ("widow/laugh.wav"));
	CurrentMove = &WidowMoveStand;
}

void CWidowStand::Run ()
{
	AIFlags &= ~AI_HOLD_FRAME;

	if (AIFlags & AI_STAND_GROUND)
		CurrentMove = &WidowMoveStand;
	else
		CurrentMove = &WidowMoveRun;
}

void CWidowStand::Walk ()
{
	CurrentMove = &WidowMoveWalk;
}

void CWidowStand::Attack ()
{
	bool rail_frames = false, blaster_frames = false, blocked = false, anger = false;

	Entity->MoveTarget = NULL;

	if (AIFlags & AI_BLOCKED)
	{
		blocked = true;
		AIFlags &= ~AI_BLOCKED;
	}
	
	if (AIFlags & AI_TARGET_ANGER)
	{
		anger = true;
		AIFlags &= ~AI_TARGET_ANGER;
	}

	if ((!Entity->Enemy) || (!Entity->Enemy->GetInUse()))
		return;

	if (BadArea)
	{
		if ((frand() < 0.1) || (Level.Frame < RailDebounceTime))
			CurrentMove = &WidowMoveAttackPreBlaster;
		else
		{
			Entity->PlaySound (CHAN_WEAPON, Sounds[SOUND_RAIL]);
			CurrentMove = &WidowMoveAttackPreRail;
		}
		return;
	}

	// frames FRAME_walk13, FRAME_walk01, FRAME_walk02, FRAME_walk03 are rail gun start frames
	// frames FRAME_walk09, FRAME_walk10, FRAME_walk11, FRAME_walk12 are spawn & blaster start frames

	if ((Entity->State.GetFrame() == FRAME_walk13) || ((Entity->State.GetFrame() >= FRAME_walk01) && (Entity->State.GetFrame() <= FRAME_walk03)))
		rail_frames = true;

	if ((Entity->State.GetFrame() >= FRAME_walk09) && (Entity->State.GetFrame() <= FRAME_walk12))
		blaster_frames = true;

	CalcSlots();

	// if we can't see the target, spawn stuff regardless of frame
	if ((AttackState == AS_BLIND) && (SlotsLeft() >= 2))
	{
		CurrentMove = &WidowMoveSpawn;
		return;
	}

	// accept bias towards spawning regardless of frame
	if (blocked && (SlotsLeft() >= 2))
	{
		CurrentMove = &WidowMoveSpawn;
		return;
	}

	if ((RangeFrom(Entity->State.GetOrigin(), Entity->Enemy->State.GetOrigin()) > 300) && (!anger) && (frand() < 0.5)  && (!blocked))
	{
		CurrentMove = &WidowMoveRunAttack;
		return;
	}

	if (blaster_frames)
	{
		if (SlotsLeft() >= 2)
		{
			CurrentMove = &WidowMoveSpawn;
			return;
		}
		else if ((BlasterDebounceTime + BLASTER_TIME) <= Level.Frame)
		{
			CurrentMove = &WidowMoveAttackPreBlaster;
			return;
		}
	}

	if (rail_frames)
	{
		if (!(Level.Frame < RailDebounceTime))
		{
			Entity->PlaySound (CHAN_WEAPON, Sounds[SOUND_RAIL]);
			CurrentMove = &WidowMoveAttackPreRail;
		}
	}

	if ((rail_frames) || (blaster_frames))
		return;

	float luck = frand();
	if (SlotsLeft() >= 2)
	{
		if ((luck <= 0.40) && ((BlasterDebounceTime + BLASTER_TIME) <= Level.Frame))
			CurrentMove = &WidowMoveAttackPreBlaster;
		else if ((luck <= 0.7) && !(Level.Frame < RailDebounceTime))
		{
			Entity->PlaySound (CHAN_WEAPON, Sounds[SOUND_RAIL]);
			CurrentMove = &WidowMoveAttackPreRail;
		}
		else
			CurrentMove = &WidowMoveSpawn;
	}
	else
	{
		if (Level.Frame < RailDebounceTime)
			CurrentMove = &WidowMoveAttackPreBlaster;
		else if ((luck <= 0.50) || ((Level.Frame + BLASTER_TIME) >= BlasterDebounceTime))
		{
			Entity->PlaySound (CHAN_WEAPON, Sounds[SOUND_RAIL]);
			CurrentMove = &WidowMoveAttackPreRail;
		}
		else // holdout to blaster
			CurrentMove = &WidowMoveAttackPreBlaster;
	}
}

void CWidowStand::AttackBlaster ()
{
	BlasterDebounceTime = Level.Frame + 10 + (20*frand());
	CurrentMove = &WidowMoveAttackBlaster;
	NextFrame = Torso ();
}

void CWidowStand::ReAttackBlaster ()
{
	FireBlaster ();

	// if WidowBlaster bailed us out of the frames, just bail
	if ((CurrentMove == &WidowMoveAttackPostBlasterL) ||
		(CurrentMove == &WidowMoveAttackPostBlasterR))
		return;

	// if we're not done with the attack, don't leave the sequence
	if (BlasterDebounceTime >= Level.Frame)
		return;

	AIFlags &= ~AI_MANUAL_STEERING;

	CurrentMove = &WidowMoveAttackPostBlaster;
}

void CWidowStand::Pain (IBaseEntity *Other, sint32 Damage)
{
	if (Entity->Health < (Entity->MaxHealth / 2))
		Entity->State.GetSkinNum() = 1;

	if (CvarList[CV_SKILL].Integer() == 3)
		return;		// no pain anims in nightmare

	if (Level.Frame < PainDebounceTime)
		return;

	PainDebounceTime = Level.Frame + 5;

	if (Damage < 15)
		Entity->PlaySound (CHAN_VOICE, Sounds[SOUND_PAIN1]);
	else if (Damage < 75)
	{
		if ((CvarList[CV_SKILL].Integer() < 3) && (frand() < (0.6f - (0.2f*(CvarList[CV_SKILL].Float())))))
		{
			CurrentMove = &WidowMovePainLight;
			AIFlags &= ~AI_MANUAL_STEERING;
		}
		Entity->PlaySound (CHAN_VOICE, Sounds[SOUND_PAIN2]);
	}
	else 
	{
		if ((CvarList[CV_SKILL].Integer() < 3) && (frand() < (0.75f - (0.1f*(CvarList[CV_SKILL].Float())))))
		{
			CurrentMove = &WidowMovePainHeavy;
			AIFlags &= ~AI_MANUAL_STEERING;
		}
		Entity->PlaySound (CHAN_VOICE, Sounds[SOUND_PAIN3]);
	}
}

void CWidowStand::Dead ()
{
	Entity->GetMins().Set (-56, -56, 0);
	Entity->GetMaxs().Set (56, 56, 80);
	Entity->PhysicsType = PHYSICS_TOSS;
	Entity->GetSvFlags() |= SVF_DEADMONSTER;
	Entity->NextThink = 0;
	Entity->Link ();
}

void CWidowStand::Die (IBaseEntity *Inflictor, IBaseEntity *Attacker, sint32 Damage, vec3f &point)
{
	Entity->DeadFlag = true;
	Entity->CanTakeDamage = false;
	QuadFramenum = DoubleFramenum = InvincibleFramenum = 0;
	CurrentMove = &WidowMoveDeath;
}

void CWidowStand::Melee ()
{
	CurrentMove = &WidowMoveAttackKick;
}

void CWidowStand::Quad (FrameNumber_t framenum)
{
	QuadFramenum = framenum;
	WidowDamageMultiplier = 4;
}

void CWidowStand::Double (FrameNumber_t framenum)
{
	DoubleFramenum = framenum;
	WidowDamageMultiplier = 2;
}

void CWidowStand::Pent (FrameNumber_t framenum)
{
	InvincibleFramenum = framenum;
}

void CWidowStand::PowerArmor ()
{
	PowerArmorType = POWER_ARMOR_SHIELD;

	// I don't like this, but it works
	if (PowerArmorPower <= 0)
		PowerArmorPower += 250 * CvarList[CV_SKILL].Integer();
}

void CWidowStand::RespondPowerup (CPlayerEntity *other)
{
	if (other->State.GetEffects() & EF_QUAD)
	{
		switch (CvarList[CV_SKILL].Integer())
		{
		case 1:
			Double (other->Client.Timers.QuadDamage);
			break;
		case 2:
			Quad (other->Client.Timers.QuadDamage);
			break;
		case 3:
			Quad (other->Client.Timers.QuadDamage);
			PowerArmor ();
			break;
		};
	}
	else if (other->State.GetEffects() & EF_DOUBLE)
	{
		switch (CvarList[CV_SKILL].Integer())
		{
		case 2:
			Double (other->Client.Timers.Double);
			break;
		case 3:
			Double (other->Client.Timers.Double);
			PowerArmor ();
			break;
		};
	}
	else
		WidowDamageMultiplier = 1;

	if (other->State.GetEffects() & EF_PENT)
	{
		switch (CvarList[CV_SKILL].Integer())
		{
		case 1:
			PowerArmor ();
			break;
		case 2:
			Pent (other->Client.Timers.Invincibility);
			break;
		case 3:
			Pent (other->Client.Timers.Invincibility);
			PowerArmor ();
			break;
		};
	}
}

void CWidowStand::Powerups ()
{
	if (!(Game.GameMode & GAME_COOPERATIVE))
		RespondPowerup (entity_cast<CPlayerEntity>(Entity->Enemy));
	else
	{
		bool FoundPent = false, FoundQuad = false, FoundDouble = false;

		// in coop, check for pents, then quads, then doubles
		for (int player = 1; player <= Game.MaxClients; player++)
		{
			CPlayerEntity *ent = entity_cast<CPlayerEntity>(Game.Entities[player].Entity);
			if (!ent->GetInUse())
				continue;

			if (!FoundPent && (ent->State.GetEffects() & EF_PENT))
			{
				RespondPowerup (ent);
				FoundPent = true;
			}

			if (!FoundQuad && (ent->State.GetEffects() & EF_QUAD))
			{
				RespondPowerup (ent);
				FoundQuad = true;
			}

			if (!FoundDouble && (ent->State.GetEffects() & EF_DOUBLE))
			{
				RespondPowerup (ent);
				FoundDouble = true;
			}
		}
	}
}

bool CWidowStand::CheckAttack ()
{
	if (!Entity->Enemy)
		return false;

	Powerups ();

	if (CurrentMove == &WidowMoveRun)
	{
		// if we're in run, make sure we're in a good frame for attacking before doing anything else
		// frames 1,2,3,9,10,11,13 good to fire
		switch (Entity->State.GetFrame())
		{
			case FRAME_walk04:
			case FRAME_walk05:
			case FRAME_walk06:
			case FRAME_walk07:
			case FRAME_walk08:
			case FRAME_walk12:
					return false;
			default:
				break;
		}
	}

	// give a LARGE bias to spawning things when we have room
	// use AI_BLOCKED as a signal to attack to spawn
	if ((frand() < 0.8) && (SlotsLeft() >= 2) && (RangeFrom(Entity->State.GetOrigin(), Entity->Enemy->State.GetOrigin()) > 150))
	{
		AIFlags |= AI_BLOCKED;
		AttackState = AS_MISSILE;
		return true;
	}

	if ((Entity->Enemy->EntityFlags & ENT_HURTABLE) && entity_cast<IHurtableEntity>(Entity->Enemy)->Health > 0)
	{
	// see if any entities are in the way of the shot
		vec3f	spot1 = Entity->State.GetOrigin() + vec3f(0, 0, Entity->ViewHeight),
				spot2 = Entity->Enemy->State.GetOrigin() + vec3f(0, 0, Entity->Enemy->ViewHeight);

		CTrace tr (spot1, spot2, Entity, CONTENTS_SOLID|CONTENTS_MONSTER|CONTENTS_SLIME|CONTENTS_LAVA);

		// do we have a clear shot?
		if (tr.Ent != Entity->Enemy)
		{	
			// go ahead and spawn stuff if we're mad a client
			if ((Entity->Enemy->EntityFlags & ENT_PLAYER) && SlotsLeft() >= 2)
			{
				AttackState = AS_BLIND;
				return true;
			}
				
			// PGM - we want them to go ahead and shoot at info_notnulls if they can.
			if (Entity->Enemy->GetSolid() != SOLID_NOT || tr.fraction < 1.0)		//PGM
				return false;
		}
	}
	
	EnemyInfront = IsInFront(Entity, Entity->Enemy);

	EnemyRange = Range(Entity, Entity->Enemy);
	IdealYaw = (Entity->Enemy->State.GetOrigin() - Entity->State.GetOrigin()).ToYaw();

	float real_enemy_range = RangeFrom(Entity->State.GetOrigin(), Entity->Enemy->State.GetOrigin());

	if (real_enemy_range <= (MELEE_DISTANCE+20))
	{
		// don't always melee in easy mode
		if (CvarList[CV_SKILL].Integer() == 0 && (randomMT()&3) )
			return false;

		AttackState = AS_MELEE;
		return true;
	}

	if (Level.Frame < AttackFinished)
		return false;
		
	float chance = 0.0f;

	if (AIFlags & AI_STAND_GROUND)
		chance = 0.4f;
	else 
	{
		switch (EnemyRange)
		{
		case RANGE_MELEE:
			chance = 0.8f;
			break;
		case RANGE_NEAR:
			chance = 0.7f;
			break;
		case RANGE_MID:
			chance = 0.6f;
			break;
		case RANGE_FAR:
			chance = 0.5f;
			break;
		};
	}

	// PGM - go ahead and shoot every time if it's a info_notnull
	if ((frand () < chance) || (Entity->Enemy->GetSolid() == SOLID_NOT))
	{
		AttackState = AS_MISSILE;
		return true;
	}

	return false;
}

bool CWidowStand::Blocked (float Dist)
{
	// if we get blocked while we're in our run/attack mode, turn on a meaningless (in this context)AI flag, 
	// and call attack to get a new attack sequence.  make sure to turn it off when we're done.
	//
	// I'm using AI_TARGET_ANGER for this purpose
	if (CurrentMove == &WidowMoveRunAttack)
	{
		AIFlags |= AI_TARGET_ANGER;
		if (CheckAttack())
			Attack();
		else
			Run();
		return true;
	}

	return false;
}

void CWidowStand::CalcSlots ()
{
	switch (CvarList[CV_SKILL].Integer())
	{
		case 0:
		case 1:
			MonsterSlots = 3;
			break;
		case 2:
			MonsterSlots = 4;
			break;
		case 3:
			MonsterSlots = 6;
			break;
		default:
			MonsterSlots = 3;
			break;
	}
	if (Game.GameMode & GAME_COOPERATIVE)
		MonsterSlots = Min<> (6, MonsterSlots + ((CvarList[CV_SKILL].Integer())*(CountPlayers()-1)));
}

/*QUAKED monster_widow (1 .5 0) (-40 -40 0) (40 40 144) Ambush Trigger_Spawn Sight
*/
void CWidowStand::Spawn ()
{
	Sounds[SOUND_PAIN1] = SoundIndex ("widow/bw1pain1.wav");
	Sounds[SOUND_PAIN2] = SoundIndex ("widow/bw1pain2.wav");
	Sounds[SOUND_PAIN3] = SoundIndex ("widow/bw1pain3.wav");
	Sounds[SOUND_SEARCH1] = SoundIndex ("bosshovr/bhvunqv1.wav");
	Sounds[SOUND_RAIL] = SoundIndex ("gladiator/railgun.wav");

	Entity->PhysicsType = PHYSICS_STEP;
	Entity->GetSolid() = SOLID_BBOX;
	Entity->State.GetModelIndex() = ModelIndex ("models/monsters/blackwidow/tris.md2");
	Entity->GetMins().Set (-40, -40, 0);
	Entity->GetMaxs().Set (40, 40, 144);

	Entity->Health = 2000 + 1000*(CvarList[CV_SKILL].Integer());
	if (Game.GameMode & GAME_COOPERATIVE)
		Entity->Health += 500*(CvarList[CV_SKILL].Integer());

	Entity->GibHealth = -5000;
	Entity->Mass = 1500;

	if (CvarList[CV_SKILL].Integer() == 3)
	{
		PowerArmorType = POWER_ARMOR_SHIELD;
		PowerArmorPower = 500;
	}

	YawSpeed = 30;
	
	Entity->Flags |= FL_IMMUNE_LASER;
	AIFlags |= AI_IGNORE_SHOTS;

	AIFlags |= (MF_HAS_ATTACK | MF_HAS_MELEE | MF_HAS_SIGHT | MF_HAS_SEARCH);
	
	Entity->Link ();

	CurrentMove = &WidowMoveStand;	

	// cache in all of the stalker stuff, widow stuff, spawngro stuff, gibs
	SoundIndex ("stalker/pain.wav");	
	SoundIndex ("stalker/death.wav");	
	SoundIndex ("stalker/sight.wav");
	SoundIndex ("stalker/melee1.wav");
	SoundIndex ("stalker/melee2.wav");
	SoundIndex ("stalker/idle.wav");

	SoundIndex ("tank/tnkatck3.wav");
	ModelIndex ("models/proj/laser2/tris.md2");

	ModelIndex ("models/monsters/stalker/tris.md2");
	ModelIndex ("models/items/spawngro2/tris.md2");
	ModelIndex ("models/objects/gibs/sm_metal/tris.md2");
	ModelIndex ("models/objects/gibs/gear/tris.md2");
	ModelIndex ("models/monsters/blackwidow/gib1/tris.md2");
	ModelIndex ("models/monsters/blackwidow/gib2/tris.md2");
	ModelIndex ("models/monsters/blackwidow/gib3/tris.md2");
	ModelIndex ("models/monsters/blackwidow/gib4/tris.md2");
	ModelIndex ("models/monsters/blackwidow2/gib1/tris.md2");
	ModelIndex ("models/monsters/blackwidow2/gib2/tris.md2");
	ModelIndex ("models/monsters/blackwidow2/gib3/tris.md2");
	ModelIndex ("models/monsters/blackwidow2/gib4/tris.md2");
	ModelIndex ("models/monsters/legs/tris.md2");
	SoundIndex ("misc/bwidowbeamout.wav");

	SoundIndex ("misc/bigtele.wav");
	SoundIndex ("widow/bwstep3.wav");
	SoundIndex ("widow/bwstep2.wav");

	CalcSlots();
	WidowDamageMultiplier = 1;

	WalkMonsterStart ();
}

LINK_MONSTER_CLASSNAME_TO_CLASS ("monster_widow", CWidowStand);

#endif