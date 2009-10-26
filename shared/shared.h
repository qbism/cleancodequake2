/*
Copyright (C) 1997-2001 Id Software, Inc.

This program is free software; you can redistribute it and/or
modify it under the terms of the GNU General Public License
as published by the Free Software Foundation; either version 2
of the License, or (at your option) any later version.

This program is distributed in the hope that it will be useful,
but WITHOUT ANY WARRANTY; without even the implied warranty of
MERCHANTABILITY or v

See the GNU General Public License for more details.

You should have received a copy of the GNU General Public License
along with this program; if not, write to the Free Software
Foundation, Inc., 59 Temple Place - Suite 330, Boston, MA 02111-1307, USA.
*/

//
// shared.h
// Included first by ALL program modules
//

#ifndef __SHARED_H__
#define __SHARED_H__

#include <assert.h>
#include <math.h>
#include <stdio.h>
#include <stdarg.h>
#include <string.h>
#include <stdlib.h>
#include <time.h>
#include <ctype.h>

void AssertExpression (const bool expr, const char *msg);

#include "Platform.h"
#include "../source/cc_memory.h"
#include "../source/cc_stl.h"
#include "Templates/Templates.h"
#include "../source/cc_property.h"
#include "MathLib/MathLib.h"
#include "ColorVec.h"

#define EGL_HOMEPAGE "http://egl.quakedev.com/"

/*
=============================================================================

	PROTOCOL

=============================================================================
*/

#ifndef GAME_IS_BEING_COMPILED_NOT_ENGINE_GO_AWAY
#define ORIGINAL_PROTOCOL_VERSION		34

#define ENHANCED_PROTOCOL_VERSION		35
#define ENHANCED_COMPATIBILITY_NUMBER	1905

#define MINOR_VERSION_R1Q2_BASE			1903
#define MINOR_VERSION_R1Q2_UCMD_UPDATES	1904
#define	MINOR_VERSION_R1Q2_32BIT_SOLID	1905
#endif

//
// server to client
// note: ONLY add things to the bottom, to keep Quake2 compatibility
//
CC_ENUM (byte, ESVCType)
{
	SVC_BAD,

	//
	// these ops are known to the game dll
	//
	SVC_MUZZLEFLASH,
	SVC_MUZZLEFLASH2,
	SVC_TEMP_ENTITY,
	SVC_LAYOUT,
	SVC_INVENTORY,

	//
	// the rest are private to the client and server (you can not modify their order!)
	//
	SVC_NOP,
	SVC_DISCONNECT,
	SVC_RECONNECT,
	SVC_SOUND,					// <see code>
	SVC_PRINT,					// [byte] id [string] null terminated string
	SVC_STUFFTEXT,				// [string] stuffed into client's console buffer, should be \n terminated
	SVC_SERVERDATA,				// [long] protocol ...
	SVC_CONFIGSTRING,			// [short] [string]
	SVC_SPAWNBASELINE,
	SVC_CENTERPRINT,			// [string] to put in center of the screen
	SVC_DOWNLOAD,				// [short] size [size bytes]
	SVC_PLAYERINFO,				// variable
	SVC_PACKETENTITIES,			// [...]
	SVC_DELTAPACKETENTITIES,	// [...]
	SVC_FRAME,

	SVC_ZPACKET,				// new for ENHANCED_PROTOCOL_VERSION
	SVC_ZDOWNLOAD,				// new for ENHANCED_PROTOCOL_VERSION

	SVC_MAX
};

//
// game print flags
//
CC_ENUM (uint8, EGamePrintLevel)
{
	PRINT_LOW,				// pickup messages
	PRINT_MEDIUM,			// death messages
	PRINT_HIGH,				// critical messages
	PRINT_CHAT,				// chat messages
	PRINT_CENTER			// center print messages (Paril)
};

//
// destination class for gi.multicast()
//
CC_ENUM (uint8, EMultiCast)
{
	MULTICAST_ALL,
	MULTICAST_PHS,
	MULTICAST_PVS,
	MULTICAST_ALL_R,
	MULTICAST_PHS_R,
	MULTICAST_PVS_R
};

// ===========================================================================

void	seedMT (uint32 seed);
uint32	randomMT ();

inline float frand ()
{
	return randomMT() * 0.00000000023283064365386962890625f;
}

inline float crand ()
{
	return ((int)randomMT() - 0x7FFFFFFF) * 0.000000000465661287307739257812f;
}

inline int irandom (const int h)
{
	return (int)(frand() * h);
}

inline int icrandom (const int h)
{
	return (int)(crand() * h);
}

/*
==============================================================================

	PARSING
 
==============================================================================
*/

#define MAX_STRING_CHARS	1024	// max length of a string passed to Cmd_TokenizeString
#define MAX_STRING_TOKENS	256		// max tokens resulting from Cmd_TokenizeString
#define MAX_TOKEN_CHARS		512		// max length of an individual token

char		*Com_Parse (char **dataPtr);
void		Com_DefaultExtension (char *path, char *extension, size_t size);
void		Com_FileBase (char *in, char *out);
void		Com_FileExtension (char *path, char *out, size_t size);
void		Com_FilePath (char *path, char *out, size_t size);
void		Com_NormalizePath (char *Dest, const size_t DestSize, const char *Source);
char		*Com_SkipPath (char *pathname);
void		Com_SkipRestOfLine (char **dataPtr);
char		*Com_SkipWhiteSpace (char *dataPtr, bool *hasNewLines);
void		Com_StripExtension (char *dest, size_t size, const char *src);
void		Com_StripPadding (char *in, char *dest);

/*
==============================================================================

	INFO STRINGS

==============================================================================
*/

#define MAX_INFO_KEY		64
#define MAX_INFO_VALUE		64
#define MAX_INFO_STRING		512

void		Info_Print (std::cc_string &s);
std::cc_string	Info_ValueForKey (std::cc_string &s, std::cc_string key);
void		Info_RemoveKey (std::cc_string &s, std::cc_string key);
void		Info_SetValueForKey (std::cc_string &s, std::cc_string key, std::cc_string value);
bool		Info_Validate (std::cc_string &s);

/*
==============================================================================

	BYTE ORDER FUNCTIONS
 
==============================================================================
*/

extern float (*LittleFloat) (float f);
extern int (*LittleLong) (int l);
extern int16 (*LittleShort) (int16 s);
extern float (*BigFloat) (float f);
extern int (*BigLong) (int l);
extern int16 (*BigShort) (int16 s);

void		Swap_Init ();

/*
==============================================================================

	NON-PORTABLE SYSTEM SERVICES

==============================================================================
*/

typedef uint32				fileHandle_t;

#define MAX_QEXT			16		// max length of a quake game pathname extension
#define MAX_QPATH			64		// max length of a quake game pathname
#define MAX_OSPATH			128		// max length of a filesystem pathname

// directory searching
CC_ENUM (uint16, ESearchFileFlags)
{
	SFF_ARCH		= BIT(0),
	SFF_HIDDEN		= BIT(1),
	SFF_RDONLY		= BIT(2),
	SFF_SUBDIR		= BIT(3),
	SFF_SYSTEM		= BIT(4)
};

// these are used for FS_OpenFile
CC_ENUM (uint8, EFSOpenMode)
{
	FS_MODE_READ_BINARY,
	FS_MODE_WRITE_BINARY,
	FS_MODE_APPEND_BINARY,

	FS_MODE_WRITE_TEXT,
	FS_MODE_APPEND_TEXT
};

// these are used for FS_Seek
CC_ENUM (uint8, EFSSeekOrigin)
{
	FS_SEEK_SET,
	FS_SEEK_CUR,
	FS_SEEK_END
};

// for FS_FindFiles
#define FS_MAX_FINDFILES	65536

//
// this is only here so the functions in shared/ can link
//
#define MAX_COMPRINT 4096

// Com_Printf
CC_ENUM (uint16, EComPrint)
{
	PRNT_WARNING			= BIT(0),
	PRNT_ERROR				= BIT(1),
	PRNT_CONSOLE			= BIT(2),
	PRNT_CHATHUD			= BIT(3)
};
void	Com_Printf (EComPrint flags, char *fmt, ...);
void	Com_DevPrintf (EComPrint flags, char *fmt, ...);

// Com_Error
CC_ENUM (uint8, EComErrorType)
{
	ERR_FATAL,				// exit the entire game with a popup window
	ERR_DROP,				// print to console and disconnect from game
	ERR_DISCONNECT			// don't kill server
};
void	Com_Error (EComErrorType code, char *fmt, ...);

//
// styles for R_DrawString/Char
//
enum
{
	FS_ALIGN_CENTER			= BIT(0),
	FS_ALIGN_RIGHT			= BIT(1),
	FS_ITALIC				= BIT(2),
	FS_SECONDARY			= BIT(3),
	FS_SHADOW				= BIT(4),
	FS_SQUARE				= BIT(5),	// Force the width/height to the character width/height value that's largest
};

/*
==============================================================================

	CVARS

	Console variables
	Do NOT modify struct fields, use the functions
==============================================================================
*/

enum
{
	CVAR_ARCHIVE		= BIT(0),	// saved to config
	CVAR_USERINFO		= BIT(1),	// added to userinfo  when changed
	CVAR_SERVERINFO		= BIT(2),	// added to serverinfo when changed
	CVAR_READONLY		= BIT(3),	// can only be changed when forced through code
	CVAR_LATCH_SERVER	= BIT(4),	// delay changes until server restart

#ifndef GAME_IS_BEING_COMPILED_NOT_ENGINE_GO_AWAY
	CVAR_LATCH_VIDEO	= BIT(5),	// delay changes until video restart
	CVAR_LATCH_AUDIO	= BIT(6),	// delay changes until audio restart
	CVAR_RESET_GAMEDIR	= BIT(7),	// reset game dir when this cvar is modified
	CVAR_CHEAT			= BIT(8),	// clamp to the default value when cheats are off
#endif
};

struct cVar_t
{
	char			*name;
	char			*string;
	char			*latchedString;	// for CVAR_LATCH vars
	int				flags;
	BOOL			modified;		// set each time the cvar is changed
	float			floatVal;

#ifndef GAME_IS_BEING_COMPILED_NOT_ENGINE_GO_AWAY
	int				intVal;
#endif
};

#ifndef GAME_IS_BEING_COMPILED_NOT_ENGINE_GO_AWAY
/*
==============================================================================

	CONSOLE COMMANDS

==============================================================================
*/

enum
{
	CMD_CGAME		= BIT(0),	// Automatically added by the engine
};
#endif

/*
==============================================================================

	CONTENTS/SURFACE FLAGS

==============================================================================
*/

//
// lower bits are stronger, and will eat weaker brushes completely
//
CC_ENUM (int, EBrushContents)
{
	CONTENTS_SOLID			= BIT(0),		// an eye is never valid in a solid
	CONTENTS_WINDOW			= BIT(1),		// translucent, but not watery
	CONTENTS_AUX			= BIT(2),
	CONTENTS_LAVA			= BIT(3),
	CONTENTS_SLIME			= BIT(4),
	CONTENTS_WATER			= BIT(5),
	CONTENTS_MIST			= BIT(6),

	#ifndef GAME_IS_BEING_COMPILED_NOT_ENGINE_GO_AWAY
	CONTENTS_FOG			= BIT(7), // Q3BSP
	#endif

	//
	// remaining contents are non-visible, and don't eat brushes
	//
	CONTENTS_AREAPORTAL		= BIT(15),

	CONTENTS_PLAYERCLIP		= BIT(16),
	CONTENTS_MONSTERCLIP	= BIT(17),

	//
	// currents can be added to any other contents, and may be mixed
	//
	CONTENTS_CURRENT_0		= BIT(18),
	CONTENTS_CURRENT_90		= BIT(19),
	CONTENTS_CURRENT_180	= BIT(20),
	CONTENTS_CURRENT_270	= BIT(21),
	CONTENTS_CURRENT_UP		= BIT(22),
	CONTENTS_CURRENT_DOWN	= BIT(23),

	CONTENTS_ORIGIN			= BIT(24),	// removed before bsping an entity
	CONTENTS_MONSTER		= BIT(25),	// should never be on a brush, only in game
	CONTENTS_DEADMONSTER	= BIT(26),
	CONTENTS_DETAIL			= BIT(27),	// brushes to be added after vis leafs
	CONTENTS_TRANSLUCENT	= BIT(28),	// auto set if any surface has trans
	CONTENTS_LADDER			= BIT(29),

	#ifndef GAME_IS_BEING_COMPILED_NOT_ENGINE_GO_AWAY
	// Q3BSP
	Q3CNTNTS_TELEPORTER		= BIT(18),
	Q3CNTNTS_JUMPPAD		= BIT(19),
	Q3CNTNTS_CLUSTERPORTAL	= BIT(20),
	Q3CNTNTS_DONOTENTER		= BIT(21),

	Q3CNTNTS_ORIGIN			= BIT(24),	// removed before bsping an entity

	Q3CNTNTS_BODY			= BIT(25),	// should never be on a brush, only in game
	Q3CNTNTS_CORPSE			= BIT(26),
	Q3CNTNTS_DETAIL			= BIT(27),	// brushes not used for the bsp
	Q3CNTNTS_STRUCTURAL		= BIT(28),	// brushes used for the bsp
	Q3CNTNTS_TRANSLUCENT	= BIT(29),	// don't consume surface fragments inside
	Q3CNTNTS_TRIGGER		= BIT(30),
	Q3CNTNTS_NODROP			= BIT(31),	// don't leave bodies or items (death fog, lava)
	// !Q3BSP
	#endif

	//
	// content masks
	//
	CONTENTS_MASK_ALL			= (-1),
	CONTENTS_MASK_SOLID			= (CONTENTS_SOLID|CONTENTS_WINDOW),
	CONTENTS_MASK_PLAYERSOLID	= (CONTENTS_SOLID|CONTENTS_PLAYERCLIP|CONTENTS_WINDOW|CONTENTS_MONSTER),
	CONTENTS_MASK_DEADSOLID		= (CONTENTS_SOLID|CONTENTS_PLAYERCLIP|CONTENTS_WINDOW),
	#ifdef MONSTERS_HIT_MONSTERSOLID
	CONTENTS_MASK_MONSTERSOLID	= (CONTENTS_SOLID|CONTENTS_WINDOW|CONTENTS_MONSTER|CONTENTS_MONSTERSOLID),
	#else
	CONTENTS_MASK_MONSTERSOLID	= (CONTENTS_SOLID|CONTENTS_WINDOW|CONTENTS_MONSTER),
	#endif
	CONTENTS_MASK_WATER			= (CONTENTS_WATER|CONTENTS_LAVA|CONTENTS_SLIME),
	CONTENTS_MASK_OPAQUE		= (CONTENTS_SOLID|CONTENTS_SLIME|CONTENTS_LAVA),
	CONTENTS_MASK_SHOT			= (CONTENTS_SOLID|CONTENTS_MONSTER|CONTENTS_WINDOW|CONTENTS_DEADMONSTER),
	CONTENTS_MASK_CURRENT		= (CONTENTS_CURRENT_0|CONTENTS_CURRENT_90|CONTENTS_CURRENT_180|CONTENTS_CURRENT_270|CONTENTS_CURRENT_UP|CONTENTS_CURRENT_DOWN),
};

CC_ENUM (uint8, EWaterLevel)
{
	WATER_NONE,
	WATER_FEET,
	WATER_WAIST,
	WATER_UNDER
};

CC_ENUM (int, ESurfaceFlags)
{
	SURF_TEXINFO_LIGHT		= BIT(0),		// value will hold the light strength
	SURF_TEXINFO_SLICK		= BIT(1),		// affects game physics
	SURF_TEXINFO_SKY		= BIT(2),		// don't draw, but add to skybox
	SURF_TEXINFO_WARP		= BIT(3),		// turbulent water warp
	SURF_TEXINFO_TRANS33	= BIT(4),
	SURF_TEXINFO_TRANS66	= BIT(5),
	SURF_TEXINFO_FLOWING	= BIT(6),	// scroll towards angle
	SURF_TEXINFO_NODRAW		= BIT(7),	// don't bother referencing the texture

	SURF_TEXINFO_HINT		= BIT(8),	// these aren't known to the engine I believe
	SURF_TEXINFO_SKIP		= BIT(9),	// only the compiler uses them
};

#ifndef GAME_IS_BEING_COMPILED_NOT_ENGINE_GO_AWAY
typedef int EQ3SurfaceFlags;
enum
{
	// Q3BSP
	SHREF_NODAMAGE			= BIT(0),		// never give falling damage
	SHREF_SLICK				= BIT(1),		// effects game physics
	SHREF_SKY				= BIT(2),		// lighting from environment map
	SHREF_LADDER			= BIT(3),
	SHREF_NOIMPACT			= BIT(4),	// don't make missile explosions
	SHREF_NOMARKS			= BIT(5),	// don't leave missile marks
	SHREF_FLESH				= BIT(6),	// make flesh sounds and effects
	SHREF_NODRAW			= BIT(7),	// don't generate a drawsurface at all
	SHREF_HINT				= BIT(8),	// make a primary bsp splitter
	SHREF_SKIP				= BIT(9),	// completely ignore, allowing non-closed brushes
	SHREF_NOLIGHTMAP		= BIT(10),	// surface doesn't need a lightmap
	SHREF_POINTLIGHT		= BIT(11),	// generate lighting info at vertexes
	SHREF_METALSTEPS		= BIT(12),	// clanking footsteps
	SHREF_NOSTEPS			= BIT(13),	// no footstep sounds
	SHREF_NONSOLID			= BIT(14),	// don't collide against curves with this set
	SHREF_LIGHTFILTER		= BIT(15),	// act as a light filter during q3map -light
	SHREF_ALPHASHADOW		= BIT(16),	// do per-pixel light shadow casting in q3map
	SHREF_NODLIGHT			= BIT(17),	// never add dynamic lights
	SHREF_DUST				= BIT(18), // leave a dust trail when walking on this surface
	// !Q3BSP
};
#endif

//
// gi.BoxEdicts() can return a list of either solid or trigger entities
//
enum
{
	AREA_SOLID	= 1,
	AREA_TRIGGERS
};

/*
==============================================================================

	PLANE

==============================================================================
*/

CC_ENUM (uint8, EPlaneInfo)
{
	// Axial planes
	PLANE_X,
	PLANE_Y,
	PLANE_Z,

	// Non-axial, snapped to the nearest
	PLANE_NON_AXIAL,
	PLANE_ANYX = 3,
	PLANE_ANYY,
	PLANE_ANYZ
};

struct plane_t
{
	vec3f			normal;
	float			dist;
	byte			type;			// for fast side tests
	byte			signBits;		// signx + (signy<<1) + (signz<<1)
};

//
// m_plane.c
//
int BoxOnPlaneSide(const vec3_t mins, const vec3_t maxs, const plane_t *plane);
EPlaneInfo PlaneTypeForNormal(const vec3_t normal);
void CategorizePlane(plane_t *plane);
void PlaneFromPoints(const vec3_t verts[3], plane_t *plane);
bool ComparePlanes(const vec3_t p1normal, const float p1dist, const vec3_t p2normal, const float p2dist);
void SnapVector(vec3_t normal);
void ProjectPointOnPlane(vec3_t dst, const vec3_t point, const vec3_t normal);
int SignbitsForPlane(const plane_t *out);

inline float PlaneDiff (vec3_t point, plane_t *plane)
{
	return ((plane->type < 3 ? point[plane->type] : Dot3Product(point, plane->normal)) - plane->dist);
}

inline int Box_On_Plane_Side (const vec3_t mins, const vec3_t maxs, const plane_t *p)
{
	return ((p->type < 3) ?
		(p->dist <= mins[p->type]) ?
			1 : ((p->dist >= maxs[p->type])
				?
				2 : 3)
				: BoxOnPlaneSide(mins, maxs, p));
};

/*
==============================================================================

	CMODEL

==============================================================================
*/

struct cmBspSurface_t
{
	char			name[16];
	ESurfaceFlags	flags;
	int				value;

#ifndef GAME_IS_BEING_COMPILED_NOT_ENGINE_GO_AWAY
	// Q3BSP
	int				contents;
#endif
};

// A trace is returned when a box is swept through the world
struct cmTrace_t
{
	BOOL			allSolid;	// if true, plane is not valid
	BOOL			startSolid;	// if true, the initial point was in a solid area
	float			fraction;	// time completed, 1.0 = didn't hit anything
	vec3_t			endPos;		// final position
	plane_t			plane;		// surface normal at impact
	cmBspSurface_t	*surface;	// surface hit
	int				contents;	// contents on other side of surface hit
	struct edict_t	*ent;		// not set by CM_*() functions
};

/*
==============================================================================

	PREDICTION

==============================================================================
*/

// pMoveState_t is the information necessary for client side movement prediction
enum
{
	// can accelerate and turn
	PMT_NORMAL,
	PMT_SPECTATOR,
	// no acceleration or turning
	PMT_DEAD,
	PMT_GIB,		// different bounding box
	PMT_FREEZE
};

// pmove->pmFlags
enum
{
	PMF_DUCKED			= BIT(0),
	PMF_JUMP_HELD		= BIT(1),
	PMF_ON_GROUND		= BIT(2),
	PMF_TIME_WATERJUMP	= BIT(3),	// pm_time is waterjump
	PMF_TIME_LAND		= BIT(4),	// pm_time is time before rejump
	PMF_TIME_TELEPORT	= BIT(5),	// pm_time is non-moving time
	PMF_NO_PREDICTION	= BIT(6)	// temporarily disables prediction (used for grappling hook)
};

// this structure needs to be communicated bit-accurate
// from the server to the client to guarantee that
// prediction stays in sync, so no floats are used.
// if any part of the game code modifies this struct, it
// will result in a prediction error of some degree.
struct pMoveState_t
{
	int				pmType;

	svec3_t			origin;			// 12.3
	svec3_t			velocity;		// 12.3
	byte			pmFlags;		// ducked, jump_held, etc
	byte			pmTime;			// each unit = 8 ms
	int16			gravity;
	svec3_t			deltaAngles;	// add to command angles to get view direction
									// changed by spawns, rotating objects, and teleporters
};

//
// button bits
//
CC_ENUM (uint8, EButtons)
{
	BUTTON_ATTACK			= BIT(0),
	BUTTON_USE				= BIT(1),

#ifndef GAME_IS_BEING_COMPILED_NOT_ENGINE_GO_AWAY
//stolen for r1q2 in the name of bandwidth
	BUTTON_UCMD_DBLFORWARD	= BIT(2),
	BUTTON_UCMD_DBLSIDE		= BIT(3),
	BUTTON_UCMD_DBLUP		= BIT(4),

	BUTTON_UCMD_DBL_ANGLE1	= BIT(5),
	BUTTON_UCMD_DBL_ANGLE2	= BIT(6),
#endif

	BUTTON_ANY				= BIT(7)			// any key whatsoever
};

// userCmd_t is sent to the server each client frame
struct userCmd_t
{
	byte		msec;
	EButtons	buttons;

	int16		angles[3];

	int16		forwardMove;
	int16		sideMove;
	int16		upMove;

	byte		impulse;		// remove?
	byte		lightLevel;		// light level the player is standing on
};

#define MAXTOUCH	32
#ifdef USE_EXTENDED_GAME_IMPORTS
struct pMove_t
{
	// state (in / out)
	pMoveState_t	state;

	// command (in)
	userCmd_t		cmd;
	BOOL			snapInitial;	// if s has been changed outside pmove

	// results (out)
	int				numTouch;
	struct edict_t	*touchEnts[MAXTOUCH];

	vec3_t			viewAngles;			// clamped
	float			viewHeight;

	vec3_t			mins, maxs;			// bounding box size

	struct edict_t	*groundEntity;
	int				waterType;
	int				waterLevel;

	// callbacks to test the world
	cmTrace_t		(*trace) (vec3_t start, vec3_t mins, vec3_t maxs, vec3_t end);
	int				(*pointContents) (vec3_t point);
};
#endif

struct pMoveNew_t
{
	// state (in / out)
	pMoveState_t	state;

	// command (in)
	userCmd_t		cmd;
	BOOL			snapInitial;	// if s has been changed outside pmove

	// results (out)
	int				numTouch;
	struct edict_t	*touchEnts[MAXTOUCH];

	vec3f			viewAngles;			// clamped
	float			viewHeight;

	vec3f			mins, maxs;			// bounding box size

	struct edict_t	*groundEntity;
	EBrushContents	waterType;
	EWaterLevel		waterLevel;
};

/*
==============================================================================

	ENTITY FX

==============================================================================
*/

// entityState_t->effects
// Effects are things handled on the client side (lights, particles, frame
// animations) that happen constantly on the given entity. An entity that has
// effects will be sent to the client even if it has a zero index model.
CC_ENUM (uint32, EEntityStateEffects)
{
	EF_ROTATE			= BIT(0),		// rotate (bonus items)
	EF_GIB				= BIT(1),		// leave a trail
	EF_BLASTER			= BIT(3),		// redlight + trail
	EF_ROCKET			= BIT(4),		// redlight + trail
	EF_GRENADE			= BIT(5),
	EF_HYPERBLASTER		= BIT(6),
	EF_BFG				= BIT(7),
	EF_COLOR_SHELL		= BIT(8),
	EF_POWERSCREEN		= BIT(9),
	EF_ANIM01			= BIT(10),		// automatically cycle between frames 0 and 1 at 2 hz
	EF_ANIM23			= BIT(11),		// automatically cycle between frames 2 and 3 at 2 hz
	EF_ANIM_ALL			= BIT(12),		// automatically cycle through all frames at 2hz
	EF_ANIM_ALLFAST		= BIT(13),		// automatically cycle through all frames at 10hz
	EF_FLIES			= BIT(14),
	EF_QUAD				= BIT(15),
	EF_PENT				= BIT(16),
	EF_TELEPORTER		= BIT(17),		// particle fountain
	EF_FLAG1			= BIT(18),
	EF_FLAG2			= BIT(19),

	// RAFAEL
	EF_IONRIPPER		= BIT(20),
	EF_GREENGIB			= BIT(21),
	EF_BLUEHYPERBLASTER = BIT(22),
	EF_SPINNINGLIGHTS	= BIT(23),
	EF_PLASMA			= BIT(24),
	EF_TRAP				= BIT(25), // IT'S A TRAP!!!

	// ROGUE
	EF_TRACKER			= BIT(26),
	EF_DOUBLE			= BIT(27),
	EF_SPHERETRANS		= BIT(28),
	EF_TAGTRAIL			= BIT(29),
	EF_HALF_DAMAGE		= BIT(30),
	EF_TRACKERTRAIL		= BIT(31),

	// Overloads
	EF_SEMITRANS_BLACKORB	= (EF_TRACKERTRAIL | EF_SPHERETRANS),
	EF_GREENBLASTER			= (EF_BLASTER | EF_TRACKER),
	EF_GREENHYPERBLASTER	= (EF_HYPERBLASTER | EF_TRACKER),
	EF_BLACKVOID			= (EF_TRACKERTRAIL | EF_TRACKER),
	EF_PLASMATRAIL			= (EF_PLASMA | EF_ANIM_ALLFAST),
};

/*
==============================================================================

	RENDERFX

==============================================================================
*/

// entityState_t->renderfx flags
CC_ENUM (int, EEntityStateRenderEffects)
{
	RF_MINLIGHT			= BIT(0),		// allways have some light (viewmodel)
	RF_VIEWERMODEL		= BIT(1),		// don't draw through eyes, only mirrors
	RF_WEAPONMODEL		= BIT(2),		// only draw through eyes
	RF_FULLBRIGHT		= BIT(3),		// allways draw full intensity
	RF_DEPTHHACK		= BIT(4),		// for view weapon Z crunching
	RF_TRANSLUCENT		= BIT(5),
	RF_FRAMELERP		= BIT(6),
	RF_BEAM				= BIT(7),
	RF_CUSTOMSKIN		= BIT(8),		// skin is an index in image_precache
	RF_GLOW				= BIT(9),		// pulse lighting for bonus items

	RF_SHELL_RED		= BIT(10),
	RF_SHELL_GREEN		= BIT(11),
	RF_SHELL_BLUE		= BIT(12),

	// Paril: What's with the gap here?

	RF_IR_VISIBLE		= BIT(15),		// 32768
	RF_SHELL_DOUBLE		= BIT(16),		// 65536
	RF_SHELL_HALF_DAM	= BIT(17),
	RF_USE_DISGUISE		= BIT(18),

	#ifndef GAME_IS_BEING_COMPILED_NOT_ENGINE_GO_AWAY
	RF_NOSHADOW			= BIT(19),
	RF_CULLHACK			= BIT(20),
	RF_FORCENOLOD		= BIT(21),
	RF_BEAM2			= BIT(22), // EGL specific effect
	#endif

	RF_SHELLMASK		= (RF_SHELL_HALF_DAM|RF_SHELL_DOUBLE|RF_SHELL_RED|RF_SHELL_GREEN|RF_SHELL_BLUE),

	// Paril: Extra mixed effects
	RF_SHELL_MIX_YELLOW = (RF_SHELL_RED|RF_SHELL_GREEN),
	RF_SHELL_MIX_WHITE = (RF_SHELL_RED|RF_SHELL_GREEN|RF_SHELL_BLUE),
	RF_SHELL_MIX_YELLOW2 = (RF_SHELL_RED|RF_SHELL_GREEN|RF_SHELL_DOUBLE),
	RF_SHELL_MIX_YELLOW3 = (RF_SHELL_RED|RF_SHELL_GREEN|RF_SHELL_HALF_DAM),
	RF_SHELL_MIX_PURPLE = (RF_SHELL_RED|RF_SHELL_BLUE),
	RF_SHELL_MIX_LIGHT_PURPLE = (RF_SHELL_RED|RF_SHELL_BLUE|RF_SHELL_DOUBLE),
	RF_SHELL_MIX_MID_PURPLE = (RF_SHELL_RED|RF_SHELL_BLUE|RF_SHELL_HALF_DAM),
	RF_SHELL_MIX_YELLOW4 = (RF_SHELL_RED|RF_SHELL_DOUBLE),
	RF_SHELL_MIX_PINKY_YELLOW = (RF_SHELL_RED|RF_SHELL_DOUBLE|RF_SHELL_HALF_DAM),
	RF_SHELL_MIX_PEACH = (RF_SHELL_RED|RF_SHELL_HALF_DAM),
	RF_SHELL_MIX_CYAN = (RF_SHELL_GREEN|RF_SHELL_BLUE),
	RF_SHELL_MIX_LIGHT_WHITE = (RF_SHELL_GREEN|RF_SHELL_BLUE|RF_SHELL_DOUBLE),
	RF_SHELL_MIX_LIGHT_CYAN = (RF_SHELL_GREEN|RF_SHELL_BLUE|RF_SHELL_HALF_DAM),
	RF_SHELL_MIX_YELLOWGREEN = (RF_SHELL_GREEN|RF_SHELL_DOUBLE),
	RF_SHELL_MIX_YELLOW5 = (RF_SHELL_GREEN|RF_SHELL_DOUBLE|RF_SHELL_HALF_DAM),
	RF_SHELL_MIX_LIGHTGREEN = (RF_SHELL_GREEN|RF_SHELL_HALF_DAM),
	RF_SHELL_MIX_LIGHTER_PURPLE = (RF_SHELL_BLUE|RF_SHELL_DOUBLE),
	RF_SHELL_MIX_LIGHTER_PURPLE2 = (RF_SHELL_BLUE|RF_SHELL_DOUBLE|RF_SHELL_HALF_DAM),
	RF_SHELL_MIX_LIGHT_BLUE = (RF_SHELL_BLUE|RF_SHELL_HALF_DAM),
	RF_SHELL_MIX_GOLD = (RF_SHELL_DOUBLE|RF_SHELL_HALF_DAM),
};

/*
==============================================================================

	MUZZLE FLASHES

==============================================================================
*/

// muzzle flashes / player effects
enum
{
	MZ_BLASTER,
	MZ_MACHINEGUN,
	MZ_SHOTGUN,
	MZ_CHAINGUN1,
	MZ_CHAINGUN2,
	MZ_CHAINGUN3,
	MZ_RAILGUN,
	MZ_ROCKET,
	MZ_GRENADE,
	MZ_LOGIN,
	MZ_LOGOUT,
	MZ_RESPAWN,
	MZ_BFG,
	MZ_SSHOTGUN,
	MZ_HYPERBLASTER,
	MZ_ITEMRESPAWN,

	// RAFAEL
	MZ_IONRIPPER,
	MZ_BLUEHYPERBLASTER,
	MZ_PHALANX,
	MZ_SILENCED			= 128,		// bit flag ORed with one of the above numbers

	// ROGUE
	MZ_ETF_RIFLE		= 30,
	MZ_UNUSED,
	MZ_SHOTGUN2,
	MZ_HEATBEAM,
	MZ_BLASTER2,
	MZ_TRACKER,
	MZ_NUKE1,
	MZ_NUKE2,
	MZ_NUKE4,
	MZ_NUKE8
	// ROGUE
};

// monster muzzle flashes

const vec3_t dumb_and_hacky_monster_MuzzFlashOffset [] = {
	0.0f,			0.0f,			0.0f,			// not used							0

	20.7f,			-18.5f,			28.7f,			// MZ2_TANK_BLASTER_1				1
	16.6f,			-21.5f,			30.1f,			// MZ2_TANK_BLASTER_2				2
	11.8f,			-23.9f,			32.1f,			// MZ2_TANK_BLASTER_3				3

	22.9f,			-0.7f,			25.3f,			// MZ2_TANK_MACHINEGUN_1			4
	22.2f,			6.2f,			22.3f,			// MZ2_TANK_MACHINEGUN_2			5
	19.4f,			13.1f,			18.6f,			// MZ2_TANK_MACHINEGUN_3			6
	19.4f,			18.8f,			18.6f,			// MZ2_TANK_MACHINEGUN_4			7
	17.9f,			25.0f,			18.6f,			// MZ2_TANK_MACHINEGUN_5			8
	14.1f,			30.5f,			20.6f,			// MZ2_TANK_MACHINEGUN_6			9
	9.3f,			35.3f,			22.1f,			// MZ2_TANK_MACHINEGUN_7			10
	4.7f,			38.4f,			22.1f,			// MZ2_TANK_MACHINEGUN_8			11
	-1.1f,			40.4f,			24.1f,			// MZ2_TANK_MACHINEGUN_9			12
	-6.5f,			41.2f,			24.1f,			// MZ2_TANK_MACHINEGUN_10			13
	3.2f,			40.1f,			24.7f,			// MZ2_TANK_MACHINEGUN_11			14
	11.7f,			36.7f,			26.0f,			// MZ2_TANK_MACHINEGUN_12			15
	18.9f,			31.3f,			26.0f,			// MZ2_TANK_MACHINEGUN_13			16
	24.4f,			24.4f,			26.4f,			// MZ2_TANK_MACHINEGUN_14			17
	27.1f,			17.1f,			27.2f,			// MZ2_TANK_MACHINEGUN_15			18
	28.5f,			9.1f,			28.0f,			// MZ2_TANK_MACHINEGUN_16			19
	27.1f,			2.2f,			28.0f,			// MZ2_TANK_MACHINEGUN_17			20
	24.9f,			-2.8f,			28.0f,			// MZ2_TANK_MACHINEGUN_18			21
	21.6f,			-7.0f,			26.4f,			// MZ2_TANK_MACHINEGUN_19			22

	6.2f,			29.1f,			49.1f,			// MZ2_TANK_ROCKET_1				23
	6.9f,			23.8f,			49.1f,			// MZ2_TANK_ROCKET_2				24
	8.3f,			17.8f,			49.5f,			// MZ2_TANK_ROCKET_3				25

	26.6f,			7.1f,			13.1f,			// MZ2_INFANTRY_MACHINEGUN_1		26
	18.2f,			7.5f,			15.4f,			// MZ2_INFANTRY_MACHINEGUN_2		27
	17.2f,			10.3f,			17.9f,			// MZ2_INFANTRY_MACHINEGUN_3		28
	17.0f,			12.8f,			20.1f,			// MZ2_INFANTRY_MACHINEGUN_4		29
	15.1f,			14.1f,			21.8f,			// MZ2_INFANTRY_MACHINEGUN_5		30
	11.8f,			17.2f,			23.1f,			// MZ2_INFANTRY_MACHINEGUN_6		31
	11.4f,			20.2f,			21.0f,			// MZ2_INFANTRY_MACHINEGUN_7		32
	9.0f,			23.0f,			18.9f,			// MZ2_INFANTRY_MACHINEGUN_8		33
	13.9f,			18.6f,			17.7f,			// MZ2_INFANTRY_MACHINEGUN_9		34
	15.4f,			15.6f,			15.8f,			// MZ2_INFANTRY_MACHINEGUN_10		35
	10.2f,			15.2f,			25.1f,			// MZ2_INFANTRY_MACHINEGUN_11		36
	-1.9f,			15.1f,			28.2f,			// MZ2_INFANTRY_MACHINEGUN_12		37
	-12.4f,			13.0f,			20.2f,			// MZ2_INFANTRY_MACHINEGUN_13		38

	10.6f * 1.2f,	7.7f * 1.2f,	7.8f * 1.2f,		// MZ2_SOLDIER_BLASTER_1			39
	21.1f * 1.2f,	3.6f * 1.2f,	19.0f * 1.2f,		// MZ2_SOLDIER_BLASTER_2			40
	10.6f * 1.2f,	7.7f * 1.2f,	7.8f * 1.2f,		// MZ2_SOLDIER_SHOTGUN_1			41
	21.1f * 1.2f,	3.6f * 1.2f,	19.0f * 1.2f,		// MZ2_SOLDIER_SHOTGUN_2			42
	10.6f * 1.2f,	7.7f * 1.2f,	7.8f * 1.2f,		// MZ2_SOLDIER_MACHINEGUN_1			43
	21.1f * 1.2f,	3.6f * 1.2f,	19.0f * 1.2f,		// MZ2_SOLDIER_MACHINEGUN_2			44

	30.1f * 1.15f,	3.9f * 1.15f,	19.6f * 1.15f,	// MZ2_GUNNER_MACHINEGUN_1			45
	29.1f * 1.15f,	2.5f * 1.15f,	20.7f * 1.15f,	// MZ2_GUNNER_MACHINEGUN_2			46
	28.2f * 1.15f,	2.5f * 1.15f,	22.2f * 1.15f,	// MZ2_GUNNER_MACHINEGUN_3			47
	28.2f * 1.15f,	3.6f * 1.15f,	22.0f * 1.15f,	// MZ2_GUNNER_MACHINEGUN_4			48
	26.9f * 1.15f,	2.0f * 1.15f,	23.4f * 1.15f,	// MZ2_GUNNER_MACHINEGUN_5			49
	26.5f * 1.15f,	0.6f * 1.15f,	20.8f * 1.15f,	// MZ2_GUNNER_MACHINEGUN_6			50
	26.9f * 1.15f,	0.5f * 1.15f,	21.5f * 1.15f,	// MZ2_GUNNER_MACHINEGUN_7			51
	29.0f * 1.15f,	2.4f * 1.15f,	19.5f * 1.15f,	// MZ2_GUNNER_MACHINEGUN_8			52
	4.6f * 1.15f,	-16.8f * 1.15f,	7.3f * 1.15f,		// MZ2_GUNNER_GRENADE_1				53
	4.6f * 1.15f,	-16.8f * 1.15f,	7.3f * 1.15f,		// MZ2_GUNNER_GRENADE_2				54
	4.6f * 1.15f,	-16.8f * 1.15f,	7.3f * 1.15f,		// MZ2_GUNNER_GRENADE_3				55
	4.6f * 1.15f,	-16.8f * 1.15f,	7.3f * 1.15f,		// MZ2_GUNNER_GRENADE_4				56

	24.8f,			-9.0f,			39.0f,			// MZ2_CHICK_ROCKET_1				57

	// Paril
	// Moved these up a bit
	12.1f,			13.4f,			-4.5f,			// MZ2_FLYER_BLASTER_1				58
	12.1f,			-7.4f,			-4.5f,			// MZ2_FLYER_BLASTER_2				59

	12.1f,			5.4f,			16.5f,			// MZ2_MEDIC_BLASTER_1				60

	30.0f,			18.0f,			28.0f,			// MZ2_GLADIATOR_RAILGUN_1			61

	32.5f,			-0.8f,			10.0f,			// MZ2_HOVER_BLASTER_1				62

	18.4f,			7.4f,			9.6f,			// MZ2_ACTOR_MACHINEGUN_1			63

	30.0f,			30.0f,			88.5f,			// MZ2_SUPERTANK_MACHINEGUN_1		64
	30.0f,			30.0f,			88.5f,			// MZ2_SUPERTANK_MACHINEGUN_2		65
	30.0f,			30.0f,			88.5f,			// MZ2_SUPERTANK_MACHINEGUN_3		66
	30.0f,			30.0f,			88.5f,			// MZ2_SUPERTANK_MACHINEGUN_4		67
	30.0f,			30.0f,			88.5f,			// MZ2_SUPERTANK_MACHINEGUN_5		68
	30.0f,			30.0f,			88.5f,			// MZ2_SUPERTANK_MACHINEGUN_6		69

	16.0f,			-22.5f,			91.2f,			// MZ2_SUPERTANK_ROCKET_1			70
	16.0f,			-33.4f,			86.7f,			// MZ2_SUPERTANK_ROCKET_2			71
	16.0f,			-42.8f,			83.3f,			// MZ2_SUPERTANK_ROCKET_3			72

	// --- Start Xian Stuff ---
	32.0f,			-40.0f,			70.0f,				// MZ2_BOSS2_MACHINEGUN_L1			73
	32.0f,			-40.0f,			70.0f,				// MZ2_BOSS2_MACHINEGUN_L2			74
	32.0f,			-40.0f,			70.0f,				// MZ2_BOSS2_MACHINEGUN_L3			75
	32.0f,			-40.0f,			70.0f,				// MZ2_BOSS2_MACHINEGUN_L4			76
	32.0f,			-40.0f,			70.0f,				// MZ2_BOSS2_MACHINEGUN_L5			77
	// --- End Xian Stuff ---

	22.0f,			16.0f,			10.0f,			// MZ2_BOSS2_ROCKET_1				78
	22.0f,			8.0f,			10.0f,			// MZ2_BOSS2_ROCKET_2				79
	22.0f,			-8.0f,			10.0f,			// MZ2_BOSS2_ROCKET_3				80
	22.0f,			-16.0f,			10.0f,			// MZ2_BOSS2_ROCKET_4				81

	32.5f,			-0.8f,			10.0f,				// MZ2_FLOAT_BLASTER_1				82

	20.8f * 1.2f,	10.1f * 1.2f,	-2.7f * 1.2f,		// MZ2_SOLDIER_BLASTER_3			83
	20.8f * 1.2f,	10.1f * 1.2f,	-2.7f * 1.2f,		// MZ2_SOLDIER_SHOTGUN_3			84
	20.8f * 1.2f,	10.1f * 1.2f,	-2.7f * 1.2f,		// MZ2_SOLDIER_MACHINEGUN_3			85
	7.6f * 1.2f,	9.3f * 1.2f,	0.8f * 1.2f,		// MZ2_SOLDIER_BLASTER_4			86
	7.6f * 1.2f,	9.3f * 1.2f,	0.8f * 1.2f,		// MZ2_SOLDIER_SHOTGUN_4			87
	7.6f * 1.2f,	9.3f * 1.2f,	0.8f * 1.2f,		// MZ2_SOLDIER_MACHINEGUN_4			88
	30.5f * 1.2f,	9.9f * 1.2f,	-18.7f * 1.2f,	// MZ2_SOLDIER_BLASTER_5			89
	30.5f * 1.2f,	9.9f * 1.2f,	-18.7f * 1.2f,	// MZ2_SOLDIER_SHOTGUN_5			90
	30.5f * 1.2f,	9.9f * 1.2f,	-18.7f * 1.2f,	// MZ2_SOLDIER_MACHINEGUN_5			91
	27.6f * 1.2f,	3.4f * 1.2f,	-10.4f * 1.2f,	// MZ2_SOLDIER_BLASTER_6			92
	27.6f * 1.2f,	3.4f * 1.2f,	-10.4f * 1.2f,	// MZ2_SOLDIER_SHOTGUN_6			93
	27.6f * 1.2f,	3.4f * 1.2f,	-10.4f * 1.2f,	// MZ2_SOLDIER_MACHINEGUN_6			94
	28.9f * 1.2f,	4.6f * 1.2f,	-8.1f * 1.2f,		// MZ2_SOLDIER_BLASTER_7			95
	28.9f * 1.2f,	4.6f * 1.2f,	-8.1f * 1.2f,		// MZ2_SOLDIER_SHOTGUN_7			96
	28.9f * 1.2f,	4.6f * 1.2f,	-8.1f * 1.2f,		// MZ2_SOLDIER_MACHINEGUN_7			97
	31.5f * 1.2f,	9.6f * 1.2f,	10.1f * 1.2f,		// MZ2_SOLDIER_BLASTER_8			98
	34.5f * 1.2f,	9.6f * 1.2f,	6.1f * 1.2f,		// MZ2_SOLDIER_SHOTGUN_8			99
	34.5f * 1.2f,	9.6f * 1.2f,	6.1f * 1.2f,		// MZ2_SOLDIER_MACHINEGUN_8			100

	// --- Xian shit below ---
	17.0f,			-19.5f,			62.9f,			// MZ2_MAKRON_BFG					101
	-3.6f,			-24.1f,			59.5f,			// MZ2_MAKRON_BLASTER_1				102
	-1.6f,			-19.3f,			59.5f,			// MZ2_MAKRON_BLASTER_2				103
	-0.1f,			-14.4f,			59.5f,			// MZ2_MAKRON_BLASTER_3				104
	2.0f,			-7.6f,			59.5f,			// MZ2_MAKRON_BLASTER_4				105
	3.4f,			1.3f,			59.5f,			// MZ2_MAKRON_BLASTER_5				106
	3.7f,			11.1f,			59.5f,			// MZ2_MAKRON_BLASTER_6				107
	-0.3f,			22.3f,			59.5f,			// MZ2_MAKRON_BLASTER_7				108
	-6.0f,			33.0f,			59.5f,			// MZ2_MAKRON_BLASTER_8				109
	-9.3f,			36.4f,			59.5f,			// MZ2_MAKRON_BLASTER_9				110
	-7.0f,			35.0f,			59.5f,			// MZ2_MAKRON_BLASTER_10			111
	-2.1f,			29.0f,			59.5f,			// MZ2_MAKRON_BLASTER_11			112
	3.9f,			17.3f,			59.5f,			// MZ2_MAKRON_BLASTER_12			113
	6.1f,			5.8f,			59.5f,			// MZ2_MAKRON_BLASTER_13			114
	5.9f,			-4.4f,			59.5f,			// MZ2_MAKRON_BLASTER_14			115
	4.2f,			-14.1f,			59.5f,			// MZ2_MAKRON_BLASTER_15			116
	2.4f,			-18.8f,			59.5f,			// MZ2_MAKRON_BLASTER_16			117
	-1.8f,			-25.5f,			59.5f,			// MZ2_MAKRON_BLASTER_17			118
	-17.3f,			7.8f,			72.4f,			// MZ2_MAKRON_RAILGUN_1				119

	78.5f,			-47.1f,			96.0f,				// MZ2_JORG_MACHINEGUN_L1			120
	78.5f,			-47.1f,			96.0f,				// MZ2_JORG_MACHINEGUN_L2			121
	78.5f,			-47.1f,			96.0f,				// MZ2_JORG_MACHINEGUN_L3			122
	78.5f,			-47.1f,			96.0f,				// MZ2_JORG_MACHINEGUN_L4			123
	78.5f,			-47.1f,			96.0f,				// MZ2_JORG_MACHINEGUN_L5			124
	78.5f,			-47.1f,			96.0f,				// MZ2_JORG_MACHINEGUN_L6			125
	78.5f,			46.7f,			96.0f,				// MZ2_JORG_MACHINEGUN_R1			126
	78.5f,			46.7f,			96.0f,				// MZ2_JORG_MACHINEGUN_R2			127
	78.5f,			46.7f,			96.0f,				// MZ2_JORG_MACHINEGUN_R3			128
	78.5f,			46.7f,			96.0f,				// MZ2_JORG_MACHINEGUN_R4			129
	78.5f,			46.7f,			96.0f,				// MZ2_JORG_MACHINEGUN_R5			130
	78.5f,			46.7f,			96.0f,				// MZ2_JORG_MACHINEGUN_R6			131
	6.3f,			-9.0f,			111.2f,			// MZ2_JORG_BFG_1					132

	32.0f,			40.0f,			70.0f,				// MZ2_BOSS2_MACHINEGUN_R1			73
	32.0f,			40.0f,			70.0f,				// MZ2_BOSS2_MACHINEGUN_R2			74
	32.0f,			40.0f,			70.0f,				// MZ2_BOSS2_MACHINEGUN_R3			75
	32.0f,			40.0f,			70.0f,				// MZ2_BOSS2_MACHINEGUN_R4			76
	32.0f,			40.0f,			70.0f,				// MZ2_BOSS2_MACHINEGUN_R5			77
	// --- End Xian Shit ---

	// ROGUE
	// - note that the above really ends at 137
	56.0f,				-32.0f,			32.0f,				// MZ2_CARRIER_MACHINEGUN_L1		138
	56.0f,				32.0f,			32.0f,				// MZ2_CARRIER_MACHINEGUN_R1		139
	42.0f,				24.0f,			50.0f,				// MZ2_CARRIER_GRENADE				140
	16.0f,				0.0f,			0.0f,				// MZ2_TURRET_MACHINEGUN			141
	16.0f,				0.0f,			0.0f,				// MZ2_TURRET_ROCKET				142
	16.0f,				0.0f,			0.0f,				// MZ2_TURRET_BLASTER				143
	24.0f,				0.0f,			6.0f,				// MZ2_STALKER_BLASTER				144
	32.5f,				-0.8f,			10.0f,			// MZ2_DAEDALUS_BLASTER				145
	12.1f,				5.4f,			16.5f,			// MZ2_MEDIC_BLASTER_2				146
	32.0f,				0.0f,			6.0f,				// MZ2_CARRIER_RAILGUN				147
	57.72f,				14.50f,			88.81f,			// MZ2_WIDOW_DISRUPTOR				148
	56.0f,				32.0f,			32.0f,				// MZ2_WIDOW_BLASTER				149
	62.0f,				-20.0f,			84.0f,				// MZ2_WIDOW_RAIL					150
	32.0f,				0.0f,			6.0f,				// MZ2_WIDOW_PLASMABEAM				151	PMM - not used!
	61.0f,				-32.0f,			12.0f,				// MZ2_CARRIER_MACHINEGUN_L2		152
	61.0f,				32.0f,			12.0f,				// MZ2_CARRIER_MACHINEGUN_R2		153
	17.0f,				-62.0f,			91.0f,				// MZ2_WIDOW_RAIL_LEFT				154
	68.0f,				12.0f,			86.0f,				// MZ2_WIDOW_RAIL_RIGHT				155
	47.5f,				56.0f,			89.0f,				// MZ2_WIDOW_BLASTER_SWEEP1			156	PMM - the sweeps need to be in sequential order
	54.0f,				52.0f,			91.0f,				// MZ2_WIDOW_BLASTER_SWEEP2			157
	58.0f,				40.0f,			91.0f,				// MZ2_WIDOW_BLASTER_SWEEP3			158
	68.0f,				30.0f,			88.0f,				// MZ2_WIDOW_BLASTER_SWEEP4			159
	74.0f,				20.0f,			88.0f,				// MZ2_WIDOW_BLASTER_SWEEP5			160
	73.0f,				11.0f,			87.0f,				// MZ2_WIDOW_BLASTER_SWEEP6			161
	73.0f,				3.0f,			87.0f,				// MZ2_WIDOW_BLASTER_SWEEP7			162
	70.0f,				-12.0f,			87.0f,				// MZ2_WIDOW_BLASTER_SWEEP8			163
	67.0f,				-20.0f,			90.0f,				// MZ2_WIDOW_BLASTER_SWEEP9			164

	-20.0f,				76.0f,			90.0f,				// MZ2_WIDOW_BLASTER_100			165
	-8.0f,				74.0f,			90.0f,				// MZ2_WIDOW_BLASTER_90				166
	0.0f,				72.0f,		90.0f,				// MZ2_WIDOW_BLASTER_80				167
	10.0f,				71.0f,			89.0f,				// MZ2_WIDOW_BLASTER_70				168		d06
	23.0f,				70.0f,			87.0f,				// MZ2_WIDOW_BLASTER_60				169		d07
	32.0f,				64.0f,			85.0f,				// MZ2_WIDOW_BLASTER_50				170		d08
	40.0f,				58.0f,			84.0f,				// MZ2_WIDOW_BLASTER_40				171
	48.0f,				50.0f,			83.0f,				// MZ2_WIDOW_BLASTER_30				172		d10
	54.0f,				42.0f,			82.0f,				// MZ2_WIDOW_BLASTER_20				173
	56.0f,				34.0f,			82.0f,				// MZ2_WIDOW_BLASTER_10				174		d12
	58.0f,				26.0f,			82.0f,				// MZ2_WIDOW_BLASTER_0				175
	60.0f,				16.0f,			82.0f,				// MZ2_WIDOW_BLASTER_10L			176		d14
	59.0f,				6.0f,			81.0f,				// MZ2_WIDOW_BLASTER_20L			177
	58.0f,				-2.0f,			80.0f,				// MZ2_WIDOW_BLASTER_30L			178		d16
	57.0f,				-10.0f,			79.0f,				// MZ2_WIDOW_BLASTER_40L			179
	54.0f,				-18.0f,			78.0f,				// MZ2_WIDOW_BLASTER_50L			180		d18
	42.0f,				-32.0f,			80.0f,				// MZ2_WIDOW_BLASTER_60L			181
	36.0f,				-40.0f,			78.0f,				// MZ2_WIDOW_BLASTER_70L			182		d20

	68.4f,				10.88f,			82.08f,			// MZ2_WIDOW_RUN_1				183
	68.51f,				8.64f,			85.14f,			// MZ2_WIDOW_RUN_2				184
	68.66f,				6.38f,			88.78f,			// MZ2_WIDOW_RUN_3				185
	68.73f,				5.1f,			84.47f,			// MZ2_WIDOW_RUN_4				186
	68.82f,				4.79f,			80.52f,			// MZ2_WIDOW_RUN_5				187
	68.77f,				6.11f,			85.37f,			// MZ2_WIDOW_RUN_6				188
	68.67f,				7.99f,			90.24f,			// MZ2_WIDOW_RUN_7				189
	68.55f,				9.54f,			87.36f,			// MZ2_WIDOW_RUN_8				190

	0.0f,				0.0f,			-5.0f,				// MZ2_CARRIER_ROCKET_1			191
	0.0f,				0.0f,			-5.0f,				// MZ2_CARRIER_ROCKET_2			192
	0.0f,				0.0f,			-5.0f,				// MZ2_CARRIER_ROCKET_3			193
	0.0f,				0.0f,			-5.0f,				// MZ2_CARRIER_ROCKET_4			194
	69.00f,				-17.63f,		93.77f,			// MZ2_WIDOW2_BEAMER_1			195
	69.00f,				-17.08f,		89.82f,			// MZ2_WIDOW2_BEAMER_2			196
	69.00f,				-18.40f,		90.70f,			// MZ2_WIDOW2_BEAMER_3			197
	69.00f,				-18.34f,		94.32f,			// MZ2_WIDOW2_BEAMER_4			198
	69.00f,				-18.30f,		97.98f,			// MZ2_WIDOW2_BEAMER_5			199
	45.04f,				-59.02f,		92.24f,			// MZ2_WIDOW2_BEAM_SWEEP_1		200
	50.68f,				-54.70f,		91.96f,			// MZ2_WIDOW2_BEAM_SWEEP_2		201
	56.57f,				-47.72f,		91.65f,			// MZ2_WIDOW2_BEAM_SWEEP_3		202
	61.75f,				-38.75f,		91.38f,			// MZ2_WIDOW2_BEAM_SWEEP_4		203
	65.55f,				-28.76f,		91.24f,			// MZ2_WIDOW2_BEAM_SWEEP_5		204
	67.79f,				-18.90f,		91.22f,			// MZ2_WIDOW2_BEAM_SWEEP_6		205
	68.60f,				-9.52f,			91.23f,			// MZ2_WIDOW2_BEAM_SWEEP_7		206
	68.08f,				0.18f,			91.32f,			// MZ2_WIDOW2_BEAM_SWEEP_8		207
	66.14f,				9.79f,			91.44f,			// MZ2_WIDOW2_BEAM_SWEEP_9		208
	62.77f,				18.91f,			91.65f,			// MZ2_WIDOW2_BEAM_SWEEP_10		209
	58.29f,				27.11f,			92.00f,			// MZ2_WIDOW2_BEAM_SWEEP_11		210

	0.0f,				0.0f,			0.0f				// end of table
};

enum
{
	MZ2_TANK_BLASTER_1				= 1,
	MZ2_TANK_BLASTER_2,
	MZ2_TANK_BLASTER_3,
	MZ2_TANK_MACHINEGUN_1,
	MZ2_TANK_MACHINEGUN_2,
	MZ2_TANK_MACHINEGUN_3,
	MZ2_TANK_MACHINEGUN_4,
	MZ2_TANK_MACHINEGUN_5,
	MZ2_TANK_MACHINEGUN_6,
	MZ2_TANK_MACHINEGUN_7,
	MZ2_TANK_MACHINEGUN_8,
	MZ2_TANK_MACHINEGUN_9,
	MZ2_TANK_MACHINEGUN_10,
	MZ2_TANK_MACHINEGUN_11,
	MZ2_TANK_MACHINEGUN_12,
	MZ2_TANK_MACHINEGUN_13,
	MZ2_TANK_MACHINEGUN_14,
	MZ2_TANK_MACHINEGUN_15,
	MZ2_TANK_MACHINEGUN_16,
	MZ2_TANK_MACHINEGUN_17,
	MZ2_TANK_MACHINEGUN_18,
	MZ2_TANK_MACHINEGUN_19,
	MZ2_TANK_ROCKET_1,
	MZ2_TANK_ROCKET_2,
	MZ2_TANK_ROCKET_3,

	MZ2_INFANTRY_MACHINEGUN_1,
	MZ2_INFANTRY_MACHINEGUN_2,
	MZ2_INFANTRY_MACHINEGUN_3,
	MZ2_INFANTRY_MACHINEGUN_4,
	MZ2_INFANTRY_MACHINEGUN_5,
	MZ2_INFANTRY_MACHINEGUN_6,
	MZ2_INFANTRY_MACHINEGUN_7,
	MZ2_INFANTRY_MACHINEGUN_8,
	MZ2_INFANTRY_MACHINEGUN_9,
	MZ2_INFANTRY_MACHINEGUN_10,
	MZ2_INFANTRY_MACHINEGUN_11,
	MZ2_INFANTRY_MACHINEGUN_12,
	MZ2_INFANTRY_MACHINEGUN_13,

	MZ2_SOLDIER_BLASTER_1,
	MZ2_SOLDIER_BLASTER_2,
	MZ2_SOLDIER_SHOTGUN_1,
	MZ2_SOLDIER_SHOTGUN_2,
	MZ2_SOLDIER_MACHINEGUN_1,
	MZ2_SOLDIER_MACHINEGUN_2,

	MZ2_GUNNER_MACHINEGUN_1,
	MZ2_GUNNER_MACHINEGUN_2,
	MZ2_GUNNER_MACHINEGUN_3,
	MZ2_GUNNER_MACHINEGUN_4,
	MZ2_GUNNER_MACHINEGUN_5,
	MZ2_GUNNER_MACHINEGUN_6,
	MZ2_GUNNER_MACHINEGUN_7,
	MZ2_GUNNER_MACHINEGUN_8,
	MZ2_GUNNER_GRENADE_1,
	MZ2_GUNNER_GRENADE_2,
	MZ2_GUNNER_GRENADE_3,
	MZ2_GUNNER_GRENADE_4,

	MZ2_CHICK_ROCKET_1,

	MZ2_FLYER_BLASTER_1,
	MZ2_FLYER_BLASTER_2,

	MZ2_MEDIC_BLASTER_1,

	MZ2_GLADIATOR_RAILGUN_1,

	MZ2_HOVER_BLASTER_1,

	MZ2_ACTOR_MACHINEGUN_1,

	MZ2_SUPERTANK_MACHINEGUN_1,
	MZ2_SUPERTANK_MACHINEGUN_2,
	MZ2_SUPERTANK_MACHINEGUN_3,
	MZ2_SUPERTANK_MACHINEGUN_4,
	MZ2_SUPERTANK_MACHINEGUN_5,
	MZ2_SUPERTANK_MACHINEGUN_6,
	MZ2_SUPERTANK_ROCKET_1,
	MZ2_SUPERTANK_ROCKET_2,
	MZ2_SUPERTANK_ROCKET_3,

	MZ2_BOSS2_MACHINEGUN_L1,
	MZ2_BOSS2_MACHINEGUN_L2,
	MZ2_BOSS2_MACHINEGUN_L3,
	MZ2_BOSS2_MACHINEGUN_L4,
	MZ2_BOSS2_MACHINEGUN_L5,
	MZ2_BOSS2_ROCKET_1,
	MZ2_BOSS2_ROCKET_2,
	MZ2_BOSS2_ROCKET_3,
	MZ2_BOSS2_ROCKET_4,

	MZ2_FLOAT_BLASTER_1,

	MZ2_SOLDIER_BLASTER_3,
	MZ2_SOLDIER_SHOTGUN_3,
	MZ2_SOLDIER_MACHINEGUN_3,
	MZ2_SOLDIER_BLASTER_4,
	MZ2_SOLDIER_SHOTGUN_4,
	MZ2_SOLDIER_MACHINEGUN_4,
	MZ2_SOLDIER_BLASTER_5,
	MZ2_SOLDIER_SHOTGUN_5,
	MZ2_SOLDIER_MACHINEGUN_5,
	MZ2_SOLDIER_BLASTER_6,
	MZ2_SOLDIER_SHOTGUN_6,
	MZ2_SOLDIER_MACHINEGUN_6,
	MZ2_SOLDIER_BLASTER_7,
	MZ2_SOLDIER_SHOTGUN_7,
	MZ2_SOLDIER_MACHINEGUN_7,
	MZ2_SOLDIER_BLASTER_8,
	MZ2_SOLDIER_SHOTGUN_8,
	MZ2_SOLDIER_MACHINEGUN_8,

	// --- Xian shit below ---
	MZ2_MAKRON_BFG,
	MZ2_MAKRON_BLASTER_1,
	MZ2_MAKRON_BLASTER_2,
	MZ2_MAKRON_BLASTER_3,
	MZ2_MAKRON_BLASTER_4,
	MZ2_MAKRON_BLASTER_5,
	MZ2_MAKRON_BLASTER_6,
	MZ2_MAKRON_BLASTER_7,
	MZ2_MAKRON_BLASTER_8,
	MZ2_MAKRON_BLASTER_9,
	MZ2_MAKRON_BLASTER_10,
	MZ2_MAKRON_BLASTER_11,
	MZ2_MAKRON_BLASTER_12,
	MZ2_MAKRON_BLASTER_13,
	MZ2_MAKRON_BLASTER_14,
	MZ2_MAKRON_BLASTER_15,
	MZ2_MAKRON_BLASTER_16,
	MZ2_MAKRON_BLASTER_17,
	MZ2_MAKRON_RAILGUN_1,
	MZ2_JORG_MACHINEGUN_L1,
	MZ2_JORG_MACHINEGUN_L2,
	MZ2_JORG_MACHINEGUN_L3,
	MZ2_JORG_MACHINEGUN_L4,
	MZ2_JORG_MACHINEGUN_L5,
	MZ2_JORG_MACHINEGUN_L6,
	MZ2_JORG_MACHINEGUN_R1,
	MZ2_JORG_MACHINEGUN_R2,
	MZ2_JORG_MACHINEGUN_R3,
	MZ2_JORG_MACHINEGUN_R4,
	MZ2_JORG_MACHINEGUN_R5,
	MZ2_JORG_MACHINEGUN_R6,
	MZ2_JORG_BFG_1,
	MZ2_BOSS2_MACHINEGUN_R1,
	MZ2_BOSS2_MACHINEGUN_R2,
	MZ2_BOSS2_MACHINEGUN_R3,
	MZ2_BOSS2_MACHINEGUN_R4,
	MZ2_BOSS2_MACHINEGUN_R5,

	// ROGUE
	MZ2_CARRIER_MACHINEGUN_L1,
	MZ2_CARRIER_MACHINEGUN_R1,
	MZ2_CARRIER_GRENADE,
	MZ2_TURRET_MACHINEGUN,
	MZ2_TURRET_ROCKET,
	MZ2_TURRET_BLASTER,
	MZ2_STALKER_BLASTER,
	MZ2_DAEDALUS_BLASTER,
	MZ2_MEDIC_BLASTER_2,
	MZ2_CARRIER_RAILGUN,
	MZ2_WIDOW_DISRUPTOR,
	MZ2_WIDOW_BLASTER,
	MZ2_WIDOW_RAIL,
	MZ2_WIDOW_PLASMABEAM,		// PMM - not used
	MZ2_CARRIER_MACHINEGUN_L2,
	MZ2_CARRIER_MACHINEGUN_R2,
	MZ2_WIDOW_RAIL_LEFT,
	MZ2_WIDOW_RAIL_RIGHT,
	MZ2_WIDOW_BLASTER_SWEEP1,
	MZ2_WIDOW_BLASTER_SWEEP2,
	MZ2_WIDOW_BLASTER_SWEEP3,
	MZ2_WIDOW_BLASTER_SWEEP4,
	MZ2_WIDOW_BLASTER_SWEEP5,
	MZ2_WIDOW_BLASTER_SWEEP6,
	MZ2_WIDOW_BLASTER_SWEEP7,
	MZ2_WIDOW_BLASTER_SWEEP8,
	MZ2_WIDOW_BLASTER_SWEEP9,
	MZ2_WIDOW_BLASTER_100,
	MZ2_WIDOW_BLASTER_90,
	MZ2_WIDOW_BLASTER_80,
	MZ2_WIDOW_BLASTER_70,
	MZ2_WIDOW_BLASTER_60,
	MZ2_WIDOW_BLASTER_50,
	MZ2_WIDOW_BLASTER_40,
	MZ2_WIDOW_BLASTER_30,
	MZ2_WIDOW_BLASTER_20,
	MZ2_WIDOW_BLASTER_10,
	MZ2_WIDOW_BLASTER_0,
	MZ2_WIDOW_BLASTER_10L,
	MZ2_WIDOW_BLASTER_20L,
	MZ2_WIDOW_BLASTER_30L,
	MZ2_WIDOW_BLASTER_40L,
	MZ2_WIDOW_BLASTER_50L,
	MZ2_WIDOW_BLASTER_60L,
	MZ2_WIDOW_BLASTER_70L,
	MZ2_WIDOW_RUN_1,
	MZ2_WIDOW_RUN_2,
	MZ2_WIDOW_RUN_3,
	MZ2_WIDOW_RUN_4,
	MZ2_WIDOW_RUN_5,
	MZ2_WIDOW_RUN_6,
	MZ2_WIDOW_RUN_7,
	MZ2_WIDOW_RUN_8,
	MZ2_CARRIER_ROCKET_1,
	MZ2_CARRIER_ROCKET_2,
	MZ2_CARRIER_ROCKET_3,
	MZ2_CARRIER_ROCKET_4,
	MZ2_WIDOW2_BEAMER_1,
	MZ2_WIDOW2_BEAMER_2,
	MZ2_WIDOW2_BEAMER_3,
	MZ2_WIDOW2_BEAMER_4,
	MZ2_WIDOW2_BEAMER_5,
	MZ2_WIDOW2_BEAM_SWEEP_1,
	MZ2_WIDOW2_BEAM_SWEEP_2,
	MZ2_WIDOW2_BEAM_SWEEP_3,
	MZ2_WIDOW2_BEAM_SWEEP_4,
	MZ2_WIDOW2_BEAM_SWEEP_5,
	MZ2_WIDOW2_BEAM_SWEEP_6,
	MZ2_WIDOW2_BEAM_SWEEP_7,
	MZ2_WIDOW2_BEAM_SWEEP_8,
	MZ2_WIDOW2_BEAM_SWEEP_9,
	MZ2_WIDOW2_BEAM_SWEEP_10,
	MZ2_WIDOW2_BEAM_SWEEP_11
	// ROGUE
};

/*
==============================================================================

	TEMP ENTITY EVENTS

==============================================================================
*/

// Temp entity events are for things that happen at a location seperate from
// any existing entity. Temporary entity messages are explicitly constructed
// and broadcast.
enum
{
	TE_GUNSHOT,
	TE_BLOOD,
	TE_BLASTER,
	TE_RAILTRAIL,
	TE_SHOTGUN,
	TE_EXPLOSION1,
	TE_EXPLOSION2,
	TE_ROCKET_EXPLOSION,
	TE_GRENADE_EXPLOSION,
	TE_SPARKS,
	TE_SPLASH,
	TE_BUBBLETRAIL,
	TE_SCREEN_SPARKS,
	TE_SHIELD_SPARKS,
	TE_BULLET_SPARKS,
	TE_LASER_SPARKS,
	TE_PARASITE_ATTACK,
	TE_ROCKET_EXPLOSION_WATER,
	TE_GRENADE_EXPLOSION_WATER,
	TE_MEDIC_CABLE_ATTACK,
	TE_BFG_EXPLOSION,
	TE_BFG_BIGEXPLOSION,
	TE_BOSSTPORT,
	TE_BFG_LASER,
	TE_GRAPPLE_CABLE,
	TE_WELDING_SPARKS,
	TE_GREENBLOOD,
	TE_BLUEHYPERBLASTER,
	TE_PLASMA_EXPLOSION,
	TE_TUNNEL_SPARKS,

	//ROGUE
	TE_BLASTER2,
	TE_RAILTRAIL2,
	TE_FLAME,
	TE_LIGHTNING,
	TE_DEBUGTRAIL,
	TE_PLAIN_EXPLOSION,
	TE_FLASHLIGHT,
	TE_FORCEWALL,
	TE_HEATBEAM,
	TE_MONSTER_HEATBEAM,
	TE_STEAM,
	TE_BUBBLETRAIL2,
	TE_MOREBLOOD,
	TE_HEATBEAM_SPARKS,
	TE_HEATBEAM_STEAM,
	TE_CHAINFIST_SMOKE,
	TE_ELECTRIC_SPARKS,
	TE_TRACKER_EXPLOSION,
	TE_TELEPORT_EFFECT,
	TE_DBALL_GOAL,
	TE_WIDOWBEAMOUT,
	TE_NUKEBLAST,
	TE_WIDOWSPLASH,
	TE_EXPLOSION1_BIG,
	TE_EXPLOSION1_NP,
	TE_FLECHETTE
	//ROGUE
};

// TE_SPLASH effects
enum
{
	SPLASH_UNKNOWN,
	SPLASH_SPARKS,
	SPLASH_BLUE_WATER,
	SPLASH_BROWN_WATER,
	SPLASH_SLIME,
	SPLASH_LAVA,
	SPLASH_BLOOD
};

/*
==============================================================================

	SOUND

==============================================================================
*/

//
// sound channels
// channel 0 never willingly overrides.
// Other channels (1-7) allways override
// a playing sound on that channel
//
CC_ENUM (uint8, EEntSndChannel)
{
	CHAN_AUTO,
	CHAN_WEAPON,
	CHAN_VOICE,
	CHAN_ITEM,
	CHAN_BODY,

	CHAN_MAX			= 8,

	// modifier flags
	CHAN_NO_PHS_ADD		= 8,	// send to all clients, not just ones in PHS (ATTN 0 will also do this)
	CHAN_RELIABLE		= 16	// send by reliable message, not datagram
};

//
// sound attenuation values
//
CC_ENUM (uint8, EAttenuation)
{
	ATTN_NONE,				// full volume the entire level
	ATTN_NORM,
	ATTN_IDLE,
	ATTN_STATIC				// diminish very rapidly with distance
};

/*
==============================================================================

	DEATHMATCH FLAGS

==============================================================================
*/

// dmflags->floatVal flags
CC_ENUM (int, EDeathmatchFlags)
{
	DF_NO_HEALTH		= BIT(0),
	DF_NO_ITEMS			= BIT(1),
	DF_WEAPONS_STAY		= BIT(2),
	DF_NO_FALLING		= BIT(3),
	DF_INSTANT_ITEMS	= BIT(4),
	DF_SAME_LEVEL		= BIT(5),
	DF_SKINTEAMS		= BIT(6),
	DF_MODELTEAMS		= BIT(7),
	DF_NO_FRIENDLY_FIRE	= BIT(8),
	DF_SPAWN_FARTHEST	= BIT(9),
	DF_FORCE_RESPAWN	= BIT(10),
	DF_NO_ARMOR			= BIT(11),
	DF_ALLOW_EXIT		= BIT(12),
	DF_INFINITE_AMMO	= BIT(13),
	DF_QUAD_DROP		= BIT(14),
	DF_FIXED_FOV		= BIT(15),

	// Xatrix
	DF_QUADFIRE_DROP	= BIT(16),

	// Rogue
	DF_NO_MINES			= BIT(17),
	DF_NO_STACK_DOUBLE	= BIT(18),
	DF_NO_NUKES			= BIT(19),
	DF_NO_SPHERES		= BIT(20),

	// CTF
#ifdef CLEANCTF_ENABLED
	DF_CTF_FORCEJOIN	= BIT(21),	
	DF_ARMOR_PROTECT	= BIT(22),
	DF_CTF_NO_TECH      = BIT(23),
#endif

	// CleanCode
	DF_DM_TECHS			= BIT(24)
};

/*
==============================================================================

	CONFIG STRINGS

==============================================================================
*/

// per-level limits
#define MAX_CS_CLIENTS		256		// absolute limit
#define MAX_CS_EDICTS		1024	// must change protocol to increase more
#define MAX_CS_LIGHTSTYLES	256
#define MAX_CS_MODELS		256		// these are sent over the net as bytes
#define MAX_CS_SOUNDS		256		// so they cannot be blindly increased
#define MAX_CS_IMAGES		256
#define MAX_CS_ITEMS		256
#define MAX_CS_GENERAL		(MAX_CS_CLIENTS*2)	// general config strings

#define Q2BSP_MAX_AREAS		256
#define MAX_AREA_BITS		(Q2BSP_MAX_AREAS/8)

// config strings are a general means of communication from the server to all
// connected clients. Each config string can be at most MAX_CFGSTRLEN characters.
CC_ENUM (int, EConfigStringIndexes)
{
	CS_NAME,
	CS_CDTRACK,
	CS_SKY,
	CS_SKYAXIS, // %f %f %f format
	CS_SKYROTATE,
	CS_STATUSBAR,

	CS_AIRACCEL = 29,
	CS_MAXCLIENTS,
	CS_MAPCHECKSUM,

	CS_MODELS,
	CS_SOUNDS			= (CS_MODELS+MAX_CS_MODELS),
	CS_IMAGES			= (CS_SOUNDS+MAX_CS_SOUNDS),
	CS_LIGHTS			= (CS_IMAGES+MAX_CS_IMAGES),
	CS_ITEMS			= (CS_LIGHTS+MAX_CS_LIGHTSTYLES),
	CS_PLAYERSKINS		= (CS_ITEMS+MAX_CS_ITEMS),
	CS_GENERAL			= (CS_PLAYERSKINS+MAX_CS_CLIENTS),

	MAX_CFGSTRINGS		= (CS_GENERAL+MAX_CS_GENERAL),
	MAX_CFGSTRLEN		= 64,

	// Paril, debugging
	CS_POINTING_SURFACE	= (MAX_CFGSTRINGS - 1)
};

/*
==============================================================================

	ENTITY STATE

==============================================================================
*/

// entityState_t->event values
// ertity events are for effects that take place reletive to an existing
// entities origin.  Very network efficient. All muzzle flashes really should
// be converted to events...
CC_ENUM (int, EEventEffect)
{
	EV_NONE,
	EV_ITEM_RESPAWN,
	EV_FOOTSTEP,
	EV_FALLSHORT,
	EV_FALL,
	EV_FALLFAR,
	EV_PLAYER_TELEPORT,
	EV_OTHER_TELEPORT
};

#ifndef GAME_IS_BEING_COMPILED_NOT_ENGINE_GO_AWAY
// entityState_t is the information conveyed from the server in an update
// message about entities that the client will need to render in some way
struct entityState_t
{
	int				number;		// edict index

	vec3_t			origin;		// entity origin or RF_BEAM start origin
	vec3_t			angles;
	vec3_t			oldOrigin;	// for interpolation or RF_BEAM end origin

	// weapons, CTF flags, etc
	int				modelIndex;
	int				modelIndex2;
	int				modelIndex3;
	int				modelIndex4;

	int				frame;		// also RF_BEAM's size
	int				skinNum;	// also RF_BEAM color index

	EEntityStateEffects			effects;	// PGM - we're filling it, so it needs to be uint32
	EEntityStateRenderEffects	renderFx;
	int				solid;		// for client side prediction, 8*(bits 0-4) is x/y radius
								// 8*(bits 5-9) is z down distance, 8(bits10-15) is z up
								// gi.linkentity sets this properly
	int				sound;		// for looping sounds, to guarantee shutoff
	EEventEffect	event;		// impulse events -- muzzle flashes, footsteps, etc
								// events only go out for a single frame, they
								// are automatically cleared each frame
	vec3_t			velocity;	// for new ENHANCED_PROTOCOL_VERSION
};
#endif

struct entityStateOld_t
{
	int				number;		// edict index

	vec3f			origin;		// entity origin or RF_BEAM start origin
	vec3f			angles;
	vec3f			oldOrigin;	// for interpolation or RF_BEAM end origin

	// weapons, CTF flags, etc
	int				modelIndex;
	int				modelIndex2;
	int				modelIndex3;
	int				modelIndex4;

	int				frame;		// also RF_BEAM's size
	int				skinNum;	// also RF_BEAM color index

	EEntityStateEffects			effects;	// PGM - we're filling it, so it needs to be uint32
	EEntityStateRenderEffects	renderFx;
	int				solid;		// for client side prediction, 8*(bits 0-4) is x/y radius
								// 8*(bits 5-9) is z down distance, 8(bits10-15) is z up
								// gi.linkentity sets this properly
	int				sound;		// for looping sounds, to guarantee shutoff
	EEventEffect	event;		// impulse events -- muzzle flashes, footsteps, etc
								// events only go out for a single frame, they
								// are automatically cleared each frame
};

/*
==============================================================================

	PLAYER STATE

==============================================================================
*/

// playerState->stats[] indexes
CC_ENUM (int16, EStatIndex)
{
	STAT_HEALTH_ICON,
	STAT_HEALTH,
	STAT_AMMO_ICON,
	STAT_AMMO,
	STAT_ARMOR_ICON,
	STAT_ARMOR,
	STAT_SELECTED_ICON,
	STAT_PICKUP_ICON,
	STAT_PICKUP_STRING,
	STAT_TIMER_ICON,
	STAT_TIMER,
	STAT_HELPICON,
	STAT_SELECTED_ITEM,
	STAT_LAYOUTS,
	STAT_FRAGS,
	STAT_FLASHES,					// cleared each frame, 1 = health, 2 = armor
	STAT_CHASE,
	STAT_SPECTATOR,

#ifdef CLEANCTF_ENABLED
	STAT_CTF_TEAM1_PIC,
	STAT_CTF_TEAM1_CAPS,
	STAT_CTF_TEAM2_PIC,
	STAT_CTF_TEAM2_CAPS,
	STAT_CTF_FLAG_PIC,
	STAT_CTF_JOINED_TEAM1_PIC,
	STAT_CTF_JOINED_TEAM2_PIC,
	STAT_CTF_TEAM1_HEADER,
	STAT_CTF_TEAM2_HEADER,
	STAT_CTF_ID_VIEW,
	STAT_CTF_MATCH,
#endif

	STAT_TECH,

	MAX_STATS				= 32
};

// playerState_t->rdFlags
CC_ENUM (int, ERenderDefFlags)
{
	RDF_UNDERWATER		= BIT(0),		// warp the screen as apropriate
	RDF_NOWORLDMODEL	= BIT(1),		// used for player configuration screen
	RDF_IRGOGGLES		= BIT(2),
	RDF_UVGOGGLES		= BIT(3),

	RDF_OLDAREABITS		= BIT(4),
};

// playerState_t is the information needed in addition to pMoveState_t to
// rendered a view.  There will only be 10 playerState_t sent each second, but
// the number of pMoveState_t changes will be reletive to client frame rates
#ifndef GAME_IS_BEING_COMPILED_NOT_ENGINE_GO_AWAY
struct playerStateNew_t
{
	pMoveState_t	pMove;				// for prediction

	// these fields do not need to be communicated bit-precise
	vec3_t			viewAngles;			// for fixed views
	vec3_t			viewOffset;			// add to pmovestate->origin
	vec3_t			kickAngles;			// add to view direction to get render angles
										// set by weapon kicks, pain effects, etc
	vec3_t			gunAngles;
	vec3_t			gunOffset;
	int				gunIndex;
	int				gunFrame;

	vec4_t			viewBlend;		// rgba full screen effect
	
	float			fov;				// horizontal field of view

	ERenderDefFlags	rdFlags;			// refdef flags

	int16			stats[MAX_STATS];	// fast status bar updates

	vec3_t			mins;
	vec3_t			maxs;
};
#endif

struct playerState_t
{
	pMoveState_t	pMove;				// for prediction

	// these fields do not need to be communicated bit-precise
	vec3f			viewAngles;			// for fixed views
	vec3f			viewOffset;			// add to pmovestate->origin
	vec3f			kickAngles;			// add to view direction to get render angles
										// set by weapon kicks, pain effects, etc
	vec3f			gunAngles;
	vec3f			gunOffset;
	int				gunIndex;
	int				gunFrame;

	colorf			viewBlend;		// rgba full screen effect
	
	float			fov;				// horizontal field of view

	ERenderDefFlags	rdFlags;			// refdef flags

	EStatIndex		stats[MAX_STATS];	// fast status bar updates
};

// CleanCode Stuff

// Means of Death definition
typedef uint32	EMeansOfDeath;

#endif // __SHARED_H__
