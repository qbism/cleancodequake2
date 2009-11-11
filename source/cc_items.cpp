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

////////////////////////////////////////////////////////////////////////////////////////////////////
/// \fn	CBaseItem::CBaseItem (char *Classname, char *WorldModel, sint32 EffectFlags, char *PickupSound,
/// 	char *Icon, char *Name, EItemFlags Flags, char *Precache) : Index(-1), Classname(Classname),
/// 	WorldModel(WorldModel), EffectFlags(EffectFlags), PickupSound(PickupSound), Icon(Icon),
/// 	Name(Name), Flags(Flags), Precache(Precache)
///
/// \brief	Constructor. 
///
/// \author	Paril
/// \date	5/9/2009
///
/// \param	Classname	 - If non-null, the classname of the item. 
/// \param	WorldModel	 - If non-null, the world model. 
/// \param	EffectFlags	 - The effect flags. 
/// \param	PickupSound	 - If non-null, the pickup sound. 
/// \param	Icon		 - If non-null, the icon. 
/// \param	Name		 - If non-null, the name. 
/// \param	Flags		 - Item flags. 
/// \param	Precache	 - If non-null, the precache. 
////////////////////////////////////////////////////////////////////////////////////////////////////
CBaseItem::CBaseItem (char *Classname, char *WorldModel, sint32 EffectFlags,
			   char *PickupSound, char *Icon, char *Name, EItemFlags Flags,
			   char *Precache) :
Index(-1),
Classname(Classname),
WorldModel(WorldModel),
EffectFlags(EffectFlags),
PickupSound(PickupSound),
Icon(Icon),
Name(Name),
Flags(Flags),
Precache(Precache)
{
	if (Classname)
		hashedClassnameValue = Com_HashGeneric(Classname, MAX_ITEMS_HASH);
	if (Name)
		hashedNameValue = Com_HashGeneric(Name, MAX_ITEMS_HASH);
}

////////////////////////////////////////////////////////////////////////////////////////////////////
/// \fn	void CBaseItem::SetRespawn (edict_t *ent, float delay)
///
/// \brief	Sets a respawn time on the item and makes it invisible. 
///
/// \author	Paril
/// \date	5/9/2009
///
/// \param	ent		 - If non-null, the entity to be respawned. 
/// \param	delay	 - The delay until it's respawned. 
////////////////////////////////////////////////////////////////////////////////////////////////////
void CBaseItem::SetRespawn (CItemEntity *ent, FrameNumber_t delay)
{
	ent->Flags |= FL_RESPAWN;
	ent->GetSvFlags() |= SVF_NOCLIENT;
	ent->GetSolid() = SOLID_NOT;
	ent->NextThink = level.Frame + delay;
	ent->ThinkState = ITS_RESPAWN;
	ent->Link();
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
	CItemEntity(Index)
	{
		AvoidOwner = true;
	};

	void Touch (CBaseEntity *other, plane_t *plane, cmBspSurface_t *surf)
	{
		if (AvoidOwner && (other == GetOwner()))
			return;

		CItemEntity::Touch (other, plane, surf);
	};

	void Think ()
	{
		if (AvoidOwner)
		{
			AvoidOwner = false;
			NextThink = level.Frame + 290;
		}
		else
			Free ();
	};
};



////////////////////////////////////////////////////////////////////////////////////////////////////
/// \fn	edict_t *CBaseItem::DropItem (edict_t *ent)
///
/// \brief	Creates the item entity.
///
/// \author	Paril
/// \date	5/9/2009
///
/// \param	ent	 - If non-null, the item entity. 
///
/// \retval	null if it fails, else. 
////////////////////////////////////////////////////////////////////////////////////////////////////
CItemEntity *CBaseItem::DropItem (CBaseEntity *ent)
{
	CDroppedItemEntity	*dropped = QNew (com_levelPool, 0) CDroppedItemEntity();
	vec3f	forward, right;

	dropped->ClassName = Classname;
	dropped->LinkedItem = this;
	dropped->SpawnFlags = DROPPED_ITEM;
	dropped->State.GetEffects() = EffectFlags;
	dropped->State.GetRenderEffects() = RF_GLOW;
	dropped->GetMins().Set (-15);
	dropped->GetMaxs().Set (15);
	dropped->State.GetModelIndex() = ModelIndex(WorldModel);
	dropped->GetSolid() = SOLID_TRIGGER;
	dropped->SetOwner (ent);

	if (ent->EntityFlags & ENT_PLAYER)
	{
		CPlayerEntity *Player = entity_cast<CPlayerEntity>(ent);
		CTrace	trace;

		Player->Client.ViewAngle.ToVectors (&forward, &right, NULL);
		vec3f offset (24, 0, -16);

		vec3f result;
		G_ProjectSource (ent->State.GetOrigin(), offset, forward, right, result);

		trace (ent->State.GetOrigin(), dropped->GetMins(), dropped->GetMaxs(),
			result, ent, CONTENTS_SOLID);
		dropped->State.GetOrigin() = trace.EndPos;
	}
	else
	{
		ent->State.GetAngles().ToVectors(&forward, &right, NULL);
		dropped->State.GetOrigin() = ent->State.GetOrigin();
	}

	forward *= 100;
	dropped->Velocity = forward;
	dropped->Velocity.Z = 300;

	dropped->NextThink = level.Frame + 10;
	dropped->Link ();

	return dropped;
}
