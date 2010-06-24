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
// cc_rogue_black_widow.h
// 
//

#if !defined(CC_GUARD_CC_ROGUE_BLACK_WIDOW_H) || !INCLUDE_GUARDS
#define CC_GUARD_CC_ROGUE_BLACK_WIDOW_H

class CWidowGib : public CGibEntity, public ITouchableEntity
{
public:
	MediaIndex		HitSound;

	CWidowGib () :
	  CGibEntity (),
	  ITouchableEntity ()
	  {
	  };

	CWidowGib (sint32 Index) :
	  IBaseEntity (Index),
	  CGibEntity (Index),
	  ITouchableEntity (Index)
	  {
	  };

	IMPLEMENT_SAVE_HEADER (CWidowGib);

	bool Run () { return CGibEntity::Run (); };

	void SaveFields (CFile &File)
	{
		File.Write<MediaIndex> (HitSound);
		CGibEntity::SaveFields (File);
		ITouchableEntity::SaveFields (File);
	}

	void LoadFields (CFile &File)
	{
		HitSound = File.Read<MediaIndex> ();
		CGibEntity::LoadFields (File);
		ITouchableEntity::LoadFields (File);
	}

	void	Touch (IBaseEntity *Other, SBSPPlane *plane, SBSPSurface *surf);

	static void Spawn (IBaseEntity *self, MediaIndex GibIndex, int damage, EGibType type, vec3f startpos, bool sized, MediaIndex hitsound, bool fade);
};

void ThrowWidowGib (IBaseEntity *self, MediaIndex GibIndex, int damage, int type);
void ThrowWidowGibLoc (IBaseEntity *self, MediaIndex GibIndex, int damage, int type, vec3f startpos, bool fade);
void ThrowWidowGibSized (IBaseEntity *self, MediaIndex GibIndex, int damage, int type, vec3f startpos, MediaIndex hitsound, bool fade);
void ThrowSmallStuff (IBaseEntity *Entity, vec3f point);
void ThrowMoreStuff (IBaseEntity *Entity, vec3f point);

class CBlackWidow : public CWidowStand
{
public:
	FrameNumber		MeleeDebounceTime;
	vec3f				BeamPos[2];

	MONSTER_SOUND_ENUM
	(
		SOUND_PAIN1,
		SOUND_PAIN2,
		SOUND_PAIN3,
		SOUND_DEATH,
		SOUND_SEARCH1,
		SOUND_TENTACLES_RETRACT,

		SOUND_MAX
	);

	CBlackWidow (uint32 ID);

	void SaveMonsterFields (CFile &File)
	{
		File.Write<FrameNumber> (MeleeDebounceTime);
		SAVE_MONSTER_SOUNDS
	}

	void LoadMonsterFields (CFile &File)
	{
		MeleeDebounceTime = File.Read<FrameNumber>();
		LOAD_MONSTER_SOUNDS
	}

	void Run ();
	void Stand ();
	void Walk ();
	void Melee ();
	void Attack ();
	bool CheckAttack ();
	void Search ();
	bool Blocked (float Dist);

	void WidowExplode ();
	static bool TongueAttackOK (vec3f &spot1, vec3f &spot2, float dist);
	void KillChildren ();
	void ReAttackBeam ();
	void AttackBeam ();
	void KeepSearching ();
	void StartSearching ();
	void WidowExplosionLeg ();
	void WidowExplosion7 ();
	void WidowExplosion6 ();
	void WidowExplosion5 ();
	void WidowExplosion4 ();
	void WidowExplosion3 ();
	void WidowExplosion2 ();
	void WidowExplosion1 ();
	void Tongue ();
	void TonguePull ();
	void Crunch ();
	void Toss ();
	void FireBeam ();
	void SaveBeamTarget ();
	void SaveDisruptLocation ();
	void FireDisrupt ();
	void DisruptReAttack ();
	void ReadySpawn ();
	void SpawnCheck ();

	void Dead ();
	void Die (IBaseEntity *Inflictor, IBaseEntity *Attacker, sint32 Damage, vec3f &Point);
	void Pain (IBaseEntity *Other, sint32 Damage);

	void Spawn ();
	
	MONSTER_ID_HEADER
};

#else
FILE_WARNING
#endif