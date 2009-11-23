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
// cc_insane.h
// Insane marine
//

#if !defined(__CC_INSANE_H__) || !defined(INCLUDE_GUARDS)
#define __CC_INSANE_H__

class CInsane : public CMonster
{
public:
	MediaIndex	SoundScream[8];

	MONSTER_SOUND_ENUM
	(
		SOUND_FIST,
		SOUND_SHAKE,
		SOUND_MOAN,

		SOUND_MAX
	)

	CInsane (uint32 ID);

	void SaveMonsterFields (CFile &File)
	{
		SAVE_MONSTER_SOUNDS
		for (uint8 i = 0; i < 8; i++)
			WriteIndex (File, SoundScream[i], INDEX_SOUND);
	}
	void LoadMonsterFields (CFile &File)
	{
		LOAD_MONSTER_SOUNDS
		for (uint8 i = 0; i < 8; i++)
			ReadIndex (File, SoundScream[i], INDEX_SOUND);
	}

	void Fist ();
	void Shake ();
	void Moan ();
	void Scream ();
	void CheckDown ();
	void OnGround ();
	void CheckUp ();
	void Cross ();

	void Run ();
	void Stand ();
	void Walk ();

	void Dead ();
	void Die (CBaseEntity *inflictor, CBaseEntity *attacker, sint32 damage, vec3f &point);
	void Pain (CBaseEntity *other, float kick, sint32 damage);

	void Spawn ();
};

#else
FILE_WARNING
#endif