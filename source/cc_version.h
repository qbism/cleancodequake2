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

#if !defined(__CC_VERSION_H__) || !defined(INCLUDE_GUARDS)
#define __CC_VERSION_H__

#define CLEANCODE_VERSION_PREFIX	"pr"
#define CLEANCODE_VERSION_MAJOR		"0"
#define CLEANCODE_VERSION_MINOR		"0000"
#define CLEANCODE_VERSION_BUILD		"00200"

static const uint8 verMajor = atoi(CLEANCODE_VERSION_MAJOR);
static const uint16 verMinor = atoi(CLEANCODE_VERSION_MINOR);
static const uint32 verBuild = atoi(CLEANCODE_VERSION_BUILD);

#define CLEANCODE_VERSION			"\""CLEANCODE_VERSION_PREFIX"."CLEANCODE_VERSION_MAJOR"."CLEANCODE_VERSION_MINOR"."CLEANCODE_VERSION_BUILD"\""

CC_ENUM (uint8, EVersionComparison)
{
	VERSION_SAME,
	VERSION_OLDER,
	VERSION_NEWER,
};

inline EVersionComparison CompareVersion (const char *Prefix, uint8 Major, uint16 Minor, uint32 Build)
{
	if (!strcmp (Prefix, CLEANCODE_VERSION_PREFIX) &&
		Major == verMajor &&
		Minor == verMinor &&
		Build == verBuild)
		return VERSION_SAME;
	else if (Major > verMajor ||
			Minor > verMinor ||
			Build > verBuild ||
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