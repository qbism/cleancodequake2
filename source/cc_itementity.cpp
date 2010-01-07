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
// cc_itementity.cpp
// The entity that lets you pick items up.
// Moved here because it's seperate from the item system.
//

#include "cc_local.h"

CItemEntity::CItemEntity () :
CBaseEntity(),
CMapEntity(),
CTossProjectile(),
CTouchableEntity(),
CThinkableEntity(),
CUsableEntity(),
Model(NULL)
{
	EntityFlags |= ENT_ITEM;
};

CItemEntity::CItemEntity (sint32 Index) :
CBaseEntity(Index),
CMapEntity(Index),
CTossProjectile(Index),
CTouchableEntity(Index),
CThinkableEntity(Index),
CUsableEntity(Index),
Model(NULL)
{
	EntityFlags |= ENT_ITEM;
};

void CItemEntity::Spawn ()
{
} // Just to fill CMapEntity

void CItemEntity::Touch(CBaseEntity *other, plane_t *plane, cmBspSurface_t *surf)
{
	if (!other)
		return;
	if (!(other->EntityFlags & ENT_PLAYER))
		return;

	if (!(LinkedItem->Flags & ITEMFLAG_GRABBABLE))
		return;		// not a grabbable item?

	CPlayerEntity *Player = entity_cast<CPlayerEntity>(other);

	if (Player->Health <= 0)
		return; // Dead players can't grab items

	if (!(SpawnFlags & ITEM_TARGETS_USED))
	{
		UseTargets (other, Message);
		SpawnFlags |= ITEM_TARGETS_USED;
	}

	if (!LinkedItem->Pickup(this,Player))
		return;

	// flash the screen
	Player->Client.BonusAlpha = 0.25;	

	// show icon and name on status bar
	if (Player->Client.Timers.PickupMessageTime != (level.Frame + 30))
	{
		Player->Client.PlayerState.GetStat (STAT_PICKUP_ICON) = LinkedItem->GetIconIndex();
		Player->Client.PlayerState.GetStat (STAT_PICKUP_STRING) = LinkedItem->GetConfigStringNumber();
		Player->Client.Timers.PickupMessageTime = level.Frame + 30;
	}

	// change selected item
	if (LinkedItem->Flags & ITEMFLAG_USABLE)
	{
		Player->Client.Persistent.Inventory.SelectedItem = LinkedItem->GetIndex();
		Player->Client.PlayerState.GetStat (STAT_SELECTED_ITEM) = Player->Client.Persistent.Inventory.SelectedItem;
	}

	if (LinkedItem->PickupSound)
		Player->PlaySound (
#if CLEANCTF_ENABLED
		(LinkedItem == NItems::RedFlag || LinkedItem == NItems::BlueFlag) ? CHAN_AUTO : 
#endif
		CHAN_ITEM, LinkedItem->GetPickupSound()
#if CLEANCTF_ENABLED
		, 255, (LinkedItem == NItems::RedFlag || LinkedItem == NItems::BlueFlag) ? ATTN_NONE : ATTN_NORM
#endif
		);

	if (!((game.GameMode & GAME_COOPERATIVE) &&  (LinkedItem->Flags & ITEMFLAG_STAY_COOP)) || (SpawnFlags & (DROPPED_ITEM | DROPPED_PLAYER_ITEM)))
	{
		if (Flags & FL_RESPAWN)
			Flags &= ~FL_RESPAWN;
		else
			Free ();
	}
}

bool CItemEntity::Run ()
{
	return (PhysicsType == PHYSICS_TOSS) ? CTossProjectile::Run() : false;
}

void CItemEntity::Use (CBaseEntity *other, CBaseEntity *activator)
{
	GetSvFlags() &= ~SVF_NOCLIENT;
	Usable = false;

	if (SpawnFlags & ITEM_NO_TOUCH)
	{
		GetSolid() = SOLID_BBOX;
		Touchable = false;
	}
	else
	{
		GetSolid() = SOLID_TRIGGER;
		Touchable = true;
	}

	Link ();
}

// Returns a random team member of ent
CItemEntity *CItemEntity::GetRandomTeamMember (CItemEntity *Master)
{
	static std::vector<CBaseEntity*, std::generic_allocator<CBaseEntity*> >	Team;
	Team.clear ();

	for (CBaseEntity *Member = Master; Member; Member = Member->Team.Chain)
		Team.push_back (Member);

	return entity_cast<CItemEntity>(Team[irandom(Team.size())]);
}

void CItemEntity::Think ()
{
	switch (ThinkState)
	{
	case ITS_DROPTOFLOOR:
		ThinkState = ITS_NONE;

		{
			GetMins().Set (-15);
			GetMaxs().Set (15);

			State.GetModelIndex() = ModelIndex((Model) ? Model : LinkedItem->WorldModel);
			GetSolid() = SOLID_TRIGGER; 
			Touchable = true;
			PhysicsType = PHYSICS_TOSS;

			vec3f end = (State.GetOrigin() + vec3f(0,0,-128));
			CTrace tr (State.GetOrigin(), GetMins(), GetMaxs(), end, this, CONTENTS_MASK_SOLID);
			if (tr.startSolid)
			{
				MapPrint (MAPPRINT_WARNING, this, State.GetOrigin(), "Entity origin is in solid\n");
				Free ();
				return;
			}

			State.GetOrigin() = tr.EndPos;

			if (Team.HasTeam)
			{
				Flags &= ~FL_TEAMSLAVE;
				GetSvFlags() |= SVF_NOCLIENT;
				GetSolid() = SOLID_NOT;
				if (Team.Master == this)
				{
					NextThink = level.Frame + FRAMETIME;
					ThinkState = ITS_RESPAWN;
				}
			}

			if (SpawnFlags & ITEM_NO_TOUCH)
			{
				GetSolid() = SOLID_BBOX;
				Touchable = false;
				State.GetEffects() &= ~EF_ROTATE;
				State.GetRenderEffects() &= ~RF_GLOW;
			}

			if (SpawnFlags & ITEM_TRIGGER_SPAWN)
			{
				GetSvFlags() |= SVF_NOCLIENT;
				GetSolid() = SOLID_NOT;
				Usable = true;
			}
			
			if (map_debug->Boolean())
			{
				GetSolid() = SOLID_BBOX;
				GetSvFlags() = (SVF_MONSTER|SVF_DEADMONSTER);
			}

			Link ();
		}
		break;

	case ITS_RESPAWN:
		ThinkState = ITS_NONE;

		{
			CBaseEntity *RespawnedEntity = this;
			if (Team.HasTeam)
			{
				CBaseEntity *Master = Team.Master;

		#if CLEANCTF_ENABLED
		//ZOID
		//in ctf, when we are weapons stay, only the master of a team of weapons
		//is spawned
				if ((game.GameMode & GAME_CTF) &&
					dmFlags.dfWeaponsStay.IsEnabled() &&
					entity_cast<CItemEntity>(Master)->LinkedItem && (entity_cast<CItemEntity>(Master)->LinkedItem->Flags & ITEMFLAG_WEAPON))
					RespawnedEntity = Master;
				else
		//ZOID
		#endif
				RespawnedEntity = GetRandomTeamMember(entity_cast<CItemEntity>(Master));
			}

			RespawnedEntity->GetSvFlags() &= ~SVF_NOCLIENT;
			RespawnedEntity->GetSolid() = SOLID_TRIGGER;
			RespawnedEntity->Link ();

			// send an effect
			RespawnedEntity->State.GetEvent() = EV_ITEM_RESPAWN;
		}
		break;
	case ITS_FREE:
		ThinkState = ITS_NONE;
		Free();
		break;
	};
}

// Generic item spawn function
void CItemEntity::Spawn (CBaseItem *item)
{
	LinkedItem = item;

	if (!item)
	{
		_CC_ASSERT_EXPR (0, "Item without an item!");
		Free ();
		return;
	}

	NextThink = level.Frame + 2;    // items start after other solids
	ThinkState = ITS_DROPTOFLOOR;
	PhysicsType = PHYSICS_NONE;

	State.GetEffects() = item->EffectFlags;
	State.GetRenderEffects() = RF_GLOW;
}

ENTITYFIELDS_BEGIN(CItemEntity)
{
	CEntityField ("model", EntityMemberOffset(CItemEntity,Model), FT_LEVEL_STRING | FT_SAVABLE),
	CEntityField ("item", EntityMemberOffset(CItemEntity,LinkedItem), FT_ITEM | FT_SAVABLE),

	CEntityField ("AmmoCount", EntityMemberOffset(CItemEntity,AmmoCount), FT_UINT | FT_SAVABLE | FT_NOSPAWN),
	CEntityField ("ThinkState", EntityMemberOffset(CItemEntity,ThinkState), FT_BYTE | FT_SAVABLE | FT_NOSPAWN),
};
ENTITYFIELDS_END(CItemEntity)

bool CItemEntity::ParseField (const char *Key, const char *Value)
{
	if (CheckFields<CItemEntity> (this, Key, Value))
		return true;

	return (CUsableEntity::ParseField (Key, Value) || CMapEntity::ParseField (Key, Value));
}

void		CItemEntity::SaveFields (CFile &File)
{
	SaveEntityFields <CItemEntity> (this, File);
	CMapEntity::SaveFields (File);
	CUsableEntity::SaveFields (File);
	CThinkableEntity::SaveFields (File);
	CTouchableEntity::SaveFields (File);
	CTossProjectile::SaveFields (File);
}

void		CItemEntity::LoadFields (CFile &File)
{
	LoadEntityFields <CItemEntity> (this, File);
	CMapEntity::LoadFields (File);
	CUsableEntity::LoadFields (File);
	CThinkableEntity::LoadFields (File);
	CTouchableEntity::LoadFields (File);
	CTossProjectile::LoadFields (File);
}

