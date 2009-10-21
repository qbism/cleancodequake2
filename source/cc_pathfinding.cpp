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
// cc_pathfinding.cpp
// Pathfinding algorithm for monsters
//


#include "cc_local.h"

#ifdef MONSTERS_USE_PATHFINDING
#include "cc_tent.h"
#include "cc_pathfinding.h"
#include "cc_cmds.h"

class CNodeEntity : public CBaseEntity
{
public:
	CNodeEntity () :
	  CBaseEntity ()
	  {
	  };
};

void SpawnNodeEntity (CPathNode *Node);
void CheckNodeFlags (CPathNode *Node);
size_t GetNodeIndex (CPathNode *Node);

std::vector<CPathNode*, std::level_allocator<CPathNode*> >		Closed, Open;

#define MAX_SAVED_PATHS	512
struct SSavedPath_t
{
	CPath	*ToEnd[MAX_SAVED_PATHS];
};

SSavedPath_t SavedPaths[512];

CPath::CPath () :
Start(NULL),
End(NULL),
Weight(0),
NumNodes(0)
{
};

CPath::CPath (CPathNode *Start, CPathNode *End) :
Start(Start),
End(End)
{
	Weight = NumNodes = 0;
	CreatePath ();
};

bool CPath::NodeIsClosed (CPathNode *Node)
{
	for (std::vector<CPathNode*, std::level_allocator<CPathNode*> >::iterator it = Closed.begin(); it < Closed.end(); ++it )
	{
		CPathNode *Check = *it;

		if (Check == Node)
			return true;
	}
	return false;
};

void CPath::AddNodeToClosed (CPathNode *Node)
{
	Closed.push_back (Node);
}

void CPath::RemoveFromClosed (CPathNode *Node)
{
	for (size_t i = 0; i < Closed.size(); i++)
	{
		if (Closed[i] == Node)
		{
			Closed.erase (Closed.begin()+i);
			break;
		}
	}
}

bool CPath::NodeIsOpen (CPathNode *Node)
{
	for (std::vector<CPathNode*, std::level_allocator<CPathNode*> >::iterator it = Open.begin(); it < Open.end(); it++ )
	{
		CPathNode *Check = *it;

		if (Check == Node)
			return true;
	}
	return false;
};

void CPath::AddNodeToOpen (CPathNode *Node)
{
	Open.push_back (Node);
}

void CPath::RemoveFromOpen (CPathNode *Node)
{
	for (size_t i = 0; i < Open.size(); i++)
	{
		if (Open[i] == Node)
		{
			Open.erase (Open.begin()+i);
			break;
		}
	}
}

uint32 CPath::DistToGoal (CPathNode *Node)
{
	return (Node->Origin - End->Origin).Length();
}

bool VecInFront (vec3f &angles, vec3f &origin1, vec3f &origin2);

void CPath::CreatePath ()
{
	Closed.clear ();
	Open.clear ();
	Start->G = 0;
	Start->H = DistToGoal (Start);
	Start->F = Start->G + Start->H;
	Start->Parent = NULL;
	Open.push_back (Start);
	while (Open.size())
	{
		CPathNode *n = Open[Open.size()-1];
		Open.pop_back();

		if (n == End)
		{
			// Construct path
			//gi.dprintf ("Path!\n");
			Incomplete = false;

			while (n)
			{
				NumNodes++;
				Weight += n->G;
				Path.push_back (n);
				n = n->Parent;
			}
			// Pop the start off the stack, because we don't want to run back and forth
			//Path.pop_back();
			return;
		}
		for (size_t i = 0; i < n->Children.size(); i++)
		{
			CPathNode *nl = n->Children[i];
			uint32 newg = n->G + (nl->Origin - n->Origin).Length();
			if (NodeIsOpen(nl))
			{
				if (nl->G <= newg)
					continue;
				RemoveFromOpen (nl);
			}
			else if (NodeIsClosed(nl))
			{
				if (nl->G <= newg)
					continue;
				RemoveFromClosed (nl);
			}
			nl->Parent = n;
			nl->G = newg;
			nl->H = DistToGoal(nl);
			nl->F = nl->G + nl->H;

			AddNodeToOpen (nl);
		}
		AddNodeToClosed (n);
	}
	// No possible path.
	Incomplete = true;
};

void CPath::Save (fileHandle_t f)
{
	size_t index;
	// Save all local data
	index = GetNodeIndex(Start);
	FS_Write (&index, sizeof(uint32), f);

	index = GetNodeIndex(End);
	FS_Write (&index, sizeof(uint32), f);

	FS_Write (&Weight, sizeof(uint32), f);
	FS_Write (&NumNodes, sizeof(uint32), f);

	// Save the path data
	size_t pathLen = Path.size();
	FS_Write (&pathLen, sizeof(uint32), f);

	for (size_t i = 0; i < pathLen; i++)
	{
		CPathNode *Node = Path[i];
		index = GetNodeIndex(Node);

		FS_Write (&index, sizeof(uint32), f);
	}
}

#define MAX_NODES 512

std::vector<CPathNode*, std::level_allocator<CPathNode*> > NodeList;

void CPath::Load (fileHandle_t f)
{
	uint32 index;
	FS_Read (&index, sizeof(uint32), f);
	Start = NodeList[index];

	FS_Read (&index, sizeof(uint32), f);
	End = NodeList[index];

	FS_Read (&Weight, sizeof(uint32), f);
	FS_Read (&NumNodes, sizeof(uint32), f);

	// Save the path data
	size_t pathLen;
	FS_Read (&pathLen, sizeof(uint32), f);

	for (size_t i = 0; i < pathLen; i++)
	{
		FS_Read (&index, sizeof(uint32), f);
		Path.push_back (NodeList[index]);
	}
}

void Cmd_Node_f (CPlayerEntity *ent);
CCvar *DebugNodes;

void Nodes_Register ()
{
	Cmd_AddCommand ("node",				Cmd_Node_f);
	DebugNodes = QNew (com_gamePool, 0) CCvar("node_debug", "0", CVAR_LATCH_SERVER);
}

void InitNodes ()
{
	NodeList.clear();
	memset (SavedPaths, 0, sizeof(SavedPaths));
}

edict_t *PlayerNearby (vec3f origin, int distance)
{
	CPlayerEntity *ent = NULL;

	while ((ent = FindRadius <CPlayerEntity, ENT_PLAYER> (ent, origin, distance)) != NULL)
	{
		if (ent->GetInUse())
			return ent->gameEntity;
	}
	return NULL;
}

void PrintVerboseNodes (vec3f origin, uint32 numNode, ENodeType Type)
{
	CPlayerEntity *ent = NULL;

	while ((ent = FindRadius <CPlayerEntity, ENT_PLAYER>(ent, origin, 25)) != NULL)
	{
		if (ent->GetInUse())
		{
			char *nodeType = "normal";
			switch (Type)
			{
			case NODE_DOOR:
				nodeType = "door";
				break;
			case NODE_PLATFORM:
				nodeType = "platform";
				break;
			case NODE_JUMP:
				nodeType = "jump";
				break;
			}
			ent->PrintToClient (PRINT_HIGH, "You are very close to node %i (%s)\n", numNode, nodeType);
		}
	}
}

CPathNode *DropNode (edict_t *ent)
{
	NodeList.push_back(QNew (com_levelPool, 0) CPathNode(ent->state.origin, NODE_REGULAR));

	SpawnNodeEntity (NodeList.at(NodeList.size() - 1));
	ClientPrintf (ent, PRINT_HIGH, "Node %i added\n", NodeList.size());

	return NodeList.at(NodeList.size() - 1);
}

void RunNodes()
{
	if (!DebugNodes->Integer())
		return;

	for (uint32 i = 0; i < NodeList.size(); i++)
	{
		if (PlayerNearby(NodeList[i]->Origin, 250))
		{
			for (std::vector<CPathNode*, std::level_allocator<CPathNode*> >::iterator it = NodeList[i]->Children.begin(); it < NodeList[i]->Children.end(); it++ )
			{
				CPathNode *Child = *it;
				CTempEnt_Trails::BFGLaser (NodeList[i]->Origin, Child->Origin);
			}

			PrintVerboseNodes (NodeList[i]->Origin, i, NodeList[i]->Type);
		}
	}
}

void SpawnNodeEntity (CPathNode *Node)
{
	if (!DebugNodes->Integer())
		return;

	Node->Ent = new CNodeEntity;
	Node->Ent->State.GetModelIndex() = ModelIndex("models/objects/grenade2/tris.md2");
	Node->Ent->State.GetOrigin() = Node->Origin;
	Node->Ent->Link ();
	CheckNodeFlags (Node);
}

void CheckNodeFlags (CPathNode *Node)
{
	if (!DebugNodes->Integer())
		return;

	Node->Ent->State.GetEffects() = Node->Ent->State.GetRenderEffects() = 0;

	switch (Node->Type)
	{
	case NODE_DOOR:
		Node->Ent->State.GetEffects() = EF_COLOR_SHELL;
		Node->Ent->State.GetRenderEffects() = RF_SHELL_RED;
		break;
	case NODE_PLATFORM:
		Node->Ent->State.GetEffects() = EF_COLOR_SHELL;
		Node->Ent->State.GetRenderEffects() = RF_SHELL_GREEN;
		break;
	case NODE_JUMP:
		Node->Ent->State.GetEffects() = EF_COLOR_SHELL;
		Node->Ent->State.GetRenderEffects() = RF_SHELL_BLUE;
		break;
	}
}

void ConnectNode (CPathNode *Node1, CPathNode *Node2);
void AddNode (CPlayerEntity *ent, vec3f origin)
{
	NodeList.push_back(QNew (com_levelPool, 0) CPathNode(origin, NODE_REGULAR));

	SpawnNodeEntity (NodeList.at(NodeList.size() - 1));
	ent->PrintToClient (PRINT_HIGH, "Node %i added\n", NodeList.size());

	if (Q_stricmp (ArgGets(2).c_str(), "connect") == 0)
	{
		if (ent->Client.Respawn.LastNode)
			ConnectNode (ent->Client.Respawn.LastNode, NodeList.at(NodeList.size() - 1));
	}
	ent->Client.Respawn.LastNode = NodeList.at(NodeList.size() - 1);
}

void ConnectNode (CPathNode *Node1, CPathNode *Node2)
{
	if (!Node1 || !Node2)
		return;

	Node1->Children.push_back (Node2);

	if (Q_stricmp (ArgGets(4).c_str(), "one") == 0)
		Node2->Children.push_back (Node1);
}

size_t GetNodeIndex (CPathNode *Node)
{
	for (size_t i = 0; i < NodeList.size(); i++)
	{
		if (Node == NodeList[i])
			return i;
	}
	DebugPrintf ("OMG BAD\n");
	return 0;
}

#define NODE_VERSION 2
void SaveNodes ()
{
	size_t numNodes = 0, numSpecialNodes = 0;
	// Try to open the file
	std::cc_string FileName;

	FileName += "maps/nodes/";
	FileName += level.mapname;
	FileName += ".ccn";

	fileHandle_t f;
	FS_OpenFile (FileName.c_str(), &f, FS_MODE_WRITE_BINARY);

	if (!f)
		return;

	// Write the header
	int version = NODE_VERSION;
	FS_Write (&version, sizeof(int), f);
	size_t siz = NodeList.size();
	FS_Write (&siz, sizeof(uint32), f);

	numNodes = siz;
	// Write each node
	for (uint32 i = 0; i < NodeList.size(); i++)
	{
		FS_Write (NodeList[i]->Origin, sizeof(NodeList[i]->Origin), f);
		FS_Write (&NodeList[i]->Type, sizeof(NodeList[i]->Type), f);

		if (NodeList[i]->Type)
			numSpecialNodes++;

		if (NodeList[i]->Type == NODE_DOOR || NodeList[i]->Type == NODE_PLATFORM)
		{
			if (NodeList[i]->LinkedEntity)
			{
				int modelNum = atoi(NodeList[i]->LinkedEntity->gameEntity->model+1);
				FS_Write (&modelNum, sizeof(int), f);
			}
		}

		size_t num = NodeList[i]->Children.size();
		FS_Write (&num, sizeof(uint32), f);
		for (size_t s = 0; s < NodeList[i]->Children.size(); s++)
		{
			size_t ind = GetNodeIndex(NodeList[i]->Children[s]);
			FS_Write (&ind, sizeof(uint32), f);
		}
	}
	FS_CloseFile (f);

	DebugPrintf ("Saved %u (%u special) nodes\n", numNodes, numSpecialNodes);
}

void LinkModelNumberToNode (CPathNode *Node, int modelNum)
{
	char tempString[7];
	Q_snprintfz (tempString, sizeof(tempString), "*%i", modelNum);

	int i;
	edict_t *e;

	for (i=1, e=g_edicts+i; i < globals.numEdicts; i++,e++)
	{
		if (!e->inUse)
			continue;
		if (e->client)
			continue;
		if (e->Entity && (e->Entity->EntityFlags & ENT_MONSTER))
			continue;
		if (!e->model)
			continue;

		if (strcmp(e->model, tempString) == 0)
		{
			Node->LinkedEntity = e->Entity;
			return;
		}
	}
	DebugPrintf ("WARNING: Couldn't find linked model for node!\n");
}

void LoadNodes ()
{
	uint32 numNodes = 0, numSpecialNodes = 0;
	// Try to open the file
	std::cc_string FileName;

	FileName += "maps/nodes/";
	FileName += level.mapname;
	FileName += ".ccn";

	fileHandle_t f;
	FS_OpenFile (FileName.c_str(), &f, FS_MODE_READ_BINARY);

	if (!f)
		return;

	// Write the header
	int version;
	uint32 lastId;

	FS_Read (&version, sizeof(int), f);
	FS_Read (&lastId, sizeof(uint32), f);

	numNodes = lastId;

	if (version != NODE_VERSION)
		DebugPrintf ("Old version of nodes!\n");

	int **tempChildren;
	tempChildren = QNew (com_genericPool, 0) int*[lastId];

	// Read each node
	for (uint32 i = 0; i < lastId; i++)
	{
		ENodeType Type;
		vec3f Origin;
		if (version == 1)
		{
			uint32 nothing;
			FS_Read (&nothing, sizeof(uint32), f);
		}
		FS_Read (&Origin, sizeof(NodeList[i]->Origin), f);
		FS_Read (&Type, sizeof(NodeList[i]->Type), f);

		NodeList.push_back(QNew (com_levelPool, 0) CPathNode(Origin, Type));

		SpawnNodeEntity (NodeList[i]);

		if(Type)
			numSpecialNodes++;
		if (Type == NODE_DOOR || Type == NODE_PLATFORM)
		{
			int modelNum;
			FS_Read (&modelNum, sizeof(int), f);

			LinkModelNumberToNode (NodeList[i], modelNum);
		}

		uint32 num;
		FS_Read (&num, sizeof(uint32), f);

		tempChildren[i] = QNew (com_genericPool, 0) int[num+1];
		tempChildren[i][0] = num;
		/*for (size_t s = 0; s < num; s++)
		{
			int tempId;
			fread (&tempId, sizeof(int), 1, fp);

			NodeList[i]->Children.push_back (NodeList[tempId]);
		}*/
		for (size_t s = 0; s < num; s++)
			FS_Read (&tempChildren[i][s+1], sizeof(uint32), f);
	}
	FS_CloseFile (f);

	for (size_t i = 0; i < lastId; i++)
	{
		for (int z = 0; z < tempChildren[i][0]; z++)
			NodeList[i]->Children.push_back (NodeList[tempChildren[i][z+1]]);
		QDelete tempChildren[i];
	}

	QDelete[] tempChildren;
	DebugPrintf ("Loaded %u (%u special) nodes\n", numNodes, numSpecialNodes);
}

bool VecInFront (vec3f &angles, vec3f &origin1, vec3f &origin2)
{	
	vec3f forward;
	angles.ToVectors (&forward, NULL, NULL);
	vec3f vec = origin1 - origin2;
	vec.Normalize ();

	if (vec.Dot (forward) > 0.3)
		return true;
	return false;
}

CPathNode *GetClosestNodeTo (vec3f origin)
{
	CPathNode *Best = NULL;
	float bestDist = 9999999;
	vec3f sub;

	for (uint32 i = 0; i < NodeList.size(); i++)
	{
		CPathNode *Node = NodeList[i];

		sub = Node->Origin - origin;
		if (sub.Z > 64 || sub.Z < -64)
			continue;

		if (!Best)
		{
			bestDist = sub.Length();
			Best = Node;

			continue;
		}

		float temp = sub.Length();
		if (temp < bestDist)
		{
			Best = Node;
			bestDist = temp;
		}
	}
	return Best;
}

void Cmd_Node_f (CPlayerEntity *ent)
{
	vec3f origin = ent->State.GetOrigin();
	std::cc_string cmd = ArgGets(1);

	if (Q_stricmp (cmd.c_str(), "save") == 0)
		SaveNodes ();
	else if (Q_stricmp (cmd.c_str(), "load") == 0)
		LoadNodes ();
	else if (Q_stricmp (cmd.c_str(), "drop") == 0)
		AddNode (ent, origin);
	else if (Q_stricmp (cmd.c_str(), "clearlastnode") == 0)
		ent->Client.Respawn.LastNode = NULL;
	else if (Q_stricmp (cmd.c_str(), "connect") == 0)
	{
		uint32 firstId = ArgGeti(2);
		uint32 secondId = ArgGeti(3);

		if (firstId >= NodeList.size())
		{
			ent->PrintToClient (PRINT_HIGH, "Node %i doesn't exist!\n", firstId);
			return;
		}
		if (secondId >= NodeList.size())
		{
			ent->PrintToClient (PRINT_HIGH, "Node %i doesn't exist!\n", secondId);
			return;
		}

		ent->PrintToClient (PRINT_HIGH, "Connecting nodes %i and %i...\n", firstId, secondId);
		ConnectNode (NodeList[firstId], NodeList[secondId]);
	}
	else if (Q_stricmp (cmd.c_str(), "clearstate") == 0)
	{
		for (uint32 i = 0; i < NodeList.size(); i++)
			NodeList[i]->Ent->State.GetModelIndex() = ModelIndex("models/objects/grenade2/tris.md2");
	}
	else if (Q_stricmp (cmd.c_str(), "settype") == 0)
	{
		if (ArgCount() < 3 || (uint32)ArgGeti(2) >= NodeList.size())
			return;

		CPathNode *Node = NodeList[ArgGeti(2)];

		if (Q_stricmp (ArgGets(3).c_str(), "door") == 0)
			Node->Type = NODE_DOOR;
		else if (Q_stricmp (ArgGets(3).c_str(), "plat") == 0)
			Node->Type = NODE_PLATFORM;
		else if (Q_stricmp (ArgGets(3).c_str(), "jump") == 0)
			Node->Type = NODE_JUMP;

		CheckNodeFlags (Node);
	}
	else if (Q_stricmp (cmd.c_str(), "linkwith") == 0)
	{
		if (ArgCount() < 3 || (uint32)ArgGeti(2) >= NodeList.size())
			return;

		CPathNode *Node = NodeList[ArgGeti(2)];

		vec3f forward;
		ent->Client.ViewAngle.ToVectors (&forward, NULL, NULL);
		vec3f end = origin.MultiplyAngles (8192, forward);

		CTrace trace (origin, end, ent->gameEntity, CONTENTS_MASK_ALL);

		if (trace.ent && trace.ent->model && trace.ent->model[0] == '*')
		{
			Node->LinkedEntity = trace.Ent;
			DebugPrintf ("Linked %u with %s\n", GetNodeIndex(Node), trace.ent->classname);
		}
	}
	else if (Q_stricmp (cmd.c_str(), "monstergoal") == 0)
	{
		vec3f forward;
		ent->Client.ViewAngle.ToVectors (&forward, NULL, NULL);
		vec3f end = origin.MultiplyAngles (8192, forward);

		CTrace trace (origin, end, ent->gameEntity, CONTENTS_MASK_ALL);

		if (trace.Ent && (trace.Ent->EntityFlags & ENT_MONSTER))
		{
			CMonster *Monster = (entity_cast<CMonsterEntity>(trace.Ent))->Monster;
			if (Q_stricmp (ArgGets(2).c_str(), "closest") == 0)
				Monster->P_CurrentNode = GetClosestNodeTo(Monster->Entity->State.GetOrigin());
			else
				Monster->P_CurrentNode = NodeList[ArgGeti(2)];
			Monster->P_CurrentGoalNode = NodeList[ArgGeti(3)];
			Monster->FoundPath ();
		}
	}
	else if (Q_stricmp (cmd.c_str(), "kill") == 0)
	{
		if (ArgCount() < 3 || (uint32)ArgGeti(2) >= NodeList.size())
			return;

		uint32 node = ArgGeti(2);
		CPathNode *Node = NodeList[node];

		Node->Children.clear();

		// Cut off all connections
		for (uint32 i = 0; i < NodeList.size(); i++)
		{
			for (uint32 z = 0; z < NodeList[i]->Children.size(); z++)
			{
				if (NodeList[i]->Children[z] == Node)
				{
					NodeList[i]->Children.erase(NodeList[i]->Children.begin() + z);
					z = 0;
				}
			}
		}

		// Delete node
		Node->Ent->Free ();
		QDelete Node;
		NodeList.erase(NodeList.begin() + node);
	}
	else if (Q_stricmp (cmd.c_str(), "move") == 0)
	{
		if (ArgCount() < 3 || (uint32)ArgGeti(2) >= NodeList.size())
			return;

		CPathNode *Node = NodeList[ArgGeti(2)];

		vec3f v (ArgGetf(3), ArgGetf(4), ArgGetf(5));

		Node->Origin += v;
		Node->Ent->State.GetOrigin() = (Node->Ent->State.GetOrigin() + v);
		Node->Ent->Link ();
	}
}

CPath *CreatePath (CPathNode *Start, CPathNode *End)
{
	CPath *New = QNew (com_levelPool, 0) CPath(Start, End);

	if (New->Incomplete)
	{
		QDelete New;
		return NULL;
	}
	else
		return New;
}

// A table of saved paths, to speed things up.
void SavePathTable ()
{
	// Try to open the file
	std::cc_string FileName;

	FileName += "maps/nodes/";
	FileName += level.mapname;
	FileName += ".cnt";

	fileHandle_t f;
	FS_OpenFile (FileName.c_str(), &f, FS_MODE_WRITE_BINARY);
	if (!f)
		return;

	DebugPrintf ("Saving node helper table...\n");

	int count = 0;
	for (int i = 0; i < MAX_SAVED_PATHS; i++)
	{
		for (int z = 0; z < MAX_SAVED_PATHS; z++)
		{
			if (SavedPaths[i].ToEnd[z])
				count++;
		}
	}
	FS_Write (&count, sizeof(int), f);

	for (int i = 0; i < MAX_SAVED_PATHS; i++)
	{
		for (int z = 0; z < MAX_SAVED_PATHS; z++)
		{
			if (SavedPaths[i].ToEnd[z])
			{
				FS_Write (&i, sizeof(int), f);
				FS_Write (&z, sizeof(int), f);
				SavedPaths[i].ToEnd[z]->Save(f);
			}
		}
	}
	FS_CloseFile (f);
}

void LoadPathTable ()
{
	// Try to open the file
	std::cc_string FileName;

	FileName += "maps/nodes/";
	FileName += level.mapname;
	FileName += ".cnt";

	fileHandle_t f;
	FS_OpenFile (FileName.c_str(), &f, FS_MODE_READ_BINARY);

	if (!f)
		return;

	DebugPrintf ("Loading node helper table...\n");

	int count;
	FS_Read (&count, sizeof(int), f);

	for (int i = 0; i < count; i++)
	{
		int indexI, indexZ;

		FS_Read (&indexI, sizeof(int), f);
		FS_Read (&indexZ, sizeof(int), f);

		SavedPaths[indexI].ToEnd[indexZ] = QNew (com_levelPool, 0) CPath();
		SavedPaths[indexI].ToEnd[indexZ]->Load (f);
	}

	FS_CloseFile (f);
}

CPath *GetPath (CPathNode *Start, CPathNode *End)
{
	size_t StartIndex = GetNodeIndex(Start);
	size_t EndIndex = GetNodeIndex(End);

	CPath *Path = SavedPaths[StartIndex].ToEnd[EndIndex];

	if (!Path)
	{
		SavedPaths[StartIndex].ToEnd[EndIndex] = CreatePath(Start, End);
		return SavedPaths[StartIndex].ToEnd[EndIndex];
	}
	return Path;
}
#endif