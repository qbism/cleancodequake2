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
// m_plane.c
//

#include "../source/cc_options.h"
#include "shared.h"

/*
==================
BoxOnPlaneSide

Returns 1, 2, or 1 + 2
==================
*/
sint32 BoxOnPlaneSide(const vec3f &mins, const vec3f &maxs, const plane_t *plane)
{
	// Fast axial cases
	if (plane->type < 3)
	{
		if (plane->dist <= mins[plane->type])
			return 1;
		if (plane->dist >= maxs[plane->type])
			return 2;
		return 3;
	}
	
	static const struct
	{
		bool IsMins[3];
	} SignBitsRemap [] =
	{
		{ false, false, false },
		{ true, false, false },
		{ false, true, false },
		{ true, true, false },
		{ false, false, true },
		{ true, false, true },
		{ false, true, true },
		{ true, true, true }
	};

	sint32 sides = 0;
	if ((plane->normal.X * (SignBitsRemap[plane->signBits].IsMins[0] ? mins.X : maxs.X) +
		plane->normal.Y * (SignBitsRemap[plane->signBits].IsMins[1] ? mins.Y : maxs.Y) +
		plane->normal.Z * (SignBitsRemap[plane->signBits].IsMins[2] ? mins.Z : maxs.Z)) >= plane->dist)
		sides = 1;
	if ((plane->normal.X * (!SignBitsRemap[plane->signBits].IsMins[0] ? mins.X : maxs.X) +
		plane->normal.Y * (!SignBitsRemap[plane->signBits].IsMins[1] ? mins.Y : maxs.Y) +
		plane->normal.Z * (!SignBitsRemap[plane->signBits].IsMins[2] ? mins.Z : maxs.Z)) < plane->dist)
		sides |= 2;

	return sides;
}


/*
=================
PlaneTypeForNormal
=================
*/
EPlaneInfo	PlaneTypeForNormal(const vec3f &normal)
{
	// NOTE: should these have an epsilon around 1.0?		
	if (normal.X >= 1.0)
		return PLANE_X;
	if (normal.Y >= 1.0)
		return PLANE_Y;
	if (normal.Z >= 1.0)
		return PLANE_Z;

	const vec3f a = normal.GetAbs();

	if (a.X >= a.Y && a.X >= a.Z)
		return PLANE_ANYX;
	if (a.Y >= a.X && a.Y >= a.Z)
		return PLANE_ANYY;
	return PLANE_ANYZ;
}


/*
=================
CategorizePlane

A slightly simplier version of SignbitsForPlane and PlaneTypeForNormal
=================
*/
void CategorizePlane(plane_t *plane)
{
	plane->signBits = 0;
	plane->type = PLANE_NON_AXIAL;

	for (uint8 i = 0; i < 3; i++)
	{
		if (plane->normal[i] < 0)
			plane->signBits |= BIT(i);
		if (plane->normal[i] == 1.0f)
			plane->type = i;
	}
}


/*
=================
PlaneFromPoints
=================
*/
void PlaneFromPoints(const vec3f verts[3], plane_t *plane)
{	
	plane->normal = ((verts[2] - verts[0]) ^ (verts[1] - verts[0])).GetNormalized();
	plane->dist = verts[0] | plane->normal;
}

#define PLANE_NORMAL_EPSILON	0.00001f
#define PLANE_DIST_EPSILON		0.01f

/*
=================
ComparePlanes
=================
*/
bool ComparePlanes(const vec3f &p1normal, const float p1dist, const vec3f &p2normal, const float p2dist)
{
	//if (Q_fabs (p1normal.X - p2normal.X) < PLANE_NORMAL_EPSILON
	//&& Q_fabs (p1normal.Y - p2normal.Y) < PLANE_NORMAL_EPSILON
	//&& Q_fabs (p1normal.Z - p2normal.Z) < PLANE_NORMAL_EPSILON

	if ((p1normal - p2normal).GetAbs() < PLANE_NORMAL_EPSILON
	&& Q_fabs (p1dist - p2dist) < PLANE_DIST_EPSILON)
		return true;

	return false;
}


/*
==============
SnapVector
==============
*/
void SnapVector(vec3f &normal)
{
	for (uint8 i = 0; i < 3; i++)
	{
		if (Q_fabs (normal[i] - 1) < PLANE_NORMAL_EPSILON)
		{
			normal.Clear();
			normal[i] = 1;
			break;
		}
		if (Q_fabs (normal[i] - -1) < PLANE_NORMAL_EPSILON)
		{
			normal.Clear();
			normal[i] = -1;
			break;
		}
	}
}


/*
===============
ProjectPointOnPlane
===============
*/
void ProjectPointOnPlane(vec3f &dst, const vec3f &point, const vec3f &normal)
{
	const float invDenom = 1.0f / (normal | normal);
	const float dot = (normal | point) * invDenom;
	dst = point - dot * (normal * invDenom);
}


/*
===============
SignbitsForPlane
===============
*/
sint32 SignbitsForPlane(const plane_t *plane)
{
	// For fast box on planeside test
	sint32 bits = 0;
	if (plane->normal.X < 0)
		bits |= 1 << 0;
	if (plane->normal.Y < 0)
		bits |= 1 << 1;
	if (plane->normal.Z < 0)
		bits |= 1 << 2;

	return bits;
}

