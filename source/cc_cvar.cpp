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
// cc_cvar.cpp
// A small wrapper for cvars that make managing and using cvars a bit easier.
//

#include "cc_local.h"

_CC_DISABLE_DEPRECATION

CCvar::CCvar () :
	cVar(NULL),
	mainValue(NULL),
	floatVal(0),
	intVal(0)
{
}
CCvar::CCvar (const char *cvarName, const char *defaultValue, sint32 flags) :
	cVar(NULL),
	mainValue(NULL),
	floatVal(0),
	intVal(0)
{
	Register(cvarName, defaultValue, flags);
}
CCvar::CCvar (const char *cvarName, sint32 defaultValue, sint32 flags) :
	cVar(NULL),
	mainValue(NULL),
	floatVal(0),
	intVal(0)
{
	Register(cvarName, defaultValue, flags);
}
CCvar::CCvar (const char *cvarName, float defaultValue, sint32 flags) :
	cVar(NULL),
	mainValue(NULL),
	floatVal(0),
	intVal(0)
{
	Register(cvarName, defaultValue, flags);
}

void CCvar::Update()
{
	mainValue = cVar->string;
	floatVal = cVar->floatVal;
	intVal = floatVal;
}

void CCvar::Register(const char *cvarName, const char *defaultValue, sint32 flags)
{
	cVar = gi.cvar ((char*)cvarName, (char*)defaultValue, flags);

	mainValue = cVar->string;
	floatVal = cVar->floatVal;
	intVal = floatVal;
}

#include <sstream>
static std::cc_stringstream str;
void CCvar::Register(const char *cvarName, float defaultValue, sint32 flags)
{
	str.str("");
	str << defaultValue;

	cVar = gi.cvar ((char*)cvarName, (char*)str.str().c_str(), flags);

	mainValue = cVar->string;
	floatVal = cVar->floatVal;
	intVal = floatVal;
}

void CCvar::Register(const char *cvarName, sint32 defaultValue, sint32 flags)
{
	str.str("");
	str << defaultValue;

	cVar = gi.cvar ((char*)cvarName, (char*)str.str().c_str(), flags);

	mainValue = cVar->string;
	floatVal = cVar->floatVal;
	intVal = floatVal;
}

void CCvar::Set (const char *value, bool Force)
{
	if (!Force)
		cVar = gi.cvar_set (cVar->name, (char*)value);
	else
		cVar = gi.cvar_forceset (cVar->name, (char*)value);
}

void CCvar::Set (float value, bool Force)
{
	str.str("");
	str << value;

	if (!Force)
		cVar = gi.cvar_set (cVar->name, (char*)str.str().c_str());
	else
		cVar = gi.cvar_forceset (cVar->name, (char*)str.str().c_str());
}

void CCvar::Set (sint32 value, bool Force)
{
	str.str("");
	str << value;

	if (!Force)
		cVar = gi.cvar_set (cVar->name, (char*)str.str().c_str());
	else
		cVar = gi.cvar_forceset (cVar->name, (char*)str.str().c_str());
}

float CCvar::Float ()
{
	Update ();
	return floatVal;
}

sint32 CCvar::Integer ()
{
	Update ();
	return intVal;
}

char *CCvar::String ()
{
	Update ();
	return mainValue;
}

bool CCvar::Boolean (bool MustBeOne)
{
	Update ();
	return (MustBeOne ? (Integer() == 1) : (Float() != 0));
}

bool CCvar::Modified ()
{
	bool modified = (cVar->modified == 1);

	cVar->modified = 0;
	return modified;
}

_CC_ENABLE_DEPRECATION
