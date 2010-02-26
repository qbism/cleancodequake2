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

#ifdef WIN32
#if !defined(CC_STDC_CONFORMANCE)
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

// Exported
void FS_Init (sint32 maxHandles);

bool FS_FileExists (const char *fileName);
fileHandle_t FS_OpenFile (const char *fileName, EFileOpMode Mode);
void FS_Close (fileHandle_t &handle);

size_t FS_LoadFile (const char *fileName, void **buffer, const bool terminate);
void FS_FreeFile (void *buffer);

typedef std::vector<cc_string, std::allocator<cc_string> > TFindFilesType;
TFindFilesType FS_FindFiles(const char *path, const char *filter, const char *extension, const bool addDir, const bool recurse);

void FS_Write (const void *buffer, size_t size, fileHandle_t &handle);
void FS_Read (void *buffer, size_t size, fileHandle_t &handle);
void FS_Seek (fileHandle_t &handle, const ESeekOrigin seekOrigin, const filePos_t seekOffset);
void FS_Print (fileHandle_t &handle, char *fmt, ...);

size_t FS_Len (fileHandle_t &handle);
filePos_t FS_Tell (fileHandle_t &handle);
bool FS_EndOfFile (fileHandle_t &handle);

void FS_RemovePath (const char *pathName);
void FS_AddPath (const char *pathName);
void FS_ReorderPath (const char *pathName);

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

	bool EndOfFile ()
	{
		return FS_EndOfFile (Handle);
	};

	static bool Exists (const char *fileName)
	{
		return FS_FileExists (fileName);
	};

	void Write (const void *buffer, size_t size)
	{
		if (!Handle)
			return;

		FS_Write (buffer, size, Handle);
	};

	template <typename TType>
	void Write (const void *buffer)
	{
		if (!Handle)
			return;

		FS_Write (buffer, sizeof(TType), Handle);
	};

	template <typename TType>
	void Write (const TType &Ref)
	{
		if (!Handle)
			return;

		FS_Write (&Ref, sizeof(TType), Handle);
	};

	void Write (const cc_string &Ref)
	{
		if (!Handle)
			return;

		sint32 Length = (Ref.empty()) ? -1 : Ref.length() + 1;

		FS_Write (&Length, sizeof(Length), Handle);

		if (Length > 1)
			FS_Write (Ref.c_str(), Length, Handle);
	};

	void WriteString (const char *Str)
	{
		if (!Handle)
			return;

		sint32 Length = (Str) ? strlen(Str) + 1 : -1;

		FS_Write (&Length, sizeof(Length), Handle);

		if (Length > 1)
			FS_Write (Str, Length, Handle);
	};

	template <typename TType>
	void WriteArray (TType *Array, size_t Length)
	{
		if (!Handle)
			return;

		FS_Write (Array, sizeof(TType) * Length, Handle);
	};

	void Read (void *buffer, size_t size)
	{
		if (!Handle)
			return;

		FS_Read (buffer, size, Handle);
	};

	template <typename TType>
	void Read (void *buffer)
	{
		if (!Handle)
			return;

		FS_Read (buffer, sizeof(TType), Handle);
	};

	template <typename TType>
	TType Read ()
	{
		TType Val;
		Mem_Zero (&Val, sizeof(TType));

		if (!Handle)
			return Val;

		FS_Read (&Val, sizeof(TType), Handle);
		return Val;
	};

	cc_string ReadCCString ()
	{
		if (!Handle)
			return "";

		sint32 Length;
		FS_Read (&Length, sizeof(Length), Handle);
		
		if (Length > 1)
		{
			char *tempBuffer = QNew (TAG_GENERIC) char[Length];
			FS_Read (tempBuffer, Length, Handle);
			cc_string str (tempBuffer);
			QDelete[] tempBuffer;
			return str;
		}
		return "";
	};

	char *ReadString (sint32 Tag = TAG_GENERIC)
	{
		if (!Handle)
			return NULL;

		sint32 Length;
		FS_Read (&Length, sizeof(Length), Handle);
		
		char *tempBuffer = NULL;
		if (Length > 1)
		{
			tempBuffer = QNew (Tag) char[Length];
			FS_Read (tempBuffer, Length, Handle);
		}
		return tempBuffer;
	};

	cc_string ReadLine ()
	{
		if (!Handle)
			return "";

		cc_string tempStr;

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
		cc_string line = ReadLine ();

		Q_snprintfz (buf, maxSize-1, "%s", line.c_str());
		buf[maxSize-1] = 0;
	};

	template <typename TType>
	void ReadArray (TType *Array, size_t Length)
	{
		if (!Handle)
			return;

		FS_Read (Array, sizeof(TType) * Length, Handle);
	};

	void Seek (const ESeekOrigin seekOrigin, const size_t seekOffset)
	{
		if (!Handle)
			return;

		FS_Seek (Handle, seekOrigin, seekOffset);
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

// An inherited class to use stream-like operators
class CFileStream : public CFile
{
public:
	CFileStream (const char *fileName, EFileOpMode Mode) :
	  CFile (fileName, Mode)
	{
	};

	CFileStream &operator<< (bool &val)
	{
		Write<bool> (val);
		return *this;
	};

	CFileStream &operator<< (sint16 &val)
	{
		Write<sint16> (val);
		return *this;
	};

	CFileStream &operator<< (uint16 &val)
	{
		Write<uint16> (val);
		return *this;
	};

	CFileStream &operator<< (sint32 &val)
	{
		Write<sint32> (val);
		return *this;
	};

	CFileStream &operator<< (uint32 &val)
	{
		Write<uint32> (val);
		return *this;
	};

	CFileStream &operator<< (long &val)
	{
		Write<long> (val);
		return *this;
	};

	CFileStream &operator<< (unsigned long &val)
	{
		Write<unsigned long> (val);
		return *this;
	};

	CFileStream &operator<< (float &val)
	{
		Write<float> (val);
		return *this;
	};

	CFileStream &operator<< (double &val)
	{
		Write<double> (val);
		return *this;
	};

	CFileStream &operator<< (long double &val)
	{
		Write<long double> (val);
		return *this;
	};

	CFileStream &operator<< (const void *val)
	{
		Write<void*> (val);
		return *this;
	};

	CFileStream &operator>> (bool &val)
	{
		val = Read<bool> ();
		return *this;
	};

	CFileStream &operator>> (sint16 &val)
	{
		val = Read<sint16> ();
		return *this;
	};

	CFileStream &operator>> (uint16 &val)
	{
		val = Read<uint16> ();
		return *this;
	};

	CFileStream &operator>> (sint32 &val)
	{
		val = Read<sint32> ();
		return *this;
	};

	CFileStream &operator>> (uint32 &val)
	{
		val = Read<uint32> ();
		return *this;
	};

	CFileStream &operator>> (long &val)
	{
		val = Read<long> ();
		return *this;
	};

	CFileStream &operator>> (unsigned long &val)
	{
		val = Read<unsigned long> ();
		return *this;
	};

	CFileStream &operator>> (float &val)
	{
		val = Read<float> ();
		return *this;
	};

	CFileStream &operator>> (double &val)
	{
		val = Read<double> ();
		return *this;
	};

	CFileStream &operator>> (long double &val)
	{
		val = Read<long double> ();
		return *this;
	};

	CFileStream &operator>> (void *&val)
	{
		val = Read<void*> ();
		return *this;
	};
};

inline CFileStream &operator<< (CFileStream &Stream, cc_string &val)
{
	Stream.Write (val);
	return Stream;
};

inline CFileStream &operator>> (CFileStream &Stream, cc_string &val)
{
	val = Stream.Read<cc_string> ();
	return Stream;
};

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
	virtual void Query (cc_string &fileName) {};
};

//(const char *path, const char *filter, const char *extension, char **fileList, sint32 maxFiles, const bool addDir, const bool recurse);
#define MAX_FINDFILES_PATH	256
class CFindFiles
{
public:
	cc_string		Path;
	cc_string		Filter;
	cc_string		Extension;
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

	void FindFiles (CFindFilesCallback *Callback)
	{
		if (Filter.empty())
			Filter = "*";
		if (Extension.empty())
			Extension = "*";

		Files = FS_FindFiles (Path.c_str(), Filter.c_str(), Extension.c_str(), AddDir, Recurse);

		if (Callback)
		{
			for (size_t i = 0; i < Files.size(); i++)
				Callback->Query (Files[i]);
		}
	};
};
