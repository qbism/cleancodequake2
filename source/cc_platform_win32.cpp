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
// cc_platform_win32.cpp
// Win32/64 specific functions.
//

#include "cc_local.h"

#ifdef _WIN32
#define _DECL_DLLMAIN   /* enable prototypes for DllMain and _CRT_INIT */
#include <windows.h>
#include <process.h>
#include <crtdbg.h>

char		sys_findBase[MAX_OSPATH];
char		sys_findPath[MAX_OSPATH];
HANDLE		sys_findHandle = INVALID_HANDLE_VALUE;

static bool Sys_CompareFileAttributes (int found, uint32 mustHave, uint32 cantHave)
{
	// read only
	if (found & FILE_ATTRIBUTE_READONLY) {
		if (cantHave & SFF_RDONLY)
			return false;
	}
	else if (mustHave & SFF_RDONLY)
		return false;

	// hidden
	if (found & FILE_ATTRIBUTE_HIDDEN) {
		if (cantHave & SFF_HIDDEN)
			return false;
	}
	else if (mustHave & SFF_HIDDEN)
		return false;

	// system
	if (found & FILE_ATTRIBUTE_SYSTEM) {
		if (cantHave & SFF_SYSTEM)
			return false;
	}
	else if (mustHave & SFF_SYSTEM)
		return false;

	// subdir
	if (found & FILE_ATTRIBUTE_DIRECTORY) {
		if (cantHave & SFF_SUBDIR)
			return false;
	}
	else if (mustHave & SFF_SUBDIR)
		return false;

	// arch
	if (found & FILE_ATTRIBUTE_ARCHIVE) {
		if (cantHave & SFF_ARCH)
			return false;
	}
	else if (mustHave & SFF_ARCH)
		return false;

	return true;
}

/*
================
Sys_FindClose
================
*/
void Sys_FindClose ()
{
	if (sys_findHandle != INVALID_HANDLE_VALUE)
		FindClose (sys_findHandle);

	sys_findHandle = INVALID_HANDLE_VALUE;
}


/*
================
Sys_FindFirst
================
*/
char *Sys_FindFirst (char *path, uint32 mustHave, uint32 cantHave)
{
	WIN32_FIND_DATA	findinfo;

	if (sys_findHandle != INVALID_HANDLE_VALUE)
		Com_Printf (0, "Sys_BeginFind without close");

	Com_FilePath (path, sys_findBase, sizeof(sys_findBase));
	sys_findHandle = FindFirstFile (path, &findinfo);

	if (sys_findHandle == INVALID_HANDLE_VALUE)
		return NULL;

	if (!Sys_CompareFileAttributes (findinfo.dwFileAttributes, mustHave, cantHave))
		return NULL;

	Q_snprintfz (sys_findPath, sizeof(sys_findPath), "%s/%s", sys_findBase, findinfo.cFileName);
	return sys_findPath;
}


/*
================
Sys_FindNext
================
*/
char *Sys_FindNext (uint32 mustHave, uint32 cantHave)
{
	WIN32_FIND_DATA	findinfo;

	if (sys_findHandle == INVALID_HANDLE_VALUE)
		return NULL;

	if (!FindNextFile (sys_findHandle, &findinfo))
		return NULL;

	if (!Sys_CompareFileAttributes (findinfo.dwFileAttributes, mustHave, cantHave))
		return NULL;

	Q_snprintfz (sys_findPath, sizeof(sys_findPath), "%s/%s", sys_findBase, findinfo.cFileName);
	return sys_findPath;
}


/*
================
Sys_FindFiles
================
*/
int Sys_FindFiles (char *path, char *pattern, char **fileList, int maxFiles, int fileCount, bool recurse, bool addFiles, bool addDirs)
{
	WIN32_FIND_DATA	findInfo;
	HANDLE			findHandle;
	BOOL			findRes = TRUE;
	char			findPath[MAX_OSPATH], searchPath[MAX_OSPATH];

	Q_snprintfz (searchPath, sizeof(searchPath), "%s/*", path);

	findHandle = FindFirstFile (searchPath, &findInfo);
	if (findHandle == INVALID_HANDLE_VALUE)
		return fileCount;

	while (findRes == TRUE) {
		// Check for invalid file name
		if (findInfo.cFileName[strlen(findInfo.cFileName)-1] == '.') {
			findRes = FindNextFile (findHandle, &findInfo);
			continue;
		}

		Q_snprintfz (findPath, sizeof(findPath), "%s/%s", path, findInfo.cFileName);

		if (findInfo.dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY) {
			// Add a directory
			if (addDirs && fileCount < maxFiles)
				fileList[fileCount++] = Mem_StrDup (findPath);

			// Recurse down the next directory
			if (recurse)
				fileCount = Sys_FindFiles (findPath, pattern, fileList, maxFiles, fileCount, recurse, addFiles, addDirs);
		}
		else {
			// Match pattern
			if (!Q_WildcardMatch (pattern, findPath, 1)) {
				findRes = FindNextFile (findHandle, &findInfo);
				continue;
			}

			// Add a file
			if (addFiles && fileCount < maxFiles)
				fileList[fileCount++] = Mem_StrDup (findPath);
		}

		findRes = FindNextFile (findHandle, &findInfo);
	}

	FindClose (findHandle);

	return fileCount;
}

/*
================
CC_OutputDebugString

Output the string to debuggers and such.
================
*/
void CC_OutputDebugString (const char *text)
{
#if defined(_DEBUG)
	// Pipe to visual studio in debug mode
	OutputDebugString(text);
#endif
}

/*
================
CC_OutputDebugString

Use this to properly break the debuggers
so you can debug fatal game errors
================
*/
void CC_ReportGameError (const char *text)
{
#if defined(_DEBUG)
	// Pipe to visual studio
	OutputDebugString(text);
#endif
	_CrtDbgBreak ();
	
	// NOTE TO SELF:
	// In unix:
	// assert (0); // Break, if debugging
}

static int sys_timeBase;
static double sys_msPerCycle;

/*
================
Sys_Cycles
================
*/
uint32 Sys_Cycles()
{
	LARGE_INTEGER PC;

	QueryPerformanceCounter(&PC);

	return PC.QuadPart;
}


/*
================
Sys_MSPerCycle
================
*/
double Sys_MSPerCycle()
{
	return sys_msPerCycle;
}


/*
================
Sys_Milliseconds
================
*/
int Sys_Milliseconds()
{
	return timeGetTime() - sys_timeBase;
}


/*
================
Sys_UMilliseconds
================
*/
uint32 Sys_UMilliseconds()
{
	return timeGetTime() - sys_timeBase;
}

// Timer class
CTimer::CTimer (bool StartNow)
{
	if (StartNow)
		Start ();
};

void CTimer::Start ()
{
	StartCycles = Sys_Cycles();
};

double CTimer::Get ()
{
	// Grab the value
	double value = (Sys_Cycles()-StartCycles) * Sys_MSPerCycle();

	// Restart the timer
	Start ();

	return value;
};

// DLL entry point (for CleanCode-managed memory)
BOOL WINAPI DllInit(HINSTANCE hinstDLL, DWORD fdwReason, LPVOID lpReserved)
{
	switch (fdwReason)
	{
	case DLL_PROCESS_ATTACH:
		DisableThreadLibraryCalls (hinstDLL);

		// Make sure the timer is high precision, otherwise NT gets 18ms resolution
		timeBeginPeriod(1);
		sys_timeBase = timeGetTime() & 0xffff0000;

		// This is later multiplied times a cycle duration to get the CPU time in MS
		{
			LARGE_INTEGER PF;
			QueryPerformanceFrequency(&PF);
			sys_msPerCycle = (1.0 / (double)PF.QuadPart) * 1000.0;
		}

		Mem_Init ();

		if (!_CRT_INIT(hinstDLL, fdwReason, lpReserved))
			return FALSE;
		break;

	case DLL_PROCESS_DETACH:
		if (!_CRT_INIT(hinstDLL, fdwReason, lpReserved))
			return FALSE;

		Mem_FreePool (com_levelPool);
		Mem_FreePool (com_gamePool);
		Mem_FreePool (com_genericPool);

		break;

	default:
		break;
	}

	return TRUE;
}
#endif