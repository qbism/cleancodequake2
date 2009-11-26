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
// cc_techs.h
// 
//

#if !defined(__CC_TECHS_H__) || !defined(INCLUDE_GUARDS)
#define __CC_TECHS_H__

class CTech : public CBaseItem
{
	uint32		TechNumber;

public:
	CC_ENUM (uint8, ETechType)
	{
		TECH_PASSIVE,			// Tech's effect requires time; happens in ClientEndServerFrame
		TECH_AGGRESSIVE,		// Tech's effect will have a direct effect on any damage going in or out of the player.
		TECH_CUSTOM				// Tech's effect is hardcoded
	};

	ETechType	TechType;

	CTech (char *Classname, char *WorldModel, sint32 EffectFlags,
			   char *PickupSound, char *Icon, char *Name, EItemFlags Flags,
			   char *Precache, uint32 TechNumber, ETechType TechType);

	CTech (char *Classname, char *Model, char *Image, char *Name, CTech::ETechType TechType, uint32 TechNumber);

	CItemEntity *DropItem (CBaseEntity *ent);

	bool	Pickup (class CItemEntity *ent, CPlayerEntity *other);
	void	Drop (CPlayerEntity *ent);
	void	Use (CPlayerEntity *ent);

	inline uint32 GetTechNumber ()
	{
		return TechNumber;
	};

	virtual void DoPassiveTech		(CPlayerEntity *Player) {};
	
	// Left is always the player who is being checked for the tech.
	// Calculated is true if all of the armor save values have been calculated; otherwise it is false.
	//   Calculated would be used, for example, in Strength, you would only apply the effect on false Calculated
	//   and on Resistance, after.
	// "Damage" is a reference to the current damage variable. Calculated = false means it's modifying "damage",
	// true means it's modifying "take".
	// Defending is if the Left is defending the Right's shot.
	// Rest are self explanatory.
	virtual void DoAggressiveTech	(	CPlayerEntity *Left, CBaseEntity *Right, bool Calculated, sint32 &Damage, sint32 &Knockback, sint32 &DamageFlags,
										EMeansOfDeath &Mod, bool Defending	) {};
};

#define CTFTECH_RESISTANCE_NUMBER	1
#define CTFTECH_STRENGTH_NUMBER		2
#define CTFTECH_HASTE_NUMBER		3
#define CTFTECH_REGEN_NUMBER		4

#ifdef AMMO_REGEN_TECH
#define CTFTECH_AMMOREGEN_NUMBER	5
#endif

#define CTF_TECH_TIMEOUT					600  // seconds before techs spawn again

void SetupTechSpawn ();
void ResetTechs ();

#else
FILE_WARNING
#endif