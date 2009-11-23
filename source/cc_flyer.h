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
// cc_flyer.h
// Flyer Monster
//

#if !defined(__CC_FLYER_H__) || !defined(INCLUDE_GUARDS)
#define __CC_FLYER_H__

class CFlyer : public CMonster
{
public:
	MONSTER_SOUND_ENUM
	(
		SOUND_SIGHT,
		SOUND_IDLE,
		SOUND_PAIN1,
		SOUND_PAIN2,
		SOUND_SLASH,
		SOUND_SPROING,
		SOUND_DIE,

		SOUND_MAX
	);

	CFlyer (uint32 ID);

	MONSTER_SAVE_LOAD_NO_FIELDS

	void Attack ();
#if (MONSTER_SPECIFIC_FLAGS & FLYER_KNOWS_HOW_TO_DODGE)
#ifdef MONSTER_USE_ROGUE_AI
	void Duck (float eta);
	void SideStep ();
#else
	void Dodge (CBaseEntity *attacker, float eta);
#endif

	void ChooseAfterDodge ();
	void AI_Roll (float Dist);
#endif
	void Idle ();
	void Run ();
	void Sight ();
	void Stand ();
	void Walk ();
	void Melee ();

	void CheckMelee ();
	void LoopMelee ();
	void SlashLeft ();
	void SlashRight ();
	void PopBlades ();
	void FireLeft ();
	void FireRight ();
	void Fire (sint32 FlashNumber);

	void Dead ();
	void Die (CBaseEntity *inflictor, CBaseEntity *attacker, sint32 damage, vec3f &point);
	void Pain (CBaseEntity *other, float kick, sint32 damage);

	void Spawn ();
};

#else
FILE_WARNING
#endif