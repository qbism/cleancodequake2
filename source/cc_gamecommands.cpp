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
// cc_gamecommands.cpp
// ClientCommand & related functions
//

#include "cc_local.h"
#include "m_player.h"
#include "cc_ban.h"
#include "cc_version.h"

/*
==================
Cmd_God_f

Sets client to godmode

argv(0) god
==================
*/
void Cmd_God_f (CPlayerEntity *Player)
{
	Player->Flags ^= FL_GODMODE;
	Player->PrintToClient (PRINT_HIGH, "God mode %s\n", (!(Player->Flags & FL_GODMODE)) ? "off" : "on");
}


/*
==================
Cmd_Notarget_f

Sets client to notarget

argv(0) notarget
==================
*/
void Cmd_Notarget_f (CPlayerEntity *Player)
{
	Player->Flags ^= FL_NOTARGET;
	Player->PrintToClient (PRINT_HIGH, "Notarget %s\n", (!(Player->Flags & FL_NOTARGET)) ? "off" : "on");
}


/*
==================
Cmd_Noclip_f

argv(0) noclip
==================
*/
void Cmd_Noclip_f (CPlayerEntity *Player)
{
	Player->NoClip = !Player->NoClip;
	Player->PrintToClient (PRINT_HIGH, "Noclip %s\n", Player->NoClip ? "on" : "off");
}

/*
=================
Cmd_Kill_f
=================
*/
void Cmd_Kill_f (CPlayerEntity *Player)
{
//ZOID
	if (Player->GetSolid() == SOLID_NOT)
		return;
//ZOID

	if((Level.Frame - Player->Client.Timers.RespawnTime) < 50)
		return;

#if ROGUE_FEATURES
	// make sure no trackers are still hurting us.
	if (Player->Client.Timers.Tracker)
		Player->RemoveAttackingPainDaemons ();

	if (Player->Client.OwnedSphere)
	{
		Player->Client.OwnedSphere->Free ();
		Player->Client.OwnedSphere = NULL;
	}
#endif

	Player->Flags &= ~FL_GODMODE;
	Player->Health = 0;
	meansOfDeath = MOD_SUICIDE;
	Player->Die (Player, Player, 100000, vec3fOrigin);
}

/*
=================
Cmd_PutAway_f
=================
*/
void Cmd_PutAway_f (CPlayerEntity *Player)
{
	Player->Client.LayoutFlags &= ~LF_SCREEN_MASK;

	if (Player->Client.Respawn.MenuState.InMenu)
		Player->Client.Respawn.MenuState.CloseMenu ();

	Player->Client.LayoutFlags |= LF_UPDATECHASE;
}


sint32 PlayerSort (void const *a, void const *b)
{
	sint32 anum = Game.Clients[*(sint32 *)a].playerState.stats[STAT_FRAGS];
	sint32 bnum = Game.Clients[*(sint32 *)b].playerState.stats[STAT_FRAGS];

	if (anum < bnum)
		return -1;
	if (anum > bnum)
		return 1;
	return 0;
}

class CPlayerListCountCallback : public CForEachPlayerCallback
{
public:
	sint32		*index;
	sint32		*count;

	CPlayerListCountCallback (sint32 *index, sint32 *count) :
	index(index),
	count(count)
	{
	};

	void Callback (CPlayerEntity *Player)
	{
		index[(*count)++] = Index;
	}
};

/*
=================
Cmd_Players_f
=================
*/
void Cmd_Players_f (CPlayerEntity *Player)
{
	sint32		count = 0;
	char	Small[MAX_INFO_KEY];
	char	Large[MAX_INFO_STRING];
	sint32		*index = QNew (TAG_GENERIC) sint32[Game.MaxClients];

	CPlayerListCountCallback (index, &count).Query ();

	// sort by frags
	qsort (index, count, sizeof(index[0]), PlayerSort);

	// print information
	Large[0] = Small[0] = 0;

	for (sint32 i = 0; i < count; i++)
	{
		CPlayerEntity *LoopPlayer = entity_cast<CPlayerEntity>(Game.Entities[i+1].Entity);
		Q_snprintfz (Small, sizeof(Small), "%3i %s\n",
			LoopPlayer->Client.PlayerState.GetStat(STAT_FRAGS),
			LoopPlayer->Client.Persistent.Name.c_str());
		if (strlen (Small) + strlen(Large) > sizeof(Large) - 100 )
		{	// can't print all of them in one packet
			Q_strcatz (Large, "...\n", MAX_INFO_STRING);
			break;
		}
		Q_strcatz (Large, Small, MAX_INFO_STRING);
	}

	Player->PrintToClient (PRINT_HIGH, "%s\n%i players\n", Large, count);

	QDelete[] index;
}

/*
=================
Cmd_Wave_f
=================
*/
void Cmd_Wave_f (CPlayerEntity *Player)
{
	// can't wave when ducked
	if (Player->Client.PlayerState.GetPMove()->pmFlags & PMF_DUCKED)
		return;

	if (Player->Client.Anim.Priority > ANIM_WAVE)
		return;

	struct waveAnimations_t
	{
		const char	*Name;
		uint16		StartFrame, EndFrame;
	} WaveAnims [] =
	{
		{ "flipoff", FRAME_flip01 - 1, FRAME_flip12 },
		{ "salute", FRAME_salute01 - 1, FRAME_salute11 },
		{ "taunt", FRAME_taunt01 - 1, FRAME_taunt17 },
		{ "wave", FRAME_wave01 - 1, FRAME_wave11 },
		NULL
	};
	static const uint32 lastIndex = ArrayCount(WaveAnims) - 1;
	uint8 WaveIndex = (ArgCount() > 1) ? 
		(ArgGeti(1) >= lastIndex) ? lastIndex-1 : ArgGeti(1)
		: 0;

	Player->PrintToClient (PRINT_HIGH, "%s\n", WaveAnims[WaveIndex].Name);
	Player->State.GetFrame() = WaveAnims[WaveIndex].StartFrame;
	Player->Client.Anim.EndFrame = WaveAnims[WaveIndex].EndFrame;
	Player->Client.Anim.Priority = ANIM_WAVE;
}

/*
==================
Cmd_Say_f
==================
*/
#define MAX_TALK_STRING 100

bool CheckFlood(CPlayerEntity *Player)
{
	if (CvarList[CV_FLOOD_MSGS].Integer())
	{
		if (Level.Frame < Player->Client.Flood.LockTill)
		{
			Player->PrintToClient (PRINT_HIGH, "You can't talk for %d more seconds\n",
				(sint32)((Player->Client.Flood.LockTill - Level.Frame)/10));
			return true;
		}
		sint32 i = Player->Client.Flood.WhenHead - CvarList[CV_FLOOD_MSGS].Integer() + 1;
		if (i < 0)
			i = (sizeof(Player->Client.Flood.When)/sizeof(Player->Client.Flood.When[0])) + i;
		if (Player->Client.Flood.When[i] && 
			((Level.Frame - Player->Client.Flood.When[i])/10) < CvarList[CV_FLOOD_PER_SECOND].Integer())
		{
			Player->Client.Flood.LockTill = Level.Frame + (CvarList[CV_FLOOD_DELAY].Float() * 10);
			Player->PrintToClient (PRINT_CHAT, "Flood protection:  You can't talk for %d seconds.\n",
				CvarList[CV_FLOOD_DELAY].Integer());
			return true;
		}
		Player->Client.Flood.WhenHead = (Player->Client.Flood.WhenHead + 1) %
			(sizeof(Player->Client.Flood.When)/sizeof(Player->Client.Flood.When[0]));
		Player->Client.Flood.When[Player->Client.Flood.WhenHead] = Level.Frame;
	}
	return false;
}

class CSayPlayerCallback : public CForEachPlayerCallback
{
public:
	std::string &Text;

	CSayPlayerCallback (std::string &Text) :
	Text(Text)
	{
	};

	CSayPlayerCallback &operator= (CSayPlayerCallback&) { return *this; }

	void Callback (CPlayerEntity *Player)
	{
		Player->PrintToClient (PRINT_CHAT, "%s", Text.c_str());
	}
};

void Cmd_Say_f (CPlayerEntity *Player, bool team, bool arg0)
{
	//char	text[MAX_TALK_STRING];
	static std::string text;

	if (ArgCount () < 2 && !arg0)
		return;

	if (Bans.IsSquelched(Player->Client.Persistent.IP) || Bans.IsSquelched(Player->Client.Persistent.Name.c_str()))
	{
		Player->PrintToClient (PRINT_HIGH, "You are squelched and may not talk.\n");
		return;
	}

	if (!(DeathmatchFlags.dfSkinTeams.IsEnabled() || DeathmatchFlags.dfModelTeams.IsEnabled()))
		team = false;

	text = (team) ? ("(" + Player->Client.Persistent.Name + "): ") : (Player->Client.Persistent.Name + ": ");

	if (arg0)
		text += ArgGets(0) + " " + ArgGetConcatenatedString();
	else
	{
		std::string p = ArgGetConcatenatedString();

		if (p[0] == '"')
		{
			p.erase (0, 1);
			p.erase (p.end()-1);
		}

#if CLEANCODE_IRC
		if (!p.empty() && p[0] == '!' && Player->Client.Respawn.IRC.Connected())
		{
			Player->Client.Respawn.IRC.SendMessage (p.substr (1));
			return;
		}
#endif

		text += p;
	}

	// don't let text be too long for malicious reasons
	if (text.length() >= MAX_TALK_STRING-1)
		text.erase (MAX_TALK_STRING-1);

	text += "\n";

	if (CheckFlood(Player))
		return;

	if (CvarList[CV_DEDICATED].Integer())
		ServerPrintf ("%s", text.c_str());

	CSayPlayerCallback (text).Query ();
}

class CPlayerListCallback : public CForEachPlayerCallback
{
public:
	char			*Text;
	size_t			SizeOf;
	CPlayerEntity	*Ent;
	bool			Spectator;

	CPlayerListCallback (char *Text, size_t SizeOf, CPlayerEntity *Ent) :
	Text(Text),
	SizeOf(SizeOf),
	Ent(Ent),
	Spectator(false)
	{
	};

	bool DoCallback (CPlayerEntity *Player)
	{
		char tempString[160];

		if (!Spectator)
			Q_snprintfz(tempString, sizeof(tempString), " - %02d:%02d %4d %3d %s\n",
				(Level.Frame - Player->Client.Respawn.EnterFrame) / 600,
				((Level.Frame - Player->Client.Respawn.EnterFrame) % 600)/10,
				Player->Client.GetPing(),
				Player->Client.Respawn.Score,
				Player->Client.Persistent.Name.c_str());
		else
			Q_snprintfz(tempString, sizeof(tempString), " - %s%s\n",
				Player->Client.Persistent.Name.c_str(),
				Player->Client.Respawn.Spectator ? " (Spectator)" : "");

		if (strlen(Text) + strlen(tempString) > SizeOf - 50)
		{
			Q_snprintfz (Text+strlen(Text), SizeOf, "And more...\n");
			Ent->PrintToClient (PRINT_HIGH, "%s", Text);
			return true;
		}

		Q_strcatz(Text, tempString, SizeOf);
		return false;
	}

	bool DoQuery (bool Spectator)
	{
		this->Spectator = Spectator;
		for (uint8 i = 1; i <= Game.MaxClients; i++)
		{
			CPlayerEntity *Player = entity_cast<CPlayerEntity>(Game.Entities[i].Entity);

			if (Spectator && (!Player->GetInUse() || Player->Client.Persistent.State == SVCS_SPAWNED))
				continue;
			else if (!Spectator && (Player->Client.Persistent.State != SVCS_SPAWNED))
				continue;

			Index = i;
			if (DoCallback (Player))
				return true;
		}
		return false;
	}

	void Query (bool)
	{
	}

	void Callback (CPlayerEntity *)
	{
	}
};

void Cmd_PlayerList_f(CPlayerEntity *Player)
{
	char text[MAX_COMPRINT/4];

	// connect time, ping, Score, name
	*text = 0;

	Q_snprintfz (text, sizeof(text), "Spawned:\n");
	CPlayerListCallback(text, sizeof(text), Player).DoQuery (false);

	Q_strcatz (text, "Connecting:\n", sizeof(text));
	if (!CPlayerListCallback(text, sizeof(text), Player).DoQuery (true))
		Player->PrintToClient (PRINT_HIGH, "%s", text);
}

void GCmd_Say_f (CPlayerEntity *Player)
{
	Cmd_Say_f (Player, false, false);
}

void GCmd_SayTeam_f (CPlayerEntity *Player)
{
	Cmd_Say_f (Player, true, false);
}

/*
==================
Cmd_Score_f

Display the scoreboard
==================
*/
void Cmd_Score_f (CPlayerEntity *Player)
{
	Player->Client.LayoutFlags &= ~(LF_SHOWINVENTORY | LF_SHOWHELP);
	if (Player->Client.Respawn.MenuState.InMenu)
	{
		Player->Client.Respawn.MenuState.CloseMenu();
		return;
	}

	if (Game.GameMode == GAME_SINGLEPLAYER)
		return;

	if (Player->Client.LayoutFlags & LF_SHOWSCORES)
	{
		Player->Client.LayoutFlags &= ~LF_SHOWSCORES;
		Player->Client.LayoutFlags |= LF_UPDATECHASE;
		return;
	}

	Player->Client.LayoutFlags |= LF_SHOWSCORES;
	Player->DeathmatchScoreboardMessage (true);
}

/*
==================
Cmd_Help_f

Display the current help message
==================
*/
void Cmd_Help_f (CPlayerEntity *Player)
{
	if (Level.IntermissionTime)
		return;

	// this is for backwards compatability
	if (Game.GameMode & GAME_DEATHMATCH)
	{
		Cmd_Score_f (Player);
		return;
	}

	Player->Client.LayoutFlags &= ~(LF_SHOWSCORES | LF_SHOWINVENTORY);
	if (Player->Client.Respawn.MenuState.InMenu)
	{
		Player->Client.Respawn.MenuState.CloseMenu();
		return;
	}

	if ((Player->Client.LayoutFlags & LF_SHOWHELP) && (Player->Client.Persistent.GameHelpChanged == Game.HelpChanged))
	{
		Player->Client.LayoutFlags &= ~LF_SHOWHELP;
		return;
	}

	Player->Client.LayoutFlags |= LF_SHOWHELP;
	Player->Client.Persistent.HelpChanged = 0;
	HelpComputer (Player);
}

void GCTFSay_Team (CPlayerEntity *Player);
void Cmd_MenuLeft_t (CPlayerEntity *Player);
void Cmd_MenuRight_t (CPlayerEntity *Player);

void Cmd_Register ()
{
	// These commands are generic, and can be executed any time
	// during play, even during intermission and by spectators.
	Cmd_AddCommand ("players",				Cmd_Players_f,			CMD_SPECTATOR);
	Cmd_AddCommand ("say",					GCmd_Say_f,				CMD_SPECTATOR);
	Cmd_AddCommand ("Score",				Cmd_Score_f,			CMD_SPECTATOR);
	Cmd_AddCommand ("help",					Cmd_Help_f,				CMD_SPECTATOR);
	Cmd_AddCommand ("putaway",				Cmd_PutAway_f,			CMD_SPECTATOR);
	Cmd_AddCommand ("playerlist",			
#if CLEANCTF_ENABLED
		CTFPlayerList,		
#else
		Cmd_PlayerList_f,		
#endif
		CMD_SPECTATOR);

	// These commands are also generic, but can only be executed
	// by in-game players during the game
	Cmd_AddCommand ("kill",					Cmd_Kill_f);
	Cmd_AddCommand ("wave",					Cmd_Wave_f);

	Cmd_AddCommand ("use",					Cmd_Use_f);
	Cmd_AddCommand ("uselist",				Cmd_UseList_f); // Paril
	Cmd_AddCommand ("drop",					Cmd_Drop_f);
	Cmd_AddCommand ("inven",				Cmd_Inven_f);
	Cmd_AddCommand ("invuse",				Cmd_InvUse_f);
	Cmd_AddCommand ("invdrop",				Cmd_InvDrop_f);
	Cmd_AddCommand ("weapprev",				Cmd_WeapPrev_f);
	Cmd_AddCommand ("weapnext",				Cmd_WeapNext_f);
	Cmd_AddCommand ("weaplast",				Cmd_WeapLast_f);
	Cmd_AddCommand ("invnext",				Cmd_SelectNextItem_f);
	Cmd_AddCommand ("invprev",				Cmd_SelectPrevItem_f);
	Cmd_AddCommand ("invnextw",				Cmd_SelectNextWeapon_f);
	Cmd_AddCommand ("invprevw",				Cmd_SelectPrevWeapon_f);
	Cmd_AddCommand ("invnextp",				Cmd_SelectNextPowerup_f);
	Cmd_AddCommand ("invprevp",				Cmd_SelectPrevPowerup_f);
#if _DEBUG
	AddTestDebugCommands ();
#endif

	// And last but certainly not least..
	Cmd_AddCommand ("god",					Cmd_God_f,				CMD_CHEAT);
	Cmd_AddCommand ("notarget",				Cmd_Notarget_f,			CMD_CHEAT);
	Cmd_AddCommand ("noclip",				Cmd_Noclip_f,			CMD_CHEAT);
	Cmd_AddCommand ("give",					Cmd_Give_f,				CMD_CHEAT);
	Cmd_AddCommand ("spawn",				Cmd_Give,				CMD_CHEAT);
	
	// CleanMenu commands
	Cmd_AddCommand ("menu_left",			Cmd_MenuLeft_t,			CMD_SPECTATOR);
	Cmd_AddCommand ("menu_right",			Cmd_MenuRight_t,		CMD_SPECTATOR);
	Cmd_AddCommand ("cc_version",			Cmd_CCVersion_t,		CMD_SPECTATOR);

#if CLEANCTF_ENABLED
	Cmd_AddCommand ("say_team",				GCTFSay_Team,			CMD_SPECTATOR);
	Cmd_AddCommand ("team",					CTFTeam_f);
	Cmd_AddCommand ("id",					CTFID_f);
	Cmd_AddCommand ("yes",					CTFVoteYes);
	Cmd_AddCommand ("no",					CTFVoteNo);
	Cmd_AddCommand ("ready",				CTFReady);
	Cmd_AddCommand ("notready",				CTFNotReady);
	Cmd_AddCommand ("ghost",				CTFGhost);
	Cmd_AddCommand ("admin",				CTFAdmin);
	Cmd_AddCommand ("stats",				CTFStats);
	Cmd_AddCommand ("warp",					CTFWarp);
	Cmd_AddCommand ("boot",					CTFBoot);
	Cmd_AddCommand ("observer",				CTFObserver);
#endif

#if CLEANCODE_IRC
	Cmd_AddCommand ("irc",					Cmd_Irc_t)
		.AddSubCommand ("connect",			Cmd_Irc_Connect_t,		0).GoUp()
		.AddSubCommand ("join",				Cmd_Irc_Join_t,			0).GoUp()
		.AddSubCommand ("say",				Cmd_Irc_Say_t,			0).GoUp()
		.AddSubCommand ("disconnect",		Cmd_Irc_Disconnect_t,	0).GoUp()
		.AddSubCommand ("leave",			Cmd_Irc_Leave_t,		0).GoUp()
		.AddSubCommand ("list",				Cmd_Irc_List_t,			0);
#endif
}

/*
=================
ClientCommand
=================
*/
void CGameAPI::ClientCommand (CPlayerEntity *Player)
{
	InitArg ();
	Cmd_RunCommand (ArgGets(0).c_str(), Player);
	EndArg ();
}

