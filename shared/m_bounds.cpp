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
// m_bounds.c
//

#include "../source/cc_options.h"
#include "shared.h"

#ifndef GAME_IS_BEING_COMPILED_NOT_ENGINE_GO_AWAY
/*
=============================================================================

	2D

=============================================================================
*/

/*
===============
AddBoundsTo2DBounds
===============
*/
void AddBoundsTo2DBounds (vec2_t inMins, vec2_t inMaxs, vec2_t outMins, vec2_t outMaxs)
{
	// X Vector
	if (inMins[0] < outMins[0])
		outMins[0] = inMins[0];
	if (inMaxs[0] > outMaxs[0])
		outMaxs[0] = inMaxs[0];

	// Y Vector
	if (inMins[1] < outMins[1])
		outMins[1] = inMins[1];
	if (inMaxs[1] > outMaxs[1])
		outMaxs[1] = inMaxs[1];
}


/*
===============
AddPointTo2DBounds
===============
*/
void AddPointTo2DBounds (vec2_t v, vec2_t mins, vec2_t maxs)
{
	// X Vector
	if (v[0] < mins[0])
		mins[0] = v[0];
	if (v[0] > maxs[0])
		maxs[0] = v[0];

	// Y Vector
	if (v[1] < mins[1])
		mins[1] = v[1];
	if (v[1] > maxs[1])
		maxs[1] = v[1];
}


/*
===============
Clear2DBounds
===============
*/
void Clear2DBounds (vec2_t mins, vec2_t maxs)
{
	mins[0] = mins[1] = 999999;
	maxs[0] = maxs[1] = -999999;
}
#endif

/*
=============================================================================

	3D

=============================================================================
*/

/*
===============
AddPointToBounds
===============
*/
void AddPointToBounds (vec3f v, vec3f &mins, vec3f &maxs)
{
	// X Vector
	if (v.X < mins.X)
		mins.X = v.X;
	if (v.X > maxs.X)
		maxs.X = v.X;

	// Y Vector
	if (v.Y < mins.Y)
		mins.Y = v.Y;
	if (v.Y > maxs.Y)
		maxs.Y = v.Y;

	// Z Vector
	if (v.Z < mins.Z)
		mins.Z = v.Z;
	if (v.Z > maxs.Z)
		maxs.Z = v.Z;
}


/*
=================
BoundsAndSphereIntersect
=================
*/
bool BoundsAndSphereIntersect (const vec3f &mins, const vec3f &maxs, const vec3f &centre, float radius)
{
	return (mins <= (centre+radius) && maxs >= (centre+radius));
}


/*
=================
BoundsIntersect
=================
*/
bool BoundsIntersect (const vec3f &mins1, const vec3f &maxs1, const vec3f &mins2, const vec3f &maxs2)
{
	return (mins1 <= maxs2 && maxs1 >= mins2);
}


/*
===============
ClearBounds
===============
*/
void ClearBounds (vec3f &mins, vec3f &maxs)
{
	mins = maxs = 999999;
}


/*
===============
MinMins
===============
*/
void MinMins (const vec3f &a, const vec3f &b, vec3f &out)
{
	out = Min<vec3f> (a, b);
}


/*
===============
MaxMaxs
===============
*/
void MaxMaxs (const vec3f &a, const vec3f &b, vec3f &out)
{
	out = Max<vec3f> (a, b);
}


/*
=================
RadiusFromBounds
=================
*/
float RadiusFromBounds (const vec3f &mins, const vec3f &maxs)
{
	return (mins.GetAbs() > maxs.GetAbs() ? mins.GetAbs() : maxs.GetAbs()).Length();
}
