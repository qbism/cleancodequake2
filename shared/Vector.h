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

// Vector.h

/*
==============================================================================

	vec2Base
 
==============================================================================
*/

template<typename TType>
class  vec2Base
{
public:
	/**
	 * Data
	 */
	TType X, Y;

	/**
	 * Constructors
	 */
	vec2Base()
	: X(0), Y(0) {}

	inline vec2Base(const vec2Base &Vec)
	: X(Vec[0]), Y(Vec[1]) {}

	inline vec2Base(const TType Value)
	: X(Value), Y(Value) {}

	inline vec2Base(const TType Values[2])
	: X(Values[0]), Y(Values[1]) {}

	inline vec2Base(const TType InX, const TType InY)
	: X(InX), Y(InY) {}

	/**
	 * Destructors
	 */
	~vec2Base() {}

	/**
	 * Operators
	 */
	inline bool operator !=(const vec2Base &Vec) { return (X != Vec[0] || Y != Vec[1]); }

	inline bool operator ==(const vec2Base &Vec) { return (X == Vec[0] && Y == Vec[1]); }

	inline const TType &operator [](const sint32 Index) const { return (&X)[Index]; }
	inline TType &operator [](const sint32 Index) { return (&X)[Index]; }

	inline operator TType *() { return (&X); }
	inline operator const TType *() const { return (&X); }

	/**
	 * Functions
	 */
	inline bool Compare(const vec2Base &Vec) const { return (X == Vec[0] && Y == Vec[1]); }

	inline void Copy(const vec2Base &Vec) { X = Vec[0]; Y = Vec[1]; }

	inline void Invert() { X = -X; Y = -Y; }
	inline vec2Base GetInverted() const { return vec2Base(-X, -Y); }

	inline bool IsZero() const { return (X == 0 && Y == 0); }

	inline void Set(const TType Number) { X = Number; Y = Number; }
	inline void Set(const TType InX, const TType InY) { X = InX; Y = InY; }
	inline void Set(const vec2Base &Vec) { X = Vec[0]; Y = Vec[1]; }
};

/*
==============================================================================

	vec2f
 
==============================================================================
*/

class  vec2f : public vec2Base<float>
{
public:
	/**
	 * Constructors
	 */
	vec2f() {}
	inline vec2f(const vec2f &Vec) : vec2Base<float>(Vec) {}
	inline vec2f(const float Value) : vec2Base<float>(Value) {}
	inline vec2f(const float Values[2]) : vec2Base<float>(Values) {}
	inline vec2f(const float InX, const float InY) : vec2Base<float>(InX, InY) {}

	/**
	 * Operators
	 */
	inline vec2f operator *(const vec2f &Vec) const { return vec2f(X*Vec[0], Y*Vec[1]); }
	inline vec2f operator *(const float Scale) const { return vec2f(X*Scale, Y*Scale); }

	inline vec2f &operator *=(const vec2f &Vec)
	{
		X *= Vec[0];
		Y *= Vec[1];

		return *this;
	}
	inline vec2f &operator *=(const float Scale)
	{
		X *= Scale;
		Y *= Scale;

		return *this;
	}

	inline vec2f operator +(const vec2f &Vec) const { return vec2f(X+Vec[0], Y+Vec[1]); }
	inline vec2f &operator +=(const vec2f &Vec)
	{
		X += Vec[0];
		Y += Vec[1];

		return *this;
	}

	inline vec2f operator -(const vec2f &Vec) const { return vec2f(X-Vec[0], Y-Vec[1]); }
	inline vec2f &operator -=(const vec2f &Vec)
	{
		X -= Vec[0];
		Y -= Vec[1];

		return *this;
	}

	inline vec2f operator /(const vec2f &Vec) const { return vec2f(X/Vec[0], Y/Vec[1]); }
	inline vec2f operator /(const float Number) const
	{
		const float InvNumber = 1.0f / Number;
		return vec2f(X*InvNumber, Y*InvNumber);
	}

	inline vec2f &operator /=(const vec2f &Vec)
	{
		X /= Vec[0];
		Y /= Vec[1];

		return *this;
	}
	inline vec2f &operator /=(const float Number)
	{
		const float InvNumber = 1.0f / Number;

		X *= InvNumber;
		Y *= InvNumber;

		return *this;
	}

	inline bool operator <(const vec2f &Vec) { return (X < Vec[0] && Y < Vec[1]); }

	inline vec2f &operator =(const vec2f &Vec)
	{
		X = Vec[0];
		Y = Vec[1];

		return *this;
	}

	inline bool operator >(const vec2f &Vec) { return (X > Vec[0] && Y > Vec[1]); }

	/**
	 * Functions
	 */
	inline void Clear() { *(sint32*)&X = 0; *(sint32*)&Y = 0; }

	bool Compare(const vec2f &Vec, const float Epsilon) const
	{
		if (Q_fabs(X-Vec[0]) > Epsilon)
			return false;
		if (Q_fabs(Y-Vec[1]) > Epsilon)
			return false;
		return true;
	}

	bool IsNearlyZero(const float Epsilon = SMALL_NUMBER) { return (Q_fabs(X) <= Epsilon && Q_fabs(Y) <= Epsilon); }

	inline void Scale(const float Scale) { X *= Scale; Y *= Scale; }
	inline void Scale(const vec2f &Vec) { X *= Vec[0]; Y *= Vec[1]; }
};

/*
==============================================================================

	vec3Base
 
==============================================================================
*/

template<typename TType>
class  vec3Base
{
public:
	/**
	 * Data
	 */
	TType X, Y, Z;

	/**
	 * Constructors
	 */
	vec3Base()
	: X(0), Y(0), Z(0) {}

	inline vec3Base(const vec3Base &Vec)
	: X(Vec[0]), Y(Vec[1]), Z(Vec[2]){}

	inline vec3Base(const TType Value)
	: X(Value), Y(Value), Z(Value) {}

	inline vec3Base(const TType Values[3])
	: X(Values[0]), Y(Values[1]), Z(Values[2]) {}

	inline vec3Base(const TType InX, const TType InY, const TType InZ)
	: X(InX), Y(InY), Z(InZ) {}

	/**
	 * Destructors
	 */
	~vec3Base() {}

	// Convert to another vec3Base type
	template <typename TNewType>
	vec3Base<TNewType> Convert ()
	{
		return vec3Base<TNewType> (X, Y, Z);
	}

	// Convert to another type based off vec3Base
	template <typename TNewType>
	TNewType ConvertDerived ()
	{
		return TNewType (X, Y, Z);
	}

	/**
	 * Operators
	 */
	inline bool operator !=(const vec3Base &Vec) { return (X != Vec[0] || Y != Vec[1] || Z != Vec[2]); }

	inline bool operator ==(const vec3Base &Vec) { return (X == Vec[0] && Y == Vec[1] && Z == Vec[2]); }

	inline const TType &operator [](const sint32 Index) const { return (&X)[Index]; }
	inline TType &operator [](const sint32 Index) { return (&X)[Index]; }

	inline operator TType *() { return (&X); }
	inline operator const TType *() const { return (&X); }

	/**
	 * Functions
	 */
	inline bool Compare(const vec3Base &Vec) const { return (X == Vec[0] && Y == Vec[1] && Z == Vec[2]); }

	inline void Copy(const vec3Base &Vec) { X = Vec[0]; Y = Vec[1]; Z = Vec[2]; }

	inline void Invert() { X = -X; Y = -Y; Z = -Z; }
	inline vec3Base GetInverted() const { return vec3Base(-X, -Y, -Z); }

	inline bool IsZero() const { return (X == 0 && Y == 0 && Z == 0); }

	inline void Set(const TType Number) { X = Number; Y = Number; Z = Number; }
	inline void Set(const TType InX, const TType InY, const TType InZ) { X = InX; Y = InY; Z = InZ; }
	inline void Set(const vec3Base &Vec) { X = Vec[0]; Y = Vec[1]; Z = Vec[2]; }
};

/*
==============================================================================

	vec3f
 
==============================================================================
*/

class  vec3f : public vec3Base<float>
{
public:
	/**
	 * Constructors
	 */
	vec3f() {}
	inline vec3f(const vec3f &Vec) : vec3Base<float>(Vec) {}
	inline vec3f(const float Value) : vec3Base<float>(Value) {}
	inline vec3f(const float Values[3]) : vec3Base<float>(Values) {}
	inline vec3f(const float InX, const float InY, const float InZ) : vec3Base<float>(InX, InY, InZ) {}

	/**
	 * Operators
	 */
	inline vec3f operator *(const vec3f &Vec) const { return vec3f(X*Vec[0], Y*Vec[1], Z*Vec[2]); }
	inline vec3f operator *(const float Scale) const { return vec3f(X*Scale, Y*Scale, Z*Scale); }

	inline vec3f &operator *=(const vec3f &Vec)
	{
		X *= Vec[0];
		Y *= Vec[1];
		Z *= Vec[2];

		return *this;
	}
	inline vec3f &operator *=(const float Scale)
	{
		X *= Scale;
		Y *= Scale;
		Z *= Scale;

		return *this;
	}

	inline vec3f operator +(const vec3f &Vec) const { return vec3f(X+Vec[0], Y+Vec[1], Z+Vec[2]); }
	inline vec3f &operator +=(const vec3f &Vec)
	{
		X += Vec[0];
		Y += Vec[1];
		Z += Vec[2];

		return *this;
	}

	inline vec3f operator -(const vec3f &Vec) const { return vec3f(X-Vec[0], Y-Vec[1], Z-Vec[2]); }
	inline vec3f &operator -=(const vec3f &Vec)
	{
		X -= Vec[0];
		Y -= Vec[1];
		Z -= Vec[2];

		return *this;
	}

	inline vec3f operator /(const vec3f &Vec) const { return vec3f(X/Vec[0], Y/Vec[1], Z/Vec[2]); }
	inline vec3f operator /(const float Number) const
	{
		const float InvNumber = 1.0f / Number;
		return vec3f(X*InvNumber, Y*InvNumber, Z*InvNumber);
	}

	inline vec3f &operator /=(const vec3f &Vec)
	{
		X /= Vec[0];
		Y /= Vec[1];
		Z /= Vec[2];

		return *this;
	}
	inline vec3f &operator /=(const float Number)
	{
		const float InvNumber = 1.0f / Number;

		X *= InvNumber;
		Y *= InvNumber;
		Z *= InvNumber;

		return *this;
	}

	inline bool operator <(const vec3f &Vec) const { return (X < Vec[0] && Y < Vec[1] && Z < Vec[2]); }
	inline bool operator <(const float Number) const { return (X < Number && Y < Number && Z < Number); }

	inline vec3f &operator =(const vec3f &Vec)
	{
		X = Vec[0];
		Y = Vec[1];
		Z = Vec[2];

		return *this;
	}

	inline bool operator >(const vec3f &Vec) const { return (X > Vec[0] && Y > Vec[1] && Z > Vec[2]); }
	inline bool operator >(const float Number) const { return (X > Number && Y > Number && Z > Number); }

	inline vec3f operator ^(const vec3f &Vec) const { return Cross(Vec); }
	inline float operator |(const vec3f &Vec) const { return Dot(Vec); }

	// Paril, unary operators
	inline vec3f operator - () const { return vec3f(-X, -Y, -Z); }

	/**
	 * Functions
	 */
	inline void Clear() { *(sint32 *)&X = 0; *(sint32 *)&Y = 0; *(sint32 *)&Z = 0; }
	
	inline void Abs() { X = Q_fabs(X); Y = Q_fabs(Y); Z = Q_fabs(Z); }
	inline vec3f GetAbs() const { return vec3f(Q_fabs(X), Q_fabs(Y), Q_fabs(Z)); }

	bool Compare(const vec3f &Vec, const float Epsilon) const
	{
		if (Q_fabs(X-Vec[0]) > Epsilon)
			return false;
		if (Q_fabs(Y-Vec[1]) > Epsilon)
			return false;
		if (Q_fabs(Z-Vec[2]) > Epsilon)
			return false;
		return true;
	}

	bool Inside (const vec3f Min, const vec3f Max) const
	{
		return ((*this > Min) && (*this < Max));
	}

	inline vec3f Cross(const vec3f &Vec) const { return vec3f(Y*Vec[2] - Z*Vec[1], Z*Vec[0] - X*Vec[2], X*Vec[1] - Y*Vec[0]); }

	inline float Dist(const vec3f &Vec) const { return sqrtf((X-Vec[0])*(X-Vec[0])+(Y-Vec[1])*(Y-Vec[1])+(Z-Vec[2])*(Z-Vec[2])); }
	inline float DistFast(const vec3f &Vec) const { return Q_FastSqrt((X-Vec[0])*(X-Vec[0])+(Y-Vec[1])*(Y-Vec[1])+(Z-Vec[2])*(Z-Vec[2])); }
	inline float DistSq(const vec3f &Vec) const { return (X-Vec[0])*(X-Vec[0])+(Y-Vec[1])*(Y-Vec[1])+(Z-Vec[2])*(Z-Vec[2]); }

	inline float Dot(const vec3f &Vec) const { return X*Vec.X + Y*Vec.Y + Z*Vec.Z; }

	bool IsNearlyZero(const float Epsilon = SMALL_NUMBER) const { return (Q_fabs(X) <= Epsilon && Q_fabs(Y) <= Epsilon && Q_fabs(Z) <= Epsilon); }

	inline float Length() const { return sqrtf(X*X + Y*Y + Z*Z); }
	inline float LengthFast() const { return Q_FastSqrt(X*X + Y*Y + Z*Z); }
	inline float LengthSq() const { return X*X + Y*Y + Z*Z; }

	float Normalize()
	{
		float Len = Length();

		if (Len)
			Scale(1.0f / Len);
		else
			Clear();

		return Len;
	}
	float NormalizeFast()
	{
		float Len = Q_RSqrtf(Dot(*this));
		Scale (Len);
		return (Len) ? (1.0f / Len) : 0;
	}

	inline vec3f const GetNormalized ()
	{
		vec3f val (*this);
		val.Normalize ();
		return val;
	}

	inline vec3f const GetNormalizedFast ()
	{
		vec3f val (*this);
		val.NormalizeFast ();
		return val;
	}

	inline vec3f const GetNormalized (float &length)
	{
		vec3f val (*this);
		length = val.Normalize ();
		return val;
	}

	inline vec3f const GetNormalizedFast (float &length)
	{
		vec3f val (*this);
		length = val.NormalizeFast ();
		return val;
	}

	float ToYaw () const
	{
		float	yaw;

		if (X == 0)
		{
			yaw = 0;
			if (Y > 0)
				yaw = 90;
			else if (Y < 0)
				yaw = -90;
		}
		else
		{
			yaw = atan2f(Y, X) * 180 / M_PI;
			if (yaw < 0)
				yaw += 360;
		}

		return yaw;
	}

	inline void Scale(const float Scale) { X *= Scale; Y *= Scale; Z *= Scale; }
	inline void Scale(const vec3f &Vec) { X *= Vec[0]; Y *= Vec[1]; Z *= Vec[2]; }

	void ToVectors (vec3f *forward, vec3f *right, vec3f *up) const
	{
		float sr, sp, sy, cr, cp, cy;
		Q_SinCosf(DEG2RAD (X), &sp, &cp);
		Q_SinCosf(DEG2RAD (Y), &sy, &cy);
		Q_SinCosf(DEG2RAD (Z), &sr, &cr);

		if (forward)
			forward->Set (cp* cy, cp*sy, -sp);
		if (right)
			right->Set (-1 * sr * sp * cy + -1 * cr * -sy,
						-1 * sr * sp * sy + -1 * cr * cy,
						-1 * sr * cp);
		if (up)
			up->Set (cr * sp * cy + -sr * -sy,
						cr * sp * sy + -sr * cy,
						cr * cp);
	};

	inline vec3f MultiplyAngles (const float scale, const vec3f &b) const
	{
		return vec3f(	X + b.X * scale,
						Y + b.Y * scale,
						Z + b.Z * scale);
	}

	vec3f ToAngles () const
	{
		float	yaw, pitch;
		
		if (Y == 0 && X == 0)
		{
			yaw = 0;
			if (Z > 0)
				pitch = 90;
			else
				pitch = 270;
		}
		else
		{
			if (X)
				yaw = atan2f(Y, X) * (180.0f / M_PI);
			else if (Y > 0)
				yaw = 90;
			else
				yaw = 270;

			if (yaw < 0)
				yaw += 360;

			pitch = atan2f(Z, sqrtf(X * X + Y * Y)) * (180.0f / M_PI);
			if (pitch < 0)
				pitch += 360;
		}

		return vec3f (-pitch, yaw, 0);
	}

	// "this" is forward
	void MakeNormalVectors (vec3f &right, vec3f &up) const
	{
		// This rotate and negate guarantees a vector not colinear with the original
		right = vec3f(Z, -X, Y).MultiplyAngles (-(right | *this), *this).GetNormalized();
		up = right ^ *this;
	};

	void ProjectOnPlane (const vec3f &point, const vec3f &normal);

	/*
	===============
	PerpendicularVector
	Assumes "src" is normalized
	===============
	*/
	void PerpendicularVector(vec3f &dst) const
	{
		uint8	pos = 5;
		float	minElem = 1.0f;
		vec3f	tempVec;

		// Find the smallest magnitude axially aligned vector
		for (uint8 i = 0; i < 3; ++i)
		{
			if (Q_fabs(*this[i]) < minElem)
			{
				pos = i;
				minElem = Q_fabs(*this[i]);
			}
		}

		CC_ASSERT_EXPR(pos != 5, "Couldn't find smallest magnitude");

		if (pos < 3)
			tempVec[pos] = 1.0f;

		// Project the point onto the plane defined by src
		dst.ProjectOnPlane(tempVec, *this);

		// Normalize the result
		dst.Normalize();
	};

	/*
	===============
	RotatePointAroundVector
	===============
	*/
	void RotateAroundVector(vec3f &dest, const vec3f &dir, const float degrees) const
	{
		float c, s;
		Q_SinCosf(DEG2RAD(degrees), &s, &c);

		vec3f vr, vu;
		dir.MakeNormalVectors (vr, vu);

		float t0, t1;
		t0 = vr.X * c + vu.X * -s;
		t1 = vr.X * s + vu.X *  c;
		dest.X = (t0 * vr.X + t1 * vu.X + dir.X * dir.X) * X
				+ (t0 * vr.Y + t1 * vu.Y + dir.X * dir.Y) * Y
				+ (t0 * vr.Z + t1 * vu.Z + dir.X * dir.Z) * Z;

		t0 = vr.Y * c + vu.Y * -s;
		t1 = vr.Y * s + vu.Y *  c;
		dest.Y = (t0 * vr.X + t1 * vu.X + dir.Y * dir.X) * X
				+ (t0 * vr.Y + t1 * vu.Y + dir.Y * dir.Y) * Y
				+ (t0 * vr.Z + t1 * vu.Z + dir.Y * dir.Z) * Z;

		t0 = vr.Z * c + vu.Z * -s;
		t1 = vr.Z * s + vu.Z *  c;
		dest.Z = (t0 * vr.X + t1 * vu.X + dir.Z * dir.X) * X
				+ (t0 * vr.Y + t1 * vu.Y + dir.Z * dir.Y) * Y
				+ (t0 * vr.Z + t1 * vu.Z + dir.Z * dir.Z) * Z;
	}

};

// Global vector operations
inline vec3f operator * (const float &l, const vec3f &r)
{
	return (r * l);
};
