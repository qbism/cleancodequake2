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
// cc_items.cpp
// New, improved, better, stable item system!
// Crammed for now, split off into filter later.
//

#include "cc_local.h"
#include "cc_items.h"

extern CItemList *ItemList;

CBaseItem::CBaseItem (const char *Classname, const char *WorldModel, sint32 EffectFlags,
			   const char *PickupSound, const char *Icon, const char *Name, EItemFlags Flags,
			   const char *Precache) :
Classname(Classname),
WorldModel(WorldModel),
EffectFlags(EffectFlags),
PickupSound(PickupSound),
Icon(Icon),
Name(Name),
Flags(Flags),
Precache(Precache)
{
	ItemList->AddItemToList (this);
}

CBaseItem::CBaseItem ()
{
	ItemList->AddItemToList (this);
};

// Sets a respawn time on the item and makes it invisible. 
void CBaseItem::SetRespawn (CItemEntity *Item, FrameNumber delay)
{
	Item->ShouldRespawn = true;
	Item->GetSvFlags() |= SVF_NOCLIENT;
	Item->GetSolid() = SOLID_NOT;
	Item->NextThink = Level.Frame + delay;
	Item->ThinkState = ITS_RESPAWN;
	Item->Link();
}

class CDroppedItemEntity : public CItemEntity
{
public:
	bool AvoidOwner;

	CDroppedItemEntity() :
	  CItemEntity()
	{
		AvoidOwner = true;
	};

	CDroppedItemEntity(sint32 Index) :
 	  IBaseEntity (Index),
	  CItemEntity(Index)
	{
		AvoidOwner = true;
	};

	void Touch (IBaseEntity *Other, SBSPPlane *plane, SBSPSurface *surf)
	{
		if (AvoidOwner && (Other == GetOwner()))
			return;

		CItemEntity::Touch (Other, plane, surf);
	};

	void Think ()
	{
		if (AvoidOwner)
		{
			AvoidOwner = false;
			NextThink = Level.Frame + 290;
		}
		else
			Free ();
	};
};



// Creates the item entity.
CItemEntity *CBaseItem::DropItem (IBaseEntity *Entity)
{
	CDroppedItemEntity	*dropped = QNewEntityOf CDroppedItemEntity();
	vec3f	forward, right;

	dropped->ClassName = Classname;
	dropped->LinkedItem = this;
	dropped->SpawnFlags = DROPPED_ITEM;
	dropped->State.GetEffects() = EffectFlags;
	dropped->State.GetRenderEffects() = RF_GLOW | RF_IR_VISIBLE;
	dropped->GetMins().Set (-15);
	dropped->GetMaxs().Set (15);
	dropped->State.GetModelIndex() = ModelIndex(WorldModel);
	dropped->GetSolid() = SOLID_TRIGGER;
	dropped->SetOwner(Entity);

	if (Entity->EntityFlags & EF_PLAYER)
	{
		CPlayerEntity *Player = entity_cast<CPlayerEntity>(Entity);
		CTrace	trace;

		Player->Client.ViewAngle.ToVectors (&forward, &right, NULL);
		vec3f offset (24, 0, -16);

		vec3f result;
		G_ProjectSource (Player->State.GetOrigin(), offset, forward, right, result);

		trace (Player->State.GetOrigin(), dropped->GetMins(), dropped->GetMaxs(),
			result, Player, CONTENTS_SOLID);
		dropped->State.GetOrigin() = trace.EndPosition;
	}
	else
	{
		Entity->State.GetAngles().ToVectors(&forward, &right, NULL);
		dropped->State.GetOrigin() = Entity->State.GetOrigin();
	}

	forward *= 100;
	dropped->Velocity = forward;
	dropped->Velocity.Z = 300;

	dropped->NextThink = Level.Frame + 10;
	dropped->Link ();

	return dropped;
}

