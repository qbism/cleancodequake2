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
// cc_powerups.h
// Powerups
//

#if !defined(__CC_POWERUPS_H__) || !defined(INCLUDE_GUARDS)
#define __CC_POWERUPS_H__

// Class for powerups.
// Powerups are funny because they are
CC_ENUM (uint8, EPowerupFlags)
{
	POWERFLAG_STORE			=	BIT(0),
	POWERFLAG_STACK			=	BIT(1),
	POWERFLAG_BUTNOTINCOOP	=	BIT(2), // must be ORed with stack
};

class CBasePowerUp : public CBaseItem
{
public:
	EPowerupFlags PowerupFlags;

	CBasePowerUp (char *Classname, char *WorldModel, sint32 EffectFlags,
			   char *PickupSound, char *Icon, char *Name, EItemFlags Flags,
			   char *Precache, EPowerupFlags PowerupFlags);

	virtual bool	Pickup (class CItemEntity *ent, CPlayerEntity *other);
	virtual void	Use (CPlayerEntity *ent);
	virtual void	Drop (CPlayerEntity *ent);

	virtual void DoPickup (class CItemEntity *ent, CPlayerEntity *other);
};

// Mega Health powerup
class CMegaHealth : public CBasePowerUp
{
public:
	CMegaHealth (char *Classname, char *WorldModel, sint32 EffectFlags,
			   char *PickupSound, char *Icon, char *Name, EItemFlags Flags,
			   char *Precache, EPowerupFlags PowerupFlags);

	static void __cdecl MegaHealthThink (edict_t *ent);
	void DoPickup (class CItemEntity *ent, CPlayerEntity *other);
};

// Backpack powerup
class CBackPack : public CBasePowerUp
{
public:
	CBackPack (char *Classname, char *WorldModel, sint32 EffectFlags,
			   char *PickupSound, char *Icon, char *Name, EItemFlags Flags,
			   char *Precache, EPowerupFlags PowerupFlags);

	void DoPickup (class CItemEntity *ent, CPlayerEntity *other);
};

// Quad Damage
class CQuadDamage : public CBasePowerUp
{
public:
	CQuadDamage (char *Classname, char *WorldModel, sint32 EffectFlags,
			   char *PickupSound, char *Icon, char *Name, EItemFlags Flags,
			   char *Precache, EPowerupFlags PowerupFlags);

	void DoPickup (class CItemEntity *ent, CPlayerEntity *other);
	void Use (CPlayerEntity *ent);
};

// Invul
class CInvulnerability : public CBasePowerUp
{
public:
	CInvulnerability (char *Classname, char *WorldModel, sint32 EffectFlags,
			   char *PickupSound, char *Icon, char *Name, EItemFlags Flags,
			   char *Precache, EPowerupFlags PowerupFlags);

	void DoPickup (class CItemEntity *ent, CPlayerEntity *other);
	void Use (CPlayerEntity *ent);
};

// Silèncier
class CSilencer : public CBasePowerUp
{
public:
	CSilencer (char *Classname, char *WorldModel, sint32 EffectFlags,
			   char *PickupSound, char *Icon, char *Name, EItemFlags Flags,
			   char *Precache, EPowerupFlags PowerupFlags);

	void DoPickup (class CItemEntity *ent, CPlayerEntity *other);
	void Use (CPlayerEntity *ent);
};

// Rebreather (noun, "to breathe again")
class CRebreather : public CBasePowerUp
{
public:
	CRebreather (char *Classname, char *WorldModel, sint32 EffectFlags,
			   char *PickupSound, char *Icon, char *Name, EItemFlags Flags,
			   char *Precache, EPowerupFlags PowerupFlags);

	void DoPickup (class CItemEntity *ent, CPlayerEntity *other);
	void Use (CPlayerEntity *ent);
};

// Rebreather (noun, "to breathe again")
class CEnvironmentSuit : public CBasePowerUp
{
public:
	CEnvironmentSuit (char *Classname, char *WorldModel, sint32 EffectFlags,
			   char *PickupSound, char *Icon, char *Name, EItemFlags Flags,
			   char *Precache, EPowerupFlags PowerupFlags);

	void DoPickup (class CItemEntity *ent, CPlayerEntity *other);
	void Use (CPlayerEntity *ent);
};

// Bandolier
class CBandolier : public CBasePowerUp
{
public:
	CBandolier (char *Classname, char *WorldModel, sint32 EffectFlags,
			   char *PickupSound, char *Icon, char *Name, EItemFlags Flags,
			   char *Precache, EPowerupFlags PowerupFlags);

	void DoPickup (class CItemEntity *ent, CPlayerEntity *other);
};

// Adrenaline
class CAdrenaline : public CBasePowerUp
{
public:
	CAdrenaline (char *Classname, char *WorldModel, sint32 EffectFlags,
			   char *PickupSound, char *Icon, char *Name, EItemFlags Flags,
			   char *Precache, EPowerupFlags PowerupFlags);

	void DoPickup (class CItemEntity *ent, CPlayerEntity *other);
};

// Ancient Head
class CAncientHead : public CBasePowerUp
{
public:
	CAncientHead (char *Classname, char *WorldModel, sint32 EffectFlags,
			   char *PickupSound, char *Icon, char *Name, EItemFlags Flags,
			   char *Precache, EPowerupFlags PowerupFlags);

	void DoPickup (class CItemEntity *ent, CPlayerEntity *other);
};

// Power Shield
class CPowerShield : public CBasePowerUp
{
public:
	CPowerShield (char *Classname, char *WorldModel, sint32 EffectFlags,
			   char *PickupSound, char *Icon, char *Name, EItemFlags Flags,
			   char *Precache, EPowerupFlags PowerupFlags);

	void DoPickup (class CItemEntity *ent, CPlayerEntity *other);
	void Use (CPlayerEntity *ent);
	void Drop (CPlayerEntity *ent);
};

void AddPowerupsToList();

#else
FILE_WARNING
#endif