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
// cc_arg.cpp
// Wrapper for the argv/s/c system
//

#include "cc_local.h"

#define MAX_ARG_CONCAT	256
#define MAX_ARG		64

char	*nullArg = "";

uint8				numArgv;
std::cc_string		argvConcatString;
std::cc_string		argvStringArray[MAX_ARG];
int					argvIntegerArray[MAX_ARG];
float				argvFloatArray[MAX_ARG];

void SetupArg ()
{
	numArgv = 0;

	for (uint8 i = 0; i < MAX_ARG; i++)
		argvStringArray[i].clear();

	memset (argvIntegerArray, 0, sizeof(argvIntegerArray));
	memset (argvFloatArray, 0, sizeof(argvFloatArray));
	argvConcatString.clear();
}

// Called at the beginning of an arg session
void InitArg ()
{
_CC_DISABLE_DEPRECATION
	if (numArgv)
	{
		DebugPrintf ("CleanCode Warning: InitArg() called before previous arg session completed!\n");
		EndArg ();
	}

	numArgv = gi.argc();

	if (numArgv == 0)
		return;

	argvConcatString = gi.args();

	for (uint8 i = 0; i < numArgv; i++)
	{
		argvStringArray[i] = gi.argv(i);
		argvFloatArray[i] = atof (argvStringArray[i].c_str());
		argvIntegerArray[i] = (int)argvFloatArray[i];
	}
_CC_ENABLE_DEPRECATION
}

// Called at the end of an arg session
void EndArg ()
{
	for (int i = 0; i < numArgv; i++)
		argvStringArray[i].clear();

	SetupArg();
}

// Actual functions
uint8 ArgCount ()
{
	return numArgv;
}

std::cc_string ArgGetConcatenatedString ()
{
	return argvConcatString;
}

std::cc_string ArgGets (int Index)
{
	if (Index >= MAX_ARG)
	{
		assert (0);
		return NULL;
	}

	return (Index >= numArgv) ? nullArg : argvStringArray[Index];
}

int ArgGeti (int Index)
{
	if (Index >= MAX_ARG)
	{
		assert (0);
		return NULL;
	}

	return (Index >= numArgv) ? 0 : argvIntegerArray[Index];
}

float ArgGetf (int Index)
{
	if (Index >= MAX_ARG)
	{
		assert (0);
		return NULL;
	}

	return (Index >= numArgv) ? 0 : argvFloatArray[Index];
}