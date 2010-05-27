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
// cc_medic.h
// Medic
//

#if !defined(CC_GUARD_MEDIC_H) || !INCLUDE_GUARDS
#define CC_GUARD_MEDIC_H

class CMedic : public CMonster
{
public:
	FrameNumber	MedicTryTime;
	uint8			MedicTries;

	MONSTER_SOUND_ENUM
	(
		SOUND_IDLE,
		SOUND_PAIN1,
		SOUND_PAIN2,
		SOUND_DIE,
		SOUND_SIGHT,
		SOUND_SEARCH,
		SOUND_HOOK_LAUNCH,
		SOUND_HOOK_HIT,
		SOUND_HOOK_HEAL,
		SOUND_HOOK_RETRACT,

		SOUND_MAX
	);

	CMedic (uint32 ID);

	ROGUE_VIRTUAL void SaveMonsterFields (CFile &File)
	{
		SAVE_MONSTER_SOUNDS
		File.Write<FrameNumber> (MedicTryTime);
		File.Write<uint8> (MedicTries);
	}
	ROGUE_VIRTUAL void LoadMonsterFields (CFile &File)
	{
		LOAD_MONSTER_SOUNDS
		MedicTryTime = File.Read<FrameNumber> ();
		MedicTries = File.Read<uint8> ();
	}

	ROGUE_VIRTUAL void Attack ();
	void Run ();
	void Search ();
	void Idle ();
	void Sight ();
	void Stand ();
	void Walk ();
	ROGUE_VIRTUAL bool CheckAttack ();
#if !ROGUE_FEATURES
	void Dodge (IBaseEntity *Attacker, float eta);
	void Duck_Down ();
	void Duck_Hold ();
	void Duck_Up ();
#else
	void Dodge (IBaseEntity *Attacker, float eta, CTrace *tr) { MonsterDodge (Attacker, eta, tr); };
	void Duck (float eta);
	void SideStep ();
#endif

	CMonsterEntity	*FindDeadMonster ();
	ROGUE_VIRTUAL void FireBlaster ();
	void ContinueFiring ();
	void HookLaunch ();
	void HookRetract();
	void CableAttack ();

#if ROGUE_FEATURES
	void CleanupHeal (bool ChangeFrame = false);
	void AbortHeal (bool Gib, bool Mark);
#endif

	void Dead ();
	void Die (IBaseEntity *Inflictor, IBaseEntity *Attacker, sint32 Damage, vec3f &point);
	void Pain (IBaseEntity *Other, sint32 Damage);

	ROGUE_VIRTUAL void Spawn ();
	
	MONSTER_ID_HEADER
};

#else
FILE_WARNING
#endif
