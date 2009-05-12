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
// cc_pathfinding.h
// Pathfinding algorithm for monsters
//

#include <vector>

enum ENodeType
{
	NODE_REGULAR,

	NODE_DOOR,
	NODE_PLATFORM
};

// One node
class CPathNode
{
public:
	uint32						NodeId; // The node's id
	ENodeType					Type; // Type of node
	vec3_t						Origin; // Node's location
	edict_t						*Ent; // Node entity

	std::vector<CPathNode*>		Children;	// Children (connected nodes, basically)
	// Testing 2
	uint32						G, F, H;
	CPathNode					*Parent;

	CPathNode (vec3_t Origin, ENodeType Type, uint32 NodeId) :
	Type(Type),
	NodeId(NodeId)
	{
		Vec3Copy (Origin, this->Origin);
	};

	void AddChild (CPathNode *Node)
	{
		Children.push_back(Node);
	};

	inline uint32 GetNodeWeight (CPathNode *NodeTwo)
	{
		vec3_t temp;
		Vec3Subtract (Origin, NodeTwo->Origin, temp);
		return (uint32)Vec3Length(temp);
	};
};

extern std::vector<CPathNode*>		Closed; // Nodes that have already been crossed
// An actual "path"
class CPath
{
public:
	// Basically, keep popping back once we reach the end node (backwards)
	CPathNode					*Start, *End;
	std::vector<CPathNode*>		Path;
	uint32						Weight;
	uint32						NumNodes;

	CPath ();
	CPath (CPathNode *Start, CPathNode *End);

	bool NodeIsClosed (CPathNode *Node);
	void RemoveFromClosed (CPathNode *Node);
	void AddNodeToClosed (CPathNode *Node);

	bool NodeIsOpen (CPathNode *Node);
	void RemoveFromOpen (CPathNode *Node);
	void AddNodeToOpen (CPathNode *Node);

	uint32	DistToGoal (CPathNode *Node);

	// Returns the closest node to End from Node (by searching Children)
	CPathNode *GetShortestConnector (CPathNode *Node, uint32 &weight);
	void CreatePath ();
};

void InitNodes ();
void RunNodes ();
void RunPlayerNodes (edict_t *ent);
