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
// cc_shared.h
// Shared header, included by g_local.
//

#if !defined(__CC_SHARED_H__) || !defined(INCLUDE_GUARDS)
#define __CC_SHARED_H__

// Pre-defined classes
class CBaseEntity;
class CPlayerEntity;

// Stand-alone classes
typedef int64	FrameNumber_t;

#include "cc_indexing.h"
#include "cc_media.h"
#include "cc_dmflags.h"
#include "cc_arg.h"
#include "cc_write.h"
#include "cc_cvar.h"
#include "cc_filesystem.h"
#include "cc_hash.h"
#define BAN_BASIC_INFO
#include "cc_ban.h"
#undef BAN_BASIC_INFO

// Main class files
#include "cc_items.h"
#include "cc_inventory.h"
#ifdef MONSTERS_USE_PATHFINDING
#define PATHFINDING_FUNCTIONSONLY
#include "cc_pathfinding.h"
#undef PATHFINDING_FUNCTIONSONLY
#endif
#include "cc_sbar.h"
#include "cc_mapprint.h"
#ifdef CLEANCTF_ENABLED
#include "cc_ctf.h"
#endif
#include "cc_gameapi.h"
#include "cc_baseentity.h"
#include "cc_trace.h"
#include "cc_monsters.h"
#include "cc_utils.h"

// Classes that require classes from the above includes
#include "cc_print.h"
#include "cc_sound.h"
#ifdef CLEANCTF_ENABLED
#include "cc_ctfadmin.h"
#include "cc_ctfmenu.h"
#endif
#include "cc_entitylist.h"


#if 0
void DrawNewton ();
#endif

#else
FILE_WARNING
#endif