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
// cc_version.h
// 
//

#if !defined(__CC_VERSION_H__) || !INCLUDE_GUARDS
#define __CC_VERSION_H__

#define CLEANCODE_VERSION_PREFIX	"rc"
#define CLEANCODE_VERSION_MAJOR_N	0			// x
#define CLEANCODE_VERSION_MINOR_N	0			// xxxx
#define CLEANCODE_VERSION_BUILD_N	239			// xxxx

#define CLEANCODE_VERSION_PRINT			"\"%s.%u.%04u.%05u\""
#define CLEANCODE_VERSION_PRINT_ARGS	CLEANCODE_VERSION_PREFIX, CLEANCODE_VERSION_MAJOR_N, CLEANCODE_VERSION_MINOR_N, CLEANCODE_VERSION_BUILD_N

CC_ENUM (uint8, EVersionComparison)
{
	VERSION_SAME,
	VERSION_OLDER,
	VERSION_NEWER,
};

inline EVersionComparison CompareVersion (const char *Prefix, uint8 Major, uint16 Minor, uint32 Build)
{
	if (!strcmp (Prefix, CLEANCODE_VERSION_PREFIX) &&
		Major == CLEANCODE_VERSION_MAJOR_N &&
		Minor == CLEANCODE_VERSION_MINOR_N &&
		Build == CLEANCODE_VERSION_BUILD_N)
		return VERSION_SAME;
	else if (Major > CLEANCODE_VERSION_MAJOR_N ||
			Minor > CLEANCODE_VERSION_MINOR_N ||
			Build > CLEANCODE_VERSION_BUILD_N ||
			strcmp (Prefix, CLEANCODE_VERSION_PREFIX))
		return VERSION_NEWER;
	else
		return VERSION_OLDER;
}

void Cmd_CCVersion_t (CPlayerEntity *Player);
void SvCmd_CCVersion_t ();
void InitVersion ();

#else
FILE_WARNING
#endif