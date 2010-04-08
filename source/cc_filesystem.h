#ifndef WIN32
#include <strings.h>
#include <ctype.h>
#endif

// Note to self: when naming variables, name them correctly.
typedef sint32 fileHandle_t;

CC_ENUM (uint32, EFileOpMode)
{
	FILEMODE_NONE			=	0, // Internally only

	FILEMODE_READ			=	1, // Open a file for reading
	FILEMODE_WRITE			=	2, // Open a file for writing (can be mixed with the reading)
	FILEMODE_APPEND			=	4, // Open a file for appending (cannot be mixed)

	FILEMODE_CREATE			=	8, // Will create the file if it doesn't exist
	FILEMODE_ASCII			=	16, // Open the file in ascii mode

	// GZ-related
	FILEMODE_GZ				=	32, // load with gz (de)compression
	FILEMODE_COMPRESS_NONE	=	64, // compression level 0
	FILEMODE_COMPRESS_LOW	=	128, // compression level 2
	FILEMODE_COMPRESS_MED	=	256, // compression level 5
	FILEMODE_COMPRESS_HIGH	=	512, // compression level 9
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
#define FS_MAX_FILE_INDICES 256

#ifdef WIN32
#if !defined(CC_STDC_CONFORMANCE)
#include <WinSock2.h>
#include <windows.h>
#endif
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

#include "zlib.h"

#ifdef COMPILING_LIB
#include "sys_portable.h"

// Not exported
void FS_Error (const char *errorMsg);
#endif

CC_ENUM (uint8, EFileType)
{
	FILE_REGULAR,		// Regular file
	FILE_GZ,			// gz-compressed file
};

#define IS_REGULAR(h)		((h->fileType) == FILE_REGULAR)
#define IS_GZ(h)			((h->fileType) == FILE_GZ)

// Path management.
// This may seem empty. It will be filled more once
// zip loading is in.
class fs_pathIndex
{
public:
	char		pathName[MAX_PATHNAME];

	fs_pathIndex ()
	{
		pathName[0] = 0;
	};
};
typedef std::vector<fs_pathIndex*> fs_pathListType;
extern fs_pathListType fs_pathList;

void FS_Init (sint32 maxHandles);

void FS_RemovePath (const char *pathName);
void FS_AddPath (const char *pathName);
void FS_ReorderPath (const char *pathName);

class fileHandleIndex_t
{
public:
	fileHandle_t			handleIndex;
	std::string				name;

	bool					inUse;
	EFileType				fileType;
	EFileOpMode				openMode;

	union
	{
		FILE					*reg;
		gzFile					gz;
	} file;

	fileHandleIndex_t() { }

	fileHandleIndex_t(fileHandle_t &handleIndex) :
	  handleIndex(handleIndex)
	{
		Clear ();
	};

	void Clear ()
	{
		name.clear();
		inUse = false;
		openMode = FILEMODE_NONE;
		file.reg = NULL;
	};
};
typedef std::map<fileHandle_t, fileHandleIndex_t> THandleIndexListType;

class CFileHandleList
{
	fileHandle_t numHandlesAllocated;

	// OpenList = allocated free keys
	// ClosedList = used keys
	THandleIndexListType OpenList, ClosedList;

	// Private members

	// Creates a new key and increases allocated handles.
	// Returns the new key.
	THandleIndexListType::iterator Create ();

	// Moves "it" from OpenList to ClosedList
	THandleIndexListType::iterator MoveToClosed (THandleIndexListType::iterator it);

	// Moves "it" from ClosedList to OpenList
	THandleIndexListType::iterator MoveToOpen (THandleIndexListType::iterator it);

public: // Interface

	// allocated = number of handles to create automatically.
	CFileHandleList (sint32 allocated = 0);

	// Returns either a free key in Open or
	// creates a new key and returns it.
	THandleIndexListType::iterator GetFreeHandle ();

	// Gets the fileHandleIndex_t of a handle in-use
	fileHandleIndex_t *GetHandle (fileHandle_t Key);

	// Pushes a key back into the Open list
	// Use this when you're done with a key
	void PushFreeHandle (fileHandle_t Key);

	// Grabs a free handle
	static fileHandle_t FS_GetFreeHandle (fileHandleIndex_t **handle);

	// Returns the handle pointer for handle "fileNum"
	static fileHandleIndex_t *FS_GetHandle (fileHandle_t &fileNum);
};

// A wrapper class for reading/writing to files.
class CFile
{
	friend class CFileHandleList;
	friend void FS_Init (sint32 maxHandles);
	static class CFileHandleList *IndexList;

	fileHandleIndex_t	*Handle;

	// Error management
	static void OutputError (const char *errorMsg)
	{
#ifdef _DEBUG
#ifdef WIN32
#if !defined(CC_STDC_CONFORMANCE)
		OutputDebugStringA (errorMsg);
#endif
		assert (0);
#endif
#endif
		ServerPrintf ("%s\n", errorMsg);
	}

	// Always returns in same order:
	// [r/a][w][b][+][c]
	static const char *OpenModeFromEnum (EFileOpMode Mode)
	{
		static char mode[5];
		sint32 currentPos = 0;

		// Reset old mode
		mode[0] = mode[1] = mode[2] = mode[3] = mode[4] = 0;

		if (Mode & FILEMODE_APPEND)
		{
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
				CFile::OutputError ("Write and Append mixed\n");
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

public:
	CFile (const char *fileName, EFileOpMode Mode) :
	  Handle (0)
	{
		Open (fileName, Mode);
	};

	virtual ~CFile ()
	{
		if (!Handle)
			return;

		Close ();
	};

	// Opens a file.
	void Open (const char *fileName, EFileOpMode Mode)
	{
		const char *openMode = CFile::OpenModeFromEnum(Mode);

		if (_CC_ASSERT_EXPR (strcmp(openMode,"ERR"), "Invalid file mode passed"))
			return;

		// Open up the file.
		void *fp = NULL;

		// Search each of the search paths
		for (fs_pathListType::iterator it = fs_pathList.begin(); it < fs_pathList.end(); ++it)
		{
			char newFileName[MAX_PATHNAME];
			fs_pathIndex *Index = (*it);

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

		// Absolute path?
		if (Mode & FILEMODE_GZ)
			fp = gzopen(fileName, openMode);
		else
			fp = fopen(fileName, openMode);

		// Did we open?
		if (!fp)
			return; // Nope

		// Yep
		// Allocate a free handle
		CFileHandleList::FS_GetFreeHandle(&Handle);

		if (Handle)
		{
			Handle->inUse = true;
			Handle->name = fileName;
			Handle->openMode = Mode;

			if (Mode & FILEMODE_GZ)
				Handle->fileType = FILE_GZ;
			else
				Handle->fileType = FILE_REGULAR;

			Handle->file.reg = (FILE*)fp;
		}
		else
		{
			if (Mode & FILEMODE_GZ)
				gzclose ((gzFile)fp);
			else
				fclose ((FILE*)fp);
		}
	};

	void Close ()
	{
		if (!Handle)
			return;

		if (IS_REGULAR(Handle))
			fclose(Handle->file.reg);
		else
			gzclose(Handle->file.gz);

		// Invalidate the handle
		IndexList->PushFreeHandle (Handle->handleIndex);
		Handle = NULL;
	};

	bool EndOfFile ()
	{
		if (!Handle)
			return true;

		if (IS_REGULAR(Handle))
			return !!feof(Handle->file.reg);
		return !!gzeof(Handle->file.gz);
	};

	static bool Exists (const char *fileName)
	{
		CFile File (fileName, FILEMODE_READ);

		if (!File.Valid())
			return false;
		return true;
	};

	void Write (const void *buffer, size_t size)
	{
		if (!Handle)
			return;

		_CC_ASSERT_EXPR ((Handle->openMode & FILEMODE_WRITE), "Tried to write on a read\n");

		if (IS_REGULAR(Handle))
			fwrite (buffer, size, 1, Handle->file.reg);
		else
			gzwrite (Handle->file.gz, buffer, size);
	};

	template <typename TType>
	void Write (const void *buffer)
	{
		Write (buffer, sizeof(TType));
	};

	template <typename TType>
	void Write (const TType &Ref)
	{
		Write (&Ref, sizeof(TType));
	};

	void Write (const std::string &Ref)
	{
		sint32 Length = (Ref.empty()) ? -1 : Ref.length() + 1;

		Write (&Length, sizeof(Length));

		if (Length > 1)
			Write (Ref.c_str(), Length);
	};

	void WriteString (const char *Str)
	{
		sint32 Length = (Str) ? strlen(Str) + 1 : -1;

		Write (&Length, sizeof(Length));

		if (Length > 1)
			Write (Str, Length);
	};

	template <typename TType>
	void WriteArray (TType *Array, size_t Length)
	{
		Write (Array, sizeof(TType) * Length);
	};

	void Read (void *buffer, size_t size)
	{
		if (!Handle)
			return;

		_CC_ASSERT_EXPR ((Handle->openMode & FILEMODE_READ), "Tried to read on a write\n");

		if (IS_REGULAR(Handle))
			fread (buffer, size, 1, Handle->file.reg);
		else
			gzread (Handle->file.gz, buffer, size);
	};

	template <typename TType>
	void Read (void *buffer)
	{
		Read (buffer, sizeof(TType));
	};

	template <typename TType>
	TType Read ()
	{
		TType Val;
		Mem_Zero (&Val, sizeof(TType));

		Read (&Val, sizeof(TType));
		return Val;
	};

	char *ReadString (sint32 Tag = TAG_GENERIC)
	{
		if (!Handle)
			return NULL;

		sint32 Length;
		Read (&Length, sizeof(Length));
		
		char *tempBuffer = NULL;
		if (Length > 1)
		{
			tempBuffer = QNew (Tag) char[Length];
			Read (tempBuffer, Length);
		}
		return tempBuffer;
	};

	std::string ReadCCString ()
	{
		if (!Handle)
			return "";

		char *stringBuffer = ReadString();

		if (!stringBuffer)
			return "";

		std::string str (stringBuffer);
		QDelete[] stringBuffer;
		return str;
	};

	std::string ReadLine ()
	{
		if (!Handle)
			return "";

		std::string tempStr;

		while (true)
		{
			char tempChar = Read<char> ();

			if (tempChar == '\n' || tempChar == '\0')
				break;

			tempStr += tempChar;
		};

		return tempStr;
	};

	void ReadLine (char *buf, size_t maxSize)
	{
		if (!Handle)
			return;

		std::string line = ReadLine ();

		Q_snprintfz (buf, maxSize-1, "%s", line.c_str());
		buf[maxSize-1] = 0;
	};

	template <typename TType>
	void ReadArray (TType *Array, size_t Length)
	{
		if (Array == NULL)
		{
			TType *TArray = QNew (TAG_GENERIC) TType[Length];
			Read (TArray, sizeof(TType) * Length);
			QDelete TArray;
			return;
		}

		Read (Array, sizeof(TType) * Length);
	};

	void Seek (const ESeekOrigin seekOrigin, const size_t seekOffset)
	{
		if (!Handle)
			return;

		if (IS_REGULAR(Handle))
			fseek (Handle->file.reg, seekOffset, seekOrigin);
		else
			gzseek (Handle->file.gz, seekOffset, seekOrigin);
	};

	// There's like three of these Print wrappers nesting..
	// Somewhere someone must die.
	void Print (const char *fmt, ...)
	{
		if (!Handle)
			return;

		va_list		argptr;
		static char		text[SHRT_MAX];

		va_start (argptr, fmt);
		vsnprintf (text, SHRT_MAX-1, fmt, argptr);
		va_end (argptr);

		text[SHRT_MAX/2-1] = 0;
		Write (text, strlen(text));
	};

	inline size_t Length ()
	{
		if (!Handle)
			return 0;

		filePos_t currentPos = Tell();

		Seek (SEEKORIGIN_END, 0);
		size_t len = Tell();
		Seek (SEEKORIGIN_SET, currentPos);

		return len;
	};

	inline filePos_t Tell ()
	{
		if (!Handle)
			return -1;

		if (IS_REGULAR(Handle))
			return ftell(Handle->file.reg);
		else
			return gztell(Handle->file.gz);
	};

	inline bool Valid ()
	{
		return (Handle != NULL);
	};

	// Stream-like functionality
	CFile &operator<< (bool &val)
	{
		Write<bool> (val);
		return *this;
	};

	CFile &operator<< (sint16 &val)
	{
		Write<sint16> (val);
		return *this;
	};

	CFile &operator<< (uint16 &val)
	{
		Write<uint16> (val);
		return *this;
	};

	CFile &operator<< (sint32 &val)
	{
		Write<sint32> (val);
		return *this;
	};

	CFile &operator<< (uint32 &val)
	{
		Write<uint32> (val);
		return *this;
	};

	CFile &operator<< (long &val)
	{
		Write<long> (val);
		return *this;
	};

	CFile &operator<< (unsigned long &val)
	{
		Write<unsigned long> (val);
		return *this;
	};

	CFile &operator<< (float &val)
	{
		Write<float> (val);
		return *this;
	};

	CFile &operator<< (double &val)
	{
		Write<double> (val);
		return *this;
	};

	CFile &operator<< (long double &val)
	{
		Write<long double> (val);
		return *this;
	};

	CFile &operator<< (const void *val)
	{
		Write<void*> (val);
		return *this;
	};

	CFile &operator>> (bool &val)
	{
		val = Read<bool> ();
		return *this;
	};

	CFile &operator>> (sint16 &val)
	{
		val = Read<sint16> ();
		return *this;
	};

	CFile &operator>> (uint16 &val)
	{
		val = Read<uint16> ();
		return *this;
	};

	CFile &operator>> (sint32 &val)
	{
		val = Read<sint32> ();
		return *this;
	};

	CFile &operator>> (uint32 &val)
	{
		val = Read<uint32> ();
		return *this;
	};

	CFile &operator>> (long &val)
	{
		val = Read<long> ();
		return *this;
	};

	CFile &operator>> (unsigned long &val)
	{
		val = Read<unsigned long> ();
		return *this;
	};

	CFile &operator>> (float &val)
	{
		val = Read<float> ();
		return *this;
	};

	CFile &operator>> (double &val)
	{
		val = Read<double> ();
		return *this;
	};

	CFile &operator>> (long double &val)
	{
		val = Read<long double> ();
		return *this;
	};

	CFile &operator>> (void *&val)
	{
		val = Read<void*> ();
		return *this;
	};
};

inline CFile &operator<< (CFile &Stream, std::string &val)
{
	Stream.Write (val);
	return Stream;
};

inline CFile &operator>> (CFile &Stream, std::string &val)
{
	val = Stream.Read<std::string> ();
	return Stream;
};

// A wrapper for FS_LoadFile
class CFileBuffer
{
	uint8 *Buffer;
	size_t BufSize;

public:
	CFileBuffer (const char *FileName, bool Terminate) :
	  Buffer(NULL),
	  BufSize(0)
	{
		Open (FileName, Terminate);
	};

	~CFileBuffer ()
	{
		Close ();
	};

	void Open (const char *FileName, bool Terminate)
	{
		CFile File (FileName, FILEMODE_READ);

		if (!File.Valid())
			return;

		size_t len = File.Length();

		size_t termLen = (Terminate) ? 2 : 0;
		Buffer = QNew (TAG_GENERIC) uint8[len + termLen];

		File.Read (Buffer, len);

		if (Terminate)
			strncpy((char *)Buffer+len, "\n\0", termLen);

		BufSize = len + termLen;
	};

	void Close ()
	{
		QDelete[] Buffer;
	};

	template <typename type>
	inline type *GetBuffer ()
	{
		return (type*)Buffer;
	};

	inline size_t GetLength ()
	{
		return BufSize;
	};

	inline bool Valid ()
	{
		return (Buffer != NULL);
	};
};

typedef std::vector<std::string> TFindFilesType;

// A wrapper for FS_FindFiles
class CFindFilesCallback
{
public:
	virtual void Query (std::string &fileName) {};
};

// Finds files in "Path" (optionally "Recurse"ing) that match the filter "Filter" and are of type "Extention".
// If "AddDir" is true, it returns the stripped names, and not the full name for opening.
// Filter and Extension can contain * for wildcards.
#define MAX_FINDFILES_PATH	256
class CFindFiles
{
public:
	std::string		Path;
	std::string		Filter;
	std::string		Extension;
	bool			AddDir;
	bool			Recurse;
	TFindFilesType	Files;

	CFindFiles () :
		Path(),
		Filter(),
		Extension(),
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

	void FindFiles (CFindFilesCallback *Callback);
};
