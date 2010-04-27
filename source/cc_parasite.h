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
// cc_parasite.h
// Parasite
//

#if !defined(CC_GUARD_PARASITE_H) || !INCLUDE_GUARDS
#define CC_GUARD_PARASITE_H

class CParasite : public CMonster
{
public:
	MONSTER_SOUND_ENUM
	(
		SOUND_PAIN1,
		SOUND_PAIN2,
		SOUND_DIE,
		SOUND_LAUNCH,
		SOUND_IMPACT,
		SOUND_SUCK,
		SOUND_REELIN,
		SOUND_SIGHT,
		SOUND_TAP,
		SOUND_SCRATCH,
		SOUND_SEARCH,

		SOUND_MAX
	);

	CParasite (uint32 ID);

	MONSTER_SAVE_LOAD_NO_FIELDS

	void Run ();
	void Walk ();
	void Sight ();
	void Stand ();
	void Idle ();
	void Attack ();
	bool CheckAttack ();

	void Launch ();
	void ReelIn ();
	void Tap ();
	void Scratch ();
	void DoFidget();
	void ReFidget();
	void DoWalk ();
	void DoRun ();
	void StartRun ();
	bool DrainAttackOK (vec3f &start, vec3f &end);
	void DrainAttack ();

	void Dead ();
	void Die (IBaseEntity *Inflictor, IBaseEntity *Attacker, sint32 Damage, vec3f &point);
	void Pain (IBaseEntity *Other, sint32 Damage);

	void Spawn ();
	
	MONSTER_ID_HEADER
};

#else
FILE_WARNING
#endif
