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
// cc_indexing.cpp
// New indexing system
//

#include "cc_local.h"

CIndexList	ModelList, SoundList, ImageList;

CIndexList::CIndexList () :
numIndexes(0),
List(),
HashList(),
firstIndex(0)
{
}

sint32 CIndexList::GetIndex (const char *String)
{
	// Check through the itemlist
	uint32 hash = Com_HashGeneric(String, MAX_INDEXES);

	for (THashedMediaIndexListType::iterator it = HashList.equal_range(hash).first; it != HashList.equal_range(hash).second; ++it)
	{
		CIndex *Index = List.at((*it).second);
		if (Q_stricmp (Index->Name.c_str(), String) == 0)
			return Index->Index;
	}
	return -1;
}

sint32 CIndexList::AddToList (const char *String, MediaIndex Index)
{
	if (!firstIndex)
		firstIndex = Index;

	CIndex *NewIndex = QNew (com_indexPool, 0) CIndex;
	NewIndex->Index = Index;
	NewIndex->Name = String;

	List.push_back (NewIndex);

	ListMap[Index] = NewIndex;

	// Link it in the hash tree
	HashList.insert (std::make_pair<size_t, size_t> (Com_HashGeneric (String, MAX_INDEXES), List.size()-1));

	numIndexes++;
	return Index;
}

inline bool OverFlow ()
{
	return ((ModelList.numIndexes + SoundList.numIndexes + ImageList.numIndexes) >= 255);
}

const char *StringFromSoundIndex (MediaIndex Index)
{
	if (SoundList.ListMap.find(Index) == SoundList.ListMap.end())
		return NULL;
	return (*SoundList.ListMap.find(Index)).second->Name.c_str();
}

const char *StringFromModelIndex (MediaIndex Index)
{
	if (Index <= ModelList.firstIndex || Index == 255)
		return NULL;
	if (ModelList.ListMap.find(Index) == ModelList.ListMap.end())
		return NULL;
	return (*ModelList.ListMap.find(Index)).second->Name.c_str();
}

const char *StringFromImageIndex (MediaIndex Index)
{
	if (ImageList.ListMap.find(Index) == ImageList.ListMap.end())
		return NULL;
	return (*ImageList.ListMap.find(Index)).second->Name.c_str();
}

_CC_DISABLE_DEPRECATION

MediaIndex ModelIndex (const char *String)
{
	// Do we exist?
	sint32 Index = ModelList.GetIndex(String);
	if (Index == -1)
	{
		//DebugPrintf ("ModelIndex: Adding new index for %s\n", String);
		if (OverFlow())
		{
			DebugPrintf ("Index overflow registering \"%s\"\n", String);
			return 0;
		}

		return ModelList.AddToList (String, gi.modelindex(const_cast<char*>(String)));
	}
	//DebugPrintf ("ModelIndex: Using existing index %s\n", String);
	return Index;
}

MediaIndex SoundIndex (const char *String)
{
	// Do we exist?
	sint32 Index = SoundList.GetIndex(String);
	if (Index == -1)
	{
		//DebugPrintf ("SoundIndex: Adding new index for %s\n", String);
		if (OverFlow())
		{
			DebugPrintf ("Index overflow registering \"%s\"\n", String);
			return 0;
		}

		return SoundList.AddToList (String, gi.soundindex(const_cast<char*>(String)));
	}
	//DebugPrintf ("SoundIndex: Using existing index %s\n", String);
	return Index;
}

MediaIndex ImageIndex (const char *String)
{
	// Do we exist?
	sint32 Index = ImageList.GetIndex(String);
	if (Index == -1)
	{
		//DebugPrintf ("ImageIndex: Adding new index for %s\n", String);
		if (OverFlow())
		{
			DebugPrintf ("Index overflow registering \"%s\"\n", String);
			return 0;
		}

		return ImageList.AddToList (String, gi.imageindex(const_cast<char*>(String)));
	}
	//DebugPrintf ("ImageIndex: Using existing index %s\n", String);
	return Index;
}

void CIndexList::Clear ()
{
	for (uint8 i = 0; i < numIndexes; i++)
		delete List[i];

	numIndexes = firstIndex = 0;
	List.clear();
	HashList.clear();
}

void ClearList ()
{
	ModelList.Clear();
	ImageList.Clear();
	SoundList.Clear();
}

extern char	ReadConfigSt[MAX_CFGSTRINGS][MAX_CFGSTRLEN];
void ListConfigstrings ()
{
	bool firstIndex = false;
	for (uint16 i = 1; i <= MAX_CS_MODELS; i++)
	{
		if (!strlen(ReadConfigSt[CS_MODELS+i]))
			continue;
		else if (CS_MODELS+i == 33)
			ModelList.firstIndex = 0;
		else if (ReadConfigSt[CS_MODELS+i][0] == '*')
			ModelList.firstIndex = atoi(ReadConfigSt[CS_MODELS+i] + 1);
		else
		{
			if (!firstIndex)
			{
				ModelList.firstIndex++;
				firstIndex = true;
			}
			ModelList.AddToList (ReadConfigSt[CS_MODELS+i], gi.modelindex(ReadConfigSt[CS_MODELS+i]));
		}
	}
	for (uint16 i = 1; i <= MAX_CS_SOUNDS; i++)
	{
		if (!strlen(ReadConfigSt[CS_SOUNDS+i]))
			continue;
		SoundList.AddToList (ReadConfigSt[CS_SOUNDS+i], gi.soundindex(ReadConfigSt[CS_SOUNDS+i]));
	}
	for (uint16 i = 1; i <= MAX_CS_IMAGES; i++)
	{
		if (!strlen(ReadConfigSt[CS_IMAGES+i]))
			continue;
		ImageList.AddToList (ReadConfigSt[CS_IMAGES+i], gi.imageindex(ReadConfigSt[CS_IMAGES+i]));
	}
}

void SvCmd_IndexList_f ()
{
	DebugPrintf ("Models: (%u) + %u inline\n", ModelList.numIndexes, ModelList.firstIndex);
	for (uint8 i = 0; i < ModelList.numIndexes; i++)
		DebugPrintf ("%s\n", ModelList.List[i]->Name.c_str());

	DebugPrintf ("\nSounds: (%u)\n", SoundList.numIndexes);
	for (uint8 i = 0; i < SoundList.numIndexes; i++)
		DebugPrintf ("%s\n", SoundList.List[i]->Name.c_str());

	DebugPrintf ("\nImages: (%u)\n", ImageList.numIndexes);
	for (uint8 i = 0; i < ImageList.numIndexes; i++)
		DebugPrintf ("%s\n", ImageList.List[i]->Name.c_str());

	DebugPrintf ("\nTotal: %u\n", ModelList.numIndexes + SoundList.numIndexes + ImageList.numIndexes);
}

_CC_ENABLE_DEPRECATION

