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
// cc_rogue_stalker.h
// 
//

#if !defined(CC_GUARD_CC_ROGUE_STALKER_H) || !INCLUDE_GUARDS
#define CC_GUARD_CC_ROGUE_STALKER_H

#if ROGUE_FEATURES
class CStalker : public CMonster
{
public:
	FrameNumber_t TimeStamp;
	MONSTER_SOUND_ENUM
	(
		SOUND_PAIN,
		SOUND_DIE,
		SOUND_SIGHT,
		SOUND_PUNCH_HIT1,
		SOUND_PUNCH_HIT2,
		SOUND_IDLE,

		SOUND_MAX
	);

	CStalker (uint32 ID);

	MONSTER_SAVE_LOAD_NO_FIELDS

	void Run ();
	void Walk ();
	void Sight ();
	void Stand ();
	void Idle ();
	void Attack ();
	void Melee ();

	bool DoPounce (vec3f Dest);
	void IdleNoise ();
	void DodgeJump ();
	void SwingCheckL ();
	void SwingCheckR ();
	void SwingAttack ();
	void JumpStraightUp ();
	void JumpWaitLand ();
	void FalseDeath ();
	void FalseDeathStart ();
	bool OKToTransition ();
	void Reactivate ();
	void Heal ();
	void ShootAttack ();
	void ShootAttack2 ();
	bool CheckLZ (CBaseEntity *target, vec3f dest);
		
	void Dodge (CBaseEntity *Attacker, float eta
#if MONSTER_USE_ROGUE_AI
		, CTrace *tr
#endif
		);

	inline bool OnCeiling ()
	{
		return Entity->GravityVector[2] > 0 ? 1 : 0;
	}

	// FIXME: this may be only used for stalker, but I should
	// do this for every monster
	inline bool HasValidEnemy ()
	{
		if (!Entity->Enemy)
			return false;

		if (!Entity->Enemy->GetInUse())
			return false;

		if (!(Entity->Enemy->EntityFlags & ENT_HURTABLE))
			return false;

		if (entity_cast<CHurtableEntity>(Entity->Enemy)->Health < 1)
			return false;

		return true;
	}

	void Dead ();
	void Die (CBaseEntity *Inflictor, CBaseEntity *Attacker, sint32 Damage, vec3f &point);
	void Pain (CBaseEntity *Other, sint32 Damage);

	void Spawn ();
};
#endif

#else
FILE_WARNING
#endif