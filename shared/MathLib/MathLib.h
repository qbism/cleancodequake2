/*
Copyright (C) 1997-2001 Id Software, Inc.

This program is free software; you can redistribute it and/or
modify it under the terms of the GNU General Public License
as published by the Free Software Foundation; either version 2
of the License, or (at your option) any later version.

This program is distributed in the hope that it will be useful,
but WITHOUT ANY WARRANTY; without even the implied warranty of
MERCHANTABILITY or v

See the GNU General Public License for more details.

You should have received a copy of the GNU General Public License
along with this program; if not, write to the Free Software
Foundation, Inc., 59 Temple Place - Suite 330, Boston, MA 02111-1307, USA.
*/

// MathLib.h

// ===========================================================================

class  vec3f; // forward declaration

// ===========================================================================

#ifndef M_PI
# define M_PI			3.14159265358979323846f		// matches value in gcc v2 math.h
#endif

// angle indexes
enum
{
	PITCH,		// up / down
	YAW,		// left / right
	ROLL		// fall over
};

inline float DEG2RAD (const float v)
{
	return v * (M_PI / 180.0f);
}

inline float RAD2DEG (const float v)
{
	return v * (180.0f / M_PI);
}


inline sint16 ANGLE2SHORT (float x)
{
	return ((sint32)(x*65536/360) & 65535);
}

inline float SHORT2ANGLE (sint16 x)
{
	return (x*(360.0f/65536));
}


inline uint8 ANGLE2BYTE (float x)
{
	return ((sint32)(x*256/360) & 255);
}

inline float BYTE2ANGLE (uint8 x)
{
	return (x*(360.0f/256));
}

// ===========================================================================

#define LARGE_EPSILON 0.1
#define SMALL_EPSILON 0.01
#define TINY_EPSILON 0.001

#define TINY_NUMBER (1.e-8)
#define SMALL_NUMBER (1.e-4)

#ifndef ArrayCount
# define ArrayCount(a) (sizeof(a)/sizeof((a)[0]))
#endif

// ===========================================================================

uint8		FloatToByte(float x);

float		ColorNormalizef(const float *in, float *out);
float		ColorNormalizeb(const float *in, uint8 *out);

// ===========================================================================

#ifdef id386
long	Q_ftol (float f);
float	Q_FastSqrt (float value);
#else // id386
inline long Q_ftol (float f) { return ((long)f); }
inline float Q_FastSqrt (float value) { return sqrt(value); }
#endif // id386
inline float Q_fabs (float val) { return ((val < 0.f) ? -val : val); }

// ===========================================================================

inline sint32 Q_rint (float x)
{
	return ((x) < 0 ? ((sint32)((x)-0.5f)) : ((sint32)((x)+0.5f)));
}

inline void Q_SinCosf(const float X, float *Sin, float *Cos)
{
#if defined(HAVE_SINCOSF)
	sincosf(X, Sin, Cos);
#elif defined(id386)
	__asm fld X
	__asm fsincos
	__asm mov ecx, Cos
	__asm mov edx, Sin
	__asm fstp dword ptr [ecx]
	__asm fstp dword ptr [edx]
#else
	*Sin = sinf(X);
	*Cos = cosf(X);
#endif
}

float	Q_RSqrtf (float number);
double	Q_RSqrtd (double number);
sint32		Q_log2 (sint32 val);

template<typename TType>
inline TType Q_NearestPow(const TType &Value, const bool bRoundDown)
{
	TType Result;
	for (Result = 1; Result < Value; Result <<= 1) ;

	if (bRoundDown && Result > Value)
		Result >>= 1;

	return Result;
}

// ===========================================================================

void		MakeNormalVectorsf (const vec3f &forward, vec3f &right, vec3f &up);
void		PerpendicularVector (const vec3f &src, vec3f &dst);
void		RotatePointAroundVector(vec3f &dest, const vec3f &dir, const vec3f &point, const float degrees);

//
// m_angles.c
//
float		AngleModf (float a);
float		LerpAngle (float a1, float a2, float frac);

//
// m_bounds.c
//

void		AddPointToBounds (vec3f v, vec3f &mins, vec3f &maxs);
bool		BoundsAndSphereIntersect (const vec3f &mins, const vec3f &maxs, const vec3f &centre, float radius);
bool		BoundsIntersect (const vec3f &mins1, const vec3f &maxs1, const vec3f &mins2, const vec3f &maxs2);
void		ClearBounds (vec3f &mins, vec3f &maxs);
void		MinMins (const vec3f &a, const vec3f &b, vec3f &out);
void		MaxMaxs (const vec3f &a, const vec3f &b, vec3f &out);
float		RadiusFromBounds (const vec3f &mins, const vec3f &maxs);

// ===========================================================================

#include "Vector.h"

extern vec2f vec2fOrigin;
extern vec3f vec3fOrigin;

// ===========================================================================

#define NUMVERTEXNORMALS	162
extern vec3f	m_byteDirs[NUMVERTEXNORMALS];

uint8		DirToByte(const vec3f &dirVec);
void		ByteToDir(const uint8 dirByte, vec3f &dirVec);

