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
#include "cc_menu.h"
#include "cc_version.h"

/**
\class	CGodCommand

\brief	God command functor.
		Toggles godmode.

\author	Paril
\date	29/05/2010
**/
class CGodCommand : public CGameCommandFunctor
{
public:
	void operator () ()
	{
		Player->Flags ^= FL_GODMODE;
		Player->PrintToClient (PRINT_HIGH, "God mode %s\n", (!(Player->Flags & FL_GODMODE)) ? "off" : "on");
	}
};

/**
\class	CNoTargetCommand

\brief	Notarget command functor.
		Toggles notarget.

\author	Paril
\date	29/05/2010
**/
class CNoTargetCommand : public CGameCommandFunctor
{
public:
	void operator () ()
	{
		Player->Flags ^= FL_NOTARGET;
		Player->PrintToClient (PRINT_HIGH, "Notarget %s\n", (!(Player->Flags & FL_NOTARGET)) ? "off" : "on");
	}
};

/**
\class	CNoClipCommand

\brief	Noclip command functor.
		Toggles noclip.

\author	Paril
\date	29/05/2010
**/
class CNoClipCommand : public CGameCommandFunctor
{
public:
	void operator () ()
	{
		Player->NoClip = !Player->NoClip;
		Player->PrintToClient (PRINT_HIGH, "Noclip %s\n", Player->NoClip ? "on" : "off");
	};
};

/**
\class	CKillCommand

\brief	Kill command functor.
		Kills self.

\author	Paril
\date	29/05/2010
**/
class CKillCommand : public CGameCommandFunctor
{
public:
	void operator () ()
	{
		if (Player->GetSolid() == SOLID_NOT)
			return;

		if ((Level.Frame - Player->Client.Timers.RespawnTime) < 50)
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
	};
};

/**
\class	CPutAwayCommand

\brief	Putaway command functor.
		Only called by server; closes menus.

\author	Paril
\date	29/05/2010
**/
class CPutAwayCommand : public CGameCommandFunctor
{
public:
	void operator () ()
	{
		Player->Client.LayoutFlags &= ~LF_SCREEN_MASK;

		if (Player->Client.Respawn.MenuState.InMenu)
			Player->Client.Respawn.MenuState.CloseMenu ();

		Player->Client.LayoutFlags |= LF_UPDATECHASE;
	};
};

/**
\fn	bool PlayerSort (const int a, const int b)

\brief	Player sort. 

\author	Paril
\date	26/05/2010

\param	a	First integer. 
\param	b	Second integer. 

\return	true if it succeeds, false if it fails. 
**/
bool PlayerSort (const int a, const int b)
{
	return (Game.Clients[a].PlayerState.Stats[STAT_FRAGS] > Game.Clients[b].PlayerState.Stats[STAT_FRAGS]);
}

/**
\class	CPlayerListCountCallback

\brief	Player list count callback.
		Pushes back all player indexes into a vector.

\author	Paril
\date	29/05/2010
**/
class CPlayerListCountCallback : public CForEachPlayerCallback
{
public:
	std::vector<int>	&index;

	CPlayerListCountCallback (std::vector<int> &index) :
	index(index)
	{
	};

	CPlayerListCountCallback &operator = (CPlayerListCountCallback &) {return *this;}

	void Callback (CPlayerEntity *Player)
	{
		index.push_back (Index);
	}
};

/**
\class	CPlayersCommand

\brief	Players command functor.
		Lists players and frags.

\author	Paril
\date	29/05/2010
**/
class CPlayersCommand : public CGameCommandFunctor
{
public:
	void operator () ()
	{
		sint32		count = 0;
		std::vector<int> index;

		CPlayerListCountCallback (index).Query ();

		// sort by frags
		std::sort (index.begin(), index.end(), PlayerSort);

		// print information
		std::string Large;

		for (sint32 i = 0; i < count; i++)
		{
			CPlayerEntity *LoopPlayer = entity_cast<CPlayerEntity>(Game.Entities[i+1].Entity);
			std::string Small = FormatString ("%3i %s\n",
				LoopPlayer->Client.PlayerState.GetStat(STAT_FRAGS),
				LoopPlayer->Client.Persistent.Name.c_str());

			if (Small.size() > 128)
			{
				// can't print all of them in one packet
				Large += "...\n";
				break;
			}
			
			Large += Small;
		}

		Player->PrintToClient (PRINT_HIGH, "%s\n%i players\n", Large.c_str(), count);
	}
};

/**
\class	CWaveCommand

\brief	Wave command functor.
		Performs an animation.

\author	Paril
\date	29/05/2010
**/
class CWaveCommand : public CGameCommandFunctor
{
public:
	void operator () ()
	{
		// can't wave when ducked
		if (Player->Client.PlayerState.GetPMove()->PMoveFlags & PMF_DUCKED)
			return;

		if (Player->Client.Anim.Priority > ANIM_WAVE)
			return;

		struct SWaveAnimations
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
	};
};

/*
==================
Cmd_Say_f
==================
*/
const int MAX_TALK_STRING = 100;

/**
\class	CSayPlayerCallback

\brief	Say player callback.
		Prints the text to each player.

\author	Paril
\date	29/05/2010
**/
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

/**
\class	CCmdSayCommandBase

\brief	Say command base.
		Prints typed text to players.
		Just provides the team,arg0 function.

\author	Paril
\date	29/05/2010
**/
class CCmdSayCommandBase : public CGameCommandFunctor
{
public:
	void operator () (bool team, bool arg0)
	{
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

		if (Player->CheckFlood())
			return;

		if (CvarList[CV_DEDICATED].Integer())
			ServerPrintf ("%s", text.c_str());

		CSayPlayerCallback (text).Query ();
	}

	virtual void operator () () = 0;
};

/**
\class	CCmdSayCommand

\brief	Say command.
		Performs the say action.

\author	Paril
\date	29/05/2010
**/
class CCmdSayCommand : public CCmdSayCommandBase
{
public:
	void operator () ()
	{
		CCmdSayCommandBase::operator() (false, false);
	};
};

/**
\class	CCmdSayTeamCommand

\brief	Say team command.
		Performs the say team action.

\author	Paril
\date	29/05/2010
**/
class CCmdSayTeamCommand : public CCmdSayCommandBase
{
public:
	void operator () ()
	{
		CCmdSayCommandBase::operator() (true, false);
	};
};

/**
\class	CPlayerListCallback

\brief	Player list callback.
		Puts together a text string to store
		player data on every player.

\author	Paril
\date	29/05/2010
**/
class CPlayerListCallback : public CForEachPlayerCallback
{
public:
	std::string		&Text;
	size_t			SizeOf;
	CPlayerEntity	*Ent;
	bool			Spectator;

	CPlayerListCallback (std::string &Text, size_t SizeOf, CPlayerEntity *Ent) :
	Text(Text),
	SizeOf(SizeOf),
	Ent(Ent),
	Spectator(false)
	{
	};

	CPlayerListCallback& operator = (CPlayerListCallback &) {return *this;}

	bool DoCallback (CPlayerEntity *Player)
	{
		static std::string tempString;
		tempString.clear();

		if (!Spectator)
			tempString = FormatString(" - %02d:%02d %4d %3d %s\n",
				(Level.Frame - Player->Client.Respawn.EnterFrame) / 600,
				((Level.Frame - Player->Client.Respawn.EnterFrame) % 600)/10,
				Player->Client.GetPing(),
				Player->Client.Respawn.Score,
				Player->Client.Persistent.Name.c_str());
		else
			tempString = FormatString(" - %s%s\n",
				Player->Client.Persistent.Name.c_str(),
				Player->Client.Respawn.Spectator ? " (Spectator)" : "");

		if (Text.size() + tempString.size() > SizeOf - 50)
		{
			Text += "And more...\n";
			Ent->PrintToClient (PRINT_HIGH, "%s", Text.c_str());
			return true;
		}

		Text += tempString;
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

/**
\fn	void CPlayerListCommand::operator() ()

\brief	Player list command functor.
		Does the actual printing.

\author	Paril
\date	29/05/2010
**/
void CPlayerListCommand::operator () ()
{
	static std::string text;
	text.clear();
		
	// connect time, ping, Score, name
	text = "Spawned:\n";
	CPlayerListCallback(text, sizeof(text), Player).DoQuery (false);

	text = "Connecting:\n";
	if (!CPlayerListCallback(text, sizeof(text), Player).DoQuery (true))
		Player->PrintToClient (PRINT_HIGH, "%s", text.c_str());
};

/**
\class	CScoreCommand

\brief	Score command functor.
		Displays scoreboard.

\author	Paril
\date	29/05/2010
**/
class CScoreCommand : public CGameCommandFunctor
{
public:
	void operator () ()
	{
		Player->ShowScores();
	}
};

/**
\class	CHelpCommand

\brief	Help command functor.
		Displays the help computer.

\author	Paril
\date	29/05/2010
**/
class CHelpCommand : public CGameCommandFunctor
{
public:
	void operator () ()
	{
		Player->ShowHelp();
	};
};

/**
\fn	void Cmd_Register ()

\brief	Registers all game commands.

\author	Paril
\date	29/05/2010
**/
void Cmd_Register ()
{
	// These commands are generic, and can be executed any time
	// during play, even during intermission and by spectators.
	Cmd_AddCommand<CPlayersCommand> ("players",			CMD_SPECTATOR);
	Cmd_AddCommand<CCmdSayCommand> ("say",				CMD_SPECTATOR);
	Cmd_AddCommand<CScoreCommand> ("score",				CMD_SPECTATOR);
	Cmd_AddCommand<CHelpCommand> ("help",				CMD_SPECTATOR);
	Cmd_AddCommand<CPutAwayCommand> ("putaway",			CMD_SPECTATOR);
	Cmd_AddCommand
#if CLEANCTF_ENABLED
		<CCTFPlayerListCommand>		
#else
		<CPlayerListCommand>	
#endif
			("playerlist",			CMD_SPECTATOR);

	// These commands are also generic, but can only be executed
	// by in-game players during the game
	Cmd_AddCommand<CKillCommand> ("kill");
	Cmd_AddCommand<CWaveCommand> ("wave");

	Cmd_AddCommand<CUseCommand> ("use");
	Cmd_AddCommand<CUseListCommand> ("uselist");
	Cmd_AddCommand<CDropCommand> ("drop");
	Cmd_AddCommand<CInventoryCommand> ("inven");
	Cmd_AddCommand<CInvUseCommand> ("invuse");
	Cmd_AddCommand<CInvDropCommand> ("invdrop");
	Cmd_AddCommand<CWeapPrevCommand> ("weapprev");
	Cmd_AddCommand<CWeapNextCommand> ("weapnext");
	Cmd_AddCommand<CWeapLastCommand> ("weaplast");
	Cmd_AddCommand<CInvNextCommand> ("invnext");
	Cmd_AddCommand<CInvPrevCommand> ("invprev");
	Cmd_AddCommand<CInvNextWCommand> ("invnextw");
	Cmd_AddCommand<CInvPrevWCommand> ("invprevw");
	Cmd_AddCommand<CInvNextPCommand> ("invnextp");
	Cmd_AddCommand<CInvPrevPCommand> ("invprevp");
#if _DEBUG
	AddTestDebugCommands ();
#endif

	// And last but certainly not least..
	Cmd_AddCommand<CGodCommand> ("god",				CMD_CHEAT);
	Cmd_AddCommand<CNoTargetCommand> ("notarget",			CMD_CHEAT);
	Cmd_AddCommand<CNoClipCommand> ("noclip",				CMD_CHEAT);
	Cmd_AddCommand<CGiveCommand> ("give",				CMD_CHEAT);
	Cmd_AddCommand<CSpawnCommand> ("spawn",				CMD_CHEAT);
	
	// CleanMenu commands
	Cmd_AddCommand<CMenuLeftCommand> ("menu_left",			CMD_SPECTATOR);
	Cmd_AddCommand<CMenuRightCommand> ("menu_right",			CMD_SPECTATOR);
	Cmd_AddCommand<CVersionCommand> ("cc_version",			CMD_SPECTATOR);

#if CLEANCTF_ENABLED
	Cmd_AddCommand<CCTFSayTeamCommand> ("say_team");
	Cmd_AddCommand<CCTFTeamCommand> ("team");
	Cmd_AddCommand<CCTFIDCommand> ("id");
	Cmd_AddCommand<CCTFVoteYesCommand> ("yes");
	Cmd_AddCommand<CCTFVoteNoCommand> ("no");
	Cmd_AddCommand<CCTFAdminCommand> ("admin");
	Cmd_AddCommand<CCTFWarpCommand> ("warp");
	Cmd_AddCommand<CCTFBootCommand> ("boot");
	Cmd_AddCommand<CCTFObserverCommand> ("observer");
#else
	Cmd_AddCommand<CCmdSayTeamCommand> ("say_team");
#endif

#if CLEANCODE_IRC
	Cmd_AddCommand<CIRCCommand> ("irc")
		.AddSubCommand<CIRCCommand::CIRCConnectCommand> ("connect").GoUp()
		.AddSubCommand<CIRCCommand::CIRCJoinCommand> ("join").GoUp()
		.AddSubCommand<CIRCCommand::CIRCSayCommand> ("say").GoUp()
		.AddSubCommand<CIRCCommand::CIRCDisconnectCommand> ("disconnect").GoUp()
		.AddSubCommand<CIRCCommand::CIRCLeaveCommand> ("leave").GoUp()
		.AddSubCommand<CIRCCommand::CIRCListCommand> ("list");
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
