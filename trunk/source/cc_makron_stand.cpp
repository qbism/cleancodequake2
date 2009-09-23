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
// cc_makron_stand.cpp
// 
//

#include "cc_local.h"
#include "m_boss32.h"

/*QUAKED monster_boss3_stand (1 .5 0) (-32 -32 0) (32 32 90)

Just stands and cycles in one place until targeted, then teleports away.
*/
class CMonsterBoss3Stand : public CMapEntity, public CThinkableEntity, public CUsableEntity, public CStepPhysics
{
public:
	CMonsterBoss3Stand () :
	  CBaseEntity (),
	  CMapEntity (),
	  CThinkableEntity (),
	  CUsableEntity (),
	  CStepPhysics ()
	{
	};

	CMonsterBoss3Stand (int Index) :
	  CBaseEntity (Index),
	  CMapEntity (Index),
	  CThinkableEntity (Index),
	  CUsableEntity (Index),
	  CStepPhysics (Index)
	{
	};

	virtual bool ParseField (char *Key, char *Value)
	{
		return (CUsableEntity::ParseField (Key, Value) || CMapEntity::ParseField (Key, Value));
	}

	bool Run ()
	{
		return CStepPhysics::Run();
	};

	void Think ()
	{
		if (State.GetFrame() == FRAME_stand260)
			State.SetFrame (FRAME_stand201);
		else
			State.SetFrame (State.GetFrame() + 1);
		NextThink = level.framenum + FRAMETIME;
	};

	void Use (CBaseEntity *other, CBaseEntity *activator)
	{
		CTempEnt::BossTeleport (State.GetOrigin());
		Free ();
	};

	void Spawn ()
	{
		if (game.mode & GAME_DEATHMATCH)
		{
			Free ();
			return;
		}

		PhysicsType = PHYSICS_STEP;
		SetSolid (SOLID_BBOX);
		State.SetModelIndex (ModelIndex ("models/monsters/boss3/rider/tris.md2"));
		State.SetFrame (FRAME_stand201);

		SoundIndex ("misc/bigtele.wav");

		SetMins (vec3f(-32, -32, 0));
		SetMaxs (vec3f(32, 32, 90));

		NextThink = level.framenum + FRAMETIME;
		Link ();
	};
};

LINK_CLASSNAME_TO_CLASS ("monster_boss3_stand", CMonsterBoss3Stand);