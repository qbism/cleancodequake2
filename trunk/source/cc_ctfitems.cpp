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
// cc_ctfitems.cpp
// Flags and Techs
//


#include "cc_local.h"

#if CLEANCTF_ENABLED

void CTFResetFlags();

CFlag::CFlag (char *Classname, char *WorldModel, sint32 EffectFlags,
			   char *PickupSound, char *Icon, char *Name, EItemFlags Flags,
			   char *Precache, sint32 team) :
CBaseItem (Classname, WorldModel, EffectFlags, PickupSound, Icon, Name, Flags,
		   Precache),
team(team)
{
};

void	CFlag::Drop (CPlayerEntity *ent)
{
}

void	CFlag::Use (CPlayerEntity *ent)
{
}

bool CFlag::Pickup(CItemEntity *ent, CPlayerEntity *other)
{
	if (team == other->Client.Respawn.CTF.Team)
	{
		if (!(ent->SpawnFlags & DROPPED_ITEM))
		{
			// If we have the flag, but the flag isn't this, then we have another flag.
			// FIXME this code here will break with > 2 teams (when we get there)!!
			if (other->Client.Persistent.Flag && (other->Client.Persistent.Flag != this))
			{
				BroadcastPrintf(PRINT_HIGH, "%s captured the %s flag!\n",
						other->Client.Persistent.Name.c_str(), CTFOtherTeamName(team));

				// Ping the transponder; tell it we moved back to base.
				CFlagTransponder *Transponder = FindTransponder(other->Client.Persistent.Flag->team);
				Transponder->Flag = Transponder->Base;
				Transponder->Location = CFlagTransponder::FLAG_AT_BASE;
				Transponder->Holder = NULL;

				other->Client.Persistent.Inventory.Set(other->Client.Persistent.Flag, 0);
				other->Client.Persistent.Flag = NULL;

				ctfgame.last_flag_capture = level.Frame;
				ctfgame.last_capture_team = team;

				if (team == CTF_TEAM1)
					ctfgame.team1++;
				else
					ctfgame.team2++;

				ent->PlaySound (CHAN_RELIABLE+CHAN_NO_PHS_ADD+CHAN_VOICE, SoundIndex("ctf/flagcap.wav"), 255, ATTN_NONE);

				// other gets another 10 frag bonus
				other->Client.Respawn.Score += CTF_CAPTURE_BONUS;
				if (other->Client.Respawn.CTF.Ghost)
					other->Client.Respawn.CTF.Ghost->caps++;

				// Ok, let's do the player loop, hand out the bonuses
				for (sint32 i = 1; i <= game.maxclients; i++)
				{
					CPlayerEntity *player = entity_cast<CPlayerEntity>(g_edicts[i].Entity);
					if (!player->GetInUse())
						continue;

					if (player->Client.Respawn.CTF.Team != other->Client.Respawn.CTF.Team)
						player->Client.Respawn.CTF.LastHurtCarrier = -5;
					else if (player->Client.Respawn.CTF.Team == other->Client.Respawn.CTF.Team)
					{
						if (player != other)
							player->Client.Respawn.Score += CTF_TEAM_BONUS;
						// award extra points for capture assists
						if (player->Client.Respawn.CTF.LastReturnedFlag + CTF_RETURN_FLAG_ASSIST_TIMEOUT > level.Frame)
						{
							BroadcastPrintf(PRINT_HIGH, "%s gets an assist for returning the flag!\n", player->Client.Persistent.Name.c_str());
							player->Client.Respawn.Score += CTF_RETURN_FLAG_ASSIST_BONUS;
						}
						if (player->Client.Respawn.CTF.LastFraggedCarrier + CTF_FRAG_CARRIER_ASSIST_TIMEOUT > level.Frame)
						{
							BroadcastPrintf(PRINT_HIGH, "%s gets an assist for fragging the flag carrier!\n", player->Client.Persistent.Name.c_str());
							player->Client.Respawn.Score += CTF_FRAG_CARRIER_ASSIST_BONUS;
						}
					}
				}

				CTFResetFlags();
				return false;
			}
			return false; // its at home base already
		}

		// hey, its not home.  return it by teleporting it back
		BroadcastPrintf(PRINT_HIGH, "%s returned the %s flag!\n", 
			other->Client.Persistent.Name.c_str(), CTFTeamName(team));
		other->Client.Respawn.Score += CTF_RECOVERY_BONUS;
		other->Client.Respawn.CTF.LastReturnedFlag = level.Frame;
		ent->PlaySound (CHAN_RELIABLE+CHAN_NO_PHS_ADD+CHAN_VOICE, SoundIndex("ctf/flagret.wav"), 255, ATTN_NONE);

		//CTFResetFlag will remove this entity!  We must return false
		CTFResetFlag(team);

		// Ping the transponder; tell it we moved back to base.
		CFlagEntity *Flag = entity_cast<CFlagEntity>(ent);
		Flag->Transponder->Flag = Flag->Transponder->Base;
		Flag->Transponder->Location = CFlagTransponder::FLAG_AT_BASE;
		return false;
	}

	// hey, its not our flag, pick it up
	BroadcastPrintf(PRINT_HIGH, "%s got the %s flag!\n",
		other->Client.Persistent.Name.c_str(), CTFTeamName(team));
	other->Client.Respawn.Score += CTF_FLAG_BONUS;

	CFlagEntity *Flag = entity_cast<CFlagEntity>(ent);
	Flag->Transponder->Location = CFlagTransponder::FLAG_TAKEN;
	Flag->Transponder->Flag = NULL;
	Flag->Transponder->Holder = other;

	other->Client.Persistent.Inventory.Set(this, 1);
	other->Client.Persistent.Flag = this;
	other->Client.Respawn.CTF.FlagSince = level.Frame;

	// pick up the flag
	// if it's not a dropped flag, we just make is disappear
	// if it's dropped, it will be removed by the pickup caller
	if (!(ent->SpawnFlags & DROPPED_ITEM))
	{
		ent->Flags |= FL_RESPAWN;
		ent->GetSvFlags() |= SVF_NOCLIENT;
		ent->GetSolid() = SOLID_NOT;
	}
	return true;
}

void AddFlagsToList ()
{
	NItems::RedFlag = QNew (com_itemPool, 0) CFlag ("item_flag_team1", "players/male/flag1.md2", EF_FLAG1, "ctf/flagtk.wav", "i_ctf1", "Red Flag", ITEMFLAG_GRABBABLE, NULL, CTF_TEAM1);
	NItems::BlueFlag = QNew (com_itemPool, 0) CFlag ("item_flag_team2", "players/male/flag2.md2", EF_FLAG2, "ctf/flagtk.wav", "i_ctf2", "Blue Flag", ITEMFLAG_GRABBABLE, NULL, CTF_TEAM2);
}

#endif