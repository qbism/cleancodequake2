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

#if !defined(CC_GUARD_CTF_H) || !INCLUDE_GUARDS
#define CC_GUARD_CTF_H

inline const char *CTF_VERSION() { return "1.09b"; }

const int CONFIG_CTF_MATCH  = (CS_MAXCLIENTS-1);

/**
\typedef	sint8 ETeamIndex

\brief	Defines an alias representing index of a team.
**/
typedef sint8 ETeamIndex;

/**
\enum	

\brief	Values that represent teams. 
**/
enum
{
	CTF_NOTEAM,
	CTF_TEAM1,
	CTF_TEAM2,
	CTF_TEAMNUM,

	CTF_RED = CTF_TEAM1,
	CTF_BLUE = CTF_TEAM2,
};

/**
\typedef	uint8 EGrappleState

\brief	Defines an alias representing state of the grappling hook.
**/
typedef uint8 EGrappleState;

/**
\enum	

\brief	Values that represent the state of a grappling hook. 
**/
enum
{
	CTF_GRAPPLE_STATE_FLY,
	CTF_GRAPPLE_STATE_PULL,
	CTF_GRAPPLE_STATE_HANG
};

class CCTFGhost
{
public:
	uint32	Code;

	std::string name;
	sint32 number;

	// stats
	sint32 deaths;
	sint32 kills;
	sint32 caps;
	sint32 basedef;
	sint32 carrierdef;

	sint32 team; // team
	sint32 Score; // frags at time of disconnect
	CPlayerEntity *Player;
};

/**
\typedef	uint8 EMatchState

\brief	Defines an alias representing the state of a competition match.
**/
typedef uint8 EMatchState;

/**
\enum	

\brief	Values that represent the state of a competition match. 
**/
enum
{
	MATCH_NONE,
	MATCH_SETUP,
	MATCH_PREGAME,
	MATCH_GAME,
	MATCH_POST
};

/**
\typedef	uint8 EElectState

\brief	Defines an alias representing state of an election.
**/
typedef uint8 EElectState;

/**
\enum	

\brief	Values that represent the state of an election. 
**/
enum
{
	ELECT_NONE,
	ELECT_MATCH,
	ELECT_ADMIN,
	ELECT_MAP
};

typedef std::map<uint32, CCTFGhost*> TGhostMapType;
class CCTFGameLocals
{
public:
	CCTFGameLocals ()
	  {
		  Clear ();
	  }

	sint32 team1, team2;
	sint32 total1, total2; // these are only set when going into intermission!
	FrameNumber last_flag_capture;
	sint32 last_capture_team;

	EMatchState match;		// match state
	float matchtime;	// time for match start/end (depends on state)
	sint32 lasttime;		// last time update

	EElectState election;	// election type
	CPlayerEntity *etarget;	// for CTF.Admin election, who's being elected
	char elevel[32];	// for map election, target level
	sint32 evotes;			// votes so far
	sint32 needvotes;		// votes needed
	FrameNumber electtime;	// remaining time until election times out
	char emsg[256];		// election name

	TGhostMapType Ghosts; // ghost codes

	void Clear ()
	{
		team1 = 0;
		team2 = 0;
		total1 = 0;
		total2 = 0;
		last_flag_capture = 0;
		last_capture_team = 0;
		match = 0;
		matchtime = 0;
		lasttime = 0;
		election = 0;
		etarget = NULL;
		evotes = 0;
		needvotes = 0;
		electtime = 0;

		for (TGhostMapType::iterator it = Ghosts.begin(); it != Ghosts.end(); ++it)
			QDelete (*it).second;
		Ghosts.clear();

		Mem_Zero (&elevel, sizeof(elevel));
		Mem_Zero (&emsg, sizeof(emsg));
	}
};

inline const char *CTF_TEAM1_SKIN() { return "ctf_r"; }
inline const char *CTF_TEAM2_SKIN() { return "ctf_b"; }

const int CTF_CAPTURE_BONUS						= 15;	// what you get for capture
const int CTF_TEAM_BONUS						= 10;	// what your team gets for capture
const int CTF_RECOVERY_BONUS					= 1;	// what you get for recovery
const int CTF_FLAG_BONUS						= 0;	// what you get for picking up enemy flag
const int CTF_FRAG_CARRIER_BONUS				= 2;	// what you get for fragging enemy flag carrier
const int CTF_FLAG_RETURN_TIME					= 400;	// seconds until auto return

const int CTF_CARRIER_DANGER_PROTECT_BONUS		= 2;	// bonus for fraggin someone who has recently hurt your flag carrier
const int CTF_CARRIER_PROTECT_BONUS				= 1;	// bonus for fraggin someone while either you or your target are near your flag carrier
const int CTF_FLAG_DEFENSE_BONUS				= 1;	// bonus for fraggin someone while either you or your target are near your flag
const int CTF_RETURN_FLAG_ASSIST_BONUS			= 1;	// awarded for returning a flag that causes a capture to happen almost immediately
const int CTF_FRAG_CARRIER_ASSIST_BONUS			= 2;	// award for fragging a flag carrier if a capture happens almost immediately

const int CTF_TARGET_PROTECT_RADIUS				= 400;	// the radius around an object being defended where a target will be worth extra frags
const int CTF_ATTACKER_PROTECT_RADIUS			= 400;	// the radius around an object being defended where an Attacker will get extra frags when making kills

const int CTF_CARRIER_DANGER_PROTECT_TIMEOUT	= 80;
const int CTF_FRAG_CARRIER_ASSIST_TIMEOUT		= 100;
const int CTF_RETURN_FLAG_ASSIST_TIMEOUT		= 100;

const int CTF_AUTO_FLAG_RETURN_TIMEOUT			= 300;	// number of seconds before dropped flag auto-returns

const int CTF_GRAPPLE_SPEED						= 650; // speed of grapple in flight
const int CTF_GRAPPLE_PULL_SPEED				= 650;	// speed player is pulled at

void CTFInit();
void CTFSpawn();

inline const char *CTFTeamName(ETeamIndex team)
{
	switch (team)
	{
	case CTF_TEAM1:
		return "RED";
	case CTF_TEAM2:
		return "BLUE";
	default:
		return "UNKNOWN";
	}
}

inline const char *CTFOtherTeamName(ETeamIndex team)
{
	switch (team)
	{
	case CTF_TEAM1:
		return "BLUE";
	case CTF_TEAM2:
		return "RED";
	default:
		return "UNKNOWN";
	}
}

inline ETeamIndex CTFOtherTeam(ETeamIndex team)
{
	switch (team)
	{
	case CTF_TEAM1:
		return CTF_TEAM2;
	case CTF_TEAM2:
		return CTF_TEAM1;
	default:
		return -1;
	}
}


void CTFCalcScores();
void CTFResetFlag(ETeamIndex Team);
void CTFFragBonuses(CPlayerEntity *targ, CPlayerEntity *Attacker);
void CTFCheckHurtCarrier(CPlayerEntity *targ, CPlayerEntity *Attacker);
void CTFOpenJoinMenu(CPlayerEntity *Player);
bool CTFNextMap();
bool CTFMatchSetup();
bool CTFMatchOn();
bool CTFInMatch();
void CTFSay_Team(CPlayerEntity *who, char *msg);

//TECH
class CCTFSayTeamCommand : public CGameCommandFunctor
{
public:
	void operator () ();
};

class CCTFTeamCommand : public CGameCommandFunctor
{
public:
	void operator () ();
};

class CCTFIDCommand : public CGameCommandFunctor
{
public:
	void operator () ();
};

class CCTFVoteYesCommand : public CGameCommandFunctor
{
public:
	void operator () ();
};

class CCTFVoteNoCommand : public CGameCommandFunctor
{
public:
	void operator () ();
};

class CCTFReadyCommand : public CGameCommandFunctor
{
public:
	void operator () ();
};

class CCTFNotReadyCommand : public CGameCommandFunctor
{
public:
	void operator () ();
};

class CCTFGhostCommand : public CGameCommandFunctor
{
public:
	void operator () ();
};

class CCTFAdminCommand : public CGameCommandFunctor
{
public:
	void operator () ();
};

class CCTFStatsCommand : public CGameCommandFunctor
{
public:
	void operator () ();
};

class CCTFWarpCommand : public CGameCommandFunctor
{
public:
	void operator () ();
};

class CCTFBootCommand : public CPlayerListCommand
{
public:
	void operator () ();
};

class CCTFObserverCommand : public CPlayerListCommand
{
public:
	void operator () ();
};

class CCTFPlayerListCommand : public CPlayerListCommand
{
public:
	void operator () ();
};

bool CTFCheckRules();
void CreateCTFStatusbar ();

extern CCTFGameLocals ctfgame;

#else
FILE_WARNING
#endif
