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
std::vector<std::cc_string, std::generic_allocator<std::cc_string> >		argvStringArray;
std::vector<sint32, std::generic_allocator<sint32> >						argvIntegerArray;
std::vector<float, std::generic_allocator<float> >							argvFloatArray;

void SetupArg ()
{
	numArgv = 0;

	argvStringArray.clear();
	argvIntegerArray.clear();
	argvFloatArray.clear();
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
		argvStringArray.push_back (gi.argv(i));
		argvFloatArray.push_back (atof (argvStringArray[i].c_str()));
		argvIntegerArray.push_back ((sint32)argvFloatArray[i]);
	}
_CC_ENABLE_DEPRECATION
}

// Called at the end of an arg session
void EndArg ()
{
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

std::cc_string &ArgGets (uint32 Index)
{
	if (Index >= argvStringArray.size())
	{
		_CC_ASSERT_EXPR (0, "ArgGets(n) index out of bounds");
		return argvStringArray[0];
	}

	return argvStringArray[Index];
}

sint32 &ArgGeti (uint32 Index)
{
	if (Index >= argvStringArray.size())
	{
		_CC_ASSERT_EXPR (0, "ArgGeti(n) index out of bounds");
		return argvIntegerArray[0];
	}

	return argvIntegerArray[Index];
}

float &ArgGetf (uint32 Index)
{
	if (Index >= argvStringArray.size())
	{
		_CC_ASSERT_EXPR (0, "ArgGetf(n) index out of bounds");
		return argvFloatArray[0];
	}

	return argvFloatArray[Index];
}