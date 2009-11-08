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

#if !defined(__CC_SOLDIER_BASE_H__) || !defined(INCLUDE_GUARDS)
#define __CC_SOLDIER_BASE_H__

#include "m_soldier.h"

class CSoldierBase : public CMonster
{
public:
	MediaIndex	SoundIdle;
	MediaIndex	SoundSight1;
	MediaIndex	SoundSight2;
	MediaIndex	SoundCock;

	MediaIndex	SoundPain;
	MediaIndex	SoundDeath;

	CSoldierBase (uint32 ID);

	virtual void Attack () = 0;
#ifndef MONSTER_USE_ROGUE_AI
	void Dodge (CBaseEntity *attacker, float eta);
#endif
	void Idle ();
	void Run ();
	void Sight ();
	void Stand ();
	void Walk ();

	void CockGun ();
#ifndef MONSTER_USE_ROGUE_AI
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
#ifdef MONSTER_USE_ROGUE_AI
	void Attack6_RefireBlaster();
#endif

	void Dead ();
	void Die (CBaseEntity *inflictor, CBaseEntity *attacker, sint32 damage, vec3f &point);
	void Pain (CBaseEntity *other, float kick, sint32 damage);

	void Spawn (); // Initialize "commonalities"
	virtual void SpawnSoldier () = 0; // Initialize health, etc.
};

#else
FILE_WARNING
#endif