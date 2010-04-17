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
// cc_rogue_widow_stand.h
// 
//

#if !defined(CC_GUARD_CC_ROGUE_WIDOW_STAND_H) || !INCLUDE_GUARDS
#define CC_GUARD_CC_ROGUE_WIDOW_STAND_H

class CWidowStand : public CMonster
{
public:
	uint8			WidowDamageMultiplier;
	FrameNumber_t	RailDebounceTime, BlasterDebounceTime;
	uint8			FireCount;
	vec3f			RailPos;
	uint32			ShotsFired;
// self->timestamp used to prevent rapid fire of railgun
// self->plat2flags used for fire count (flashes)
// self->monsterinfo.pausetime used for timing of blaster shots

	MONSTER_SOUND_ENUM
	(
		SOUND_PAIN1,
		SOUND_PAIN2,
		SOUND_PAIN3,
		SOUND_SEARCH1,
		SOUND_RAIL,

		SOUND_MAX
	);

	CWidowStand (uint32 ID);

	virtual void SaveMonsterFields (CFile &File)
	{
		File.Write<uint8> (WidowDamageMultiplier);
		File.Write<FrameNumber_t> (RailDebounceTime);
		File.Write<FrameNumber_t> (BlasterDebounceTime);
		File.Write<uint8> (FireCount);
		File.Write<vec3f> (RailPos);
		File.Write<uint8> (ShotsFired);
		SAVE_MONSTER_SOUNDS
	}

	virtual void LoadMonsterFields (CFile &File)
	{
		WidowDamageMultiplier = File.Read<uint8> ();
		RailDebounceTime = File.Read<FrameNumber_t> ();
		BlasterDebounceTime = File.Read<FrameNumber_t> ();
		FireCount = File.Read<uint8> ();
		RailPos = File.Read<vec3f> ();
		ShotsFired = File.Read<uint8> ();
		LOAD_MONSTER_SOUNDS
	}

	virtual void Run ();
	virtual void Stand ();
	virtual void Walk ();
	virtual void Melee ();
	virtual void Attack ();
	virtual bool CheckAttack ();
	void Sight ();
	virtual void Search ();
	virtual bool Blocked (float Dist);

	void CalcSlots ();
	void Powerups ();
	void RespondPowerup (CPlayerEntity *other);
	void PowerArmor ();
	void Pent (FrameNumber_t frametime);
	void Quad (FrameNumber_t frametime);
	void Double (FrameNumber_t frametime);
	void ReAttackBlaster ();
	void AttackBlaster ();
	void FireBlaster ();
	void AttackKick ();
	void SpawnOutStart ();
	void SpawnOutDo ();
	void DoneSpawn ();
	void StartSpawn ();
	virtual void ReadySpawn ();
	virtual void SpawnCheck ();
	void AttackRail ();
	float TargetAngle ();
	void SaveLoc ();
	void FireRail ();
	void RailDone ();
	void StartRail ();
	void StartRun12 ();
	void StepShoot ();
	void Step ();
	void DoSpawn ();
	FrameNumber_t Torso ();

	virtual void Dead ();
	virtual void Die (IBaseEntity *Inflictor, IBaseEntity *Attacker, sint32 Damage, vec3f &point);
	virtual void Pain (IBaseEntity *Other, sint32 Damage);

	virtual void Spawn ();
};

#else
FILE_WARNING
#endif