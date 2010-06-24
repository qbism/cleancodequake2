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
// cc_local.h
// Local header. Definitions local to the Game.
//

#if !defined(CC_GUARD_LOCAL_H) || !INCLUDE_GUARDS
#define CC_GUARD_LOCAL_H

#include "cc_options.h"
#include "../shared/Shared.h"

inline const char *BuildDate()
{
	return __DATE__;
};

inline const char *TimeStamp()
{
	return __TIMESTAMP__;
};

// Convenience vector printing
#define VECTOR_STRING "%f %f %f"
#define PRINT_VECTOR_ARGS(x) (x).X, (x).Y, (x).Z

// Forward declarations
class IBaseEntity;
class CPlayerEntity;

typedef sint32	FrameNumber;

#define TO_STRING2(x) #x
#define TO_STRING(x) TO_STRING2(x)

// Stand-alone classes
#include "cc_hash.h"
#include "cc_print.h"
#include "cc_file_system.h"
#include "cc_platform.h"
#include "cc_commands.h"
#include "cc_indexing.h"
#include "cc_media.h"
#include "cc_dmflags.h"
#include "cc_write.h"
#include "cc_cvar.h"
#include "cc_parse.h"
#define BAN_BASIC_INFO
#include "cc_ban.h"
#undef BAN_BASIC_INFO
#include "cc_timer.h"
#include "cc_save.h"
#include "cc_irc.h"

// Main class files
#include "cc_game_api.h"
#include "cc_game.h"
#include "cc_items.h"
#include "cc_inventory.h"
#include "cc_status_bar.h"
#include "cc_map_print.h"
#if CLEANCTF_ENABLED
#include "cc_ctf.h"
#endif
#include "cc_base_entity.h"
#include "cc_trace.h"
#include "cc_monsters.h"
#include "cc_modules.h"
#include "cc_utils.h"
// Arcade Quake II
#include "cc_arcadeq2.h"
// Arcade Quake II

// Classes that require classes from the above includes
#include "cc_sound.h"
#include "cc_entity_list.h"
#else
FILE_WARNING
#endif
