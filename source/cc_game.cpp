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
#include "cc_cmds.h"
#include "cc_gamecommands.h"
#include "cc_servercommands.h"

game_locals_t	game;
CLevelLocals	level;
edict_t		*g_edicts;

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
	if (dmFlags.dfSameLevel)
	{
		BeginIntermission (CreateTargetChangeLevel (level.ServerLevelName.c_str()));
		return;
	}

	if (!level.ForceMap.empty())
	{
		BeginIntermission (CreateTargetChangeLevel (level.ForceMap.c_str()));
		return;
	}

	// see if it's in the map list
	if (*sv_maplist->String())
	{
		s = Mem_StrDup(sv_maplist->String());
		f = NULL;
		t = strtok(s, seps);

		while (t != NULL)
		{
			if (Q_stricmp(t, level.ServerLevelName.c_str()) == 0) {
				// it's in the list, go to the next one
				t = strtok(NULL, seps);

				if (t == NULL) { // end of list, go to first one
					if (f == NULL) // there isn't a first one, same level
						BeginIntermission (CreateTargetChangeLevel (level.ServerLevelName.c_str()) );
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

	if (!level.NextMap.empty()) // go to a specific map
		BeginIntermission (CreateTargetChangeLevel (level.NextMap.c_str()) );
	else
	{	// search for a changelevel
		CTargetChangeLevel *ent = CC_Find<CTargetChangeLevel, ENT_BASE, EntityMemberOffset(CBaseEntity,ClassName)> (NULL, "target_changelevel");
		if (!ent)
		{	// the map designer didn't include a changelevel,
			// so create a fake ent that goes back to the same level
			BeginIntermission (CreateTargetChangeLevel (level.ServerLevelName.c_str()) );
			return;
		}
		BeginIntermission (ent);
	}
}

/*
=================
CheckDMRules
=================
*/
void CheckDMRules ()
{
	if (level.IntermissionTime)
		return;

	if (!(game.mode & GAME_DEATHMATCH))
		return;

#ifdef CLEANCTF_ENABLED
//ZOID
	if ((game.mode & GAME_CTF) && CTFCheckRules())
	{
		EndDMLevel ();
		return;
	}
	if (CTFInMatch())
		return; // no checking in match mode
//ZOID
#endif

	if (timelimit->Float())
	{
		if (level.Frame >= ((timelimit->Float()*60)*10))
		{
			BroadcastPrintf (PRINT_HIGH, "Timelimit hit.\n");
			EndDMLevel ();
			return;
		}
	}

	if (fraglimit->Integer())
	{
		for (sint32 i=0 ; i<game.maxclients ; i++)
		{
			CPlayerEntity *cl = entity_cast<CPlayerEntity>(g_edicts[i+1].Entity);
			if (!cl->GetInUse())
				continue;

			if (cl->Client.Respawn.Score >= fraglimit->Integer())
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
	if (CTFNextMap())
		return;

	//level.ChangeMap
	gi.AddCommandString ((char*)(std::cc_string("gamemap \"") + level.ChangeMap + "\"\n").c_str());
	level.ChangeMap = NULL;
	level.ExitIntermission = false;
	level.IntermissionTime = 0;
	ClientEndServerFrames ();

	// clear some things before going to next level
	for (sint32 i = 0; i < game.maxclients; i++)
	{
		CPlayerEntity *ent = entity_cast<CPlayerEntity>(g_edicts[1 + i].Entity);
		if (!ent->GetInUse())
			continue;
		if (ent->Health > ent->Client.Persistent.max_health)
			ent->Health = ent->Client.Persistent.max_health;
	}

#ifdef MONSTERS_USE_PATHFINDING
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
	if (password->Modified() || spectator_password->Modified()) 
	{
		sint32 need = 0;

		if (*password->String() && Q_stricmp(password->String(), "none"))
			need |= 1;
		if (*spectator_password->String() && Q_stricmp(spectator_password->String(), "none"))
			need |= 2;

		needpass->Set(need);
	}
}

void ClientEndServerFrames ()
{
	// calc the player views now that all pushing
	// and damage has been added
	for (sint32 i = 1; i <= game.maxclients ; i++)
	{
		CPlayerEntity *Player = entity_cast<CPlayerEntity>(g_edicts[i].Entity);
		if (!Player->GetInUse())
			continue;

		if (map_debug->Boolean())
		{
			memset (&Player->PlayedSounds, 0, sizeof(Player->PlayedSounds));
			Player->BeginServerFrame ();
		}

		Player->EndServerFrame ();
	}
}

/*
================
CC_RunFrame

Advances the world by 0.1 seconds
================
*/

void			RunPrivateEntities ();

#if defined(WIN32)
#include <crtdbg.h>
#endif

extern bool requestedBreak;
void CC_RunFrame ()
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

	if (level.Frame >= 3 && map_debug->Boolean())
	{
		level.Frame ++;
		// Run the players only
		// build the playerstate_t structures for all players
		ClientEndServerFrames ();

#ifdef MONSTERS_USE_PATHFINDING
		RunNodes();
#endif
		return;
	}

	sint32		i;
	edict_t	*ent;

	level.Frame++;

	if (level.Frame == 2)
		EndMapCounter();

	// choose a client for monsters to target this frame
	// Only do it when we have spawned everything
	if (!(game.mode & GAME_DEATHMATCH) && level.Frame > 20) // Paril, lol
		AI_SetSightClient ();

	// exit intermissions

	if (level.ExitIntermission)
	{
		ExitLevel ();
		return;
	}

	//
	// treat each object in turn
	// even the world gets a chance to think
	//
	ent = &g_edicts[0];
	for (i=0 ; i<globals.numEdicts ; i++, ent++)
	{
		if (!ent->inUse)
			continue;

		if (ent->Entity)
		{
			CBaseEntity *Entity = ent->Entity;
			
			memset (&Entity->PlayedSounds, 0, sizeof(Entity->PlayedSounds));
			
			level.CurrentEntity = Entity;
			Entity->State.GetOldOrigin() = Entity->State.GetOrigin();

			// if the ground entity moved, make sure we are still on it
			if ((Entity->GroundEntity) && ((!Entity->GroundEntity->gameEntity) || (Entity->GroundEntity->GetLinkCount() != Entity->GroundEntityLinkCount)))
			{
				Entity->GroundEntity = NULL;
				if ( !(Entity->Flags & (FL_SWIM|FL_FLY)) && (Entity->EntityFlags & ENT_MONSTER))
					(entity_cast<CMonsterEntity>(Entity))->Monster->CheckGround ();
			}

			if (!Entity->Freed && (Entity->EntityFlags & ENT_THINKABLE)) 
				entity_cast<CThinkableEntity>(Entity)->PreThink ();

			Entity->Run ();

			if (!Entity->Freed && (Entity->EntityFlags & ENT_THINKABLE))
				entity_cast<CThinkableEntity>(Entity)->RunThink ();

			// Were we freed?
			// This has to be processed after thinking and running, because
			// the entity still has to be intact after that
			if (Entity->Freed)
			{
				QDelete Entity;
				ent->Entity = NULL;
			}
			continue;
		}
	}

	RunPrivateEntities ();
	RunTimers ();

	// see if it is time to end a deathmatch
	CheckDMRules ();

	// see if needpass needs updated
	CheckNeedPass ();

	// build the playerstate_t structures for all players
	ClientEndServerFrames ();

	if (dmflags->Modified())
		dmFlags.UpdateFlags(dmflags->Integer());

#if 0
	DrawNewton();
#endif

#ifdef MONSTERS_USE_PATHFINDING
	RunNodes();
#endif
}

void SetupGamemode ()
{
	sint32 dmInt = deathmatch->Integer(),
		coopInt = coop->Integer();
#ifdef CLEANCTF_ENABLED
	sint32 ctfInt = ctf->Integer();
#endif

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
				coop->Set (0, false);
				// Let it fall through
			}
			else if (coopInt > dmInt)
			{
				// We want coop
				deathmatch->Set (0, false);
				game.mode = GAME_COOPERATIVE;
				return;
			}
			// We don't know what we want, forcing DM
			else
			{
				coop->Set (0, false);
				DebugPrintf		("CleanCode Warning: Both deathmatch and coop are 1; forcing to deathmatch.\n"
								 "Did you know you can make one take priority if you intend to only set one?\n"
								 "If deathmatch is 1 and you want to switch to coop, just type \"coop 2\" and change maps!\n");
				// Let it fall through
			}
		}
		game.mode = GAME_DEATHMATCH;
	}
	// Did we request cooperative?
	else if (coopInt)
	{
		// All the above code handles the case if deathmatch is true.
		game.mode = GAME_COOPERATIVE;
		return;
	}
	else
	{
		game.mode = GAME_SINGLEPLAYER;
		return;
	}

	// If we reached here, we wanted deathmatch
#ifdef CLEANCTF_ENABLED
	if (ctfInt)
		game.mode |= GAME_CTF;
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

CCvar	*deathmatch;
CCvar	*coop;
CCvar	*dmflags;
CCvar	*skill;
CCvar	*fraglimit;
CCvar	*timelimit;
CCvar	*password;
CCvar	*spectator_password;
CCvar	*needpass;
CCvar	*maxclients;
CCvar	*maxspectators;
CCvar	*maxentities;
CCvar	*g_select_empty;
CCvar	*dedicated;
CCvar	*developer;

CCvar	*filterban;

CCvar	*sv_gravity;

CCvar	*sv_rollspeed;
CCvar	*sv_rollangle;
CCvar	*gun_x;
CCvar	*gun_y;
CCvar	*gun_z;

CCvar	*run_pitch;
CCvar	*run_roll;
CCvar	*bob_up;
CCvar	*bob_pitch;
CCvar	*bob_roll;

CCvar	*sv_cheats;

CCvar	*flood_msgs;
CCvar	*flood_persecond;
CCvar	*flood_waitdelay;

CCvar	*sv_maplist;
CCvar	*map_debug;
CCvar	*cc_techflags;

#ifdef CLEANCTF_ENABLED
//ZOID
CCvar	*capturelimit;
CCvar	*instantweap;
//ZOID
#endif

// Registers all cvars and commands
void G_Register ()
{
	gun_x = QNew (com_gamePool, 0) CCvar ("gun_x", "0", 0);
	gun_y = QNew (com_gamePool, 0) CCvar ("gun_y", "0", 0);
	gun_z = QNew (com_gamePool, 0) CCvar ("gun_z", "0", 0);

	//FIXME: sv_ prefix is wrong for these
	sv_rollspeed = QNew (com_gamePool, 0) CCvar ("sv_rollspeed", "200", 0);
	sv_rollangle = QNew (com_gamePool, 0) CCvar ("sv_rollangle", "2", 0);
	sv_gravity = QNew (com_gamePool, 0) CCvar ("sv_gravity", "800", 0);

	// noset vars
	dedicated = QNew (com_gamePool, 0) CCvar ("dedicated", "0", CVAR_READONLY);

	developer = QNew (com_gamePool, 0) CCvar ("developer", "0", 0);

	// latched vars
	sv_cheats = QNew (com_gamePool, 0) CCvar ("cheats", "0", CVAR_SERVERINFO|CVAR_LATCH_SERVER);
	CCvar ("gamename", GAMENAME , CVAR_SERVERINFO|CVAR_LATCH_SERVER);
	CCvar ("gamedate", __DATE__ , CVAR_SERVERINFO|CVAR_LATCH_SERVER);

	maxclients = QNew (com_gamePool, 0) CCvar ("maxclients", "4", CVAR_SERVERINFO | CVAR_LATCH_SERVER);
	maxspectators = QNew (com_gamePool, 0) CCvar ("maxspectators", "4", CVAR_SERVERINFO);
	skill = QNew (com_gamePool, 0) CCvar ("skill", "1", CVAR_LATCH_SERVER);
	maxentities = QNew (com_gamePool, 0) CCvar ("maxentities", 1024, CVAR_LATCH_SERVER);

	// change anytime vars
	dmflags = QNew (com_gamePool, 0) CCvar ("dmflags", "0", CVAR_SERVERINFO);
	fraglimit = QNew (com_gamePool, 0) CCvar ("fraglimit", "0", CVAR_SERVERINFO);
	timelimit = QNew (com_gamePool, 0) CCvar ("timelimit", "0", CVAR_SERVERINFO);
	password = QNew (com_gamePool, 0) CCvar ("password", "", CVAR_USERINFO);
	spectator_password = QNew (com_gamePool, 0) CCvar ("spectator_password", "", CVAR_USERINFO);
	needpass = QNew (com_gamePool, 0) CCvar ("needpass", "0", CVAR_SERVERINFO);
	filterban = QNew (com_gamePool, 0) CCvar ("filterban", "1", 0);

	g_select_empty = QNew (com_gamePool, 0) CCvar ("g_select_empty", "0", CVAR_ARCHIVE);

	run_pitch = QNew (com_gamePool, 0) CCvar ("run_pitch", "0.002", 0);
	run_roll = QNew (com_gamePool, 0) CCvar ("run_roll", "0.005", 0);
	bob_up  = QNew (com_gamePool, 0) CCvar ("bob_up", "0.005", 0);
	bob_pitch = QNew (com_gamePool, 0) CCvar ("bob_pitch", "0.002", 0);
	bob_roll = QNew (com_gamePool, 0) CCvar ("bob_roll", "0.002", 0);

	// flood control
	flood_msgs = QNew (com_gamePool, 0) CCvar ("flood_msgs", "4", 0);
	flood_persecond = QNew (com_gamePool, 0) CCvar ("flood_persecond", "4", 0);
	flood_waitdelay = QNew (com_gamePool, 0) CCvar ("flood_waitdelay", "10", 0);

	// dm map list
	sv_maplist = QNew (com_gamePool, 0) CCvar ("sv_maplist", "", 0);
	
	map_debug = QNew (com_gamePool, 0) CCvar ("map_debug", "0", CVAR_LATCH_SERVER);
	cc_techflags = QNew (com_gamePool, 0) CCvar ("cc_techflags", "0", CVAR_LATCH_SERVER);

	SetupArg ();
	Cmd_Register ();
	SvCmd_Register ();

	// Gamemodes
	deathmatch = QNew (com_gamePool, 0) CCvar ("deathmatch", "0", CVAR_SERVERINFO|CVAR_LATCH_SERVER);
	coop = QNew (com_gamePool, 0) CCvar ("coop", "0", CVAR_LATCH_SERVER);

#ifdef CLEANCTF_ENABLED
//ZOID
	capturelimit = QNew (com_gamePool, 0) CCvar ("capturelimit", "0", CVAR_SERVERINFO);
	instantweap = QNew (com_gamePool, 0) CCvar ("instantweap", "0", CVAR_SERVERINFO);

	// Setup CTF if we have it
	CTFInit();
#endif

#ifdef MONSTERS_USE_PATHFINDING
	Nodes_Register ();
#endif
}

void CC_InitGame ()
{
	CTimer LoadTimer;

	//Mem_Init ();
	DebugPrintf ("==== InitGame ====\n");
	DebugPrintf ("Running CleanCode Quake2 version "CLEANCODE_VERSION_PRINT", built on %s (%s %s)\nInitializing game...", CLEANCODE_VERSION_PRINT_ARGS, __TIMESTAMP__, BUILDSTRING, CPUSTRING);

	seedMT (time(NULL));

	// Register cvars/commands
	G_Register();

	// File-system
	FS_Init (4);
	FS_AddPath (GAMENAME);

	// Setup the gamemode
	SetupGamemode ();

	// items
	InitItemlist ();

	Q_snprintfz (game.helpmessage1, sizeof(game.helpmessage1), "");
	Q_snprintfz (game.helpmessage2, sizeof(game.helpmessage2), "");

	// initialize all entities for this game
	game.maxentities = maxentities->Integer();
	g_edicts = QNew (com_gamePool, 0) edict_t[game.maxentities];//(edict_t*)gi.TagMalloc (game.maxentities * sizeof(g_edicts[0]), TAG_GAME);
	globals.edicts = g_edicts;
	globals.maxEdicts = game.maxentities;

	// initialize all clients for this game
	game.maxclients = maxclients->Integer();
	game.clients = QNew (com_gamePool, 0) gclient_t[game.maxclients];//(gclient_t*)gi.TagMalloc (game.maxclients * sizeof(game.clients[0]), TAG_GAME);
	globals.numEdicts = game.maxclients+1;

	// Vars
	game.maxspectators = maxspectators->Integer();
	game.cheats = (sv_cheats->Integer()) ? true : false;

	Bans.LoadFromFile ();

	Mem_Register ();

	DebugPrintf ("\nGame initialized in "TIMER_STRING".\n", LoadTimer.Get());
	InitVersion ();
}

void CC_ShutdownGame ()
{
	DebugPrintf ("==== ShutdownGame ====\n");

	Cmd_RemoveCommands ();
	SvCmd_RemoveCommands ();

	ShutdownBodyQueue ();
	Shutdown_Junk ();
}