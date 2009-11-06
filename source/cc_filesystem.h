#include <stdio.h>
#include <stdarg.h>
#include <stdlib.h>
#include <malloc.h>
#include <memory.h>
#include <string.h>
#include <assert.h>
#include <vector>

#ifndef WIN32
#include <strings.h>
#include <ctype.h>
#endif

// Note to self: when naming variables, name them correctly.
typedef int fileHandle_t;

CC_ENUM (uint8, EFileOpMode)
{
	FILEMODE_NONE		=	0, // Internally only

	FILEMODE_READ		=	1, // Open a file for reading
	FILEMODE_WRITE		=	2, // Open a file for writing (can be mixed with the reading)
	FILEMODE_APPEND		=	4, // Open a file for appending (cannot be mixed)

	FILEMODE_CREATE		=	8, // Will create the file if it doesn't exist
	FILEMODE_ASCII		=	16, // Open the file in ascii mode
};

CC_ENUM (uint8, ESeekOrigin)
{
	SEEKORIGIN_CUR = SEEK_CUR,
	SEEKORIGIN_SET = SEEK_SET,
	SEEKORIGIN_END = SEEK_END
};

typedef long int filePos_t;

// directory searching
#define SFF_ARCH	0x01
#define SFF_HIDDEN	0x02
#define SFF_RDONLY	0x04
#define SFF_SUBDIR	0x08
#define SFF_SYSTEM	0x10

#define FS_MAX_FINDFILES 512
#define MAX_PATHNAME 128

#ifdef WIN32
#include <windows.h>
#ifdef COMPILING_LIB
#include "sys_win32.h"
#endif
#else
#ifdef COMPILING_LIB
#include "sys_unix.h"
#define stricmp strcasecmp
#endif
#define EXPORT_FUNCTION
#endif

#ifdef COMPILING_LIB
#include "sys_portable.h"

// Not exported
void FS_Error (const char *errorMsg);
#endif

// Exported
void FS_Init (int maxHandles);

bool FS_FileExists (const char *fileName);
fileHandle_t FS_OpenFile (const char *fileName, EFileOpMode Mode);
void FS_Close (fileHandle_t &handle);

size_t FS_LoadFile (const char *fileName, void **buffer, const bool terminate);
void FS_FreeFile (void *buffer);

typedef std::vector<std::cc_string, std::level_allocator<std::cc_string> > TFindFilesType;
TFindFilesType FS_FindFiles(const char *path, const char *filter, const char *extension, const bool addDir, const bool recurse);
void FS_FreeFileList (char **fileList, int numFiles);

void FS_Write (void *buffer, size_t size, fileHandle_t &handle);
void FS_Read (void *buffer, size_t size, fileHandle_t &handle);
void FS_Seek (fileHandle_t &handle, const ESeekOrigin seekOrigin, const filePos_t seekOffset);
void FS_Print (fileHandle_t &handle, char *fmt, ...);

size_t FS_Len (fileHandle_t &handle);
filePos_t FS_Tell (fileHandle_t &handle);

void FS_RemovePath (const char *pathName);
void FS_AddPath (const char *pathName);
void FS_ReorderPath (const char *pathName);

#pragma warning (push)
#pragma warning (disable : 4996)
// A wrapper class for reading/writing to files.
class CFile
{
	fileHandle_t	Handle;
public:
	CFile (const char *fileName, EFileOpMode Mode) :
	  Handle (0)
	{
		Handle = FS_OpenFile (fileName, Mode);
	};

	virtual ~CFile ()
	{
		if (!Handle)
			return;

		FS_Close (Handle);
	};

	static bool Exists (const char *fileName)
	{
		return FS_FileExists (fileName);
	};

	void Write (void *buffer, size_t size)
	{
		if (!Handle)
			return;

		FS_Write (buffer, size, Handle);
	};

	void Read (void *buffer, size_t size)
	{
		if (!Handle)
			return;

		FS_Read (buffer, size, Handle);
	};

	void Seek (const ESeekOrigin seekOrigin, const size_t seekOffset)
	{
		if (!Handle)
			return;

		FS_Seek (Handle, seekOrigin, seekOffset);
	};

	// There's like three of these Print wrappers nesting..
	// Somewhere someone must die.
	void Print (char *fmt, ...)
	{
		if (!Handle)
			return;

		va_list		argptr;
		char		text[2048];

		va_start (argptr, fmt);
		vsnprintf (text, sizeof(text), fmt, argptr);
		va_end (argptr);
		
		Write (text, strlen(text));
	};

	inline size_t Length ()
	{
		if (!Handle)
			return 0;

		return FS_Len (Handle);
	};

	inline filePos_t Tell ()
	{
		if (!Handle)
			return 0;

		return FS_Tell (Handle);
	};

	inline bool Valid ()
	{
		return (Handle != 0);
	};
};
#pragma warning (pop)

// A wrapper for FS_LoadFile
class CFileBuffer
{
	void *buffer;
	size_t bufSize;

public:
	CFileBuffer (const char *fileName, bool Terminate) :
	  buffer(NULL),
	  bufSize(0)
	{
		bufSize = FS_LoadFile (fileName, &buffer, Terminate);
	};

	~CFileBuffer ()
	{
		FS_FreeFile (buffer);
	};

	template <typename type>
	inline type *GetBuffer ()
	{
		return (type*)buffer;
	};

	inline size_t GetLength ()
	{
		return bufSize;
	};

	inline bool Valid ()
	{
		return (buffer != NULL);
	};
};

// A wrapper for FS_FindFiles
class CFindFilesCallback
{
public:
	virtual void Query (std::cc_string &fileName) {};
};

//(const char *path, const char *filter, const char *extension, char **fileList, int maxFiles, const bool addDir, const bool recurse);
#define MAX_FINDFILES_PATH	256
class CFindFiles
{
public:
	char	*Path;
	char	*Filter;
	char	*Extension;
	bool	AddDir;
	bool	Recurse;

	TFindFilesType	Files;

	CFindFiles () :
		Path(NULL),
		Filter(NULL),
		Extension(NULL),
		AddDir(false),
		Recurse(false),
		Files()
		{
		};

	CFindFiles (char *Path, char *Filter, char *Extension, bool AddDir = true, bool Recurse = false) :
		Path(Path),
		Filter(Filter),
		Extension(Extension),
		AddDir(AddDir),
		Recurse(Recurse),
		Files()
		{
			FindFiles (NULL);
		};

	CFindFiles (CFindFilesCallback *CallBack, char *Path, char *Filter, char *Extension, bool AddDir = true, bool Recurse = false) :
		Path(Path),
		Filter(Filter),
		Extension(Extension),
		AddDir(AddDir),
		Recurse(Recurse),
		Files()
		{
			FindFiles (CallBack);
		};

	CFindFiles (CFindFilesCallback &CallBack, char *Path, char *Filter, char *Extension, bool AddDir = true, bool Recurse = false) :
		Path(Path),
		Filter(Filter),
		Extension(Extension),
		AddDir(AddDir),
		Recurse(Recurse),
		Files()
		{
			FindFiles (&CallBack);
		};

	~CFindFiles ()
	{
	};

	void FindFiles (CFindFilesCallback *Callback)
	{
		if (!Path)
			Path = "";
		if (!Filter)
			Filter = "*";
		if (!Extension)
			Extension = "*";

		Files = FS_FindFiles (Path, Filter, Extension, AddDir, Recurse);

		if (Callback)
		{
			for (size_t i = 0; i < Files.size(); i++)
				Callback->Query (Files[i]);
		}
	};
};