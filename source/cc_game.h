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
// cc_game.h
// Contains actual game code; the code needed to make Q2 go anywhere.
// Also the API.
//

#if !defined(__CC_GAME_H__) || !defined(INCLUDE_GUARDS)
#define __CC_GAME_H__

// edict->svFlags

CC_ENUM (sint32, EServerFlags)
{
	SVF_NOCLIENT			= BIT(0), // don't send entity to clients, even if it has effects
	SVF_DEADMONSTER			= BIT(1), // treat as CONTENTS_DEADMONSTER for collision
	SVF_MONSTER				= BIT(2), // treat as CONTENTS_MONSTER for collision

// ZOID
// entity is simple projectile, used for network optimization
// if an entity is projectile, the model index/x/y/z/pitch/yaw are sent, encoded into
// seven (or eight) bytes.  This is to speed up projectiles.  Currently, only the
// hyperblaster makes use of this.  use for items that are moving with a constant
// velocity that don't change direction or model
	SVF_PROJECTILE			= BIT(3)
// ZOID
};

// edict->solid values
CC_ENUM (sint32, ESolidType)
{
	SOLID_NOT,			// no interaction with other objects
	SOLID_TRIGGER,		// only touch when inside, after moving
	SOLID_BBOX,			// touch on edge
	SOLID_BSP			// bsp clip, touch on edge
};

// ==========================================================================

// link_t is only used for entity area links now
struct link_t
{
	link_t	*prev, *next;
};

#define MAX_ENT_CLUSTERS	16

#ifndef GAME_INCLUDE

struct gclient_t
{
	playerState_t		playerState;		// communicated by server to clients
	sint32					ping;
	// the game dll can add anything it wants after
	// this point in the structure
};

struct edict_t
{
	entityStateOld_t	s;
	gclient_t			*client;	// NULL if not a player
									// the server expects the first part
									// of gclient_s to be a player_state_t
									// but the rest of it is opaque
	BOOL				inUse;
	sint32					linkCount;

	// FIXME: move these fields to a server private sv_entity_t
	link_t				area;				// linked to a division node or leaf
	
	sint32					numClusters;		// if -1, use headnode instead
	sint32					clusterNums[MAX_ENT_CLUSTERS];
	sint32					headNode;			// unused if numClusters != -1
	sint32					areaNum, areaNum2;

	//================================

	EServerFlags		svFlags;			// SVF_NOCLIENT, SVF_DEADMONSTER, SVF_MONSTER, etc
	vec3_t				mins, maxs;
	vec3_t				absMin, absMax, size;
	ESolidType			solid;
	EBrushContents		clipMask;
	edict_t				*owner;

	// the game dll can add anything it wants after
	// this point in the structure
};

#else		// GAME_INCLUDE

struct edict_t;
struct gclient_t;

#endif		// GAME_INCLUDE

// ==========================================================================

#else
FILE_WARNING
#endif
