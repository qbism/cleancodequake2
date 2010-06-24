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
// cc_mutant.cpp
// Mutant
//

#if !defined(CC_GUARD_MUTANT_H) || !INCLUDE_GUARDS
#define CC_GUARD_MUTANT_H

class CMutant : public CMonster
{
public:
	bool		AttemptJumpToLastSight;
	bool		Jumping;

	MONSTER_SOUND_ENUM
	(
		SOUND_SWING,
		SOUND_HIT1,
		SOUND_HIT2,
		SOUND_DEATH,
		SOUND_IDLE,
		SOUND_PAIN1,
		SOUND_PAIN2,
		SOUND_SIGHT,
		SOUND_SEARCH,
		SOUND_STEP1,
		SOUND_STEP2,
		SOUND_STEP3,
		SOUND_THUD,

		SOUND_MAX
	);

	CMutant (uint32 ID);

	void SaveMonsterFields (CFile &File)
	{
		SAVE_MONSTER_SOUNDS
		File.Write<bool> (AttemptJumpToLastSight);
		File.Write<bool> (Jumping);
	}
	void LoadMonsterFields (CFile &File)
	{
		LOAD_MONSTER_SOUNDS
		AttemptJumpToLastSight = File.Read<bool> ();
		Jumping = File.Read<bool> ();
	}

	void Step ();
	void Swing ();
	void IdleLoop ();
	void WalkLoop ();
	void HitLeft ();
	void HitRight ();
	void CheckRefire ();
	void JumpTakeOff ();
	void CheckLanding ();
	bool CheckMelee ();
	bool CheckJump ();
	
	void Touch (IBaseEntity *Other, SBSPPlane *plane, SBSPSurface *surf);

	void Attack ();
	void Melee ();
	void Run ();
	void Sight ();
	void Stand ();
	void Walk ();
	void Search ();
	void Idle ();

	bool CheckAttack ();

	void Dead ();
	void Die (IBaseEntity *Inflictor, IBaseEntity *Attacker, sint32 Damage, vec3f &Point);
	void Pain (IBaseEntity *Other, sint32 Damage);

	void Spawn ();
	
	MONSTER_ID_HEADER
};

#else
FILE_WARNING
#endif
