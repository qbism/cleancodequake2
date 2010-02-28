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
// cc_filesystem.cpp
// 
//

#include "cc_local.h"

CFileHandleList *CFile::IndexList;

fs_pathListType fs_pathList;

// Adds a path to the path list
void FS_AddPath (const char *pathName)
{
	fs_pathIndex *path = QNew (TAG_GENERIC) fs_pathIndex;
	strncpy (path->pathName, pathName, sizeof(path->pathName));
	fs_pathList.push_back (path);
}

// Removes a path from the list
void FS_RemovePath (const char *pathName)
{
	for (fs_pathListType::iterator it = fs_pathList.begin(); it < fs_pathList.end(); ++it)
	{
		if (strcmp(pathName, (*it)->pathName) == 0)
		{
			delete (*it);
			fs_pathList.erase (it);
			return;
		}
	}
}

// Re-orders this path to be at the first position.
// If it doesn't exist, it adds it.
void FS_ReorderPath (const char *pathName)
{
	for (fs_pathListType::iterator it = fs_pathList.begin(); it < fs_pathList.end(); ++it)
	{
		if (strcmp(pathName, (*it)->pathName) == 0)
		{
			fs_pathIndex *Path = (*it);

			fs_pathList.erase (it);
			fs_pathList.insert (fs_pathList.begin(), Path);
			return;
		}
	}

	fs_pathIndex *Path = QNew (TAG_GENERIC) fs_pathIndex;
	strncpy (Path->pathName, pathName, sizeof(Path->pathName));

	fs_pathList.insert (fs_pathList.begin(), Path);
}

THandleIndexListType::iterator CFileHandleList::Create ()
{
	fileHandle_t CreatedKey = numHandlesAllocated++;
	OpenList[CreatedKey] = fileHandleIndex_t(CreatedKey);
	return OpenList.find(CreatedKey);
};

// Moves "it" from OpenList to ClosedList
THandleIndexListType::iterator CFileHandleList::MoveToClosed (THandleIndexListType::iterator it)
{
	fileHandle_t Key = (*it).first;
	ClosedList[Key] = (*it).second;
	OpenList.erase (it);
	return ClosedList.find(Key);
};

// Moves "it" from ClosedList to OpenList
THandleIndexListType::iterator CFileHandleList::MoveToOpen (THandleIndexListType::iterator it)
{
	(*it).second.Clear ();

	fileHandle_t Key = (*it).first;
	OpenList[Key] = (*it).second;
	ClosedList.erase (it);
	return OpenList.find(Key);
};

// allocated = number of handles to create automatically.
CFileHandleList::CFileHandleList (sint32 allocated) :
	numHandlesAllocated (0)
{
	for (fileHandle_t i = 0; i < allocated; i++)
		Create ();
};

// Returns either a free key in Open or
// creates a new key and returns it.
THandleIndexListType::iterator CFileHandleList::GetFreeHandle ()
{
	if (!OpenList.empty())
		// Re-use a key
		return MoveToClosed(OpenList.begin());
	else
		// Create a new key and use that
		return MoveToClosed(Create ());
};

// Gets the fileHandleIndex_t of a handle in-use
fileHandleIndex_t *CFileHandleList::GetHandle (fileHandle_t Key)
{
	return &(*ClosedList.find(Key)).second;
};

// Pushes a key back into the Open list
// Use this when you're done with a key
void CFileHandleList::PushFreeHandle (fileHandle_t Key)
{
	MoveToOpen (ClosedList.find(Key));
};

fileHandle_t CFileHandleList::FS_GetFreeHandle (fileHandleIndex_t **handle)
{
	THandleIndexListType::iterator it = CFile::IndexList->GetFreeHandle();

	*handle = &(*it).second;
	return (*it).first + 1;
}

fileHandleIndex_t *CFileHandleList::FS_GetHandle (fileHandle_t &fileNum)
{
	fileHandleIndex_t *hIndex;

	if (fileNum < 0 || fileNum > FS_MAX_FILE_INDICES)
		CFile::OutputError ("FS_GetHandle: invalid file number");

	hIndex = CFile::IndexList->GetHandle(fileNum-1);
	if (!hIndex->inUse)
		CFile::OutputError ("FS_GetHandle: invalid handle index");

	return hIndex;
}

void CFindFiles::FindFiles (CFindFilesCallback *Callback)
{
	if (Filter.empty())
		Filter = "*";
	if (Extension.empty())
		Extension = "*";

	// Search through the path, one element at a time
	for (fs_pathListType::iterator it = fs_pathList.begin(); it < fs_pathList.end(); ++it)
	{
		fs_pathIndex *search = (*it);

		// Directory tree
		char dir[MAX_PATHNAME];
		snprintf(dir, sizeof(dir), "%s/%s", search->pathName, Path.c_str());

		TFindFilesType dirFiles;

		if (!Extension.empty())
		{
			char ext[MAX_PATHNAME/4];
			snprintf(ext, sizeof(ext), "*.%s", Extension.c_str());
			Sys_FindFiles(dirFiles, dir, ext, Recurse, true, false);
		}
		else
			Sys_FindFiles(dirFiles, dir, "*", Recurse, true, true);

		for (size_t i = 0; i < dirFiles.size(); i++)
		{
			// Match filter
			if (Filter.c_str())
			{
				if (!Q_WildcardMatch(Filter.c_str(), dirFiles[i].c_str()+strlen(search->pathName)+1, 1))
					continue;
			}

			// Found something
			const char *name = dirFiles[i].c_str() + strlen(search->pathName) + 1;

			// Ignore duplicates
			bool bFound = false;
			for (size_t z = 0; z < Files.size(); z++)
			{
				if (!Q_stricmp(Files[z].c_str(), name))
					break;
			}

			if (!bFound)
				Files.push_back ((AddDir) ? dirFiles[i].c_str() : cc_string(name));
		}
	}

	if (Callback)
	{
		for (size_t i = 0; i < Files.size(); i++)
			Callback->Query (Files[i]);
	}
};

void FS_Init (sint32 maxHandles)
{
	CFile::IndexList = QNew(TAG_GENERIC) CFileHandleList (maxHandles);
	FS_AddPath (".");
	if (strcmp(GAMENAME, "baseq2"))
		FS_AddPath (GAMENAME);
	FS_AddPath ("baseq2");
}

