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
// mathlib.cpp
// Required initialization of MathLib.h and included files
//

#include "../source/cc_local.h"

vec2f vec2fOrigin (0, 0);
vec3f vec3fOrigin (0, 0, 0);

void vec3f::ProjectOnPlane (const vec3f &point, const vec3f &normal)
{
	const float invDenom = 1.0f / (normal | normal);
	const float dot = (normal | point) * invDenom;
	*this = point - dot * (normal * invDenom);
};