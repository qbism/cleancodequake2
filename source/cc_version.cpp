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
// cc_version.cpp
// 
//

#include "cc_local.h"

#define CURL_STATICLIB
#include "curl\curl.h"

#define VERSION_PATH GAMENAME"/version.ver"

void WriteVersion ()
{
	fileHandle_t handle = FS_OpenFile (VERSION_PATH, FILEMODE_WRITE|FILEMODE_CREATE);

	if (!handle)
		return;

	FS_Print (handle, "%s %s %s %s", CLEANCODE_VERSION_PREFIX, CLEANCODE_VERSION_MAJOR, CLEANCODE_VERSION_MINOR, CLEANCODE_VERSION_BUILD);
	FS_Close (handle);
}

void VerifyVersionFile ()
{
	CFileBuffer Buffer (VERSION_PATH, true);
	CParser Parser (Buffer.GetBuffer<char> (), PSP_COMMENT_LINE);

	const char *token;

	std::cc_string prefix;
	Parser.ParseToken (PSF_ALLOW_NEWLINES, &token);
	prefix = token;

	uint8 minor;
	uint16 major;
	uint32 build;
	Parser.ParseDataType<uint8> (PSF_ALLOW_NEWLINES, &minor, 1);
	Parser.ParseDataType<uint16> (PSF_ALLOW_NEWLINES, &major, 1);
	Parser.ParseDataType<uint32> (PSF_ALLOW_NEWLINES, &build, 1);

	if (CompareVersion (prefix.c_str(), minor, major, build))
	{
		DebugPrintf ("Version file out of date; updating...\n");
		WriteVersion ();
	}
}

void Cmd_CCVersion_t (CPlayerEntity *Player)
{
	Player->PrintToClient (PRINT_HIGH, "This server is running CleanCode version %s\n", CLEANCODE_VERSION);
}

void SvCmd_CCVersion_t ()
{
	DebugPrintf ("This server is running CleanCode version %s\n", CLEANCODE_VERSION);
}

#include <curl/curl.h>
#include <curl/types.h>
#include <curl/easy.h>

struct MemoryStruct
{
	char *memory;
	size_t size;
};

static void *myrealloc(void *ptr, size_t size)
{
	/* There might be a realloc() out there that doesn't like reallocing
	NULL pointers, so we take care of it here */
	if(ptr)
		return Mem_ReAlloc (ptr, size);
	else
		return Mem_Alloc (size);
}

static size_t WriteMemoryCallback(void *ptr, size_t size, size_t nmemb, void *data)
{
	size_t realsize = size * nmemb;
	struct MemoryStruct *mem = (struct MemoryStruct *)data;

	mem->memory = (char*)myrealloc(mem->memory, mem->size + realsize + 1);

	if (mem->memory)
	{
		memcpy(&(mem->memory[mem->size]), ptr, realsize);
		mem->size += realsize;
		mem->memory[mem->size] = 0;
	}

	return realsize;
}

void CheckNewVersion ()
{
	CURL *curl_handle;

	struct MemoryStruct chunk;

	chunk.memory=NULL; /* we expect realloc(NULL, size) to work */
	chunk.size = 0;    /* no data at this point */

	curl_global_init(CURL_GLOBAL_ALL);

	/* init the curl session */
	curl_handle = curl_easy_init();

	/* specify URL to get */
	curl_easy_setopt(curl_handle, CURLOPT_URL, "http://cool.haxx.se/");

	/* send all data to this function  */
	curl_easy_setopt(curl_handle, CURLOPT_WRITEFUNCTION, WriteMemoryCallback);

	/* we pass our 'chunk' struct to the callback function */
	curl_easy_setopt(curl_handle, CURLOPT_WRITEDATA, (void *)&chunk);

	/* some servers don't like requests that are made without a user-agent
	field, so we provide one */
	curl_easy_setopt(curl_handle, CURLOPT_USERAGENT, "libcurl-agent/1.0");

	/* get it! */
	curl_easy_perform(curl_handle);

	/* cleanup curl stuff */
	curl_easy_cleanup(curl_handle);

	/*
	* Now, our chunk.memory points to a memory block that is chunk.size
	* bytes big and contains the remote file.
	*
	* Do something nice with it!
	*
	* You should be aware of the fact that at this point we might have an
	* allocated data block, and nothing has yet deallocated that data. So when
	* you're done with it, you should free() it as a nice application.
	*/

	if(chunk.memory)
		Mem_Free (chunk.memory);

	/* we're done with libcurl, so clean it up */
	curl_global_cleanup();
}

void InitVersion ()
{
	DebugPrintf ("Checking for new version...\n");

	if (!FS_FileExists(VERSION_PATH))
	{
		DebugPrintf ("Version file non-existant, writing... ");
		WriteVersion ();
		DebugPrintf ("done\n");
	}
	else
		VerifyVersionFile ();
	
	CheckNewVersion ();
}
