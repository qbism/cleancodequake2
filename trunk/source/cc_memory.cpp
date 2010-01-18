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
Foundation, Inc., 59 Temple Place - Suite 330, Boston, MA 02111-1307, USA.
*/

//
// cc_memory.cpp
// Memory handling with sentinel checking and pools with tags for grouped free'ing.
//

#include "cc_local.h"

#define MEM_MAX_ALLOC			(128 * 1024 * 1024) // 128MB
#define MEM_SENTINEL_TOP(b)		(uint8)((/*0x32 +*/ (b)->realSize) & 0xff)
#define MEM_SENTINEL_FOOT(b)	(uint8)((/*0x64 +*/ (b)->realSize) & 0xff)
#define MEM_TOUCH_STEP			256

struct memBlock_t
{
	uint8				topSentinel;				// For memory integrity checking

	memBlock_t			*next, *prev;				// Next/Previous block in this pool
	struct memPool_t	*pool;						// Owner pool
	struct memPuddle_t	*puddle;					// Puddle allocated from
	sint32				tagNum;						// For group free

	const char			*allocFile;					// File the memory was allocated in
	sint32				allocLine;					// Line the memory was allocated at

	void				*memPointer;				// pointer to allocated memory
	size_t				memSize;					// Size minus the header, sentinel, and any rounding up to the uint8 barrier
	size_t				realSize;					// Actual size of block

	bool				Array;						// true if allocated with new[]
};

#define MEM_MAX_POOL_COUNT		32
#define MEM_MAX_POOL_NAME		64

struct memPool_t
{
	char				name[MEM_MAX_POOL_NAME];	// Name of pool
	bool				inUse;						// Slot in use?

	memBlock_t			blockHeadNode;				// Allocated blocks

	uint32				blockCount;					// Total allocated blocks
	size_t				byteCount;					// Total allocated bytes

	const char			*createFile;				// File this pool was created on
	sint32				createLine;					// Line this pool was created on
};

static memPool_t		m_poolList[MEM_MAX_POOL_COUNT];
static uint32			m_numPools;
 
void			*com_genericPool, // Generic memory; memory that will be freed on level change, but not necessarily needed for anything
				*com_levelPool, // Flushed per level
				*com_gamePool, // Flushed per entire game
				*com_fileSysPool, // File system (same as game, just here for easy pointer access)
				*com_entityPool, // Flushed specially
				*com_testPool,
				*com_zPool,
				*com_itemPool,
				*com_cvarPool,
				*com_writePool,
				*com_indexPool,
				*com_commandPool;

#define MEM_MAX_PUDDLES			42
#define MEM_MAX_PUDDLE_SIZE		(32768+1)

struct memPuddle_t
{
	memPuddle_t			*next, *prev;
	memBlock_t			*block;
};

struct memPuddleInfo_t
{
	size_t				blockSize;
	size_t				granularity;

	memPuddle_t			headNode;
	memPuddle_t			*freePuddles;
};

static memPuddleInfo_t	m_puddleList[MEM_MAX_PUDDLES];
static memPuddleInfo_t	*m_sizeToPuddle[MEM_MAX_PUDDLE_SIZE];
static uint32			m_puddleAdds;

/*
==============================================================================

	PUDDLE MANAGEMENT

==============================================================================
*/

/*
========================
Mem_AddPuddles
========================
*/
static void Mem_AddPuddles(memPuddleInfo_t *pInfo)
{
	size_t PuddleSize = (sizeof(memPuddle_t) + sizeof(memBlock_t) + pInfo->blockSize + sizeof(uint8));
	size_t TotalSize = PuddleSize * pInfo->granularity;

	void *Buffer = calloc(TotalSize, 1);
	if (!Buffer)
		GameError ("Mem_AddPuddles: failed on allocation of %i bytes", TotalSize);

	for (size_t i = 0; i < pInfo->granularity; i++)
	{
		memPuddle_t *Puddle = (memPuddle_t*)((uint8 *)Buffer + (PuddleSize * i));
		memBlock_t *MemBlock = (memBlock_t*)((uint8*)Puddle + sizeof(memPuddle_t));

		MemBlock->realSize = PuddleSize;
		MemBlock->memSize = pInfo->blockSize;
		MemBlock->memPointer = (void*)((uint8*)MemBlock + sizeof(memBlock_t));
		MemBlock->puddle = Puddle;

		Puddle->block = MemBlock;

		// Link this in
		Puddle->next = pInfo->freePuddles;
		pInfo->freePuddles = Puddle;
	}

	m_puddleAdds++;
}

/*
========================
Mem_PuddleAlloc
========================
*/
static memBlock_t *Mem_PuddleAlloc(const size_t Size)
{
	memPuddleInfo_t *pInfo = m_sizeToPuddle[Size];
	if (!pInfo->freePuddles)
		Mem_AddPuddles(pInfo);

	if (!pInfo->freePuddles)
		return NULL;

	memPuddle_t *Result = pInfo->freePuddles;

	_CC_ASSERT_EXPR (Result->block->realSize >= Size, "realSize >= Size");

	// Remove from free list
	pInfo->freePuddles = pInfo->freePuddles->next;

	// Move to the beginning of the list
	Result->prev = &pInfo->headNode;
	Result->next = pInfo->headNode.next;
	Result->next->prev = Result;
	Result->prev->next = Result;

	return Result->block;
}

/*
========================
Mem_PuddleFree
========================
*/
static void Mem_PuddleFree(memPuddle_t *Puddle)
{
	memPuddleInfo_t *pInfo = m_sizeToPuddle[Puddle->block->memSize];

	// Remove from linked active list
	Puddle->prev->next= Puddle->next;
	Puddle->next->prev = Puddle->prev;

	// Insert into free list
	Puddle->next = pInfo->freePuddles;
	pInfo->freePuddles = Puddle;

	// Zero-fill the memory
	Mem_Zero (Puddle->block->memPointer, Puddle->block->memSize);
}

/*
========================
Mem_PuddleInit
========================
*/
static void Mem_PuddleInit()
{
	size_t Size;

	m_puddleList[ 0].blockSize = 8;
	m_puddleList[ 1].blockSize = 12;
	m_puddleList[ 2].blockSize = 16;
	m_puddleList[ 3].blockSize = 32;
	m_puddleList[ 4].blockSize = 48;
	m_puddleList[ 5].blockSize = 64;
	m_puddleList[ 6].blockSize = 80;
	m_puddleList[ 7].blockSize = 96;
	m_puddleList[ 8].blockSize = 112;
	m_puddleList[ 9].blockSize = 128;
	m_puddleList[10].blockSize = 160;
	m_puddleList[11].blockSize = 192;
	m_puddleList[12].blockSize = 224;
	m_puddleList[13].blockSize = 256;
	m_puddleList[14].blockSize = 320;
	m_puddleList[15].blockSize = 384;
	m_puddleList[16].blockSize = 448;
	m_puddleList[17].blockSize = 512;
	m_puddleList[18].blockSize = 640;
	m_puddleList[19].blockSize = 768;
	m_puddleList[20].blockSize = 896;
	m_puddleList[21].blockSize = 1024;
	m_puddleList[22].blockSize = 1280;
	m_puddleList[23].blockSize = 1536;
	m_puddleList[24].blockSize = 1792;
	m_puddleList[25].blockSize = 2048;
	m_puddleList[26].blockSize = 2560;
	m_puddleList[27].blockSize = 3072;
	m_puddleList[28].blockSize = 3584;
	m_puddleList[29].blockSize = 4096;
	m_puddleList[30].blockSize = 5120;
	m_puddleList[31].blockSize = 6144;
	m_puddleList[32].blockSize = 7168;
	m_puddleList[33].blockSize = 8192;
	m_puddleList[34].blockSize = 10240;
	m_puddleList[35].blockSize = 12288;
	m_puddleList[36].blockSize = 14336;
	m_puddleList[37].blockSize = 16384;
	m_puddleList[38].blockSize = 20480;
	m_puddleList[39].blockSize = 24576;
	m_puddleList[40].blockSize = 28672;
	m_puddleList[41].blockSize = 32768;

	// Create a lookup table
	for (Size = 0; Size <MEM_MAX_PUDDLE_SIZE; Size++)
	{
		size_t Index = 0;
		for (; m_puddleList[Index].blockSize < Size; Index++);

		m_sizeToPuddle[Size] = &m_puddleList[Index];
	}

	for (Size = 0; Size < MEM_MAX_PUDDLES; Size++)
	{
		// Setup granularity values to allocate 1/4 MB at a time
		m_puddleList[Size].granularity = (32768*8) / m_puddleList[Size].blockSize;

		// Setup linked lists
		m_puddleList[Size].headNode.prev = &m_puddleList[Size].headNode;
		m_puddleList[Size].headNode.next = &m_puddleList[Size].headNode;
		m_puddleList[Size].freePuddles = NULL;
	}
}

/*
========================
Mem_PuddleWorthy
========================
*/
static inline bool Mem_PuddleWorthy(const size_t Size)
{
	return (Size < MEM_MAX_PUDDLE_SIZE);
}

/*
==============================================================================

	POOL MANAGEMENT

==============================================================================
*/

/*
========================
Mem_FindPool
========================
*/
static memPool_t *Mem_FindPool (const char *name)
{
	memPool_t	*pool = &m_poolList[0];
	uint32		i = 0;

	for (; i < m_numPools; pool++, i++)
	{
		if (!pool->inUse)
			continue;
		if (strcmp (name, pool->name))
			continue;

		return pool;
	}

	return NULL;
}

/*
========================
CC_Mem_CreatePool
========================
*/
void *CC_Mem_CreatePool(const char *name, const char *fileName, const sint32 fileLine)
{
	memPool_t	*pool;
	uint32		i;

	// Check name
	if (!name || !name[0])
	{
		GameError ("Mem_CreatePool: NULL name %s:#%i", fileName, fileLine);
		return NULL;
	}
	if (strlen(name)+1 >= MEM_MAX_POOL_NAME)
		DebugPrintf ("Mem_CreatePoole: name '%s' too long, truncating!\n", name);

	// See if it already exists
	pool = Mem_FindPool (name);
	if (pool)
		return pool;

	// Nope, create a slot
	for (i = 0, pool = &m_poolList[0]; i < m_numPools; pool++, i++)
	{
		if (!pool->inUse)
			break;
	}
	if (i == m_numPools)
	{
		if (m_numPools+1 >= MEM_MAX_POOL_COUNT)
			GameError ("Mem_CreatePool: MEM_MAX_POOL_COUNT");
		pool = &m_poolList[m_numPools++];
	}

	// Set defaults
	pool->blockHeadNode.prev = &pool->blockHeadNode;
	pool->blockHeadNode.next = &pool->blockHeadNode;
	pool->blockCount = 0;
	pool->byteCount = 0;
	pool->createFile = fileName;
	pool->createLine = fileLine;
	pool->inUse = true;
	Q_strncpyz (pool->name, name, sizeof(pool->name));
	return pool;
}


/*
========================
CC_Mem_DeletePool
========================
*/
size_t CC_Mem_DeletePool(void *_pool, const char *fileName, const sint32 fileLine)
{
	memPool_t *pool = (memPool_t*)_pool;
	size_t size;

	if (!pool)
		return 0;

	// Release all allocated memory
	size = CC_Mem_FreePool(pool, fileName, fileLine);

	// Simple, yes?
	pool->inUse = false;
	pool->name[0] = '\0';

	return size;
}


/*
==============================================================================

	POOL AND TAG MEMORY ALLOCATION

==============================================================================
*/

/*
========================
CC_Mem_CheckBlockIntegrity
========================
*/
static void CC_Mem_CheckBlockIntegrity (memBlock_t *mem, const char *fileName, const sint32 fileLine)
{
	if (mem->topSentinel != MEM_SENTINEL_TOP(mem))
	{
		_CC_ASSERT_EXPR (0, "Bad memory block top sentinel");
		GameError (
			"Mem_Free: bad memory block top sentinel\n"
			"check: %s:#%i",
			fileName, fileLine);
	}
	else if (*((uint8*)mem->memPointer+mem->memSize) != MEM_SENTINEL_FOOT(mem))
	{
		_CC_ASSERT_EXPR (0, "Bad memory footer sentinel (buffer overflow)");
		GameError (
			"Mem_Free: bad memory footer sentinel [buffer overflow]\n"
			"pool: %s\n"
			"alloc: %s:#%i\n"
			"check: %s:#%i",
			mem->pool ? mem->pool->name : "UNKNOWN", mem->allocFile, mem->allocLine, fileName, fileLine);
	}
}


/*
========================
CC_Mem_Free
========================
*/
size_t CC_Mem_Free (const void *ptr, const char *fileName, const sint32 fileLine, bool Array)
{
	memBlock_t	*mem;
	size_t		size;

	//_CC_ASSERT_EXPR (ptr, "Attempted to free NULL");
	if (!ptr)
		return 0;

	// Check sentinels
	mem = (memBlock_t *)((uint8 *)ptr - sizeof(memBlock_t));
	CC_Mem_CheckBlockIntegrity(mem, fileName, fileLine);

#ifdef _DEBUG
	if (Array != mem->Array)
		assert (0);
#endif

	// Decrement counters
	mem->pool->blockCount--;
	mem->pool->byteCount -= mem->realSize;
	size = mem->realSize;

	// De-link it
	mem->next->prev = mem->prev;
	mem->prev->next = mem->next;

	// Free it
	if (mem->puddle)
		Mem_PuddleFree(mem->puddle);
	else
		free (mem);

#ifdef _DEBUG
//	CC_Mem_CheckGlobalIntegrity (fileName, fileLine);
#endif

	return size;
}


/*
========================
CC_Mem_FreeTag

Free memory blocks assigned to a specified tag within a pool
========================
*/
size_t CC_Mem_FreeTag (void *_pool, const sint32 tagNum, const char *fileName, const sint32 fileLine)
{
	memPool_t *pool = (memPool_t*)_pool;
	memBlock_t	*mem, *next;
	memBlock_t	*headNode = &pool->blockHeadNode;
	size_t		size;

	if (!pool)
		return 0;

	size = 0;

	for (mem = pool->blockHeadNode.prev; mem != headNode; mem = next)
	{
		next = mem->prev;
		if (mem->tagNum == tagNum)
			size += CC_Mem_Free (mem->memPointer, fileName, fileLine, false);
	}

	return size;
}


/*
========================
CC_Mem_FreePool

Free all items within a pool
========================
*/
size_t CC_Mem_FreePool (void *_pool, const char *fileName, const sint32 fileLine)
{
	memPool_t *pool = (memPool_t*)_pool;
	memBlock_t	*mem, *next;
	memBlock_t	*headNode = &pool->blockHeadNode;
	size_t		size;

	if (!pool)
		return 0;

	size = 0;

	for (mem = pool->blockHeadNode.prev; mem != headNode; mem = next)
	{
		memBlock_t *oldMem = mem;

		next = mem->prev;
		
		if (oldMem == next)
			break;
		
		size += CC_Mem_Free (mem->memPointer, fileName, fileLine, mem->Array);
	}

	_CC_ASSERT_EXPR (pool->blockCount == 0, "Pool block count is not empty (improper free?)");
	_CC_ASSERT_EXPR (pool->byteCount == 0, "Pool byte count is not empty (improper free?)");
	return size;
}

#include <typeinfo>

/*
========================
CC_Mem_Alloc

Returns 0 filled memory allocated in a pool with a tag
========================
*/
void *CC_Mem_Alloc(size_t size, void *_pool, const sint32 tagNum, const char *fileName, const sint32 fileLine, bool Array)
{
	memPool_t *pool = (memPool_t*)_pool;
	memBlock_t *mem;

	// Check pool
	if (!pool)
		return NULL;

	// Check size
	if (size <= 0)
	{
		DebugPrintf ("Mem_Alloc: Attempted allocation of '%i' memory ignored\n" "alloc: %s:#%i\n", size, fileName, fileLine);
		return NULL;
	}
	if (size > MEM_MAX_ALLOC)
		GameError ("Mem_Alloc: Attempted allocation of '%i' bytes!\n" "alloc: %s:#%i\n", size, fileName, fileLine);

	// Try to allocate in a puddle
	if (Mem_PuddleWorthy(size))
		mem = Mem_PuddleAlloc(size);
	else
		mem = NULL;

	if (!mem)
	{
		// Add header and round to cacheline
		const size_t newSize = (size + sizeof(memBlock_t) + sizeof(uint8) + 31) & ~31;
		mem = (memBlock_t*)calloc (1, newSize);
		if (!mem)
		{
			GameError ("Mem_Alloc: failed on allocation of %i bytes\n" "alloc: %s:#%i", newSize, fileName, fileLine);
			return NULL;
		}

		mem->memPointer = (void*)((uint8*)mem + sizeof(memBlock_t));
		mem->memSize = size;
		mem->puddle = NULL;
		mem->realSize = newSize;
		mem->Array = Array;
	}

	// Fill in the header
	mem->tagNum = tagNum;
	mem->pool = pool;
	mem->allocFile = fileName;
	mem->allocLine = fileLine;
	mem->Array = Array;

	// Fill in the integrity sentinels
	mem->topSentinel = MEM_SENTINEL_TOP(mem);
	*((uint8*)mem->memPointer+mem->memSize) = MEM_SENTINEL_FOOT(mem);

	// For integrity checking and stats
	pool->blockCount++;
	pool->byteCount += mem->realSize;

	// Link it in to the appropriate pool
	mem->prev = &pool->blockHeadNode;
	mem->next = pool->blockHeadNode.next;
	mem->next->prev = mem;
	mem->prev->next = mem;

#ifdef _DEBUG
	//CC_Mem_CheckGlobalIntegrity (fileName, fileLine);
#endif

	return mem->memPointer;
}


/*
========================
CC_Mem_ReAlloc
========================
*/
void *CC_Mem_ReAlloc(void *ptr, size_t newSize, const char *fileName, const sint32 fileLine)
{
	void *Result;
	if (ptr && newSize)
	{
		memBlock_t *Block = (memBlock_t*)((uint8*)ptr - sizeof(memBlock_t));

		// Just in case...
		if (Block->memSize == newSize)
			return ptr;

		// Buffer check
		CC_Mem_CheckBlockIntegrity(Block, fileName, fileLine);

		// Locate the memory block
		_CC_ASSERT_EXPR (Block->memPointer == ptr, "Block's memory pointer doesn't point to wanted memory");

		// Allocate
		Result = CC_Mem_Alloc(newSize, Block->pool, Block->tagNum, fileName, fileLine, false);
		memcpy(Result, ptr, Min(newSize,Block->memSize));

		// Release old memory
		CC_Mem_Free(ptr, fileName, fileLine, false);
	}
	else if (!ptr)
	{
		// FIXME: The pool and tag are 'lost' in this case...
		Result = CC_Mem_Alloc(newSize, com_genericPool, 0, fileName, fileLine, false);
	}
	else
	{
		CC_Mem_Free(ptr, fileName, fileLine, false);
		Result = NULL;
	}

	return Result;
}

/*
==============================================================================

	MISC FUNCTIONS

==============================================================================
*/

/*
================
CC_Mem_PoolStrDup

No need to null terminate the extra spot because Mem_Alloc returns zero-filled memory
================
*/
char *CC_Mem_PoolStrDup (const char *in, void *_pool, const sint32 tagNum, const char *fileName, const sint32 fileLine)
{
	memPool_t *pool = (memPool_t*)_pool;
	char	*out;

	out = (char*)CC_Mem_Alloc ((size_t)(strlen (in) + 1), pool, tagNum, fileName, fileLine, false);
	strcpy_s (out, (size_t)(strlen (in) + 1), in);

	return out;
}


/*
================
CC_Mem_PoolSize
================
*/
size_t CC_Mem_PoolSize (void *_pool)
{
	memPool_t *pool = (memPool_t*)_pool;
	if (!pool)
		return 0;

	return pool->byteCount;
}


/*
================
CC_Mem_TagSize
================
*/
size_t CC_Mem_TagSize (void *_pool, const sint32 tagNum)
{
	memPool_t *pool = (memPool_t*)_pool;
	memBlock_t	*mem;
	memBlock_t	*headNode = &pool->blockHeadNode;
	size_t		size;

	if (!pool)
		return 0;

	size = 0;
	for (mem = pool->blockHeadNode.prev; mem != headNode; mem = mem->prev)
	{
		if (mem->tagNum == tagNum)
			size += mem->realSize;
	}

	return size;
}


/*
========================
CC_Mem_ChangeTag
========================
*/
size_t CC_Mem_ChangeTag (void *_pool, const sint32 tagFrom, const sint32 tagTo)
{
	memPool_t *pool = (memPool_t*)_pool;
	memBlock_t	*mem;
	memBlock_t	*headNode = &pool->blockHeadNode;
	uint32		numChanged;

	if (!pool)
		return 0;

	numChanged = 0;
	for (mem = pool->blockHeadNode.prev; mem != headNode; mem = mem->prev)
	{
		if (mem->tagNum == tagFrom)
		{
			mem->tagNum = tagTo;
			numChanged++;
		}
	}

	return numChanged;
}


/*
========================
CC_Mem_CheckPoolIntegrity
========================
*/
void CC_Mem_CheckPoolIntegrity (void *_pool, const char *fileName, const sint32 fileLine)
{
	memPool_t *pool = (memPool_t*)_pool;
	memBlock_t	*mem;
	memBlock_t	*headNode = &pool->blockHeadNode;
	uint32		blocks;
	size_t		size;

	_CC_ASSERT_EXPR (pool, "Tried to check integrity of a NULL pool");
	if (!pool)
		return;

	// Check sentinels
	for (mem = pool->blockHeadNode.prev, blocks = 0, size = 0; mem != headNode; blocks++, mem = mem->prev)
	{
		size += mem->realSize;
		CC_Mem_CheckBlockIntegrity (mem, fileName, fileLine);
	}

	// Check block/uint8 counts
	if (pool->blockCount != blocks)
	{
		_CC_ASSERT_EXPR (0, "Bad block count");
		DebugPrintf ("Mem_CheckPoolIntegrity: bad block count\n" "check: %s:#%i", fileName, fileLine);
	}
	if (pool->byteCount != size)
	{
		_CC_ASSERT_EXPR (0, "Bad block count");
		DebugPrintf ("Mem_CheckPoolIntegrity: bad pool size\n" "check: %s:#%i", fileName, fileLine);
	}
}


/*
========================
CC_Mem_CheckGlobalIntegrity
========================
*/
void CC_Mem_CheckGlobalIntegrity(const char *fileName, const sint32 fileLine)
{
	//CTimer Timer;

	for (uint32 i = 0; i < m_numPools; i++)
	{
		memPool_t *pool = &m_poolList[i];
		if (pool->inUse)
			CC_Mem_CheckPoolIntegrity(pool, fileName, fileLine);
	}

	//DebugPrintf ("Mem_CheckGlobalIntegrity: "TIMER_STRING"\n", Timer.Get());
}


/*
========================
CC_Mem_TouchPool
========================
*/
void CC_Mem_TouchPool(void *_pool, const char *fileName, const sint32 fileLine)
{
	memPool_t *pool = (memPool_t*)_pool;
	memBlock_t	*mem;
	memBlock_t	*headNode = &pool->blockHeadNode;
	uint32		i;
	sint32			sum;

	_CC_ASSERT_EXPR (pool, "Attempted to touch a NULL pool");
	if (!pool)
		return;

	sum = 0;

	// Cycle through the blocks
	for (mem = pool->blockHeadNode.prev; mem != headNode; mem = mem->prev)
	{
		// Touch each page
		for (i = 0; i < mem->memSize; i += MEM_TOUCH_STEP)
			sum += ((uint8 *)mem->memPointer)[i];
	}
}


/*
========================
CC_Mem_TouchGlobal
========================
*/
void CC_Mem_TouchGlobal(const char *fileName, const sint32 fileLine)
{
	CTimer Timer;

	// Touch every pool
	uint32 numTouched = 0;
	for (uint32 i = 0; i < m_numPools; i++)
	{
		memPool_t *pool = &m_poolList[i];
		if (!pool->inUse)
			continue;

		CC_Mem_TouchPool(pool, fileName, fileLine);
		numTouched++;
	}

	DebugPrintf("Mem_TouchGlobal: %u pools touched in "TIMER_STRING"\n", numTouched, Timer.Get());
}

/*
==============================================================================

	CONSOLE COMMANDS

==============================================================================
*/

/*
========================
Mem_Check_f
========================
*/
static void Mem_Check_f(CPlayerEntity *ent)
{
	Mem_CheckGlobalIntegrity();
}


/*
========================
Mem_Stats_f
========================
*/
void Mem_Stats_f(CPlayerEntity *ent)
{
	ent->PrintToClient (PRINT_HIGH, "Memory stats:\n");
	ent->PrintToClient (PRINT_HIGH, "    blocks size                  puddle name\n");
	ent->PrintToClient (PRINT_HIGH, "--- ------ ---------- ---------- ------ --------\n");

	uint32 totalBlocks = 0;
	size_t totalBytes = 0;
	uint32 totalPuddles =0 ;
	uint32 poolCount = 0;
	for (uint32 i = 0; i < m_numPools; i++)
	{
		memPool_t *pool = &m_poolList[i];
		if (!pool->inUse)
			continue;

		poolCount++;

		// Cycle through the blocks, and find out how many are puddle allocations
		uint32 numPuddles = 0;
		memBlock_t	*headNode = &pool->blockHeadNode;
		for (memBlock_t *mem = pool->blockHeadNode.prev; mem != headNode; mem = mem->prev)
		{
			if (mem->puddle)
				numPuddles++;
		}

		totalPuddles += numPuddles;
		const float puddlePercent = (pool->blockCount) ? ((float)numPuddles/(float)pool->blockCount) * 100.0f : 0.0f;

		ent->PrintToClient (PRINT_HIGH, "#%2i %6i %9iB (%6.3fMB) %5.0f%% %s\n", poolCount, pool->blockCount, pool->byteCount, pool->byteCount/1048576.0f, puddlePercent, pool->name);

		totalBlocks += pool->blockCount;
		totalBytes += pool->byteCount;
	}

	const float puddlePercent = (totalBlocks) ? ((float)totalPuddles/(float)totalBlocks) * 100.0f : 0.0f;

	ent->PrintToClient (PRINT_HIGH, "----------------------------------------\n");
	ent->PrintToClient (PRINT_HIGH, "Total: %i pools, %i blocks, %i bytes (%6.3fMB) (%5.2f%% in %i puddles)\n", poolCount, totalBlocks, totalBytes, totalBytes/1048576.0f, puddlePercent, m_puddleAdds);
}


/*
========================
Mem_Touch_f
========================
*/
static void Mem_Touch_f(CPlayerEntity *ent)
{
	Mem_TouchGlobal();
}

/*
========================
Mem_FreePools
========================
*/
void Mem_FreePools ()
{
	for (uint32 i = 0; i < m_numPools; i++)
		Mem_DeletePool (&m_poolList[i]);
	m_numPools = 0;
}

/*
==============================================================================

	INIT / SHUTDOWN

==============================================================================
*/

/*
========================
Mem_Register
========================
*/
void Mem_Register ()
{
	Cmd_AddCommand ("g_memcheck",		Mem_Check_f);
	Cmd_AddCommand ("g_memstats",		Mem_Stats_f);
	Cmd_AddCommand ("g_memtouch",		Mem_Touch_f);
}

#include "zlib.h"

void *Mem_ZAlloc (uint32 size)
{
	return Mem_PoolAlloc (size, com_zPool, 0);
}

void Mem_ZFree (void *ptr)
{
	Mem_Free (ptr);
}

/*
========================
Mem_Init
========================
*/
void Mem_Init ()
{
	// Clear
	Mem_Zero (&m_poolList, sizeof(m_poolList));
	m_numPools = 0;

	// Setup puddles
	Mem_PuddleInit();

	com_genericPool = Mem_CreatePool ("Generic memory pool");
	com_levelPool = Mem_CreatePool ("Level memory pool");
	com_gamePool = Mem_CreatePool ("Game memory pool");
	com_fileSysPool = Mem_CreatePool ("File system pool");
	com_entityPool = Mem_CreatePool ("Entity system pool");
	com_itemPool = Mem_CreatePool ("Item system pool");
	com_cvarPool = Mem_CreatePool ("Cvar system pool");
	com_commandPool = Mem_CreatePool ("Cvar system pool");
	com_writePool = Mem_CreatePool ("Message system pool");
	com_indexPool = Mem_CreatePool ("Indexing system pool");
	com_testPool = Mem_CreatePool ("Test pool");
	com_zPool = Mem_CreatePool ("ZLib Pool");

	zsetfunctions (Mem_ZAlloc, Mem_ZFree);
}

