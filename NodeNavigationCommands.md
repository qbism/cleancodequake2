# Introduction #

Before we start, I have to say that this page is the documentation that teaches you how to create node tables,
used on Clean Code, for your maps.


# Commands #
Those are the commands for the lasted revision. They have been tested, and they should work.

node\_debug #			- 1 or 0. Makes visible the Nodes on the map.

node load				- Loads the table of nodes for the existing map. NOTE: They must be the same name to be loaded.

node save				- Saves the node table with the Map Name.

node drop [connect](connect.md)		- Drops a node at your origin. Optional connect connects the last dropped node with the one you drop with the command.

node drop connect			- Drops a node and connect it with the one placed before of it.

node connect # # ["one"]	- Connects two nodes bi-directional, unless "one" is into the command. Eg: node connect 7 9 will connect 7 -> 9 and 9 -> 7, but node connect 7 9 one will only connect 7->9.

node testpath # #			- Test a path.

node clearstate			- Cleans the grenade models from testpath.

node settype # type		- This is for setting the types for the nodes. Refer to "Types of Nodes".

node linkwith #			- Links the node number with the entity you're aiming at (currently only brush models, like doors and platforms).

node monstergoal # #		- Tells the monster you are aiming at to go from node  X to node Y. NOTE: start can be "closest" to make him start at the closest node.

node kill #				- Destroys/removes the node.

node move # x y z			- Moves a node, relative to it's origin (5 0 0 moves it 5 units on the x axis)


# Types of Nodes #

As of now, the following types exist:
  * "door" = A brush model that has states, will wait at node before this one for the state to reach top before moving
  * "platform" = A func\_plat. This node includes safety measures so that monsters don't get stuck underneath it.
  * "jump" = A jump node. Monsters will jump from this node to the node it's connected to. Best to be one-way.

# Distributing and Terms #

Nodes are loaded automatically from 42 and newer releases. If you are going to distribute your maps,
you need to give the map + the node table which are created in a sub-folder of the maps folder (named /nodes/),
file named the same as the map but with a different extension. They are all made in-game.

If you have a use for the nodes, you may use the node format found in cc\_pathfinding for your own
uses.