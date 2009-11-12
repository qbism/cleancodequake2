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
// cc_platform.h
// Main header file. Contains the common definitions.
//

#if !defined(__CC_PLATFORM_H__) || !defined(INCLUDE_GUARDS)
#define __CC_PLATFORM_H__

void Sys_FindFiles (TFindFilesType &files, char *path, char *pattern, char **fileList, bool recurse, bool addFiles, bool addDirs);
void CC_OutputDebugString (const char *text);
void CC_ReportGameError (const char *text);

// A class to simplify calls that will be ultimately different on
// OSes
#define TIMER_STRING "%6.2fms"

class CTimer
{
public:
	double StartCycles;

public:
	CTimer (bool StartNow = true);

	void Start ();
	double Get (); // Automatically resets the timer as well
};

/*
#ifdef WIN32
#define _DECL_DLLMAIN   // enable prototypes for DllMain and _CRT_INIT
#include <Windows.h>
#include <process.h>

BOOL WINAPI DllInit(HINSTANCE hinstDLL, DWORD fdwReason,
	LPVOID lpReserved)
{
	if (fdwReason == DLL_PROCESS_ATTACH)
	{

#else

void __attribute__ ((constructor)) my_load();
void __attribute__ ((destructor)) my_unload();

// Called when the library is loaded and before dlopen() returns
void my_load()
{
#endif
		Mem_Init ();

#ifdef WIN32
		if (!_CRT_INIT(hinstDLL, fdwReason, lpReserved))
			return FALSE;
	}

	if (fdwReason == DLL_PROCESS_DETACH)
	{
#else
// Add initialization code�
}

// Called when the library is unloaded and before dlclose()
// returns
void my_unload()
{
#endif
		Mem_FreePool (com_gamePool);
		Mem_FreePool (com_levelPool);
		Mem_FreePool (com_genericPool);

#ifdef WIN32
		if (!_CRT_INIT(hinstDLL, fdwReason, lpReserved))
			return FALSE;

		DisableThreadLibraryCalls (hinstDLL);
	}

	return TRUE;
#endif
}
*/

#else
FILE_WARNING
#endif