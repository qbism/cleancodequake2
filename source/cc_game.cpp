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
// cc_game.cpp
// Contains actual game code; the code needed to make Q2 go anywhere.
//

#include "cc_local.h"
#include "cc_ban.h"
#include "cc_target_entities.h"
#include "cc_bodyqueue.h"
#include "cc_gamecommands.h"
#include "cc_servercommands.h"
#include "cc_version.h"

CGameLocals		Game;
CLevelLocals	Level;

/*
=================
EndDMLevel

The timelimit or fraglimit has been exceeded
=================
*/
void EndDMLevel ()
{
	char *s, *t, *f;
	static const char *seps = " ,\n\r";

	// stay on same level flag
	if (DeathmatchFlags.dfSameLevel.IsEnabled())
	{
		BeginIntermission (CreateTargetChangeLevel (Level.ServerLevelName.c_str()));
		return;
	}

	if (!Level.ForceMap.empty())
	{
		BeginIntermission (CreateTargetChangeLevel (Level.ForceMap.c_str()));
		return;
	}

	// see if it's in the map list
	if (*CvarList[CV_MAPLIST].String())
	{
		s = Mem_StrDup(CvarList[CV_MAPLIST].String());
		f = NULL;
		t = strtok(s, seps);

		while (t != NULL)
		{
			if (Q_stricmp(t, Level.ServerLevelName.c_str()) == 0) {
				// it's in the list, go to the next one
				t = strtok(NULL, seps);

				if (t == NULL) { // end of list, go to first one
					if (f == NULL) // there isn't a first one, same level
						BeginIntermission (CreateTargetChangeLevel (Level.ServerLevelName.c_str()) );
					else
						BeginIntermission (CreateTargetChangeLevel (f) );
				} else
					BeginIntermission (CreateTargetChangeLevel (t) );
				QDelete s;
				return;
			}
			if (!f)
				f = t;
			t = strtok(NULL, seps);
		}
		free(s);
	}

	if (!Level.NextMap.empty()) // go to a specific map
		BeginIntermission (CreateTargetChangeLevel (Level.NextMap.c_str()) );
	else
	{	// search for a changelevel
		CTargetChangeLevel *Entity = CC_FindByClassName<CTargetChangeLevel, ENT_BASE> (NULL, "target_changelevel");
		if (!Entity)
		{	// the map designer didn't include a changelevel,
			// so create a fake ent that goes back to the same level
			BeginIntermission (CreateTargetChangeLevel (Level.ServerLevelName.c_str()) );
			return;
		}
		BeginIntermission (Entity);
	}
}

/*
=================
CheckDMRules
=================
*/
void CheckDMRules ()
{
	if (Level.IntermissionTime)
		return;

	if (!(Game.GameMode & GAME_DEATHMATCH))
		return;

#if CLEANCTF_ENABLED
//ZOID
	if ((Game.GameMode & GAME_CTF) && CTFCheckRules())
	{
		EndDMLevel ();
		return;
	}
	if (CTFInMatch())
		return; // no checking in match mode
//ZOID
#endif

	if (CvarList[CV_TIME_LIMIT].Float())
	{
		if (Level.Frame >= ((CvarList[CV_TIME_LIMIT].Float()*60)*10))
		{
			BroadcastPrintf (PRINT_HIGH, "Timelimit hit.\n");
			EndDMLevel ();
			return;
		}
	}

	if (CvarList[CV_FRAG_LIMIT].Integer())
	{
		for (uint8 i = 0; i < Game.MaxClients; i++)
		{
			CPlayerEntity *cl = entity_cast<CPlayerEntity>(Game.Entities[i+1].Entity);
			if (!cl->GetInUse())
				continue;

			if (cl->Client.Respawn.Score >= CvarList[CV_FRAG_LIMIT].Integer())
			{
				BroadcastPrintf (PRINT_HIGH, "Fraglimit hit.\n");
				EndDMLevel ();
				return;
			}
		}
	}
}

/*
=============
ExitLevel
=============
*/
void ExitLevel ()
{
#if CLEANCTF_ENABLED
	if (CTFNextMap())
		return;
#endif

	gi.AddCommandString ((char*)(std::string("gamemap \"") + Level.ChangeMap + "\"\n").c_str());
	Level.ChangeMap = NULL;
	Level.ExitIntermission = false;
	Level.IntermissionTime = 0;
	ClientEndServerFrames ();

	// clear some things before going to next level
	for (sint32 i = 0; i < Game.MaxClients; i++)
	{
		CPlayerEntity *Player = entity_cast<CPlayerEntity>(Game.Entities[1 + i].Entity);
		if (!Player->GetInUse())
			continue;
		if (Player->Health > Player->Client.Persistent.MaxHealth)
			Player->Health = Player->Client.Persistent.MaxHealth;
	}

#if MONSTERS_USE_PATHFINDING
	SavePathTable ();
#endif
}

/*
=================
CheckNeedPass
=================
*/
inline void CheckNeedPass ()
{
	// if password or spectator_password has changed, update needpass
	// as needed
	if (CvarList[CV_PASSWORD].Modified() || CvarList[CV_SPECTATOR_PASSWORD].Modified()) 
	{
		sint32 need = 0;

		if (*CvarList[CV_PASSWORD].String() && Q_stricmp(CvarList[CV_PASSWORD].String(), "none"))
			need |= 1;
		if (*CvarList[CV_SPECTATOR_PASSWORD].String() && Q_stricmp(CvarList[CV_SPECTATOR_PASSWORD].String(), "none"))
			need |= 2;

		CvarList[CV_NEEDPASS].Set(need);
	}
}

void ClientEndServerFrames ()
{
	// calc the player views now that all pushing
	// and damage has been added
	for (sint32 i = 1; i <= Game.MaxClients ; i++)
	{
		CPlayerEntity *Player = entity_cast<CPlayerEntity>(Game.Entities[i].Entity);
		if (!Player->GetInUse())
			continue;

		if (CvarList[CV_MAP_DEBUG].Boolean())
		{
			Mem_Zero (&Player->PlayedSounds, sizeof(Player->PlayedSounds));
			Player->BeginServerFrame ();
		}

		Player->EndServerFrame ();
	}
}

void CheckVersionReturnance ();

/*
================
RunFrame

Advances the world by 0.1 seconds
================
*/

void			RunPrivateEntities ();

#if defined(WIN32)
#include <crtdbg.h>
#endif

void ProcessEntity (edict_t *ent)
{
	if (!ent->inUse)
	{
		if (ent->state.number > (Game.MaxClients + BODY_QUEUE_SIZE))
			ent->AwaitingRemoval = true;
		return;
	}

	if (ent->Entity)
	{
		CBaseEntity *Entity = ent->Entity;
		
		Mem_Zero (&Entity->PlayedSounds, sizeof(Entity->PlayedSounds));
		
		Level.CurrentEntity = Entity;
		Entity->State.GetOldOrigin() = Entity->State.GetOrigin();

		// if the ground entity moved, make sure we are still on it
		if ((Entity->GroundEntity) && ((!Entity->GroundEntity->gameEntity) || (Entity->GroundEntity->GetLinkCount() != Entity->GroundEntityLinkCount)))
		{
			Entity->GroundEntity = NULL;
			if ( !(Entity->Flags & (FL_SWIM|FL_FLY)) && (Entity->EntityFlags & ENT_MONSTER))
				(entity_cast<CMonsterEntity>(Entity))->Monster->CheckGround ();
		}

		CThinkableEntity *Thinkable = (!Entity->Freed && (Entity->EntityFlags & ENT_THINKABLE)) ? entity_cast<CThinkableEntity>(Entity) : NULL;

		if (Thinkable) 
			Thinkable->PreThink ();

		Entity->Run ();

		if (Thinkable)
			Thinkable->RunThink ();

		// Were we freed?
		// This has to be processed after thinking and running, because
		// the entity still has to be intact after that
		if (Entity->Freed)
			ent->AwaitingRemoval = true;
	}
}

bool RemoveEntity (edict_t *ent);

extern bool requestedBreak;

void CGameAPI::RunFrame ()
{
	if (requestedBreak)
	{
		requestedBreak = false;
#if defined(WIN32)
		_CrtDbgBreak();
#else
		assert(0);
#endif
	}

#if !NO_VERSION_CHECKING
	CheckVersionReturnance ();
#endif

	if (Level.Frame >= 3 && CvarList[CV_MAP_DEBUG].Boolean())
	{
		Level.Frame ++;
		// Run the players only
		// build the playerstate_t structures for all players
		ClientEndServerFrames ();

#if MONSTERS_USE_PATHFINDING
		RunNodes();
#endif
		return;
	}

	Level.Frame++;

	if (Level.Frame == 2)
		EndMapCounter();

	// choose a client for monsters to target this frame
	// Only do it when we have spawned everything
	if (!(Game.GameMode & GAME_DEATHMATCH) && Level.Frame > 20) // Paril, lol
		AI_SetSightClient ();

	// exit intermissions

	if (Level.ExitIntermission)
	{
		if (Level.ExitIntermissionOnNextFrame)
		{
			Level.ExitIntermissionOnNextFrame = false;
			return;
		}
		ExitLevel ();
		return;
	}

	//
	// treat each object in turn
	// even the world gets a chance to think
	//
	std::for_each (Level.Entities.Closed.begin(), Level.Entities.Closed.end(), ProcessEntity);
	Level.Entities.Closed.remove_if (RemoveEntity);
#ifdef _DEBUG
	_CC_ASSERT_EXPR ((Level.Entities.Closed.size() + Level.Entities.Open.size()) == GameAPI.GetMaxEdicts(), "Entities don't equal max!");
#endif

	RunPrivateEntities ();
	RunTimers ();

	// see if it is time to end a deathmatch
	CheckDMRules ();

	// see if needpass needs updated
	CheckNeedPass ();

	// build the playerstate_t structures for all players
	ClientEndServerFrames ();

	if (CvarList[CV_DMFLAGS].Modified())
		DeathmatchFlags.UpdateFlags(CvarList[CV_DMFLAGS].Integer());

#if MONSTERS_USE_PATHFINDING
	RunNodes();
#endif

#if CLEANCODE_IRC
	UpdateIRCServers ();
#endif
}

void SetupGamemode ()
{
	sint32 dmInt = CvarList[CV_DEATHMATCH].Integer(),
		coopInt = CvarList[CV_COOP].Integer();

	// Did we request deathmatch?
	if (dmInt)
	{
		// Did we also request coop?
		if (coopInt)
		{
			// Which one takes priority?
			if (dmInt > coopInt)
			{
				// We want deathmatch
				CvarList[CV_COOP].Set (0, false);
				CvarList[CV_DEATHMATCH].Set (1, false);
				// Let it fall through
			}
			else if (coopInt > dmInt)
			{
				// We want coop
				CvarList[CV_DEATHMATCH].Set (0, false);
				CvarList[CV_COOP].Set (1, false);
				Game.GameMode = GAME_COOPERATIVE;
				return;
			}
			// We don't know what we want, forcing DM
			else
			{
				CvarList[CV_COOP].Set (0, false);
				DebugPrintf		("CleanCode Warning: Both deathmatch and coop are 1; forcing to CvarList[CV_DEATHMATCH].\n"
								 "Did you know you can make one take priority if you intend to only set one?\n"
								 "If deathmatch is 1 and you want to switch to coop, just type \"coop 2\" and change maps!\n");
				// Let it fall through
			}
		}
		Game.GameMode = GAME_DEATHMATCH;
	}
	// Did we request cooperative?
	else if (coopInt)
	{
		// All the above code handles the case if deathmatch is true.
		Game.GameMode = GAME_COOPERATIVE;
		return;
	}
	else
	{
		Game.GameMode = GAME_SINGLEPLAYER;
		return;
	}

	// If we reached here, we wanted deathmatch
#if CLEANCTF_ENABLED
	if (CvarList[CV_CTF].Integer())
		Game.GameMode |= GAME_CTF;
#endif
}

/*
============
InitGame

This will be called when the dll is first loaded, which
only happens when a new game is started or a save game
is loaded.
============
*/

// Registers all cvars and commands
void G_Register ()
{
	Cvar_Register ();

	SetupArg ();
	Cmd_Register ();
	SvCmd_Register ();

#if MONSTERS_USE_PATHFINDING
	Nodes_Register ();
#endif
}

#ifdef WIN32
	#ifdef _FRONTEND
		#ifdef _DEBUG
			#define CONFIGURATIONSTRING "Win32 FrontEnd Debug"
		#else
			#define CONFIGURATIONSTRING "Win32 FrontEnd Release"
	#endif
	#else
		#ifdef _DEBUG
			#define CONFIGURATIONSTRING "Win32 Debug"
		#else
			#define CONFIGURATIONSTRING "Win32 Release"
		#endif
	#endif
#else
	#ifdef _FRONTEND
		#ifdef _DEBUG
			#define CONFIGURATIONSTRING "FrontEnd Debug"
		#else
			#define CONFIGURATIONSTRING "FrontEnd Release"
		#endif
	#else
		#ifdef _DEBUG
			#define CONFIGURATIONSTRING "Debug"
		#else
			#define CONFIGURATIONSTRING "Release"
		#endif
	#endif
#endif

std::string ConfigTimeString ()
{
	return std::string(TimeStamp()) + " (running on " + CPUSTRING + " " + CONFIGURATIONSTRING + ")";
}

void CGameAPI::Init ()
{
	CTimer LoadTimer;

	//Mem_Init ();
	ServerPrintf ("==== InitGame ====\nRunning CleanCode Quake2 version "CLEANCODE_VERSION_PRINT", built on %s\nInitializing Game...\n", CLEANCODE_VERSION_PRINT_ARGS, ConfigTimeString().c_str());

	seedMT (time(NULL));

	// Register cvars/commands
	G_Register();

	// File-system
	FS_Init (4);

	// Setup the gamemode
	SetupGamemode ();

	DeathmatchFlags.UpdateFlags(CvarList[CV_DMFLAGS].Integer());

	// items
	InitItemlist ();

	Game.HelpMessages[0].clear();
	Game.HelpMessages[1].clear();

	// initialize all entities for this game
	Game.MaxEntities = CvarList[CV_MAXENTITIES].Integer();
	Game.Entities = QNew (TAG_GAME) edict_t[Game.MaxEntities];
	GameAPI.GetEntities() = Game.Entities;
	GameAPI.GetMaxEdicts() = Game.MaxEntities;

	// initialize all clients for this game
	Game.MaxClients = CvarList[CV_MAXCLIENTS].Integer();
	Game.Clients = QNew (TAG_GAME) gclient_t[Game.MaxClients];
	GameAPI.GetNumEdicts() = Game.MaxClients + 1;

	// Vars
	Game.MaxSpectators = CvarList[CV_MAXSPECTATORS].Integer();
	Game.CheatsEnabled = (CvarList[CV_CHEATS].Integer()) ? true : false;

	Bans.LoadFromFile ();

//	Mem_Register ();

#if 0
	LoadModules ();
	InitializeModules ();
#endif

	// Set R1 protocol flags
	Game.R1Protocol = (CCvar("protocol", "").Integer() == ENHANCED_PROTOCOL_VERSION);

	ServerPrintf ("\nGame initialized in "TIMER_STRING".\n", LoadTimer.Get());
}

void CGameAPI::Shutdown ()
{
	ServerPrintf ("==== ShutdownGame ====\n");

	ShutdownBodyQueue ();
	Shutdown_Junk ();

	Bans.SaveList ();

	Mem_FreeTag (TAG_GAME);
	Mem_FreeTag (TAG_LEVEL);
	Mem_FreeTag (TAG_ENTITY);
}
