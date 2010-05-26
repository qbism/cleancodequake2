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
// cc_soldier_base.h
// The base for soldier-like monsters.
//

#if !defined(CC_GUARD_SOLDIER_BASE_H) || !INCLUDE_GUARDS
#define CC_GUARD_SOLDIER_BASE_H

#include "m_soldier.h"

class CSoldierBase : public CMonster
{
public:
	MONSTER_SOUND_ENUM
	(
		SOUND_IDLE,
		SOUND_SIGHT1,
		SOUND_SIGHT2,
		SOUND_COCK,
		SOUND_PAIN,
		SOUND_DEATH,

		SOUND_MAX
	);

	/**
	\typedef	uint8 ESoldierAIType
	
	\brief	Defines an alias representing soldier AI weapon types.
	**/
	typedef uint8 ESoldierAIType;

	/**
	\enum	
	
	\brief	Values that represent soldier AI weapon types. 
	**/
	enum
	{
		AI_BLASTER,			// Regular firing, run-fire
		AI_MACHINEGUN,		// Special fire frame, no running
		AI_SHOTGUN,			// Regular firing, no run fire
	};

	ESoldierAIType		SoldierAI;

	CSoldierBase (uint32 ID);

	void SaveMonsterFields (CFile &File)
	{ 
		SAVE_MONSTER_SOUNDS
		File.Write<ESoldierAIType> (SoldierAI);
	}
	void LoadMonsterFields (CFile &File)
	{
		LOAD_MONSTER_SOUNDS
		SoldierAI = File.Read<ESoldierAIType> ();
	}

	virtual void Attack () = 0;
#if !ROGUE_FEATURES
	void Dodge (IBaseEntity *Attacker, float eta);
#else
	void Dodge (IBaseEntity *Attacker, float eta, CTrace *tr) { MonsterDodge (Attacker, eta, tr); };
#endif
	void Idle ();
	void Run ();
	void Sight ();
	void Stand ();
	void Walk ();

	void CockGun ();
#if !ROGUE_FEATURES
	void Duck_Down ();
	void Duck_Hold ();
	void Duck_Up ();
#else
	void StartCharge ();
	void StopCharge();

	void SideStep ();
	void Duck (float eta);
#endif
	virtual void FireGun (sint32 FlashNumber) = 0;

	void Walk1_Random ();

	void Fire1 ();
	void Fire2 ();
	void Fire3 ();
	void Fire4 ();
	void Fire7 ();
	void Fire6 ();
	void Fire8 ();
	void Attack1_Refire1 ();
	void Attack1_Refire2 ();
	void Attack2_Refire1 ();
	void Attack2_Refire2 ();
	void Attack3_Refire ();
	void Attack6_Refire ();
#if ROGUE_FEATURES
	void Attack6_RefireBlaster();
#endif

	void Dead ();
	void Die (IBaseEntity *Inflictor, IBaseEntity *Attacker, sint32 Damage, vec3f &point);
	void Pain (IBaseEntity *Other, sint32 Damage);

	void Spawn (); // Initialize "commonalities"
	virtual void SpawnSoldier () = 0; // Initialize health, etc.
};

#else
FILE_WARNING
#endif
