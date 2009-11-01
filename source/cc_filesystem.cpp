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

// Error management
void FS_Error (const char *errorMsg)
{
#ifdef _DEBUG
#ifdef WIN32
	OutputDebugString (errorMsg);
#endif
	assert (0);
#endif
}

// Path management.
// This may seem empty. It will be filled more once
// zip loading is in.
class fs_pathIndex
{
public:
	char		pathName[MAX_PATHNAME];
};

typedef std::vector<fs_pathIndex*> fs_pathListType;
fs_pathListType fs_pathList;

// Adds a path to the path list
void FS_AddPath (const char *pathName)
{
	fs_pathIndex *path = new fs_pathIndex;
	strncpy (path->pathName, pathName, sizeof(path->pathName));
	fs_pathList.push_back (path);
}

// Removes a path from the list
void FS_RemovePath (const char *pathName)
{
	for (fs_pathListType::iterator i = fs_pathList.begin(); i < fs_pathList.end(); i++)
	{
		if (strcmp(pathName, (*i)->pathName) == 0)
		{
			delete (*i);
			fs_pathList.erase (i);
			return;
		}
	}
}

// Re-orders this path to be at the first position.
// If it doesn't exist, it adds it.
void FS_ReorderPath (const char *pathName)
{
	for (fs_pathListType::iterator i = fs_pathList.begin(); i < fs_pathList.end(); i++)
	{
		if (strcmp(pathName, (*i)->pathName) == 0)
		{
			fs_pathIndex *Path = (*i);

			fs_pathList.erase (i);
			fs_pathList.insert (fs_pathList.begin(), Path);
			return;
		}
	}

	fs_pathIndex *Path = new fs_pathIndex;
	strncpy (Path->pathName, pathName, sizeof(Path->pathName));

	fs_pathList.insert (fs_pathList.begin(), Path);
}

// Handle management
// Handle index is used internally
static int FS_MAX_FILEINDICES = 256;

class fsHandleIndex
{
public:
	fsHandleIndex() :
	  inUse(false),
	  openMode(FILEMODE_NONE),
	  regFile(NULL)
	  {
	  };

	char					name[MAX_PATHNAME];

	bool					inUse;
	EFileOpMode				openMode;

	FILE					*regFile;
};

static fsHandleIndex *fs_fileIndices;

static void FS_InitHandles ()
{
	fs_fileIndices = new fsHandleIndex[FS_MAX_FILEINDICES];
}

static fileHandle_t FS_GetFreeHandle (fsHandleIndex **handle)
{
	fileHandle_t		i;
	fsHandleIndex		*hIndex;

	for (i=0, hIndex=fs_fileIndices ; i<FS_MAX_FILEINDICES ; hIndex++, i++)
	{
		if (hIndex->inUse)
			continue;

		hIndex->inUse = true;
		if (handle)
			*handle = hIndex;
		return i+1;
	}

	FS_Error ("no free handles!\n");
	return 0;
}

static fsHandleIndex *FS_GetHandle (fileHandle_t &fileNum)
{
	fsHandleIndex *hIndex;

	if (fileNum < 0 || fileNum > FS_MAX_FILEINDICES)
		FS_Error ("FS_GetHandle: invalid file number");

	hIndex = &fs_fileIndices[fileNum-1];
	if (!hIndex->inUse)
		FS_Error ("FS_GetHandle: invalid handle index");

	return hIndex;
}

// Always returns in same order:
// [r/a][w][b][+]
static const char *FS_OpenModeFromEnum (EFileOpMode Mode)
{
	static char mode[4];
	int currentPos = 0;

	// Reset old mode
	mode[0] = mode[1] = mode[2] = mode[3] = 0;

	if (Mode & FILEMODE_APPEND)
	{
		if (!(Mode & FILEMODE_CREATE))
		{
			FS_Error ("Can't append and have the file exist (yet)\n");
			return "ERR";
		}

		// Shove the a
		mode[currentPos++] = 'a';

		// Shove the + if we want to write
		if (Mode & FILEMODE_WRITE)
			mode[currentPos++] = '+';

		// Are we binary?
		if (!(Mode & FILEMODE_ASCII))
			mode[currentPos++] = 'b';
	}
	else if (Mode & FILEMODE_READ)
	{
		// Calculate the characters
		// We wanted reading, so push that in there.
		if (Mode & FILEMODE_CREATE)
			mode[currentPos++] = 'w';
		else
			mode[currentPos++] = 'r';

		// Did we want writing?
		// If we must exist, we need the +
		// to get r+ (reading + writing and must exist)
		if (Mode & FILEMODE_WRITE)
			mode[currentPos++] = '+';

		// Are we binary?
		if (!(Mode & FILEMODE_ASCII))
			mode[currentPos++] = 'b';
	}
	// If we got here, we only wanted
	// to open a file for writing.
	else if (Mode & FILEMODE_WRITE)
	{
		// Error checking
		if (Mode & FILEMODE_APPEND)
		{
			FS_Error ("Write and Append mixed\n");
			return "ERR";
		}

		if (Mode & FILEMODE_CREATE)
			mode[currentPos++] = 'w';
		else
		{
			mode[currentPos++] = 'r';
			mode[currentPos++] = '+';
		}

		// Are we binary?
		if (!(Mode & FILEMODE_ASCII))
			mode[currentPos++] = 'b';
	}
	return mode;
}

// Opens a file and returns the allocated handle
// 0 if file was not opened.
// Optionally, you can pass a size as third argument
// to store the length of the file automatically.
fileHandle_t FS_OpenFile (const char *fileName, EFileOpMode Mode)
{
	const char *openMode = FS_OpenModeFromEnum(Mode);

	if (!strcmp(openMode,"ERR"))
		return 0;

	// Open up the file.
	FILE *fp;

	// Search each of the search paths
	for (fs_pathListType::iterator i = fs_pathList.begin(); i < fs_pathList.end(); i++)
	{
		char newFileName[MAX_PATHNAME];
		fs_pathIndex *Index = (*i);

		char slashCheck = Index->pathName[strlen(Index->pathName)-1];
		if (slashCheck != '\\' && slashCheck != '/')
			snprintf (newFileName, sizeof(newFileName), "%s/%s", Index->pathName, fileName);
		else
			snprintf (newFileName, sizeof(newFileName), "%s%s", Index->pathName, fileName);

		// Try opening it
		fp = fopen(newFileName, openMode);

		if (fp != NULL)
			break; // We got it
	}

	// Did we open?
	if (!fp)
		return 0; // Nope

	// Yep
	// Allocate a free handle and
	// return it.
	fsHandleIndex *handleIndex = NULL;
	fileHandle_t handle = FS_GetFreeHandle(&handleIndex);

	if (handleIndex)
	{
		handleIndex->inUse = true;
		strncpy (handleIndex->name, fileName, sizeof(handleIndex->name));
		handleIndex->openMode = Mode;
		handleIndex->regFile = fp;
		return handle;
	}
	else
	{
		fclose (fp);
		return 0; // Couldn't allocate a handle
	}
}

// Reads "size" bytes from handle into "buffer"
void FS_Read (void *buffer, size_t size, fileHandle_t &handle)
{
	fsHandleIndex *handleIndex = FS_GetHandle(handle);

	if (handleIndex)
		fread (buffer, size, 1, handleIndex->regFile);
}

// Writes "size" bytes from handle from "buffer"
void FS_Write (void *buffer, size_t size, fileHandle_t &handle)
{
	fsHandleIndex *handleIndex = FS_GetHandle(handle);

	if (handleIndex)
		fwrite (buffer, size, 1, handleIndex->regFile);
}

// Prints the format and arguments into the current position
void FS_Print (fileHandle_t &handle, char *fmt, ...)
{
	fsHandleIndex *handleIndex = FS_GetHandle(handle);
	va_list		argptr;
	char		text[2048];

	va_start (argptr, fmt);
	vsnprintf (text, sizeof(text), fmt, argptr);
	va_end (argptr);

	if (handleIndex)
		fwrite (text, strlen(text), 1, handleIndex->regFile);
}

// Opens a file, reads the entire contents,
// fills a buffer pointed by buf (if not NULL) and
// returns the length.
// Use "terminate" if you want to terminate it with an \0
size_t FS_LoadFile (const char *fileName, void **buffer, const bool terminate)
{
	fileHandle_t handle = FS_OpenFile(fileName, FILEMODE_READ);

	if (!handle)
		return 0;

	size_t len = FS_Len (handle);

	size_t termLen = (terminate) ? 2 : 0;
	byte *buf = new byte[len + termLen];
	*buffer = buf;

	FS_Read(buf, len, handle);

	if (terminate)
		strncpy((char *)buf+len, "\n\0", termLen);

	FS_Close (handle);
	return len + termLen;
}

// Returns true if the file exists
bool FS_FileExists (const char *fileName)
{
	fileHandle_t handle = FS_OpenFile(fileName, FILEMODE_READ);

	if (!handle)
		return false;
	FS_Close (handle);
	return true;
}

void FS_FreeFile (void *buffer)
{
	delete buffer;
}

void FS_Close (fileHandle_t &handle)
{
	fsHandleIndex *handleIndex = FS_GetHandle(handle);

	if (handleIndex)
	{
		fclose(handleIndex->regFile);
		handleIndex->regFile = NULL;
		handleIndex->inUse = false;
		// Invalidate the handle
		handle = -1;
	}
}

filePos_t FS_Tell (fileHandle_t &handle)
{
	fsHandleIndex *handleIndex = FS_GetHandle (handle);

	if (handleIndex)
		return ftell(handleIndex->regFile);
	return -1;
}

// Returns the length of the file in bytes.
// -1 if error occurs.
size_t FS_Len (fileHandle_t &handle)
{
	filePos_t currentPos = FS_Tell(handle);

	FS_Seek (handle, SEEKORIGIN_END, 0);
	size_t len = FS_Tell(handle);
	FS_Seek (handle, SEEKORIGIN_SET, currentPos);

	return len;
}

void FS_Seek (fileHandle_t &handle, const ESeekOrigin seekOrigin, const filePos_t seekOffset)
{
	fsHandleIndex *handleIndex = FS_GetHandle (handle);

	if (handleIndex)
		fseek (handleIndex->regFile, seekOffset, seekOrigin);
}


// Finds "maxFiles" files in "path" (optionally "recurse"ing) that match the filter "filter" and are of type "extention".
// If "addDir" is true, it returns the stripped names, and not the full name for opening.
// Filter can contain * for a wildcard.
// fileList is filled with the found files.
int FS_FindFiles(const char *path, const char *filter, const char *extension, char **fileList, int maxFiles, const bool addDir, const bool recurse)
{
	// Sanity check
	if (maxFiles > FS_MAX_FINDFILES)
	{
		FS_Error("maxFiles > FS_MAX_FINDFILES");
		maxFiles = FS_MAX_FINDFILES;
	}

	// Search through the path, one element at a time
	int fileCount = 0;
	for (fs_pathListType::iterator i = fs_pathList.begin(); i < fs_pathList.end(); i++)
	{
		fs_pathIndex *search = (*i);

		// Directory tree
		char dir[MAX_PATHNAME];
		snprintf(dir, sizeof(dir), "%s/%s", search->pathName, path);

		char *dirFiles[FS_MAX_FINDFILES];
		int dirCount;
		if (extension)
		{
			char ext[MAX_PATHNAME/4];
			snprintf(ext, sizeof(ext), "*.%s", extension);
			dirCount = Sys_FindFiles(dir, ext, dirFiles, FS_MAX_FINDFILES, 0, recurse, true, false);
		}
		else
		{
			dirCount = Sys_FindFiles(dir, "*", dirFiles, FS_MAX_FINDFILES, 0, recurse, true, true);
		}

		for (int fileNum=0 ; fileNum<dirCount ; fileNum++)
		{
			// Match filter
			if (filter)
			{
				if (!Q_WildcardMatch(filter, dirFiles[fileNum]+strlen(search->pathName)+1, 1))
				{
					delete dirFiles[fileNum];
					continue;
				}
			}

			// Found something
			char *name = dirFiles[fileNum] + strlen(search->pathName) + 1;
			if (fileCount < maxFiles)
			{
				// Ignore duplicates
				bool bFound = false;
				for (int i=0 ; i<fileCount ; i++)
				{
					if (!stricmp(fileList[i], name))
						break;
				}

				if (!bFound)
				{
					if (addDir)
					{
						char tempFile[MAX_PATHNAME];
						snprintf (tempFile, sizeof(tempFile), "%s/%s", search->pathName, name);
						fileList[fileCount] = new char[strlen(tempFile)+1];
						snprintf (fileList[fileCount], strlen(tempFile)+1, "%s", tempFile);
						fileCount++;
					}
					else
					{
						fileList[fileCount] = new char[strlen(name)+1];
						snprintf (fileList[fileCount], strlen(name)+1, "%s", name);
						fileCount++;
					}
				}
			}

			delete dirFiles[fileNum];
		}
	}

	return fileCount;
}

// Frees the file list made by the above function
void FS_FreeFileList (char **fileList, int numFiles)
{
	for (int i=0 ; i<numFiles ; i++)
	{
		if (!fileList[i])
			continue;

		delete fileList[i];
		fileList[i] = NULL;
	}
}

void FS_Init (int maxHandles)
{
	FS_MAX_FILEINDICES = maxHandles;
	FS_InitHandles ();
	FS_AddPath (".");
}
