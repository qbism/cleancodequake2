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

#if !defined(__CC_CTF_H__) || !INCLUDE_GUARDS
#define __CC_CTF_H__

#define CTF_VERSION			1.09b
#define CTF_VSTRING2(x) #x
#define CTF_VSTRING(x) CTF_VSTRING2(x)
#define CTF_STRING_VERSION  CTF_VSTRING(CTF_VERSION)

#define CONFIG_CTF_MATCH (CS_MAXCLIENTS-1)

CC_ENUM (sint8, ETeamIndex)
{
	CTF_NOTEAM,
	CTF_TEAM1,
	CTF_TEAM2,
	CTF_TEAMNUM,

	CTF_RED = CTF_TEAM1,
	CTF_BLUE = CTF_TEAM2,
};

CC_ENUM (uint8, EGrappleState)
{
	CTF_GRAPPLE_STATE_FLY,
	CTF_GRAPPLE_STATE_PULL,
	CTF_GRAPPLE_STATE_HANG
};

class CCTFGhost
{
public:
	uint32	Code;

	std::cc_string name;
	sint32 number;

	// stats
	sint32 deaths;
	sint32 kills;
	sint32 caps;
	sint32 basedef;
	sint32 carrierdef;

	sint32 team; // team
	sint32 Score; // frags at time of disconnect
	CPlayerEntity *ent;
};

CC_ENUM (uint8, EMatchState)
{
	MATCH_NONE,
	MATCH_SETUP,
	MATCH_PREGAME,
	MATCH_GAME,
	MATCH_POST
};

CC_ENUM (uint8, EElectState)
{
	ELECT_NONE,
	ELECT_MATCH,
	ELECT_ADMIN,
	ELECT_MAP
};

typedef std::map<uint32, CCTFGhost*, std::less<uint16>, std::generic_allocator<std::pair <const uint32, CCTFGhost*> > > TGhostMapType;
class CCTFGameLocals
{
public:
	CCTFGameLocals ()
	  {
		  Clear ();
	  }

	sint32 team1, team2;
	sint32 total1, total2; // these are only set when going into intermission!
	FrameNumber_t last_flag_capture;
	sint32 last_capture_team;

	EMatchState match;		// match state
	float matchtime;	// time for match start/end (depends on state)
	sint32 lasttime;		// last time update

	EElectState election;	// election type
	CPlayerEntity *etarget;	// for CTF.Admin election, who's being elected
	char elevel[32];	// for map election, target level
	sint32 evotes;			// votes so far
	sint32 needvotes;		// votes needed
	FrameNumber_t electtime;	// remaining time until election times out
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

		for (TGhostMapType::iterator it = Ghosts.begin(); it != Ghosts.end(); it++)
			QDelete (*it).second;
		Ghosts.clear();

		memset (&elevel, 0, sizeof(elevel));
		memset (&emsg, 0, sizeof(emsg));
	}
};

extern CCvar *ctf;

#define CTF_TEAM1_SKIN "ctf_r"
#define CTF_TEAM2_SKIN "ctf_b"

#define CTF_CAPTURE_BONUS		15	// what you get for capture
#define CTF_TEAM_BONUS			10	// what your team gets for capture
#define CTF_RECOVERY_BONUS		1	// what you get for recovery
#define CTF_FLAG_BONUS			0	// what you get for picking up enemy flag
#define CTF_FRAG_CARRIER_BONUS	2	// what you get for fragging enemy flag carrier
#define CTF_FLAG_RETURN_TIME	400	// seconds until auto return

#define CTF_CARRIER_DANGER_PROTECT_BONUS	2	// bonus for fraggin someone who has recently hurt your flag carrier
#define CTF_CARRIER_PROTECT_BONUS			1	// bonus for fraggin someone while either you or your target are near your flag carrier
#define CTF_FLAG_DEFENSE_BONUS				1	// bonus for fraggin someone while either you or your target are near your flag
#define CTF_RETURN_FLAG_ASSIST_BONUS		1	// awarded for returning a flag that causes a capture to happen almost immediately
#define CTF_FRAG_CARRIER_ASSIST_BONUS		2	// award for fragging a flag carrier if a capture happens almost immediately

#define CTF_TARGET_PROTECT_RADIUS			400	// the radius around an object being defended where a target will be worth extra frags
#define CTF_ATTACKER_PROTECT_RADIUS			400	// the radius around an object being defended where an attacker will get extra frags when making kills

#define CTF_CARRIER_DANGER_PROTECT_TIMEOUT	80
#define CTF_FRAG_CARRIER_ASSIST_TIMEOUT		100
#define CTF_RETURN_FLAG_ASSIST_TIMEOUT		100

#define CTF_AUTO_FLAG_RETURN_TIMEOUT		300	// number of seconds before dropped flag auto-returns

#define CTF_GRAPPLE_SPEED					650 // speed of grapple in flight
#define CTF_GRAPPLE_PULL_SPEED				650	// speed player is pulled at

void CTFInit();
void CTFSpawn();

inline char *CTFTeamName(ETeamIndex team)
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

inline char *CTFOtherTeamName(ETeamIndex team)
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


edict_t *SelectCTFSpawnPoint (edict_t *ent);
void CTFCalcScores();
void SetCTFStats(edict_t *ent);
void CTFTeam_f (CPlayerEntity *ent);
void CTFID_f (CPlayerEntity *ent);
void CTFSay_Team(CPlayerEntity *who, char *msg);
void CTFResetFlag(ETeamIndex Team);
void CTFFragBonuses(CPlayerEntity *targ, CPlayerEntity *attacker);
void CTFCheckHurtCarrier(CPlayerEntity *targ, CPlayerEntity *attacker);

//TECH
void CTFOpenJoinMenu(CPlayerEntity *ent);
void CTFVoteYes(CPlayerEntity *ent);
void CTFVoteNo(CPlayerEntity *ent);
void CTFReady(CPlayerEntity *ent);
void CTFNotReady(CPlayerEntity *ent);
bool CTFNextMap();
bool CTFMatchSetup();
bool CTFMatchOn();
void CTFGhost(CPlayerEntity *ent);
void CTFAdmin(CPlayerEntity *ent);
bool CTFInMatch();
void CTFStats(CPlayerEntity *ent);
void CTFWarp(CPlayerEntity *ent);
void CTFBoot(CPlayerEntity *ent);
void CTFPlayerList(CPlayerEntity *ent);

bool CTFCheckRules();
void CreateCTFStatusbar ();

void CTFObserver(CPlayerEntity *ent);

extern CCTFGameLocals ctfgame;

extern	CCvar *ctf;
extern	CCvar *ctf_forcejoin;

extern	CCvar *competition;
extern	CCvar *matchlock;
extern	CCvar *electpercentage;
extern	CCvar *matchtime;
extern	CCvar *matchsetuptime;
extern	CCvar *matchstarttime;
extern	CCvar *admin_password;
extern	CCvar *warp_list;

#else
FILE_WARNING
#endif