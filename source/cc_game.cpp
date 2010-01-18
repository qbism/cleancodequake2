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
	if (dmFlags.dfSameLevel.IsEnabled())
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
	if (*sv_maplist.String())
	{
		s = Mem_StrDup(sv_maplist.String());
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

	if (timelimit.Float())
	{
		if (Level.Frame >= ((timelimit.Float()*60)*10))
		{
			BroadcastPrintf (PRINT_HIGH, "Timelimit hit.\n");
			EndDMLevel ();
			return;
		}
	}

	if (fraglimit.Integer())
	{
		for (uint8 i = 0; i < Game.MaxClients; i++)
		{
			CPlayerEntity *cl = entity_cast<CPlayerEntity>(Game.Entities[i+1].Entity);
			if (!cl->GetInUse())
				continue;

			if (cl->Client.Respawn.Score >= fraglimit.Integer())
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

	//Level.ChangeMap
	gi.AddCommandString ((char*)(std::cc_string("gamemap \"") + Level.ChangeMap + "\"\n").c_str());
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
	if (password.Modified() || spectator_password.Modified()) 
	{
		sint32 need = 0;

		if (*password.String() && Q_stricmp(password.String(), "none"))
			need |= 1;
		if (*spectator_password.String() && Q_stricmp(spectator_password.String(), "none"))
			need |= 2;

		needpass.Set(need);
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

		if (map_debug.Boolean())
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

	if (Level.Frame >= 3 && map_debug.Boolean())
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

	if (dmflags.Modified())
		dmFlags.UpdateFlags(dmflags.Integer());

#if MONSTERS_USE_PATHFINDING
	RunNodes();
#endif
}

void SetupGamemode ()
{
	sint32 dmInt = deathmatch.Integer(),
		coopInt = coop.Integer();

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
				coop.Set (0, false);
				deathmatch.Set (1, false);
				// Let it fall through
			}
			else if (coopInt > dmInt)
			{
				// We want coop
				deathmatch.Set (0, false);
				coop.Set (1, false);
				Game.GameMode = GAME_COOPERATIVE;
				return;
			}
			// We don't know what we want, forcing DM
			else
			{
				coop.Set (0, false);
				DebugPrintf		("CleanCode Warning: Both deathmatch and coop are 1; forcing to deathmatch.\n"
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
	if (ctf.Integer())
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

CCvar	deathmatch,
		coop,
		dmflags,
		skill,
		fraglimit,
		timelimit,
		password,
		spectator_password,
		needpass,
		maxclients,
		maxspectators,
		maxentities,
		g_select_empty,
		dedicated,
		developer,
		filterban,
		sv_gravity,
		sv_rollspeed,
		sv_rollangle,
		gun_x,
		gun_y,
		gun_z,
		run_pitch,
		run_roll,
		bob_up,
		bob_pitch,
		bob_roll,
		sv_cheats,
		flood_msgs,
		flood_persecond,
		flood_waitdelay,
		sv_maplist,
		map_debug,
		cc_techflags,
		sv_airaccelerate;

// Registers all cvars and commands
void G_Register ()
{
	gun_x.Register ("gun_x", "0", 0);
	gun_y.Register ("gun_y", "0", 0);
	gun_z.Register ("gun_z", "0", 0);

	//FIXME: sv_ prefix is wrong for these
	sv_rollspeed.Register ("sv_rollspeed", "200", 0);
	sv_rollangle.Register ("sv_rollangle", "2", 0);
	sv_gravity.Register ("sv_gravity", "800", 0);

	// noset vars
	dedicated.Register ("dedicated", "0", CVAR_READONLY);

	developer.Register ("developer", "0", 0);

	// latched vars
	sv_cheats.Register ("cheats", "0", CVAR_SERVERINFO|CVAR_LATCH_SERVER);
	CCvar ("gamename", GAMENAME , CVAR_SERVERINFO|CVAR_LATCH_SERVER);
	CCvar ("gamedate", BuildDate(), CVAR_SERVERINFO|CVAR_LATCH_SERVER);

	maxclients.Register ("maxclients", "4", CVAR_SERVERINFO | CVAR_LATCH_SERVER);
	maxspectators.Register ("maxspectators", "4", CVAR_SERVERINFO);
	skill.Register ("skill", "1", CVAR_LATCH_SERVER);
	maxentities.Register ("maxentities", 1024, CVAR_LATCH_SERVER);

	// change anytime vars
	dmflags.Register ("dmflags", "0", CVAR_SERVERINFO);
	fraglimit.Register ("fraglimit", "0", CVAR_SERVERINFO);
	timelimit.Register ("timelimit", "0", CVAR_SERVERINFO);
	password.Register ("password", "", CVAR_USERINFO);
	spectator_password.Register ("spectator_password", "", CVAR_USERINFO);
	needpass.Register ("needpass", "0", CVAR_SERVERINFO);
	filterban.Register ("filterban", "1", 0);

	g_select_empty.Register ("g_select_empty", "0", CVAR_ARCHIVE);

	run_pitch.Register ("run_pitch", "0.002", 0);
	run_roll.Register ("run_roll", "0.005", 0);
	bob_up .Register ("bob_up", "0.005", 0);
	bob_pitch.Register ("bob_pitch", "0.002", 0);
	bob_roll.Register ("bob_roll", "0.002", 0);

	// flood control
	flood_msgs.Register ("flood_msgs", "4", 0);
	flood_persecond.Register ("flood_persecond", "4", 0);
	flood_waitdelay.Register ("flood_waitdelay", "10", 0);

	// dm map list
	sv_maplist.Register ("sv_maplist", "", 0);
	
	map_debug.Register ("map_debug", "0", CVAR_LATCH_SERVER);
	cc_techflags.Register ("cc_techflags", "0", CVAR_LATCH_SERVER);

	SetupArg ();
	Cmd_Register ();
	SvCmd_Register ();

	// Gamemodes
	deathmatch.Register ("deathmatch", "0", CVAR_SERVERINFO|CVAR_LATCH_SERVER);
	coop.Register ("coop", "0", CVAR_LATCH_SERVER);

#if CLEANCTF_ENABLED
//ZOID
	capturelimit.Register ("capturelimit", "0", CVAR_SERVERINFO);
	instantweap.Register ("instantweap", "0", CVAR_SERVERINFO);

	// Setup CTF if we have it
	CTFInit();
#endif

	sv_airaccelerate.Register ("sv_airaccelerate", "0", CVAR_SERVERINFO);

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

void CGameAPI::Init ()
{
	CTimer LoadTimer;

	//Mem_Init ();
	DebugPrintf ("==== InitGame ====\nRunning CleanCode Quake2 version "CLEANCODE_VERSION_PRINT", built on %s (%s %s)\nInitializing Game...\n", CLEANCODE_VERSION_PRINT_ARGS, TimeStamp(), CONFIGURATIONSTRING, CPUSTRING);

	seedMT (time(NULL));

	// Register cvars/commands
	G_Register();

	// File-system
	FS_Init (4);

	// Setup the gamemode
	SetupGamemode ();

	dmFlags.UpdateFlags(dmflags.Integer());

	// items
	InitItemlist ();

	Game.HelpMessages[0].clear();
	Game.HelpMessages[1].clear();

	// initialize all entities for this game
	Game.MaxEntities = maxentities.Integer();
	Game.Entities = QNew (com_gamePool, 0) edict_t[Game.MaxEntities];
	GameAPI.SetEntities(Game.Entities);
	GameAPI.GetMaxEdicts() = Game.MaxEntities;

	// initialize all clients for this game
	Game.MaxClients = maxclients.Integer();
	Game.Clients = QNew (com_gamePool, 0) gclient_t[Game.MaxClients];
	GameAPI.GetNumEdicts() = Game.MaxClients + 1;

	// Vars
	Game.MaxSpectators = maxspectators.Integer();
	Game.CheatsEnabled = (sv_cheats.Integer()) ? true : false;

	Bans.LoadFromFile ();

	Mem_Register ();

#if 0
	LoadModules ();
	InitializeModules ();
#endif

	ServerPrintf ("\nGame initialized in "TIMER_STRING".\n", LoadTimer.Get());
}

void CGameAPI::Shutdown ()
{
	ServerPrintf ("==== ShutdownGame ====\n");

	//Cmd_RemoveCommands ();
	//SvCmd_RemoveCommands ();

	ShutdownBodyQueue ();
	Shutdown_Junk ();

	Bans.SaveList ();
}
