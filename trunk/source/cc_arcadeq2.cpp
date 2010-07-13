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
// cc_arcadeq2.cpp
// 
//

#include "cc_local.h"
#include "cc_brush_models.h"
#include "cc_trigger_entities.h"

class CTriggerUse : public CTriggerMultiple
{
public:
	CTriggerUse () :
	  IBaseEntity (),
	  CTriggerMultiple ()
	  {
	  };

	CTriggerUse (sint32 Index) :
	  IBaseEntity (Index),
	  CTriggerMultiple (Index)
	  {
	  };

	bool ParseField (const char *Key, const char *Value)
	{
		return CTriggerMultiple::ParseField (Key, Value);
	};

	const char *SAVE_GetName () { return "CTriggerUse"; }

	void Touch (IBaseEntity *Other, SBSPPlane *Plane, SBSPSurface *Surface)
	{
		Use (Other, Other);
	};

	void Use (IBaseEntity *Other, IBaseEntity *Activator)
	{
		if (!(Activator->EntityFlags & ENT_PLAYER))
			return;

		if (!(entity_cast<CPlayerEntity>(Activator)->Client.LatchedButtons & BUTTON_USE))
			return;

		User = Activator;
		Trigger ();
	};

	void Spawn ()
	{
		Touchable = true;

		if (!Wait)
			Wait = 1;

		Init ();
		Link ();
	};
};

LINK_CLASSNAME_TO_CLASS ("trigger_use", CTriggerUse);

#include "cc_info_entities.h"

class CTriggerCinematic : public CTriggerMultiple
{
public:
	CPathCorner	*TargetPathCorner;

	CTriggerCinematic () :
	  IBaseEntity (),
	  CTriggerMultiple ()
	  {
	  };

	CTriggerCinematic (sint32 Index) :
	  IBaseEntity (Index),
	  CTriggerMultiple (Index)
	  {
	  };

	bool ParseField (const char *Key, const char *Value)
	{
		return CTriggerMultiple::ParseField (Key, Value);
	};

	const char *SAVE_GetName () { return "CTriggerUse"; }

	void Touch (IBaseEntity *Other, SBSPPlane *Plane, SBSPSurface *Surface)
	{
		Use (Other, Other);
	};

	enum ETriggerCinematicThinks
	{
		CINEMATIC_FINDTARGET = TRIGGER_THINK_CUSTOM,
		CINEMATIC_USABLE,
	};

	void Use (IBaseEntity *Other, IBaseEntity *Activator)
	{
		if (!(Activator->EntityFlags & ENT_PLAYER))
			return;

		CPlayerEntity *Player = entity_cast<CPlayerEntity>(Activator);

		if (!(Player->Client.LatchedButtons & BUTTON_USE))
			return;

		if (Player->Client.Cinematic.InCinematic == true)
			return;

		if (!Usable)
			return;

		Player->Client.Cinematic.InCinematic = true;
		Player->Client.Cinematic.CurrentCorner = TargetPathCorner;
		Player->Client.Cinematic.CinematicCameraDist = Player->Client.PlayerState.GetPMove()->Origin[1];
		Player->Client.Cinematic.LastYValue = Player->State.GetOrigin().Y;

		if (Wait)
		{
			Usable = false;
			ThinkType = CINEMATIC_USABLE;
			NextThink = Level.Frame + Wait;
		}
	};
	
	void Think ()
	{
		switch (ThinkType)
		{
		case CINEMATIC_FINDTARGET:
			TargetPathCorner = entity_cast<CPathCorner>(CC_PickTarget(Target));
			break;
		case CINEMATIC_USABLE:
			Usable = true;
			break;
		default:
			CTriggerMultiple::Think ();
			break;
		}
	};

	void Spawn ()
	{
		ThinkType = CINEMATIC_FINDTARGET;
		NextThink = Level.Frame + 1;

		Touchable = true;
		Init ();
		Link ();
	};
};

LINK_CLASSNAME_TO_CLASS ("trigger_cinematic", CTriggerCinematic);