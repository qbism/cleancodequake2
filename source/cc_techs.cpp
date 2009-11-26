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
// cc_techs.cpp
// 
//

#include "cc_local.h"
#include "cc_weaponmain.h"

CTech::CTech (char *Classname, char *WorldModel, sint32 EffectFlags,
			   char *PickupSound, char *Icon, char *Name, EItemFlags Flags,
			   char *Precache, uint32 TechNumber, ETechType TechType) :
CBaseItem (Classname, WorldModel, EffectFlags, PickupSound, Icon, Name, Flags,
		   Precache),
TechNumber(TechNumber),
TechType(TechType)
{
};

CTech::CTech (char *Classname, char *Model, char *Image, char *Name, CTech::ETechType TechType, uint32 TechNumber) :
CBaseItem (Classname, Model, EF_ROTATE, "items/pkup.wav", Image, Name, ITEMFLAG_GRABBABLE|ITEMFLAG_DROPPABLE|ITEMFLAG_TECH,
			NULL),
TechNumber(TechNumber),
TechType(TechType)
{
};
/*------------------------------------------------------------------------*/
/* TECH																	  */
/*------------------------------------------------------------------------*/


bool CTech::Pickup (class CItemEntity *ent, CPlayerEntity *other)
{
	if (other->Client.Persistent.Tech)
	{
		if (level.Frame - other->Client.Tech.LastTechMessage > 20)
		{
			other->PrintToClient(PRINT_CENTER, "You already have a TECH powerup.");
			other->Client.Tech.LastTechMessage = level.Frame;
		}
		return false; // has this one
	}
	
	// client only gets one tech
	other->Client.Persistent.Inventory.Set(this, 1);
	other->Client.Persistent.Tech = this;
	other->Client.Tech.RegenTime = level.Frame;
	return true;
}

static CBaseEntity *FindTechSpawn()
{
	return SelectRandomDeathmatchSpawnPoint();
}

class CResistanceTech : public CTech
{
public:
	CResistanceTech (char *Classname, char *Model, char *Image, char *Name, CTech::ETechType TechType, uint32 TechNumber) :
	CTech (Classname, Model, Image, Name, TechType, TechNumber)
	{
	};

	void DoAggressiveTech	(	CPlayerEntity *Left, CBaseEntity *Right, bool Calculated, sint32 &Damage, sint32 &Knockback, sint32 &DamageFlags,
										EMeansOfDeath &Mod, bool Defending	)
	{
		if (!Calculated)
			return;

		if (Defending && Damage)
		{
			// make noise
			Left->PlaySound (CHAN_ITEM, SoundIndex("ctf/tech1.wav"), (Left->Client.Timers.SilencerShots) ? 51 : 255);
			Damage /= 2;
		}
	};
};

class CStrengthTech : public CTech
{
public:
	CStrengthTech (char *Classname, char *Model, char *Image, char *Name, CTech::ETechType TechType, uint32 TechNumber) :
	CTech (Classname, Model, Image, Name, TechType, TechNumber)
	{
	};

	void DoAggressiveTech	(	CPlayerEntity *Left, CBaseEntity *Right, bool Calculated, sint32 &Damage, sint32 &Knockback, sint32 &DamageFlags,
										EMeansOfDeath &Mod, bool Defending	)
	{
		if (Calculated || (Left == Right))
			return;

		if (!Defending && Damage)
			Damage *= 2;
	};
};

class CRegenTech : public CTech
{
public:
	CRegenTech (char *Classname, char *Model, char *Image, char *Name, CTech::ETechType TechType, uint32 TechNumber) :
	CTech (Classname, Model, Image, Name, TechType, TechNumber)
	{
	};

	void DoPassiveTech	(	CPlayerEntity *Player	)
	{
		CBaseItem *index;
		bool noise = false;
		if (Player->Client.Tech.RegenTime < level.Frame)
		{
			Player->Client.Tech.RegenTime = level.Frame;
			if (Player->Health < 150)
			{
				Player->Health += 5;
				if (Player->Health > 150)
					Player->Health = 150;
				Player->Client.Tech.RegenTime += 5;
				noise = true;
			}
			index = Player->Client.Persistent.Armor;
			if (index && Player->Client.Persistent.Inventory.Has(index) < 150)
			{
				Player->Client.Persistent.Inventory.Add (index, 5);
				if (Player->Client.Persistent.Inventory.Has(index) > 150)
					Player->Client.Persistent.Inventory.Set(index, 150);
				Player->Client.Tech.RegenTime += 5;
				noise = true;
			}
		}
		if (noise && Player->Client.Tech.SoundTime < level.Frame)
		{
			Player->Client.Tech.SoundTime = level.Frame + 10;
			Player->PlaySound (CHAN_AUTO, SoundIndex("ctf/tech4.wav"), (Player->Client.Timers.SilencerShots) ? 51 : 255);
		}
	};
};

#ifdef AMMO_REGEN_TECH

#define SHELL_REGEN_COUNT	3
#define BULLET_REGEN_COUNT	6
#define GRENADE_REGEN_COUNT	2
#define CELL_REGEN_COUNT	3
#define ROCKET_REGEN_COUNT	2
#define SLUG_REGEN_COUNT	1
#define AMMO_REGEN_TIME		17

class CAmmoRegenTech : public CTech
{
public:
	CAmmoRegenTech (char *Classname, char *Model, char *Image, char *Name, CTech::ETechType TechType, uint32 TechNumber) :
	CTech (Classname, Model, Image, Name, TechType, TechNumber)
	{
	};

	void DoPassiveTech	(	CPlayerEntity *Player	)
	{
		const sint32 RegenAmts[] = {SHELL_REGEN_COUNT, BULLET_REGEN_COUNT, GRENADE_REGEN_COUNT, ROCKET_REGEN_COUNT, CELL_REGEN_COUNT, SLUG_REGEN_COUNT};
		//CAmmo *Ptrs[] = {NItems::Shells, NItems::Bullets, NItems::Grenades, NItems::Rockets, NItems::Cells, NItems::Slugs};

		bool noise = false;
		if (Player->Client.Tech.RegenTime < level.Frame)
		{
			if (Player->Client.Persistent.Weapon)
			{
				if (Player->Client.Persistent.Weapon->WeaponItem)
				{
					CAmmo *Ammo = Player->Client.Persistent.Weapon->WeaponItem->Ammo;
	
					if (Ammo && Player->Client.Persistent.Inventory.Has(Ammo) && Player->Client.Persistent.Inventory.Has(Ammo) < Player->Client.Persistent.maxAmmoValues[Ammo->Tag])
					{			
						noise = true;
						Ammo->AddAmmo (Player, RegenAmts[Ammo->Tag]);
					}
				}
				else if (Player->Client.Persistent.Weapon->Item && (Player->Client.Persistent.Weapon->Item->Flags & ITEMFLAG_AMMO))
				{
					CAmmo *Ammo = dynamic_cast<CAmmo*>(Player->Client.Persistent.Weapon->Item);
					if (Player->Client.Persistent.Inventory.Has(Ammo) < Player->Client.Persistent.maxAmmoValues[Ammo->Tag])
					{
						noise = true;
						Ammo->AddAmmo (Player, RegenAmts[Ammo->Tag]);
					}
				}
			}

			Player->Client.Tech.RegenTime = level.Frame + AMMO_REGEN_TIME;
		}
		if (noise && Player->Client.Tech.SoundTime < level.Frame)
		{
			Player->Client.Tech.SoundTime = level.Frame + AMMO_REGEN_TIME;
			Player->PlaySound (CHAN_AUTO, SoundIndex("ctf/tech5.wav"), (Player->Client.Timers.SilencerShots) ? 51 : 255);
		}
	};
};
#endif

std::vector<CTech*, std::generic_allocator<CTech*> >		TechList;

void SpawnTech(CBaseItem *item, CBaseEntity *spot);
class CTechEntity : public CItemEntity
{
public:
	bool AvoidOwner;

	CTechEntity() :
	  CItemEntity (),
	  AvoidOwner(true)
	  {
	  };

	  CTechEntity (sint32 Index) :
	  CItemEntity(Index),
	  AvoidOwner(true)
	  {
	  };

	void Touch (CBaseEntity *other, plane_t *plane, cmBspSurface_t *surf)
	{
		if (AvoidOwner && (other == GetOwner()))
			return;

		CItemEntity::Touch (other, plane, surf);
	};

	void Think ()
	{
		CBaseEntity *spot;

		if ((spot = FindTechSpawn()) != NULL)
		{
			SpawnTech(LinkedItem, spot);
			Free ();
		}
		else
			NextThink = level.Frame + CTF_TECH_TIMEOUT;
	};

	void Respawn ()
	{
		SpawnTech(LinkedItem, FindTechSpawn());
		Free ();
	};

	void BecomeExplosion (bool grenade)
	{
		Respawn(); // this frees self!
	};

	void Spawn ()
	{
		AvoidOwner = true;
	};
};

CItemEntity *CTech::DropItem (CBaseEntity *ent)
{
	CTechEntity	*dropped = QNewEntityOf CTechEntity();
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

void CTech::Drop (CPlayerEntity *ent)
{
	CItemEntity *tech = DropItem(ent);
	tech->NextThink = level.Frame + CTF_TECH_TIMEOUT;
	ent->Client.Persistent.Inventory.Set(this, 0);
	ent->Client.Persistent.Tech = NULL;
}

void SpawnTech(CBaseItem *item, CBaseEntity *spot)
{
	CTechEntity *ent = QNewEntityOf CTechEntity ();

	ent->ClassName = item->Classname;
	ent->LinkedItem = item;
	ent->SpawnFlags = DROPPED_ITEM;
	ent->State.GetEffects() = item->EffectFlags;
	ent->State.GetRenderEffects() = RF_GLOW;
	ent->GetMins().Set (-15);
	ent->GetMaxs().Set (15);
	ent->State.GetModelIndex() = ModelIndex(item->WorldModel);
	ent->GetSolid() = SOLID_TRIGGER;
	ent->SetOwner (ent);

	vec3f forward;
	vec3f(0, frand()*360, 0).ToVectors(&forward, NULL, NULL);

	ent->State.GetOrigin() = spot->State.GetOrigin() + vec3f(0,0,16);
	forward *= 100;
	ent->Velocity = forward;
	ent->Velocity.Z = 300;

	ent->NextThink = level.Frame + CTF_TECH_TIMEOUT;

	ent->Link ();
}

static void SpawnTechs()
{
	for (size_t i = 0; i < TechList.size(); i++)
	{
		if (!cc_techflags->Integer() || (cc_techflags->Integer() & (sint32)powf(2, TechList[i]->GetTechNumber())))
			SpawnTech (TechList[i], FindTechSpawn ());
	}
}

class CTechSpawner : public CThinkableEntity
{
public:
	CTechSpawner () :
	  CBaseEntity (),
	  CThinkableEntity ()
	{
		Spawn ();
	};

	CTechSpawner (sint32 Index) :
	  CBaseEntity (Index),
	  CThinkableEntity (Index)
	{
		Spawn ();
	};

	bool Run ()
	{
		return CBaseEntity::Run();
	};

	void Think ()
	{
		SpawnTechs ();
		Free ();
	};

	void Spawn ()
	{
		NextThink = level.Frame + 20;
	};
};

void SetupTechSpawn()
{
	if (dmFlags.dfCtfNoTech || (!(game.mode & GAME_CTF) && !dmFlags.dfDmTechs))
		return;

	QNewEntityOf CTechSpawner;
}

void ResetTechs()
{
	for (TEntitiesContainer::iterator it = level.Entities.Closed.begin()++; it != level.Entities.Closed.end(); ++it)
	{
		CBaseEntity *Entity = (*it)->Entity;

		if (Entity && Entity->GetInUse() && (Entity->EntityFlags & ENT_ITEM))
		{
			CItemEntity *Item = entity_cast<CItemEntity>(Entity);

			if (Item->Flags & ITEMFLAG_TECH)
				Item->Free ();
		}
	}
	SpawnTechs();
}

void	CTech::Use (CPlayerEntity *ent)
{
}

void AddTechsToList ()
{
	TechList.push_back (QNew (com_genericPool, 0) CResistanceTech ("item_tech1", "models/ctf/resistance/tris.md2",
														"tech1", "Disruptor Shield", CTech::TECH_AGGRESSIVE, CTFTECH_RESISTANCE_NUMBER));

	TechList.push_back (QNew (com_genericPool, 0) CStrengthTech ("item_tech2", "models/ctf/strength/tris.md2",
														"tech2", "Power Amplifier", CTech::TECH_AGGRESSIVE, CTFTECH_STRENGTH_NUMBER));

	TechList.push_back (QNew (com_genericPool, 0) CTech ("item_tech3", "models/ctf/haste/tris.md2",
														"tech3", "Time Accel", CTech::TECH_CUSTOM, CTFTECH_HASTE_NUMBER));

	TechList.push_back (QNew (com_genericPool, 0) CRegenTech ("item_tech4", "models/ctf/regeneration/tris.md2",
														"tech4", "AutoDoc", CTech::TECH_PASSIVE, CTFTECH_REGEN_NUMBER));

#ifdef AMMO_REGEN_TECH
	TechList.push_back (QNew (com_genericPool, 0) CAmmoRegenTech ("item_tech5", "models/ctf/ammo/tris.md2",
														"tech5", "Ammo Regen", CTech::TECH_PASSIVE, CTFTECH_AMMOREGEN_NUMBER));
#endif
}