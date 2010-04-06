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
// mathlib.c
//

#include "../source/cc_local.h"

vec2f vec2fOrigin (0, 0);
vec3f vec3fOrigin (0, 0, 0);

/*
=============================================================================

	MATHLIB

=============================================================================
*/

vec3f m_byteDirs[NUMVERTEXNORMALS] = {
	vec3f(-0.525731f,	0.000000f,	0.850651f),		vec3f(-0.442863f,	0.238856f,	0.864188f),		vec3f(-0.295242f,	0.000000f,	0.955423f),
	vec3f(-0.309017f,	0.500000f,	0.809017f),		vec3f(-0.162460f,	0.262866f,	0.951056f),		vec3f(0.000000f,		0.000000f,	1.000000f),
	vec3f(0.000000f,		0.850651f,	0.525731f),		vec3f(-0.147621f,	0.716567f,	0.681718f),		vec3f(0.147621f,		0.716567f,	0.681718f),
	vec3f(0.000000f,		0.525731f,	0.850651f),		vec3f(0.309017f,		0.500000f,	0.809017f),		vec3f(0.525731f,		0.000000f,	0.850651f),
	vec3f(0.295242f,		0.000000f,	0.955423f),		vec3f(0.442863f,		0.238856f,	0.864188f),		vec3f(0.162460f,		0.262866f,	0.951056f),
	vec3f(-0.681718f,	0.147621f,	0.716567f),		vec3f(-0.809017f,	0.309017f,	0.500000f),		vec3f(-0.587785f,	0.425325f,	0.688191f),
	vec3f(-0.850651f,	0.525731f,	0.000000f),		vec3f(-0.864188f,	0.442863f,	0.238856f),		vec3f(-0.716567f,	0.681718f,	0.147621f),
	vec3f(-0.688191f,	0.587785f,	0.425325f),		vec3f(-0.500000f,	0.809017f,	0.309017f),		vec3f(-0.238856f,	0.864188f,	0.442863f),
	vec3f(-0.425325f,	0.688191f,	0.587785f),		vec3f(-0.716567f,	0.681718f,	-0.147621f),	vec3f(-0.500000f,	0.809017f,	-0.309017f),
	vec3f(-0.525731f,	0.850651f,	0.000000f),		vec3f(0.000000f,		0.850651f,	-0.525731f),	vec3f(-0.238856f,	0.864188f,	-0.442863f),
	vec3f(0.000000f,		0.955423f,	-0.295242f),	vec3f(-0.262866f,	0.951056f,	-0.162460f),	vec3f(0.000000f,		1.000000f,	0.000000f),
	vec3f(0.000000f,		0.955423f,	0.295242f),		vec3f(-0.262866f,	0.951056f,	0.162460f),		vec3f(0.238856f,		0.864188f,	0.442863f),
	vec3f(0.262866f,		0.951056f,	0.162460f),		vec3f(0.500000f,		0.809017f,	0.309017f),		vec3f(0.238856f,		0.864188f,	-0.442863f),
	vec3f(0.262866f,		0.951056f,	-0.162460f),	vec3f(0.500000f,		0.809017f,	-0.309017f),	vec3f(0.850651f,		0.525731f,	0.000000f),
	vec3f(0.716567f,		0.681718f,	0.147621f),		vec3f(0.716567f,		0.681718f,	-0.147621f),	vec3f(0.525731f,		0.850651f,	0.000000f),
	vec3f(0.425325f,		0.688191f,	0.587785f),		vec3f(0.864188f,		0.442863f,	0.238856f),		vec3f(0.688191f,		0.587785f,	0.425325f),
	vec3f(0.809017f,		0.309017f,	0.500000f),		vec3f(0.681718f,		0.147621f,	0.716567f),		vec3f(0.587785f,		0.425325f,	0.688191f),
	vec3f(0.955423f,		0.295242f,	0.000000f),		vec3f(1.000000f,		0.000000f,	0.000000f),		vec3f(0.951056f,		0.162460f,	0.262866f),
	vec3f(0.850651f,		-0.525731f,	0.000000f),		vec3f(0.955423f,		-0.295242f,	0.000000f),		vec3f(0.864188f,		-0.442863f,	0.238856f),
	vec3f(0.951056f,		-0.162460f,	0.262866f),		vec3f(0.809017f,		-0.309017f,	0.500000f),		vec3f(0.681718f,		-0.147621f,	0.716567f),
	vec3f(0.850651f,		0.000000f,	0.525731f),		vec3f(0.864188f,		0.442863f,	-0.238856f),	vec3f(0.809017f,		0.309017f,	-0.500000f),
	vec3f(0.951056f,		0.162460f,	-0.262866f),	vec3f(0.525731f,		0.000000f,	-0.850651f),	vec3f(0.681718f,		0.147621f,	-0.716567f),
	vec3f(0.681718f,		-0.147621f,	-0.716567f),	vec3f(0.850651f,		0.000000f,	-0.525731f),	vec3f(0.809017f,		-0.309017f,	-0.500000f),
	vec3f(0.864188f,		-0.442863f,	-0.238856f),	vec3f(0.951056f,		-0.162460f,	-0.262866f),	vec3f(0.147621f,		0.716567f,	-0.681718f),
	vec3f(0.309017f,		0.500000f,	-0.809017f),	vec3f(0.425325f,		0.688191f,	-0.587785f),	vec3f(0.442863f,		0.238856f,	-0.864188f),
	vec3f(0.587785f,		0.425325f,	-0.688191f),	vec3f(0.688191f,		0.587785f,	-0.425325f),	vec3f(-0.147621f,	0.716567f,	-0.681718f),
	vec3f(-0.309017f,	0.500000f,	-0.809017f),	vec3f(0.000000f,		0.525731f,	-0.850651f),	vec3f(-0.525731f,	0.000000f,	-0.850651f),
	vec3f(-0.442863f,	0.238856f,	-0.864188f),	vec3f(-0.295242f,	0.000000f,	-0.955423f),	vec3f(-0.162460f,	0.262866f,	-0.951056f),
	vec3f(0.000000f,		0.000000f,	-1.000000f),	vec3f(0.295242f,		0.000000f,	-0.955423f),	vec3f(0.162460f,		0.262866f,	-0.951056f),
	vec3f(-0.442863f,	-0.238856f,	-0.864188f),	vec3f(-0.309017f,	-0.500000f,	-0.809017f),	vec3f(-0.162460f,	-0.262866f,	-0.951056f),
	vec3f(0.000000f,		-0.850651f,	-0.525731f),	vec3f(-0.147621f,	-0.716567f,	-0.681718f),	vec3f(0.147621f,		-0.716567f,	-0.681718f),
	vec3f(0.000000f,		-0.525731f,	-0.850651f),	vec3f(0.309017f,		-0.500000f,	-0.809017f),	vec3f(0.442863f,		-0.238856f,	-0.864188f),
	vec3f(0.162460f,		-0.262866f,	-0.951056f),	vec3f(0.238856f,		-0.864188f,	-0.442863f),	vec3f(0.500000f,		-0.809017f,	-0.309017f),
	vec3f(0.425325f,		-0.688191f,	-0.587785f),	vec3f(0.716567f,		-0.681718f,	-0.147621f),	vec3f(0.688191f,		-0.587785f,	-0.425325f),
	vec3f(0.587785f,		-0.425325f,	-0.688191f),	vec3f(0.000000f,		-0.955423f,	-0.295242f),	vec3f(0.000000f,		-1.000000f,	0.000000f),
	vec3f(0.262866f,		-0.951056f,	-0.162460f),	vec3f(0.000000f,		-0.850651f,	0.525731f),		vec3f(0.000000f,		-0.955423f,	0.295242f),
	vec3f(0.238856f,		-0.864188f,	0.442863f),		vec3f(0.262866f,		-0.951056f,	0.162460f),		vec3f(0.500000f,		-0.809017f,	0.309017f),
	vec3f(0.716567f,		-0.681718f,	0.147621f),		vec3f(0.525731f,		-0.850651f,	0.000000f),		vec3f(-0.238856f,	-0.864188f,	-0.442863f),
	vec3f(-0.500000f,	-0.809017f,	-0.309017f),	vec3f(-0.262866f,	-0.951056f,	-0.162460f),	vec3f(-0.850651f,	-0.525731f,	0.000000f),
	vec3f(-0.716567f,	-0.681718f,	-0.147621f),	vec3f(-0.716567f,	-0.681718f,	0.147621f),		vec3f(-0.525731f,	-0.850651f,	0.000000f),
	vec3f(-0.500000f,	-0.809017f,	0.309017f),		vec3f(-0.238856f,	-0.864188f,	0.442863f),		vec3f(-0.262866f,	-0.951056f,	0.162460f),
	vec3f(-0.864188f,	-0.442863f,	0.238856f),		vec3f(-0.809017f,	-0.309017f,	0.500000f),		vec3f(-0.688191f,	-0.587785f,	0.425325f),
	vec3f(-0.681718f,	-0.147621f,	0.716567f),		vec3f(-0.442863f,	-0.238856f,	0.864188f),		vec3f(-0.587785f,	-0.425325f,	0.688191f),
	vec3f(-0.309017f,	-0.500000f,	0.809017f),		vec3f(-0.147621f,	-0.716567f,	0.681718f),		vec3f(-0.425325f,	-0.688191f,	0.587785f),
	vec3f(-0.162460f,	-0.262866f,	0.951056f),		vec3f(0.442863f,		-0.238856f,	0.864188f),		vec3f(0.162460f,		-0.262866f,	0.951056f),
	vec3f(0.309017f,		-0.500000f,	0.809017f),		vec3f(0.147621f,		-0.716567f,	0.681718f),		vec3f(0.000000f,		-0.525731f,	0.850651f),
	vec3f(0.425325f,		-0.688191f,	0.587785f),		vec3f(0.587785f,		-0.425325f,	0.688191f),		vec3f(0.688191f,		-0.587785f,	0.425325f),
	vec3f(-0.955423f,	0.295242f,	0.000000f),		vec3f(-0.951056f,	0.162460f,	0.262866f),		vec3f(-1.000000f,	0.000000f,	0.000000f),
	vec3f(-0.850651f,	0.000000f,	0.525731f),		vec3f(-0.955423f,	-0.295242f,	0.000000f),		vec3f(-0.951056f,	-0.162460f,	0.262866f),
	vec3f(-0.864188f,	0.442863f,	-0.238856f),	vec3f(-0.951056f,	0.162460f,	-0.262866f),	vec3f(-0.809017f,	0.309017f,	-0.500000f),
	vec3f(-0.864188f,	-0.442863f,	-0.238856f),	vec3f(-0.951056f,	-0.162460f,	-0.262866f),	vec3f(-0.809017f,	-0.309017f,	-0.500000f),
	vec3f(-0.681718f,	0.147621f,	-0.716567f),	vec3f(-0.681718f,	-0.147621f,	-0.716567f),	vec3f(-0.850651f,	0.000000f,	-0.525731f),
	vec3f(-0.688191f,	0.587785f,	-0.425325f),	vec3f(-0.587785f,	0.425325f,	-0.688191f),	vec3f(-0.425325f,	0.688191f,	-0.587785f),
	vec3f(-0.425325f,	-0.688191f,	-0.587785f),	vec3f(-0.587785f,	-0.425325f,	-0.688191f),	vec3f(-0.688191f,	-0.587785f,	-0.425325f)
};

/*
=================
DirToByte

This isn't a real cheap function to call!
=================
*/
uint8 DirToByte(const vec3f &dirVec)
{
	if (!dirVec)
		return 0;

	uint8 best = 0;
	float bestDot = 0;
	for (uint8 i = 0; i < NUMVERTEXNORMALS; i++)
	{
		float dot = vec3f(m_byteDirs[i]) | dirVec;
		if (dot > bestDot)
		{
			bestDot = dot;
			best = i;
		}
	}

	return best;
}

/*
=================
ByteToDir
=================
*/
void ByteToDir(const uint8 dirByte, vec3f &dirVec)
{
	if (dirByte >= NUMVERTEXNORMALS)
	{
		dirVec.Clear();
		return;
	}

	dirVec = m_byteDirs[dirByte];
}

// ===========================================================================

/*
==============
FloatToByte
==============
*/
uint8 FloatToByte(float x)
{
	union
	{
		float			f;
		uint32			i;
	} f2i;

	// Shift float to have 8bit fraction at base of number
	f2i.f = x + 32768.0f;
	f2i.i &= 0x7FFFFF;

	// Then read as integer and kill float bits...
	return (uint8)Min<sint32>(f2i.i, 255);
}


/*
===============
ColorNormalizef
===============
*/
float ColorNormalizef(const float *in, float *out)
{
	float f = Max<>(Max<>(in[0], in[1]), in[2]);

	if (f > 1.0f)
	{
		f = 1.0f / f;
		out[0] = in[0] * f;
		out[1] = in[1] * f;
		out[2] = in[2] * f;
	}
	else
	{
		out[0] = in[0];
		out[1] = in[1];
		out[2] = in[2];
	}

	return f;
}


/*
===============
ColorNormalizeb
===============
*/
float ColorNormalizeb(const float *in, uint8 *out)
{
	float f = Max<>(Max<>(in[0], in[1]), in[2]);

	if (f > 1.0f)
	{
		f = 1.0f / f;
		out[0] = FloatToByte(in[0] * f);
		out[1] = FloatToByte(in[1] * f);
		out[2] = FloatToByte(in[2] * f);
	}
	else
	{
		out[0] = FloatToByte(in[0]);
		out[1] = FloatToByte(in[1]);
		out[2] = FloatToByte(in[2]);
	}

	return f;
}

// ===========================================================================

/*
===============
Q_ftol
===============
*/
#ifdef id386
__declspec_naked long Q_ftol(float f)
{
	static sint32	tmp;
	__asm {
		fld dword ptr [esp+4]
		fistp tmp
		mov eax, tmp
		ret
	}
}
#endif // id386


/*
===============
Q_FastSqrt

5% margin of error
===============
*/
#ifdef id386
float Q_FastSqrt(float value)
{
	float result = 0;
	__asm {
		mov eax, value
		sub eax, 0x3f800000
		sar eax, 1
		add eax, 0x3f800000
		mov result, eax
	}
	return result;
}
#endif // id386


/*
===============
Q_RSqrtf

1/sqrt, faster but not as precise
===============
*/
float Q_RSqrtf(float number)
{
	float	y;

	if (number == 0.0f)
		return 0.0f;
	*((sint32 *)&y) = 0x5f3759df - ((* (sint32 *) &number) >> 1);
	return y * (1.5f - (number * 0.5f * y * y));
}


/*
===============
Q_RSqrtd

1/sqrt, faster but not as precise
===============
*/
double Q_RSqrtd(double number)
{
	double	y;

	if (number == 0.0)
		return 0.0;
	*((sint32 *)&y) = 0x5f3759df - ((* (sint32 *) &number) >> 1);
	return y * (1.5f - (number * 0.5 * y * y));
}


/*
===============
Q_log2
===============
*/
sint32 Q_log2(sint32 val)
{
	sint32 answer = 0;
	while(val >>= 1)
		answer++;
	return answer;
}

// ===========================================================================

/*
===============
MakeNormalVectorsf
===============
*/
void MakeNormalVectorsf(const vec3f &forward, vec3f &right, vec3f &up)
{
	// This rotate and negate guarantees a vector not colinear with the original
	right.Set (forward[2], -forward[0], forward[1]);

	right = right.MultiplyAngles (-(right | forward), forward).GetNormalized();
	up = right ^ forward;
}


/*
===============
PerpendicularVector

Assumes "src" is normalized
===============
*/
void PerpendicularVector(const vec3f &src, vec3f &dst)
{
	uint8	pos = 5;
	float	minElem = 1.0f;
	vec3f	tempVec;

	// Find the smallest magnitude axially aligned vector
	if (Q_fabs(src.X) < minElem)
	{
		pos = 0;
		minElem = Q_fabs(src.X);
	}
	if (Q_fabs(src.Y) < minElem)
	{
		pos = 1;
		minElem = Q_fabs(src.Y);
	}
	if (Q_fabs(src.Z) < minElem)
	{
		pos = 2;
		minElem = Q_fabs(src.Z);
	}

	_CC_ASSERT_EXPR(pos != 5, "Couldn't find smallest magnitude");

	if (pos < 3)
		tempVec[pos] = 1.0F;

	// Project the point onto the plane defined by src
	ProjectPointOnPlane(dst, tempVec, src);

	// Normalize the result
	dst.Normalize();
}


/*
===============
RotatePointAroundVector
===============
*/
void RotatePointAroundVector(vec3f &dest, const vec3f &dir, const vec3f &point, const float degrees)
{
	float c, s;
	Q_SinCosf(DEG2RAD(degrees), &s, &c);

	vec3f vr, vu;
	MakeNormalVectorsf(dir, vr, vu);

	float t0, t1;
	t0 = vr.X * c + vu.X * -s;
	t1 = vr.X * s + vu.X *  c;
	dest.X = (t0 * vr.X + t1 * vu.X + dir.X * dir.X) * point.X
			+ (t0 * vr.Y + t1 * vu.Y + dir.X * dir.Y) * point.Y
			+ (t0 * vr.Z + t1 * vu.Z + dir.X * dir.Z) * point.Z;

	t0 = vr.Y * c + vu.Y * -s;
	t1 = vr.Y * s + vu.Y *  c;
	dest.Y = (t0 * vr.X + t1 * vu.X + dir.Y * dir.X) * point.X
			+ (t0 * vr.Y + t1 * vu.Y + dir.Y * dir.Y) * point.Y
			+ (t0 * vr.Z + t1 * vu.Z + dir.Y * dir.Z) * point.Z;

	t0 = vr.Z * c + vu.Z * -s;
	t1 = vr.Z * s + vu.Z *  c;
	dest.Z = (t0 * vr.X + t1 * vu.X + dir.Z * dir.X) * point.X
			+ (t0 * vr.Y + t1 * vu.Y + dir.Z * dir.Y) * point.Y
			+ (t0 * vr.Z + t1 * vu.Z + dir.Z * dir.Z) * point.Z;
}
