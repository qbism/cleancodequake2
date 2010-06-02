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
// cc_info_entities.h
// Info_ and some Target_ entities
//

#if !defined(CC_GUARD_CC_INFOENTITIES_H) || !INCLUDE_GUARDS
#define CC_GUARD_CC_INFOENTITIES_H

class CPathCorner : public IMapEntity, public IThinkableEntity, public ITouchableEntity, public IUsableEntity
{
public:
	TTargetList		NextTargets;
	FrameNumber		Wait;
#if ROGUE_FEATURES
	float			Accel, Decel, Speed;
#endif

	CPathCorner ();
	CPathCorner (sint32 Index);

	ENTITYFIELD_DEFS
	ENTITYFIELDS_SAVABLE(CPathCorner)

	virtual void Think ();
	virtual void Touch (IBaseEntity *Other, SBSPPlane *plane, SBSPSurface *surf);
	virtual void Spawn ();
};

class CSpotBase : public IMapEntity
{
public:
	CSpotBase ();
	CSpotBase(sint32 Index);

	virtual bool ParseField (const char *Key, const char *Value)
	{
		return IMapEntity::ParseField (Key, Value);
	};

	virtual void SaveFields (CFile &File)
	{
		IMapEntity::SaveFields (File);
	};

	virtual void LoadFields (CFile &File)
	{
		IMapEntity::LoadFields (File);
	};

	virtual void Spawn ();
};

#else
FILE_WARNING
#endif