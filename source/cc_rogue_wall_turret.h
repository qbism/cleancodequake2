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
// cc_rogue_wall_turret.h
// 
//

#if !defined(CC_GUARD_CC_ROGUE_WALL_TURRET_H) || !INCLUDE_GUARDS
#define CC_GUARD_CC_ROGUE_WALL_TURRET_H

#if ROGUE_FEATURES

CC_ENUM (uint8, ETurretMoveEnum)
{
	TURRETTHINK_MOVEDONE,
	TURRETTHINK_MOVEFINAL,
	TURRETTHINK_MOVEBEGIN
};

class CWallTurret : public CMonster
{
public:
	bool		TurretEnabled;
	vec3f		Offset;

	// Move stuff
	vec3f			Dir;
	float			RemainingDistance;
	ETurretMoveEnum	ThinkType;

	CWallTurret (uint32 ID);

	void SaveMonsterFields (CFile &File)
	{
		File.Write<bool> (TurretEnabled);
		File.Write<vec3f> (Offset);
	}

	void LoadMonsterFields (CFile &File)
	{
		TurretEnabled = File.Read<bool> ();
		Offset = File.Read<vec3f> ();
	}

	void DamageEffect (vec3f &dir, vec3f &point, vec3f &normal, sint32 &damage, EDamageFlags &dflags, EMeansOfDeath &mod);

	void Run ();
	void Walk ();
	void Stand ();
	void Attack ();
	bool CheckAttack ();

	void MoveCalc (vec3f dest);
	void MoveBegin ();
	void MoveFinal ();
	void MoveDone ();
	void MoveThink ();

	void Wake ();
	void FireBlind ();
	void Fire ();
	void ReadyGun ();
	void Aim ();

	void Use (IBaseEntity *Other, IBaseEntity *Activator);
	void Dead ();
	void Die (IBaseEntity *Inflictor, IBaseEntity *Attacker, sint32 Damage, vec3f &point);
	void Pain (IBaseEntity *Other, sint32 Damage);

	void Spawn ();
	
	MONSTER_ID_HEADER
};
#endif

#else
FILE_WARNING
#endif