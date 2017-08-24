#ifndef ANGEOUTILITY_VECTOR3_H_
#define ANGEOUTILITY_VECTOR3_H_

#ifdef WIN32
#pragma warning(disable:4244) // Gdouble to Gfloat
#endif

#include "utility_math.h"

template<typename N>
class TVector3
{
public:
	typedef N ValueType;

public:
	TVector3() :x(N(0)), y(N(0)), z(N(0)){}

	TVector3(N a, N b, N c) : x(a), y(b), z(c){}
	TVector3(const TVector3& v) : x(v.x), y(v.y), z(v.z){}

	template<typename V>
	TVector3(const V& v):x(N(v.x)),y(N(v.y)),z(N(v.z)){}

public:
	template<typename V>
	TVector3& operator = (const V& v)
	{
		x = N(v.x);
		y = N(v.y);
		z = N(v.z);
		return *this;
	}

	TVector3& operator += (const TVector3& v)
	{
		x += v.x;
		y += v.y;
		z += v.z;
		return *this;
	}

	TVector3& operator -= (const TVector3& v)
	{
		x -= v.x;
		y -= v.y;
		z -= v.z;
		return *this;
	}

	TVector3& operator *= (N s)
	{
		x *= s;
		y *= s;
		z *= s;
		return *this;
	}

	TVector3& operator /= (N s)
	{
		s = N(1) / s;
		x *= s;
		y *= s;
		z *= s;
		return *this;
	}

	TVector3 operator + () const
	{
		return *this;
	}
		
	TVector3 operator - () const
	{
		return TVector3(-x, -y, -z);
	}

	TVector3 operator + (const TVector3& v) const
	{
		return TVector3(x + v.x, y + v.y, z + v.z);
	}

	TVector3 operator - (const TVector3& v) const
	{
		return TVector3(x - v.x, y - v.y, z - v.z);
	}

	TVector3 operator * (N s) const
	{
		return TVector3<N>(x * s, y * s, z * s);
	}

	TVector3 operator / (const TVector3& v) const
	{
		AGO_ASSERT(v.x != N(0));
		AGO_ASSERT(v.y != N(0));
		AGO_ASSERT(v.z != N(0));
		return TVector3(x / v.x, y / v.y, z / v.z);
	}

	TVector3 operator / (N s) const
	{
		s = N(1) / s;
		return TVector3<N>(x * s, y * s, z * s);
	}

	Gbool operator == (const TVector3& v) const
	{
		return x == v.x && y == v.y && z == v.z;
	}

	Gbool operator != (const TVector3& v) const
	{
		return x != v.x || y != v.y || z != v.z;
	}

	Gbool operator > (const TVector3& v) const
	{
		return (x > v.x && y > v.y && z > v.z);
	}

	Gbool operator < (const TVector3& v) const
	{
		return (x < v.x && y < v.y && z < v.z);
	}

	TVector3 operator ^ (const TVector3& vec) const 
	{ 
		return Cross(vec); 
	}

	operator N* () 
	{ 
		return &x; 
	}

	operator const N* () const 
	{ 
		return &x; 
	}

	friend TVector3 operator * ( N scalar, const TVector3& vec )
	{
		return TVector3(scalar * vec.x, scalar * vec.y, scalar * vec.z);
	}
	
public:

	void SetZero()
	{
		x = y = z = 0.0;
	}

	Gbool IsZero() const
	{
		return Equal(TVector3<N>(), 0.0001);
	}

	void Scale( N scale_value )
	{
		x *= scale_value;
		y *= scale_value;
		z *= scale_value;
	}

	void Normalize(void)
	{
		N len = LengthSquare();
		if (len > TMath<N>::s_tolerance)
		{
			len = TMath<N>::recip_sqrt_(len);
			x *= len;
			y *= len;
			z *= len;
		}
	}

	N NormalizeLength()
	{
		N len = Length();
		if (len > TMath<N>::s_tolerance)
		{
			N r = N(1.0) / N(len);
			x *= r;
			y *= r;
			z *= r;
		}
		return len;
	}

	N Length(void) const
	{
		return TMath<N>::sqrt_(x * x + y * y + z * z);
	}

	N LengthSquare(void) const
	{
		return x * x + y * y + z * z;
	}

	N Dot( const TVector3& vec ) const
	{
		return vec.x * x + vec.y * y + vec.z * z;
	}

	N operator * (const TVector3& vec) const
	{
		return Dot(vec);
	}

	TVector3 Cross( const TVector3& vec ) const
	{
		return TVector3(	y * vec.z - z * vec.y,
						z * vec.x - x * vec.z,
						x * vec.y - y * vec.x);
	}

	Gbool Equal(const TVector3& v, N A_Tolerance) const
	{
	    if (x < v.x - A_Tolerance)
	        return false;
	    if (x > v.x + A_Tolerance)
	        return false;
	
	    if (y < v.y - A_Tolerance)
	        return false;
	    if (y > v.y + A_Tolerance)
	        return false;
	
	    if (z < v.z - A_Tolerance)
	        return false;
	    if (z > v.z + A_Tolerance)
	        return false;
	
	    return true;
	}
	
	N DistanceTo(const TVector3& v) const
	{
		return (*this - v).Length();
	}

public:
	static TVector3 Normalize( const TVector3& vec )
	{
		N len = vec.LengthSquare();
		if (len > TMath<N>::s_tolerance)
		{
			len = TMath<N>::recip_sqrt_(len);
			return TVector3(vec.x * len, vec.y * len, vec.z * len);
		}
		return vec;
	}

	static N Length( const TVector3& vec )
	{
		return TMath<N>::sqrt_(vec.x * vec.x + vec.y * vec.y + vec.z * vec.z);
	}

	static N LengthSquare(const TVector3& v)
	{
		return v.x * v.x + v.y * v.y + v.z * v.z;
	}

	template<typename N1,typename N2>
	static N Dot( const TVector3<N1>& vec1, const TVector3<N2>& vec2 )
	{
		return vec1.x * vec2.x + vec1.y * vec2.y + vec1.z * vec2.z;
	}

	template<typename N1, typename N2>
	static TVector3 Cross( const TVector3<N1>& vec1, const TVector3<N2>& vec2 )
	{
		return TVector3(	vec1.y * vec2.z - vec1.z * vec2.y,
						vec1.z * vec2.x - vec1.x * vec2.z,
						vec1.x * vec2.y - vec1.y * vec2.x);
	}

public:
	N x, y, z;

public:
	// special vectors
	static const TVector3 s_zero;
	static const TVector3 s_unit_x;
	static const TVector3 s_unit_y;
	static const TVector3 s_unit_z;

	inline static TVector3 UnitX() { return TVector3<N>::s_unit_x; }
	inline static TVector3 UnitY() { return TVector3<N>::s_unit_y; }
	inline static TVector3 UnitZ() { return TVector3<N>::s_unit_z; }
	inline static TVector3 Zero() { return TVector3<N>::s_zero; }
};

//------------------------------------------------------------------------------

template<typename V, typename V1>
inline void Vector3Assign(V& vOut, const V1& v1)
{
	vOut.x = v1.x;
	vOut.y = v1.y;
	vOut.z = v1.z;
}

template<typename N>
inline N Vector3Length(const TVector3<N>& v)
{
	return TMath<N>::sqrt_(v.x * v.x + v.y * v.y + v.z * v.z);
}

template<typename N>
inline N Vector3LengthSquare(const TVector3<N>& v)
{
	return v.x * v.x + v.y * v.y + v.z * v.z;
}

template<typename N, typename N1>
inline void Vector3Normalize(TVector3<N>& vOut, const TVector3<N1>& v)
{
	vOut.x = v.x;
	vOut.y = v.y;
	vOut.z = v.z;

	N len = Vector3LengthSquare(v);
	if (len > TMath<N>::s_tolerance)
	{
		len = TMath<N>::recip_sqrt_(len);
		vOut.x *= len;
		vOut.y *= len;
		vOut.z *= len;
	}
}

template<typename N>
inline TVector3<N> Vector3Normalize(const TVector3<N>& v)
{
	N len = Vector3LengthSquare(v);
	if (len > TMath<N>::s_tolerance)
	{
		len = TMath<N>::recip_sqrt_(len);
		return TVector3<N>(v.x * len, v.y * len, v.z * len);
	}
	return v;
}

template<typename V, typename V1, typename V2>
inline void Vector3Add(V& vOut, const V1& v1, const V2& v2)
{
	vOut.x = v1.x + v2.x;
	vOut.y = v1.y + v2.y;
	vOut.z = v1.z + v2.z;
}

template<typename V1, typename V2>
inline V1 Vector3Add(const V1& v1, const V2& v2)
{
	return V1(v1.x + v2.x, v1.y + v2.y, v1.z + v2.z);
}

template<typename V, typename V1, typename V2>
inline void Vector3Subtract(V& vOut, const V1& v1, const V2& v2)
{
	vOut.x = v1.x - v2.x;
	vOut.y = v1.y - v2.y;
	vOut.z = v1.z - v2.z;
}

template<typename V1, typename V2>
inline V1 Vector3Subtract(const V1& v1, const V2& v2)
{
	return V1(v1.x - v2.x, v1.y - v2.y, v1.z - v2.z);
}

template<typename V, typename V1, typename N1>
inline void Vector3Scale(V& vOut, const V1& v, N1 s)
{
	vOut.x = v.x * s;
	vOut.y = v.y * s;
	vOut.z = v.z * s;
}

template<typename V1, typename N1>
inline V1 Vector3Scale(const V1& v, N1 s)
{
	return V1(v.x * s, v.y * s, v.z * s);
}

template<typename N, typename V1, typename V2>
inline void Vector3Dot(N& dot, const V1& v1, const V2& v2)
{
	dot = v1.x * v2.x + v1.y * v2.y + v1.z * v2.z;
}

template<typename N, typename N1>
inline N Vector3Dot(const TVector3<N>& v1, const TVector3<N1>& v2)
{
	return (v1.x * v2.x + v1.y * v2.y + v1.z * v2.z);
}

template<typename N, typename N1, typename N2>
inline void Vector3Cross(TVector3<N>& vOut, const TVector3<N1>& v1, const TVector3<N2>& v2)
{
	N x = v1.y * v2.z - v1.z * v2.y;
	N y = v1.z * v2.x - v1.x * v2.z;
	vOut.z = v1.x * v2.y - v1.y * v2.x;
	vOut.x = x;
	vOut.y = y;
}

template<typename N, typename N1>
inline TVector3<N> Vector3Cross(const TVector3<N>& v1, const TVector3<N1>& v2)
{
	return TVector3<N>(
			v1.y * v2.z - v1.z * v2.y, 
			v1.z * v2.x - v1.x * v2.z, 
			v1.x * v2.y - v1.y * v2.x);
}

template<typename N>
inline TVector3<N> Vector3Reflect(const TVector3<N>& src, const TVector3<N>& normal)
{
	return TVector3<N>(src - (2 * TVector3<N>::Dot(src, normal) * normal));
}

template<typename V, typename V1, typename V2>
inline void Vector3Minimize(V& vOut, const V1& v1, const V2& v2)
{
	vOut.x = v1.x < v2.x ? v1.x : v2.x;
	vOut.y = v1.y < v2.y ? v1.y : v2.y;
	vOut.z = v1.z < v2.z ? v1.z : v2.z;
}

template<typename V1, typename V2>
inline V1 Vector3Minimize(const V1& v1, const V2& v2)
{
	return V1(	v1.x < v2.x ? v1.x : v2.x,
				v1.y < v2.y ? v1.y : v2.y,
				v1.z < v2.z ? v1.z : v2.z);
}

template<typename V, typename V1, typename V2>
inline void Vector3Maximize(V& vOut, const V1& v1, const V2& v2)
{
	vOut.x = v1.x > v2.x ? v1.x : v2.x;
	vOut.y = v1.y > v2.y ? v1.y : v2.y;
	vOut.z = v1.z > v2.z ? v1.z : v2.z;
}

template<typename V1, typename V2>
inline V1 Vector3Maximize(const V1& v1, const V2& v2)
{
	return V1(	v1.x > v2.x ? v1.x : v2.x,
				v1.y > v2.y ? v1.y : v2.y,
				v1.z > v2.z ? v1.z : v2.z);
}

template<typename V, typename V1, typename V2, typename N1>
inline void Vector3Lerp(V& vOut, const V1& v1, const V2& v2, N1 s)
{
	vOut.x = v1.x + (v2.x - v1.x) * s;
	vOut.y = v1.y + (v2.y - v1.y) * s;
	vOut.z = v1.z + (v2.z - v1.z) * s;
}

template<typename V1, typename V2, typename N1>
inline V1 Vector3Lerp(const V1& v1, const V2& v2, N1 s)
{
	return V1(	v1.x + (v2.x - v1.x) * s,
				v1.y + (v2.y - v1.y) * s,
				v1.z + (v2.z - v1.z) * s);
}

template <typename N>
inline N Vector3Angle(const TVector3<N>& lhs, const TVector3<N>& rhs)
{
	return TMath<N>::acos_(Vector3Dot(lhs, rhs) / (Vector3Length(lhs) * Vector3Length(rhs)));
}

//------------------------------------------------------------------------------

typedef TVector3<Gfloat>		TVector3f;
typedef TVector3<Gdouble>		TVector3d;

#endif//ANGEOUTILITY_VECTOR3_H_
