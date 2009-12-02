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
#include "zlib.h"

// Error management
void FS_Error (const char *errorMsg)
{
#ifdef _DEBUG
#ifdef WIN32
	OutputDebugStringA (errorMsg);
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

typedef std::vector<fs_pathIndex*, std::filesystem_allocator<fs_pathIndex*> > fs_pathListType;
fs_pathListType fs_pathList;

// Adds a path to the path list
void FS_AddPath (const char *pathName)
{
	fs_pathIndex *path = QNew (com_fileSysPool, 0) fs_pathIndex;
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

	fs_pathIndex *Path = QNew (com_fileSysPool, 0) fs_pathIndex;
	strncpy (Path->pathName, pathName, sizeof(Path->pathName));

	fs_pathList.insert (fs_pathList.begin(), Path);
}

// Handle management
// Handle index is used internally
static sint32 FS_MAX_FILEINDICES = 256;

class fileHandleIndex_t
{
public:
	fileHandleIndex_t()
	{
		Clear ();
	};

	void Clear ()
	{
		name.clear();
		inUse = false;
		openMode = FILEMODE_NONE;
		file.reg = NULL;
		file.gz = NULL;
	};

	std::cc_string			name;

	bool					inUse;
	EFileType				fileType;
	EFileOpMode				openMode;

	union
	{
		FILE					*reg;
		gzFile					gz;
	} file;
};

typedef std::map<fileHandle_t, fileHandleIndex_t, std::less<fileHandle_t>, std::filesystem_allocator <std::pair<fileHandle_t, fileHandleIndex_t> > > THandleIndexListType;
class CFileHandleList
{
	fileHandle_t numHandlesAllocated;

	// OpenList = allocated free keys
	// ClosedList = used keys
	THandleIndexListType OpenList, ClosedList;

	// Private members

	// Creates a new key and increases allocated handles.
	// Returns the new key.
	THandleIndexListType::iterator Create ()
	{
		fileHandle_t CreatedKey = numHandlesAllocated++;
		OpenList[CreatedKey] = fileHandleIndex_t();
		return OpenList.find(CreatedKey);
	};

	// Moves "it" from OpenList to ClosedList
	THandleIndexListType::iterator MoveToClosed (THandleIndexListType::iterator it)
	{
		fileHandle_t Key = (*it).first;
		ClosedList[Key] = (*it).second;
		OpenList.erase (it);
		return ClosedList.find(Key);
	};

	// Moves "it" from ClosedList to OpenList
	THandleIndexListType::iterator MoveToOpen (THandleIndexListType::iterator it)
	{
		(*it).second.Clear ();

		fileHandle_t Key = (*it).first;
		OpenList[Key] = (*it).second;
		ClosedList.erase (it);
		return OpenList.find(Key);
	};

public: // Interface

	// allocated = number of handles to create automatically.
	CFileHandleList (sint32 allocated = 0) :
		numHandlesAllocated (0)
	{
		for (fileHandle_t i = 0; i < allocated; i++)
			Create ();
	};

	// Returns either a free key in Open or
	// creates a new key and returns it.
	THandleIndexListType::iterator GetFreeHandle ()
	{
		if (!OpenList.empty())
			// Re-use a key
			return MoveToClosed(OpenList.begin());
		else
			// Create a new key and use that
			return MoveToClosed(Create ());
	};

	// Gets the fileHandleIndex_t of a handle in-use
	fileHandleIndex_t *GetHandle (fileHandle_t Key)
	{
		return &(*ClosedList.find(Key)).second;
	};

	// Pushes a key back into the Open list
	// Use this when you're done with a key
	void PushFreeHandle (fileHandle_t Key)
	{
		MoveToOpen (ClosedList.find(Key));
	};
};

CFileHandleList *IndexList;

static fileHandle_t FS_GetFreeHandle (fileHandleIndex_t **handle)
{
	THandleIndexListType::iterator it = IndexList->GetFreeHandle();

	*handle = &(*it).second;
	return (*it).first + 1;
}

static fileHandleIndex_t *FS_GetHandle (fileHandle_t &fileNum)
{
	fileHandleIndex_t *hIndex;

	if (fileNum < 0 || fileNum > FS_MAX_FILEINDICES)
		FS_Error ("FS_GetHandle: invalid file number");

	hIndex = IndexList->GetHandle(fileNum-1);
	if (!hIndex->inUse)
		FS_Error ("FS_GetHandle: invalid handle index");

	return hIndex;
}

void FS_Close (fileHandle_t &handle)
{
	fileHandleIndex_t *handleIndex = FS_GetHandle(handle);

	if (handleIndex)
	{
		if (IS_REGULAR(handleIndex))
			fclose(handleIndex->file.reg);
		else
			gzclose(handleIndex->file.gz);

		// Invalidate the handle
		IndexList->PushFreeHandle (handle - 1);
		handle = -1;
	}
}

// Always returns in same order:
// [r/a][w][b][+][c]
static const char *FS_OpenModeFromEnum (EFileOpMode Mode)
{
	static char mode[5];
	sint32 currentPos = 0;

	// Reset old mode
	mode[0] = mode[1] = mode[2] = mode[3] = mode[4] = 0;

	if (Mode & FILEMODE_APPEND)
	{
		/*if (!(Mode & FILEMODE_CREATE))
		{
			FS_Error ("Can't append and have the file exist (yet)\n");
			return "ERR";
		}*/

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

	if (Mode & FILEMODE_GZ)
	{
		if (Mode & FILEMODE_COMPRESS_NONE)
			mode[currentPos++] = '0';
		else if (Mode & FILEMODE_COMPRESS_LOW)
			mode[currentPos++] = '2';
		else if (Mode & FILEMODE_COMPRESS_MED)
			mode[currentPos++] = '5';
		else if (Mode & FILEMODE_COMPRESS_HIGH)
			mode[currentPos++] = '9';
		else
			mode[currentPos++] = '5';
	}

	return mode;
}

// Opens a file and returns the allocated handle
// 0 if file was not opened.
fileHandle_t FS_OpenFile (const char *fileName, EFileOpMode Mode)
{
	const char *openMode = FS_OpenModeFromEnum(Mode);

	if (!strcmp(openMode,"ERR"))
	{
		_CC_ASSERT_EXPR (0, "Invalid file mode passed");
		return 0;
	}

	// Open up the file.
	void *fp = NULL;

	// Search each of the search paths
	for (fs_pathListType::iterator i = fs_pathList.begin(); i < fs_pathList.end(); i++)
	{
		char newFileName[MAX_PATHNAME];
		fs_pathIndex *Index = (*i);

		char slashCheck = Index->pathName[strlen(Index->pathName)-1];
		if (slashCheck != '\\' && slashCheck != '/')
			snprintf (newFileName, sizeof(newFileName)-1, "%s/%s", Index->pathName, fileName);
		else
			snprintf (newFileName, sizeof(newFileName)-1, "%s%s", Index->pathName, fileName);
		newFileName[sizeof(newFileName)-1] = 0;

		// Try opening it
		if (Mode & FILEMODE_GZ)
			fp = gzopen(newFileName, openMode);
		else
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
	fileHandleIndex_t *handleIndex = NULL;
	fileHandle_t handle = FS_GetFreeHandle(&handleIndex);

	if (handleIndex)
	{
		handleIndex->inUse = true;
		handleIndex->name = fileName;
		handleIndex->openMode = Mode;

		if (Mode & FILEMODE_GZ)
		{
			handleIndex->fileType = FILE_GZ;
			handleIndex->file.gz = (gzFile)fp;
		}
		else
		{
			handleIndex->fileType = FILE_REGULAR;
			handleIndex->file.reg = (FILE*)fp;
		}
		return handle;
	}
	else
	{
		if (Mode & FILEMODE_GZ)
			gzclose ((gzFile)fp);
		else
			fclose ((FILE*)fp);
		return 0; // Couldn't allocate a handle
	}
}

// Reads "size" bytes from handle into "buffer"
void FS_Read (void *buffer, size_t size, fileHandle_t &handle)
{
	fileHandleIndex_t *handleIndex = FS_GetHandle(handle);

	if (!(handleIndex->openMode & FILEMODE_READ))
		_CC_ASSERT_EXPR (0, "Tried to read on a write\n");

	if (handleIndex)
	{
		if (handleIndex->fileType == FILE_REGULAR)
			fread (buffer, size, 1, handleIndex->file.reg);
		else
			gzread (handleIndex->file.gz, buffer, size);
	}
}

// Writes "size" bytes from handle from "buffer"
void FS_Write (const void *buffer, size_t size, fileHandle_t &handle)
{
	fileHandleIndex_t *handleIndex = FS_GetHandle(handle);

	if (!(handleIndex->openMode & FILEMODE_WRITE))
		_CC_ASSERT_EXPR (0, "Tried to write on a read\n");

	if (handleIndex)
	{
		if (handleIndex->fileType == FILE_REGULAR)
			fwrite (buffer, size, 1, handleIndex->file.reg);
		else
			gzwrite (handleIndex->file.gz, buffer, size);
	}
}

// Prints the format and arguments into the current position
void FS_Print (fileHandle_t &handle, char *fmt, ...)
{
	fileHandleIndex_t *handleIndex = FS_GetHandle(handle);
	va_list		argptr;
	static char		text[MAX_COMPRINT/2];

	va_start (argptr, fmt);
	vsnprintf (text, sizeof(text)-1, fmt, argptr);
	va_end (argptr);

	text[MAX_COMPRINT/2-1] = 0;

	if (handleIndex)
	{
		if (handleIndex->fileType == FILE_REGULAR)
			fwrite (text, strlen(text), 1, handleIndex->file.reg);
		else
			gzwrite (handleIndex->file.gz, text, strlen(text));
	}
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
	uint8 *buf = QNew (com_fileSysPool, 0) uint8[len + termLen];
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
	QDelete[] buffer;
}

filePos_t FS_Tell (fileHandle_t &handle)
{
	fileHandleIndex_t *handleIndex = FS_GetHandle (handle);

	if (handleIndex)
	{
		if (handleIndex->fileType == FILE_REGULAR)
			return ftell(handleIndex->file.reg);
		else
			return gztell(handleIndex->file.gz);
	}
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
	fileHandleIndex_t *handleIndex = FS_GetHandle (handle);

	if (handleIndex)
	{
		if (handleIndex->fileType == FILE_REGULAR)
			fseek (handleIndex->file.reg, seekOffset, seekOrigin);
		else
			gzseek (handleIndex->file.gz, seekOffset, seekOrigin);
	}
}


// Finds "maxFiles" files in "path" (optionally "recurse"ing) that match the filter "filter" and are of type "extention".
// If "addDir" is true, it returns the stripped names, and not the full name for opening.
// Filter can contain * for a wildcard.
TFindFilesType FS_FindFiles(const char *path, const char *filter, const char *extension, const bool addDir, const bool recurse)
{
	TFindFilesType files;

	// Search through the path, one element at a time
	for (fs_pathListType::iterator it = fs_pathList.begin(); it < fs_pathList.end(); it++)
	{
		fs_pathIndex *search = (*it);

		// Directory tree
		char dir[MAX_PATHNAME];
		snprintf(dir, sizeof(dir), "%s/%s", search->pathName, path);

		TFindFilesType dirFiles;

		if (extension)
		{
			char ext[MAX_PATHNAME/4];
			snprintf(ext, sizeof(ext), "*.%s", extension);
			Sys_FindFiles(dirFiles, dir, ext, 0, recurse, true, false);
		}
		else
			Sys_FindFiles(dirFiles, dir, "*", 0, recurse, true, true);

		for (size_t i = 0; i < dirFiles.size(); i++)
		{
			// Match filter
			if (filter)
			{
				if (!Q_WildcardMatch(filter, dirFiles[i].c_str()+strlen(search->pathName)+1, 1))
					continue;
			}

			// Found something
			const char *name = dirFiles[i].c_str() + strlen(search->pathName) + 1;

			// Ignore duplicates
			bool bFound = false;
			for (size_t z = 0; z < files.size(); z++)
			{
				if (!Q_stricmp(files[z].c_str(), name))
					break;
			}

			if (!bFound)
				files.push_back ((addDir) ? std::cc_string(search->pathName) + "/" + name : std::cc_string(name));
		}
	}

	return files;
}

void FS_Init (sint32 maxHandles)
{
	IndexList = QNew(com_fileSysPool, 0) CFileHandleList (maxHandles);
	FS_AddPath (".");
}
