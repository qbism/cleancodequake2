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
// Voting.cpp
// Voting plugin for CleanCode
//

#include "Local.h"

typedef uint8 EVoteType;

enum
{
	VOTE_NONE,
	VOTE_MAP,
	VOTE_NEXTMAP,
	VOTE_KICK,
	VOTE_BAN
};

typedef uint8 EVoteState;

enum
{
	VOTE_UNDECIDED,
	VOTE_NO,
	VOTE_YES
};

class CVoteData : public IModuleData
{
public:
	EVoteState VoteState;
};

class CVoteStringData : public CVoteData
{
public:
	std::string		String;
};

class CVotePlayerData : public CVoteData
{
public:
	CPlayerEntity	*Player;
};

const FrameNumber VoteFrameCount = 300;

class CVote
{
public:
	bool			Voting;
	FrameNumber		EndTime;
	EVoteType		Type;
	CVoteData		*Data;
	uint8			VotesYes;
	uint8			VotesNo;

	CVote(EVoteType VoteType, CVoteData *Data = NULL) :
	  Voting(false),
	  EndTime(0),
	  Type(VoteType),
	  Data(Data),
	  VotesYes(0),
	  VotesNo(0)
	{
		if (VoteType != VOTE_NONE)
			StartVote();
	};

	void StartVote ()
	{
		EndTime = Level.Frame + VoteFrameCount;
		Voting = true;
		VotesYes = VotesNo = 0;
	};
};

static CVote CurrentVote (VOTE_NONE);

class VotingModule : public IModule
{
public:
	VotingModule () :
	  IModule ()
	  {
	  }

	const char *GetName()
	{
		return "Voting Module\n  Version 1.0\n  By Paril";
	};

	void Init ();

	void Shutdown()
	{
	};

	void RunFrame()
	{
	};

	IModuleData *ModuleDataRequested ()
	{
		return QNew(TAG_GENERIC) CVoteData;
	};
} module;

// Commands
class CCmdVoteCommand : public CGameCommandFunctor
{
public:
	void Execute ()
	{
		Player->PrintToClient (PRINT_HIGH,
			"Supported commands:\n" \
			"  vote yes        - Vote yes to a current vote\n" \
			"  vote no         - Vote no to a current vote\n" \
			"  vote view       - See the details of the vote\n" \
			"  vote map n      - Vote to change the current map\n" \
			"  vote nextmap n  - Vote to change the next map\n" \
			"  vote kick n     - Vote to kick a player\n" \
			"  vote ban n      - Vote to ban a player\n");
	}
};

void PlayerConnectTest (CPlayerEntity *Player)
{
	ServerPrintf("Sup brah");
}

void PlayerDisconnectTest (CPlayerEntity *Player)
{
	ServerPrintf("Brah, sup!");
}

class CCmdVoteMapCommand : public CGameCommandFunctor
{
public:
	void Execute()
	{
		if (ArgCount() != 3)
		{
			Player->PrintToClient (PRINT_HIGH, "Syntax: vote map mapname\n");
			return;
		}

		CVoteData *data = CModuleContainer::RequestModuleData<CVoteData>(Player, &module);
		Player->PrintToClient (PRINT_HIGH, "Your vote state is %s\n", (data->VoteState == VOTE_UNDECIDED) ? "Undecided" : (data->VoteState == VOTE_YES) ? "Yes" : "No");

		data->VoteState++;

		if (data->VoteState > VOTE_YES)
			data->VoteState = VOTE_UNDECIDED;
	}
};

void VotingModule::Init ()
{
	Cmd_AddCommand<CCmdVoteCommand> ("vote", CMD_SPECTATOR)
		.AddSubCommand<CCmdVoteMapCommand> ("map", CMD_SPECTATOR).GoUp()
		;

	PlayerEvents::PlayerConnected += PlayerConnectTest;
	PlayerEvents::PlayerDisconnected += PlayerDisconnectTest;
};